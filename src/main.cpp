/*
 * main.cpp
 *
 *  Created on: Apr 3, 2012
 *      Author: jrbcast
 */
//#define _CRTDBG_MAP_ALLOC


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <ctime>
#include <chrono>
#include <ostream>
#include <fstream>
#include "Maths/Random.h"
#include "Maths/Parabola.h"
#include "Misc/Utilities.h"
#include <cxxopts.hpp>
#include "World/WorldFactory.h"
#include "Exceptions/LineInfoException.h"

#ifdef USE_CPU_PROFILER
	#include "gperftools/profiler.h"
#endif

#ifdef USE_HEAP_PROFILER
	#include "gperftools/heap-profiler.h"
#endif

// Library nlohmann JSON
#include <nlohmann/json.hpp>



using namespace std;

using json = nlohmann::json;



int main(int argc, char ** argv)
{
	auto start = std::chrono::high_resolution_clock::now();

	#ifdef USE_CPU_PROFILER
		ProfilerStart("./profiler/cpuProfiler.txt");
	#endif
	
	#ifdef USE_HEAP_PROFILER
		HeapProfilerStart("./profiler/heapProfiler");
	#endif

	fs::path WeaverRootPath = fs::path(__FILE__).parent_path().parent_path();

	cxxopts::Options options("Weaver", "Weaver description");

	fs::path inputConfigPath;
    options.add_options()
        ("I,inputConfig", "Input config path", cxxopts::value<fs::path>(inputConfigPath));

	fs::path outputFolder;
    options.add_options()
        ("O,outputFolder", "Result output folder path", cxxopts::value<fs::path>(outputFolder)->default_value((WeaverRootPath / RESULT_SIMULATION_FOLDER).string()));

	bool silentMode;
	options.add_options()
        ("silent", "Run the program in silent mode, suppressing standard output", cxxopts::value<bool>(silentMode)->default_value("false"));

	options.add_options()
		("h,help", "Print usage");

	auto result = options.parse(argc, argv);

	if(result.count("help"))
	{
		cout << options.help() << endl;
		return 0;
	}

	if(!result.count("inputConfig"))
	{
		cerr << "Error: Required arguments are missing." << endl;
		cerr << "Try 'Weaver --help' for more information." << endl;
        return 1;
	}


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
	catch(const std::filesystem::filesystem_error& e)
	{
		throwLineInfoException("The input config directory does not exist or is not a valid directory");
	}

	// Convert output folder relative path to absolute path
	try
	{
		outputFolder = fs::canonical(outputFolder);
	}
	catch(const std::filesystem::filesystem_error& e)
	{
		throwLineInfoException("The output directory does not exist or is not a valid directory");
	}

	// Directory that contains the executable
	fs::path WeaverFolder = std::filesystem::path(argv[0]).parent_path();


	//timestamp_t t0 = get_timestamp();

	// Read configuration file
	json simulationConfiguration = readConfigFile(inputConfigPath / "simulation_params.json", WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(SIMULATION_PARAMS_SCHEMA));

	fs::path resultFolder = obtainResultFolder(inputConfigPath.filename(), outputFolder);
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

	json worldConfig = readConfigFile(inputConfigPath / fs::path("world_params.json"), WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(WORLD_PARAMS_SCHEMA));

	unique_ptr<WorldInterface> myWorld = WorldFactory::createInstance(&simulationConfiguration, worldConfig, resultFolder, WeaverFolder, inputConfigPath);

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
	
	return 0;
}
