
#include "App/Model/IBM/IBM.h"

#include "Misc/JsonValidator/ConfigurationValidator.h"
#include "Misc/JsonValidator/EmbeddedConfigurationSchemas.h"

#include "App/Manager/LogManager.h"



using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;


IBM::IBM()
	: Model()
{

}

IBM::~IBM()
{

}

pair<bool,fs::path> IBM::existsCheckpoint(const fs::path& checkpointFolderPath, const bool isBinaryCheckpoint) {
    if (!fs::exists(checkpointFolderPath) || !fs::is_directory(checkpointFolderPath)) {
        return make_pair<>(false, fs::path());
    }

    for (const auto& entry : fs::directory_iterator(checkpointFolderPath)) {
        if (fs::is_regular_file(entry)) {
			if(isBinaryCheckpoint) {
				if (entry.path().extension() == ".bin") {
					return make_pair<>(true, entry.path());
				}
			}
			else {
				if (entry.path().extension() == ".txt") {
					return make_pair<>(true, entry.path());
				}
			}
        }
    }

    return make_pair<>(false, fs::path());
}

void IBM::run(const RunMode runMode, const fs::path& inputConfigPath, const fs::path& outputFolderPath)
{
	Landscape* myLandscape = nullptr;

	if(runMode == RunMode::FromConfig)
	{
		ConfigurationValidator validator(WeaverEmbeddedSchemas::configurationStructure,
			WeaverEmbeddedSchemas::configurationConstraints, WeaverEmbeddedSchemas::jsonSchemas());
		auto validation = validator.validate(inputConfigPath, WEAVER_SCHEMA_VERSION);
		if (!validation.valid()) throwLineInfoException("Configuration validation failed:\n" + validation.toPrettyString());
		if (validation.hasWarnings()) {
			std::string warningMessage = "Configuration validation produced warnings:\n";
			for (const auto& warning : validation.warnings) {
				warningMessage += " - ";
				if (!warning.file.empty()) warningMessage += warning.file;
				if (!warning.instancePath.empty()) warningMessage += warning.instancePath;
				if (!warning.file.empty() || !warning.instancePath.empty()) warningMessage += ": ";
				warningMessage += warning.message;
				if (warningMessage.back() != '\n') warningMessage += '\n';
			}
			LogManager::emitWarning(std::move(warningMessage));
		}

		LogManager::emit("===================================================\n");
		LogManager::emit("Reading configuration and initializing landscape...\n");
		LogManager::emit("===================================================\n");


		json landscapeConfig = readConfigFile(inputConfigPath / "landscape_params.json");
		
		myLandscape = Landscape::createInstance(landscapeConfig["landscape"]["simulationType"]);
	}
	else
	{
		LogManager::emit("============================================\n");
		LogManager::emit("Reading checkpoint and resuming landscape...\n");
		LogManager::emit("============================================\n\n");


		bool isBinaryCheckpoint = (runMode == RunMode::FromBinaryCheckpoint);

		auto result = existsCheckpoint(inputConfigPath / "checkpoint", isBinaryCheckpoint);

		if(!result.first)
		{
			throwLineInfoException("The checkpoint file does not exist or is not a valid file");
		}

		std::ifstream ifs(result.second.string());

		if(isBinaryCheckpoint)
		{
			boost::archive::binary_iarchive ia(ifs);
			ia & myLandscape;
		}
		else
		{
			boost::archive::text_iarchive ia(ifs);
			ia & myLandscape;
		}

		ifs.close();
	}


	myLandscape->init(inputConfigPath, outputFolderPath, (runMode != RunMode::FromConfig));


	LogManager::emit("DONE\n\n");

	LogManager::emit("======================\n");
	LogManager::emit("Running simulation ...\n");
	LogManager::emit("======================\n\n");

	myLandscape->evolveLandscape();

	LogManager::emit("DONE\n");

	LogManager::emit("Result folder: " + myLandscape->getResultFolder().string() + "\n", true);

	delete myLandscape;
}
