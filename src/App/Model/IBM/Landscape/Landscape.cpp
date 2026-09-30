
/*
#include "tbb/tbb.h"
*/
#include <thread>
#include <utility>
#include <algorithm> // transform
#include <oneapi/tbb/parallel_for.h>
#include <tbb/spin_mutex.h>

#include "App/Manager/TbbAdaptiveWrapper.h"

#include "App/Model/IBM/Landscape/Landscape.h"

#include "Misc/Utilities.h"
#include "App/Model/IBM/Landscape/ArthropodsLandscape.h"
#include "App/Model/IBM/Landscape/DinosaursLandscape.h"
#include "App/Model/IBM/Landscape/AquaticLandscape.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalSignature.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/ResourceSignature.h"

#include "App/Manager/LogManager.h"
#include "App/IO/StorageBridge.h"

#include "schema/simulation_params_schema_json.h"
#include "schema/landscape_params_schema_json.h"
#include "schema/species_schema_json.h"
#include "schema/resource_schema_json.h"
#include "schema/resource_patch_schema_json.h"
#include "schema/obstacle_patch_schema_json.h"
#include "schema/habitat_domain_patch_schema_json.h"
#include "schema/moisture_patch_schema_json.h"

#include <fmt/compile.h>
#include <fmt/format.h>
#include <iterator>

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
	: localProgressBarCounter(0), serializationVersion(WEAVER_SERIALIZATION_VERSION)
{
	
}

void Landscape::init(fs::path configPath, fs::path newOutputFolder, bool fromCheckpoint)
{
	if(!fromCheckpoint) {
		edibleIdCounter = 1;
		resourceIdCounter = 1;
		animalIdCounter = 1;
	}


	setSimulationParams(configPath);


	setOutputFolder(newOutputFolder, configPath.filename().string());


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

	ResourceSignature::configureBits(getExistingResourceSpecies().size());


	CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, unsigned int>> initialPopulation(getExistingAnimalSpecies().size());

	bool newAnimalSpecies = readAnimalSpeciesFromJSONFiles(configPath, initialPopulation);


	size_t maxNumberOfInstar = 0;

	for (const AnimalSpecies* const& animalSpecies : getExistingAnimalSpecies())
	{
		if (animalSpecies->getGrowthBuildingBlock().getNumberOfInstars() > maxNumberOfInstar)
		{
			maxNumberOfInstar = animalSpecies->getGrowthBuildingBlock().getNumberOfInstars();
		}
	}

	AnimalSignature::configureBits(maxNumberOfInstar, getExistingAnimalSpecies().size());


	this->localBuffersAnimalConstitutiveTraits = tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, fmt::memory_buffer>>([&]() {
		CustomIndexedVector<AnimalSpeciesID, fmt::memory_buffer> newVector;
		newVector.resize(getExistingAnimalSpecies().size());
		return newVector;
		});


	this->localBuffersGenetics = tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, std::vector<fmt::memory_buffer>>>([&]() {
		CustomIndexedVector<AnimalSpeciesID, std::vector<fmt::memory_buffer>> newVector;
		newVector.resize(getExistingAnimalSpecies().size());

		for (auto& animalSpecies : getExistingAnimalSpecies()) {
			newVector[animalSpecies->getAnimalSpeciesId()].resize(animalSpecies->getGenetics().getIndividualLevelTraits().size());
		}

		return newVector;
		});
	


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


	if (newEcosystem) {
		setOntogeneticLinks(configPath);
	}


	for (AnimalSpecies* animalSpecies : getMutableExistingAnimalSpecies())
	{
		animalSpecies->obtainSearchParams(getExistingSpecies(), getExistingAnimalSpecies(), getExistingResourceSpecies().size());
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

	LogManager::emit(fmt::format(" - Total initial ecosystem size from input: {} individuals.\n", actualEcosystemSize));

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



	AnimalNonStatistical::getHeader(printAnimalsAlongCellsHeader);



	printCellAlongCellsHeader.append(landscapeMap->getMapPositionHeader());

	for(const auto &resourceSpecies : existingResourceSpecies)
	{
		printCellAlongCellsHeader.append("\t" + resourceSpecies->getScientificName());
		printCellAlongCellsHeader.append("\t" + resourceSpecies->getScientificName() + "_available_dry_mass");
	}

	for(const AnimalSpecies* const& animalSpecies : getExistingAnimalSpecies())
	{
		printCellAlongCellsHeader.append("\t" + animalSpecies->getScientificName());
	}
}

void Landscape::setSimulationParams(const fs::path& configPath)
{
	JsonValidator simulationValidator(EmbeddedResources::simulation_params_schema_json, "simulation_params_schema");

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

	saveExtendedDailySummary = simulationConfiguration["simulation"]["outputs"]["saveExtendedDailySummary"].get<bool>();

	saveMovements = simulationConfiguration["simulation"]["outputs"]["saveMovements"].get<bool>();

	saveAnimalsEachDayStart = simulationConfiguration["simulation"]["outputs"]["saveAnimalsEachDayStart"].get<bool>();

	saveAnimalsEachDayEnd = simulationConfiguration["simulation"]["outputs"]["saveAnimalsEachDayEnd"].get<bool>();

	saveCellsEachDay = simulationConfiguration["simulation"]["outputs"]["saveCellsEachDay"].get<bool>();

	saveAnimalsEachDayVoracities = simulationConfiguration["simulation"]["outputs"]["saveAnimalsEachDayVoracities"].get<bool>();

	saveAnimalsEachDayPredationProbabilities = simulationConfiguration["simulation"]["outputs"]["saveAnimalsEachDayPredationProbabilities"].get<bool>();

	saveActivity = simulationConfiguration["simulation"]["outputs"]["saveActivity"].get<bool>();

	saveMassInfo = simulationConfiguration["simulation"]["outputs"]["saveMassInfo"].get<bool>();


	saveSnapshots = simulationConfiguration["simulation"]["outputs"]["saveSnapshots"].get<bool>();
	if(saveSnapshots)
	{
		saveSnapshotsPeriodicity = simulationConfiguration["simulation"]["outputs"]["saveSnapshotsPeriodicity"].get<unsigned int>();
	}
}

void Landscape::setLandscapeParams(const fs::path& configPath, bool fromCheckpoint)
{
	JsonValidator landscapeValidator(EmbeddedResources::landscape_params_schema_json, "landscape_params_schema");

	json landscapeConfig = readConfigFile(configPath / fs::path("landscape_params.json"), landscapeValidator);


	minExploitableResource = landscapeConfig["landscape"]["life"]["minExploitableResource"].get<double>();

	competitionAmongResourceSpecies = landscapeConfig["landscape"]["life"]["competitionAmongResourceSpecies"].get<bool>();

	pdfThreshold = landscapeConfig["landscape"]["life"]["pdfThreshold"].get<double>();

	setExitTimeThreshold(landscapeConfig["landscape"]["life"]["exitTimeThreshold"]);

	multiplierForFieldMetabolicRate = landscapeConfig["landscape"]["life"]["multiplierForFieldMetabolicRate"].get<double>();


	if(!fromCheckpoint)
	{
		initializeMap(landscapeConfig["landscape"]["mapConfig"]);
	}

	terrainCells.resize(static_cast<size_t>(pow(landscapeMap->getNumberOfCellsPerAxis(), DIMENSIONS)), nullptr);

	landscapeMap->registerCells(terrainCells);
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

    filename.append(to_string(1900 + tmLocal.tm_year) + "-");	// Año
    filename.append(to_string(1 + tmLocal.tm_mon) + "-");		// Mes
    filename.append(to_string(tmLocal.tm_mday) + "_");       	// Día
    filename.append(to_string(tmLocal.tm_hour) + "-");       	// Hora
    filename.append(to_string(tmLocal.tm_min) + "-");        	// Minuto
    filename.append(to_string(tmLocal.tm_sec));        			// Segundo

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

	if (saveSnapshots)
	{
		fs::create_directories(resultFolder / fs::path("Snapshots"));
	}

	if (savePredationEventsOnOtherSpecies)
	{
		fs::create_directories(resultFolder / fs::path("Matrices"));
	}

	if (saveAnimalsEachDayStart)
	{
		fs::create_directories(resultFolder / fs::path("animals_each_day_start"));
	}

	if (saveAnimalsEachDayEnd)
	{
		fs::create_directories(resultFolder / fs::path("animals_each_day_end"));
	}

	if (saveCellsEachDay)
	{
		fs::create_directories(resultFolder / fs::path("cells_each_day"));
	}

	if (saveAnimalsEachDayVoracities)
	{
		fs::create_directories(resultFolder / fs::path("animals_each_day_voracities"));
	}

	if (saveAnimalsEachDayPredationProbabilities) 
	{
		fs::create_directories(resultFolder / fs::path("animals_each_day_predationProbabilities"));
	}

	if (saveActivity)
	{
		fs::create_directories(resultFolder / fs::path("animals_each_day_activity"));
	}

	if (saveMassInfo)
	{
		fs::create_directories(resultFolder / fs::path("massInfo"));
	}

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
}

bool Landscape::readAnimalSpeciesFromJSONFiles(const fs::path& configPath, CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, unsigned int>>& initialPopulation)
{
	LogManager::emit("Reading all animal species from JSON files ... \n");

	fs::path speciesFolder = configPath / SPECIES_FOLDER_NAME;
	
	if (fs::exists(speciesFolder) && fs::is_directory(speciesFolder))
	{
		bool newAnimalSpecies = false;

		JsonValidator animalSpeciesValidator(EmbeddedResources::species_schema_json, "species_schema");

		for (const auto& entry : fs::directory_iterator(speciesFolder))
		{
			if (entry.path().extension() == ".json")
			{
				json ptMain = readConfigFile(entry.path(), animalSpeciesValidator);

				LogManager::emit(fmt::format(" - Animal scientific name: {}\n", ptMain["animal"]["name"].get<string>()));

				addAnimalSpecies(ptMain, initialPopulation);

				newAnimalSpecies = true;
			}
		}

		LogManager::emit("DONE\n\n");

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
		LogManager::emit(fmt::format("Animal species {} eats: \n", animalSpecies->getScientificName()));

		animalSpecies->setOntogeneticLinks(getExistingSpecies(), ontogeneticLinksPreference, ontogeneticLinksProfitability);
	}


	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		landscapeAnimals[i]->setInitialPreferences(getTimeStepsPerDay());
	}
}

bool Landscape::readResourceSpeciesFromJSONFiles(const fs::path& configPath)
{
	LogManager::emit("Reading all resource species from JSON files ... \n");
	
	fs::path resourceFolder = configPath / RESOURCE_FOLDER_NAME;
	
	if (fs::exists(resourceFolder) && fs::is_directory(resourceFolder))
	{
		bool newResourceSpecies = false;

		JsonValidator resourceSpeciesValidator(EmbeddedResources::resource_schema_json, "resource_schema");

		for (const auto& entry : fs::directory_iterator(resourceFolder))
		{
			if (entry.path().extension() == ".json")
			{
				json ptMain = readConfigFile(entry.path(), resourceSpeciesValidator);

				LogManager::emit(fmt::format(" - Resource scientific name: {}\n\n", ptMain["resource"]["name"].get<string>()));

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

	LogManager::emit("Reading all resource patches from JSON files ... \n");
	
	vector<PatchPriorityQueue> resourcePatchesToAplly(getExistingResourceSpecies().size());

	fs::path resourcePatchesFolder = configPath / RESOURCE_FOLDER_NAME / "patches";

	if(fs::exists(resourcePatchesFolder) && fs::is_directory(resourcePatchesFolder))
	{
		JsonValidator resourcePatchesValidator(EmbeddedResources::resource_patch_schema_json, "resource_patch_schema");

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
						ptMain["patch"]["source"], getMutableExistingResourceSpecies()[resourceSpeciesId], 
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
		LogManager::emit(fmt::format(" - Resource scientific name: {}\n\n", resourceSpecies->getScientificName()));

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

	landscapeMap->addResourceSpecies(this, landscapeResources, *getMutableExistingResourceSpecies().back(), resourceBaseSource);


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


	landscapeMap->addAnimalSpecies(*getExistingAnimalSpecies().back());
}

void Landscape::addSpecies(Species* newSpecies)
{
	LogManager::emit(fmt::format("numberOfInstars: {}\n", newSpecies->getGrowthBuildingBlock().getNumberOfInstars()));

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


void Landscape::printAnimalsAlongCells(const TimeStep numberOfTimeSteps, const int simulationPoint)
{
	if((saveAnimalsEachDayStart && simulationPoint == 0) || (saveAnimalsEachDayEnd && simulationPoint == 1))
	{
		if(((numberOfTimeSteps % recordEach) == 0) || (numberOfTimeSteps == TimeStep(0)))
		{
			string pathBySimulationPoint = (simulationPoint == 0) ? "animals_each_day_start" : "animals_each_day_end";
			string timeStepStr = string(MAX_NUM_DIGITS_DAY - to_string(numberOfTimeSteps.getValue()).length(), '0') + to_string(numberOfTimeSteps.getValue());
			fs::path filePath = resultFolder / fs::path(pathBySimulationPoint) / (std::string("animals_day_") + timeStepStr + ".txt");

			StorageBridge::writeHeaderPackToDisk(filePath, printAnimalsAlongCellsHeader);


			// 1. Obtener de forma segura el límite máximo de hilos concurrentes de la arena actual
			size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());
			constexpr size_t estimatedAnimalLineSize = 2000u;

			size_t estimatedSize = landscapeAnimals.size() * estimatedAnimalLineSize;

			// 2. Calcular la estimación del búfer basada en la concurrencia real de la arena
			size_t estimatedPerThreadSize = estimatedSize / maxThreadsInArena;

			auto& persistentBuffers = (simulationPoint == 0) ? localBuffersAnimalsStart : localBuffersAnimalsEnd;


			TbbAdaptiveWrapper::Execute(landscapeAnimals, 0, landscapeAnimals.size(), 3u,
				[&](auto& animals, size_t start, size_t end) {
					fmt::memory_buffer& localStr = persistentBuffers.local();
					localStr.reserve(estimatedSize);

					for (size_t i = start; i < end; ++i) {
						animals[i]->formatToBufferDirect(localStr);
					}
				},
				[&](auto& animals, size_t start, size_t end) {
					tbb::parallel_for(tbb::blocked_range<size_t>(start, end),
						[&](const tbb::blocked_range<size_t>& r) {
							fmt::memory_buffer& localStr = persistentBuffers.local();
							localStr.reserve(estimatedPerThreadSize);

							for (size_t i = r.begin(); i < r.end(); ++i) {
								animals[i]->formatToBufferDirect(localStr);
							}
						}
					);
				}
			);


			// 4. Enviar un paquete por cada hebra usando std::move (Cero copias, contención mínima)
			for (fmt::memory_buffer& threadBuffer : persistentBuffers) {
				if (threadBuffer.size() != 0) {
					StorageBridge::writeContentPackToDisk(filePath, threadBuffer);
				}
			}

			StorageBridge::writeClosePackToDisk(filePath);
		}
	}
}

void Landscape::printCellAlongCells(const TimeStep numberOfTimeSteps)
{
	if(saveCellsEachDay)
	{
		if(((numberOfTimeSteps % recordEach) == 0) || (numberOfTimeSteps == TimeStep(0)))
		{
			string timeStepStr = string(MAX_NUM_DIGITS_DAY - to_string(numberOfTimeSteps.getValue()).length(), '0') + to_string(numberOfTimeSteps.getValue());

			fs::path filePath = resultFolder / fs::path("cells_each_day") / (std::string("cells_day_") + timeStepStr + ".txt");

			StorageBridge::writeHeaderPackToDisk(filePath, printCellAlongCellsHeader);

			const size_t numAnimalSpecies = existingAnimalSpecies.size();

			size_t estimatedSize = terrainCells.size() * 256;

			// 1. Obtener de forma segura el límite máximo de hilos concurrentes de la arena actual
			size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());

			// 2. Calcular la estimación del búfer basada en la concurrencia real de la arena
			size_t estimatedPerThreadSize = estimatedSize / maxThreadsInArena;


			TbbAdaptiveWrapper::Execute(terrainCells, 0, terrainCells.size(), 1u,
				[&](auto& cells, size_t start, size_t end) {
					fmt::memory_buffer& localStr = localBuffersCells.local();
					localStr.reserve(estimatedSize);

					for (size_t i = start; i < end; ++i) {
						const auto& cell = cells[i];

						const PointMap& position = cell->getPosition();

						fmt::format_to(fmt::appender(localStr), "{}", position.get(magic_enum::enum_cast<Axis>(0).value()));

						for (unsigned int axis = 1; axis < DIMENSIONS; axis++)
						{
							fmt::format_to(fmt::appender(localStr), "\t{}", position.get(magic_enum::enum_cast<Axis>(axis).value()));
						}


						for (size_t j = 0; j < cell->getPatchApplicator().getNumberOfResources(); j++)
						{
							const CellResourceInterface& resource = cell->getPatchApplicator().getCellResource(j);

							fmt::format_to(fmt::appender(localStr), "\t{}\t{}",
								resource.getGrowthBuildingBlock().getCurrentTotalWetMass().getValue().getValue(),
								resource.calculateDryMassAvailable(true, nullptr, 0.0).getValue().getValue()
							);
						}


						std::vector<uint32_t> animalCounts(numAnimalSpecies, 0u);

						for (const auto* animal : landscapeAnimals) {
							if (cell->isAnimalInside(animal->getPosition())) {
								animalCounts[animal->getAnimalSpeciesId()]++;
							}
						}

						for (uint32_t count : animalCounts)
						{
							fmt::format_to(fmt::appender(localStr), "\t{}", count);
						}


						localStr.push_back('\n');
					}
				},
				[&](auto& cells, size_t start, size_t end) {
					tbb::parallel_for(tbb::blocked_range<size_t>(start, end),
						[&](const tbb::blocked_range<size_t>& r) {
							fmt::memory_buffer& localStr = localBuffersCells.local();
							localStr.reserve(estimatedPerThreadSize);

							for (size_t i = r.begin(); i < r.end(); ++i) {
								const auto& cell = cells[i];

								const PointMap& position = cell->getPosition();

								fmt::format_to(fmt::appender(localStr), "{}", position.get(magic_enum::enum_cast<Axis>(0).value()));

								for (unsigned int axis = 1; axis < DIMENSIONS; axis++)
								{
									fmt::format_to(fmt::appender(localStr), "\t{}", position.get(magic_enum::enum_cast<Axis>(axis).value()));
								}


								for (size_t j = 0; j < cell->getPatchApplicator().getNumberOfResources(); j++)
								{
									const CellResourceInterface& resource = cell->getPatchApplicator().getCellResource(j);

									fmt::format_to(fmt::appender(localStr), "\t{}\t{}",
										resource.getGrowthBuildingBlock().getCurrentTotalWetMass().getValue().getValue(),
										resource.calculateDryMassAvailable(true, nullptr, 0.0).getValue().getValue()
									);
								}


								std::vector<uint32_t> animalCounts(numAnimalSpecies, 0u);

								for (const auto* animal : landscapeAnimals) {
									if (cell->isAnimalInside(animal->getPosition())) {
										animalCounts[animal->getAnimalSpeciesId()]++;
									}
								}

								for (uint32_t count : animalCounts)
								{
									fmt::format_to(fmt::appender(localStr), "\t{}", count);
								}


								localStr.push_back('\n');
							}
						}
					);
				}
			);


			// 4. Enviar un paquete por cada hebra usando std::move (Cero copias, contención mínima)
			for (fmt::memory_buffer& threadBuffer : localBuffersCells) {
				if (threadBuffer.size() != 0) {
					StorageBridge::writeContentPackToDisk(filePath, threadBuffer);
				}
			}


			StorageBridge::writeClosePackToDisk(filePath);
		}
	}
}

void Landscape::printExtendedDailySummary(const TimeStep numberOfTimeSteps)
{
	vector<double> landscapeResourceBiomass(getExistingResourceSpecies().size());
	CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<LifeStage, unsigned int>> landscapeAnimalsPopulation(getExistingAnimalSpecies().size(), CustomIndexedVector<LifeStage, unsigned int>(EnumClass<LifeStage>::size(), 0));

	landscapeMap->obtainResourcesBiomass(landscapeResourceBiomass);

	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		landscapeAnimalsPopulation[landscapeAnimals[i]->getSpecies()->getAnimalSpeciesId()][landscapeAnimals[i]->getLifeStage()]++;
	}
	

	fmt::format_to(fmt::appender(extendedDailySummaryBuffer), FMT_COMPILE("{}"), numberOfTimeSteps);

	for (const double& resourceBiomass : landscapeResourceBiomass)
	{
		fmt::format_to(fmt::appender(extendedDailySummaryBuffer), FMT_COMPILE("\t{}"), resourceBiomass);
	}

	for (const auto& animalSpeciesPopulation : landscapeAnimalsPopulation)
	{
		for (const auto& lifeStagePopulation : animalSpeciesPopulation)
		{
			fmt::format_to(fmt::appender(extendedDailySummaryBuffer), FMT_COMPILE("\t{}"), lifeStagePopulation);
		}
	}

	extendedDailySummaryBuffer.push_back('\n');


	StorageBridge::writeContentPackToDisk(resultFolder / fs::path("extendedDailySummary.txt"), extendedDailySummaryBuffer);
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
		string fullPath = createOutputFile(file, filenameRoot, filename + "_" + scientificName + "_day_", "dat", numberOfTimeSteps.getValue(), recordEach, ios::out | ios::binary);

		LogManager::emit(fmt::format("Saving Animal as {}... ", fullPath));


		unsigned int value = 0;

		for(size_t i = 0; i < landscapeAnimals.size(); i++)
		{
			if(landscapeAnimals[i]->getLifeStage() == LifeStage::ACTIVE)
			{
				value++;
			}
		}

		file.write((char *) &value, sizeof(unsigned int));


		LogManager::emit("DONE\n");

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
	string fullPath = createOutputFile(file, filenameRoot, filename + "_" + scientificName + "_day_", "dat", numberOfTimeSteps.getValue(), recordEach, ios::out | ios::binary);

	LogManager::emit(fmt::format("Saving Resource as {}... ", fullPath));

	landscapeMap->saveResourceSpeciesSnapshot(file, species);

	LogManager::emit("DONE\n");

	file.close();
}


void Landscape::saveWaterSnapshot(fs::path filenameRoot, string filename, const TimeStep numberOfTimeSteps)
{
	std::ofstream file;
	string fullPath = createOutputFile(file, filenameRoot, filename + "_day_", "dat", numberOfTimeSteps.getValue(), recordEach, ios::out | ios::binary);

	LogManager::emit(fmt::format("Saving Water volume as {}... ", fullPath));

	landscapeMap->saveWaterSnapshot(file);

	file.close();
	LogManager::emit("DONE\n");
}


void Landscape::printPredationEventsOnOtherSpeciesMatrix(const TimeStep numberOfTimeSteps)
{
	std::string header;
	header.reserve(256);

	header = "prey\\predator";
	for (const auto& predatorAnimalSpecies : getExistingAnimalSpecies())
	{
		fmt::format_to(std::back_inserter(header), FMT_COMPILE("\t{}"), predatorAnimalSpecies->getScientificName());
	}

	for (const auto& preySpecies : getExistingSpecies())
	{
		const std::string& preyScientificName = preySpecies->getScientificName();
		predationEventsOnOtherSpeciesMatrixBuffer.append(preyScientificName.data(), preyScientificName.data() + preyScientificName.size());

		for (const auto& predatorAnimalSpecies : getExistingAnimalSpecies())
		{
			fmt::format_to(std::back_inserter(predationEventsOnOtherSpeciesMatrixBuffer), FMT_COMPILE("\t{}"), predatorAnimalSpecies->getPredationEventsOnOtherSpecies(preySpecies->getId()));
		}

		predationEventsOnOtherSpeciesMatrixBuffer.push_back('\n');
	}

	StorageBridge::writeFullFilePackToDisk(resultFolder / fs::path("Matrices") / fmt::format("predationOnSpecies_{}.txt", numberOfTimeSteps), header, predationEventsOnOtherSpeciesMatrixBuffer);
}

void Landscape::updateMap(const TimeStep numberOfTimeSteps)
{
	if(numberOfTimeSteps > TimeStep(0))
	{
		updateAllSources();
		landscapeMap->update(numberOfTimeSteps);
	}



	// 1. Obtener de forma segura el límite máximo de hilos concurrentes de la arena actual
	size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());

	size_t estimatedSize = (landscapeAnimals.size() * 256);

	// 2. Calcular la estimación del búfer basada en la concurrencia real de la arena
	size_t estimatedPerThreadSize = estimatedSize / maxThreadsInArena;


	tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>> maximumInteractionAreaLocal(
		[&]() {
			CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> localVector(getExistingAnimalSpecies().size());

			for (size_t i = 0; i < getExistingAnimalSpecies().size(); ++i) {
				localVector[i] = CustomIndexedVector<Instar, PreciseDouble>(getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
			}

			return localVector;
		}
	);

	tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>> maximumVoracityLocal(
		[&]() {
			CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> localVector(getExistingAnimalSpecies().size());

			for (size_t i = 0; i < getExistingAnimalSpecies().size(); ++i) {
				localVector[i] = CustomIndexedVector<Instar, PreciseDouble>(getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
			}

			return localVector;
		}
	);



	TbbAdaptiveWrapper::Execute(landscapeAnimals, 0, landscapeAnimals.size(), 8u,
		[&](auto& animals, size_t start, size_t end) {
			fmt::memory_buffer& localStr = localBuffersMassInfo.local();
			if (saveMassInfo) {
				localStr.reserve(estimatedSize);
			}

			CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumInteractionArea = maximumInteractionAreaLocal.local();
			CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumVoracity = maximumVoracityLocal.local();


			for (size_t i = start; i < end; ++i) {
				auto animalPtr = animals[i];

				AnimalSpeciesID id = animalPtr->getSpecies()->getAnimalSpeciesId();

				CustomIndexedVector<Instar, PreciseDouble>& maximumInteractionArea = animalSpeciesMaximumInteractionArea[id];
				CustomIndexedVector<Instar, PreciseDouble>& maximumVoracity = animalSpeciesMaximumVoracity[id];


				animalPtr->resetControlVariables(numberOfTimeSteps, getTimeStepsPerDay());

				if (animalPtr->getLifeStage() != LifeStage::UNBORN)
				{
					animalPtr->increaseAge(this, numberOfTimeSteps, getTimeStepsPerDay());
				}

				if (animalPtr->getLifeStage() == LifeStage::UNBORN)
				{
					animalPtr->isReadyToBeBorn(this, getTimeStepsPerDay());
				}

				if (animalPtr->getLifeStage() == LifeStage::DIAPAUSE)
				{
					animalPtr->isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(this);
				}

				if (animalPtr->getLifeStage() == LifeStage::PUPA)
				{
					animalPtr->isReadyToResumeFromPupaOrDecreasePupaTimer(this);
				}

				if (animalPtr->getLifeStage() != LifeStage::UNBORN)
				{
					animalPtr->tune(this, saveMassInfo, localStr, numberOfTimeSteps, getTimeStepsPerDay(), maximumInteractionArea, maximumVoracity);
				}

				if (animalPtr->getLifeStage() == LifeStage::ACTIVE)
				{
					animalPtr->grow(this, numberOfTimeSteps, getTimeStepsPerDay());
				}
			}
		},
		[&](auto& animals, size_t start, size_t end) {
			tbb::parallel_for(tbb::blocked_range<size_t>(start, end),
				[&](const tbb::blocked_range<size_t>& r) {
					fmt::memory_buffer& localStr = localBuffersMassInfo.local();
					if (saveMassInfo) {
						localStr.reserve(estimatedPerThreadSize);
					}

					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumInteractionArea = maximumInteractionAreaLocal.local();
					CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumVoracity = maximumVoracityLocal.local();

					for (size_t i = r.begin(); i < r.end(); ++i) {
						auto animalPtr = animals[i];

						AnimalSpeciesID id = animalPtr->getSpecies()->getAnimalSpeciesId();

						CustomIndexedVector<Instar, PreciseDouble>& maximumInteractionArea = animalSpeciesMaximumInteractionArea[id];
						CustomIndexedVector<Instar, PreciseDouble>& maximumVoracity = animalSpeciesMaximumVoracity[id];


						animalPtr->resetControlVariables(numberOfTimeSteps, getTimeStepsPerDay());

						if (animalPtr->getLifeStage() != LifeStage::UNBORN)
						{
							animalPtr->increaseAge(this, numberOfTimeSteps, getTimeStepsPerDay());
						}

						if (animalPtr->getLifeStage() == LifeStage::UNBORN)
						{
							animalPtr->isReadyToBeBorn(this, getTimeStepsPerDay());
						}

						if (animalPtr->getLifeStage() == LifeStage::DIAPAUSE)
						{
							animalPtr->isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(this);
						}

						if (animalPtr->getLifeStage() == LifeStage::PUPA)
						{
							animalPtr->isReadyToResumeFromPupaOrDecreasePupaTimer(this);
						}

						if (animalPtr->getLifeStage() != LifeStage::UNBORN)
						{
							animalPtr->tune(this, saveMassInfo, localStr, numberOfTimeSteps, getTimeStepsPerDay(), maximumInteractionArea, maximumVoracity);
						}

						if (animalPtr->getLifeStage() == LifeStage::ACTIVE)
						{
							animalPtr->grow(this, numberOfTimeSteps, getTimeStepsPerDay());
						}
					}
				}
			);
		}
	);


	if (saveMassInfo) {
		// 4. Enviar un paquete por cada hebra usando std::move (Cero copias, contención mínima)
		for (fmt::memory_buffer& threadBuffer : localBuffersMassInfo) {
			if (threadBuffer.size() != 0) {
				StorageBridge::writeContentPackToDisk(resultFolder / fs::path("massInfo") / fmt::format("mass_info_{}.txt", numberOfTimeSteps), threadBuffer);
			}
		}
	}


	for (auto& animalSpeciesMaximumInteractionArea : maximumInteractionAreaLocal) {
		for (size_t i = 0; i < animalSpeciesMaximumInteractionArea.size(); ++i) {
			getMutableExistingAnimalSpecies()[i]->updateMaximumInteractionArea(animalSpeciesMaximumInteractionArea[i]);
		}
	}

	for (auto& animalSpeciesMaximumVoracity : maximumVoracityLocal) {
		for (size_t i = 0; i < animalSpeciesMaximumVoracity.size(); ++i) {
			getMutableExistingAnimalSpecies()[i]->getMutableDecisionsBuildingBlock()->updateMaximumVoracity(animalSpeciesMaximumVoracity[i]);
		}
	}


	updateAnimalSpeciesGlobalMaximum();
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
					static_cast<AnimalSpecies*>(getMutableExistingSpecies()[preySpeciesId])->getMutableDecisionsBuildingBlock()->updateMaximumPredatorInteractionArea(preyInstar, predatorAnimalSpecies->getMaximumInteractionArea(predatorInstar));

					predatorAnimalSpecies->getMutableDecisionsBuildingBlock()->updateMaximumPreyVoracity(predatorInstar, static_cast<AnimalSpecies*>(getMutableExistingSpecies()[preySpeciesId])->getDecisionsBuildingBlock()->getMaximumVoracity(preyInstar));
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

	if (saveSnapshots)
	{
		saveWaterSnapshot(resultFolder / fs::path("Snapshots"), "Water_initial", TimeStep(0));

		for(ResourceSpecies*& resourceSpecies : existingResourceSpecies)
		{
			saveResourceSpeciesSnapshot(resultFolder / fs::path("Snapshots"), "Resource_initial", TimeStep(0), resourceSpecies);
		}
	}


	TimeStep numberOfTimeSteps = TimeStep(0);

	const TimeStep totalNumberOfTimeSteps(getRunDays(), getTimeStepsPerDay());


	TbbAdaptiveWrapper::Execute(landscapeAnimals, 0, landscapeAnimals.size(), 45u,
		[this](auto& animals, size_t start, size_t end) {
			for (size_t i = start; i < end; ++i) {
				animals[i]->initControlVariables(getExistingSpecies());
			}
		},
		[this](auto& animals, size_t start, size_t end) {
			tbb::parallel_for(tbb::blocked_range<size_t>(start, end),
				[this, &animals](const tbb::blocked_range<size_t>& r) {
					for (size_t i = r.begin(); i < r.end(); ++i) {
						animals[i]->initControlVariables(getExistingSpecies());
					}
				}
			);
		}
	);


	while(numberOfTimeSteps < totalNumberOfTimeSteps)
	{
		LogManager::emit(fmt::format("Running on timeStep {} out of {}\n", numberOfTimeSteps.getValue(), totalNumberOfTimeSteps.getValue()));

		if (saveMassInfo) {
			StorageBridge::writeHeaderPackToDisk(resultFolder / fs::path("massInfo") / fmt::format("mass_info_{}.txt", numberOfTimeSteps), "TimeStep\tId\tCurrentMass\tMoltingMassTarget\tReproductionMassTarget\tGrowthCurve\tMassPredicted\tCurrentAge\tMoltingAgeTarget\tReproductionAgeTarget");
		}

//#####################################################################
//##########################  UPDATING MAP   ##########################
//#####################################################################

		LogManager::emit(" - Updating map ... \n");
		
		auto t0 = chrono::high_resolution_clock::now();
		updateMap(numberOfTimeSteps);
		auto t1 = chrono::high_resolution_clock::now();

		LogManager::emit(fmt::format("Time: {} secs.\n", chrono::duration<double>(t1 - t0).count()));

		fmt::format_to(fmt::appender(timeSpentBuffer), FMT_COMPILE("{}\t"),
			chrono::duration<double>(t1 - t0).count()
		);
		
		LogManager::emit("DONE\n");

//#####################################################################
//#################  PRINTING ANIMALS ALONG CELLS   ###################
//#####################################################################

		LogManager::emit(" - Printing animals along cells ... \n");

		t0 = chrono::high_resolution_clock::now();
		printAnimalsAlongCells(numberOfTimeSteps, 0);
		t1 = chrono::high_resolution_clock::now();

		LogManager::emit(fmt::format("Time: {} secs.\n", chrono::duration<double>(t1 - t0).count()));

		fmt::format_to(fmt::appender(timeSpentBuffer), FMT_COMPILE("{}\t"),
			chrono::duration<double>(t1 - t0).count()
		);

		LogManager::emit("DONE\n");

//#####################################################################
//#######################  EXECUTING ACTIONS   ########################
//#####################################################################
		
		LogManager::emit(" - Executing actions ... \n");

		t0 = chrono::high_resolution_clock::now();
		executingActions(numberOfTimeSteps);
		t1 = chrono::high_resolution_clock::now();
		
		LogManager::emit(fmt::format("Time: {} secs.\n", chrono::duration<double>(t1 - t0).count()));

		fmt::format_to(fmt::appender(timeSpentBuffer), FMT_COMPILE("{}\t"),
			chrono::duration<double>(t1 - t0).count()
		);

		LogManager::emit("DONE\n");

//#####################################################################
//##########  BACKGROUND, ASSIMILATING FOOD & REPRODUCING   ###########
//#####################################################################

		LogManager::emit(" - Background, assimilating food and reproducing ... \n");
		
		t0 = chrono::high_resolution_clock::now();
		performAnimalsActions(numberOfTimeSteps);
		t1 = chrono::high_resolution_clock::now();

		LogManager::emit(fmt::format("Time: {} secs.\n", chrono::duration<double>(t1 - t0).count()));

		fmt::format_to(fmt::appender(timeSpentBuffer), FMT_COMPILE("{}\t"),
			chrono::duration<double>(t1 - t0).count()
		);

		LogManager::emit("DONE\n");

//#####################################################################
//#################  PRINTING ANIMALS ALONG CELLS   ###################
//#####################################################################

		LogManager::emit(" - Printing animals along cells ... \n");

		t0 = chrono::high_resolution_clock::now();
		printAnimalsAlongCells(numberOfTimeSteps, 1);
		t1 = chrono::high_resolution_clock::now();

		LogManager::emit(fmt::format("Time: {} secs.\n", chrono::duration<double>(t1 - t0).count()));

		fmt::format_to(fmt::appender(timeSpentBuffer), FMT_COMPILE("{}\t"),
			chrono::duration<double>(t1 - t0).count()
		);

		LogManager::emit("DONE\n");

//#####################################################################
//##################  PRINTING EXTENDED SUMMARY   #####################
//#####################################################################

		if(saveExtendedDailySummary)
		{
			LogManager::emit(" - Printing summary file ... \n");

			t0 = chrono::high_resolution_clock::now();
			printExtendedDailySummary(numberOfTimeSteps);
			t1 = chrono::high_resolution_clock::now();

			LogManager::emit(fmt::format("Time: {} secs.\n", chrono::duration<double>(t1 - t0).count()));

			fmt::format_to(fmt::appender(timeSpentBuffer), FMT_COMPILE("{}\t"),
				chrono::duration<double>(t1 - t0).count()
			);

			LogManager::emit("DONE\n");
		}

//#####################################################################
//####################  PURGING DEAD ANIMALS   ########################
//#####################################################################

		LogManager::emit(" - Purging dead animals ... \n");

		t0 = chrono::high_resolution_clock::now();
		purgeDeadAnimals();
		t1 = chrono::high_resolution_clock::now();

		LogManager::emit(fmt::format("Time: {} secs.\n", chrono::duration<double>(t1 - t0).count()));

		fmt::format_to(fmt::appender(timeSpentBuffer), FMT_COMPILE("{}"),
			chrono::duration<double>(t1 - t0).count()
		);

		LogManager::emit("DONE\n");

		printCellAlongCells(numberOfTimeSteps);

		
		if(savePredationEventsOnOtherSpecies)
		{
			printPredationEventsOnOtherSpeciesMatrix(numberOfTimeSteps);
		}


		if (saveSnapshots && (((numberOfTimeSteps + TimeStep(1)) % saveSnapshotsPeriodicity) == 0))
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


		timeSpentBuffer.push_back('\n');


		if (saveMassInfo) {
			StorageBridge::writeClosePackToDisk(resultFolder / fs::path("massInfo") / fmt::format("mass_info_{}.txt", numberOfTimeSteps));
		}


		++numberOfTimeSteps;


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


	if (saveSnapshots)
	{
		saveWaterSnapshot(resultFolder / fs::path("Snapshots"), "Water_final", numberOfTimeSteps);

		for(auto &resourceSpecies : existingResourceSpecies)
		{
			saveResourceSpeciesSnapshot(resultFolder / fs::path("Snapshots"), "Resource_final", numberOfTimeSteps, resourceSpecies);
		}

		for (AnimalSpecies*& animalSpecies : getMutableExistingAnimalSpecies())
		{
			saveAnimalSpeciesSnapshot(resultFolder / fs::path("Snapshots"), "Animal_final", numberOfTimeSteps, animalSpecies);
		}
	}



	auto end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> elapsed = end - start;

	fmt::format_to(fmt::appender(executionTimeBuffer), FMT_COMPILE("{} segs\n"), elapsed.count());

	StorageBridge::writeFullFilePackToDisk(resultFolder / "executionTime.txt", "", executionTimeBuffer);


	string timeStepHeader = "updateMap\tinitialPrintAnimalsAlongCells\texecutingActions\tperformAnimalsActions\tfinalPrintAnimalsAlongCells";

	if (saveExtendedDailySummary)
	{
		timeStepHeader.append("\tprintExtendedDailySummary");
	}

	timeStepHeader.append("\tpurgeDeadAnimals");

	StorageBridge::writeFullFilePackToDisk(resultFolder / "time_spent.txt", timeStepHeader, timeSpentBuffer);
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

	breakConditionsInfo << fmt::to_string(coefficientOfVariation) << "\t" << fmt::to_string(numberOfTimeSteps) << "\n";

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
		checkpointFilename.append(".bin");
		std::ofstream ofs((resultFolder / fs::path("checkpoints") / checkpointFilename).string());
		boost::archive::binary_oarchive oa(ofs);
		oa << this;
		ofs.close();
	}
	else
	{
		checkpointFilename.append(".txt");
		std::ofstream ofs((resultFolder / fs::path("checkpoints") / checkpointFilename).string());
		boost::archive::text_oarchive oa(ofs);
		oa << this;
		ofs.close();
	}
}

void Landscape::registerAnimal(AnimalNonStatistical* animal)
{
	std::lock_guard<std::mutex> lock(landscapeAnimalsMutex);

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

void Landscape::executingActions(const TimeStep& numberOfTimeSteps)
{
	auto& localRng = Random::getEngine();
	std::shuffle(landscapeAnimals.begin(), landscapeAnimals.end(), localRng);


	size_t totalElements = landscapeAnimals.size();

	ProgressBar progressBar(totalElements);

	chrono::duration<double> actionPlanningTime(0.0);
	chrono::duration<double> actionExecutionTime(0.0);


	string timeStepStr = string(MAX_NUM_DIGITS_DAY - to_string(numberOfTimeSteps.getValue()).length(), '0') + to_string(numberOfTimeSteps.getValue());


	if (saveActivity) {
		StorageBridge::writeHeaderPackToDisk(resultFolder / fs::path("animals_each_day_activity") / (std::string("animals_activity_day_") + timeStepStr + ".txt"), "id\tspecies\tactivityType\tinitialDay\tfinalDay\tactivityDuration");
	}
	

	if (saveAnimalsEachDayPredationProbabilities) {
		StorageBridge::writeHeaderPackToDisk(resultFolder / fs::path("animals_each_day_predationProbabilities") / (std::string("animals_predationProbabilities_day_") + timeStepStr + ".txt"), "randomProbability\tprobabilityToCompare\tretaliation\tidHunter\tidHunted\tspeciesHunter\tspeciesHunted\thuntedIsPredator\tmassHunter\tmassHunted\tsuccessfulKill");
	}


	tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Species::ID, unsigned int>>> predationEventsOnOtherSpeciesLocal(getExistingAnimalSpecies().size(), CustomIndexedVector<Species::ID, unsigned int>(getExistingSpecies().size(), 0u));

	tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>> maximumPatchEdibilityValueGlobalLocal(
		[&]() {
			CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> localVector(getExistingAnimalSpecies().size());

			for (size_t i = 0; i < getExistingAnimalSpecies().size(); ++i) {
				localVector[i] = CustomIndexedVector<Instar, PreciseDouble>(getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
			}

			return localVector;
		}
	);

	tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>> maximumPatchPredationRiskGlobalLocal(
		[&]() {
			CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> localVector(getExistingAnimalSpecies().size());

			for (size_t i = 0; i < getExistingAnimalSpecies().size(); ++i) {
				localVector[i] = CustomIndexedVector<Instar, PreciseDouble>(getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
			}

			return localVector;
		}
	);

	tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>> maximumPatchConspecificBiomassGlobalLocal(
		[&]() {
			CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>> localVector(getExistingAnimalSpecies().size());

			for (size_t i = 0; i < getExistingAnimalSpecies().size(); ++i) {
				localVector[i] = CustomIndexedVector<Instar, PreciseDouble>(getExistingAnimalSpecies()[i]->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);
			}

			return localVector;
		}
	);


	// 1. Obtener de forma segura el límite máximo de hilos concurrentes de la arena actual
	size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());


	// Reemplazamos el vector de booleanos por un contador de elementos activos
	size_t activeElements = totalElements;


	while (!progressBar.finished())
	{
		auto t0_actionPlanning = chrono::high_resolution_clock::now();

		// 2. Usamos enumerable_thread_specific solo para mantener vivo el búfer de cada hebra
		maximumPatchEdibilityValueGlobalLocal.clear();
		maximumPatchPredationRiskGlobalLocal.clear();
		maximumPatchConspecificBiomassGlobalLocal.clear();

		size_t estimatedSize = activeElements * 256;

		size_t estimatedPerThreadSize = estimatedSize / maxThreadsInArena;


		TbbAdaptiveWrapper::Execute(landscapeAnimals, 0, activeElements, 48u,
			[&](auto& animals, size_t start, size_t end) {
				fmt::memory_buffer& localStr = localBuffersEdibilities.local();

				if (getSaveEdibilitiesFile()) {
					localStr.reserve(estimatedSize);
				}

				CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumPatchEdibilityValueGlobal = maximumPatchEdibilityValueGlobalLocal.local();
				CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumPatchPredationRiskGlobal = maximumPatchPredationRiskGlobalLocal.local();
				CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumPatchConspecificBiomassGlobal = maximumPatchConspecificBiomassGlobalLocal.local();

				const bool saveEd = getSaveEdibilitiesFile();
				const auto& timeStepsPerDay = getTimeStepsPerDay();

				for (size_t i = start; i < end; ++i) {
#ifdef DEBUG
					auto t0 = chrono::high_resolution_clock::now();
#endif

					auto animalPtr = animals[i];

					auto speciesId = animalPtr->getAnimalSpeciesId();
					auto& maxEd = animalSpeciesMaximumPatchEdibilityValueGlobal[speciesId];
					auto& maxPred = animalSpeciesMaximumPatchPredationRiskGlobal[speciesId];
					auto& maxCons = animalSpeciesMaximumPatchConspecificBiomassGlobal[speciesId];

					animalPtr->actionPlanning(
						this,
						numberOfTimeSteps,
						timeStepsPerDay,
						saveEd,
						localStr,
						maxEd,
						maxPred,
						maxCons
					);

#ifdef DEBUG
					auto t1 = chrono::high_resolution_clock::now();

					if (chrono::duration<double>(t1 - t0).count() > exitTimeThreshold)
					{
						throwLineInfoException("too many animals for too little food!!!");
					}
#endif
				}
			},
			[&](auto& animals, size_t start, size_t end) {
				tbb::parallel_for(tbb::blocked_range<size_t>(start, end),
					[&](const tbb::blocked_range<size_t>& r) {
						fmt::memory_buffer& localStr = localBuffersEdibilities.local();

						if (getSaveEdibilitiesFile()) {
							localStr.reserve(estimatedPerThreadSize);
						}

						CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumPatchEdibilityValueGlobal = maximumPatchEdibilityValueGlobalLocal.local();
						CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumPatchPredationRiskGlobal = maximumPatchPredationRiskGlobalLocal.local();
						CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, PreciseDouble>>& animalSpeciesMaximumPatchConspecificBiomassGlobal = maximumPatchConspecificBiomassGlobalLocal.local();

						const bool saveEd = getSaveEdibilitiesFile();
						const auto& timeStepsPerDay = getTimeStepsPerDay();

						for (size_t i = r.begin(); i < r.end(); ++i) {
#ifdef DEBUG
							auto t0 = chrono::high_resolution_clock::now();
#endif

							auto animalPtr = animals[i];

							auto speciesId = animalPtr->getAnimalSpeciesId();
							auto& maxEd = animalSpeciesMaximumPatchEdibilityValueGlobal[speciesId];
							auto& maxPred = animalSpeciesMaximumPatchPredationRiskGlobal[speciesId];
							auto& maxCons = animalSpeciesMaximumPatchConspecificBiomassGlobal[speciesId];

							animalPtr->actionPlanning(
								this,
								numberOfTimeSteps,
								timeStepsPerDay,
								saveEd,
								localStr,
								maxEd,
								maxPred,
								maxCons
							);

#ifdef DEBUG
							auto t1 = chrono::high_resolution_clock::now();

							if (chrono::duration<double>(t1 - t0).count() > exitTimeThreshold)
							{
								throwLineInfoException("too many animals for too little food!!!");
							}
#endif
						}
					}
				);
			}
		);


		for (auto& animalSpeciesMaximumPatchEdibilityValueGlobal : maximumPatchEdibilityValueGlobalLocal) {
			for (size_t i = 0; i < animalSpeciesMaximumPatchEdibilityValueGlobal.size(); ++i) {
				getMutableExistingAnimalSpecies()[i]->getMutableDecisionsBuildingBlock()->updateMaximumPatchEdibilityValueGlobal(animalSpeciesMaximumPatchEdibilityValueGlobal[i]);
			}
		}

		for (auto& animalSpeciesMaximumPatchPredationRiskGlobal : maximumPatchPredationRiskGlobalLocal) {
			for (size_t i = 0; i < animalSpeciesMaximumPatchPredationRiskGlobal.size(); ++i) {
				getMutableExistingAnimalSpecies()[i]->getMutableDecisionsBuildingBlock()->updateMaximumPatchPredationRiskGlobal(animalSpeciesMaximumPatchPredationRiskGlobal[i]);
			}
		}

		for (auto& animalSpeciesMaximumPatchConspecificBiomassGlobal : maximumPatchConspecificBiomassGlobalLocal) {
			for (size_t i = 0; i < animalSpeciesMaximumPatchConspecificBiomassGlobal.size(); ++i) {
				getMutableExistingAnimalSpecies()[i]->getMutableDecisionsBuildingBlock()->updateMaximumPatchConspecificBiomassGlobal(animalSpeciesMaximumPatchConspecificBiomassGlobal[i]);
			}
		}

		auto t1_actionPlanning = chrono::high_resolution_clock::now();

		actionPlanningTime += chrono::duration<double>(t1_actionPlanning - t0_actionPlanning);


		auto t0_actionExecution = chrono::high_resolution_clock::now();


		// =================================================================
		// 2. EL ALGORITMO CLAVE: PARTADO TRIPLE (Three-Way Partition)
		// =================================================================
		// Reorganizamos el vector en 3 bloques contiguos en tiempo O(N) secuencial rápido
		size_t predateEnd = 0;
		size_t parallelEnd = activeElements;
		size_t i = 0;

		while (i < parallelEnd) {
			auto action = landscapeAnimals[i]->getNextAction();

			if (action == AnimalNonStatistical::Action::PREDATE) {
				if (i != predateEnd) {
					std::swap(landscapeAnimals[i], landscapeAnimals[predateEnd]);
				}
				predateEnd++;
				i++;
			}
			else if (action == AnimalNonStatistical::Action::NONE) {
				// Mandamos los 'NONE' al final de la zona activa
				parallelEnd--;
				std::swap(landscapeAnimals[i], landscapeAnimals[parallelEnd]);
				// No incrementamos 'i' porque el elemento intercambiado desde 'parallelEnd' debe ser evaluado
			}
			else {
				// Es una acción común paralelizable
				i++;
			}
		}

		// Calculamos cuántos terminaron sin hacer nada (NONE) en esta iteración
		size_t nonesCount = activeElements - parallelEnd;
		if (nonesCount > 0) {
			progressBar.update(nonesCount);
		}


		if (predateEnd > 0) {
			// =================================================================
			// 3. FASE SECUENCIAL: Ejecución de Predación
			// =================================================================
			// Eliminado el 'if' dentro del bucle. Iteramos estrictamente sobre el Bloque 1 [0 ... predateEnd)
			CustomIndexedVector<AnimalSpeciesID, uint64_t> animalSpeciesMaximumPredationEncountersPerDay(getExistingAnimalSpecies().size(), 0u);

			size_t predateEstimatedPerThreadSize = predateEnd * 256;


			for (size_t k = 0; k < predateEnd; k++) {
				fmt::memory_buffer& predationProbabilitiesLocalStr = localBuffersPredationProbabilities.local();
				CustomIndexedVector<Species::ID, unsigned int>& predationEventsOnOtherSpecies = predationEventsOnOtherSpeciesLocal.local()[landscapeAnimals[k]->getSpecies()->getAnimalSpeciesId()];

				if (saveAnimalsEachDayPredationProbabilities) {
					predationProbabilitiesLocalStr.reserve(predateEstimatedPerThreadSize);
				}

				landscapeAnimals[k]->predate(false, saveAnimalsEachDayPredationProbabilities, predationProbabilitiesLocalStr,
					this, numberOfTimeSteps, getTimeStepsPerDay(), getCompetitionAmongResourceSpecies(), predationEventsOnOtherSpecies,
					animalSpeciesMaximumPredationEncountersPerDay);
			}

			for (size_t idx = 0; idx < animalSpeciesMaximumPredationEncountersPerDay.size(); ++idx) {
				getExistingAnimalSpecies()[idx]->updateMaximumPredationEncountersPerDay(animalSpeciesMaximumPredationEncountersPerDay[idx]);
			}
		}


		if (predateEnd < parallelEnd) {
			// =================================================================
			// 4. FASE PARALELA: Ejecución de Acciones Propias
			// =================================================================
			// Eliminado el 'if' de predicado. Iteramos estrictamente sobre el Bloque 2 [predateEnd ... parallelEnd)

			size_t nonPredateEstimatedSize = ((parallelEnd - predateEnd) * 256);

			size_t nonPredateEstimatedPerThreadSize = nonPredateEstimatedSize / maxThreadsInArena;


			TbbAdaptiveWrapper::Execute(landscapeAnimals, predateEnd, parallelEnd, 3u,
				[&](auto& animals, size_t start, size_t end) {
					fmt::memory_buffer& activitiesLocalStr = localBuffersActivities.local();
					fmt::memory_buffer& movementsLocalStr = localBuffersMovements.local();

					if (saveActivity) {
						activitiesLocalStr.reserve(nonPredateEstimatedSize);
					}

					if (saveMovements) {
						movementsLocalStr.reserve(nonPredateEstimatedSize);
					}

					for (size_t i = start; i < end; ++i) {
						animals[i]->actionExecution(this, saveActivity, activitiesLocalStr,
							numberOfTimeSteps, getTimeStepsPerDay(), saveMovements, movementsLocalStr);
					}
				},
				[&](auto& animals, size_t start, size_t end) {
					tbb::parallel_for(tbb::blocked_range<size_t>(start, end),
						[&](const tbb::blocked_range<size_t>& r) {
							fmt::memory_buffer& activitiesLocalStr = localBuffersActivities.local();
							fmt::memory_buffer& movementsLocalStr = localBuffersMovements.local();

							if (saveActivity) {
								activitiesLocalStr.reserve(nonPredateEstimatedPerThreadSize);
							}

							if (saveMovements) {
								movementsLocalStr.reserve(nonPredateEstimatedPerThreadSize);
							}

							for (size_t i = r.begin(); i < r.end(); ++i) {
								animals[i]->actionExecution(this, saveActivity, activitiesLocalStr,
									numberOfTimeSteps, getTimeStepsPerDay(), saveMovements, movementsLocalStr);
							}
						}
					);
				}
			);
		}


		// El nuevo límite de animales activos es igual a los que NO terminaron
		activeElements = parallelEnd;


		auto t1_actionExecution = chrono::high_resolution_clock::now();

		actionExecutionTime += chrono::duration<double>(t1_actionExecution - t0_actionExecution);
	}


	if (getSaveEdibilitiesFile()) {
		// 4. Enviar un paquete por cada hebra usando std::move (Cero copias, contención mínima)
		for (fmt::memory_buffer& threadBuffer : localBuffersEdibilities) {
			if (threadBuffer.size() != 0) {
				StorageBridge::writeContentPackToDisk(resultFolder / std::string("edibilities.txt"), threadBuffer);
			}
		}
	}



	if (saveActivity) {
		// 4. Enviar un paquete por cada hebra usando std::move (Cero copias, contención mínima)
		for (fmt::memory_buffer& threadBuffer : localBuffersActivities) {
			if (threadBuffer.size() != 0) {
				StorageBridge::writeContentPackToDisk(resultFolder / fs::path("animals_each_day_activity") / (std::string("animals_activity_day_") + timeStepStr + ".txt"), threadBuffer);
			}
		}
	}

	if (saveMovements) {
		// 4. Enviar un paquete por cada hebra usando std::move (Cero copias, contención mínima)
		for (fmt::memory_buffer& threadBuffer : localBuffersMovements) {
			if (threadBuffer.size() != 0) {
				StorageBridge::writeContentPackToDisk(resultFolder / (std::string("movements.txt")), threadBuffer);
			}
		}
	}

	if (saveAnimalsEachDayPredationProbabilities) {
		// 4. Enviar un paquete por cada hebra usando std::move (Cero copias, contención mínima)
		for (fmt::memory_buffer& threadBuffer : localBuffersPredationProbabilities) {
			if (threadBuffer.size() != 0) {
				StorageBridge::writeContentPackToDisk(resultFolder / fs::path("animals_each_day_predationProbabilities") / (std::string("animals_predationProbabilities_day_") + timeStepStr + ".txt"), threadBuffer);
			}
		}
	}


	TbbAdaptiveWrapper::Execute(landscapeAnimals, 0, landscapeAnimals.size(), 1321u,
		[](auto& animals, size_t start, size_t end) {
			for (size_t i = start; i < end; ++i) {
				animals[i]->updateTimeStepsWithoutFood();
			}
		},
		[](auto& animals, size_t start, size_t end) {
			tbb::parallel_for(tbb::blocked_range<size_t>(start, end),
				[&](const tbb::blocked_range<size_t>& r) {
					for (size_t i = r.begin(); i < r.end(); ++i) {
						animals[i]->updateTimeStepsWithoutFood();
					}
				}
			);
		}
	);



	if (saveActivity) {
		StorageBridge::writeClosePackToDisk(resultFolder / fs::path("animals_each_day_activity") / (std::string("animals_activity_day_") + timeStepStr + ".txt"));
	}

	if (saveAnimalsEachDayPredationProbabilities) {
		StorageBridge::writeClosePackToDisk(resultFolder / fs::path("animals_each_day_predationProbabilities") / (std::string("animals_predationProbabilities_day_") + timeStepStr + ".txt"));
	}


	for (const auto& threadPredationEventsOnOtherSpecies : predationEventsOnOtherSpeciesLocal) {
		for (size_t i = 0; i < threadPredationEventsOnOtherSpecies.size(); ++i) {
			getExistingAnimalSpecies()[i]->addPredationEventOnOtherSpecies(threadPredationEventsOnOtherSpecies[i]);
		}
	}


	LogManager::emit("   - Action planning ... \n");
	LogManager::emit(fmt::format("Time: {} secs.\n", actionPlanningTime.count()));

	LogManager::emit("   - Action execution ... \n");
	LogManager::emit(fmt::format("Time: {} secs.\n", actionExecutionTime.count()));
}

void Landscape::performAnimalsActions(const TimeStep numberOfTimeSteps)
{
	fs::path voracitiesFilePath = resultFolder / fs::path("animals_each_day_voracities") / fmt::format("animals_voracities_day_{}.txt", numberOfTimeSteps);

	if (saveAnimalsEachDayVoracities)
	{
		StorageBridge::writeHeaderPackToDisk(voracitiesFilePath, "id\tspecies\tstate\tcurrentAge\tinstar\tmature\tbody_size\tenergy_tank\tdryMass\tcurrentWetMass\ttankAtGrowth\tnextDinoMass\tmaxVoracityTimeStep\tmin_mass_for_death\tvoracity_ini\tpreT_search\tpreT_speed\tafter_encounters_voracity\tafter_encounters_search\tfinal_speed\texpectedDryMassFromMaxVor\tfood_mass\tdryMassAfterAssim\ttotalMetabolicDryMassLossAfterAssim\tmaxSearchArea\teatenToday\th\tsteps\tstepsAttempted\tafter_encounters_search\tsated\tpercentMoving\tvoracity_body_mass_ratio\tgender\tmated\teggDryMass\tK\tfactorEggMass\tdeath_date\tageOfFirstMaturation\treproCounter");
	}

	size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());
	size_t estimatedSize = (landscapeAnimals.size() * 256);
	size_t estimatedPerThreadSize = estimatedSize / maxThreadsInArena;

	tbb::enumerable_thread_specific<CustomIndexedVector<AnimalSpeciesID, unsigned int>> animalSpeciesPopulationLocal(getExistingAnimalSpecies().size(), 0u);

	// Invocamos el Wrapper adaptativo
	TbbAdaptiveWrapper::Execute(landscapeAnimals, 0, landscapeAnimals.size(), 19u,
		// === OPERACIÓN SECUENCIAL (SeqOp) ===
		[&](auto& animals, size_t start, size_t end) {
			// En entorno puramente secuencial, accedemos directamente de forma segura
			fmt::memory_buffer& voracitiesLocalStr = localBuffersVoracities.local();
			CustomIndexedVector<AnimalSpeciesID, unsigned int>& animalSpeciesPopulation = animalSpeciesPopulationLocal.local();
			CustomIndexedVector<AnimalSpeciesID, fmt::memory_buffer>& animalConstitutiveTraitsLocalStr = localBuffersAnimalConstitutiveTraits.local();
			CustomIndexedVector<AnimalSpeciesID, std::vector<fmt::memory_buffer>>& animalSpeciesGeneticsLocalStr = localBuffersGenetics.local();

			if (saveAnimalsEachDayVoracities) {
				voracitiesLocalStr.reserve(estimatedSize);
			}

			for (size_t i = start; i < end; ++i) {
				auto animalPtr = animals[i];

				AnimalSpeciesID id = animalPtr->getSpecies()->getAnimalSpeciesId();
				fmt::memory_buffer& animalSpeciesTraitsLocalStr = animalConstitutiveTraitsLocalStr[id];
				std::vector<fmt::memory_buffer>& geneticsLocalStr = animalSpeciesGeneticsLocalStr[id];
				unsigned int& population = animalSpeciesPopulation[id];

				if (getSaveAnimalConstitutiveTraits()) {
					animalSpeciesTraitsLocalStr.reserve(estimatedSize);
				}
				if (saveGenetics) {
					geneticsLocalStr.reserve(estimatedSize);
				}

				if (animalPtr->getLifeStage() == LifeStage::ACTIVE) {
					animalPtr->printVoracities(this, voracitiesLocalStr, getTimeStepsPerDay());
					animalPtr->dieFromBackground(this, numberOfTimeSteps, getTimeStepsPerDay(), isGrowthAndReproTest());
				}
				if (animalPtr->getLifeStage() == LifeStage::ACTIVE) {
					animalPtr->transferAssimilatedFoodToEnergyTank(numberOfTimeSteps);
					animalPtr->metabolize(this, numberOfTimeSteps);
				}
				if (animalPtr->getLifeStage() == LifeStage::REPRODUCING) {
					if (animalPtr->isInBreedingZone()) {
						population += animalPtr->breed(this, numberOfTimeSteps, saveGenetics, geneticsLocalStr, getTimeStepsPerDay(), getSaveAnimalConstitutiveTraits(), animalSpeciesTraitsLocalStr);
						animalPtr->setInBreedingZone(false);
					}
				}
				if (animalPtr->getLifeStage() == LifeStage::ACTIVE) {
					animalPtr->checkEnergyTank(this, numberOfTimeSteps, getTimeStepsPerDay());
				}
			}
		},
		// === OPERACIÓN PARALELA (ParOp) ===
		[&](auto& animals, size_t start, size_t end) {
			tbb::parallel_for(tbb::blocked_range<size_t>(start, end),
				[&](const tbb::blocked_range<size_t>& r) {
					// CADA HILO obtiene su propia referencia local de forma segura dentro del rango asignado
					fmt::memory_buffer& voracitiesLocalStr = localBuffersVoracities.local();
					CustomIndexedVector<AnimalSpeciesID, unsigned int>& animalSpeciesPopulation = animalSpeciesPopulationLocal.local();
					CustomIndexedVector<AnimalSpeciesID, fmt::memory_buffer>& animalConstitutiveTraitsLocalStr = localBuffersAnimalConstitutiveTraits.local();
					CustomIndexedVector<AnimalSpeciesID, std::vector<fmt::memory_buffer>>& animalSpeciesGeneticsLocalStr = localBuffersGenetics.local();

					if (saveAnimalsEachDayVoracities) {
						voracitiesLocalStr.reserve(estimatedPerThreadSize);
					}

					for (size_t i = r.begin(); i < r.end(); ++i) {
						auto animalPtr = animals[i];

						AnimalSpeciesID id = animalPtr->getSpecies()->getAnimalSpeciesId();
						fmt::memory_buffer& animalSpeciesTraitsLocalStr = animalConstitutiveTraitsLocalStr[id];
						std::vector<fmt::memory_buffer>& geneticsLocalStr = animalSpeciesGeneticsLocalStr[id];
						unsigned int& population = animalSpeciesPopulation[id];

						if (getSaveAnimalConstitutiveTraits()) {
							animalSpeciesTraitsLocalStr.reserve(estimatedPerThreadSize);
						}
						if (saveGenetics) {
							geneticsLocalStr.reserve(estimatedPerThreadSize);
						}

						if (animalPtr->getLifeStage() == LifeStage::ACTIVE) {
							animalPtr->printVoracities(this, voracitiesLocalStr, getTimeStepsPerDay());
							animalPtr->dieFromBackground(this, numberOfTimeSteps, getTimeStepsPerDay(), isGrowthAndReproTest());
						}
						if (animalPtr->getLifeStage() == LifeStage::ACTIVE) {
							animalPtr->transferAssimilatedFoodToEnergyTank(numberOfTimeSteps);
							animalPtr->metabolize(this, numberOfTimeSteps);
						}
						if (animalPtr->getLifeStage() == LifeStage::REPRODUCING) {
							if (animalPtr->isInBreedingZone()) {
								population += animalPtr->breed(this, numberOfTimeSteps, saveGenetics, geneticsLocalStr, getTimeStepsPerDay(), getSaveAnimalConstitutiveTraits(), animalSpeciesTraitsLocalStr);
								animalPtr->setInBreedingZone(false);
							}
						}
						if (animalPtr->getLifeStage() == LifeStage::ACTIVE) {
							animalPtr->checkEnergyTank(this, numberOfTimeSteps, getTimeStepsPerDay());
						}
					}
				}
			);
		}
	);


	// Reducción y escritura a disco (se mantiene igual a tu lógica original)
	if (saveAnimalsEachDayVoracities)
	{
		for (fmt::memory_buffer& threadBuffer : localBuffersVoracities) {
			if (threadBuffer.size() != 0) {
				StorageBridge::writeContentPackToDisk(voracitiesFilePath, threadBuffer);
			}
		}
		StorageBridge::writeClosePackToDisk(voracitiesFilePath);
	}

	if (getSaveAnimalConstitutiveTraits())
	{
		for (CustomIndexedVector<AnimalSpeciesID, fmt::memory_buffer>& threadBuffer : localBuffersAnimalConstitutiveTraits) {
			for (unsigned int i = 0; i < threadBuffer.size(); ++i) {
				if (threadBuffer[i].size() != 0) {
					StorageBridge::writeContentPackToDisk(animalConstitutiveTraitsFilePath[i], threadBuffer[i]);
				}
			}
		}
	}

	if (saveGenetics)
	{
		for (auto& threadBuffer : localBuffersGenetics) {
			for (size_t i = 0; i < threadBuffer.size(); ++i) {
				std::vector<fmt::memory_buffer>& traitBuffer = threadBuffer[i];
				AnimalSpecies* animalSpecies = getExistingAnimalSpecies()[i];
				std::string scientificNameReplaced = animalSpecies->getScientificNameReplaced();
				const std::vector<IndividualLevelTrait*>& individualLevelTraits = animalSpecies->getGenetics().getIndividualLevelTraits();

				for (size_t j = 0; j < traitBuffer.size(); ++j) {
					if (traitBuffer[j].size() != 0) {
						IndividualLevelTrait* trait = individualLevelTraits[j];
						StorageBridge::writeContentPackToDisk(resultFolder / fs::path("genetics") / scientificNameReplaced / (trait->getFileName() + ".txt"), traitBuffer[j]);
					}
				}
			}
		}
	}

	for (const auto& threadAnimalSpeciesPopulation : animalSpeciesPopulationLocal) {
		for (size_t i = 0; i < threadAnimalSpeciesPopulation.size(); ++i) {
			getExistingAnimalSpecies()[i]->increasePopulation(threadAnimalSpeciesPopulation[i]);
		}
	}
}

void Landscape::purgeDeadAnimals()
{
	CustomIndexedVector<AnimalSpeciesID, unsigned int> animalSpeciesDeadPopulation(getExistingAnimalSpecies().size(), 0u);

	auto it = landscapeAnimals.begin();

	while(it != landscapeAnimals.end())
	{
		if((*it)->getLifeStage() == LifeStage::STARVED || (*it)->getLifeStage() == LifeStage::PREDATED ||
			(*it)->getLifeStage() == LifeStage::BACKGROUND || (*it)->getLifeStage() == LifeStage::SENESCED ||
			(*it)->getLifeStage() == LifeStage::SHOCKED)
		{
			++animalSpeciesDeadPopulation[(*it)->getSpecies()->getAnimalSpeciesId()];
			(*it)->getMutableTerrainCell()->eraseAnimal((*it));
			delete (*it);

			it = landscapeAnimals.erase(it);
		}
		else
		{
			it++;
		}
	}

	for (size_t i = 0; i < animalSpeciesDeadPopulation.size(); ++i) {
		getExistingAnimalSpecies()[i]->decreasePopulation(animalSpeciesDeadPopulation[i]);
	}
}

bool Landscape::isExtinguished() const
{
	bool extinctionStatus = false; 

	vector<double> landscapeResourceBiomass(getExistingResourceSpecies().size());
	CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<LifeStage, unsigned int>> landscapeAnimalsPopulation(getExistingAnimalSpecies().size(), CustomIndexedVector<LifeStage, unsigned int>(EnumClass<LifeStage>::size(), 0));


	landscapeMap->obtainResourcesBiomass(landscapeResourceBiomass);

	for(size_t i = 0; i < landscapeAnimals.size(); i++)
	{
		landscapeAnimalsPopulation[landscapeAnimals[i]->getSpecies()->getAnimalSpeciesId()][landscapeAnimals[i]->getLifeStage()]++;
	}


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
	LogManager::emit("Initializing terrain voxels ...\n");

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

	LogManager::emit("DONE\n");
}

bool Landscape::isDinosaurs() const
{
    return false;
}

void Landscape::readObstaclePatchesFromJSONFiles(const fs::path& configPath)
{
	LogManager::emit("Reading obstacle patches from JSON files ... \n");

	PatchPriorityQueue obstaclePatchesToAplly;

	fs::path obstacleFolder = configPath / OBSTACLE_FOLDER_NAME;

	if (fs::exists(obstacleFolder) && fs::is_directory(obstacleFolder))
	{
		JsonValidator obstaclePatchesValidator(EmbeddedResources::obstacle_patch_schema_json, "obstacle_patch_schema");

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
	LogManager::emit("Reading habitat domain patches from JSON files ... \n");

	PatchPriorityQueue habitatDomainPatchesToAplly;

	fs::path habitatDomainFolder = configPath / HABITAT_DOMAIN_FOLDER_NAME;

	if (fs::exists(habitatDomainFolder) && fs::is_directory(habitatDomainFolder))
	{
		JsonValidator habitatDomainPatchesValidator(EmbeddedResources::habitat_domain_patch_schema_json, "habitat_domain_patch_schema");

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
	LogManager::emit("Reading moisture patches from JSON files ... \n");

	PatchPriorityQueue moisturePatchesToAplly;

	fs::path moistureFolder = configPath / MOISTURE_FOLDER_NAME;

	if (fs::exists(moistureFolder) && fs::is_directory(moistureFolder))
	{
		JsonValidator moisturePatchesValidator(EmbeddedResources::moisture_patch_schema_json, "moisture_patch_schema");

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
	LogManager::emit(fmt::format("{}\n\n", patch.getDescription()));

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

PreciseDouble Landscape::getMinExploitableResource() const
{
	return minExploitableResource;
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

const vector<AnimalNonStatistical*>& Landscape::getLandscapeAnimals() const
{
	return landscapeAnimals;
}

vector<AnimalNonStatistical*>& Landscape::getLandscapeAnimals()
{
	return landscapeAnimals;
}

const std::vector<TerrainCell*>& Landscape::getLandscapeTerrainCells() const
{
	return terrainCells;
}

std::vector<TerrainCell*>& Landscape::getLandscapeTerrainCells() 
{
	return terrainCells;
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
				predator = (*potentialPredators[predatorInstar])[predatorIndex];
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
				prey = animalsPopulation[static_cast<AnimalSpecies*>(getExistingSpecies()[speciesIt->first])->getAnimalSpeciesId()][preyInstar][preyIndex];
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
	LogManager::emit(fmt::format("Size of the Animal class: {}\n", sizeof(Animal)));
	LogManager::emit(fmt::format("Size of the Genome class: {}\n", sizeof(Genome)));
	LogManager::emit(fmt::format("Size of the TerrainCell class: {}\n", sizeof(TerrainCell)));
	LogManager::emit("Creating heating code individuals... \n");

	vector<CustomIndexedVector<Instar, vector<AnimalStatistical*>>> animalsPopulation;
	unsigned int populationSize = landscapeMap->generateStatisticsPopulation(animalsPopulation, this, getMutableExistingAnimalSpecies(), mapSpeciesInhabitableTerrainCells, TimeStep(0), getTimeStepsPerDay());
	
	LogManager::emit(fmt::format("A total of {} heating code individuals have been created.\n", populationSize));

	//Only for predators. The experiment is carried out for every predator species and its linked species, up until the specified numberOfCombinations.

	LogManager::emit("Calculating attack statistics: \n");
	for(AnimalSpecies*& predatorAnimalSpecies : getMutableExistingAnimalSpecies())
	{
		LogManager::emit(fmt::format(">> Simulating {} attacks from the species \"{}\"... \n", numberOfCombinations, predatorAnimalSpecies->getScientificName()));

		size_t numberOfTotalPotentialPredators = 0u;
		CustomIndexedVector<Instar, vector<AnimalStatistical*>*> potentialPredators(predatorAnimalSpecies->getGrowthBuildingBlock().getNumberOfInstars(), nullptr);

		CustomIndexedVector<Instar, size_t> numberOfPotentialPreysPerInstar(predatorAnimalSpecies->getGrowthBuildingBlock().getNumberOfInstars(), 0);

		for(const Instar &predatorInstar : predatorAnimalSpecies->getGrowthBuildingBlock().getInstarsRange())
		{
			if(predatorAnimalSpecies->eatsAnimalSpecies(predatorInstar))
			{
				potentialPredators[predatorInstar] = &animalsPopulation[predatorAnimalSpecies->getAnimalSpeciesId()][predatorInstar];
				numberOfTotalPotentialPredators += potentialPredators[predatorInstar]->size();

				for(const auto &[preySpecies, speciesInstars] : predatorAnimalSpecies->getInstarEdibleAnimalSpecies(predatorInstar))
				{
					for(const Instar &preyInstar : speciesInstars)
					{
						numberOfPotentialPreysPerInstar[predatorInstar] += animalsPopulation[static_cast<AnimalSpecies*>(getExistingSpecies()[preySpecies])->getAnimalSpeciesId()][preyInstar].size();
					}
				}
			}
		}

		if(numberOfTotalPotentialPredators > 0)
		{
			CustomIndexedVector<Instar, PreciseDouble> maximumInteractionArea(predatorAnimalSpecies->getGrowthBuildingBlock().getNumberOfInstars(), 0.0);

			ProgressBar progressBar(numberOfCombinations);

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
						predatorAnimalSpecies->interactionRanges(*predator, *prey, maximumInteractionArea);

						progressBar.update();
					}
				}
			}

			predatorAnimalSpecies->updateMaximumInteractionArea(maximumInteractionArea);
		}
		else
		{
			ProgressBar progressBar(1u);

			progressBar.update();
		}
	}

	eraseStatisticsPopulation(animalsPopulation);

	LogManager::emit("Calculating attack statistics DONE\n");
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

				delete animal;
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
	LogManager::emit("Giving life to animals... \n");

	for(AnimalSpecies*& animalSpecies : getMutableExistingAnimalSpecies())
	{
		if(initialPopulation[animalSpecies->getAnimalSpeciesId()].size() > 0)
    	{
			unsigned int totalInitialPopulation = 0;
			for(const auto& instarPopulation : initialPopulation[animalSpecies->getAnimalSpeciesId()])
			{
				totalInitialPopulation += instarPopulation;
			}


			// 1. Obtener de forma segura el límite máximo de hilos concurrentes de la arena actual
			size_t maxThreadsInArena = static_cast<size_t>(tbb::this_task_arena::max_concurrency());

			// 2. Calcular la estimación del búfer basada en la concurrencia real de la arena
			size_t estimatedPerThreadSize = (totalInitialPopulation * 256) / std::max<size_t>(1, maxThreadsInArena);


			LogManager::emit(fmt::format("Creating {} individuals of the species \"{}\"...\n", totalInitialPopulation, animalSpecies->getScientificName()));


			AnimalSpeciesID id = animalSpecies->getAnimalSpeciesId();

			fmt::memory_buffer& animalConstitutiveTraitsLocalStr = localBuffersAnimalConstitutiveTraits.local()[id];

			std::vector<fmt::memory_buffer>& geneticsLocalStr = localBuffersGenetics.local()[id];

			if (getSaveAnimalConstitutiveTraits())
			{
				if (animalConstitutiveTraitsLocalStr.capacity() == 0) {
					animalConstitutiveTraitsLocalStr.reserve(estimatedPerThreadSize + (totalInitialPopulation * 256));
				}
				else if (animalConstitutiveTraitsLocalStr.capacity() < animalConstitutiveTraitsLocalStr.size() + (totalInitialPopulation * 256)) {
					// Salvaguarda por si un hilo recibe más carga de la estimada promedialmente
					animalConstitutiveTraitsLocalStr.reserve(animalConstitutiveTraitsLocalStr.size() + (totalInitialPopulation * 256));
				}
			}

			if (saveGenetics) {
				if (geneticsLocalStr.capacity() == 0) {
					geneticsLocalStr.reserve(estimatedPerThreadSize + (totalInitialPopulation * 256));
				}
				else if (geneticsLocalStr.capacity() < geneticsLocalStr.size() + (totalInitialPopulation * 256)) {
					// Salvaguarda por si un hilo recibe más carga de la estimada promedialmente
					geneticsLocalStr.reserve(geneticsLocalStr.size() + (totalInitialPopulation * 256));
				}
			}

			for (size_t i = 0; i < totalInitialPopulation; ++i) {
				Instar instar;
				unsigned int prevPopulationSum = 0;

				while (i >= prevPopulationSum && i < prevPopulationSum + initialPopulation[animalSpecies->getAnimalSpeciesId()][instar]) {
					prevPopulationSum += initialPopulation[animalSpecies->getAnimalSpeciesId()][instar];
					instar.moveOnNextInstar();
				}


				// Get a random index from this species inhabitable cells
				size_t randomCellIndex = Random::randomIndex(mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar].size());

				const Genome* genome;

				if (initialGenomesPool.empty()) {
					genome = nullptr;
				}
				else {
					genome = &initialGenomesPool[animalSpecies->getAnimalSpeciesId()][i];
				}


				auto newTerrainCell = (*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar][randomCellIndex])->randomInsertAnimal(this, instar, animalSpecies, false, genome, saveGenetics, geneticsLocalStr, TimeStep(0), timeStepsPerDay);

				if (get<0>(newTerrainCell))
				{
					bool found = false;
					for (unsigned int index = 0; index < mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar].size() && !found; ++index)
					{
						if ((*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar][index]) == get<1>(newTerrainCell))
						{
							(*mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar][index]) = get<2>(newTerrainCell);
							found = true;
						}
					}
				}

				AnimalNonStatistical* newAnimal = static_cast<AnimalNonStatistical*>(get<3>(newTerrainCell));

				newAnimal->calculateGrowthCurves(timeStepsPerDay);
				newAnimal->forceMolting(this, TimeStep(0), timeStepsPerDay);

				if (saveAnimalConstitutiveTraits)
				{
					newAnimal->printTraits(animalConstitutiveTraitsLocalStr);
				}
			}


			animalSpecies->increasePopulation(totalInitialPopulation);
		}
	}


	if (getSaveAnimalConstitutiveTraits())
	{
		for (CustomIndexedVector<AnimalSpeciesID, fmt::memory_buffer>& threadBuffer : localBuffersAnimalConstitutiveTraits) {
			for (unsigned int i = 0; i < threadBuffer.size(); ++i) {
				if (threadBuffer[i].size() != 0) {
					StorageBridge::writeContentPackToDisk(animalConstitutiveTraitsFilePath[i], threadBuffer[i]);
				}
			}
		}
	}


	if (saveGenetics)
	{
		for (auto& threadBuffer : localBuffersGenetics) {
			for (size_t i = 0; i < threadBuffer.size(); ++i) {
				std::vector<fmt::memory_buffer>& traitBuffer = threadBuffer[i];
				AnimalSpecies* animalSpecies = getExistingAnimalSpecies()[i];

				std::string scientificNameReplaced = animalSpecies->getScientificNameReplaced();

				const std::vector<IndividualLevelTrait*>& individualLevelTraits = animalSpecies->getGenetics().getIndividualLevelTraits();

				for (size_t j = 0; j < traitBuffer.size(); ++j) {
					if (traitBuffer[j].size() != 0) {
						IndividualLevelTrait* trait = individualLevelTraits[j];


						StorageBridge::writeContentPackToDisk(resultFolder / fs::path("genetics") / scientificNameReplaced / (trait->getFileName() + ".txt"), traitBuffer[j]);
					}
				}
			}
		}
	}

	LogManager::emit("DONE\n");
}

void Landscape::initializeOutputFiles(fs::path configPath)
{
	fmt::format_to(fmt::appender(versionsBuffer),
		"PROGRAM_VERSION:{}\nSCHEMA_VERSION:{}\nSERIALIZATION_VERSION:{}\n",
		WEAVER_PROGRAM_VERSION, WEAVER_SCHEMA_VERSION, WEAVER_SERIALIZATION_VERSION
	);

	StorageBridge::writeFullFilePackToDisk(resultFolder / fs::path("versions.txt"), "", versionsBuffer);

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

		animalConstitutiveTraitsFilePath.resize(getExistingAnimalSpecies().size());


		std::string animalConstitutiveTraitsHeader;

		animalConstitutiveTraitsHeader.append("id\tspecies");

		for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
		{
			animalConstitutiveTraitsHeader.append(fmt::format("\t{}", magic_enum::enum_names<Axis>()[axis]));
		}

		animalConstitutiveTraitsHeader.append("\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\tdateEgg");


		for(const auto &animalSpecies : getExistingAnimalSpecies())
		{
			std::string header = animalConstitutiveTraitsHeader;

			for (Trait::ExecutionOrder order : EnumClass<Trait::ExecutionOrder>::getEnumValues())
			{
				for (Trait* trait : animalSpecies->getMutableGenetics().getAllTraits()[order])
				{
					if (trait->getValue()->getType() == IndividualLevelTrait::Type::IndividualLevel)
					{
						fmt::format_to(std::back_inserter(header), "\t{}",
							static_cast<IndividualLevelTrait*>(trait->getValue())->getTraitStr()
						);
					}
				}
			}

			std::filesystem::path filePath = resultFolder / fs::path("animal_constitutive_traits") / (animalSpecies->getScientificNameReplaced() + ".txt");

			animalConstitutiveTraitsFilePath[animalSpecies->getAnimalSpeciesId()] = filePath;

			StorageBridge::writeHeaderPackToDisk(filePath, header);
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

	if(saveExtendedDailySummary)
	{
		std::string header = "day";

		for (auto itResourceSpecies = existingResourceSpecies.begin(); itResourceSpecies != existingResourceSpecies.end(); itResourceSpecies++)
		{
			fmt::format_to(std::back_inserter(header), "\t{}_biomass",
				(*itResourceSpecies)->getScientificNameReplaced()
			);
		}

		for (const AnimalSpecies* const& animalSpecies : getExistingAnimalSpecies())
		{
			for (const LifeStage& lifeStage : EnumClass<LifeStage>::getEnumValues())
			{
				fmt::format_to(std::back_inserter(header), "\t{}_{}",
					animalSpecies->getScientificNameReplaced(),
					EnumClass<LifeStage>::to_string(lifeStage)
				);
			}
		}

		StorageBridge::writeHeaderPackToDisk(resultFolder / fs::path("extendedDailySummary.txt"), header);
	}

	///////////////////////////////////////////////////////////////////////////

	if(saveMovements)
	{
		StorageBridge::writeHeaderPackToDisk(resultFolder / (std::string("movements.txt")), "timeStep\tid\tstartPointX\tstartPointY\tendPointX\tendPointY\tdistanceTravelled\tsearchAreaRadius\texhausted");
	}

	///////////////////////////////////////////////////////////////////////////
		
	if(saveEdibilitiesFile)
	{
		StorageBridge::writeHeaderPackToDisk(resultFolder / (std::string("edibilities.txt")), "timeStep\tsearcherId\tsearcherSpecies\tfoodMass\tpredatorId\tpredatorSpecies\tpredatorDryMass\tpredatedId\tpredatedSpecies\tpredatedDryMass\tpredationProbability\tedibility\tpreference\texperience");
	}

	///////////////////////////////////////////////////////////////////////////

	if(isCheckpointsEnabled())
	{
		fs::create_directories(resultFolder / fs::path("checkpoints"));
	}
}

id_type Landscape::generateEdibleId()
{
	return edibleIdCounter++;
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
		if (serializationVersion != WEAVER_SERIALIZATION_VERSION) {
			throwLineInfoException(
				"Version mismatch: serialized data was created with version " + serializationVersion +
				", but current serialization version is " + WEAVER_SERIALIZATION_VERSION + "."
			);
		}
	}

	ar & edibleIdCounter;
	ar & resourceIdCounter;
	ar & animalIdCounter;

	ar & appliedMoisture;

	ar & printAnimalsAlongCellsHeader;

	ar & printCellAlongCellsHeader;

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

