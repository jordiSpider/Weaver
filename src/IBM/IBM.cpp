
#include "IBM/IBM.h"


using namespace std;
using json = nlohmann::json;
namespace fs = boost::filesystem;


void IBM::run(fs::path inputConfigPath, fs::path outputFolder, bool silentMode, int burnIn)
{
    #ifdef USE_CPU_PROFILER
		ProfilerStart("./profiler/cpuProfiler.txt");
	#endif
	
	#ifdef USE_HEAP_PROFILER
		HeapProfilerStart("./profiler/heapProfiler");
	#endif

    auto start = std::chrono::high_resolution_clock::now();

    #ifdef UNIX
	ofstream nullFile("/dev/null"); 
	#endif
	#ifdef WINDOWS
	ofstream nullFile("NUL");
	#endif

	streambuf* backing = nullptr;
	if(silentMode)
	{
		backing = cout.rdbuf(nullFile.rdbuf());
	}

    // Convert input folder relative path to absolute path
	try
	{
		inputConfigPath = fs::canonical(inputConfigPath);
	}
	catch(const boost::filesystem::filesystem_error& e)
	{
		throwLineInfoException("The input config directory does not exist or is not a valid directory");
	}

    // Convert output folder relative path to absolute path
	try
	{
		outputFolder = fs::canonical(outputFolder);
	}
	catch(const boost::filesystem::filesystem_error& e)
	{
		throwLineInfoException("The output directory does not exist or is not a valid directory");
	}


	fs::path schemaFolder = fs::canonical(fs::path(SCHEMA_FOLDER));


	// Read configuration file
	json simulationConfiguration = readConfigFile(inputConfigPath / "simulation_params.json", schemaFolder / fs::path(SIMULATION_PARAMS_SCHEMA));

	fs::path resultFolder = obtainResultFolder(inputConfigPath.filename().string(), outputFolder);
	fs::create_directories(resultFolder);
    
    if(simulationConfiguration["simulation"]["initFromFixedSeed"]["enabled"])
	{
		unsigned int fixedSeedValue = simulationConfiguration["simulation"]["initFromFixedSeed"]["fixedSeedValue"];
		Random::initRandomGenerator(fixedSeedValue);
	}
	else
	{
		Random::initRandomGenerator((unsigned)time(NULL));
	}

    cout << "===============================================" << endl;
	cout << "Reading configuration and initializing world..." << endl;
	cout << "===============================================" << endl << endl;

	json worldConfig = readConfigFile(inputConfigPath / fs::path("world_params.json"), schemaFolder / fs::path(WORLD_PARAMS_SCHEMA));

	unique_ptr<WorldInterface> myWorld = WorldInterface::createInstance(&simulationConfiguration, worldConfig, resultFolder, inputConfigPath, burnIn);

	myWorld->initializeAnimals();

    #ifdef USE_HEAP_PROFILER
		HeapProfilerDump("prueba");
	#endif


	cout << "DONE" << endl << endl;

	cout << "======================" << endl;
	cout << "Running simulation ..." << endl;
	cout << "======================" << endl << endl;

	myWorld->evolveWorld();

	cout << "DONE" << endl;

	if(burnIn != -1)
	{
		myWorld->saveOptimizationResult(resultFolder);
	}

	#ifdef USE_CPU_PROFILER
		ProfilerStop();
	#endif

	#ifdef USE_HEAP_PROFILER
		HeapProfilerStop();
	#endif

	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed = end - start;

	ofstream timeFile(resultFolder / fs::path("executionTime.txt"));
	timeFile << elapsed.count() << " segs" << endl;

	if(silentMode)
	{
		cout.rdbuf(backing);
	}
}

void IBM::run(fs::path checkpoint, bool isBinaryCheckpoint, bool silentMode)
{
	#ifdef USE_CPU_PROFILER
		ProfilerStart("./profiler/cpuProfiler.txt");
	#endif
	
	#ifdef USE_HEAP_PROFILER
		HeapProfilerStart("./profiler/heapProfiler");
	#endif

    auto start = std::chrono::high_resolution_clock::now();

    #ifdef UNIX
	ofstream nullFile("/dev/null"); 
	#endif
	#ifdef WINDOWS
	ofstream nullFile("NUL");
	#endif

	streambuf* backing = nullptr;
	if(silentMode)
	{
		backing = cout.rdbuf(nullFile.rdbuf());
	}

	// Convert checkpoint relative path to absolute path
	try
	{
		checkpoint = fs::canonical(checkpoint);
	}
	catch(const boost::filesystem::filesystem_error& e)
	{
		throwLineInfoException("The checkpoint file does not exist or is not a valid file");
	}

	cout << "========================================" << endl;
	cout << "Reading checkpoint and resuming world..." << endl;
	cout << "========================================" << endl << endl;

	WorldInterface* myWorld;
	unsigned int projectVersion = projectVersionStringToNumber(PROJECT_VERSION);

	if(isBinaryCheckpoint)
	{
		InputFileStream ifs(checkpoint);
		boost::archive::binary_iarchive ia(ifs);
		boost::serialization::serialize(ia, myWorld, projectVersion);
		ifs.close();
	}
	else
	{
		InputFileStream ifs(checkpoint);
		boost::archive::text_iarchive ia(ifs);
		boost::serialization::serialize(ia, myWorld, projectVersion);
		ifs.close();
	}

	#ifdef USE_HEAP_PROFILER
		HeapProfilerDump("prueba");
	#endif


	cout << "DONE" << endl << endl;

	cout << "======================" << endl;
	cout << "Running simulation ..." << endl;
	cout << "======================" << endl << endl;

	myWorld->evolveWorld(true);

	cout << "DONE" << endl;

	#ifdef USE_CPU_PROFILER
		ProfilerStop();
	#endif

	#ifdef USE_HEAP_PROFILER
		HeapProfilerStop();
	#endif

	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed = end - start;

	ofstream timeFile(myWorld->getOutputFolder() / fs::path("executionTime.txt"));
	timeFile << elapsed.count() << " segs" << endl;

	if(silentMode)
	{
		cout.rdbuf(backing);
	}

	delete myWorld;
}
