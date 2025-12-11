
#include "App/Model/IBM/IBM.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;


IBM::IBM(View& view)
	: Model(view)
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
    #ifdef USE_CPU_PROFILER
		ProfilerStart("./profiler/cpuProfiler.txt");
	#endif
	
	#ifdef USE_HEAP_PROFILER
		HeapProfilerStart("./profiler/heapProfiler");
	#endif


	Landscape* myLandscape = nullptr;

	if(runMode == RunMode::FromConfig)
	{
		view.updateLog("===================================================\n");
		view.updateLog("Reading configuration and initializing landscape...\n");
		view.updateLog("===================================================\n");


		JsonValidator landscapeValidator(LANDSCAPE_PARAMS_SCHEMA);

		json landscapeConfig = readConfigFile(inputConfigPath / "landscape_params.json", landscapeValidator);
		
		myLandscape = Landscape::createInstance(landscapeConfig["landscape"]["simulationType"]);
	}
	else
	{
		view.updateLog("============================================\n");
		view.updateLog("Reading checkpoint and resuming landscape...\n");
		view.updateLog("============================================\n\n");


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


	myLandscape->init(&view, inputConfigPath, outputFolderPath, (runMode != RunMode::FromConfig));

    #ifdef USE_HEAP_PROFILER
		HeapProfilerDump("prueba");
	#endif


	view.updateLog("DONE\n\n");

	view.updateLog("======================\n");
	view.updateLog("Running simulation ...\n");
	view.updateLog("======================\n\n");

	myLandscape->evolveLandscape();

	view.updateLog("DONE\n");

	#ifdef USE_CPU_PROFILER
		ProfilerStop();
	#endif

	#ifdef USE_HEAP_PROFILER
		HeapProfilerStop();
	#endif

	view.updateLog("Result folder: " + myLandscape->getResultFolder().string() + "\n", true);

	delete myLandscape;
}
