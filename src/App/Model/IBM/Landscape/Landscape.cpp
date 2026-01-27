
/*
#include "tbb/tbb.h"
*/
#include <thread>
#include <algorithm> // transform

#include "App/Model/IBM/Landscape/Landscape.h"

#include "Misc/Utilities.h"
#include "Misc/GlobalVariable.h"
#include "App/Model/IBM/Landscape/ArthropodsLandscape.h"
#include "App/Model/IBM/Landscape/DinosaursLandscape.h"
#include "App/Model/IBM/Landscape/AquaticLandscape.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




Landscape* Landscape::createInstance(const std::string& simulationType) {
    switch(EnumClass<Type>::stringToEnumValue(simulationType)) {
        case Type::Arthropods: {
            return new ArthropodsLandscape();
            break;
        }
        case Type::Dinosaurs: {
            return new DinosaursLandscape();
            break;
        }
		case Type::Aquatic: {
            return new AquaticLandscape();
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}



Landscape::Landscape()
	: serializationVersion(SERIALIZATION_VERSION)
{
	
}

void Landscape::init(View* newView, fs::path configPath, fs::path newOutputFolder, bool fromCheckpoint)
{
	if(!fromCheckpoint) {
		edibleIdCounter = 0;
		resourceIdCounter = 0;
		animalIdCounter = 0;
	}


	setView(newView);


	setOutputFolder(newOutputFolder, configPath.filename().string());


	setSimulationParams(configPath);


	setLandscapeParams(configPath, fromCheckpoint);


	if(fromCheckpoint) {
		landscapeResources = vector<vector<vector<CellResource*>>>(
			landscapeMap->getNumberOfCellsPerAxis(), 
			vector<vector<CellResource*>>(
				landscapeMap->getNumberOfCellsPerAxis(),
				vector<CellResource*>(
					getExistingResourceSpecies().size(),
					nullptr
				)
			)
		);

		landscapeMap->registerEdibles(landscapeResources, landscapeAnimals);

		for(AnimalNonStatistical* animal : landscapeAnimals)
		{
			animal->setInfoMassFileHeader(getResultFolder(), saveMassInfo);
		}

		landscapeMap->deserializeSpecies(existingResourceSpecies, existingAnimalSpecies);

		landscapeMap->deserializeSources(appliedMoisture, appliedResource);

		#ifdef DEBUG
		for(AnimalNonStatistical* animal : landscapeAnimals)
		{
			animal->deserializeDebugVariables();
		}
		#endif
	}
	else
	{
		landscapeResources = vector<vector<vector<CellResource*>>>(
			landscapeMap->getNumberOfCellsPerAxis(), 
			vector<vector<CellResource*>>(
				landscapeMap->getNumberOfCellsPerAxis()
			)
		);
	}


	bool newResourceSpecies = readResourceSpeciesFromJSONFiles(configPath);


	CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, unsigned int>> initialPopulation(getExistingAnimalSpecies().size());

	bool newAnimalSpecies = readAnimalSpeciesFromJSONFiles(configPath, initialPopulation);

	CustomIndexedVector<AnimalSpeciesID, std::vector<Genome>> initialGenomesPool(getExistingAnimalSpecies().size());

	if(!fromCheckpoint) {
		for(AnimalSpecies* animalSpecies : getMutableExistingAnimalSpecies())
		{
			animalSpecies->generateInitialGenomesPool(initialPopulation[animalSpecies->getAnimalSpeciesId()], initialGenomesPool[animalSpecies->getAnimalSpeciesId()]);
		}
	}

	readObstaclePatchesFromJSONFiles(configPath);

	readHabitatDomainPatchesFromJSONFiles(configPath);


	readMoisturePatchesFromJSONFiles(configPath);


	bool newResourcePatches = readResourcePatchesFromJSONFiles(configPath);


	fs::path ontogeneticLinksPreferencePath = configPath / SPECIES_FOLDER_NAME / "ontogeneticLinksPreference.csv";
	bool newOntogeneticLinks = fs::exists(ontogeneticLinksPreferencePath) && fs::is_regular_file(ontogeneticLinksPreferencePath);

	if((newResourceSpecies || newAnimalSpecies) && !newOntogeneticLinks) {
		throwLineInfoException("Error: If there are new species, the ontogenetic links must be redefined.");
	}


	bool newEcosystem = newResourceSpecies || newAnimalSpecies || newOntogeneticLinks;


	if(newEcosystem) {
		setOntogeneticLinks(configPath);
	}


	calculateAnimalSpeciesStatistics();


	landscapeMap->obtainInhabitableTerrainCells();


	initializeOutputFiles(configPath);


	unsigned int actualEcosystemSize = 0;

	for(const CustomIndexedVector<Instar, unsigned int>& individualsPerInstar : initialPopulation)
	{
		for(const unsigned int individuals : individualsPerInstar)
		{
			actualEcosystemSize += individuals;
		}
	}

	view->updateLog({" - Total initial ecosystem size from input: ", to_string(actualEcosystemSize), " individuals.\n"});

	if(newEcosystem || newResourcePatches || actualEcosystemSize > 0) {
		CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, std::vector<ResourceSpecies::ResourceID>>> involvedResourceSpecies;

		for(const AnimalSpecies* const animalSpecies : getExistingAnimalSpecies())
		{
			involvedResourceSpecies.emplace_back();

			animalSpecies->generateInvolvedResourceSpecies(getExistingSpecies(), getExistingAnimalSpecies(), involvedResourceSpecies);
		}


		vector<CustomIndexedVector<Instar, vector<vector<TerrainCell*>::iterator>>> mapSpeciesInhabitableTerrainCells;
		landscapeMap->obtainSpeciesInhabitableTerrainCells(mapSpeciesInhabitableTerrainCells, getExistingAnimalSpecies(), involvedResourceSpecies);


		if(newEcosystem || newResourcePatches) {
			calculateAttackStatistics(mapSpeciesInhabitableTerrainCells);
		}


		if(actualEcosystemSize > 0)
		{
			initializeAnimals(initialPopulation, initialGenomesPool, mapSpeciesInhabitableTerrainCells);
		}
	}
}

void Landscape::setSimulationParams(const fs::path& configPath)
{
	JsonValidator simulationValidator(SIMULATION_PARAMS_SCHEMA);

	json simulationConfiguration = readConfigFile(configPath / fs::path("simulation_params.json"), simulationValidator);


	if(simulationConfiguration["simulation"]["initFromFixedSeed"]["enabled"].get<bool>())
	{
		fixedSeedValue = simulationConfiguration["simulation"]["initFromFixedSeed"]["fixedSeedValue"].get<unsigned int>();
	}
	else
	{
		fixedSeedValue = (unsigned)time(NULL);
	}

	Random::initRandomGenerator(fixedSeedValue);


	checkpointsEnabled = simulationConfiguration["simulation"]["checkpoints"]["enabled"].get<bool>();
	if(checkpointsEnabled)
	{
		checkpointsRecordEach = simulationConfiguration["simulation"]["checkpoints"]["recordEach"].get<unsigned int>();
		binaryCheckpointEnabled = simulationConfiguration["simulation"]["checkpoints"]["binaryEnabled"].get<bool>();
	}


	exitAtFirstExtinction = simulationConfiguration["simulation"]["breakConditions"]["exitAtFirstExtinction"].get<bool>();


	runDays = Day(simulationConfiguration["simulation"]["runDays"].get<double>());
	recordEach = simulationConfiguration["simulation"]["outputs"]["recordEach"].get<unsigned int>();
	numberOfCombinations = simulationConfiguration["simulation"]["numberOfCombinations"].get<unsigned int>();
	timeStepsPerDay = simulationConfiguration["simulation"]["timeStepsPerDay"].get<double>();
	growthAndReproTest = simulationConfiguration["simulation"]["growthAndReproTest"].get<bool>();


	stabilityEnabled = simulationConfiguration["simulation"]["breakConditions"]["stability"]["enabled"].get<bool>();
	if(stabilityEnabled)
	{
		coefficientOfVariationThreshold = simulationConfiguration["simulation"]["breakConditions"]["stability"]["coefficientOfVariationThreshold"].get<double>();
		
		stabilityCalculationInterval = TimeStep(static_cast<unsigned int>(ceil(static_cast<double>(TimeStep(runDays, timeStepsPerDay).getValue()) * simulationConfiguration["simulation"]["breakConditions"]["stability"]["stabilityCalculationInterval"].get<double>())));
	}


	savePredationEventsOnOtherSpecies = simulationConfiguration["simulation"]["outputs"]["savePredationEventsOnOtherSpecies"].get<bool>();


	saveAnimalConstitutiveTraits = simulationConfiguration["simulation"]["outputs"]["saveAnimalConstitutiveTraits"].get<bool>();


	saveEdibilitiesFile = simulationConfiguration["simulation"]["outputs"]["saveEdibilitiesFile"].get<bool>();


	saveGenetics = simulationConfiguration["simulation"]["outputs"]["saveGenetics"].get<bool>();

	saveDailySummary = simulationConfiguration["simulation"]["outputs"]["saveDailySummary"].get<bool>();

	saveExtendedDailySummary = simulationConfiguration["simulation"]["outputs"]["saveExtendedDailySummary"].get<bool>();

	saveMovements = simulationConfiguration["simulation"]["outputs"]["saveMovements"].get<bool>();

	saveAnimalsEachDayStart = simulationConfiguration["simulation"]["outputs"]["saveAnimalsEachDayStart"].get<bool>();

	saveAnimalsEachDayEnd = simulationConfiguration["simulation"]["outputs"]["saveAnimalsEachDayEnd"].get<bool>();

	saveCellsEachDay = simulationConfiguration["simulation"]["outputs"]["saveCellsEachDay"].get<bool>();

	saveAnimalsEachDayVoracities = simulationConfiguration["simulation"]["outputs"]["saveAnimalsEachDayVoracities"].get<bool>();

	saveAnimalsEachDayPredationProbabilities = simulationConfiguration["simulation"]["outputs"]["saveAnimalsEachDayPredationProbabilities"].get<bool>();

	saveActivity = simulationConfiguration["simulation"]["outputs"]["saveActivity"].get<bool>();

	saveMassInfo = simulationConfiguration["simulation"]["outputs"]["saveMassInfo"].get<bool>();


	saveIntermidiateVolumes = simulationConfiguration["simulation"]["outputs"]["saveIntermidiateVolumes"].get<bool>();
	if(saveIntermidiateVolumes)
	{
		saveIntermidiateVolumesPeriodicity = simulationConfiguration["simulation"]["outputs"]["saveIntermidiateVolumesPeriodicity"].get<unsigned int>();
	}
}

void Landscape::setLandscapeParams(const fs::path& configPath, bool fromCheckpoint)
{
	JsonValidator landscapeValidator(LANDSCAPE_PARAMS_SCHEMA);

	json landscapeConfig = readConfigFile(configPath / fs::path("landscape_params.json"), landscapeValidator);


	competitionAmongResourceSpecies = landscapeConfig["landscape"]["life"]["competitionAmongResourceSpecies"].get<bool>();

	pdfThreshold = landscapeConfig["landscape"]["life"]["pdfThreshold"].get<double>();

	setExitTimeThreshold(landscapeConfig["landscape"]["life"]["exitTimeThreshold"]);

	multiplierForFieldMetabolicRate = landscapeConfig["landscape"]["life"]["multiplierForFieldMetabolicRate"].get<double>();


	if(!fromCheckpoint)
	{
		initializeMap(landscapeConfig["landscape"]["mapConfig"]);
	}
}


fs::path Landscape::getResultFolderName(const fs::path& outputFolder, const string& configName)
{
	auto tiempoActual = std::chrono::system_clock::now();
    std::time_t tiempoT = std::chrono::system_clock::to_time_t(tiempoActual);

	
    std::stringstream ss;

	std::tm tmLocal = {};

	#ifdef _WIN32
	localtime_s(&tmLocal, &tiempoT);
	#else
	localtime_r(&tiempoT, &tmLocal);
	#endif

    ss << std::put_time(&tmLocal, "%Y%m%d_%H%M%S");
    ss >> std::get_time(&tmLocal, "%Y%m%d_%H%M%S");

	string filename = configName + "__";

    filename += to_string(1900 + tmLocal.tm_year) + "-"; // Año
    filename += to_string(1 + tmLocal.tm_mon) + "-";    	// Mes
    filename += to_string(tmLocal.tm_mday) + "_";       	// Día
    filename += to_string(tmLocal.tm_hour) + "-";       	// Hora
    filename += to_string(tmLocal.tm_min) + "-";        	// Minuto
    filename += to_string(tmLocal.tm_sec);        		// Segundo

	return outputFolder / fs::path(filename);
}

bool Landscape::getSaveAnimalConstitutiveTraits() const 
{ 
	return saveAnimalConstitutiveTraits; 
}

bool Landscape::getSaveEdibilitiesFile() const 
{ 
	return saveEdibilitiesFile; 
}

void Landscape::setOutputFolder(fs::path newOutputFolder, const string& configName)
{
	if(!fs::is_directory(newOutputFolder)) 
	{
		throwLineInfoException("The specified path \"" + newOutputFolder.string() + "\" does not exist or it is not a directory");
	}

	outputFolder = newOutputFolder;

	resultFolder = Landscape::getResultFolderName(outputFolder, configName);

	fs::create_directories(resultFolder);

	fs::create_directories(resultFolder / fs::path("Snapshots"));
	fs::create_directories(resultFolder / fs::path("Matrices"));
	fs::create_directories(resultFolder / fs::path("animals_each_day_start"));
	fs::create_directories(resultFolder / fs::path("animals_each_day_end"));
	fs::create_directories(resultFolder / fs::path("cells_each_day"));
	fs::create_directories(resultFolder / fs::path("animals_each_day_voracities"));
	fs::create_directories(resultFolder / fs::path("animals_each_day_predationProbabilities"));
	fs::create_directories(resultFolder / fs::path("animals_each_day_activity"));
	fs::create_directories(resultFolder / fs::path("massInfo"));

	if(isCheckpointsEnabled())
	{
		fs::create_directories(resultFolder / fs::path("checkpoints"));
	}
}


Landscape::~Landscape()
{
	delete landscapeMap;

	for(AnimalSpecies* animalSpecies : existingAnimalSpecies)
	{
		delete animalSpecies;
	}

	for(ResourceSpecies* resourceSpecies : existingResourceSpecies)
	{
		delete resourceSpecies;
	}

	for (const auto& [priority, moisture] : appliedMoisture)
	{
		delete moisture;
	}

	for(const auto& resources : appliedResource)
	{
		for (const auto& [priority, resource] : resources)
		{
			delete resource;
		}
	}

	for(auto& animalSpeciesFile : animalConstitutiveTraitsFile)
	{
		delete animalSpeciesFile;
	}
}

void Landscape::setView(View* newView)
{
	view = newView;
}

bool Landscape::readAnimalSpeciesFromJSONFiles(const fs::path& configPath, CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, unsigned int>>& initialPopulation)
{
	view->updateLog("Reading all animal species from JSON files ... \n");

	fs::path speciesFolder = configPath / SPECIES_FOLDER_NAME;
	
	if (fs::exists(speciesFolder) && fs::is_directory(speciesFolder))
	{
		bool newAnimalSpecies = false;

		JsonValidator animalSpeciesValidator(SPECIES_SCHEMA);

		for (const auto& entry : fs::directory_iterator(speciesFolder))
		{
			if (entry.path().extension() == ".json")
			{
				json ptMain = readConfigFile(entry.path(), animalSpeciesValidator);

				view->updateLog({" - Animal scientific name: ", ptMain["animal"]["name"].get<string>(), "\n"});

				addAnimalSpecies(ptMain, initialPopulation);

				newAnimalSpecies = true;
			}
		}

		view->updateLog("DONE\n\n");

		return newAnimalSpecies;
	}
	else
	{
		throwLineInfoException("Error: The specified path \"" + speciesFolder.string() + "\" does not exist or it is not a directory");
	}
}

void Landscape::checkOntogeneticLinksHeaders(const rapidcsv::Document& ontogeneticLinksPreference, const rapidcsv::Document& ontogeneticLinksProfitability) const
{
	unsigned int numRows = 0, numCols = 0;

	for(const AnimalSpecies* const animalSpecies : getExistingAnimalSpecies())
	{
		numCols += animalSpecies->getGrowthBuildingBlock().getNumberOfInstars();
		numRows += animalSpecies->getGrowthBuildingBlock().getNumberOfInstars();
	}

	for(const ResourceSpecies* const resourceSpecies : getExistingResourceSpecies())
	{
		numRows += resourceSpecies->getGrowthBuildingBlock().getNumberOfInstars();
	}

	if(numRows != ontogeneticLinksPreference.GetRowCount())
	{
		throwLineInfoException("Error: Incorrect 'ontogeneticLinksPreference.csv' row headers.");
	}

	if(numCols != ontogeneticLinksPreference.GetColumnCount())
	{
		throwLineInfoException("Error: Incorrect 'ontogeneticLinksPreference.csv' column headers.");
	}

	if(numRows != ontogeneticLinksProfitability.GetRowCount())
	{
		throwLineInfoException("Error: Incorrect 'ontogeneticLinksProfitability.csv' row headers.");
	}

	if(numCols != ontogeneticLinksProfitability.GetColumnCount())
	{
		throwLineInfoException("Error: Incorrect 'ontogeneticLinksProfitability.csv' column headers.");
	}
}

void Landscape::setOntogeneticLinks(const fs::path& configPath)
{
	rapidcsv::Document ontogeneticLinksPreference((configPath / SPECIES_FOLDER_NAME / "ontogeneticLinksPreference.csv").string(), rapidcsv::LabelParams(0, 0));
	rapidcsv::Document ontogeneticLinksProfitability((configPath / SPECIES_FOLDER_NAME / "ontogeneticLinksProfitability.csv").string(), rapidcsv::LabelParams(0, 0));


	checkOntogeneticLinksHeaders(ontogeneticLinksPreference, ontogeneticLinksProfitability);


	for(AnimalSpecies* animalSpecies : getMutableExistingAnimalSpecies())
	{
		view->updateLog({"Animal species ", animalSpecies->getScientificName(), " eats: \n"});

		animalSpecies->setOntogeneticLinks(view, getExistingSpecies(), ontogeneticLinksPreference, ontogeneticLinksProfitability);
	}


	for(auto &animalSpecies : getMutableExistingAnimalSpecies())
	{
		animalSpecies->obtainPreyAndPredatorSearchParams(getExistingSpecies(), getExistingAnimalSpecies(), getExistingResourceSpecies());
	}


	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		landscapeAnimals[i]->updateVariablesAssociatedWithInstar();
	}
}

bool Landscape::readResourceSpeciesFromJSONFiles(const fs::path& configPath)
{
	view->updateLog("Reading all resource species from JSON files ... \n");
	
	fs::path resourceFolder = configPath / RESOURCE_FOLDER_NAME;
	
	if (fs::exists(resourceFolder) && fs::is_directory(resourceFolder))
	{
		bool newResourceSpecies = false;

		JsonValidator resourceSpeciesValidator(RESOURCE_SCHEMA);

		for (const auto& entry : fs::directory_iterator(resourceFolder))
		{
			if (entry.path().extension() == ".json")
			{
				json ptMain = readConfigFile(entry.path(), resourceSpeciesValidator);

				view->updateLog({" - Resource scientific name: ", ptMain["resource"]["name"].get<string>(), "\n\n"});

				addResourceSpecies(ptMain);

				newResourceSpecies = true;
			}
		}

		return newResourceSpecies;
	}
	else
	{
		throwLineInfoException("Error: The specified path \"" + resourceFolder.string() + "\" does not exist or it is not a directory");
	}
}


bool Landscape::readResourcePatchesFromJSONFiles(const fs::path& configPath)
{
	bool newResourcePatches = false;

	view->updateLog("Reading all resource patches from JSON files ... \n");
	
	vector<PatchPriorityQueue> resourcePatchesToAplly(getExistingResourceSpecies().size());

	fs::path resourcePatchesFolder = configPath / RESOURCE_FOLDER_NAME / RESOURCE_PATCH_FOLDER;

	if(fs::exists(resourcePatchesFolder) && fs::is_directory(resourcePatchesFolder))
	{
		JsonValidator resourcePatchesValidator(RESOURCE_PATCH_SCHEMA);

		for (const auto& entry : fs::directory_iterator(resourcePatchesFolder))
		{
			if(entry.path().extension() == ".json")
			{
				json ptMain = readConfigFile(entry.path(), resourcePatchesValidator);

				bool foundResourceSpecies = false;
				size_t resourceSpeciesId = 0u;
				for(const ResourceSpecies* const &resourceSpecies : getExistingResourceSpecies())
				{
					if(resourceSpecies->getScientificName() == ptMain["patch"]["resourceSpecies"].get<string>())
					{
						resourceSpeciesId = resourceSpecies->getResourceSpeciesId();
						foundResourceSpecies = true;
					}
				}

				if(!foundResourceSpecies)
				{
					throwLineInfoException("The resource species associated with the patch has not been found.");
				}

				resourcePatchesToAplly[resourceSpeciesId].push(make_unique<Patch>(
					new ResourceSource(
						ptMain["patch"]["source"], getMutableExistingResourceSpecies().at(resourceSpeciesId), 
						getMap()->getMinHyperVolume(), getScaleMass(), getTimeStepsPerDay()
					),
					ptMain["patch"], entry.path().string()
				));
			
				newResourcePatches = true;
			}
		}
	}
	else
	{
		throwLineInfoException("Error: The specified path \"" + resourcePatchesFolder.string() + "\" does not exist or it is not a directory");
	}


	for(const ResourceSpecies* const &resourceSpecies : getExistingResourceSpecies())
	{
		view->updateLog({" - Resource scientific name: ", resourceSpecies->getScientificName(), "\n\n"});

		while(!resourcePatchesToAplly[resourceSpecies->getResourceSpeciesId()].empty())
		{
			const bool isApplied = applyPatch(*resourcePatchesToAplly[resourceSpecies->getResourceSpeciesId()].top());

			if(isApplied)
			{
				addAppliedResource(static_cast<ResourceSource* const>(resourcePatchesToAplly[resourceSpecies->getResourceSpeciesId()].top()->moveSource()), resourcePatchesToAplly[resourceSpecies->getResourceSpeciesId()].top()->getPriority());
			}

			resourcePatchesToAplly[resourceSpecies->getResourceSpeciesId()].pop();
		}
	}

	return newResourcePatches;
}


void Landscape::addResourceSpecies(const json &resourceSpeciesInfo)
{
	ResourceSpecies* newResourceSpecies = new ResourceSpecies(getExistingSpecies().size(), getExistingResourceSpecies().size(), resourceSpeciesInfo["resource"]);

	for(const auto& resourceSpecies : getExistingResourceSpecies())
	{
		if(resourceSpecies->getScientificName() == newResourceSpecies->getScientificName())
		{
			throwLineInfoException("Error, the " + newResourceSpecies->getScientificName() + " resource species was already added to this landscape");
		}
	}

	existingResourceSpecies.push_back(newResourceSpecies);

	addSpecies(existingResourceSpecies.back());


	for(const auto& animalSpecies : getMutableExistingAnimalSpecies())
	{
		animalSpecies->addResourceSpecies();
	}


	ResourceSource* resourceBaseSource = new ResourceSource(getMutableExistingResourceSpecies().back());

	landscapeMap->addResourceSpecies(this, landscapeResources, *getMutableExistingResourceSpecies().back(), resourceBaseSource, getTimeStepsPerDay());


	appliedResource.emplace_back();

	addAppliedResource(resourceBaseSource, 0u);
}


void Landscape::addAnimalSpecies(const json &animalSpeciesInfo, CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, unsigned int>>& initialPopulation)
{
	AnimalSpecies* newAnimalSpecies = new AnimalSpecies(getExistingSpecies().size(), getExistingAnimalSpecies().size(), animalSpeciesInfo["animal"], getTimeStepsPerDay(), getPdfThreshold(), existingSpecies.size());

	pair<bool, const AnimalSpecies*> animalSpeciesAlreadyAdded = make_pair<>(false, nullptr);

	for(const auto& animalSpecies : getExistingAnimalSpecies())
	{
		if(animalSpecies->getScientificName() == newAnimalSpecies->getScientificName())
		{
			animalSpeciesAlreadyAdded = make_pair<>(true, animalSpecies);
		}
	}

	if(animalSpeciesAlreadyAdded.first)
	{
		initialPopulation[animalSpeciesAlreadyAdded.second->getAnimalSpeciesId()] = newAnimalSpecies->getIndividualsPerInstar();

		delete newAnimalSpecies;
	}
	else
	{
		existingAnimalSpecies.push_back(newAnimalSpecies);

		addSpecies(existingAnimalSpecies.back());

		initialPopulation.push_back(existingAnimalSpecies.back()->getIndividualsPerInstar());
	}


	for(const auto& animalSpecies : getMutableExistingAnimalSpecies())
	{
		animalSpecies->addAnimalSpecies();
	}


	getMutableExistingAnimalSpecies().back()->calculateCellDepthPerInstar(landscapeMap);


	landscapeMap->addAnimalSpecies(*getExistingAnimalSpecies().back(), getTimeStepsPerDay());
}

void Landscape::addSpecies(Species* newSpecies)
{
	view->updateLog({"numberOfInstars: ", to_string(newSpecies->getGrowthBuildingBlock().getNumberOfInstars()), "\n"});

	existingSpecies.push_back(newSpecies);
}

ResourceSpecies* Landscape::getResourceSpecies(string name)
{
	for(auto &resourceSpecies : existingResourceSpecies)
	{
		if(resourceSpecies->getScientificName() == name)
		{
			return resourceSpecies;
		}
	}

	throwLineInfoException("\"" + name + "\" doesn't exist. Please check the resource species name or contact developers");
}


AnimalSpecies* Landscape::getAnimalSpecies(const string& name)
{
	for(auto &animalSpecies : getMutableExistingAnimalSpecies())
	{
		if(animalSpecies->getScientificName() == name)
		{
			return animalSpecies;
		}
	}

	throwLineInfoException("\"" + name + "\" doesn't exist. Please check the animal species name or contact developers");
}


void Landscape::printAnimalsAlongCells(const TimeStep numberOfTimeSteps, const int simulationPoint) const
{
	if((saveAnimalsEachDayStart && simulationPoint == 0) ||
		(saveAnimalsEachDayEnd && simulationPoint == 1)
	)
	{
		if(((numberOfTimeSteps % recordEach) == 0) || (numberOfTimeSteps == TimeStep(0)))
		{
			ostringstream header;

			header << "id\tspecies\tgender";

			for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
			{
				header << "\t" << magic_enum::enum_names<Axis>()[axis];
			}
			
			header << "\tstate\tinstar\tage\tpheno_ini\tdateEgg\tage_first_rep\treproCounter\tfecundity\tdate_death\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\tencounters_pred\tglobal_pred_encs\tvor_ini\tsearch_ini\tspeed_ini\ttank_ini\tpheno_ini\tcurrentBodySize\tcurrentDryMass\tcurrentEnergyTank";
			
			header << "\t" << Trait::getHeader();

			header << "\n";

			ostringstream content;

			for(size_t i = 0; i < landscapeAnimals.size(); i++)
			{
				content << static_cast<string>(*static_cast<AnimalNonStatistical*>(landscapeAnimals[i])) << "\n";
			}



			string pathBySimulationPoint;
			if (simulationPoint == 0)
			{
				pathBySimulationPoint = "animals_each_day_start";
			}
			else if (simulationPoint == 1)
			{
				pathBySimulationPoint = "animals_each_day_end";
			}
			
			std::ofstream file;

			createOutputFile(file, resultFolder / fs::path(pathBySimulationPoint), "animals_day_", "txt", numberOfTimeSteps, recordEach);
			if (!file.is_open())
			{
				throwLineInfoException("Error opening the file");
			}

			file << header.str() + content.str();

			file.close();
		}
	}
}

void Landscape::printCellAlongCells(const TimeStep numberOfTimeSteps) const
{
	if(saveCellsEachDay)
	{
		if(((numberOfTimeSteps % recordEach) == 0) || (numberOfTimeSteps == TimeStep(0)))
		{
			ostringstream header;

			header << landscapeMap->getMapPositionHeader();

			for(const auto &resourceSpecies : existingResourceSpecies)
			{
				header << "\t" << resourceSpecies->getScientificName();
				header << "\t" << resourceSpecies->getScientificName() << "_available_dry_mass";
			}

			for(const AnimalSpecies* const& animalSpecies : getExistingAnimalSpecies())
			{
				header << "\t" << animalSpecies->getScientificName();
			}

			header << "\n";


			ostringstream content;

			vector<vector<vector<size_t>>> animalsCellsInfo(
				landscapeMap->getNumberOfCellsPerAxis(),
				vector<vector<size_t>>(
					landscapeMap->getNumberOfCellsPerAxis(),
					vector<size_t>(getExistingAnimalSpecies().size(), 0u)
				)
			);


			for(size_t i = 0; i < landscapeAnimals.size(); i++)
			{
				size_t x = static_cast<size_t>((getPositionAxisValue(landscapeAnimals[i]->getPosition(),0u) / landscapeMap->getMinCellSize()).getValue());
				size_t y = static_cast<size_t>((getPositionAxisValue(landscapeAnimals[i]->getPosition(),1u) / landscapeMap->getMinCellSize()).getValue());

				animalsCellsInfo[x][y][landscapeAnimals[i]->getSpecies()->getAnimalSpeciesId()]++;
			}


			for(unsigned int x = 0u; x < landscapeMap->getNumberOfCellsPerAxis(); x++)
			{
				for(unsigned int y = 0u; y < landscapeMap->getNumberOfCellsPerAxis(); y++)
				{
					content << x << "\t" << y;

					for(const CellResource* const resource : landscapeResources[x][y])
					{
						content << "\t" << resource->getGrowthBuildingBlock().getCurrentTotalWetMass() << "\t" << resource->calculateDryMassAvailable(true, nullptr, 0.0, nullptr).getValue();
					}

					for(const size_t &animalSpeciesValue : animalsCellsInfo[x][y])
					{
						content << "\t" << animalSpeciesValue;
					}

					content << "\n";
				}
			}



			std::ofstream file;

			createOutputFile(file, resultFolder / fs::path("cells_each_day"), "cells_day_", "txt", numberOfTimeSteps, recordEach);
			if (!file.is_open())
			{
				throwLineInfoException("Error opening the file");
			}

			file << header.str() + content.str();

			file.close();
		}
	}
}

void Landscape::obtainLandscapeResourceBiomassAndAnimalsPopulation(vector<WetMass> &landscapeResourceBiomass, CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<LifeStage, unsigned int>> &landscapeAnimalsPopulation) const
{
	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		landscapeAnimalsPopulation[landscapeAnimals[i]->getSpecies()->getAnimalSpeciesId()][landscapeAnimals[i]->getLifeStage()]++;
	}


	landscapeMap->obtainResourceBiomass(landscapeResourceBiomass);
}

void Landscape::printExtendedDailySummary(ostream& os, const TimeStep numberOfTimeSteps) const
{
	vector<WetMass> landscapeResourceBiomass(getExistingResourceSpecies().size());
	CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<LifeStage, unsigned int>> landscapeAnimalsPopulation(getExistingAnimalSpecies().size(), CustomIndexedVector<LifeStage, unsigned int>(EnumClass<LifeStage>::size(), 0));

	obtainLandscapeResourceBiomassAndAnimalsPopulation(landscapeResourceBiomass, landscapeAnimalsPopulation);

	ostringstream content;

	content << numberOfTimeSteps;

	for(const WetMass &resourceBiomass : landscapeResourceBiomass)
	{
		content << "\t" << resourceBiomass.getValue();
	}

	for(const auto &animalSpeciesPopulation : landscapeAnimalsPopulation)
	{
		for(const auto &lifeStagePopulation : animalSpeciesPopulation)
		{
			content << "\t" << lifeStagePopulation;
		}
	}
	
	content << "\n";


	os << content.str();
	os.flush();
}


const PreciseDouble& Landscape::getTimeStepsPerDay() const
{
	return timeStepsPerDay;
}

void Landscape::saveAnimalSpeciesSnapshot(fs::path filenameRoot, string filename, const TimeStep numberOfTimeSteps, AnimalSpecies* species)
{
	if(species->getTotalInitialPopulation() > 0)
	{
		string scientificName = species->getScientificName();
		std::replace(scientificName.begin(), scientificName.end(), ' ', '_');

		std::ofstream file;
		string fullPath = createOutputFile(file, filenameRoot, filename + "_" + scientificName + "_day_", "dat", numberOfTimeSteps, recordEach, ios::out | ios::binary);

		view->updateLog({"Saving Animal as ", fullPath, "... "});


		unsigned int value = 0;

		for(size_t i = 0; i < landscapeAnimals.size(); i++)
		{
			if(landscapeAnimals[i]->getLifeStage() == LifeStage::ACTIVE)
			{
				value++;
			}
		}

		file.write((char *) &value, sizeof(unsigned int));


		view->updateLog("DONE\n");

		file.close();
	}
}

bool Landscape::isCheckpointsEnabled() const
{
	return checkpointsEnabled;
}

unsigned int Landscape::getCheckpointsRecordEach() const
{
	return checkpointsRecordEach;
}

bool Landscape::isBinaryCheckpointEnabled() const
{
	return binaryCheckpointEnabled;
}

void Landscape::saveResourceSpeciesSnapshot(fs::path filenameRoot, string filename, const TimeStep numberOfTimeSteps, ResourceSpecies* species)
{
	string scientificName = species->getScientificName();
	std::replace(scientificName.begin(), scientificName.end(), ' ', '_');

	std::ofstream file;
	string fullPath = createOutputFile(file, filenameRoot, filename + "_" + scientificName + "_day_", "dat", numberOfTimeSteps, recordEach, ios::out | ios::binary);

	view->updateLog({"Saving Resource as ", fullPath, "... "});

	landscapeMap->saveResourceSpeciesSnapshot(file, species);

	view->updateLog("DONE\n");

	file.close();
}


void Landscape::saveWaterSnapshot(fs::path filenameRoot, string filename, const TimeStep numberOfTimeSteps)
{
	std::ofstream file;
	string fullPath = createOutputFile(file, filenameRoot, filename + "_day_", "dat", numberOfTimeSteps, recordEach, ios::out | ios::binary);

	view->updateLog({"Saving Water volume as ", fullPath, "... "});

	landscapeMap->saveWaterSnapshot(file);

	file.close();
	view->updateLog("DONE\n");
}


void Landscape::printPredationEventsOnOtherSpeciesMatrix()
{
	ostringstream content;

	content << "prey\\predator";

	for(const auto& predatorAnimalSpecies : getExistingAnimalSpecies())
	{
		content << "\t" << predatorAnimalSpecies->getScientificName();
	}

	content << "\n";

	for(const auto& preySpecies : getExistingSpecies())
	{
		content << preySpecies->getScientificName();

		for(const auto& predatorAnimalSpecies : getExistingAnimalSpecies())
		{
			content << "\t" << predatorAnimalSpecies->getPredationEventsOnOtherSpecies(preySpecies->getId());
		}

		content << "\n";
	}



	std::ofstream predationEventsOnOtherSpeciesFile;
	createOutputFile(predationEventsOnOtherSpeciesFile, resultFolder / fs::path("Matrices"), "predationOnSpecies", "txt");
	if (!predationEventsOnOtherSpeciesFile.is_open())
	{
		throwLineInfoException("Error opening the file");
	}

	predationEventsOnOtherSpeciesFile << content.str();

	predationEventsOnOtherSpeciesFile.close();
}

void Landscape::updateMap(const TimeStep numberOfTimeSteps)
{
	if(numberOfTimeSteps > TimeStep(0))
	{
		updateAllSources();
		landscapeMap->update(numberOfTimeSteps);
	}


	// resetAnimalControlVariables;
	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->resetControlVariables(numberOfTimeSteps, getTimeStepsPerDay());
	}



	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		if(landscapeAnimals[i]->getLifeStage() != LifeStage::UNBORN)
		{
			static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->increaseAge(this, numberOfTimeSteps, getTimeStepsPerDay());	
		}
	}

	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		if(landscapeAnimals[i]->getLifeStage() == LifeStage::UNBORN)
		{
			static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->isReadyToBeBorn(this, getTimeStepsPerDay());
		}
	}

	


	// activateAndResumeAnimals;
	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		if(landscapeAnimals[i]->getLifeStage() == LifeStage::DIAPAUSE)
		{
			static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(this);
		}
	}

	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		if(landscapeAnimals[i]->getLifeStage() == LifeStage::PUPA)
		{
			static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->isReadyToResumeFromPupaOrDecreasePupaTimer(this);
		}
	}


	// tuneAnimals
	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		if(landscapeAnimals[i]->getLifeStage() != LifeStage::UNBORN)
		{
			static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->tune(this, saveMassInfo, numberOfTimeSteps, getTimeStepsPerDay());
		}
	}



	updateAnimalSpeciesGlobalMaximum();



    growAnimals(numberOfTimeSteps);
}


void Landscape::updateAnimalSpeciesGlobalMaximum()
{
	for(auto &predatorAnimalSpecies : getMutableExistingAnimalSpecies())
	{
		for(const Instar &predatorInstar : predatorAnimalSpecies->getGrowthBuildingBlock().getInstarsRange())
		{
			for(const auto& [preySpeciesId, preyInstars] : predatorAnimalSpecies->getInstarEdibleAnimalSpecies(predatorInstar))
			{
				for(const Instar& preyInstar : preyInstars)
				{
					static_cast<AnimalSpecies*>(getMutableExistingSpecies().at(preySpeciesId))->getMutableDecisionsBuildingBlock()->updateMaximumPredatorInteractionArea(preyInstar, predatorAnimalSpecies->getMaximumInteractionArea(predatorInstar));

					predatorAnimalSpecies->getMutableDecisionsBuildingBlock()->updateMaximumPreyVoracity(predatorInstar, static_cast<AnimalSpecies*>(getMutableExistingSpecies().at(preySpeciesId))->getDecisionsBuildingBlock()->getMaximumVoracity(preyInstar));
				}
			}
		}
	}
}


void Landscape::updateAllSources()
{
	for (const auto& [priority, source] : appliedMoisture)
	{
		source->update();
	}

	for(const auto& resources : appliedResource)
	{
		for (const auto& [priority, source] : resources)
		{
			source->update();
		}
	}

	vector<size_t> xAxis;
	Random::createIndicesVector(xAxis, landscapeResources.size());

	for(const size_t xPos : xAxis)
	{
		vector<size_t> yAxis;
		Random::createIndicesVector(yAxis, landscapeResources[xPos].size());

		for(const size_t yPos : yAxis)
		{
			if(getCompetitionAmongResourceSpecies())
			{
				vector<size_t> resourceSpecies;
				Random::createIndicesVector(resourceSpecies, landscapeResources[xPos][yPos].size());

				for(const size_t id : resourceSpecies)
				{
					landscapeResources[xPos][yPos][id]->growth(this);
				}
			}
			else
			{
				for(CellResource* resource : landscapeResources[xPos][yPos])
				{
					resource->growth(this);
				}
			}
			
		}
	}
	

}

const PreciseDouble& Landscape::getPdfThreshold() const
{
    return pdfThreshold;
}

const Day Landscape::getRunDays() const
{
	return runDays;
}

void Landscape::evolveLandscape()
{
	auto start = std::chrono::high_resolution_clock::now();

	
	saveWaterSnapshot(resultFolder / fs::path("Snapshots"), "Water_initial", TimeStep(0));

	for(ResourceSpecies*& resourceSpecies : existingResourceSpecies)
	{
		saveResourceSpeciesSnapshot(resultFolder / fs::path("Snapshots"), "Resource_initial", TimeStep(0), resourceSpecies);
	}


	std::ofstream timeSpentFile;

	createOutputFile(timeSpentFile, resultFolder, "time_spent", "txt");
	if (!timeSpentFile.is_open())
	{
		throwLineInfoException("Error opening the file");
	}

	TimeStep numberOfTimeSteps = TimeStep(0);

	const TimeStep totalNumberOfTimeSteps(getRunDays(), getTimeStepsPerDay());

	while(numberOfTimeSteps < totalNumberOfTimeSteps)
	{
		view->updateLog({"Running on timeStep ", to_string(numberOfTimeSteps.getValue()), " out of ", to_string(totalNumberOfTimeSteps.getValue()), "\n"});

//#####################################################################
//##########################  UPDATING MAP   ##########################
//#####################################################################

		view->updateLog(" - Updating map ... \n");
		
		auto t0 = chrono::high_resolution_clock::now();
		updateMap(numberOfTimeSteps);
		auto t1 = chrono::high_resolution_clock::now();

		view->updateLog({"Time: ", to_string(chrono::duration<double>(t1-t0).count()), " secs.\n"});
		timeSpentFile << chrono::duration<double>(t1-t0).count() << "\t";
		
		view->updateLog("DONE\n");

//#####################################################################
//#################  PRINTING ANIMALS ALONG CELLS   ###################
//#####################################################################

		view->updateLog(" - Printing animals along cells ... \n");

		t0 = chrono::high_resolution_clock::now();
		printAnimalsAlongCells(numberOfTimeSteps, 0);
		t1 = chrono::high_resolution_clock::now();

		view->updateLog({"Time: ", to_string(chrono::duration<double>(t1-t0).count()), " secs.\n"});
		timeSpentFile << chrono::duration<double>(t1-t0).count() << "\t";

		view->updateLog("DONE\n");

//#####################################################################
//#########################  MOVING ANIMALS   #########################
//#####################################################################

		view->updateLog(" - Moving animals ... \n");

		t0 = chrono::high_resolution_clock::now();
		moveAnimals(numberOfTimeSteps);
		t1 = chrono::high_resolution_clock::now();
		
		view->updateLog({"Time: ", to_string(chrono::duration<double>(t1-t0).count()), " secs.\n"});
		timeSpentFile << chrono::duration<double>(t1-t0).count() << "\t";

		view->updateLog("DONE\n");

//#####################################################################
//##########  BACKGROUND, ASSIMILATING FOOD & REPRODUCING   ###########
//#####################################################################

		view->updateLog(" - Background, assimilating food and reproducing ... \n");
		
		t0 = chrono::high_resolution_clock::now();
		performAnimalsActions(numberOfTimeSteps);
		t1 = chrono::high_resolution_clock::now();

		view->updateLog({"Time: ", to_string(chrono::duration<double>(t1-t0).count()), " secs.\n"});
		timeSpentFile << chrono::duration<double>(t1-t0).count() << "\t";

		view->updateLog("DONE\n");

//#####################################################################
//#################  PRINTING ANIMALS ALONG CELLS   ###################
//#####################################################################

		view->updateLog(" - Printing animals along cells ... \n");

		t0 = chrono::high_resolution_clock::now();
		printAnimalsAlongCells(numberOfTimeSteps, 1);
		t1 = chrono::high_resolution_clock::now();

		view->updateLog({"Time: ", to_string(chrono::duration<double>(t1-t0).count()), " secs.\n"});
		timeSpentFile << chrono::duration<double>(t1-t0).count() << "\t";

		view->updateLog("DONE\n");

//#####################################################################
//##################  PRINTING EXTENDED SUMMARY   #####################
//#####################################################################

		if(saveExtendedDailySummary)
		{
			view->updateLog(" - Printing summary file ... \n");

			t0 = chrono::high_resolution_clock::now();
			printExtendedDailySummary(extendedDailySummaryFile, numberOfTimeSteps);
			t1 = chrono::high_resolution_clock::now();

			view->updateLog({"Time: ", to_string(chrono::duration<double>(t1-t0).count()), " secs.\n"});
			timeSpentFile << chrono::duration<double>(t1-t0).count() << "\t";

			view->updateLog("DONE\n");
		}

//#####################################################################
//####################  PURGING DEAD ANIMALS   ########################
//#####################################################################

		view->updateLog(" - Purging dead animals ... \n");

		t0 = chrono::high_resolution_clock::now();
		purgeDeadAnimals();
		t1 = chrono::high_resolution_clock::now();

		view->updateLog({"Time: ", to_string(chrono::duration<double>(t1-t0).count()), " secs.\n"});
		timeSpentFile << chrono::duration<double>(t1-t0).count() << "\t" << endl;

		view->updateLog("DONE\n");

		printCellAlongCells(numberOfTimeSteps);

		
		if(savePredationEventsOnOtherSpecies)
		{
			printPredationEventsOnOtherSpeciesMatrix();
		}


		if (saveIntermidiateVolumes && (((numberOfTimeSteps + TimeStep(1)) % saveIntermidiateVolumesPeriodicity) == 0))
		{
			saveWaterSnapshot(resultFolder / fs::path("Snapshots"), "Water", numberOfTimeSteps);

			for(auto &resourceSpecies : existingResourceSpecies)
			{
				saveResourceSpeciesSnapshot(resultFolder / fs::path("Snapshots"), "Resource", numberOfTimeSteps, resourceSpecies);
			}

			for(AnimalSpecies*& animalSpecies : getMutableExistingAnimalSpecies())
			{
				saveAnimalSpeciesSnapshot(resultFolder / fs::path("Snapshots"), "Animal", numberOfTimeSteps, animalSpecies);
			}
		}

		if(checkBreakConditions(numberOfTimeSteps))
		{
			break;
		}

		numberOfTimeSteps = numberOfTimeSteps + TimeStep(1);


		if(isCheckpointsEnabled() && (numberOfTimeSteps % getCheckpointsRecordEach())==0)
		{
			saveCheckpoint(numberOfTimeSteps);
		}
	}


	if(exitAtFirstExtinction || stabilityEnabled)
	{
		saveBreakConditionsInfo(numberOfTimeSteps);
	}


	if(isCheckpointsEnabled())
	{
		saveCheckpoint(numberOfTimeSteps);
	}


	saveWaterSnapshot(resultFolder / fs::path("Snapshots"), "Water_final", numberOfTimeSteps);

	for(auto &resourceSpecies : existingResourceSpecies)
	{
		saveResourceSpeciesSnapshot(resultFolder / fs::path("Snapshots"), "Resource_final", numberOfTimeSteps, resourceSpecies);
	}

	for(AnimalSpecies*& animalSpecies : getMutableExistingAnimalSpecies())
	{
		saveAnimalSpeciesSnapshot(resultFolder / fs::path("Snapshots"), "Animal_final", numberOfTimeSteps, animalSpecies);
	}

	for(auto& animalSpeciesFile : animalConstitutiveTraitsFile)
	{
		animalSpeciesFile->close();
	}

	if(getSaveEdibilitiesFile())
	{
		edibilitiesFile.close();
	}

	timeSpentFile.close();

	if(saveDailySummary)
	{
		dailySummaryFile.close();
	}
	
	if(saveExtendedDailySummary)
	{
		extendedDailySummaryFile.close();
	}

	if(saveMovements)
	{
		movementsFile.close();
	}


	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed = end - start;

	ofstream timeFile((resultFolder / fs::path("executionTime.txt")).string());
	timeFile << elapsed.count() << " segs" << endl;
	timeFile.close();
}

bool Landscape::checkBreakConditions(const TimeStep& numberOfTimeSteps)
{
	bool breakCondition = false;

	if(exitAtFirstExtinction)
	{
		breakCondition = breakCondition || isExtinguished();
	}

	if(stabilityEnabled)
	{
		breakCondition = breakCondition || isSimulationStabilised(numberOfTimeSteps);
	}

	return breakCondition;
}

void Landscape::saveBreakConditionsInfo(const TimeStep& numberOfTimeSteps)
{
	std::ofstream breakConditionsInfo;

	createOutputFile(breakConditionsInfo, resultFolder, "breakConditionsInfo", "txt", std::ofstream::out | std::ofstream::trunc);
	
	breakConditionsInfo << "coefficientOfVariation\ttotalTime\n";

	PreciseDouble coefficientOfVariation;

	if(stabilityEnabled)
	{
		if(numberOfTimeSteps < (stabilityCalculationInterval-TimeStep(1)))
		{
			coefficientOfVariation = DBL_MAX;
		}
		else
		{
			coefficientOfVariation = 0.0;

			for(AnimalSpecies* animalSpecies : getMutableExistingAnimalSpecies())
			{
				coefficientOfVariation = fmax(coefficientOfVariation, animalSpecies->getCoefficientOfVariation());
			}
		}
	}
	else
	{
		coefficientOfVariation = DBL_MAX;
	}

	breakConditionsInfo << coefficientOfVariation << "\t" << numberOfTimeSteps << "\n";

	breakConditionsInfo.close();
}

const fs::path& Landscape::getResultFolder() const
{
	return resultFolder;
}

void Landscape::saveCheckpoint(const TimeStep& numberOfTimeSteps)
{
	const TimeStep totalNumberOfTimeSteps(getRunDays(), getTimeStepsPerDay());

	string timeStepStr = string(MAX_NUM_DIGITS_DAY - to_string(numberOfTimeSteps.getValue()).length(), '0') + to_string(numberOfTimeSteps.getValue());
	string totalTimeStepsStr = string(MAX_NUM_DIGITS_DAY - to_string(totalNumberOfTimeSteps.getValue()).length(), '0') + to_string(totalNumberOfTimeSteps.getValue());
	string checkpointFilename = "checkpoint_" + timeStepStr + "_" + totalTimeStepsStr;

	if(isBinaryCheckpointEnabled())
	{
		checkpointFilename += ".bin";
		std::ofstream ofs((resultFolder / fs::path("checkpoints") / checkpointFilename).string());
		boost::archive::binary_oarchive oa(ofs);
		oa << this;
		ofs.close();
	}
	else
	{
		checkpointFilename += ".txt";
		std::ofstream ofs((resultFolder / fs::path("checkpoints") / checkpointFilename).string());
		boost::archive::text_oarchive oa(ofs);
		oa << this;
		ofs.close();
	}
}

void Landscape::registerAnimal(AnimalNonStatistical* animal)
{
	landscapeAnimals.push_back(animal);
}

void Landscape::unregisterAnimal(AnimalNonStatistical* animal)
{
	auto it = find_if(landscapeAnimals.begin(), landscapeAnimals.end(), [animal](Animal* elem) {
        return animal->getId() == elem->getId(); // Compara cada elemento con el valor
    });

	landscapeAnimals.erase(it);
}

DryMass Landscape::getBasalMetabolicDryMassLossPerTimeStep(WetMass wetMass, const PreciseDouble& actE_metValue, const bool actE_metThermallyDependent, const PreciseDouble& met_rateValue, const bool met_rateThermallyDependent, const Temperature& tempFromLab, const Temperature& terrainCellTemperature, const PreciseDouble& conversionToWetMass) const
{
	return getMetabolicDryMassLossPerTimeStep(wetMass, 0.0, actE_metValue, actE_metThermallyDependent, met_rateValue, met_rateThermallyDependent, 0.0, tempFromLab, terrainCellTemperature, conversionToWetMass);
}

DryMass Landscape::getMetabolicDryMassLossPerTimeStep(WetMass wetMass, const PreciseDouble& proportionOfTimeTheAnimalWasMoving, const PreciseDouble& actE_metValue, const bool actE_metThermallyDependent, const PreciseDouble& met_rateValue, const bool met_rateThermallyDependent, const PreciseDouble& search_areaValue, const Temperature& tempFromLab, const Temperature& terrainCellTemperature, const PreciseDouble& conversionToWetMass) const
{
	DryMass metabolicDryMassLossPerDay = calculateMetabolicDryMassLossPerDay(wetMass, proportionOfTimeTheAnimalWasMoving, actE_metValue, actE_metThermallyDependent, met_rateValue, met_rateThermallyDependent, search_areaValue, tempFromLab, terrainCellTemperature, conversionToWetMass);

	return DryMass(metabolicDryMassLossPerDay.getValue() * getTimeStepsPerDay());
}

bool Landscape::isGrowthAndReproTest() const 
{ 
	return growthAndReproTest; 
}

void Landscape::moveAnimals(const TimeStep& numberOfTimeSteps)
{
	ostringstream predationProbabilitiesContent, activityContent, edibilitiesContent, movementsContent;

	vector<size_t> randomIndexLandscapeAnimals;
	Random::createIndicesVector(randomIndexLandscapeAnimals, landscapeAnimals.size());

	ProgressBar progressBar(view, randomIndexLandscapeAnimals.size());

	for(const size_t pos : randomIndexLandscapeAnimals)
	{
		if(landscapeAnimals[pos]->getLifeStage() == LifeStage::ACTIVE ||
	       landscapeAnimals[pos]->getLifeStage() == LifeStage::REPRODUCING)
		{
			auto t0 = chrono::high_resolution_clock::now();
					
			static_cast<AnimalNonStatistical*>(landscapeAnimals[pos])->moveAnimal(view, this, numberOfTimeSteps, getTimeStepsPerDay(), saveAnimalsEachDayPredationProbabilities, predationProbabilitiesContent, getSaveEdibilitiesFile(), edibilitiesContent, saveActivity, activityContent, saveMovements, movementsContent, getCompetitionAmongResourceSpecies());
		
			auto t1 = chrono::high_resolution_clock::now();

			if(chrono::duration<double>(t1-t0).count() > exitTimeThreshold)
			{
				throwLineInfoException("too many animals for too little food!!!");
			}
		}

		progressBar.update();
	}


	if(getSaveEdibilitiesFile())
	{
		edibilitiesFile << edibilitiesContent.str();
		edibilitiesFile.flush();
	}

	if(saveMovements)
	{
		movementsFile << movementsContent.str();
		movementsFile.flush();
	}



	if(saveAnimalsEachDayPredationProbabilities)
	{
		std::ofstream predationProbabilitiesFile;
		
		createOutputFile(predationProbabilitiesFile, resultFolder / fs::path("animals_each_day_predationProbabilities"), "animals_predationProbabilities_day_", "txt", numberOfTimeSteps, recordEach);
		if(!predationProbabilitiesFile.is_open())
		{
			throwLineInfoException("Error opening the file");
		}

		ostringstream predationProbabilitiesHeader;

		predationProbabilitiesHeader << "randomProbability\tprobabilityToCompare\tretaliation\texposedAttack\tidHunter\tidHunted\tspeciesHunter\tspeciesHunted\thuntedIsPredator\tmassHunter\tmassHunted\tsuccessfulKill\n";

		predationProbabilitiesFile << predationProbabilitiesHeader.str() + predationProbabilitiesContent.str();

		predationProbabilitiesFile.close();
	}
		

	if(saveActivity)
	{
		std::ofstream activityFile;

		createOutputFile(activityFile, resultFolder / fs::path("animals_each_day_activity"), "animals_activity_day_", "txt", numberOfTimeSteps, recordEach);
		if (!activityFile.is_open())
		{
			throwLineInfoException("Error opening the file");
		}

		ostringstream activityHeader;

		activityHeader << "id\tspecies\tactivityTipe\tinitialDay\tfinalDay\tactivityDuration\n"; 

		activityFile << activityHeader.str() + activityContent.str();
		
		activityFile.close();
	}
}

void Landscape::performAnimalsActions(const TimeStep numberOfTimeSteps)
{
    printAnimalsVoracities(numberOfTimeSteps);
    dieFromBackground(numberOfTimeSteps);
    transferAssimilatedFoodToEnergyTank(numberOfTimeSteps);
    metabolizeAnimals(numberOfTimeSteps);
	breedAnimals(numberOfTimeSteps);



	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		if(landscapeAnimals[i]->getLifeStage() == LifeStage::ACTIVE)
		{
			static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->checkEnergyTank(this, numberOfTimeSteps, getTimeStepsPerDay());
		}
	}
}

void Landscape::printAnimalsVoracities(const TimeStep& numberOfTimeSteps)
{
	if(saveAnimalsEachDayVoracities)
	{
		ostringstream voracitiesHeader;

		voracitiesHeader << "id\tspecies\tstate\tcurrentAge\tinstar\tmature\tbody_size\tenergy_tank\tdryMass\tcurrentWetMass\ttankAtGrowth\tnextDinoMass\tmaxVoracityTimeStep\tmin_mass_for_death\tvoracity_ini\tpreT_search\tpreT_speed\tafter_encounters_voracity\tafter_encounters_search\tfinal_speed\texpectedDryMassFromMaxVor\tfood_mass\tdryMassAfterAssim\ttotalMetabolicDryMassLossAfterAssim\tmaxSearchArea\teatenToday\tsteps\tstepsAttempted\tafter_encounters_search\tsated\tpercentMoving\tvoracity_body_mass_ratio\tgender\tmated\teggDryMass\tK\tfactorEggMass\tdeath_date\tageOfFirstMaturation\treproCounter\n";


		ostringstream voracitiesContent;

		for(size_t i = 0; i < landscapeAnimals.size(); i++)
		{
			if(landscapeAnimals[i]->getLifeStage() == LifeStage::ACTIVE)
			{
				static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->printVoracities(this, voracitiesContent, getTimeStepsPerDay());
			}
		}



		std::ofstream voracitiesFile;
		createOutputFile(voracitiesFile, resultFolder / fs::path("animals_each_day_voracities"), "animals_voracities_day_", "txt", numberOfTimeSteps, recordEach);
		if (!voracitiesFile.is_open())
		{
			throwLineInfoException("Error opening the file");
		}

		voracitiesFile << voracitiesHeader.str() + voracitiesContent.str();

		voracitiesFile.close();
	}
}

void Landscape::dieFromBackground(const TimeStep& numberOfTimeSteps)
{
	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		if(landscapeAnimals[i]->getLifeStage() == LifeStage::ACTIVE)
		{
			static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->dieFromBackground(this, numberOfTimeSteps, getTimeStepsPerDay(), isGrowthAndReproTest());
		}
	}
}

void Landscape::transferAssimilatedFoodToEnergyTank(const TimeStep& numberOfTimeSteps)
{
	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		if(landscapeAnimals[i]->getLifeStage() == LifeStage::ACTIVE)
		{
			static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->transferAssimilatedFoodToEnergyTank(numberOfTimeSteps);
		}
	}
}

void Landscape::metabolizeAnimals(const TimeStep numberOfTimeSteps)
{
	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		if(landscapeAnimals[i]->getLifeStage() == LifeStage::ACTIVE)
		{
			static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->metabolize(view, this, numberOfTimeSteps);
		}
	}
}

void Landscape::growAnimals(const TimeStep& numberOfTimeSteps)
{
	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		if(landscapeAnimals[i]->getLifeStage() == LifeStage::ACTIVE)
		{
			static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->grow(this, numberOfTimeSteps, getTimeStepsPerDay());
		}
	}
}

void Landscape::breedAnimals(const TimeStep& numberOfTimeSteps)
{
	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		if(landscapeAnimals[i]->getLifeStage() == LifeStage::REPRODUCING)
		{
			if(static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->isInBreedingZone())
			{
				ostringstream animalConstitutiveTraitsContent;

				list<AnimalNonStatistical*> offspring;
				static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->breed(offspring, view, this, numberOfTimeSteps, saveGenetics, saveMassInfo, getTimeStepsPerDay());

				for(auto &newAnimal : offspring)
				{
					newAnimal->setPosition(landscapeAnimals[i]->getPosition());
					landscapeAnimals[i]->getMutableTerrainCell()->insertAnimal(this, newAnimal);

					if(getSaveAnimalConstitutiveTraits())
					{
						newAnimal->printTraits(animalConstitutiveTraitsContent);
					}
				}

				static_cast<AnimalNonStatistical*>(landscapeAnimals[i])->setInBreedingZone(false);

				if(getSaveAnimalConstitutiveTraits())
				{
					if(getSaveAnimalConstitutiveTraits())
					{
						*animalConstitutiveTraitsFile[landscapeAnimals[i]->getSpecies()->getAnimalSpeciesId()] << animalConstitutiveTraitsContent.str();
						animalConstitutiveTraitsFile[landscapeAnimals[i]->getSpecies()->getAnimalSpeciesId()]->flush();
					}
				}
			}
		}
	}
}

void Landscape::purgeDeadAnimals()
{
	auto it = landscapeAnimals.begin();

	while(it != landscapeAnimals.end())
	{
		if((*it)->getLifeStage() == LifeStage::STARVED || (*it)->getLifeStage() == LifeStage::PREDATED ||
			(*it)->getLifeStage() == LifeStage::BACKGROUND || (*it)->getLifeStage() == LifeStage::SENESCED ||
			(*it)->getLifeStage() == LifeStage::SHOCKED)
		{
			(*it)->getMutableTerrainCell()->eraseAnimal((*it));
			delete (*it);

			it = landscapeAnimals.erase(it);
		}
		else
		{
			it++;
		}
	}
}

bool Landscape::isExtinguished() const
{
	bool extinctionStatus = false; 

	vector<WetMass> landscapeResourceBiomass(getExistingResourceSpecies().size());
	CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<LifeStage, unsigned int>> landscapeAnimalsPopulation(getExistingAnimalSpecies().size(), CustomIndexedVector<LifeStage, unsigned int>(EnumClass<LifeStage>::size(), 0));

	obtainLandscapeResourceBiomassAndAnimalsPopulation(landscapeResourceBiomass, landscapeAnimalsPopulation);

	for(ResourceSpecies* const resourceSpecies : getExistingResourceSpecies())
	{
		if(landscapeResourceBiomass[resourceSpecies->getResourceSpeciesId()] == 0.0)
		{
			resourceSpecies->setExtinguished(true);
			extinctionStatus = true;
		}
	}

	for(AnimalSpecies* const animalSpecies : getExistingAnimalSpecies())
	{
		unsigned int population = 0;

		for(const auto &elem : landscapeAnimalsPopulation[animalSpecies->getAnimalSpeciesId()])
		{
			population += elem;
		}


		if(population == 0)
		{
			animalSpecies->setExtinguished(true);
			extinctionStatus = true;
		}
	}

	return extinctionStatus;
}

bool Landscape::isSimulationStabilised(const TimeStep& numberOfTimeSteps)
{
	for(AnimalSpecies* animalSpecies : getMutableExistingAnimalSpecies())
	{
		animalSpecies->updatePopulationHistory(stabilityCalculationInterval);
	}

	if(numberOfTimeSteps < (stabilityCalculationInterval-TimeStep(1)))
	{
		return false;
	}
	else
	{
		for(AnimalSpecies* animalSpecies : getMutableExistingAnimalSpecies())
		{
			animalSpecies->calculateCoefficientOfVariation();

			if(animalSpecies->getCoefficientOfVariation() > coefficientOfVariationThreshold)
			{
				return false;
			}
		}

		return true;
	}
}


void Landscape::initializeMap(const json &mapConfig)
{
	view->updateLog("Initializing terrain voxels ... ");

	MoistureSource* moistureBaseSource = new MoistureSource(mapConfig["moistureBasePatch"], getTimeStepsPerDay());

	landscapeMap = Map::createInstance(mapConfig, moistureBaseSource);

	addAppliedMoisture(moistureBaseSource, 0u);

	// Rellenar con obstáculos los bordes de relleno para llegar a la potencia de 2 de numero de celdas
	PreciseDouble maxSize = landscapeMap->getNumberOfCellsPerAxis() * landscapeMap->getMinCellSize();

	unsigned int numberOfCellsAxisX = mapConfig["landscapeWideParams"]["numberOfCellsAxisX"].get<unsigned int>();
	PreciseDouble sizeAxisX = numberOfCellsAxisX * landscapeMap->getMinCellSize();

	if(landscapeMap->getNumberOfCellsPerAxis() != numberOfCellsAxisX)
	{
		Patch edgeObstacles(
			new ObstacleSource(),
			1u, 
			new CubicPatch(
				vector<double>{sizeAxisX.getValue(), 0.0}, 
				vector<double>{maxSize.getValue(), maxSize.getValue()}
			)
		);

		landscapeMap->applyPatch(this, edgeObstacles);
	}

	unsigned int numberOfCellsAxisY = mapConfig["landscapeWideParams"]["numberOfCellsAxisY"].get<unsigned int>();
	PreciseDouble sizeAxisY = numberOfCellsAxisY * landscapeMap->getMinCellSize();

	if(landscapeMap->getNumberOfCellsPerAxis() != numberOfCellsAxisY)
	{
		Patch edgeObstacles(
			new ObstacleSource(),
			1u, 
			new CubicPatch(
				vector<double>{0.0, sizeAxisY.getValue()}, 
				vector<double>{sizeAxisX.getValue(), maxSize.getValue()}
			)
		);

		landscapeMap->applyPatch(this, edgeObstacles);
	}

	view->updateLog("DONE\n");
}

bool Landscape::isDinosaurs() const
{
    return false;
}

void Landscape::readObstaclePatchesFromJSONFiles(const fs::path& configPath)
{
	view->updateLog("Reading obstacle patches from JSON files ... \n");

	PatchPriorityQueue obstaclePatchesToAplly;

	fs::path obstacleFolder = configPath / OBSTACLE_FOLDER_NAME;

	if (fs::exists(obstacleFolder) && fs::is_directory(obstacleFolder))
	{
		JsonValidator obstaclePatchesValidator(OBSTACLE_PATCH_SCHEMA);

		for (const auto& entry : fs::directory_iterator(obstacleFolder))
		{
			if (entry.path().extension() == ".json")
			{
				json ptMain = readConfigFile(entry.path(), obstaclePatchesValidator);

				obstaclePatchesToAplly.push(make_unique<Patch>(new ObstacleSource(), ptMain["patch"], entry.path().string()));
			}
		}
	}
	else
	{
		throwLineInfoException("Error: The specified path \"" + obstacleFolder.string() + "\" does not exist or it is not a directory.");
	}

	while(!obstaclePatchesToAplly.empty())
	{
		applyPatch(*obstaclePatchesToAplly.top());
		obstaclePatchesToAplly.pop();
	}
}

void Landscape::readHabitatDomainPatchesFromJSONFiles(const fs::path& configPath)
{
	view->updateLog("Reading habitat domain patches from JSON files ... \n");

	PatchPriorityQueue habitatDomainPatchesToAplly;

	fs::path habitatDomainFolder = configPath / HABITAT_DOMAIN_FOLDER_NAME;

	if (fs::exists(habitatDomainFolder) && fs::is_directory(habitatDomainFolder))
	{
		JsonValidator habitatDomainPatchesValidator(HABITAT_DOMAIN_PATCH_SCHEMA);

		for (const auto& entry : fs::directory_iterator(habitatDomainFolder))
		{
			if (entry.path().extension() == ".json")
			{
				json ptMain = readConfigFile(entry.path(), habitatDomainPatchesValidator);

				habitatDomainPatchesToAplly.push(make_unique<Patch>(new HabitatDomainSource(ptMain["patch"]["source"], getExistingAnimalSpecies()), ptMain["patch"], entry.path().string()));
			}
		}
	}
	else
	{
		throwLineInfoException("Error: The specified path \"" + habitatDomainFolder.string() + "\" does not exist or it is not a directory.");
	}

	while(!habitatDomainPatchesToAplly.empty())
	{
		applyPatch(*habitatDomainPatchesToAplly.top());
		habitatDomainPatchesToAplly.pop();
	}
}

void Landscape::calculateAnimalSpeciesStatistics()
{
	//unique_ptr<unordered_set<Temperature>> globalTemperatureRange = landscapeMap->obtainGlobalTemperatureRange();

	for(AnimalSpecies*& animalSpecies : getMutableExistingAnimalSpecies()) {
		(void) animalSpecies;
		// animalSpecies->calculateStatistics();
	}
}

void Landscape::readMoisturePatchesFromJSONFiles(const fs::path& configPath)
{
	view->updateLog("Reading moisture patches from JSON files ... \n");

	PatchPriorityQueue moisturePatchesToAplly;

	fs::path moistureFolder = configPath / MOISTURE_FOLDER_NAME;

	if (fs::exists(moistureFolder) && fs::is_directory(moistureFolder))
	{
		JsonValidator moisturePatchesValidator(MOISTURE_PATCH_SCHEMA);

		for (const auto& entry : fs::directory_iterator(moistureFolder))
		{
			if (entry.path().extension() == ".json")
			{
				json ptMain = readConfigFile(entry.path(), moisturePatchesValidator);

				moisturePatchesToAplly.push(make_unique<Patch>(new MoistureSource(ptMain["patch"]["source"], getTimeStepsPerDay()), ptMain["patch"], entry.path().string()));
			}
		}
	}
	else
	{
		throwLineInfoException("Error: The specified path \"" + moistureFolder.string() + "\" does not exist or it is not a directory.");
	}

	while(!moisturePatchesToAplly.empty())
	{
		const bool isApplied = applyPatch(*moisturePatchesToAplly.top());

		if(isApplied)
		{
			addAppliedMoisture(static_cast<MoistureSource* const>(moisturePatchesToAplly.top()->moveSource()), moisturePatchesToAplly.top()->getPriority());
		}

		moisturePatchesToAplly.pop();
	}
}



bool Landscape::applyPatch(Patch& patch)
{
	view->updateLog({patch.getDescription(), "\n\n"});

	return landscapeMap->applyPatch(this, patch);
}


void Landscape::addAppliedMoisture(MoistureSource* const source, const size_t priority)
{
	appliedMoisture.push_back(make_pair<>(priority, source));
}


void Landscape::addAppliedResource(ResourceSource* const source, const size_t priority)
{
	appliedResource[source->getResourceSpeciesId()].push_back(make_pair<>(priority, source));
}


bool Landscape::getCompetitionAmongResourceSpecies() const
{
	return competitionAmongResourceSpecies;
}

const vector<ResourceSpecies*>& Landscape::getExistingResourceSpecies() const
{
	return existingResourceSpecies;
}

vector<ResourceSpecies*>& Landscape::getMutableExistingResourceSpecies()
{
	return existingResourceSpecies;
}

vector<AnimalSpecies*>& Landscape::getMutableExistingAnimalSpecies()
{
	return existingAnimalSpecies;
}

const vector<AnimalSpecies*>& Landscape::getExistingAnimalSpecies() const
{
	return existingAnimalSpecies;
}

const vector<Species*>& Landscape::getExistingSpecies() const
{
	return existingSpecies;
}

vector<Species*>& Landscape::getMutableExistingSpecies()
{
	return existingSpecies;
}

void Landscape::resetEdibleIdCounter() 
{ 
	edibleIdCounter = resourceIdCounter; 
}

void Landscape::setExitTimeThreshold(float exitTimeThresholdValue)
{
	exitTimeThreshold = exitTimeThresholdValue;
}


/*
void Landscape::setGaussianResourcePatch(ResourceSpecies* species, unsigned int xpos, unsigned int ypos, unsigned int zpos,
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

void Landscape::setRandomGaussianResourcePatches(ResourceSpecies* species, unsigned int number, float radius, float newSigma, bool useRandomSigma, float newAmplitude, bool useRandomAmplitude, double resourceMaximumCapacity, bool patchSpread)
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
void Landscape::setGaussianWaterPatch(unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, float sigma, float amplitude)
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

void Landscape::setRandomGaussianWaterPatches(unsigned int number, float radius, float newSigma, bool useRandomSigma,
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

pair<AnimalStatistical*, Instar> Landscape::getRandomPredator(const size_t numberOfTotalPotentialPredators,
		const CustomIndexedVector<Instar, vector<AnimalStatistical*>*> &potentialPredators) const
{
	size_t predatorIndex = Random::randomIndex(numberOfTotalPotentialPredators);
	AnimalStatistical* predator;

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

AnimalStatistical* Landscape::getRandomPrey(const CustomIndexedVector<Instar, size_t> &numberOfPotentialPreysPerInstar,
		const Instar &predatorInstar, const AnimalSpecies* const predatorSpecies,
		const vector<CustomIndexedVector<Instar, vector<AnimalStatistical*>>> &animalsPopulation) const
{
	size_t preyIndex = Random::randomIndex(numberOfPotentialPreysPerInstar[predatorInstar]);
	AnimalStatistical* prey = nullptr;

	bool foundPrey = false;

	auto speciesIt = predatorSpecies->getInstarEdibleAnimalSpecies(predatorInstar).begin();
	while(!foundPrey)
	{
		for(const Instar &preyInstar : speciesIt->second)
		{
			if(preyIndex < animalsPopulation[static_cast<AnimalSpecies*>(getExistingSpecies()[speciesIt->first])->getAnimalSpeciesId()][preyInstar].size())
			{
				foundPrey = true;
				prey = animalsPopulation[static_cast<AnimalSpecies*>(getExistingSpecies()[speciesIt->first])->getAnimalSpeciesId()][preyInstar].at(preyIndex);
			}
			else
			{
				preyIndex -= animalsPopulation[static_cast<AnimalSpecies*>(getExistingSpecies()[speciesIt->first])->getAnimalSpeciesId()][preyInstar].size();
			}
		}
		
		if(!foundPrey)
		{
			++speciesIt;
		}
	}

	return prey;
}

void Landscape::calculateAttackStatistics(vector<CustomIndexedVector<Instar, vector<vector<TerrainCell*>::iterator>>> &mapSpeciesInhabitableTerrainCells)
{
	view->updateLog({"Size of the Animal class: ", to_string(sizeof(Animal)), "\n"});
	view->updateLog({"Size of the Genome class: ", to_string(sizeof(Genome)), "\n"});
	view->updateLog({"Size of the TerrainCell class: ", to_string(sizeof(TerrainCell)), "\n"});
	view->updateLog("Creating heating code individuals... \n");

	vector<CustomIndexedVector<Instar, vector<AnimalStatistical*>>> animalsPopulation;
	unsigned int populationSize = landscapeMap->generateStatisticsPopulation(animalsPopulation, view, this, getMutableExistingAnimalSpecies(), mapSpeciesInhabitableTerrainCells, TimeStep(0), getTimeStepsPerDay());
	
	view->updateLog({"A total of ", to_string(populationSize), " heating code individuals have been created.\n"});

	//Only for predators. The experiment is carried out for every predator species and its linked species, up until the specified numberOfCombinations.

	view->updateLog("Calculating attack statistics: \n");
	for(AnimalSpecies*& predatorAnimalSpecies : getMutableExistingAnimalSpecies())
	{
		view->updateLog({">> Simulating ", to_string(numberOfCombinations), " attacks from the species \"", predatorAnimalSpecies->getScientificName(), "\"... \n"});

		size_t numberOfTotalPotentialPredators = 0u;
		CustomIndexedVector<Instar, vector<AnimalStatistical*>*> potentialPredators(predatorAnimalSpecies->getGrowthBuildingBlock().getNumberOfInstars(), nullptr);

		CustomIndexedVector<Instar, size_t> numberOfPotentialPreysPerInstar(predatorAnimalSpecies->getGrowthBuildingBlock().getNumberOfInstars(), 0);

		for(const Instar &predatorInstar : predatorAnimalSpecies->getGrowthBuildingBlock().getInstarsRange())
		{
			if(predatorAnimalSpecies->eatsAnimalSpecies(predatorInstar))
			{
				potentialPredators[predatorInstar] = &animalsPopulation.at(predatorAnimalSpecies->getAnimalSpeciesId())[predatorInstar];
				numberOfTotalPotentialPredators += potentialPredators[predatorInstar]->size();

				for(const auto &[preySpecies, speciesInstars] : predatorAnimalSpecies->getInstarEdibleAnimalSpecies(predatorInstar))
				{
					for(const Instar &preyInstar : speciesInstars)
					{
						numberOfPotentialPreysPerInstar[predatorInstar] += animalsPopulation.at(static_cast<AnimalSpecies*>(getExistingSpecies()[preySpecies])->getAnimalSpeciesId())[preyInstar].size();
					}
				}
			}
		}

		if(numberOfTotalPotentialPredators > 0)
		{
			ProgressBar progressBar(view, numberOfCombinations);

			vector<pair<AnimalStatistical*, AnimalStatistical*>> vectorOfAttacks;
			vectorOfAttacks.reserve(numberOfCombinations);

			while(vectorOfAttacks.size() < numberOfCombinations)
			{
				Instar predatorInstar;
				AnimalStatistical* predator;
				tie(predator, predatorInstar) = getRandomPredator(numberOfTotalPotentialPredators, potentialPredators);
				
				AnimalStatistical* prey = getRandomPrey(numberOfPotentialPreysPerInstar, predatorInstar, predatorAnimalSpecies, animalsPopulation);
				
				if(predator->getSpecies()->canEatEdible(prey->getSpecies()->getId(), predator->getGrowthBuildingBlock().getInstar(), prey->getGrowthBuildingBlock().getInstar()) && predator != prey)
				{
					pair<AnimalStatistical*, AnimalStatistical*> currentAttack = make_pair(predator, prey);
					if(find(vectorOfAttacks.begin(), vectorOfAttacks.end(), currentAttack) == vectorOfAttacks.end())
					{
						vectorOfAttacks.push_back(currentAttack);

						//Computing the total mean values.
						predatorAnimalSpecies->interactionRanges(*predator, *prey);

						progressBar.update();
					}
				}
			}
		}
		else
		{
			ProgressBar progressBar(view, 1u);

			progressBar.update();
		}
	}

	eraseStatisticsPopulation(animalsPopulation);

	view->updateLog("Calculating attack statistics DONE\n");
}

void Landscape::eraseStatisticsPopulation(std::vector<CustomIndexedVector<Instar, std::vector<AnimalStatistical *>>>& population)
{
	for(auto& animalSpecies : population)
	{
		for(auto& instar : animalSpecies)
		{
			for(AnimalStatistical *& animal : instar)
			{
				animal->getMutableTerrainCell()->eraseAnimal((animal));
			}
		}
	}


	resetEdibleIdCounter();
}

PreciseDouble Landscape::Yodzis(const PreciseDouble& wetMass, const PreciseDouble& newA, const PreciseDouble& newB) const
{
	return newA*pow(wetMass, newB);
}

PreciseDouble Landscape::Garland1983(const PreciseDouble& wetMass) const
{
	return (152*pow(wetMass,0.738)) / 1000;
}

PreciseDouble Landscape::calculateNewVoracity(const PreciseDouble&wetMass, const PreciseDouble&conversionToWetMass) const
{
	return calculateWetFood(wetMass)/conversionToWetMass;
}

Map* Landscape::getMutableMap()
{
	return landscapeMap;
}

const Map* Landscape::getMap() const
{
	return landscapeMap;
}

void Landscape::initializeAnimals(const CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, unsigned int>>& initialPopulation, const CustomIndexedVector<AnimalSpeciesID, std::vector<Genome>>& initialGenomesPool, std::vector<CustomIndexedVector<Instar, std::vector<std::vector<TerrainCell*>::iterator>>>& mapSpeciesInhabitableTerrainCells)
{
	view->updateLog("Giving life to animals... \n");

	for(AnimalSpecies*& animalSpecies : getMutableExistingAnimalSpecies())
	{
		if(initialPopulation[animalSpecies->getAnimalSpeciesId()].size() > 0)
    	{
			ostringstream animalConstitutiveTraitsContent;

			landscapeMap->generatePopulation(view, this, animalSpecies, initialPopulation[animalSpecies->getAnimalSpeciesId()], initialGenomesPool[animalSpecies->getAnimalSpeciesId()], mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()], getSaveAnimalConstitutiveTraits(), animalConstitutiveTraitsContent, saveGenetics, saveMassInfo, TimeStep(0), getTimeStepsPerDay());
		
			if(getSaveAnimalConstitutiveTraits())
			{
				if(getSaveAnimalConstitutiveTraits())
				{
					*animalConstitutiveTraitsFile[animalSpecies->getAnimalSpeciesId()] << animalConstitutiveTraitsContent.str();
					animalConstitutiveTraitsFile[animalSpecies->getAnimalSpeciesId()]->flush();
				}
			}
		}
	}

	view->updateLog("DONE\n");
}

void Landscape::initializeOutputFiles(fs::path configPath)
{
	std::ofstream versionsFile;

	versionsFile.open((resultFolder / fs::path("versions.txt")).string());

	versionsFile << "PROGRAM_VERSION:" << PROGRAM_VERSION << endl; 
	versionsFile << "SCHEMA_VERSION:" << SCHEMA_VERSION << endl; 
	versionsFile << "SERIALIZATION_VERSION:" << SERIALIZATION_VERSION << endl;

	versionsFile.close();

	///////////////////////////////////////////////////////////////////////////

	fs::create_directories(resultFolder / fs::path("config"));

	// Copy simulation configuration
	for (const auto& entry : fs::directory_iterator(configPath)) {
		fs::path destinationPath = resultFolder / fs::path("config") / entry.path().filename();

		if (fs::is_directory(entry.path())) {
			fs::copy(entry.path(), destinationPath, fs::copy_options::recursive);
		} else {
			fs::copy_file(entry.path(), destinationPath, fs::copy_options::overwrite_existing);
		}
	}

	///////////////////////////////////////////////////////////////////////////

	if(getSaveAnimalConstitutiveTraits())
	{
		fs::create_directories(resultFolder / fs::path("animal_constitutive_traits"));

		animalConstitutiveTraitsFile.resize(getExistingAnimalSpecies().size());

		ostringstream animalConstitutiveTraitsHeader;

		animalConstitutiveTraitsHeader << "id\tspecies";

		for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
		{
			animalConstitutiveTraitsHeader << "\t" << magic_enum::enum_names<Axis>()[axis];
		}

		animalConstitutiveTraitsHeader << "\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\tdateEgg";

		for(const auto &animalSpecies : getExistingAnimalSpecies())
		{
			string scientificName = animalSpecies->getScientificNameReplaced();

			animalConstitutiveTraitsFile[animalSpecies->getAnimalSpeciesId()] = new std::ofstream();

			string filename = createOutputFile(*animalConstitutiveTraitsFile[animalSpecies->getAnimalSpeciesId()], resultFolder / fs::path("animal_constitutive_traits"), scientificName, "txt");

			if(!animalConstitutiveTraitsFile[animalSpecies->getAnimalSpeciesId()]->is_open())
			{
				throwLineInfoException("Error opening the file '" + scientificName + " constitutive traits'");
			}

			*animalConstitutiveTraitsFile[animalSpecies->getAnimalSpeciesId()] << animalConstitutiveTraitsHeader.str();

			for(Trait::ExecutionOrder order : EnumClass<Trait::ExecutionOrder>::getEnumValues())
			{
				for(Trait* trait : animalSpecies->getMutableGenetics().getAllTraits()[order])
				{
					if(trait->getValue()->getType() == IndividualLevelTrait::Type::IndividualLevel)
					{
						*animalConstitutiveTraitsFile[animalSpecies->getAnimalSpeciesId()] << "\t" << static_cast<IndividualLevelTrait*>(trait->getValue())->getTraitStr();
					}
				}
			}

			*animalConstitutiveTraitsFile[animalSpecies->getAnimalSpeciesId()] << endl;

			animalConstitutiveTraitsFile[animalSpecies->getAnimalSpeciesId()]->flush();
		}
	}

	///////////////////////////////////////////////////////////////////////////

	if(saveGenetics)
	{
		for(const auto &animalSpecies : getExistingAnimalSpecies())
		{
			animalSpecies->initializeGeneticFiles(resultFolder / fs::path("genetics"));
		}
	}

	///////////////////////////////////////////////////////////////////////////

	if(saveDailySummary)
	{
		dailySummaryFile.open((resultFolder / fs::path("dailySummary.txt")).string());

		if (!dailySummaryFile.is_open())
		{
			throwLineInfoException("Error opening the dailySummaryFile.");
		}
		else
		{
			dailySummaryFile << "DAY\tBIOMASS\tPREY_UNBORN\tPREY_ACTIVE\tPREY_STARVED\tPREY_PREDATED\tPREY_REPRODUCING\tPREY_SENESCED\tPREY_SHOCKED\tPREDATOR_UNBORN\tPREDATOR_ACTIVE\tPREDATOR_STARVED\tPREDATOR_PREDATED\tPREDATOR_REPRODUCING\tPREDATOR_BACKGROUND\tPREDATOR_SENESCED\tPREDATOR_SHOCKED" << endl;
		}
	}

	///////////////////////////////////////////////////////////////////////////

	if(saveExtendedDailySummary)
	{
		extendedDailySummaryFile.open((resultFolder / fs::path("extendedDailySummary.txt")).string());

		if (!extendedDailySummaryFile.is_open())
		{
			throwLineInfoException("Error opening the extendedDailySummaryFile.");
		}
		else
		{
			ostringstream extendedDailySummaryHeader;

			extendedDailySummaryHeader << "day";

			for (auto itResourceSpecies = existingResourceSpecies.begin(); itResourceSpecies != existingResourceSpecies.end(); itResourceSpecies++)
			{
				extendedDailySummaryHeader << "\t" << (*itResourceSpecies)->getScientificName() << "_biomass";
			}

			for(const AnimalSpecies* const&animalSpecies : getExistingAnimalSpecies())
			{
				for(const LifeStage &lifeStage : EnumClass<LifeStage>::getEnumValues())
				{
					extendedDailySummaryHeader << "\t" << animalSpecies->getScientificName() << "_" << EnumClass<LifeStage>::to_string(lifeStage);
				}
			}

			extendedDailySummaryHeader << "\n";


			extendedDailySummaryFile << extendedDailySummaryHeader.str();
			extendedDailySummaryFile.flush();
		}
	}

	///////////////////////////////////////////////////////////////////////////

	if(saveMovements)
	{
		movementsFile.open((resultFolder / fs::path("movements.txt")).string());

		if (!movementsFile.is_open())
		{
			throwLineInfoException("Error opening the movementsFile.");
		}
		else
		{
			movementsFile << "timeStep\tid\tstartPointX\tstartPointY\tendPointX\tendPointY" << endl;
		}
	}

	///////////////////////////////////////////////////////////////////////////
		
	if(saveEdibilitiesFile)
	{
		createOutputFile(edibilitiesFile, resultFolder, "edibilities", "txt", std::ofstream::out | std::ofstream::trunc);
	
		edibilitiesFile << "timeStep\tsearcherId\tsearcherSpecies\tfoodMass\tpredatorId\tpredatorSpecies\tpredatorDryMass\tpredatedId\tpredatedSpecies\tpredatedDryMass\tpredationProbability\tedibility\tpreference\texperience\n";
	}

	///////////////////////////////////////////////////////////////////////////

	if(isCheckpointsEnabled())
	{
		fs::create_directories(resultFolder / fs::path("checkpoints"));
	}
}

EdibleID Landscape::generateEdibleId()
{
	return EdibleID(edibleIdCounter++);
}

id_type Landscape::generateResourceId()
{
	return resourceIdCounter++;
}

id_type Landscape::generateAnimalId()
{
	return animalIdCounter++;
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(Landscape)

template <class Archive>
void Landscape::serialize(Archive &ar, const unsigned int) {
	ar & serializationVersion;

	if (Archive::is_loading::value) {
		if (serializationVersion != SERIALIZATION_VERSION) {
			throwLineInfoException(
				"Version mismatch: serialized data was created with version " + serializationVersion +
				", but current serialization version is " + SERIALIZATION_VERSION + "."
			);
		}
	}

	ar & edibleIdCounter;
	ar & resourceIdCounter;
	ar & animalIdCounter;

	ar & appliedMoisture;

	ar & appliedResource;

	ar & heatingCodeTemperatureCycle;

	ar & existingResourceSpecies;

	if(Archive::is_loading::value)
	{
		for(ResourceSpecies*& newSpecies : existingResourceSpecies)
		{
			existingSpecies.push_back(newSpecies);
		}
	}

	ar & existingAnimalSpecies;

	if(Archive::is_loading::value)
	{
		for(AnimalSpecies*& newSpecies : existingAnimalSpecies)
		{
			existingSpecies.push_back(newSpecies);
		}
	}
	
	ar & landscapeMap;

	ar & pdfThreshold;

	ar & multiplierForFieldMetabolicRate;
}

template void Landscape::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive &, const unsigned int);
template void Landscape::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive &, const unsigned int);
template void Landscape::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive &, const unsigned int);
template void Landscape::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive &, const unsigned int);


