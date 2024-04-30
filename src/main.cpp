/*
 * main.cpp
 *
 *  Created on: Apr 3, 2012
 *      Author: jrbcast
 */
//#define _CRTDBG_MAP_ALLOC
#include <stdio.h>
#include <stdlib.h>
#include "World.h"
#include <iostream>
#include <sys/time.h>
#include <ctime>
#include <chrono>
#include <ostream>
#include <fstream>
#include "Maths/Random.h"
#include "Maths/Parabola.h"
#include "SimulationConstants.h"
#include "GlobalVariable.h"
#include "Utilities.h"
#include <cxxopts.hpp>
#include "WorldInterface.h"
#include "LineInfoException.h"
#include "Output.h"

#ifdef USE_CPU_PROFILER
	#include "gperftools/profiler.h"
#endif

#ifdef USE_HEAP_PROFILER
	#include "gperftools/heap-profiler.h"
#endif

// Library nlohmann JSON
#include <nlohmann/json.hpp>


//#include <crtdbg.h>
/*
#include "vtkImageReader.h"
#include "vtkImageViewer.h"
#include "vtkImageMagnify.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPiecewiseFunction.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
*/

/*
#include "tbb/task_scheduler_init.h"

using namespace tbb;
*/

using namespace std;

using json = nlohmann::json;



int main(int argc, char ** argv)
{

//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

/*
#ifdef _TBB
	task_scheduler_init init(TBB_NUM_THREADS);
#endif
*/
	auto start = std::chrono::high_resolution_clock::now();

	#ifdef USE_CPU_PROFILER
		ProfilerStart("./profiler/cpuProfiler.txt");
	#endif
	
	#ifdef USE_HEAP_PROFILER
		HeapProfilerStart("./profiler/heapProfiler");
	#endif

	cxxopts::Options options("Weaver", "Weaver description");

	fs::path inputFolder;
    options.add_options()
        ("I,inputFolder", "Location of the input file folder", cxxopts::value<fs::path>(inputFolder)->default_value(fs::current_path().string()));

    fs::path inputFile;
    options.add_options()
        ("i,inputFile", "Simulation input file definition", cxxopts::value<fs::path>(inputFile)->default_value("run_params.json"));

    fs::path outputFolder;
    options.add_options()
        ("o,outputFolder", "Directory where the simulation results will be stored", cxxopts::value<fs::path>(outputFolder)->default_value(RESULT_SIMULATION_FOLDER));

	auto result = options.parse(argc, argv);

	// Convert input folder relative path to absolute path
	try
	{
		inputFolder = fs::canonical(inputFolder);
	}
	catch(const std::filesystem::filesystem_error& e)
	{
		throwLineInfoException("The input directory does not exist or is not a valid directory");
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


	if(!fs::exists(inputFolder / inputFile))
	{
        throwLineInfoException("The input file does not exist or is not a valid file");
    }
	


	//timestamp_t t0 = get_timestamp();

	// Read configuration file
	json mainConfiguration = readConfigFile(inputFolder / inputFile, WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(RUN_PARAMS_SCHEMA));

	fs::path resultFolder = obtainResultFolder(mainConfiguration, outputFolder);
	fs::create_directories(resultFolder);

	
	if(mainConfiguration["simulation"]["outputStream"]["enabled"])
	{
		Output::setOutputStream(resultFolder, mainConfiguration["simulation"]["outputStream"]["selectedChannel"]);
	}
	else
	{
		FILE* nullFile = fopen("/dev/null", "w");
		Output::setOutputStream(nullFile);
	}

	Output::setErrorOutputStream(stderr);

	if(mainConfiguration["simulation"]["initFromFixedSeed"]["enabled"])
	{
		unsigned int fixedSeedValue = mainConfiguration["simulation"]["initFromFixedSeed"]["fixedSeedValue"];
		Random::initRandomGenerator(fixedSeedValue);
	}
	else
	{
		Random::initRandomGenerator((unsigned)time(NULL));
	}	

	Output::cout("===============================================\n");
	Output::cout("Reading configuration and initializing world...\n");
	Output::cout("===============================================\n\n");

	string configFolder;
	try
	{
		configFolder = mainConfiguration["simulation"].at("configFolder");
	}
	catch(const json::out_of_range& e)
	{
		configFolder = CONFIG_FOLDER;
	}
	string configName = mainConfiguration["simulation"]["configName"];
	fs::path configPath = inputFolder / fs::path(configFolder) / fs::path(configName);

	json worldConfig = readConfigFile(configPath / fs::path("world_params.json"), WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(WORLD_PARAMS_SCHEMA));

	World * myWorld;

	switch (WorldType::stringToEnumValue((string)worldConfig["world"]["worldType"])) {
		case WorldType::Old: {
			myWorld = new OldWorld(&mainConfiguration, worldConfig, inputFolder / inputFile, resultFolder, WeaverFolder, configPath);
			break;
		}
		case WorldType::Matrix3D: {
			myWorld = new Matrix3DWorld(&mainConfiguration, worldConfig, inputFolder / inputFile, resultFolder, WeaverFolder, configPath);
			break;
		}
		default: {
			throwValidatorSchemaJSONException("': Default case");
			break;
		}
	}

	myWorld->initializeAnimals();


	#ifdef USE_HEAP_PROFILER
		HeapProfilerDump("prueba");
	#endif


	Output::cout("DONE\n\n");

	Output::cout("======================\n");
	Output::cout("Running simulation ...\n");
	Output::cout("======================\n\n");

	myWorld->evolveWorld();

	Output::cout("DONE\n");

	delete myWorld;

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

	exit(0);
}
