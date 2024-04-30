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
        ("I,inputFolder", "Folder where to look for the input file", cxxopts::value<fs::path>(inputFolder)->default_value(fs::current_path().string()));

    fs::path inputFile;
    options.add_options()
        ("i,inputFile", "Input file defining the simulation", cxxopts::value<fs::path>(inputFile)->default_value("run_params.json"));

    fs::path outputFolder;
    options.add_options()
        ("o,outputFolder", "Output directory where the simulation results folder will be created", cxxopts::value<fs::path>(outputFolder)->default_value(RESULT_SIMULATION_FOLDER));

	auto result = options.parse(argc, argv);


	fs::path WeaverFolder = std::filesystem::path(argv[0]).parent_path();
	


	//timestamp_t t0 = get_timestamp();

	// Read configuration file
	json configuration = readConfigFile1(inputFolder / inputFile, WeaverFolder);

	fs::path resultFolder = obtainResultFolder(configuration, outputFolder);
	fs::create_directories(resultFolder);

	
	if(configuration["simulation"]["outputStream"]["enabled"])
	{
		Output::setOutputStream(resultFolder, configuration["simulation"]["outputStream"]["selectedChannel"]);
	}
	else
	{
		FILE* nullFile = fopen("/dev/null", "w");
		Output::setOutputStream(nullFile);
	}

	Output::setErrorOutputStream(stderr);

	if(configuration["simulation"]["initFromFixedSeed"]["enabled"])
	{
		unsigned int fixedSeedValue = configuration["simulation"]["initFromFixedSeed"]["fixedSeedValue"];
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
		configFolder = configuration["simulation"].at("configFolder");
	}
	catch(const json::out_of_range& e)
	{
		configFolder = CONFIG_FOLDER;
	}
	string configName = configuration["simulation"]["configName"];
	fs::path configPath = fs::path(configFolder) / fs::path(configName);

	json worldConfig = readConfigFile1(configPath / fs::path("world_params.json"), WeaverFolder);

	World * myWorld;

	switch (WorldType::stringToEnumValue((string)worldConfig["world"]["worldType"])) {
		case WorldType::Old: {
			myWorld = new OldWorld(&configuration, worldConfig, inputFolder / inputFile, resultFolder, WeaverFolder, configPath);
			break;
		}
		case WorldType::Matrix3D: {
			myWorld = new Matrix3DWorld(&configuration, worldConfig, inputFolder / inputFile, resultFolder, WeaverFolder, configPath);
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

	//timestamp_t t1 = get_timestamp();
	//double secs = (t1 - t0) / 1000000.0L;
	//cout << "Time: " << secs << " secs." << endl;

	/*myWorld->eraseTerrain();

	vtkImageReader *imgReader = vtkImageReader::New();
	imgReader->SetFileDimensionality(3);
	imgReader->SetDataScalarType(VTK_FLOAT);
	imgReader->SetFileName("water.bin");
	imgReader->SetNumberOfScalarComponents(1);
	imgReader->SetDataExtent(0, myWorld->getLength() - 1, 0, myWorld->getWidth() - 1, 0, myWorld->getDepth() - 1);

	//Create a mapper
	vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> mapper_conc =
			vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
	mapper_conc->SetInput(imgReader->GetOutput());

	//create volume
	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();

	//let it be semitransparent for all data
	vtkPiecewiseFunction *oTFun = vtkPiecewiseFunction::New();
	oTFun->AddPoint(0., 0.);
	oTFun->AddPoint(100., 0.5);

	//color transfer function -- blue for empty and red for high concentration
	vtkColorTransferFunction *cTFun = vtkColorTransferFunction::New();
	cTFun->AddRGBPoint(0.0, 0.0, 0.0, 0.01);
	cTFun->AddRGBPoint(100.0, 0.0, 0.0, 1.0);

	//set volume property
	vtkVolumeProperty *property = vtkVolumeProperty::New();
	property->SetScalarOpacity(oTFun);
	property->SetColor(cTFun);
	property->SetInterpolationTypeToLinear();
	property->SetScalarOpacityUnitDistance(2.0);

	volume->SetMapper(mapper_conc);
	volume->SetProperty(property);

	// a renderer and render window
	vtkSmartPointer<vtkRenderer> coutow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetSize(900, 600);
	renderWindow->AddRenderer(renderer);

	// add the volume to renderer
	renderer->AddVolume(volume);

	// an interactor
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renderWindow);

	renderWindow->Render();
	// begin mouse interaction
	iren->Start();
*/
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
