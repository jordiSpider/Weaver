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
#include <ostream>
#include <fstream>
#include "Maths/Random.h"
#include "Maths/Parabola.h"
#include "FungusSpecies.h"
#include "SimulationConstants.h"
#include "Utilities.h"

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

	//timestamp_t t0 = get_timestamp();

	// Read configuration file
	json configuration = readConfigFile(fs::path("run_params.json"));

	fs::path outputDirectory = obtainOutputDirectory(configuration);
	fs::create_directories(outputDirectory);

	ofstream logFile;
	if(configuration["simulation"]["outputStream"]["enabled"])
	{
		setOutputStream(logFile, outputDirectory, configuration["simulation"]["outputStream"]["selectedChannel"]);
	}
	else
	{
		cout.setstate(std::ios_base::failbit);
	}

	if(configuration["simulation"]["initFromFixedSeed"]["enabled"])
	{
		unsigned int fixedSeedValue = configuration["simulation"]["initFromFixedSeed"]["fixedSeedValue"];
		Random::initRandomGenerator(fixedSeedValue);
	}
	else
	{
		Random::initRandomGenerator((unsigned)time(NULL));
	}	

	cout << "===============================================" << endl;
	cout << "Reading configuration and initializing world..." << endl;
	cout << "===============================================" << endl;
	cout << endl;

	World * myWorld = new World(&configuration, outputDirectory);

	myWorld->initializeAnimals();


	cout << "DONE" << endl;

	cout << endl;
	cout << "======================" << endl;
	cout << "Running simulation ..." << endl;
	cout << "======================" << endl;
	cout << endl;

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
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(0.9, 0.9, 0.9);

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
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
	cout << "DONE" << std::endl;

	delete myWorld;

	exit(0);
}
