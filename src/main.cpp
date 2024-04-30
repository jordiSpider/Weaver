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
#include "CLI11/CLI11.h"
#include "Output.h"

#ifdef USE_CPU_PROFILER
	#include "gperftools/profiler.h"
#endif

#ifdef USE_HEAP_PROFILER
	#include "gperftools/heap-profiler.h"
#endif

// Library nlohmann JSON
#include "nlohmann/json.h"


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

	CLI::App app{"Weaver description"};

	fs::path inputFolder;
    auto inputFolderOption = app.add_option("-I,--inputFolder", inputFolder, "Folder where to look for the input file")->required(false);
	inputFolderOption->default_val(fs::current_path());

    fs::path inputFile;
    auto inputFileOption = app.add_option("-i,--inputFile", inputFile, "Input file defining the simulation")->required(false);
	inputFileOption->default_val("run_params.json");

	fs::path outputFolder;
    auto outputFolderOption = app.add_option("-o,--outputFolder", outputFolder, "Output directory where the simulation results folder will be created")->required(false);
	outputFolderOption->default_val(RESULT_SIMULATION_FOLDER);

	try {
		app.parse(argc, argv);
	} catch (const CLI::ParseError &e) {
		return app.exit(e);
	}


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
		Output::setOutputStream(Output::nullFile);
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

	World * myWorld = new World(&configuration, inputFolder, inputFolder / inputFile, resultFolder, WeaverFolder);

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
