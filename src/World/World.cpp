
/*
#include "tbb/tbb.h"
*/
#include <thread>
#include <algorithm> // transform

#include "Misc/SimulationConstants.h"
#include "World/World.h"
#include "Misc/Utilities.h"
#include "Misc/GlobalVariable.h"
#include "World/LivingBeings/LifeStage.h"


using namespace std;
using json = nlohmann::json;




World::World(json * jsonTree, json &worldConfig, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath, const double &massRatio)
	: currentMovePercentage(printBarEach), currentNumberOfTerrainCellsMoved(0), WeaverFolder(WeaverFolder), massRatio(massRatio)
{
	inputFolder = configPath;

	exitAtFirstExtinction = (*jsonTree)["simulation"]["exitAtFirstExtinction"];
	runDays = (*jsonTree)["simulation"]["runDays"];
	recordEach = (*jsonTree)["simulation"]["recordEach"];
	numberOfCombinations = (*jsonTree)["simulation"]["numberOfCombinations"];
	timeStepsPerDay = (*jsonTree)["simulation"]["timeStepsPerDay"];
	growthAndReproTest = (*jsonTree)["simulation"]["growthAndReproTest"];
	//TODO tunetraits and metab every X STEPS
	//TODO growth every X STEPS

	// Set output directory
	setOutputFolder(outputFolder);

	// Read space configuration
	


	setResourceToPreysCapacityTransference(worldConfig["world"]["mapConfig"]["resourceToPreysCapacityTransference"]);
	setPreysToPredatorsCapacityTransference(worldConfig["world"]["mapConfig"]["preysToPredatorsCapacityTransference"]);

	setEncounterHuntedSizeAH(worldConfig["world"]["life"]["encounterHuntedSizeAH"]);
	setEncounterHuntedSizeSAW(worldConfig["world"]["life"]["encounterHuntedSizeSAW"]);
	setEncounterHuntedVoracityAH(worldConfig["world"]["life"]["encounterHuntedVoracityAH"]);
	setEncounterHuntedVoracitySAW(worldConfig["world"]["life"]["encounterHuntedVoracitySAW"]);
	setEncounterHunterSizeAH(worldConfig["world"]["life"]["encounterHunterSizeAH"]);
	setEncounterHunterSizeSAW(worldConfig["world"]["life"]["encounterHunterSizeSAW"]);
	setEncounterHunterVoracityAH(worldConfig["world"]["life"]["encounterHunterVoracityAH"]);
	setEncounterHunterVoracitySAW(worldConfig["world"]["life"]["encounterHunterVoracitySAW"]);
	setEncounterProbabilityDensityFunctionAH(worldConfig["world"]["life"]["encounterProbabilityDensityFunctionAH"]);
	setEncounterProbabilityDensityFunctionSAW(worldConfig["world"]["life"]["encounterProbabilityDensityFunctionSAW"]);
	setEncounterVoracitiesProductAH(worldConfig["world"]["life"]["encounterVoracitiesProductAH"]);
	setEncounterVoracitiesProductSAW(worldConfig["world"]["life"]["encounterVoracitiesProductSAW"]);
	setPredationHunterVoracityAH(worldConfig["world"]["life"]["predationHunterVoracityAH"]);
	setPredationHunterVoracitySAW(worldConfig["world"]["life"]["predationHunterVoracitySAW"]);
	setPredationProbabilityDensityFunctionAH(worldConfig["world"]["life"]["predationProbabilityDensityFunctionAH"]);
	setPredationProbabilityDensityFunctionSAW(worldConfig["world"]["life"]["predationProbabilityDensityFunctionSAW"]);
	setPredationSpeedRatioAh(worldConfig["world"]["life"]["predationSpeedRatioAH"]);
	setPredationSpeedRatioSaw(worldConfig["world"]["life"]["predationSpeedRatioSAW"]);

	maxSearchArea = 0.0;

	setObstacleFolderName(fs::path(worldConfig["world"]["mapConfig"]["obstacleFolder"]));
	setMoistureFolderName(fs::path(worldConfig["world"]["mapConfig"]["moistureFolder"]));
	setResourceFolderName(fs::path(worldConfig["world"]["life"]["resourceFolder"]));
	setSpeciesFolderName(fs::path(worldConfig["world"]["life"]["speciesFolder"]));
	
	initIndividualsPerDensities = worldConfig["world"]["life"]["initIndividualsPerDensities"]["enabled"];

	competitionAmongResourceSpecies = worldConfig["world"]["life"]["competitionAmongResourceSpecies"];

	setExitTimeThreshold(worldConfig["world"]["life"]["exitTimeThreshold"]);
	setInitialEcosystemSize(worldConfig["world"]["life"]["initIndividualsPerDensities"]["initialEcosystemSize"]);
	setMaxLogMassRatio(worldConfig["world"]["life"]["maxLogMassRatio"]);
	setMinLogMassRatio(worldConfig["world"]["life"]["minLogMassRatio"]);
	setSigmaForPDF(worldConfig["world"]["life"]["sigmaForPDF"]);
	setMuForPDF(worldConfig["world"]["life"]["muForPDF"]);

	// TODO Mario Initialize animals with initIndividualsPerDensities
	/*
	setHeatingCodeTemperatureCycle(worldConfig["world"]["mapConfig"]["temperatureFilename"]); // By default, 20 Celsius degrees
	*/

	readResourceSpeciesFromJSONFiles();
	readAnimalSpeciesFromJSONFiles();

	initializeMap(worldConfig["world"]["mapConfig"]);

	readPatchesFromJSONFiles(worldConfig["world"]["mapConfig"]["moistureBasePatch"], true);

	worldMap->obtainInhabitableTerrainCells();
	//initializeAnimals();

	calculateAnimalSpeciesK_Density();

	encountersMatrixFilename = (*jsonTree)["simulation"]["encountersMatrixFilename"];
	predationsMatrixFilename = (*jsonTree)["simulation"]["predationsMatrixFilename"];
	nodesMatrixFilename = (*jsonTree)["simulation"]["nodesMatrixFilename"];
	predationEventsOnOtherSpeciesFilename = (*jsonTree)["simulation"]["predationEventsOnOtherSpeciesFilename"];

	saveAnimalConstitutiveTraits = (*jsonTree)["simulation"]["saveAnimalConstitutiveTraits"];

	saveEdibilitiesFile = (*jsonTree)["simulation"]["saveEdibilitiesFile"];

	saveGeneticsSummaries = (*jsonTree)["simulation"]["saveGeneticsSummaries"];

	if ((*jsonTree)["simulation"]["saveIntermidiateVolumes"])
	{
		saveIntermidiateVolumes = true;
		saveIntermidiateVolumesPeriodicity = (*jsonTree)["simulation"]["saveIntermidiateVolumesPeriodicity"];
	}
	else
	{
		saveIntermidiateVolumes = false;
	}

	initializeOutputFiles(jsonTree);
}


void World::calculateAnimalSpeciesK_Density() const
{
	vector<InstarVector<bool>> checkedAnimalSpecies(getExistingAnimalSpecies().size()), cannibalismAnimalSpecies(getExistingAnimalSpecies().size());
	for(AnimalSpecies* const &animalSpecies : getMutableExistingAnimalSpecies()) {
		checkedAnimalSpecies[animalSpecies->getAnimalSpeciesId()] = InstarVector<bool>(animalSpecies->getNumberOfInstars(), false);
		cannibalismAnimalSpecies[animalSpecies->getAnimalSpeciesId()] = InstarVector<bool>(animalSpecies->getNumberOfInstars(), false);
	}

	for(AnimalSpecies* const &animalSpecies : getMutableExistingAnimalSpecies()) {
		animalSpecies->calculateK_Density(checkedAnimalSpecies, cannibalismAnimalSpecies);
	}
}


void World::setOutputFolder(fs::path outputFolder)
{
  this->outputFolder = outputFolder;

  fs::create_directories(outputFolder / fs::path("Snapshots"));
  fs::create_directories(outputFolder / fs::path("Matrices"));
  fs::create_directories(outputFolder / fs::path("animals_each_day_start"));
  fs::create_directories(outputFolder / fs::path("animals_each_day_end"));
  fs::create_directories(outputFolder / fs::path("cells_each_day"));
  fs::create_directories(outputFolder / fs::path("animals_each_day_growth"));
  fs::create_directories(outputFolder / fs::path("animals_each_day_voracities"));
  fs::create_directories(outputFolder / fs::path("animals_each_day_encounterProbabilities"));
  fs::create_directories(outputFolder / fs::path("animals_each_day_predationProbabilities"));
}


void World::setResourceToPreysCapacityTransference(double resourceToPreysCapacityTransference)
{
	if (resourceToPreysCapacityTransference > 0)
	{
		this->resourceToPreysCapacityTransference = resourceToPreysCapacityTransference;
	}
	else
	{
		std::cerr << "Error, resourceToPreysCapacityTransference must be a positive value. You entered " << resourceToPreysCapacityTransference << " ... EXITING" << std::endl;
		exit(-1);
	}
}


void World::setPreysToPredatorsCapacityTransference(double preysToPredatorsCapacityTransference)
{
	if (preysToPredatorsCapacityTransference >= 0)
	{
		this->preysToPredatorsCapacityTransference = preysToPredatorsCapacityTransference;
	}
	else
	{
		std::cerr << "Error, preysToPredatorsCapacityTransference must be a non-negative value. You entered " << preysToPredatorsCapacityTransference << " ... EXITING" << std::endl;
		exit(-1);
	}
}


World::~World()
{
	
}


void World::readAnimalSpeciesFromJSONFiles()
{
	cout << "Reading all animal species from JSON files ... " << endl;
	
	fs::path speciesFolder = inputFolder / speciesFolderName;
	fs::directory_iterator end_iter;
	if (fs::exists(speciesFolder) && fs::is_directory(speciesFolder))
	{
		vector<fs::path> filePaths;
		copy(fs::directory_iterator(speciesFolder), fs::directory_iterator(), back_inserter(filePaths));
		sort(filePaths.begin(), filePaths.end());             // directory iteration is not ordered on some file systems, so we sort them
		json ptMain;

		map<AnimalSpecies*, json> ontogeneticLinks;

		for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(SPECIES_SCHEMA));

				cout << " - Animal scientific name: " << (string)ptMain["animal"]["name"] << endl;

				AnimalSpecies* newSpecies = addAnimalSpecies(AnimalSpeciesFactory::createInstance(ptMain["animal"], getInitIndividualsPerDensities(), this));

				ontogeneticLinks[newSpecies] = ptMain["ontogeneticLinks"];
			}
		}

		cout << "DONE" << endl << endl;
		cout << "Assigning links between species from JSON files edible species information ... " << endl;


		addOntogeneticLinks(ontogeneticLinks);

		for(auto &animalSpecies : getMutableExistingAnimalSpecies())
		{
			animalSpecies->obtainEdibleSearchParams(this);
			animalSpecies->obtainBreedSearchParams(this);
			animalSpecies->obtainCellEvaluationSearchParams(this);
			animalSpecies->obtainMatureFemalesSearchParams(this);
			animalSpecies->obtainPopulationSearchParams(this);
			animalSpecies->obtainLifeStageSearchParams(this);
		}
	}
	else
	{
		cerr << "The specified path \"" + speciesFolder.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}

	cout << "DONE" << endl << endl;

	for(AnimalSpecies* animalSpecies : getMutableExistingAnimalSpecies())
	{
		animalSpecies->setInitialPredationEventsOnOtherSpecies(getExistingAnimalSpecies().size() + existingResourceSpecies.size());
	}

	for(auto &animalSpecies : getMutableExistingAnimalSpecies())
	{
		animalSpecies->generateInvolvedResourceSpecies(getExistingSpecies());
	}
}


void World::addSpeciesOntogeneticLinks(AnimalSpecies* const predatorSpecies, const json &resourceLinks, const json &animalLinks)
{
	for(const auto &link : resourceLinks)
	{
		ResourceSpecies* resourceSpeciesToBeAdded = getResourceSpecies(link["species"]);

		cout << " - Resource: " << resourceSpeciesToBeAdded->getScientificName() << endl;

		predatorSpecies->addEdibleOntogeneticLink(Species::Type::Resource, resourceSpeciesToBeAdded, link);
	}

	for(const auto &link : animalLinks)
	{
		AnimalSpecies* animalSpeciesToBeAdded = getAnimalSpecies(link["species"]);

		cout << " - Animal: " << animalSpeciesToBeAdded->getScientificName() << endl;

		predatorSpecies->addEdibleOntogeneticLink(Species::Type::Animal, animalSpeciesToBeAdded, link);
	}
}

void World::addOntogeneticLinks(const map<AnimalSpecies*, json> &ontogeneticLinks)
{
	for(const auto &[animalSpecies, link] : ontogeneticLinks) 
	{
		cout << "Animal species " << animalSpecies->getScientificName() << " eats: " << endl;

		animalSpecies->initEdibleOntogeneticLink(getExistingSpecies());

		addSpeciesOntogeneticLinks(animalSpecies, link["resourceLinks"], link["animalLinks"]);

		animalSpecies->checkOntogeneticLinksPreference();
	}

	calculateTotalNumberOfOntogeneticLinks();
}

void World::calculateTotalNumberOfOntogeneticLinks()
{
	vector<unsigned int> numberOfLinksAsPredator(getExistingAnimalSpecies().size(), 0);
	vector<unsigned int> numberOfLinksAsPrey(getExistingAnimalSpecies().size(), 0);

	for(const AnimalSpecies* predatorAnimalSpecies : getExistingAnimalSpecies())
	{
		for(const AnimalSpecies* preyAnimalSpecies : getExistingAnimalSpecies())
		{
			for(const Instar &predatorInstar : predatorAnimalSpecies->getInstarsRange())
			{
				for(const Instar &preyInstar : preyAnimalSpecies->getInstarsRange())
				{
					if(predatorAnimalSpecies->canEatEdible(preyAnimalSpecies->getId(), predatorInstar, preyInstar))
					{
						numberOfLinksAsPredator[predatorAnimalSpecies->getAnimalSpeciesId()]++;
						numberOfLinksAsPrey[preyAnimalSpecies->getAnimalSpeciesId()]++;
					}
				}
				
			}
		}
	}

	for(AnimalSpecies* animalSpecies : getMutableExistingAnimalSpecies())
	{
		animalSpecies->setNumberOfLinks(numberOfLinksAsPredator[animalSpecies->getAnimalSpeciesId()], numberOfLinksAsPrey[animalSpecies->getAnimalSpeciesId()]);
	}
}

void World::readResourceSpeciesFromJSONFiles()
{
	cout << "Reading all resource species from JSON files ... " << endl;
	
	fs::path resourceFolder = inputFolder / resourceFolderName;
	fs::directory_iterator end_iter;
	if (fs::exists(resourceFolder) && fs::is_directory(resourceFolder))
	{
		vector<fs::path> filePaths;
		copy(fs::directory_iterator(resourceFolder), fs::directory_iterator(), back_inserter(filePaths));
		sort(filePaths.begin(), filePaths.end());             // directory iteration is not ordered on some file systems, so we sort them
		json ptMain;
		for(vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(RESOURCE_SCHEMA));

				cout << " - Resource scientific name: " << (string)ptMain["resource"]["name"] << endl << endl;

				addResourceSpecies(ResourceSpeciesFactory::createInstance(ptMain["resource"]));
			}
		}
	}
	else
	{
		cerr << "The specified path \"" + resourceFolder.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}
}


void World::readResourcePatchesFromJSONFiles()
{
	cout << "Reading all resource patches from JSON files ... " << endl;
	
	vector<priority_queue<unique_ptr<const ResourcePatch>>> resourcePatchesToAplly(getExistingResourceSpecies().size());

	fs::path resourcePatchesFolder = inputFolder / resourceFolderName / RESOURCE_PATCH_FOLDER;
	if(fs::exists(resourcePatchesFolder) && fs::is_directory(resourcePatchesFolder))
	{
		vector<fs::path> filePaths;
		copy(fs::directory_iterator(resourcePatchesFolder), fs::directory_iterator(), back_inserter(filePaths));

		for(vector<fs::path>::const_iterator it = filePaths.begin(); it != filePaths.end(); ++it)
		{
			if(it->extension() == ".json")
			{
				string patchFilename = it->string();

				// Read configuration file
				json ptMain = readConfigFile(*it, WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(RESOURCE_PATCH_SCHEMA));

				bool foundResourceSpecies = false;
				unsigned int resourceSpeciesId;
				for(const ResourceSpecies* const &resourceSpecies : getExistingResourceSpecies())
				{
					if(resourceSpecies->getScientificName() == ptMain["patch"]["resourceSpecies"])
					{
						resourceSpeciesId = resourceSpecies->getResourceSpeciesId();
						foundResourceSpecies = true;
					}
				}

				if(!foundResourceSpecies)
				{
					throwLineInfoException("The resource species associated with the patch has not been found.");
				}

				resourcePatchesToAplly[resourceSpeciesId].push(ResourcePatchFactory::createInstance(ptMain["patch"], getMutableExistingResourceSpecies().at(resourceSpeciesId), patchFilename));
			}
		}
	}
	else
	{
		throwLineInfoException("The specified path \"" + resourcePatchesFolder.string() + "\" does not exist or it is not a directory");
	}


	for(const ResourceSpecies* const &resourceSpecies : getExistingResourceSpecies())
	{
		cout << " - Resource scientific name: " << resourceSpecies->getScientificName() << endl << endl;

		while(!resourcePatchesToAplly[resourceSpecies->getResourceSpeciesId()].empty())
		{
			applyResourcePatch(*resourcePatchesToAplly[resourceSpecies->getResourceSpeciesId()].top());
			resourcePatchesToAplly[resourceSpecies->getResourceSpeciesId()].pop();
		}
	}
}


void World::addResourceSpecies(unique_ptr<ResourceSpecies> newSpecies)
{
	for(const auto &resourceSpecies : getExistingResourceSpecies())
	{
		if(*resourceSpecies == *newSpecies)
		{
			throwLineInfoException("Error, the " + newSpecies->getScientificName() + " was already added to this world");
		}
	}

	existingResourceSpecies.push_back(move(newSpecies));
	existingResourceSpeciesPointers.push_back(existingResourceSpecies.back().get());
	existingResourceSpeciesConstPointers.push_back(existingResourceSpeciesPointers.back());

	addSpecies(existingResourceSpeciesConstPointers.back());
}


AnimalSpecies* World::addAnimalSpecies(unique_ptr<AnimalSpecies> newSpecies)
{
	for(const auto &animalSpecies : getExistingAnimalSpecies())
	{
		if(*animalSpecies == *newSpecies)
		{
			throwLineInfoException("Error, the " + newSpecies->getScientificName() + " was already added to this world");
		}
	}

	existingAnimalSpecies.push_back(move(newSpecies));
	existingAnimalSpeciesPointers.push_back(existingAnimalSpecies.back().get());
	existingAnimalSpeciesConstPointers.push_back(existingAnimalSpeciesPointers.back());

	addSpecies(existingAnimalSpeciesConstPointers.back());

	return existingAnimalSpeciesPointers.back();
}

void World::addSpecies(const Species* newSpecies)
{
	existingSpeciesConstPointers.push_back(newSpecies);
}

ResourceSpecies* World::getResourceSpecies(string name)
{
	for(auto &resourceSpecies : existingResourceSpecies)
	{
		if(resourceSpecies->getScientificName() == name)
		{
			return resourceSpecies.get();
		}
	}

	throwLineInfoException("\"" + name + "\" doesn't exist. Please check the resource species name or contact developers");
}


AnimalSpecies * World::getAnimalSpecies(const string& name)
{
	for(const auto &animalSpecies : getMutableExistingAnimalSpecies())
	{
		if(animalSpecies->getScientificName() == name)
		{
			return animalSpecies;
		}
	}

	throwLineInfoException("\"" + name + "\" doesn't exist. Please check the animal species name or contact developers");
}


void World::printAnimalsAlongCells(const int day, const int simulationPoint) const
{
	if(((day%recordEach)==0) | (day==0))
	{
		string pathBySimulationPoint;
		if (simulationPoint == 0)
		{
			pathBySimulationPoint = "animals_each_day_start";
		}
		else if (simulationPoint == 1)
		{
			pathBySimulationPoint = "animals_each_day_end";
		}
		
		ofstream file;

		createOutputFile(file, outputFolder / fs::path(pathBySimulationPoint), "animals_day_", "txt", day, recordEach);
		if (!file.is_open())
		{
			throwLineInfoException("Error opening the file");
		}
		
		file << "id\tspecies\tgender\t";

		for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
		{
			file << magic_enum::enum_names<Axis>()[axis] << "\t";
		}
		
		file << "state\tinstar\tpheno_ini\tdate_egg\tage_first_rep\trep_count\tfecundity\tdate_death\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\tencounters_pred\tglobal_pred_encs\tdays_digest\tvor_ini\tsearch_ini\tspeed_ini\ttank_ini\tpheno_ini\tcurrentBodySize\tcurrentDryMass\t" << Trait::printAvailableTraits() << endl;
		
		worldMap->printAnimalsAlongCells(file);

		file.close();
	}
}


void World::printCellAlongCells(const int day) const
{
	if(((day%recordEach)==0) | (day==0))
	{
		string pathBySimulationPoint = "cells_each_day";
		
		ofstream file;

		createOutputFile(file, outputFolder / fs::path(pathBySimulationPoint), "cells_day_", "txt", day, recordEach);
		if (!file.is_open())
		{
			throwLineInfoException("Error opening the file");
		}
		
		file << worldMap->getMapPositionHeader() << "\t";

		for(const auto &resourceSpecies : existingResourceSpecies)
		{
			file << resourceSpecies->getScientificName() << "\t";
		}

		for(const AnimalSpecies* const &animalSpecies : getExistingAnimalSpecies())
		{
			file << animalSpecies->getScientificName() << "\t";
		}

		file << endl;

		worldMap->printCellAlongCells(file);

		file.close();
	}
}


ostream& World::printExtendedDailySummary(ostream& os, int day)
{
	vector<double> worldResourceBiomass(getExistingResourceSpecies().size(), 0.0);
	vector<vector<unsigned int>> worldAnimalsPopulation(getExistingAnimalSpecies().size(), vector<unsigned int>(LifeStage::size(), 0));

	worldMap->obtainWorldResourceBiomassAndAnimalsPopulation(worldResourceBiomass, worldAnimalsPopulation);

	os << day << "\t";

	for(const auto &resourceBiomass : worldResourceBiomass)
	{
		os << resourceBiomass << "\t";
	}

	for(const auto &animalSpeciesPopulation : worldAnimalsPopulation)
	{
		for(const auto &lifeStagePopulation : animalSpeciesPopulation)
		{
			os << lifeStagePopulation << "\t";
		}
	}
	
	os << endl;

	return os;
}


const unsigned int World::getTimeStepsPerDay() const
{
	return timeStepsPerDay;
}


void World::printGeneticsSummaries(int day)
{
	vector<vector<unsigned int>> worldAnimalsPopulation(getExistingAnimalSpecies().size(), vector<unsigned int>(LifeStage::size(), 0));
	vector<vector<pair<vector<double>, vector<double>>>> worldGeneticsFrequencies(getExistingAnimalSpecies().size());

	for(const auto &animalSpecies : getExistingAnimalSpecies())
	{
		worldGeneticsFrequencies[animalSpecies->getAnimalSpeciesId()].resize(animalSpecies->getNumberOfVariableTraits());
	}

	worldMap->obtainAnimalsPopulationAndGeneticsFrequencies(worldAnimalsPopulation, worldGeneticsFrequencies);

	for(const auto &animalSpecies : getExistingAnimalSpecies())
	{
		geneticsSummaryFile[animalSpecies->getAnimalSpeciesId()] << day << "\t";

		unsigned int population = 0;

		for(const auto &lifeStage : {LifeStage::UNBORN, LifeStage::ACTIVE, LifeStage::REPRODUCING})
		{
			population += worldAnimalsPopulation[animalSpecies->getAnimalSpeciesId()][lifeStage];
		}

		geneticsSummaryFile[animalSpecies->getAnimalSpeciesId()] << population << "\t";

		for(unsigned int variableTrait = 0; variableTrait < animalSpecies->getNumberOfVariableTraits(); variableTrait++)
		{
			const pair<vector<double>, vector<double>>* const traitValues = &worldGeneticsFrequencies[animalSpecies->getAnimalSpeciesId()][variableTrait];
			
			geneticsSummaryFile[animalSpecies->getAnimalSpeciesId()] << accumulate(traitValues->first.begin(), traitValues->first.end(), 0.0) / traitValues->first.size() << "\t";
			geneticsSummaryFile[animalSpecies->getAnimalSpeciesId()] << accumulate(traitValues->second.begin(), traitValues->second.end(), 0.0) / traitValues->second.size() << "\t";
		}
		
		geneticsSummaryFile[animalSpecies->getAnimalSpeciesId()] << endl;
	}
}


void World::saveAnimalSpeciesSnapshot(fs::path filenameRoot, string filename, int day, AnimalSpecies* species)
{
	if(species->getTotalInitialPopulation() > 0)
	{
		string scientificName = species->getScientificName();
		std::replace(scientificName.begin(), scientificName.end(), ' ', '_');

		ofstream file;
		string fullPath = createOutputFile(file, filenameRoot, filename + "_" + scientificName + "_day_", "dat", day, recordEach, ios::out | ios::binary);

		cout << "Saving Animal as " << fullPath << "... ";

		worldMap->saveAnimalSpeciesSnapshot(file, species);

		cout << "DONE" << endl;

		file.close();
	}
}


void World::saveResourceSpeciesSnapshot(fs::path filenameRoot, string filename, int day, ResourceSpecies* species)
{
	string scientificName = species->getScientificName();
	std::replace(scientificName.begin(), scientificName.end(), ' ', '_');

	ofstream file;
	string fullPath = createOutputFile(file, filenameRoot, filename + "_" + scientificName + "_day_", "dat", day, recordEach, ios::out | ios::binary);

	cout << "Saving Resource as " << fullPath << "... ";

	worldMap->saveResourceSpeciesSnapshot(file, species);

	cout << "DONE" << endl;

	file.close();
}


void World::saveWaterSnapshot(fs::path filenameRoot, string filename, int day)
{
	ofstream file;
	string fullPath = createOutputFile(file, filenameRoot, filename + "_day_", "dat", day, recordEach, ios::out | ios::binary);

	cout << "Saving Water volume as " << fullPath << "... ";

	worldMap->saveWaterSnapshot(file);

	file.close();
	cout << "DONE" << endl;
}


void World::printPredationEventsOnOtherSpeciesMatrix(ostream& predationEventsOnOtherSpeciesFile)
{
	predationEventsOnOtherSpeciesFile << "\t";
	for(const AnimalSpecies* const &animalSpecies : getExistingAnimalSpecies())
	{
		predationEventsOnOtherSpeciesFile << animalSpecies->getScientificName() << "\t";
	}

	predationEventsOnOtherSpeciesFile << endl;

	for(const auto &animalSpecies : getExistingAnimalSpecies())
	{
		predationEventsOnOtherSpeciesFile << animalSpecies->getScientificName() << "\t";
		for(const auto &otherAnimalSpecies : getExistingAnimalSpecies())
		{
			predationEventsOnOtherSpeciesFile
					<< animalSpecies->getPredationEventsOnOtherSpecies().at(otherAnimalSpecies->getId()) << "\t";
		}
		predationEventsOnOtherSpeciesFile << endl;
	}

}

/*

void World::printInteractionMatrices(ostream& encountersMatrixFile, ostream& predationsMatrixFile,
		ostream& nodesMatrixFile)
{
	unsigned int totalInitialPopulation = 0;

	for (unsigned int i = 0; i < existingAnimalSpecies.size(); i++)
	{
		totalInitialPopulation += existingAnimalSpecies[i]->getTotalInitialPopulation();
	}

	for (unsigned int i = 0; i < totalInitialPopulation; ++i)
	{
		encountersMatrixFile << "\t" << i;
		predationsMatrixFile << "\t" << i;
	}

	nodesMatrixFile << "node\tcategory" << endl;

	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = 0; z < depth; z++)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				currentTerrainCell = getCell(z,y,x);
				if(!currentTerrainCell->isObstacle())
				{
					currentTerrainCell->printInteractionMatrices(encountersMatrixFile, predationsMatrixFile, nodesMatrixFile, totalInitialPopulation);
				}
			}
		}
	}
}
*/

void World::updateInitialMapMoisture()
{
	updateAllMoistureSource(0);
	worldMap->updateMoisture();
}

void World::updateMap(const unsigned int timeStep, ofstream &tuneTraitsFile)
{
	updateAllMoistureSource(timeStep);
	worldMap->update(timeStep, tuneTraitsFile);
}


void World::updateAllMoistureSource(const unsigned int timeStep)
{
	for(auto &elem : appliedMoisture)
	{
		elem->refreshValue(timeStep);
	}
}

void World::evolveWorld()
{
	saveWaterSnapshot(outputFolder / fs::path("Snapshots"), "Water_initial", 0);

	for(auto &resourceSpecies : existingResourceSpecies)
	{
		saveResourceSpeciesSnapshot(outputFolder / fs::path("Snapshots"), "Resource_initial", 0, resourceSpecies.get());
	}

	// Next is intentionally commented because it would give empty volumes (animals are unborn)
	/*
	 std::vector<AnimalSpecies *>::iterator animIt;

	 for (animIt = existingAnimalSpecies.begin(); animIt != existingAnimalSpecies.end(); animIt++)
	 {
	 saveAnimalSpeciesSnapshot(outputFolder + "/Snapshots/" + "Animal_initial", 0, **animIt);
	 }
	 */

	ofstream timeSpentFile;
	createOutputFile(timeSpentFile, outputFolder, "time_spent", "txt");
	if (!timeSpentFile.is_open())
	{
		cerr << "Error opening the file." << endl;
	}

	for (unsigned int timeStep = 0; timeStep < runDays*timeStepsPerDay; timeStep++)
	{
		cout << "Running on day " << timeStep << " out of " << runDays*timeStepsPerDay << endl;

		printAnimalsAlongCells(timeStep, 0);

//#####################################################################
//##########################  UPDATING MAP   ##########################
//#####################################################################

		string pathBySimulationPointTuneTraits = "animals_each_day_growth";

		ofstream tuneTraitsFile;
		createOutputFile(tuneTraitsFile, outputFolder / fs::path(pathBySimulationPointTuneTraits), "animals_growth_day_", "txt", timeStep, recordEach);
		if (!tuneTraitsFile.is_open())
		{
			throwLineInfoException("Error opening the file");
		}
		
		
		tuneTraitsFile << "growth\tLinf\tid\tspecies\tstate\tcurrent_age\tinstar\tbody_size\tenergy_tank\ttankAtGrowth\tbody_mass\tmature\tmin_mass_for_death\tfinalJMinVB\tfinalJMaxVB\tvoracity_ini\texpectedDryMassFromMinVor\texpectedDryMassFromMaxVor\tmaxMassNextInstarPlasticity\tcurrentWetMass\tpreT_search\tpreT_speed\tpostT_search\tpostT_speed\tini_mass_instar\ttarget_next_mass\tminimum_met_loss\tcondition_search\tcondition_speed\tnon_condition_voracity\tcondition_voracity\tafter_encounters_voracity\tafter_encounters_search\tfinal_speed\tdeath_date" << endl;
		
		cout << " - Updating map ... " << endl << flush;
		
		auto t0 = clock();
		updateMap(timeStep, tuneTraitsFile);
		auto t1 = clock();

		cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
		timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";
		
		cout << "DONE" << endl << flush;

		tuneTraitsFile.close();

//#####################################################################
//#########################  MOVING ANIMALS   #########################
//#####################################################################

		string pathBySimulationPoint = "animals_each_day_encounterProbabilities";

		ofstream encounterProbabilitiesFile, predationProbabilitiesFile;

		createOutputFile(encounterProbabilitiesFile, outputFolder / fs::path(pathBySimulationPoint), "animals_encounterProbabilities_day_", "txt", timeStep, recordEach);
		if (!encounterProbabilitiesFile.is_open())
		{
			throwLineInfoException("Error opening the file");
		}
		
		pathBySimulationPoint = "animals_each_day_predationProbabilities";

		createOutputFile(predationProbabilitiesFile, outputFolder / fs::path(pathBySimulationPoint), "animals_predationProbabilities_day_", "txt", timeStep, recordEach);
		if (!predationProbabilitiesFile.is_open())
		{
			throwLineInfoException("Error opening the file");
		}

		encounterProbabilitiesFile << "idSearcher\tidSearched\tspeciesSearcher\tspeciesSearched\tsearchedIsPredator\tmassSearcher\tmassSearched\tprobRandomEncounter\tprobLogisticEncounter\tprobAttack\tsuccessfulEncounter" << endl; 
		predationProbabilitiesFile << "idHunter\tidHunted\tspeciesHunter\tspeciesHunted\thuntedIsPredator\tmassHunter\tmassHunted\tprobRandomPredation\tprobLogisticPredation\tsuccessfulPredation" << endl;

		cout << " - Moving animals ... " << endl << flush;

		cout << "0%|";
		for(double currentPercentage = printBarEach; currentPercentage < 1.0; currentPercentage += printBarEach)
		{
			cout << "  ";
		}
		cout << "|100%" << endl;
		cout << "   ";

		t0 = clock();
		worldMap->moveAnimals(timeStep, encounterProbabilitiesFile, predationProbabilitiesFile, getSaveEdibilitiesFile(), edibilitiesFile, exitTimeThreshold, getPdfThreshold(), getMuForPDF(), getSigmaForPDF(), getPredationSpeedRatioAH(), getPredationHunterVoracityAH(), getPredationProbabilityDensityFunctionAH(), getPredationSpeedRatioSAW(), getPredationHunterVoracitySAW(), getPredationProbabilityDensityFunctionSAW(), getMaxSearchArea(), getEncounterHuntedVoracitySAW(), getEncounterHunterVoracitySAW(), getEncounterVoracitiesProductSAW(), getEncounterHunterSizeSAW(), getEncounterHuntedSizeSAW(), getEncounterProbabilityDensityFunctionSAW(), getEncounterHuntedVoracityAH(), getEncounterHunterVoracityAH(), getEncounterVoracitiesProductAH(), getEncounterHunterSizeAH(), getEncounterHuntedSizeAH(), getEncounterProbabilityDensityFunctionAH());
		t1 = clock();

		cout << endl;
		
		cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
		timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";

		cout << "DONE" << endl << flush;

		encounterProbabilitiesFile.close();
		predationProbabilitiesFile.close();

//#####################################################################
//##########  BACKGROUND, ASSIMILATING FOOD & REPRODUCING   ###########
//#####################################################################

		string pathBySimulationPointVoracities = "animals_each_day_voracities";

		ofstream voracitiesFile;
		createOutputFile(voracitiesFile, outputFolder / fs::path(pathBySimulationPointVoracities), "animals_voracities_day_", "txt", timeStep, recordEach);
		if (!voracitiesFile.is_open())
		{
			throwLineInfoException("Error opening the file");
		}
		
		voracitiesFile << "id\tspecies\tstate\tcurrentAge\tinstar\tbody_size\tenergy_tank\tdryMass\tnextDinoMass\tmin_mass_for_death\tafter_encounters_voracity\tfood_mass\tdryMassAfterAssim\ttotalMetabolicDryMassLossAfterAssim\teatenToday\tsteps\tstepsAttempted\tafter_encounters_search\tsated\tpercentMoving\tpercentHandling\tvoracity_body_mass_ratio\tgender\tmated\teggDryMass\tK\tL\tpseudoK\tpseudoL\tfactorEggMass\teggDryMassAtBirth\tdeath_date\tageOfFirstMaturation\trep_count" << endl;
		
		cout << " - Background, assimilating food and reproducing ... " << endl << flush;
		
		t0 = clock();
		worldMap->performAnimalsActions(timeStep, voracitiesFile, outputFolder, getSaveAnimalConstitutiveTraits(), getConstitutiveTraitsFile());
		t1 = clock();

		cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
		timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";

		cout << "DONE" << endl << flush;

		voracitiesFile.close();

//#####################################################################
//#################  PRINTING ANIMALS ALONG CELLS   ###################
//#####################################################################

		cout << " - Printing animals along cells ... " << endl << flush;

		t0 = clock();
		printAnimalsAlongCells(timeStep, 1);
		t1 = clock();

		cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
		timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";

		cout << "DONE" << endl << flush;

//#####################################################################
//##################  PRINTING EXTENDED SUMMARY   #####################
//#####################################################################

		cout << " - Printing summary file ... " << endl << flush;

		t0 = clock();
		printExtendedDailySummary(extendedDailySummaryFile, timeStep);
		t1 = clock();

		cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
		timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";

		cout << "DONE" << endl << flush;

//#####################################################################
//####################  PURGING DEAD ANIMALS   ########################
//#####################################################################

		cout << " - Purging dead animals ... " << endl << flush;

		t0 = clock();
		worldMap->purgeDeadAnimals();
		t1 = clock();

		cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
		timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t" << endl;

		cout << "DONE" << endl << flush;


		//ALWAYS print this genetics after purguing dead animals or before the whole day
		if(saveGeneticsSummaries)
		{
			//TODO FIX THIS INSIDE according to new Genome classes... (use debugging)
			printGeneticsSummaries(timeStep);
		}

		printCellAlongCells(timeStep);

		ofstream predationEventsOnOtherSpeciesFile;
		createOutputFile(predationEventsOnOtherSpeciesFile, outputFolder / fs::path("Matrices"), predationEventsOnOtherSpeciesFilename, "txt");
		if (!predationEventsOnOtherSpeciesFile.is_open())
		{
			throwLineInfoException("Error opening the file");
		}
		
		printPredationEventsOnOtherSpeciesMatrix(predationEventsOnOtherSpeciesFile);
		predationEventsOnOtherSpeciesFile.close();

		if (saveIntermidiateVolumes && (((timeStep + 1) % saveIntermidiateVolumesPeriodicity) == 0))
		{
			saveWaterSnapshot(outputFolder / fs::path("Snapshots"), "Water", timeStep);

			for(auto &resourceSpecies : existingResourceSpecies)
			{
				saveResourceSpeciesSnapshot(outputFolder / fs::path("Snapshots"), "Resource", timeStep, resourceSpecies.get());
			}

			for(AnimalSpecies* const animalSpecies : getMutableExistingAnimalSpecies())
			{
				saveAnimalSpeciesSnapshot(outputFolder / fs::path("Snapshots"), "Animal", timeStep, animalSpecies);
			}
		}

		if(exitAtFirstExtinction)
		{
			if (isExtinguished())
			{
				break;
			}
		}
	}

	saveWaterSnapshot(outputFolder / fs::path("Snapshots"), "Water_final", runDays*timeStepsPerDay);

	for(auto &resourceSpecies : existingResourceSpecies)
	{
		saveResourceSpeciesSnapshot(outputFolder / fs::path("Snapshots"), "Resource_final", runDays*timeStepsPerDay, resourceSpecies.get());
	}

	for(AnimalSpecies* const animalSpecies : getMutableExistingAnimalSpecies())
	{
		saveAnimalSpeciesSnapshot(outputFolder / fs::path("Snapshots"), "Animal_final", runDays*timeStepsPerDay, animalSpecies);
	}

	/*
	encountersMatrixFilename = outputFolder + "/Matrices/" + encountersMatrixFilename;
	predationsMatrixFilename = outputFolder + "/Matrices/" + predationsMatrixFilename;
	nodesMatrixFilename = outputFolder + "/Matrices/" + nodesMatrixFilename;

	ofstream encountersMatrixFile;
	encountersMatrixFile.open(encountersMatrixFilename.c_str());
	if (!encountersMatrixFile.is_open())
	{
		cerr << "Error opening the file." << endl;
	}
	else
	{
		ofstream predationsMatrixFile;
		predationsMatrixFile.open(predationsMatrixFilename.c_str());
		if (!predationsMatrixFile.is_open())
		{
			cerr << "Error opening the file." << endl;
		}
		else
		{
			ofstream nodesMatrixFile;
			nodesMatrixFile.open(nodesMatrixFilename.c_str());
			if (!nodesMatrixFile.is_open())
			{
				cerr << "Error opening the file." << endl;
			}
			else
			{
				//printInteractionMatrices(encountersMatrixFile, predationsMatrixFile, nodesMatrixFile);
				encountersMatrixFile.close();
				predationsMatrixFile.close();
				nodesMatrixFile.close();
			}
		}
	}
	cout << endl;
	cout << "============================================================" << endl;
	cout << "File \"" << encountersMatrixFilename << "\" saved" << endl;
	cout << "============================================================" << endl;

	cout << endl;
	cout << "============================================================" << endl;
	cout << "File \"" << predationsMatrixFilename << "\" saved" << endl;
	cout << "============================================================" << endl;

	cout << endl;
	cout << "============================================================" << endl;
	cout << "File \"" << nodesMatrixFilename << "\" saved" << endl;
	cout << "============================================================" << endl;

*/
	edibilitiesFile.close();
	timeSpentFile.close();
	dailySummaryFile.close();
	extendedDailySummaryFile.close();
}

/*

void World::deleteExtinguishedReproducingAnimals()
{
	std::vector<double> animalPopulation(existingAnimalSpecies.size(), 0);

	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = 0; z < depth; z++)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				for (unsigned int k = 0; k < existingAnimalSpecies.size(); k++)
				{
					if(existingAnimalSpecies[k]->isExtinguished() == false)
					{
						currentTerrainCell = getCell(z,y,x);
						if(!currentTerrainCell->isObstacle())
						{
							animalPopulation[k] += currentTerrainCell->getLifeStagePopulation(LifeStage::ACTIVE, existingAnimalSpecies[k]);
							animalPopulation[k] += currentTerrainCell->getLifeStagePopulation(LifeStage::UNBORN, existingAnimalSpecies[k]);
							animalPopulation[k] += currentTerrainCell->getLifeStagePopulation(LifeStage::SATIATED, existingAnimalSpecies[k]);
							animalPopulation[k] += currentTerrainCell->getLifeStagePopulation(LifeStage::HANDLING, existingAnimalSpecies[k]);
						}
					}
				}
			}
		}
	}

	for (unsigned int k = 0; k < animalPopulation.size(); k++)
	{
		if (animalPopulation[k] == 0 && existingAnimalSpecies[k]->isExtinguished() == false)
		{

			for (unsigned int z = 0; z < depth; z++)
			{
				for (unsigned int y = 0; y < length; y++)
				{
					for (unsigned int x = 0; x < width; x++)
					{
						currentTerrainCell = getCell(z,y,x);
						if(!currentTerrainCell->isObstacle())
						{
							currentTerrainCell->deleteExtinguishedReproducingAnimals(existingAnimalSpecies[k]);
						}
					}
				}
			}
			existingAnimalSpecies[k]->setExtinguished(true);
				if(exitAtFirstExtinction){
					break;
				}
		}
	}

}
*/

void World::increaseMovePrintBar()
{
	currentNumberOfTerrainCellsMoved++;

	if(currentNumberOfTerrainCellsMoved >= currentMovePercentage * worldMap->getTotalNumberOfTerrainCells())
	{
		cout << "==" << flush;
		currentMovePercentage += printBarEach;
	}

	// Reset currentMovePercentage
	if(currentNumberOfTerrainCellsMoved == worldMap->getTotalNumberOfTerrainCells())
	{
		currentMovePercentage = printBarEach;
		currentNumberOfTerrainCellsMoved = 0;
	}
}

bool World::isExtinguished()
{
	vector<double> worldResourceBiomass(getExistingResourceSpecies().size(), 0.0);
	vector<vector<unsigned int>> worldAnimalsPopulation(getExistingAnimalSpecies().size(), vector<unsigned int>(LifeStage::size(), 0));

	worldMap->obtainWorldResourceBiomassAndAnimalsPopulation(worldResourceBiomass, worldAnimalsPopulation);

	for(const auto &resourceBiomass : worldResourceBiomass)
	{
		if(resourceBiomass < numeric_limits<double>::epsilon())
		{
			return true;
		}
	}

	for(const auto &animalPopulation : worldAnimalsPopulation)
	{
		unsigned int population = 0;

		for(const auto &elem : animalPopulation)
		{
			population += elem;
		}

		if(population == 0)
		{
			return true;
		}
	}

	return false;
}

// TODO Mario Initialize animals with initIndividualsPerDensities
/*
void World::setHeatingCodeTemperatureCycle(string temperatureFilename)
{
	ifstream temperatureFile(inputFolder + "/temperature/" + temperatureFilename);
	if(!temperatureFile.good())
	{
		cerr <<"The file \"" << temperatureFilename << "\" does not exist.";
		exit(1);
	}
	istream_iterator<float> start(temperatureFile), end;
	heatingCodeTemperatureCycle = vector<float>(start, end);
	//cout << "Read " << temperatureCycle.size() << " numbers" << endl;
}
*/


void World::initializeMap(const json &mapConfig)
{
	cout << "Initializing terrain voxels ... " << flush;

	worldMap = MapFactory::createInstance(mapConfig, this);

	for(AnimalSpecies* const animalSpecies : getMutableExistingAnimalSpecies())
	{
		animalSpecies->calculateCellDepthPerInstar(worldMap.get());
	}

	worldMap->obtainLifeStageSearchParams();

	cout << "DONE" << endl;
}


void World::readPatchesFromJSONFiles(const json &moistureBasePatchInfo, const bool initialPatches)
{
	readObstaclePatchesFromJSONFiles();

	readMoisturePatchesFromJSONFiles(moistureBasePatchInfo);
	if(initialPatches)
	{
		updateInitialMapMoisture();
	}

	readResourcePatchesFromJSONFiles();
}


void World::readObstaclePatchesFromJSONFiles()
{
	cout << "Reading obstacle patches from JSON files ... " << endl;

	priority_queue<unique_ptr<const ObstaclePatch>> obstaclePatchesToAplly;

	fs::path obstacleFolder = inputFolder / obstacleFolderName;
	fs::directory_iterator end_iter;

	if (fs::exists(obstacleFolder) && fs::is_directory(obstacleFolder))
	{
		vector<fs::path> filePaths;
		copy(fs::directory_iterator(obstacleFolder), fs::directory_iterator(), back_inserter(filePaths));
		sort(filePaths.begin(), filePaths.end());             // directory iteration is not ordered on some file systems, so we sort them
		json ptMain;
		for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				string patchFilename = it->string();

				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(OBSTACLE_PATCH_SCHEMA));

				obstaclePatchesToAplly.push(ObstaclePatchFactory::createInstance(ptMain["patch"], patchFilename));
			}
		}
	}
	else
	{
		cerr << "The specified path \"" + obstacleFolder.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}

	while(!obstaclePatchesToAplly.empty())
	{
		applyObstaclePatch(obstaclePatchesToAplly.top());
		obstaclePatchesToAplly.pop();
	}
}


void World::readMoisturePatchesFromJSONFiles(const json &moistureBasePatchInfo)
{
	cout << "Reading moisture patches from JSON files ... " << endl;
	
	priority_queue<unique_ptr<MoisturePatch>> moisturePatchesToAplly;

	moisturePatchesToAplly.push(MoisturePatchFactory::createInstance(moistureBasePatchInfo));

	fs::path moistureFolder = inputFolder / moistureFolderName;
	fs::directory_iterator end_iter;

	if (fs::exists(moistureFolder) && fs::is_directory(moistureFolder))
	{
		vector<fs::path> filePaths;
		copy(fs::directory_iterator(moistureFolder), fs::directory_iterator(), back_inserter(filePaths));
		
		json ptMain;
		for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				string patchFilename = it->string();

				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(MOISTURE_PATCH_SCHEMA));

				moisturePatchesToAplly.push(MoisturePatchFactory::createInstance(ptMain["patch"], patchFilename));
			}
		}
	}
	else
	{
		cerr << "The specified path \"" + moistureFolder.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}

	while(!moisturePatchesToAplly.empty())
	{
		applyMoisturePatch(*moisturePatchesToAplly.top());
		moisturePatchesToAplly.pop();
	}
}


void World::applyMoisturePatch(MoisturePatch &moisturePatch)
{
	cout << moisturePatch.showPatchInfo() << endl << endl;
	bool isApplied = worldMap->applyMoisturePatch(moisturePatch);
	if(isApplied)
	{
		addAppliedMoisture(moisturePatch);
	}
}


void World::addAppliedMoisture(MoisturePatch &moisturePatch)
{
	appliedMoisture.push_back(move(moisturePatch.getMutableMoistureInfo()));
}

void World::applyResourcePatch(const ResourcePatch &resourcePatch)
{
	cout << resourcePatch.showPatchInfo() << endl << endl;
	bool isApplied = worldMap->applyResourcePatch(resourcePatch);
	if(isApplied)
	{
		resourcePatch.getSpecies()->updateK_Density(resourcePatch.getResourceMaximumCapacityDensity());
	}
}


void World::applyObstaclePatch(const std::unique_ptr<const ObstaclePatch> &obstaclePatch)
{
	cout << obstaclePatch->showPatchInfo() << endl << endl;
	worldMap->applyObstaclePatch(obstaclePatch);
}


bool World::getCompetitionAmongResourceSpecies() const
{
	return competitionAmongResourceSpecies;
}


const vector<const ResourceSpecies*>& World::getExistingResourceSpecies() const
{
	return existingResourceSpeciesConstPointers;
}

const vector<ResourceSpecies*>& World::getMutableExistingResourceSpecies() const
{
	return existingResourceSpeciesPointers;
}

const vector<AnimalSpecies*>& World::getMutableExistingAnimalSpecies() const
{
	return existingAnimalSpeciesPointers;
}

const vector<const AnimalSpecies*>& World::getExistingAnimalSpecies() const
{
	return existingAnimalSpeciesConstPointers;
}

const vector<const Species*>& World::getExistingSpecies() const
{
	return existingSpeciesConstPointers;
}

void World::setInitialEcosystemSize(unsigned long newInitialEcosystemSize)
{
	if (newInitialEcosystemSize <= 0)
		throw WorldSizeException("Trying to assign wrong value to world initial ecosystem size (must be positive)");

	initialEcosystemSize = newInitialEcosystemSize;
}


void World::setExitTimeThreshold(float exitTimeThreshold)
{
	this->exitTimeThreshold = exitTimeThreshold;
}


void World::setMaxLogMassRatio(float maxLogMassRatio)
{
	this->maxLogMassRatio = maxLogMassRatio;
}


void World::setMinLogMassRatio(float minLogMassRatio)
{
	this->minLogMassRatio = minLogMassRatio;
}


void World::setSigmaForPDF(float sigmaForPDF)
{
	this->sigmaForPDF = sigmaForPDF;
}


void World::setMuForPDF(float muForPDF)
{
	this->muForPDF = muForPDF;
}


void World::setObstacleFolderName(fs::path newObstacleFolderName)
{
	obstacleFolderName = newObstacleFolderName;
}


void World::setMoistureFolderName(fs::path newMoistureFolderName)
{
	moistureFolderName = newMoistureFolderName;
}


void World::setResourceFolderName(fs::path newResourceFolderName)
{
	resourceFolderName = newResourceFolderName;
}


void World::setSpeciesFolderName(fs::path newSpeciesFolderName)
{
	speciesFolderName = newSpeciesFolderName;
}


/*
void World::setGaussianResourcePatch(ResourceSpecies* species, unsigned int xpos, unsigned int ypos, unsigned int zpos,
		unsigned int radius, float sigma, float amplitude, double resourceMaximumCapacity, bool patchSpread)
{
	// Generate water patches

	// Generate random coordinates for the center of the patch
	Coordinate3D<int> center(xpos, ypos, zpos);
	IsotropicGaussian3D gauss;

	Output::cout("Initializing Gaussian resource patch ({}) ...\n\n", species->getScientificName());

	Output::cout(" - Position (x,y,z) = {},{},{} ... ", xpos, ypos, zpos);
	Output::cout(" - Parameters (Influence radius, Amplitude, Sigma) = {},{},{} ... ", radius, amplitude, sigma);

	gauss.setSigma(sigma);
	gauss.setAmplitude(amplitude);
	cout << "DONE" << endl;

	double resourceAsGauss;

	// Now iterate around the center to fill up with new water contents
	TerrainCell* currentTerrainCell = NULL;
	for (int x = (center.getX() - (int) radius); x <= (center.getX() + (int) radius); x++)
	{
		if (x >= 0 && x < (int) width) // Ckeck limits
		{
			for (int y = (center.getY() - (int) radius); y <= (center.getY() + (int) radius); y++)
			{
				if (y >= 0 && y < (int) length) // Ckeck limits
				{
					for (int z = (center.getZ() - (int) radius); z <= (center.getZ() + (int) radius); z++)
					{
						if (z >= 0 && z < (int) depth) // Ckeck limits
						{
							currentTerrainCell = getCell(z,y,x);
							if (!currentTerrainCell->isObstacle())
							{
								resourceAsGauss = gauss.getValueAtDistance(center.getX() - x, center.getY() - y, center.getZ() - z); // Value obtained from Gaussian

								Resource* aux = currentTerrainCell->getResource(species);

								if (aux == NULL)
								{
									currentTerrainCell->addResource(new Resource(species, resourceAsGauss, resourceMaximumCapacity, getCompetitionAmongResourceSpecies(), massRatio, patchSpread));
									//TODO PROVISIONAL!!!!
									currentTerrainCell->setAuxInitialResourceBiomass(resourceAsGauss, species->getId());
								}
								else
								{
									aux->setBiomass(max(resourceAsGauss, aux->calculateWetMass()));
									//TODO PROVISIONAL!!!!
									currentTerrainCell->setAuxInitialResourceBiomass(max(resourceAsGauss, aux->calculateWetMass()), species->getId());
								}
							}
						}
					}
				}
			}
		}
	}
}

void World::setRandomGaussianResourcePatches(ResourceSpecies* species, unsigned int number, float radius, float newSigma, bool useRandomSigma, float newAmplitude, bool useRandomAmplitude, double resourceMaximumCapacity, bool patchSpread)
{
	// Generate water patches
	IsotropicGaussian3D gauss;

	Output::cout("Initializing random Gaussian resource patches ({}) ... \n\n", species->getScientificName());

	float sigma, amplitude;

	for (unsigned int i = 0; i < number; i++)
	{
		if (useRandomSigma)
		{
			sigma = Random::randomFloatInRange(1, radius / 3.0);
		}
		else
		{
			sigma = newSigma;
		}

		if (useRandomAmplitude)
		{
			amplitude = Random::randomFloatInRange(0, newAmplitude);
		}
		else
		{
			amplitude = newAmplitude;
		}

		Output::cout(" - Creating random Gaussian shape with (Influence, Amplitude, Sigma) = {},{},{} ... ", radius, amplitude, sigma);
		gauss.setSigma(sigma);
		gauss.setAmplitude(amplitude);

		// Generate random coordinates for the center of the patch
		Coordinate3D<int> center(
			Random::randomIntegerInRange(0, width - 1),
			Random::randomIntegerInRange(0, length - 1),
			Random::randomIntegerInRange(0, depth - 1)
		);
		Output::cout(" - Position (x,y,z) = {},{},{} ... ", center.getX(), center.getY(), center.getZ());

		cout << "DONE" << endl;

		double resourceAsGauss;

		// Now iterate around the center to fill up with new water contents
		TerrainCell* currentTerrainCell = NULL;
		for (int x = (center.getX() - (int) radius); x <= (center.getX() + (int) radius); x++)
		{
			if (x >= 0 && x < (int) width) // Ckeck limits
			{
				for (int y = (center.getY() - (int) radius); y <= (center.getY() + (int) radius); y++)
				{
					if (y >= 0 && y < (int) length) // Ckeck limits
					{
						for (int z = (center.getZ() - (int) radius); z <= (center.getZ() + (int) radius); z++)
						{
							if (z >= 0 && z < (int) depth) // Ckeck limits
							{
								currentTerrainCell = getCell(z,y,x);
								if (!currentTerrainCell->isObstacle())
								{
									resourceAsGauss = gauss.getValueAtDistance(center.getX() - x, center.getY() - y, center.getZ() - z); // Value obtained from Gaussian

									Resource* aux = currentTerrainCell->getResource(species);

									if (aux == NULL)
									{
										currentTerrainCell->addResource(new Resource(species, resourceAsGauss, resourceMaximumCapacity, getCompetitionAmongResourceSpecies(), massRatio, patchSpread));
										//TODO PROVISIONAL!!!!
										currentTerrainCell->setAuxInitialResourceBiomass(resourceAsGauss, species->getId());
									}
									else
									{
										aux->setBiomass(max(resourceAsGauss, aux->calculateWetMass()));
										//TODO PROVISIONAL!!!!
										currentTerrainCell->setAuxInitialResourceBiomass(max(resourceAsGauss, aux->calculateWetMass()), species->getId());
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

*/


/*
void World::setGaussianWaterPatch(unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, float sigma, float amplitude)
{
	// Generate water patches
	IsotropicGaussian3D gauss;

	Output::cout("Initializing Gaussian water patch ... \n\n");

	Output::cout(" - Position (x,y,z) = {},{},{} ... ", xpos, ypos, zpos);
	Output::cout(" - Parameters (Influence radius, Amplitude, Sigma) = {},{},{} ... ", radius, amplitude, sigma);

	gauss.setSigma(sigma);
	gauss.setAmplitude(amplitude);
	cout << "DONE" << endl;

	// Generate random coordinates for the center of the patch
	Coordinate3D<int> center(xpos, ypos, zpos);

	float waterAsGauss;

	// Now iterate around the center to fill up with new water contents
	TerrainCell* currentTerrainCell = NULL;
	for (int x = (center.getX() - (int) radius); x <= (center.getX() + (int) radius); x++)
	{
		if (x >= 0 && x < (int) width) // Ckeck limits
		{
			for (int y = (center.getY() - (int) radius); y <= (center.getY() + (int) radius); y++)
			{
				if (y >= 0 && y < (int) length) // Ckeck limits
				{
					for (int z = (center.getZ() - (int) radius); z <= (center.getZ() + (int) radius); z++)
					{
						if (z >= 0 && z < (int) depth) // Ckeck limits
						{
							currentTerrainCell = getCell(z,y,x);
							if (!currentTerrainCell->isObstacle())
							{
								waterAsGauss = gauss.getValueAtDistance(center.getX() - x, center.getY() - y, center.getZ() - z); // Value obtained from Gaussian
								currentTerrainCell->setRelativeHumidityOnRainEvent(waterAsGauss);
							}
						}
					}
				}
			}
		}
	}
}

void World::setRandomGaussianWaterPatches(unsigned int number, float radius, float newSigma, bool useRandomSigma,
		float newAmplitude, bool useRandomAmplitude)
{
	// Generate water patches
	IsotropicGaussian3D gauss;

	Output::cout("Initializing random Gaussian water patches ... \n\n");

	float sigma, amplitude;

	for (unsigned int i = 0; i < number; i++)
	{
		if (useRandomSigma)
		{
			sigma = Random::randomFloatInRange(1, radius / 3.0);
		}
		else
		{
			sigma = newSigma;
		}

		if (useRandomAmplitude)
		{
			amplitude = Random::randomFloatInRange(0, newAmplitude);
		}
		else
		{
			amplitude = newAmplitude;
		}

		Output::cout(" - Creating random Gaussian shape with (Influence, Amplitude, Sigma) = {},{},{} ... ", radius, amplitude, sigma);
		gauss.setSigma(sigma);
		gauss.setAmplitude(amplitude);

		// Generate random coordinates for the center of the patch
		Coordinate3D<int> center(
			Random::randomIntegerInRange(0, width - 1),
			Random::randomIntegerInRange(0, length - 1),
			Random::randomIntegerInRange(0, depth - 1)
		);
		Output::cout(" - Position (x,y,z) = {},{},{} ... ", center.getX(), center.getY(), center.getZ());

		cout << "DONE" << endl;

		float waterAsGauss;

		// Now iterate around the center to fill up with new water contents
		TerrainCell* currentTerrainCell = NULL;
		for (int x = (center.getX() - (int) radius); x <= (center.getX() + (int) radius); x++)
		{
			if (x >= 0 && x < (int) width) // Ckeck limits
			{
				for (int y = (center.getY() - (int) radius); y <= (center.getY() + (int) radius); y++)
				{
					if (y >= 0 && y < (int) length) // Ckeck limits
					{
						for (int z = (center.getZ() - (int) radius); z <= (center.getZ() + (int) radius); z++)
						{
							if (z >= 0 && z < (int) depth) // Ckeck limits
							{
								currentTerrainCell = getCell(z,y,x);
								if (!currentTerrainCell->isObstacle())
								{
									waterAsGauss = gauss.getValueAtDistance(center.getX() - x, center.getY() - y, center.getZ() - z); // Value obtained from Gaussian
									currentTerrainCell->setRelativeHumidityOnRainEvent(waterAsGauss);
								}
							}
						}
					}
				}
			}
		}
	}
}
*/

pair<AnimalInterface*, Instar> World::getRandomPredator(const unsigned int numberOfTotalPotentialPredators,
		const InstarVector<vector<AnimalInterface*>*> &potentialPredators) const
{
	unsigned int predatorIndex = Random::randomIndex(numberOfTotalPotentialPredators);
	AnimalInterface* predator;

	bool foundPredator = false;

	Instar predatorInstar(1);
	while(!foundPredator)
	{
		if(potentialPredators[predatorInstar] != nullptr)
		{
			if(predatorIndex < potentialPredators[predatorInstar]->size())
			{
				foundPredator = true;
				predator = potentialPredators[predatorInstar]->at(predatorIndex);
			}
			else
			{
				predatorIndex -= potentialPredators[predatorInstar]->size();
			}
		}

		if(!foundPredator)
		{
			predatorInstar.moveOnNextInstar();
		}
	}

	return make_pair(predator, predatorInstar);
}

AnimalInterface* World::getRandomPrey(const InstarVector<unsigned int> &numberOfPotentialPreysPerInstar,
		const Instar &predatorInstar, const AnimalSpecies* const predatorSpecies,
		const vector<InstarVector<vector<AnimalInterface*>>> &animalsPopulation) const
{
	unsigned int preyIndex = Random::randomIndex(numberOfPotentialPreysPerInstar[predatorInstar]);
	AnimalInterface* prey;

	bool foundPrey = false;

	auto speciesIt = predatorSpecies->getInstarEdibleAnimalSpecies(predatorInstar).begin();
	while(!foundPrey)
	{
		for(const Instar &preyInstar : speciesIt->second)
		{
			if(preyIndex < animalsPopulation[static_cast<AnimalSpecies*>(speciesIt->first)->getAnimalSpeciesId()][preyInstar].size())
			{
				foundPrey = true;
				prey = animalsPopulation[static_cast<AnimalSpecies*>(speciesIt->first)->getAnimalSpeciesId()][preyInstar].at(preyIndex);
			}
			else
			{
				preyIndex -= animalsPopulation[static_cast<AnimalSpecies*>(speciesIt->first)->getAnimalSpeciesId()][preyInstar].size();
			}
		}
		
		if(!foundPrey)
		{
			++speciesIt;
		}
	}

	return prey;
}

void World::calculateAttackStatistics(vector<InstarVector<vector<vector<TerrainCellInterface*>::iterator>>> &mapSpeciesInhabitableTerrainCells)
{
	cout << "Size of the Animal class: " << sizeof(Animal) << endl;
	cout << "Size of the Genome class: " << sizeof(Genome) << endl;
	cout << "Size of the TerrainCell class: " << sizeof(TerrainCell) << endl;
	cout << "Creating heating code individuals... " << endl;

	unique_ptr<vector<InstarVector<vector<AnimalInterface*>>>> animalsPopulation;
	unsigned int populationSize;

	tie(animalsPopulation, populationSize) = worldMap->generateStatisticsPopulation(mapSpeciesInhabitableTerrainCells);

	//Calculating pseudoGrowthSd
	for(auto &speciesPopulation : *animalsPopulation)
	{
		for(auto &instarPopulation : speciesPopulation)
		{
			for(auto &animal : instarPopulation)
			{
				animal->sumPseudoGrowthSd();
			}
		}
	}

	for(AnimalSpecies* const animalSpecies : getMutableExistingAnimalSpecies())
	{
		animalSpecies->calculatePseudoGrowthSd();
	}

	for(auto &speciesPopulation : *animalsPopulation)
	{
		for(auto &instarPopulation : speciesPopulation)
		{
			for(auto &animal : instarPopulation)
			{
				//TODO Ver en CUÁNTO cambian usando los 1.000.000 animales , probando el calentamiento 2 o 3 veces
				animal->interpolateTraits();
				//DONE Usar calculateGrowthCurves CADA DÍA , y no al nacer.
				//DONE ELIMINAR SPEED DEL ADJUST TRAITS YA QUE QUEDA INCLUIDO EN EL TUNETRAITS DE ABAJO
				animal->adjustTraits();
				//DONE Forzar el crecimiento de forma DIARIA y utilizando growtcurves+tunetraits CADA DÍA para el ciclo de temperaturas
				Temperature temperatureOnMolting = animal->forceMolting();
				animal->setExperiencePerSpecies();
				ofstream noStream;
				animal->tuneTraits(-1, timeStepsPerDay, temperatureOnMolting, 100, noStream, false, true);

				updateMaxSearchArea(animal->getSearchArea());

				//DONE tuneTraits tiene que ir AQUI y ADEMÁS con la temperatura final en la que mudaron CADA UNO DE ELLOS
			}
		}
	}
	cout << "A total of " << populationSize << " heating code individuals have been created." << endl;

	//Only for predators. The experiment is carried out for every predator species and its linked species, up until the specified numberOfCombinations.

	cout << "Calculating attack statistics: " << endl;
	for(AnimalSpecies* const predatorAnimalSpecies : getMutableExistingAnimalSpecies())
	{
		cout << ">> Simulating " << numberOfCombinations << " attacks from the species \"" << predatorAnimalSpecies->getScientificName() << "\"... " << endl;

		unsigned int numberOfTotalPotentialPredators = 0;
		InstarVector<vector<AnimalInterface*>*> potentialPredators(predatorAnimalSpecies->getNumberOfInstars(), nullptr);

		InstarVector<unsigned int> numberOfPotentialPreysPerInstar(predatorAnimalSpecies->getNumberOfInstars(), 0);

		for(const Instar &predatorInstar : predatorAnimalSpecies->getInstarsRange())
		{
			if(predatorAnimalSpecies->eatsAnimalSpecies(predatorInstar))
			{
				potentialPredators[predatorInstar] = &animalsPopulation->at(predatorAnimalSpecies->getAnimalSpeciesId())[predatorInstar];
				numberOfTotalPotentialPredators += potentialPredators[predatorInstar]->size();

				for(const auto &[preySpecies, speciesInstars] : predatorAnimalSpecies->getInstarEdibleAnimalSpecies(predatorInstar))
				{
					for(const Instar &preyInstar : speciesInstars)
					{
						numberOfPotentialPreysPerInstar[predatorInstar] += animalsPopulation->at(static_cast<AnimalSpecies*>(preySpecies)->getAnimalSpeciesId())[preyInstar].size();
					}
				}
			}
		}

		if(numberOfTotalPotentialPredators > 0)
		{
			float percentageForPrinting = 0.1;
			float currentPercentageForPrinting = percentageForPrinting;

			vector<pair<AnimalInterface*, AnimalInterface*>> vectorOfAttacks;
			vectorOfAttacks.reserve(numberOfCombinations);

			while(vectorOfAttacks.size() < numberOfCombinations)
			{
				Instar predatorInstar;
				AnimalInterface* predator;
				tie(predator, predatorInstar) = getRandomPredator(numberOfTotalPotentialPredators, potentialPredators);
				
				AnimalInterface* prey = getRandomPrey(numberOfPotentialPreysPerInstar, predatorInstar, predatorAnimalSpecies, *animalsPopulation);
				
				
				double probabilityDensityFunction = exp(-0.5 * pow((log(predator->calculateWetMass()/prey->calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
				
				double pdfThreshold = getPdfThreshold(); //to avoid feeding on too small prey

				if(probabilityDensityFunction >= pdfThreshold){  //Dinosaurs but everything else might be - p=0.003 from Alamosaurus - Tytannsaurus in PDF_fix_in heating_code.xls
					if(predator->canEatEdible(prey, list<const EdibleInterface *>(), prey->calculateDryMass()) && predator != prey)
					{
						auto currentAttack = make_pair(predator, prey);
						if(find(vectorOfAttacks.begin(), vectorOfAttacks.end(), currentAttack) == vectorOfAttacks.end())
						{
							vectorOfAttacks.push_back(currentAttack);

							//Computing the total mean values.
							predatorAnimalSpecies->sumStatisticMeans(predator->getCurrentBodySize(), predator->getVoracity(), predator->getSpeed(), predator->calculateDryMass(), prey->getCurrentBodySize(), prey->getVoracity(), prey->getSpeed(), prey->calculateDryMass(), muForPDF, sigmaForPDF);
							predatorAnimalSpecies->interactionRanges(predator->getCurrentBodySize(), predator->getVoracity(), predator->getSpeed(), predator->calculateDryMass(), prey->getCurrentBodySize(), prey->getVoracity(), prey->getSpeed(), prey->calculateDryMass(), muForPDF, sigmaForPDF);
						}
					}
					
					if(vectorOfAttacks.size() >= (numberOfCombinations-1)*currentPercentageForPrinting)
					{
						cout << ">>>> " << (int)(currentPercentageForPrinting*100) << "%... \n";
						currentPercentageForPrinting += percentageForPrinting;
					}
				}
			}

			//Computing the means
			predatorAnimalSpecies->computeStatisticMeans(vectorOfAttacks.size());

			for(unsigned int i = 0; i < vectorOfAttacks.size(); i++)
			{
				//Computing the total sd values.
				auto predator = vectorOfAttacks[i].first;
				auto prey = vectorOfAttacks[i].second;

				//Dinosaurs but everything else might be - p=0.003 from Alamosaurus - Tytannsaurus in PDF_fix_in heating_code.xls
				predatorAnimalSpecies->sumStatisticSds(predator->getCurrentBodySize(), predator->getVoracity(), predator->getSpeed(), predator->calculateDryMass(), prey->getCurrentBodySize(), prey->getVoracity(), prey->getSpeed(), prey->calculateDryMass(), muForPDF, sigmaForPDF);
			}

			//Computing the sds
			predatorAnimalSpecies->computeStatisticSds(vectorOfAttacks.size());
		}
	}

	worldMap->eraseStatisticsPopulation();

	Edible::resetIds(Resource::resourceCounter);
	Animal::animalCounter = 0;
	cout << "Calculating attack statistics DONE" << endl;
}


void World::updateMaxSearchArea(double currentAnimalMaxSearchArea)
{
	if(currentAnimalMaxSearchArea > maxSearchArea)
	{
		maxSearchArea = currentAnimalMaxSearchArea;
	}
}


void World::printActualEcosystemSize()
{
	unsigned int actualEcosystemSize = 0;

	if(initIndividualsPerDensities)
	{
		throwLineInfoException("No implementation");

		// TODO Mario Initialize animals with initIndividualsPerDensities
		/*
		cout << "Calculating total initial population based on density per species ... " << endl;

		double totalResourceWetBiomass = 0.0;
		//Initializing trophic level biomass based on Hatton et al 2015
		for(vector<TerrainCell*>::const_iterator inhabitableTerrainCellsIt = inhabitableTerrainCells.cbegin(); inhabitableTerrainCellsIt != inhabitableTerrainCells.cend(); inhabitableTerrainCellsIt++)
		{
			totalResourceWetBiomass += (*inhabitableTerrainCellsIt)->getTotalResourceBiomass();
		}

		double totalPreyWetBiomass = 0.08*pow(totalResourceWetBiomass,0.75);
		double totalPredatorWetBiomass = 0.08*pow(totalPreyWetBiomass,0.75);

		cout << " - Initial ecosystem size from power-law relation: " << (totalPreyWetBiomass + totalPredatorWetBiomass) << endl;

		for (vector<Species *>::iterator speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
		{
			(*speciesIt)->initWetBiomassDensitiesPerAge(heatingCodeTemperatureCycle[0], timeStepsPerDay);
		}

		double totalPreyWetBiomassDensity = 0.0;
		double totalPredatorWetBiomassDensity = 0.0;
		for (vector<Species *>::iterator speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
		{
			if((*speciesIt)->canEatAnyResourceSpecies())
			{
				totalPreyWetBiomassDensity += (*speciesIt)->getTotalInitialPopulation();
			}
			if((*speciesIt)->canEatAnyAnimalSpecies())
			{
				totalPredatorWetBiomassDensity += (*speciesIt)->getTotalInitialPopulation();
			}
		}

		for (vector<Species *>::iterator speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
		{
			if((*speciesIt)->canEatAnyResourceSpecies())
			{
				(*speciesIt)->scaleInitialPopulation(totalPreyWetBiomassDensity, totalPreyWetBiomass);
				//InitialPopulation contains now the mass on each instar
				actualEcosystemSize += (*speciesIt)->getTotalInitialPopulation();
			}
			if((*speciesIt)->canEatAnyAnimalSpecies())
			{
				//(*speciesIt)->scaleInitialPopulation(totalPopulationDensity, initialEcosystemSize);
				(*speciesIt)->scaleInitialPopulation(totalPredatorWetBiomassDensity, totalPredatorWetBiomass);
				//InitialPopulation contains now the mass on each instar
				actualEcosystemSize += (*speciesIt)->getTotalInitialPopulation();
			}
		}

		cout << " - Total initial ecosystem biomass calculated: " << actualEcosystemSize << endl;
		cout << "(values may be different due to rounding) ... DONE" << endl << endl;
		*/
	}
	else
	{
		for(const AnimalSpecies* const &animalSpecies : getExistingAnimalSpecies())
		{
			actualEcosystemSize += animalSpecies->getTotalInitialPopulation();
		}
		cout << " - Total initial ecosystem size from input: " << actualEcosystemSize << " individuals." << endl;
	}
}


void World::initializeAnimals()
{
	printActualEcosystemSize();

	auto mapSpeciesInhabitableTerrainCells = worldMap->obtainSpeciesInhabitableTerrainCells();

	calculateAttackStatistics(*mapSpeciesInhabitableTerrainCells);

	cout << "Giving life to animals... " << endl;

	ofstream geneticsFile;

	createOutputFile(geneticsFile, outputFolder, "animal_genetics", "txt", ofstream::app);
	if (!geneticsFile.is_open())
	{
		throwLineInfoException("Error opening the file");
	}

	geneticsFile << "id\tspecies\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\t" << endl;

	for(AnimalSpecies* const animalSpecies : getMutableExistingAnimalSpecies())
	{
		int numberOfDiscardedIndividualsOutsideRestrictedRanges = 0;

		if(initIndividualsPerDensities)
		{
			throwLineInfoException("No implementation");

			// TODO Mario Initialize animals with initIndividualsPerDensities
			/*
			initialPopulation = currentAnimalSpecies->getInitialPopulation();

			cout << "Creating " << currentAnimalSpecies->getTotalInitialPopulation() << " biomass worth of the species \"" << currentAnimalSpecies->getScientificName() << "\"..." << endl;
			cout << ">> Number of inhabitable cells (with involved resources): " << speciesInhabitableTerrainCells->size() << endl;
			float percentageForPrinting = 0.1;
			float currentPercentageForPrinting = percentageForPrinting;
			double leftOverFromTotalBiomassAtPreviousAges = 0.0;
			for (unsigned int currentAgeStep = 0; currentAgeStep < initialPopulation->size(); ++currentAgeStep)
			{
				double ageAtInitialization = currentAgeStep * 1.0/timeStepsPerDay;
				double totalBiomassAtThisAge = initialPopulation->at(currentAgeStep) + leftOverFromTotalBiomassAtPreviousAges;
				double currentCreatedBiomassAtThisAge = 0.0;
				bool reachedTotalBiomassAtThisAge = false;
				while (!reachedTotalBiomassAtThisAge)
				{
					// Get a random index from this species inhabitable cells
					randomCellIndex = randomIndex(speciesInhabitableTerrainCells->size());
					TerrainCell * newCell = (*speciesInhabitableTerrainCells)[randomCellIndex];

					Animal * newAnimal = new Animal(currentAnimalSpecies->getNewGenome(), -1, newCell, 0, timeStepsPerDay, 0, 0, -1,
							-1, currentAnimalSpecies, currentAnimalSpecies->getRandomGender(), currentAnimalSpecies->getDefaultHuntingMode());

					pair<bool, bool> isInsideRestrictedRangesAndIsViableOffSpring = newAnimal->interpolateTraits();
					while(!isInsideRestrictedRangesAndIsViableOffSpring.first)
					{
						numberOfDiscardedIndividualsOutsideRestrictedRanges++;
						delete newAnimal;
						newAnimal = new Animal(currentAnimalSpecies->getNewGenome(), -1, newCell, 0, timeStepsPerDay, 0, 0, -1,
												-1, currentAnimalSpecies, currentAnimalSpecies->getRandomGender(), currentAnimalSpecies->getDefaultHuntingMode());
						isInsideRestrictedRangesAndIsViableOffSpring = newAnimal->interpolateTraits();
					}
					//ALWAYS print the traits after interpolating and before adjusting
					//newAnimal->printGenetics(geneticsFile);
					newAnimal->printTraits(constitutiveTraitsFile);
					//DONE Adjust traits now includes the first CalculateGrowthCurves call. For every animal.
					newAnimal->adjustTraits();
					newAnimal->forceMolting(timeStepsPerDay, ageAtInitialization, -1);
					double newAnimalWetMass = newAnimal->calculateWetMass();
					if(currentCreatedBiomassAtThisAge + newAnimalWetMass <= totalBiomassAtThisAge)
					{
						currentCreatedBiomassAtThisAge += newAnimalWetMass;
						newCell->addAnimal(newAnimal);
					}
					else
					{
						if(currentAgeStep != initialPopulation->size()-1)
						{
							delete newAnimal;
						}
						else
						{
							currentCreatedBiomassAtThisAge += newAnimalWetMass;
							newCell->addAnimal(newAnimal);
						}
						reachedTotalBiomassAtThisAge = true;
						leftOverFromTotalBiomassAtPreviousAges = totalBiomassAtThisAge - currentCreatedBiomassAtThisAge;
					}
				}
				if(currentAgeStep >= (initialPopulation->size()-1)*currentPercentageForPrinting)
				{
					cout << ">> " << (int)(currentPercentageForPrinting*100) << "%... " << endl;
					currentPercentageForPrinting += percentageForPrinting;
				}
			}
			*/
		}
		else
		{
			numberOfDiscardedIndividualsOutsideRestrictedRanges = worldMap->generatePopulation(animalSpecies, (*mapSpeciesInhabitableTerrainCells)[animalSpecies->getAnimalSpeciesId()]);
		}

		cout << ">> A total of " << numberOfDiscardedIndividualsOutsideRestrictedRanges << " individuals have been discarded due to at least one trait value being outside restricted ranges." << endl;
	}

	geneticsFile.close();

	cout << "DONE" << endl;
}


void World::initializeOutputFiles(json * jsonTree)
{
	fs::create_directories(outputFolder / fs::path("config"));

	// Copy simulation configuration
	fs::copy(inputFolder, outputFolder / fs::path("config") / inputFolder.filename(), fs::copy_options::recursive);

	fs::path dailySummariFilename = outputFolder / fs::path("dailySummary.txt");
	dailySummaryFile.open(dailySummariFilename);

	if (!dailySummaryFile.is_open())
	{
		cerr << "Error opening the dailySummaryFile." << endl;
	}
	else
	{
		dailySummaryFile
				<< "DAY\tBIOMASS\tPREY_UNBORN\tPREY_ACTIVE\tPREY_STARVED\tPREY_PREDATED\tPREY_REPRODUCING\tPREY_SENESCED\tPREY_SHOCKED\tPREDATOR_UNBORN\tPREDATOR_ACTIVE\tPREDATOR_STARVED\tPREDATOR_PREDATED\tPREDATOR_REPRODUCING\tPREDATOR_BACKGROUND\tPREDATOR_SENESCED\tPREDATOR_SHOCKED"
				<< endl;
	}

	fs::path extendedDailySummaryFilename = outputFolder / fs::path("extendedDailySummary.txt");
	extendedDailySummaryFile.open(extendedDailySummaryFilename);

	if (!extendedDailySummaryFile.is_open())
	{
		cerr << "Error opening the dailySummaryFile." << endl;
	}
	else
	{
		extendedDailySummaryFile << "day" << "\t";

		for (auto itResourceSpecies = existingResourceSpecies.begin(); itResourceSpecies != existingResourceSpecies.end(); itResourceSpecies++)
		{
			extendedDailySummaryFile << (*itResourceSpecies)->getScientificName() << "_biomass" << "\t";
		}

		for(const AnimalSpecies* const &animalSpecies : getExistingAnimalSpecies())
		{
			for(const auto &lifeStage : LifeStage::getEnumValues())
			{
				extendedDailySummaryFile << animalSpecies->getScientificName() << "_" << LifeStage::to_string(lifeStage) << "\t";
			}
		}
		extendedDailySummaryFile << endl;
	}

	if(saveGeneticsSummaries) {
		fs::create_directories(outputFolder / fs::path("geneticsSummaries"));

		geneticsSummaryFile.resize(getExistingAnimalSpecies().size());

		for(const auto &animalSpecies : getExistingAnimalSpecies())
		{
			string scientificName = animalSpecies->getScientificName();
			std::replace(scientificName.begin(), scientificName.end(), ' ', '_');

			createOutputFile(geneticsSummaryFile[animalSpecies->getAnimalSpeciesId()], outputFolder / fs::path("geneticsSummaries"), scientificName + "_geneticsSummary", "txt");

			if (!geneticsSummaryFile[animalSpecies->getAnimalSpeciesId()].is_open())
			{
				throwLineInfoException("Error opening the file '" + scientificName + "_geneticsSummary.txt'");
			}

			geneticsSummaryFile[animalSpecies->getAnimalSpeciesId()] << "day" << "\t" << "population" << "\t";

			for(const auto &variableTrait : *animalSpecies->getTypeVariableTraits())
			{
				geneticsSummaryFile[animalSpecies->getAnimalSpeciesId()] << magic_enum::enum_name(variableTrait) << "_cr1" << "\t";
				geneticsSummaryFile[animalSpecies->getAnimalSpeciesId()] << magic_enum::enum_name(variableTrait) << "_cr2" << "\t";
			}

			geneticsSummaryFile[animalSpecies->getAnimalSpeciesId()] << endl;
		}
	}
		
	if(saveEdibilitiesFile)
	{
		createOutputFile(edibilitiesFile, outputFolder, (*jsonTree)["simulation"]["edibilitiesFilename"], "txt", std::ofstream::out | std::ofstream::trunc);
	
		edibilitiesFile
		<< "timeStep" << "\t"
		<< "searcherId" << "\t"
		<< "searcherSpecies" << "\t"
		<< "foodMass" << "\t"
		<< "predatorId" << "\t"
		<< "predatorSpecies" << "\t"
		<< "predatorDryMass" << "\t"
		<< "predatedId" << "\t"
		<< "predatedSpecies" << "\t"
		<< "predatedDryMass" << "\t"
		<< "encounterProbability" << "\t"
		<< "predationProbability" << "\t"
		<< "preference" << "\t"
		<< "experience" << "\t"
		<< "edibility" << endl;
	}
	

	if(saveAnimalConstitutiveTraits) {
		createOutputFile(constitutiveTraitsFile, outputFolder, "animal_constitutive_traits", "txt");

		if(!constitutiveTraitsFile.is_open())
		{
			throwLineInfoException("Error opening the file 'animal_constitutive_traits.txt'");
		}

		constitutiveTraitsFile << "id\tspecies\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\t" << Trait::printAvailableTraits() << endl;
	}
}


const bool World::getInitIndividualsPerDensities() const
{
	return initIndividualsPerDensities;
}
