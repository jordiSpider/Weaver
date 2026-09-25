
#include "App/Model/IBM/IBM.h"

#include "schema/landscape_params_schema_json.h"

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
		LogManager::emit("===================================================\n");
		LogManager::emit("Reading configuration and initializing landscape...\n");
		LogManager::emit("===================================================\n");


		JsonValidator landscapeValidator(EmbeddedResources::landscape_params_schema_json, "landscape_params_schema");

		json landscapeConfig = readConfigFile(inputConfigPath / "landscape_params.json", landscapeValidator);
		
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
