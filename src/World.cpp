#include "World.h"
/*
#include "tbb/tbb.h"
*/
#include "SimulationConstants.h"
#include <thread>
#include <algorithm> // transform
#include "Utilities.h"
#include "GlobalVariable.h"

using namespace std;

/*
using namespace tbb;
*/

SimulType::SimulType(const string& typeStr) : value(stringToEnumValue(typeStr)) {};

const unordered_map<string_view, const SimulType::SimulTypeValue> SimulType::generateMap() 
{
	unordered_map<string_view, const SimulType::SimulTypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const SimulType::SimulTypeValue simulType = static_cast<const SimulType::SimulTypeValue>(i);
		enumMap.insert({to_string(simulType), simulType});
	}

	return enumMap;
}

const unordered_map<string_view, const SimulType::SimulTypeValue> SimulType::stringToEnum = SimulType::generateMap();

const string SimulType::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<SimulTypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string SimulType::enumValues = SimulType::generateAvailableValues();


constexpr size_t World::ResourceType::size() 
{ 
	return magic_enum::enum_count<ResourceTypeValue>(); 
}

std::string_view World::ResourceType::to_string(const World::ResourceType::ResourceTypeValue& resourceType) 
{ 
	return magic_enum::enum_name(resourceType); 
}

const unordered_map<string_view, const World::ResourceType::ResourceTypeValue> World::ResourceType::generateMap() 
{
	unordered_map<string_view, const ResourceTypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const ResourceTypeValue resourceType = static_cast<const ResourceTypeValue>(i);
		enumMap.insert({to_string(resourceType), resourceType});
	}

	return enumMap;
}

const unordered_map<string_view, const World::ResourceType::ResourceTypeValue> World::ResourceType::stringToEnum = World::ResourceType::generateMap();

const string World::ResourceType::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<ResourceTypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string World::ResourceType::enumValues = World::ResourceType::generateAvailableValues();

std::string_view World::ResourceType::printAvailableValues() 
{ 
	return enumValues; 
}

World::ResourceType::ResourceTypeValue World::ResourceType::stringToEnumValue(const std::string &str) 
{ 
	try
	{
		return stringToEnum.at(str);
	}
	catch(const std::out_of_range& e) 
	{
		throwLineInfoException(fmt::format("Unknown simulation type '{}'. Valid values are {}", str, printAvailableValues()));
	}
}



Terrain3D World::terrain = Terrain3D();


World::World(json * jsonTree, fs::path FolderI, fs::path inputFile, fs::path outputFolder, fs::path WeaverFolder) : WeaverFolder(WeaverFolder)
{
	string configFolder;
	try
	{
		configFolder = (*jsonTree)["simulation"].at("configFolder");
	}
	catch(const json::out_of_range& e)
	{
		configFolder = CONFIG_FOLDER;
	}
	string configName = (*jsonTree)["simulation"]["configName"];
	inputFolder = FolderI / fs::path(configFolder) / fs::path(configName);

	json worldConfig = readConfigFile1(inputFolder / fs::path("world_params.json"), WeaverFolder);

	simulType = SimulType(string(worldConfig["world"]["simulationType"]));

	switch (simulType) {
		case SimulType::dinosaurs: {
			massRatio = 1000.0; //in kg for dinosaurs
			break;
		}
		case SimulType::arthropods: {
			massRatio = 0.001; //in mg for arthropods
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}

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
	


	setResourceToPreysCapacityTransference(worldConfig["world"]["spaceConfiguration"]["resourceToPreysCapacityTransference"]);
	setPreysToPredatorsCapacityTransference(worldConfig["world"]["spaceConfiguration"]["preysToPredatorsCapacityTransference"]);

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

	setDepth(worldConfig["world"]["spaceConfiguration"]["dimensions"]["depth"]);
	setLength(worldConfig["world"]["spaceConfiguration"]["dimensions"]["length"]);
	setWidth(worldConfig["world"]["spaceConfiguration"]["dimensions"]["width"]);
	setObstacleFolderName(fs::path(worldConfig["world"]["spaceConfiguration"]["obstacleFolder"]));
	setMoistureFolderName(fs::path(worldConfig["world"]["spaceConfiguration"]["moistureFolder"]));
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

	setCellSize(worldConfig["world"]["spaceConfiguration"]["dimensions"]["cellSize"]);

	// TODO Mario Initialize animals with initIndividualsPerDensities
	/*
	setHeatingCodeTemperatureCycle(worldConfig["world"]["spaceConfiguration"]["temperatureFilename"]); // By default, 20 Celsius degrees
	*/

	readResourceSpeciesFromJSONFiles();
	readAnimalSpeciesFromJSONFiles();

	initializeTerrainDimensions(worldConfig["world"]["spaceConfiguration"]["moistureBasePatch"]);

	readObstaclePatchesFromJSONFiles();
	readMoisturePatchesFromJSONFiles();

	initializeResource();
	//initializeAnimals();

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

	initializeOutputFiles(jsonTree, inputFile);
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
	eraseTerrain();

	for(auto elem : existingResourceSpecies) {
		delete elem;
	}
	existingResourceSpecies.clear();

	for(auto elem : existingAnimalSpecies) {
		delete elem;
	}
	existingAnimalSpecies.clear();
}

void World::readAnimalSpeciesFromJSONFiles()
{
	Output::cout("Reading all animal species from JSON files ... \n");
	
	fs::path speciesFolder = inputFolder / speciesFolderName;
	fs::directory_iterator end_iter;
	if (fs::exists(speciesFolder) && fs::is_directory(speciesFolder))
	{
		vector<fs::path> filePaths;
		copy(fs::directory_iterator(speciesFolder), fs::directory_iterator(), back_inserter(filePaths));
		sort(filePaths.begin(), filePaths.end());             // directory iteration is not ordered on some file systems, so we sort them
		json ptMain;
		for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(SPECIES_SCHEMA));

				Output::cout(" - Animal scientific name: {}\n", (string)ptMain["animal"]["name"]);

				AnimalSpecies* newSpecies = new AnimalSpecies(
					ptMain["animal"]["name"], ptMain["animal"]["genetics"]["numberOfLociPerTrait"], 
					ptMain["animal"]["genetics"]["numberOfAllelesPerLocus"], ptMain["animal"]["genetics"]["restrictPerTrait"], 
					ptMain["animal"]["genetics"]["correlationCoefficientRhoPerModule"], ptMain["animal"]["genetics"]["traitsPerModule"], 
					ptMain["animal"]["genetics"]["numberOfChiasmasPerChromosome"], ptMain["animal"]["traits"]["fixedTraits"], ptMain["animal"]["traits"]["variableTraits"]["minTraitsRanges"], 
					ptMain["animal"]["traits"]["variableTraits"]["maxTraitsRanges"], ptMain["animal"]["traits"]["variableTraits"]["minTraitLimits"], 
					ptMain["animal"]["traits"]["variableTraits"]["maxTraitLimits"], ptMain["animal"]["traits"]["variableTraits"]["order"], ptMain["animal"]["sexualType"],
					ptMain["animal"]["defaultHuntingMode"], ptMain["animal"]["growthCurve"]
				);
				

				//Added for new growth_curves
				if(initIndividualsPerDensities == false)
				{
					newSpecies->setInitialPopulation(ptMain["animal"]["individualsPerInstar"]);
				}
				newSpecies->setStatisticsInitialPopulation(ptMain["animal"]["statisticsIndividualsPerInstar"]);

				unsigned int numberOfInstars = newSpecies->getInitialPopulation().size();
				Output::cout("numberOfInstars: {}\n", numberOfInstars);

				newSpecies->setAssignedForMolt(ptMain["animal"]["assignedForMolt"]);
				newSpecies->setBetaScaleTank(ptMain["animal"]["betaScaleTank"]);
				newSpecies->setExcessInvestInSize(ptMain["animal"]["excessInvestInSize"]);
				newSpecies->setNumberOfInstars(numberOfInstars);
				newSpecies->setPupaPeriodLength(ptMain["animal"]["pupaPeriodLength"]);
				newSpecies->setMinRelativeHumidityThreshold(ptMain["animal"]["minRelativeHumidityThreshold"]);
				newSpecies->setMaxEncountersT(ptMain["animal"]["maxEncountersT"]);

				//Added for new growth_curves
				newSpecies->setDevTimeVector(ptMain["animal"]["devTimeVector"]);
				newSpecies->setLinfKcorr(ptMain["animal"]["LinfKcorr"]);
				newSpecies->setDevTimeConstant(ptMain["animal"]["devTimeConstant"]);
				newSpecies->setLongevitySinceMaturation(ptMain["animal"]["longevitySinceMaturation"]);
				newSpecies->setReproTimeFactor(ptMain["animal"]["reproTimeFactor"]);
				newSpecies->setTempOptGrowth(ptMain["animal"]["tempOptGrowth"]);
				newSpecies->setTempOptVoracity(ptMain["animal"]["tempOptVoracity"]);
				newSpecies->setTempOptSearch(ptMain["animal"]["tempOptSearch"]);
				newSpecies->setTempOptSpeed(ptMain["animal"]["tempOptSpeed"]);
				newSpecies->setEdGrowth(ptMain["animal"]["EdGrowth"]);
				newSpecies->setEdVoracity(ptMain["animal"]["EdVoracity"]);
				newSpecies->setEdSearch(ptMain["animal"]["EdSearch"]);
				newSpecies->setEdSpeed(ptMain["animal"]["EdSpeed"]);
				newSpecies->setDevInter(ptMain["animal"]["devInter"]);
				newSpecies->setFractSearchExtremeT(ptMain["animal"]["fractSearchExtremeT"]);
				newSpecies->setFractSpeedExtremeT(ptMain["animal"]["fractSpeedExtremeT"]);
				newSpecies->setMinVorExtremeT(ptMain["animal"]["minVorExtremeT"]);

				newSpecies->setTempFromLab(ptMain["animal"]["tempFromLab"]);
				newSpecies->setTempSizeRuleConstant(ptMain["animal"]["tempSizeRuleConstant"]);

				newSpecies->setExperienceInfluencePerDay(ptMain["animal"]["experienceInfluencePerDay"]);

				newSpecies->setMassInfo(ptMain["animal"]["conversionToWetMass"], ptMain["animal"]["eggClutchFromEquation"],
										ptMain["animal"]["forClutchMassCoefficient"], ptMain["animal"]["forClutchMassScale"],
										ptMain["animal"]["forEggMassCoefficient"], ptMain["animal"]["forEggMassScale"],
										ptMain["animal"]["eggDryMass"], ptMain["animal"]["femaleWetMass"], 
										ptMain["animal"]["eggMassFromEquation"]);

				newSpecies->setScaleForVoracity(ptMain["animal"]["scaleForVoracity"]);
				newSpecies->setScaleForSearchArea(ptMain["animal"]["scaleForSearchArea"]);
				newSpecies->setScaleForSpeed(ptMain["animal"]["scaleForSpeed"]);
				newSpecies->setMaxPlasticityKVonBertalanffy(ptMain["animal"]["maxPlasticityKVonBertalanffy"]);
				newSpecies->setMinPlasticityKVonBertalanffy(ptMain["animal"]["minPlasticityKVonBertalanffy"]);
				newSpecies->setMaxPlasticityDueToConditionVor(ptMain["animal"]["maxPlasticityDueToConditionVor"]);
				newSpecies->setMinPlasticityDueToConditionVor(ptMain["animal"]["minPlasticityDueToConditionVor"]);
				newSpecies->setMaxPlasticityDueToConditionSearch(ptMain["animal"]["maxPlasticityDueToConditionSearch"]);
				newSpecies->setMinPlasticityDueToConditionSearch(ptMain["animal"]["minPlasticityDueToConditionSearch"]);
				newSpecies->setMaxPlasticityDueToConditionSpeed(ptMain["animal"]["maxPlasticityDueToConditionSpeed"]);
				newSpecies->setMinPlasticityDueToConditionSpeed(ptMain["animal"]["minPlasticityDueToConditionSpeed"]);

				newSpecies->setAttackProbability(ptMain["animal"]["attackProbability"]);
				newSpecies->setExposedAttackProbability(ptMain["animal"]["exposedAttackProbability"]);
				newSpecies->setKillProbability(ptMain["animal"]["killProbability"]);
				newSpecies->setOptimalTemperatureModifier(ptMain["animal"]["optimalTemperatureModifier"]);

				newSpecies->setDaysWithoutFoodForMetabolicDownregulation(ptMain["animal"]["daysWithoutFoodForMetabolicDownregulation"]);
				newSpecies->setPercentageMetabolicDownregulation(ptMain["animal"]["percentageMetabolicDownregulation"]);
				newSpecies->setPercentageCostForMetabolicDownregulationVoracity(ptMain["animal"]["percentageCostForMetabolicDownregulationVoracity"]);
				newSpecies->setPercentageCostForMetabolicDownregulationSearchArea(ptMain["animal"]["percentageCostForMetabolicDownregulationSearchArea"]);
				newSpecies->setPercentageCostForMetabolicDownregulationSpeed(ptMain["animal"]["percentageCostForMetabolicDownregulationSpeed"]);

				newSpecies->setCellEvaluationBiomass(ptMain["animal"]["cellEvaluationBiomass"]);
				newSpecies->setCellEvaluationRisk(ptMain["animal"]["cellEvaluationRisk"]);
				newSpecies->setCellEvaluationProConspecific(ptMain["animal"]["cellEvaluationProConspecific"]);
				newSpecies->setCellEvaluationAntiConspecific(ptMain["animal"]["cellEvaluationAntiConspecific"]);
				newSpecies->setConspecificWeighing(ptMain["animal"]["conspecificWeighing"]);

				newSpecies->setCoefficientForMassA(ptMain["animal"]["coefficientForMassA"]);
				newSpecies->setScaleForMassB(ptMain["animal"]["scaleForMassB"]);

				newSpecies->setCoefficientForMassAforMature(ptMain["animal"]["coefficientForMassAforMature"]);
				newSpecies->setScaleForMassBforMature(ptMain["animal"]["scaleForMassBforMature"]);

				newSpecies->setForDensitiesA(ptMain["animal"]["forDensitiesA"]);
				newSpecies->setForDensitiesB(ptMain["animal"]["forDensitiesB"]);

				newSpecies->setIndeterminateGrowth(ptMain["animal"]["indeterminateGrowth"]);
				if(ptMain["animal"]["indeterminateGrowth"]) {
					newSpecies->setInstarFirstReproduction(Instar(ptMain["animal"]["instarFirstReproduction"]));
				}
				else {
					newSpecies->setInstarFirstReproduction(Instar(ptMain["animal"]["individualsPerInstar"].size()));
				}
				newSpecies->setInstarsForNextReproduction(ptMain["animal"]["instarsForNextReproduction"]);

				newSpecies->setSexRatio(ptMain["animal"]["sexRatio"]);
				newSpecies->setFemaleMaxReproductionEvents(ptMain["animal"]["femaleMaxReproductionEvents"]);
				newSpecies->setEggsPerBatch(ptMain["animal"]["eggsPerBatch"]);
				newSpecies->setMaleMaxReproductionEvents(ptMain["animal"]["maleMaxReproductionEvents"]);
				newSpecies->setMaleMobility(ptMain["animal"]["maleMobility"]);
				newSpecies->setQuitCellThreshold(ptMain["animal"]["quitCellThreshold"]);
				newSpecies->setDecreaseOnTraitsDueToEncounters(ptMain["animal"]["decreaseOnTraitsDueToEncounters"]);
				newSpecies->setMaleReproductionFactor(ptMain["animal"]["maleReproductionFactor"]);
				newSpecies->setProbabilityDeathFromBackground(ptMain["animal"]["probabilityDeathFromBackground"]);

				addAnimalSpecies(newSpecies);
			}
		}

		Output::cout("DONE\n\n");
		Output::cout("Assigning links between species from JSON files edible species information ... \n");

		//Who eats whom
		for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(SPECIES_SCHEMA));

				string scientificName = ptMain["animal"]["name"];
				Output::cout("Animal species {} eats: \n", scientificName);

				for( auto elem : ptMain["animal"]["edibleResourceSpecies"])
				{
					string speciesToBeAddedName = elem;
					ResourceSpecies* resourceSpeciesToBeAdded = getResourceSpecies(speciesToBeAddedName);
					if (resourceSpeciesToBeAdded == NULL)
					{
						cerr << "Trying to add an edible resource species that does not exist. Please check the species name or contact developers." << endl;
						exit(-1);
					}
					Output::cout(" - Resource: {}\n", speciesToBeAddedName);
					getAnimalSpecies(scientificName)->addEdibleResourceSpecies(resourceSpeciesToBeAdded);
				}

				int i = 0;
				for( auto elem : ptMain["animal"]["edibleResourcePreferences"])
				{
					float edibleResourcePreferenceToBeAdded = elem;
					Species* resourceSpeciesToBeAdded = getAnimalSpecies(scientificName)->getEdibleResourceSpecies()->at(i);

					Output::cout(" - Resource: {} -> Preference: {}\n", resourceSpeciesToBeAdded->getScientificName(), edibleResourcePreferenceToBeAdded);
					getAnimalSpecies(scientificName)->addEdiblePreference(resourceSpeciesToBeAdded, edibleResourcePreferenceToBeAdded);
					++i;
				}

				i = 0;
				for( auto elem : ptMain["animal"]["edibleResourceProfitability"])
				{
					float edibleResourceProfitabilityToBeAdded = elem;
					Species* resourceSpeciesToBeAdded = getAnimalSpecies(scientificName)->getEdibleResourceSpecies()->at(i);

					Output::cout(" - Resource: {} -> Profitability: {}\n", resourceSpeciesToBeAdded->getScientificName(), edibleResourceProfitabilityToBeAdded);
					getAnimalSpecies(scientificName)->addEdibleProfitability(resourceSpeciesToBeAdded, edibleResourceProfitabilityToBeAdded);
					++i;
				}

				for( auto elem : ptMain["animal"]["edibleAnimalSpecies"])
				{
					string speciesToBeAddedName = elem;
					AnimalSpecies* animalSpeciesToBeAdded = getAnimalSpecies(speciesToBeAddedName);
					if (animalSpeciesToBeAdded == NULL)
					{
						cerr << "Trying to add an edible animal species that does not exist. Please check the species name or contact developers." << endl;
						exit(-1);
					}
					Output::cout(" - Animal: {}\n", speciesToBeAddedName);
					getAnimalSpecies(scientificName)->addEdibleAnimalSpecies(animalSpeciesToBeAdded);
				}

				i = 0;
				for( auto elem : ptMain["animal"]["edibleAnimalPreferences"])
				{
					float edibleAnimalPreferenceToBeAdded = elem;
					AnimalSpecies* animalSpeciesToBeAdded = getAnimalSpecies(scientificName)->getEdibleAnimalSpecies()->at(i);

					Output::cout(" - Animal: {} -> Preference: {}\n", animalSpeciesToBeAdded->getScientificName(), edibleAnimalPreferenceToBeAdded);
					getAnimalSpecies(scientificName)->addEdiblePreference(animalSpeciesToBeAdded, edibleAnimalPreferenceToBeAdded);
					++i;
				}

				i = 0;
				for( auto elem : ptMain["animal"]["edibleAnimalProfitability"])
				{
					float edibleAnimalProfitabilityToBeAdded = elem;
					AnimalSpecies* animalSpeciesToBeAdded = getAnimalSpecies(scientificName)->getEdibleAnimalSpecies()->at(i);

					Output::cout(" - Animal: {} -> Profitability: {}\n", animalSpeciesToBeAdded->getScientificName(), edibleAnimalProfitabilityToBeAdded);
					getAnimalSpecies(scientificName)->addEdibleProfitability(animalSpeciesToBeAdded, edibleAnimalProfitabilityToBeAdded);
					++i;
				}

				Output::cout("\n");
			}
		}
	}
	else
	{
		cerr << "The specified path \"" + speciesFolder.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}

	Output::cout("DONE\n\n");

	for(auto speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
	{
		(*speciesIt)->setInitialPredationEventsOnOtherSpecies(existingAnimalSpecies.size() + existingResourceSpecies.size());
	}

}

void World::readResourceSpeciesFromJSONFiles()
{
	Output::cout("Reading all resource species from JSON files ... \n");
	
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

				Output::cout(" - Resource scientific name: {}\n\n", (string)ptMain["resource"]["name"]);

				ResourceSpecies * newSpecies;

				
				switch(ResourceType::stringToEnumValue(ptMain["resource"]["resourceType"])) {
					case ResourceType::BasalResource: {
						newSpecies = new BasalResourceSpecies(ptMain["resource"]["name"], ptMain["resource"]["ACTIVATION_ENERGY"], 
							ptMain["resource"]["NORMALIZATION_B"], ptMain["resource"]["patchesFolder"]
						);
						break;
					}
					case ResourceType::NoBasalResource: {
						newSpecies = new NoBasalResourceSpecies(ptMain["resource"]["name"], ptMain["resource"]["ACTIVATION_ENERGY"], 
							ptMain["resource"]["NORMALIZATION_B"], ptMain["resource"]["patchesFolder"]
						);
						break;
					}
					default: {
						throwLineInfoException("Default case");
						break;
					}
				}
				
				newSpecies->setCellMass(ptMain["resource"]["cellMass"]);
				newSpecies->setConversionToWetMass(ptMain["resource"]["conversionToWetMass"]);
				newSpecies->setGrowingRateParams(ptMain["resource"]["minHR"], ptMain["resource"]["maxHR"], ptMain["resource"]["maxRScale"], ptMain["resource"]["variableIntrinsicRateOfIncrease"]);
				newSpecies->setMinimumEdibleBiomass(ptMain["resource"]["minimumNumberOfEdibleSpores"], ptMain["resource"]["cellMass"]);

				addResourceSpecies(newSpecies);
			}
		}
	}
	else
	{
		cerr << "The specified path \"" + resourceFolder.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}
}

void World::initializeResource()
{
	Output::cout("Reading all resource patches from JSON files ... \n");
	
	fs::directory_iterator end_iter;

	for (auto itResourceSpecies = existingResourceSpecies.begin(); itResourceSpecies != existingResourceSpecies.end(); itResourceSpecies++)
	{
		ResourceSpecies* currentResourceSpecies = *itResourceSpecies;
		Output::cout(" - Resource scientific name: {}\n\n", currentResourceSpecies->getScientificName());
		fs::path patchesFolder = inputFolder / resourceFolderName / fs::path(currentResourceSpecies->getPatchesFolderName());
		json ptMain;
		if (fs::exists(patchesFolder) && fs::is_directory(patchesFolder))
		{
			vector<fs::path> filePaths;
			copy(fs::directory_iterator(patchesFolder), fs::directory_iterator(), back_inserter(filePaths));
			sort(filePaths.begin(), filePaths.end());             // directory iteration is not ordered on some file systems, so we sort them

			for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
			{
				if (it->extension() == ".json")
				{
					string patchFilename = it->string();
					Output::cout("{}\n", patchFilename);

					ptMain.clear();

					// Read configuration file
					ptMain = readConfigFile(*it, WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(PATCH_SCHEMA));

					string type = ptMain["patch"]["type"];

					double resourceMaximumCapacity = ptMain["patch"]["resourceMaximumCapacity"];

					if (type == "homogeneous")
					{
						float value = ptMain["patch"]["value"];
						setHomogeneousResource(currentResourceSpecies, value, resourceMaximumCapacity);
					}
					else if (type == "gaussian")
					{
						unsigned int radius = ptMain["patch"]["radius"];
						unsigned int x = ptMain["patch"]["xPos"];
						unsigned int y = ptMain["patch"]["yPos"];
						unsigned int z = ptMain["patch"]["zPos"];
						float sigma = ptMain["patch"]["sigma"];
						float amplitude = ptMain["patch"]["amplitude"];
						setGaussianResourcePatch(currentResourceSpecies, x, y, z, radius, sigma, amplitude, resourceMaximumCapacity);
					}
					else if (type == "sphere")
					{
						unsigned int radius = ptMain["patch"]["radius"];
						unsigned int x = ptMain["patch"]["xPos"];
						unsigned int y = ptMain["patch"]["yPos"];
						unsigned int z = ptMain["patch"]["zPos"];
						float value = ptMain["patch"]["value"];
						setSphericalResourcePatch(currentResourceSpecies, x, y, z, radius, value, resourceMaximumCapacity);
					}
					else if (type == "cubic")
					{
						Output::cout("CUBICOOOOO\n\n\n");

						Coordinate3D<int> patchCenter(
							ptMain["patch"]["center"]["x"], 
							ptMain["patch"]["center"]["y"], 
							ptMain["patch"]["center"]["z"]
						);
						Coordinate3D<int> patchDimensions(
							ptMain["patch"]["dimensions"]["x"], 
							ptMain["patch"]["dimensions"]["y"], 
							ptMain["patch"]["dimensions"]["z"]
						);

						setCubicResourcePatch(currentResourceSpecies, patchCenter, patchDimensions, ptMain["patch"]["value"], resourceMaximumCapacity);

					}
					else if (type == "randomGaussian")
					{
						bool useRandomAmplitude = ptMain["patch"]["useRandomAmplitude"];
						bool useRandomSigma = ptMain["patch"]["useRandomSigma"];
						unsigned int numberOfPatches = ptMain["patch"]["numberOfPatches"];
						unsigned int patchesRadius = ptMain["patch"]["patchesRadius"];
						float maxSigma = ptMain["patch"]["maxSigma"];
						float maxAmplitude = ptMain["patch"]["maxAmplitude"];

						setRandomGaussianResourcePatches(currentResourceSpecies, numberOfPatches, patchesRadius, maxSigma, useRandomSigma, maxAmplitude, useRandomAmplitude, resourceMaximumCapacity);
					}
				}
			}
		}
		else
		{
			cerr << "The specified path \"" + patchesFolder.string() + "\" does not exist or it is not a directory" << endl;
			exit(-1);
		}
	}
}

void World::addResourceSpecies(ResourceSpecies * newSpecies)
{
	if (std::find(existingResourceSpecies.begin(), existingResourceSpecies.end(), newSpecies) == existingResourceSpecies.end())
	{
		existingResourceSpecies.push_back(newSpecies);
	}
	else
	{
		throwLineInfoException("Error, the " + newSpecies->getScientificName() + " was already added to this world");
	}
}

void World::addAnimalSpecies(AnimalSpecies * newSpecies)
{
	if (std::find(existingAnimalSpecies.begin(), existingAnimalSpecies.end(), newSpecies)
			== existingAnimalSpecies.end())
	{
		existingAnimalSpecies.push_back(newSpecies);
	}
	else
	{
		throwLineInfoException("Error, the " + newSpecies->getScientificName() + " was already added to this world");
	}
}

ResourceSpecies * World::getResourceSpecies(string name)
{
	for (auto it = existingResourceSpecies.begin(); it != existingResourceSpecies.end(); it++)
	{
		if ((*it)->getScientificName() == name)
		{
			return (*it);
		}
	}

	return NULL;
}

AnimalSpecies * World::getAnimalSpecies(const string& name)
{
	for (auto it = existingAnimalSpecies.begin(); it != existingAnimalSpecies.end(); it++)
	{
		if ((*it)->getScientificName() == name)
		{
			return (*it);
		}
	}

	return NULL;
}

void World::printAnimalsAlongCells(int day, int simulationPoint)
{
	if(((day%recordEach)==0) | (day==0)){
	
		string pathBySimulationPoint;
		if (simulationPoint == 0)
		{
			pathBySimulationPoint = "animals_each_day_start";
		}
		else if (simulationPoint == 1)
		{
			pathBySimulationPoint = "animals_each_day_end";
		}
		
		FILE* file = nullptr;
		createOutputFile(&file, outputFolder / fs::path(pathBySimulationPoint), "animals_day_", "txt", day, recordEach);
		if (!isFileOpen(file))
		{
			Output::cerr("Error opening the file.\n");
		}
		else
		{
			Output::print(file, "id\tspecies\tgender\tx\ty\tz\tstate\tinstar\tpheno_ini\tdate_egg\tage_first_rep\trep_count\tfecundity\tdate_death\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\tencounters_pred\tglobal_pred_encs\tdays_digest\tvor_ini\tsearch_ini\tspeed_ini\ttank_ini\tpheno_ini\tcurrentBodySize\tcurrentDryMass\t{}\n", Trait::printAvailableTraits());
			TerrainCell* cellToPrintFrom;
			for (unsigned int z = 0; z < depth; z++)
			{
				for (unsigned int y = 0; y < length; y++)
				{
					for (unsigned int x = 0; x < width; x++)
					{
						cellToPrintFrom = getCell(z,y,x);
						if(!cellToPrintFrom->isObstacle())
						{
							getCell(z,y,x)->printAnimals(file);
						}
					}
				}
			}
			fclose(file);
		}
	}
}

 void World::printCellAlongCells(int day)
 {
	if(((day%recordEach)==0) | (day==0)){

		string pathBySimulationPoint = "cells_each_day";
		
		FILE* file = nullptr;
		createOutputFile(&file, outputFolder / fs::path(pathBySimulationPoint), "cells_day_", "txt", day, recordEach);
		if(!isFileOpen(file))
		{
			Output::cerr("Error opening the file.\n");
		}
		else
		{
			Output::print(file, "x\ty\tz\t");

			for (auto itResourceSpecies = existingResourceSpecies.begin(); itResourceSpecies != existingResourceSpecies.end(); itResourceSpecies++)
			{
				Output::print(file, "{}\t", (*itResourceSpecies)->getScientificName());
			}

			for (auto itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
			{
				Output::print(file, "{}\t", (*itSpecies)->getScientificName());
			}
			Output::print(file, "\n");

			TerrainCell* cellToPrintFrom;
			for (unsigned int z = 0; z < depth; z++)
			{
				for (unsigned int y = 0; y < length; y++)
				{
					for (unsigned int x = 0; x < width; x++)
					{
						cellToPrintFrom = getCell(z,y,x);
						if(!cellToPrintFrom->isObstacle())
						{
							getCell(z,y,x)->printCell(file);
						}
					}
				}
			}
			fclose(file);
		}
	}
 }


void World::printDailySummary(FILE* file, int day)
{
	double biomass = 0;
	int numberOfUnbornPreys = 0, numberOfActivePreys = 0, numberOfStarvedPreys = 0, numberOfPredatedPreys = 0,
			numberOfReproducingPreys = 0, numberOfSenescedPreys = 0, numberOfShockedPreys = 0;
	int numberOfUnbornPredators = 0, numberOfActivePredators = 0, numberOfStarvedPredators = 0,
			numberOfPredatedPredators = 0, numberOfReproducingPredators = 0, numberOfBackgroundPredators = 0,
			numberOfSenescedPredators = 0, numberOfShockedPredators = 0;

	for (unsigned int z = 0; z < depth; z++)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				biomass += getCell(z,y,x)->getTotalResourceBiomass();

				numberOfUnbornPreys += getCell(z,y,x)->getLifeStagePopulation(LifeStage::UNBORN,
						HuntingMode::does_not_hunt);
				numberOfActivePreys += getCell(z,y,x)->getLifeStagePopulation(LifeStage::ACTIVE,
						HuntingMode::does_not_hunt);
				numberOfStarvedPreys += getCell(z,y,x)->getLifeStagePopulation(LifeStage::STARVED,
						HuntingMode::does_not_hunt);
				numberOfPredatedPreys += getCell(z,y,x)->getLifeStagePopulation(LifeStage::PREDATED,
						HuntingMode::does_not_hunt);
				numberOfReproducingPreys += getCell(z,y,x)->getLifeStagePopulation(LifeStage::REPRODUCING,
						HuntingMode::does_not_hunt);
				numberOfSenescedPreys += getCell(z,y,x)->getLifeStagePopulation(LifeStage::SENESCED,
						HuntingMode::does_not_hunt);
				numberOfShockedPreys += getCell(z,y,x)->getLifeStagePopulation(LifeStage::SHOCKED,
						HuntingMode::does_not_hunt);

				numberOfUnbornPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::UNBORN,
						HuntingMode::active_hunting);
				numberOfUnbornPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::UNBORN,
						HuntingMode::sit_and_wait);
				numberOfActivePredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::ACTIVE,
						HuntingMode::active_hunting);
				numberOfActivePredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::ACTIVE,
						HuntingMode::sit_and_wait);
				numberOfStarvedPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::STARVED,
						HuntingMode::active_hunting);
				numberOfStarvedPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::STARVED,
						HuntingMode::sit_and_wait);
				numberOfPredatedPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::PREDATED,
						HuntingMode::active_hunting);
				numberOfPredatedPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::PREDATED,
						HuntingMode::sit_and_wait);
				numberOfReproducingPredators += getCell(z,y,x)->getLifeStagePopulation(
						LifeStage::REPRODUCING, HuntingMode::active_hunting);
				numberOfReproducingPredators += getCell(z,y,x)->getLifeStagePopulation(
						LifeStage::REPRODUCING, HuntingMode::sit_and_wait);
				numberOfBackgroundPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::BACKGROUND,
						HuntingMode::active_hunting);
				numberOfBackgroundPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::BACKGROUND,
						HuntingMode::sit_and_wait);
				numberOfSenescedPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::SENESCED,
						HuntingMode::active_hunting);
				numberOfSenescedPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::SENESCED,
						HuntingMode::sit_and_wait);
				numberOfShockedPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::SHOCKED,
						HuntingMode::active_hunting);
				numberOfShockedPredators += getCell(z,y,x)->getLifeStagePopulation(LifeStage::SHOCKED,
						HuntingMode::sit_and_wait);

			}
		}
	}

	Output::print(
		file, "{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t\n",
		day, biomass, numberOfUnbornPreys, numberOfActivePreys, numberOfStarvedPreys,
		numberOfPredatedPreys, numberOfReproducingPreys, numberOfSenescedPreys,
		numberOfShockedPreys, numberOfUnbornPredators, numberOfActivePredators,
		numberOfStarvedPredators, numberOfPredatedPredators, numberOfReproducingPredators,
		numberOfBackgroundPredators, numberOfSenescedPredators, numberOfShockedPredators
	);
}

void World::printExtendedDailySummary(FILE* file, int day)
{
	map<ResourceSpecies*, double>* worldResourceBiomass = new map<ResourceSpecies*, double>();
	for (auto itResourceSpecies = existingResourceSpecies.begin(); itResourceSpecies != existingResourceSpecies.end(); itResourceSpecies++)
	{
		worldResourceBiomass->insert(std::pair<ResourceSpecies*, double>((*itResourceSpecies),0));
	}

	map<AnimalSpecies*, vector<unsigned int>* >* worldAnimalsPopulation = new map<AnimalSpecies*, vector<unsigned int>* >();
	for(auto itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
	{
		worldAnimalsPopulation->insert(std::pair<AnimalSpecies*, vector<unsigned int>*>((*itSpecies), new vector<unsigned int>(LifeStage::SHOCKED+1, 0)));
	}

	TerrainCell* cellToUpdateFrom;
	for (unsigned int z = 0; z < depth; z++)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				cellToUpdateFrom = getCell(z,y,x);
				if(!cellToUpdateFrom->isObstacle())
				{
					cellToUpdateFrom->updateWorldResourceBiomassAndAnimalsPopulation(worldResourceBiomass, worldAnimalsPopulation);
				}
			}
		}
	}

	Output::print(file, "{}\t", day);

	for (auto itResourceSpecies = existingResourceSpecies.begin(); itResourceSpecies != existingResourceSpecies.end(); itResourceSpecies++)
	{
		Output::print(file, "{}\t", worldResourceBiomass->at(*itResourceSpecies));
	}

	for (auto itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
	{
		for(const auto &lifeStage : LifeStage::getEnumValues())
		{
			Output::print(file, "{}\t", worldAnimalsPopulation->at(*itSpecies)->at(lifeStage));
		}
	}
	Output::print(file, "\n");

/*
	os << day << "\t" << biomass << "\t" << numberOfUnbornPreys << "\t" << numberOfActivePreys << "\t"
			<< numberOfStarvedPreys << "\t" << numberOfPredatedPreys << "\t" << numberOfReproducingPreys << "\t"
			<< numberOfSenescedPreys << "\t" << numberOfUnbornPredators << "\t" << numberOfActivePredators << "\t"
			<< numberOfStarvedPredators << "\t" << numberOfPredatedPredators << "\t" << numberOfReproducingPredators
			<< "\t" << numberOfBackgroundPredators << "\t" << numberOfSenescedPredators << endl;
			*/

	worldResourceBiomass->clear();
	delete worldResourceBiomass;
	
	for (auto itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
	{
	
		delete worldAnimalsPopulation->at(*itSpecies);
		
	}
	worldAnimalsPopulation->clear();
	delete worldAnimalsPopulation;
}

void World::printGeneticsSummaries(int day)
{
	map<AnimalSpecies*, vector<unsigned int>* >* worldAnimalsPopulation = new map<AnimalSpecies*, vector<unsigned int>* >();
	map<AnimalSpecies*, vector<set<double>* >* >* worldGeneticsFrequencies = new map<AnimalSpecies*, vector<set<double>* >* >();
	for(auto itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
	{
		worldAnimalsPopulation->insert(std::pair<AnimalSpecies*, vector<unsigned int>*>((*itSpecies), new vector<unsigned int>(LifeStage::SHOCKED+1, 0)));
		//We multiply x2 because there are PAIRS of chromosomes. We store this data this way: T1_CR1, T2_CR1... T13_CR1.. T1_CR2, T2_CR2... T13_CR2
		unsigned int totalNumberOfLoci = (*itSpecies)->getNumberOfVariableTraits() * (*itSpecies)->getNumberOfLociPerTrait() * 2;
		worldGeneticsFrequencies->insert(std::pair<AnimalSpecies*, vector<set<double>* >*>((*itSpecies), new vector<set<double>* >(totalNumberOfLoci)));
		for(unsigned int i = 0; i < totalNumberOfLoci; ++i)
		{
			worldGeneticsFrequencies->at((*itSpecies))->at(i) = new set<double>();
		}
	}

	TerrainCell* cellToUpdateFrom;
	for (unsigned int z = 0; z < depth; z++)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				cellToUpdateFrom = getCell(z,y,x);
				if(!cellToUpdateFrom->isObstacle())
				{
					cellToUpdateFrom->updateAnimalsPopulationAndGeneticsFrequencies(worldAnimalsPopulation, worldGeneticsFrequencies);
				}

			}

		}
	}

	double sum, mean;
	for(auto itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
	{
		Output::print(geneticsSummaryFile[*itSpecies], "{}\t", day);
		Output::print(geneticsSummaryFile[*itSpecies], "{}\t", worldAnimalsPopulation->at(*itSpecies)->at(LifeStage::UNBORN) + worldAnimalsPopulation->at(*itSpecies)->at(LifeStage::ACTIVE) + worldAnimalsPopulation->at(*itSpecies)->at(LifeStage::REPRODUCING));

		//We multiply x2 because there are PAIRS of chromosomes. We store this data this way: T1_CR1, T2_CR1... T13_CR1.. T1_CR2, T2_CR2... T13_CR2
		for(unsigned int selectedChromosome = 0; selectedChromosome < (*itSpecies)->getNumberOfVariableTraits()*2; ++selectedChromosome)
		{
			sum = 0;
			mean = 0;
			for(unsigned int selectedLoci = 0; selectedLoci < (*itSpecies)->getNumberOfLociPerTrait(); ++selectedLoci)
			{
				sum += worldGeneticsFrequencies->at((*itSpecies))->at(selectedChromosome*(*itSpecies)->getNumberOfLociPerTrait()+selectedLoci)->size();
			}
			mean = sum / (*itSpecies)->getNumberOfLociPerTrait();
			Output::print(geneticsSummaryFile[*itSpecies], "{}\t", mean);
		}
		Output::print(geneticsSummaryFile[*itSpecies], "\n");
	}

	// Delete worldAnimalsPopulation
	for(auto &[key, value] : *worldAnimalsPopulation) {
		delete value;
	}
	worldAnimalsPopulation->clear();
	delete worldAnimalsPopulation;

	// Delete worldGeneticsFrequencies
	for(auto &[key, value] : *worldGeneticsFrequencies) {
		for(auto elem : *value) {
			delete elem;
		}
		value->clear();
		delete value;
	}
	worldGeneticsFrequencies->clear();
	delete worldGeneticsFrequencies;
}

void World::saveAnimalSpeciesSnapshot(fs::path filenameRoot, string filename, int day, AnimalSpecies* species)
{
	if (species->getTotalInitialPopulation() > 0)
	{
		string scientificName = species->getScientificName();
		std::replace(scientificName.begin(), scientificName.end(), ' ', '_');

		FILE* file = nullptr;
		string fullPath = createOutputFile(&file, filenameRoot, filename + "_" + scientificName + "_day_", "dat", day, recordEach);

		Output::cout("Saving Animal as {}... ", fullPath);

		float value;
		for (unsigned int z = 0; z < depth; z++)
		{
			for (unsigned int y = 0; y < length; y++)
			{
				for (unsigned int x = 0; x < width; x++)
				{
					TerrainCell* currentTerrainCell = getCell(z,y,x);
					if(!currentTerrainCell->isObstacle())
					{
						value = currentTerrainCell->getLifeStagePopulation(LifeStage::ACTIVE, species);
					}
					else
					{
						value = 0.0;
					}

					Output::print(file, "{}", value);
				}
			}
		}

		Output::cout("DONE\n");

		fclose(file);
	}
}

void World::saveResourceSpeciesSnapshot(fs::path filenameRoot, string filename, int day, ResourceSpecies* species)
{
	string scientificName = species->getScientificName();
	std::replace(scientificName.begin(), scientificName.end(), ' ', '_');

	FILE* file = nullptr;
	string fullPath = createOutputFile(&file, filenameRoot, filename + "_" + scientificName + "_day_", "dat", day, recordEach);

	Output::cout("Saving Resource as {}... ", fullPath);

	float value;

	TerrainCell * currentTerrainCell;
	for (unsigned int z = 0; z < depth; ++z)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				currentTerrainCell = getCell(z,y,x);
				if(!currentTerrainCell->isObstacle())
				{
					value = currentTerrainCell->getResourceBiomass(species);
				}
				else
				{
					value = 0.0;
				}

				Output::print(file, "{}", value);
			}
		}
	}

	Output::cout("DONE\n");

	fclose(file);
}

void World::saveWaterSnapshot(fs::path filenameRoot, string filename, int day)
{
	FILE* file = nullptr;
	string fullPath = createOutputFile(&file, filenameRoot, filename + "_day_", "dat", day, recordEach);

	Output::cout("Saving Water volume as {}... ", fullPath);

	float value;

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
					value = currentTerrainCell->getWater();
					Output::print(file, "{}", value);
				}
			}
		}
	}

	fclose(file);
	Output::cout("DONE\n");
}

void World::printPredationEventsOnOtherSpeciesMatrix(FILE* predationEventsOnOtherSpeciesFile)
{
	Output::print(predationEventsOnOtherSpeciesFile, "\t");
	for (unsigned int i = 0; i < existingAnimalSpecies.size(); i++)
	{
		Output::print(predationEventsOnOtherSpeciesFile, "{}\t", existingAnimalSpecies[i]->getScientificName());
	}

	Output::print(predationEventsOnOtherSpeciesFile, "\n");

	vector<int>* currentPredationEventsOnOtherSpecies;
	for (unsigned int i = 0; i < existingAnimalSpecies.size(); i++)
	{
		Output::print(predationEventsOnOtherSpeciesFile, "{}\t", existingAnimalSpecies[i]->getScientificName());
		currentPredationEventsOnOtherSpecies = existingAnimalSpecies[i]->getPredationEventsOnOtherSpecies();
		for (unsigned int j = 0; j < existingAnimalSpecies.size(); j++)
		{
			Output::print(predationEventsOnOtherSpeciesFile, "{}\t", currentPredationEventsOnOtherSpecies->at(existingAnimalSpecies[j]->getId()));
		}
		Output::print(predationEventsOnOtherSpeciesFile, "\n");
	}
}

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

void World::obtainInhabitableTerrainCells(vector<TerrainCell*> &inhabitableTerrainCells) {
	for (unsigned int z = 0; z < depth; z++)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				TerrainCell* currentTerrainCell = getCell(z,y,x);
				if(!currentTerrainCell->isObstacle())
				{
					inhabitableTerrainCells.push_back(currentTerrainCell);
				}
			}
		}
	}
}

void World::evolveWorld()
{
	saveWaterSnapshot(outputFolder / fs::path("Snapshots"), "Water_initial", 0);

	for (auto resourceSpeciesIt = existingResourceSpecies.begin(); resourceSpeciesIt != existingResourceSpecies.end(); resourceSpeciesIt++)
	{
		saveResourceSpeciesSnapshot(outputFolder / fs::path("Snapshots"), "Resource_initial", 0, *resourceSpeciesIt);
	}

	// Next is intentionally commented because it would give empty volumes (animals are unborn)
	/*
	 std::vector<AnimalSpecies *>::iterator animIt;

	 for (animIt = existingAnimalSpecies.begin(); animIt != existingAnimalSpecies.end(); animIt++)
	 {
	 saveAnimalSpeciesSnapshot(outputFolder + "/Snapshots/" + "Animal_initial", 0, **animIt);
	 }
	 */

	vector<TerrainCell*> inhabitableTerrainCells;
	obtainInhabitableTerrainCells(inhabitableTerrainCells);

	FILE* timeSpentFile = nullptr;
	createOutputFile(&timeSpentFile, outputFolder, "time_spent", "txt");
	if (!isFileOpen(timeSpentFile))
	{
		Output::cerr("Error opening the file.\n");
	}

	for (unsigned int timeStep = 0; timeStep < runDays*timeStepsPerDay; timeStep++)
	{
		Output::cout("Running on day {} out of {}\n", timeStep, runDays*timeStepsPerDay);

		printAnimalsAlongCells(timeStep, 0);

		#ifdef _DEBUG
		multipleSameSearchedAnimalToday = 0;
		multipleSameEncounteredAnimalToday = 0;
		multipleSameHuntedAnimalToday = 0;
		multipleSamePredatedAnimalToday = 0;
		#endif

//#####################################################################
//#######################  ACTIVATING ANIMALS   #######################
//#####################################################################
		


		string pathBySimulationPointTuneTraits = "animals_each_day_growth";

		FILE* tuneTraitsFile = nullptr;
		createOutputFile(&tuneTraitsFile, outputFolder / fs::path(pathBySimulationPointTuneTraits), "animals_growth_day_", "txt", timeStep, recordEach);
		if (!isFileOpen(tuneTraitsFile))
		{
			Output::cerr("Error opening the file.\n");
		}
		else
		{
			Output::print(tuneTraitsFile, "growth\tLinf\tid\tspecies\tstate\tcurrent_age\tinstar\tbody_size\tenergy_tank\ttankAtGrowth\tbody_mass\tmature\tmin_mass_for_death\tfinalJMinVB\tfinalJMaxVB\tvoracity_ini\texpectedDryMassFromMinVor\texpectedDryMassFromMaxVor\tmaxMassNextInstarPlasticity\tcurrentWetMass\tpreT_search\tpreT_speed\tpostT_search\tpostT_speed\tini_mass_instar\ttarget_next_mass\tminimum_met_loss\tcondition_search\tcondition_speed\tnon_condition_voracity\tcondition_voracity\tafter_encounters_voracity\tafter_encounters_search\tfinal_speed\tdeath_date\n");
			Output::coutFlush(" - Activating animals and tuning traits ... \n");
			auto t0 = clock();
				
		


/*
#ifdef _TBB
			// using parallel_for approach
			//		parallel_for(size_t(0), size_t(inhabitableTerrainCells.size()), [&inhabitableTerrainCells, &day](size_t i)
			//		{
			//			inhabitableTerrainCells[i]->activateAnimals(day);
			//			inhabitableTerrainCells[i]->tuneTraits();
			//		});
			//
			ActivateAnimalsRangerTask& activateAnimalsRangerTask = *new(task::allocate_root()) ActivateAnimalsRangerTask(width, length, depth, terrain, timeStep);
			task::spawn_root_and_wait(activateAnimalsRangerTask);
#elif _PTHREAD
			ThreadRangerArgument toDoArguments[PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION];
			pthread_t workerThreads[PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION];

			unsigned int xSliceSize = width / PARTITIONS_PER_DIMENSION;
			unsigned int ySliceSize = length / PARTITIONS_PER_DIMENSION;
			unsigned int zSliceSize = depth / PARTITIONS_PER_DIMENSION;
			int errorCreatingThread;
			unsigned int x0, x1, y0, y1;

			for (unsigned int i = 0; i < PARTITIONS_PER_DIMENSION; ++i)
			{
				x0 = i*xSliceSize;
				x1 = (i!=PARTITIONS_PER_DIMENSION-1)?((i+1)*xSliceSize):width;
				for (unsigned int j = 0; j < PARTITIONS_PER_DIMENSION; ++j)
				{
					y0 = j*ySliceSize;
					y1 = (j!=PARTITIONS_PER_DIMENSION-1)?((j+1)*ySliceSize):length;
					for (unsigned int k = 0; k < PARTITIONS_PER_DIMENSION; ++k)
					{
						toDoArguments[i][j][k].world = this;
						toDoArguments[i][j][k].x0 = x0;
						toDoArguments[i][j][k].x1 = x1;
						toDoArguments[i][j][k].y0 = y0;
						toDoArguments[i][j][k].y1 = y1;
						toDoArguments[i][j][k].z0 = k*zSliceSize;
						toDoArguments[i][j][k].z1 = (k!=PARTITIONS_PER_DIMENSION-1)?((k+1)*zSliceSize):depth;
						toDoArguments[i][j][k].timeStep = timeStep;

						errorCreatingThread = pthread_create(&workerThreads[i][j][k], NULL, &World::activateAnimalsThreadMaker, static_cast<void*>(&toDoArguments[i][j][k]));
						if (errorCreatingThread){
						 cout << "Error:unable to create thread," << errorCreatingThread << endl;
						 exit(-1);
						}
					}
				}
			}

			for (unsigned int i = 0; i < PARTITIONS_PER_DIMENSION; ++i)
			{
				for (unsigned int j = 0; j < PARTITIONS_PER_DIMENSION; ++j)
				{
					for (unsigned int k = 0; k < PARTITIONS_PER_DIMENSION; ++k)
					{
						pthread_join(workerThreads[i][j][k], NULL);
					}
				}
			}
#else
*/
			for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
			{
				TerrainCell* currentTerrainCell = inhabitableTerrainCells[i];
				currentTerrainCell->updateTemperature(timeStep);
				currentTerrainCell->updateRelativeHumidity(timeStep);
				currentTerrainCell->activateAndResumeAnimals(timeStep, timeStepsPerDay);
				currentTerrainCell->tuneTraits(timeStep, timeStepsPerDay, tuneTraitsFile, getSimulType(), &World::getCell);
			}
/*
#endif
*/
			auto t1 = clock();
			Output::cout("Time: {} secs.\n", double(t1-t0)/CLOCKS_PER_SEC);
			Output::print(timeSpentFile, "{}\t", double(t1-t0)/CLOCKS_PER_SEC);
			Output::coutFlush("DONE\n");

			fclose(tuneTraitsFile);
		}

//#####################################################################
//#######################     GROWING FUNGI     #######################
//#####################################################################

		Output::coutFlush(" - Growing resource ... \n");
		std::random_shuffle(inhabitableTerrainCells.begin(), inhabitableTerrainCells.end());
		auto t0 = clock();

/*
#ifdef _TBB
		// using the taskgroup approach
		//task_group taskGroup;
		//int cellsPerTask = inhabitableTerrainCells.size()/2;
		//for (unsigned int i = 0; i < 2; i++)
		//{
		//	taskGroup.run([&]{
		//		for (unsigned int j = i*cellsPerTask; j < (i+1)*cellsPerTask; j++)
		//		{
		//			currentTerrainCell = inhabitableTerrainCells[j];
		//			currentTerrainCell->growResource();
		//		}
		//	});
		//}
		//taskGroup.wait();
		//
		GrowResourceRangerTask& growResourceRangerTask = *new(task::allocate_root()) GrowResourceRangerTask(width, length, depth, terrain);
		task::spawn_root_and_wait(growResourceRangerTask);
#elif _PTHREAD
		//
		//ThreadRangerArgument toDoArguments[PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION];
		//pthread_t workerThreads[PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION];

    	//unsigned int xSliceSize = width / PARTITIONS_PER_DIMENSION;
    	//unsigned int ySliceSize = length / PARTITIONS_PER_DIMENSION;
    	//unsigned int zSliceSize = depth / PARTITIONS_PER_DIMENSION;
    	//int errorCreatingThread;
    	//unsigned int x0, x1, y0, y1;
		//

    	for (unsigned int i = 0; i < PARTITIONS_PER_DIMENSION; ++i)
    	{
    		x0 = i*xSliceSize;
    		x1 = (i!=PARTITIONS_PER_DIMENSION-1)?((i+1)*xSliceSize):width;
			for (unsigned int j = 0; j < PARTITIONS_PER_DIMENSION; ++j)
			{
				y0 = j*ySliceSize;
				y1 = (j!=PARTITIONS_PER_DIMENSION-1)?((j+1)*ySliceSize):length;
				for (unsigned int k = 0; k < PARTITIONS_PER_DIMENSION; ++k)
				{
					toDoArguments[i][j][k].world = this;
					toDoArguments[i][j][k].x0 = x0;
					toDoArguments[i][j][k].x1 = x1;
					toDoArguments[i][j][k].y0 = y0;
					toDoArguments[i][j][k].y1 = y1;
					toDoArguments[i][j][k].z0 = k*zSliceSize;
					toDoArguments[i][j][k].z1 = (k!=PARTITIONS_PER_DIMENSION-1)?((k+1)*zSliceSize):depth;

					errorCreatingThread = pthread_create(&workerThreads[i][j][k], NULL, &World::growResourceThreadMaker, static_cast<void*>(&toDoArguments[i][j][k]));
					if (errorCreatingThread){
					 cout << "Error:unable to create thread," << errorCreatingThread << endl;
					 exit(-1);
					}
				}
			}
    	}

    	for (unsigned int i = 0; i < PARTITIONS_PER_DIMENSION; ++i)
    	{
			for (unsigned int j = 0; j < PARTITIONS_PER_DIMENSION; ++j)
			{
				for (unsigned int k = 0; k < PARTITIONS_PER_DIMENSION; ++k)
				{
					pthread_join(workerThreads[i][j][k], NULL);
				}
			}
    	}
#else
*/
		for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
		{
			TerrainCell* currentTerrainCell = inhabitableTerrainCells[i];
			currentTerrainCell->growResource(timeStep, getCompetitionAmongResourceSpecies(), currentTerrainCell->getTotalMaximumResourceCapacity(), 
			existingResourceSpecies.size(), getSimulType(), getDepth(), getLength(), getWidth(), &World::getCell, getCompetitionAmongResourceSpecies(), getMassRatio());
		}
/*
#endif
*/
		auto t1 = clock();
		Output::cout("Time: {} secs.\n", (double(t1-t0)/CLOCKS_PER_SEC));
		Output::print(timeSpentFile, "{}\t", double(t1-t0)/CLOCKS_PER_SEC);
		Output::coutFlush("DONE\n");
		
#ifndef _TEST
		// After a resource has grown, it can expand to other cells. In this case,
		// when the new cell is processed, a wrong amount (in excess) of resource would
		// be found. Therefore, new amounts are added to a temporary variable that
		// must be processed upon completion of all cells processing

		Output::coutFlush(" - Spreading resource ... \n");

		// TODO ROMAN All this can be done in parallel
		t0 = clock();
		/*
		parallel_for(size_t(0), size_t(inhabitableTerrainCells.size()), size_t(1) , [=](size_t i)
		{
			inhabitableTerrainCells[i]->commitResourceSpread();
		});
		*/
		for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
		{
			//cout << "Cell: (" << inhabitableTerrainCells[i]->getX() << "," << inhabitableTerrainCells[i]->getY() << "," << inhabitableTerrainCells[i]->getZ() << ")" << endl;
			inhabitableTerrainCells[i]->commitResourceSpread(); //OK			// Grows and spreads excess to neighbors
		}

		t1 = clock();
		Output::cout("Time: {} secs.\n", (double(t1-t0)/CLOCKS_PER_SEC));
		Output::print(timeSpentFile, "{}\t", double(t1-t0)/CLOCKS_PER_SEC);
		Output::coutFlush("DONE\n");
		
		/*
		cout << " - Applying humidity decay over time effect ... " << endl << flush;
		// Moisture decays each day
		//TODO Should moisture decay depending on the amount consumed by resource?
		t0 = tick_count::now();

		parallel_for(size_t(0), size_t(inhabitableTerrainCells.size()), size_t(1) , [=](size_t i)
		{
			inhabitableTerrainCells[i]->decayWater();
		});


		for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
		{
			currentTerrainCell = inhabitableTerrainCells[i];
			currentTerrainCell->decayWater(relativeHumidityDecayOverTime);
		}


		t1 = tick_count::now();
		cout << "Time: " << (t1-t0).seconds() << " secs." << endl;
		timeSpentFile << (t1-t0).seconds() << "\t";
		cout << "DONE" << endl << flush;


		cout << " - Applying chemostat effect ... " << endl << flush;
		// Adds moisture from chemostat effect.
		//TODO Add standardDeviation so the rain event is not totally constant every X days
		t0 = tick_count::now();
		if (timeStep > 0 && timeStep % timelapseForRainEvent == 0)
		{
			for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
			{
				currentTerrainCell = inhabitableTerrainCells[i];
				currentTerrainCell->chemostatEffect();
			}
		}
		t1 = tick_count::now();
		cout << "Time: " << (t1-t0).seconds() << " secs." << endl;
		timeSpentFile << (t1-t0).seconds() << "\t";
		cout << "DONE" << endl << flush;
		*/

#endif // _TEST

		string pathBySimulationPoint = "animals_each_day_encounterProbabilities";

		FILE *encounterProbabilitiesFile, *predationProbabilitiesFile;
		createOutputFile(&encounterProbabilitiesFile, outputFolder / fs::path(pathBySimulationPoint), "animals_encounterProbabilities_day_", "txt", timeStep, recordEach);
		if (!isFileOpen(encounterProbabilitiesFile))
		{
			Output::cerr("Error opening the file.\n");
		}
		else
		{
			pathBySimulationPoint = "animals_each_day_predationProbabilities";

			createOutputFile(&predationProbabilitiesFile, outputFolder / fs::path(pathBySimulationPoint), "animals_predationProbabilities_day_", "txt", timeStep, recordEach);
			if (!isFileOpen(predationProbabilitiesFile))
			{
				Output::cerr("Error opening the file.\n");
			}
			else
			{
				Output::print(encounterProbabilitiesFile, "idSearcher\tidSearched\tspeciesSearcher\tspeciesSearched\tsearchedIsPredator\tmassSearcher\tmassSearched\tprobRandomEncounter\tprobLogisticEncounter\tprobAttack\tsuccessfulEncounter\n");
				Output::print(predationProbabilitiesFile, "idHunter\tidHunted\tspeciesHunter\tspeciesHunted\thuntedIsPredator\tmassHunter\tmassHunted\tprobRandomPredation\tprobLogisticPredation\tsuccessfulPredation\n");

				Output::coutFlush(" - Moving animals ... \n");
				std::random_shuffle(inhabitableTerrainCells.begin(), inhabitableTerrainCells.end());
				t0 = clock();
/*				
#ifdef _TBB
				// using task_group approach
				//task_group taskGroup;
				//for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
				//{
				//	currentTerrainCell = inhabitableTerrainCells[i];
				//
				//	taskGroup.run([&]{currentTerrainCell->moveAnimals(day, encounterProbabilitiesFile, predationProbabilitiesFile);});
				//
				//	//MoveAnimalsTask& moveAnimalsTask = *new(task::allocate_root()) MoveAnimalsTask(currentTerrainCell, day, encounterProbabilitiesFile, predationProbabilitiesFile);
				//	//task::spawn_root_and_wait(moveAnimalsTask);
				//}
				//taskGroup.wait();
				//
				MoveAnimalsRangerTask& moveAnimalsRangerTask = *new(task::allocate_root()) MoveAnimalsRangerTask(width, length, depth, terrain, timeStep);
				task::spawn_root_and_wait(moveAnimalsRangerTask);
#elif _PTHREAD
				//
				//ThreadRangerArgument toDoArguments[PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION];
				//pthread_t workerThreads[PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION][PARTITIONS_PER_DIMENSION];
				//
		    	//unsigned int xSliceSize = width / PARTITIONS_PER_DIMENSION;
		    	//unsigned int ySliceSize = length / PARTITIONS_PER_DIMENSION;
		    	//unsigned int zSliceSize = depth / PARTITIONS_PER_DIMENSION;
		    	//int errorCreatingThread;
		    	//unsigned int x0, x1, y0, y1;
				//

		    	for (unsigned int i = 0; i < PARTITIONS_PER_DIMENSION; ++i)
		    	{
		    		x0 = i*xSliceSize;
		    		x1 = (i!=PARTITIONS_PER_DIMENSION-1)?((i+1)*xSliceSize):width;
					for (unsigned int j = 0; j < PARTITIONS_PER_DIMENSION; ++j)
					{
						y0 = j*ySliceSize;
						y1 = (j!=PARTITIONS_PER_DIMENSION-1)?((j+1)*ySliceSize):length;
						for (unsigned int k = 0; k < PARTITIONS_PER_DIMENSION; ++k)
						{
							toDoArguments[i][j][k].world = this;
							toDoArguments[i][j][k].x0 = x0;
							toDoArguments[i][j][k].x1 = x1;
							toDoArguments[i][j][k].y0 = y0;
							toDoArguments[i][j][k].y1 = y1;
							toDoArguments[i][j][k].z0 = k*zSliceSize;
							toDoArguments[i][j][k].z1 = (k!=PARTITIONS_PER_DIMENSION-1)?((k+1)*zSliceSize):depth;
							toDoArguments[i][j][k].timeStep = timeStep;

							errorCreatingThread = pthread_create(&workerThreads[i][j][k], NULL, &World::moveAnimalsThreadMaker, static_cast<void*>(&toDoArguments[i][j][k]));
							if (errorCreatingThread){
							 cout << "Error:unable to create thread," << errorCreatingThread << endl;
							 exit(-1);
							}
						}
					}
		    	}

		    	for (unsigned int i = 0; i < PARTITIONS_PER_DIMENSION; ++i)
		    	{
					for (unsigned int j = 0; j < PARTITIONS_PER_DIMENSION; ++j)
					{
						for (unsigned int k = 0; k < PARTITIONS_PER_DIMENSION; ++k)
						{
							pthread_join(workerThreads[i][j][k], NULL);
						}
					}
		    	}
#else
*/

		    	float printBarEach = 0.05;
		    	float currentPercentage;

		    	Output::cout("0%|");
		    	for(currentPercentage = printBarEach; currentPercentage < 1.0; currentPercentage+=printBarEach)
		    	{
		    		Output::cout("  ");
		    	}
		    	Output::cout("|100%\n");

		    	currentPercentage = printBarEach;
				Output::cout("   ");
		    	for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
				{
					TerrainCell* currentTerrainCell = inhabitableTerrainCells[i];
					currentTerrainCell->moveAnimals(timeStep, timeStepsPerDay, encounterProbabilitiesFile, predationProbabilitiesFile, getSaveEdibilitiesFile(), edibilitiesFile, exitTimeThreshold, &getCellByBearing, getDepth(), getLength(), getWidth(), &World::getCell, getPdfThreshold(), getMuForPDF(), getSigmaForPDF(), getPredationSpeedRatioAH(), getPredationHunterVoracityAH(), getPredationProbabilityDensityFunctionAH(), getPredationSpeedRatioSAW(), getPredationHunterVoracitySAW(), getPredationProbabilityDensityFunctionSAW(), getMaxSearchArea(), getEncounterHuntedVoracitySAW(), getEncounterHunterVoracitySAW(), getEncounterVoracitiesProductSAW(), getEncounterHunterSizeSAW(), getEncounterHuntedSizeSAW(), getEncounterProbabilityDensityFunctionSAW(), getEncounterHuntedVoracityAH(), getEncounterHunterVoracityAH(), getEncounterVoracitiesProductAH(), getEncounterHunterSizeAH(), getEncounterHuntedSizeAH(), getEncounterProbabilityDensityFunctionAH());
					if(i >= currentPercentage * inhabitableTerrainCells.size())
					{
						Output::coutFlush("==");
						currentPercentage += printBarEach;
					}
				}
				Output::cout("\n");
/*
#endif
*/
				t1 = clock();
				Output::cout("Time: {} secs.\n", (double(t1-t0)/CLOCKS_PER_SEC));
				Output::print(timeSpentFile, "{}\t", double(t1-t0)/CLOCKS_PER_SEC);
				Output::coutFlush("DONE\n");

				fclose(predationProbabilitiesFile);
			}

			fclose(encounterProbabilitiesFile);
		}

		#ifdef _DEBUG
		Output::cout("Animals searched for by the same animal at least twice the same day: {}\n", multipleSameSearchedAnimalToday);
		Output::cout("Animals encountered by the same animal at least twice the same day: {}\n", multipleSameEncounteredAnimalToday);
		Output::cout("Animals hunted off by the same animal at least twice the same day: {}\n", multipleSameHuntedAnimalToday);
		Output::cout("Animals predated by the same animal at least twice the same day: {}\n", multipleSamePredatedAnimalToday);
		#endif

		string pathBySimulationPointVoracities = "animals_each_day_voracities";

		FILE* voracitiesFile = nullptr;
		createOutputFile(&voracitiesFile, outputFolder / fs::path(pathBySimulationPointVoracities), "animals_voracities_day_", "txt", timeStep, recordEach);
		if (!isFileOpen(voracitiesFile))
		{
			Output::cerr("Error opening the file.\n");
		}
		else
		{
			Output::print(voracitiesFile, "id\tspecies\tstate\tcurrentAge\tinstar\tbody_size\tenergy_tank\tdryMass\tnextDinoMass\tmin_mass_for_death\tafter_encounters_voracity\tfood_mass\tdryMassAfterAssim\ttotalMetabolicDryMassLossAfterAssim\teatenToday\tsteps\tstepsAttempted\tafter_encounters_search\tsated\tpercentMoving\tpercentHandling\tvoracity_body_mass_ratio\tgender\tmated\teggDryMass\tK\tL\tpseudoK\tpseudoL\tfactorEggMass\teggDryMassAtBirth\tdeath_date\tageOfFirstMaturation\trep_count\n");
			Output::coutFlush(" - Background, assimilating food and reproducing ... \n");
			t0 = clock();

/*
#ifdef _TBB

#elif _PTHREAD
	    	for (unsigned int i = 0; i < PARTITIONS_PER_DIMENSION; ++i)
	    	{
	    		x0 = i*xSliceSize;
	    		x1 = (i!=PARTITIONS_PER_DIMENSION-1)?((i+1)*xSliceSize):width;
				for (unsigned int j = 0; j < PARTITIONS_PER_DIMENSION; ++j)
				{
					y0 = j*ySliceSize;
					y1 = (j!=PARTITIONS_PER_DIMENSION-1)?((j+1)*ySliceSize):length;
					for (unsigned int k = 0; k < PARTITIONS_PER_DIMENSION; ++k)
					{
						toDoArguments[i][j][k].world = this;
						toDoArguments[i][j][k].x0 = x0;
						toDoArguments[i][j][k].x1 = x1;
						toDoArguments[i][j][k].y0 = y0;
						toDoArguments[i][j][k].y1 = y1;
						toDoArguments[i][j][k].z0 = k*zSliceSize;
						toDoArguments[i][j][k].z1 = (k!=PARTITIONS_PER_DIMENSION-1)?((k+1)*zSliceSize):depth;
						toDoArguments[i][j][k].timeStep = timeStep;

						errorCreatingThread = pthread_create(&workerThreads[i][j][k], NULL, &World::assimilateThreadMaker, static_cast<void*>(&toDoArguments[i][j][k]));
						if (errorCreatingThread){
						 cout << "Error:unable to create thread," << errorCreatingThread << endl;
						 exit(-1);
						}
					}
				}
	    	}

	    	for (unsigned int i = 0; i < PARTITIONS_PER_DIMENSION; ++i)
	    	{
				for (unsigned int j = 0; j < PARTITIONS_PER_DIMENSION; ++j)
				{
					for (unsigned int k = 0; k < PARTITIONS_PER_DIMENSION; ++k)
					{
						pthread_join(workerThreads[i][j][k], NULL);
					}
				}
	    	}
#else
*/
			for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
			{
				TerrainCell* currentTerrainCell = inhabitableTerrainCells[i];
				currentTerrainCell->printAnimalsVoracities(timeStep, timeStepsPerDay, voracitiesFile, getSimulType());
				currentTerrainCell->diePredatorsFromBackground(timeStep, isGrowthAndReproTest());
				currentTerrainCell->assimilateFoodMass(timeStep);
				currentTerrainCell->metabolizeAnimals(timeStep, timeStepsPerDay, getSimulType());
				currentTerrainCell->growAnimals(timeStep, timeStepsPerDay);
				currentTerrainCell->breedAnimals(timeStep, timeStepsPerDay, outputFolder, getSaveAnimalConstitutiveTraits(), getConstitutiveTraitsFile());
			}
/*
#endif
*/
			t1 = clock();
			Output::cout("Time: {} secs.\n", (double(t1-t0)/CLOCKS_PER_SEC));
			Output::print(timeSpentFile, "{}\t", double(t1-t0)/CLOCKS_PER_SEC);
			Output::coutFlush("DONE\n");

			fclose(voracitiesFile);
		}

		/*
		cout << " - Deleting extinguished reproducing animals ... " << endl << flush;
		t0 = tick_count::now();

		deleteExtinguishedReproducingAnimals();

		t1 = tick_count::now();
		cout << "Time: " << (t1-t0).seconds() << " secs." << endl;
		timeSpentFile << (t1-t0).seconds() << "\t";
		cout << "DONE" << endl << flush;

		*/
		Output::coutFlush(" - Printing animals along cells ... \n");
		t0 = clock();

		printAnimalsAlongCells(timeStep, 1);

		t1 = clock();
		Output::cout("Time: {} secs.\n", (double(t1-t0)/CLOCKS_PER_SEC));
		Output::print(timeSpentFile, "{}\t", double(t1-t0)/CLOCKS_PER_SEC);
		Output::coutFlush("DONE\n");

		Output::coutFlush(" - Printing summary file ... \n");
		t0 = clock();

		//printDailySummary(file, day);
		printExtendedDailySummary(extendedDailySummaryFile, timeStep);

		t1 = clock();
		Output::cout("Time: {} secs.\n", (double(t1-t0)/CLOCKS_PER_SEC));
		Output::print(timeSpentFile, "{}\t", double(t1-t0)/CLOCKS_PER_SEC);
		Output::coutFlush("DONE\n");

		Output::coutFlush(" - Purging dead animals ... \n");
		t0 = clock();

		/* vector<Edible*> * cosa = NULL;
		vector<Edible*> * cosa2 = NULL; */
		for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
		{
			TerrainCell* currentTerrainCell = inhabitableTerrainCells[i];

 		//Dinosaur Debug or else - cutting reproduction at a given date
		/*
		if(timeStep<1){
		for(vector<Species *>::iterator animalSpeciesIt = existingAnimalSpecies.begin(); animalSpeciesIt != existingAnimalSpecies.end(); animalSpeciesIt++)
		{

			//Species* currentAnimalSpecies = *animalSpeciesIt;

            (*animalSpeciesIt)->setEggsPerBatch(0);
		}
		}
		*/
///Below Dinosaur Debug - do animals really disappear from the animals vector?
/* 		for(vector<Species *>::iterator animalSpeciesIt = existingAnimalSpecies.begin(); animalSpeciesIt != existingAnimalSpecies.end(); animalSpeciesIt++)
		{

			Species* currentAnimalSpecies = *animalSpeciesIt;

			vector<Edible*> * cosa = currentTerrainCell->getAnimalsBySpecies(Animal::BACKGROUND, currentAnimalSpecies);
			cout << "before   " << cosa->size() << endl;
 */			
			currentTerrainCell->purgeDeadAnimals();
			
/* 			vector<Edible*> * cosa2 = currentTerrainCell->getAnimalsBySpecies(Animal::BACKGROUND, currentAnimalSpecies);
			cout << "after   " << cosa2->size() << endl;
			//exit(-1);
		}
 */ //end Dinosaur Debug
		}
		t1 = clock();
		Output::cout("Time: {} secs.\n", (double(t1-t0)/CLOCKS_PER_SEC));
		Output::print(timeSpentFile, "{}\t\n", double(t1-t0)/CLOCKS_PER_SEC);
		Output::coutFlush("DONE\n");


		//ALWAYS print this genetics after purguing dead animals or before the whole day
		if(saveGeneticsSummaries)
		{
			//TODO FIX THIS INSIDE according to new Genome classes... (use debugging)
			printGeneticsSummaries(timeStep);
		}

		printCellAlongCells(timeStep);

		FILE* predationEventsOnOtherSpeciesFile = nullptr;
		createOutputFile(&predationEventsOnOtherSpeciesFile, outputFolder / fs::path("Matrices"), predationEventsOnOtherSpeciesFilename, "txt");
		if (!isFileOpen(predationEventsOnOtherSpeciesFile))
		{
			Output::cerr("Error opening the file.\n");
		}
		else
		{
			printPredationEventsOnOtherSpeciesMatrix(predationEventsOnOtherSpeciesFile);
			fclose(predationEventsOnOtherSpeciesFile);
		}

		if (saveIntermidiateVolumes && (((timeStep + 1) % saveIntermidiateVolumesPeriodicity) == 0))
		{
			saveWaterSnapshot(outputFolder / fs::path("Snapshots"), "Water", timeStep);

			for (auto fungIt = existingResourceSpecies.begin(); fungIt != existingResourceSpecies.end(); fungIt++)
			{
				saveResourceSpeciesSnapshot(outputFolder / fs::path("Snapshots"), "Resource", timeStep, *fungIt);
			}

			for (auto animIt = existingAnimalSpecies.begin(); animIt != existingAnimalSpecies.end(); animIt++)
			{
				saveAnimalSpeciesSnapshot(outputFolder / fs::path("Snapshots"), "Animal", timeStep, *animIt);
			}
		}
		if(exitAtFirstExtinction){
			if (isExtinguished())
			{
				break;
			}
		}
	}

	saveWaterSnapshot(outputFolder / fs::path("Snapshots"), "Water_final", runDays*timeStepsPerDay);

	for (auto resourceSpeciesIt = existingResourceSpecies.begin(); resourceSpeciesIt != existingResourceSpecies.end(); resourceSpeciesIt++)
	{
		saveResourceSpeciesSnapshot(outputFolder / fs::path("Snapshots"), "Resource_final", runDays*timeStepsPerDay, *resourceSpeciesIt);
	}

	for(auto animalSpeciesIt = existingAnimalSpecies.begin(); animalSpeciesIt != existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		saveAnimalSpeciesSnapshot(outputFolder / fs::path("Snapshots"), "Animal_final", runDays*timeStepsPerDay, *animalSpeciesIt);
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
	if(isFileOpen(edibilitiesFile)) {
		fclose(edibilitiesFile);
	}
	if(isFileOpen(timeSpentFile)) {
		fclose(timeSpentFile);
	}
	if(isFileOpen(constitutiveTraitsFile)) {
		fclose(constitutiveTraitsFile);
	}
	if(isFileOpen(dailySummaryFile)) {
		fclose(dailySummaryFile);
	}
	if(isFileOpen(extendedDailySummaryFile)) {
		fclose(extendedDailySummaryFile);
	}
}

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

bool World::isExtinguished()
{
	std::vector<double> animalPopulation(existingAnimalSpecies.size());
	std::vector<double> resourceBiomass(existingResourceSpecies.size());

	unsigned int i = 0;

	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = 0; z < depth; z++)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				currentTerrainCell = getCell(z,y,x);
				if (!currentTerrainCell->isObstacle())
				{
					i = 0;
					for (auto resourceSpeciesIt = existingResourceSpecies.begin(); resourceSpeciesIt != existingResourceSpecies.end(); resourceSpeciesIt++)
					{
						resourceBiomass[i] += currentTerrainCell->getResourceBiomass(*resourceSpeciesIt);
						i++;
					}

					i = 0;
					for (auto animalSpeciesIt = existingAnimalSpecies.begin(); animalSpeciesIt != existingAnimalSpecies.end(); animalSpeciesIt++)
					{
						animalPopulation[i] += currentTerrainCell->getLifeStagePopulation(LifeStage::ACTIVE, *animalSpeciesIt);
						animalPopulation[i] += currentTerrainCell->getLifeStagePopulation(LifeStage::UNBORN, *animalSpeciesIt);
						i++;
					}
				}
			}
		}
	}

	for (i = 0; i < resourceBiomass.size(); i++)
	{
		if (resourceBiomass[i] == 0)
		{
			return true;
		}
	}

	for (i = 0; i < animalPopulation.size(); i++)
	{
		if (animalPopulation[i] == 0)
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

void World::restart(unsigned int newWidth, unsigned int newLength, unsigned int newDepth = 1, float newCellSize = 1)
{
	eraseTerrain();

	setWidth(newWidth);
	setLength(newLength);
	setDepth(newDepth);
	setCellSize(newCellSize);

	//initialize();
}

void World::initializeTerrainDimensions(const json &moistureBasePatch)
{
	Output::coutFlush("Initializing terrain voxels ... ");
	try
	{
		World::terrain.resize(depth); // Create depth

		for (unsigned int z = 0; z < depth; ++z)
		{
			World::terrain[z].resize(length);

			for (unsigned int y = 0; y < length; y++)
			{
				World::terrain[z][y].resize(width);

				// Now data can be accesed this way:
				// terrain[i][j][k] = 6.0;

				for (unsigned int x = 0; x < width; x++)
				{
					World::terrain[z][y][x] = new TerrainCell(x, y, z, cellSize, moistureBasePatch, &existingAnimalSpecies, &existingResourceSpecies);
					getCell(z,y,x)->updateTemperature(0);
				}

			}
		}
	} catch (std::bad_alloc&) // Treat different exceptions
	{
		Output::cout("caught bad alloc exception, perhaps out of memory?, exiting..\n");
		exit(-1);
	} catch (const std::exception& x)
	{
		std::cerr << typeid(x).name() << std::endl;
	} catch (...)
	{
		std::cerr << "unknown exception" << std::endl;
	}

	Output::cout("DONE\n");
}

void World::readObstaclePatchesFromJSONFiles()
{
	Output::cout("Reading obstacle patches from JSON files ... \n");

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
				ptMain = readConfigFile(*it, WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(OBSTACLE_SCHEMA));

				//string type = jsonTree->get_child("patch.type").data().c_str(); //type is useless for now
				unsigned int widthStartPoint = ptMain["patch"]["xPos"];
				unsigned int lengthStartPoint = ptMain["patch"]["yPos"];
				unsigned int depthStartPoint = ptMain["patch"]["zPos"];
				unsigned int patchWidth = ptMain["patch"]["width"];
				unsigned int patchLength = ptMain["patch"]["length"];
				unsigned int patchDepth = ptMain["patch"]["depth"];

				setCubicObstaclePatch(patchFilename, depthStartPoint, lengthStartPoint, widthStartPoint, patchDepth, patchLength, patchWidth);
			}
		}
	}
	else
	{
		cerr << "The specified path \"" + obstacleFolder.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}
}

void World::readMoisturePatchesFromJSONFiles()
{
	Output::cout("Reading moisture patches from JSON files ... \n");
	
	fs::path moistureFolder = inputFolder / moistureFolderName;
	fs::directory_iterator end_iter;

	if (fs::exists(moistureFolder) && fs::is_directory(moistureFolder))
	{
		vector<fs::path> filePaths;
		copy(fs::directory_iterator(moistureFolder), fs::directory_iterator(), back_inserter(filePaths));
		sort(filePaths.begin(), filePaths.end());             // directory iteration is not ordered on some file systems, so we sort them
		json ptMain;
		for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				string patchFilename = it->string();

				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(MOISTURE_SCHEMA));

				string type = ptMain["patch"]["type"];

				if (type == "homogeneous")
				{
					float value = ptMain["patch"]["value"];
					float relativeHumidityDecayOverTime = ptMain["patch"]["relativeHumidityDecayOverTime"];
					setHomogeneousWater(value, relativeHumidityDecayOverTime);
				}
				//TODO Agregar gaussianas que contemplen la relativeHumidityDecayOverTime
				/*
				else if (type == "gaussian")
				{
					unsigned int radius = ptMain.get<unsigned int>("patch.radius");
					unsigned int x = ptMain.get<unsigned int>("patch.xPos");
					unsigned int y = ptMain.get<unsigned int>("patch.yPos");
					unsigned int z = ptMain.get<unsigned int>("patch.zPos");
					float sigma = ptMain.get<float>("patch.sigma");
					float amplitude = ptMain.get<float>("patch.amplitude");
					setGaussianWaterPatch(x, y, z, radius, sigma, amplitude);
				}
				*/
				else if (type == "sphere")
				{
					unsigned int radius = ptMain["patch"]["radius"];
					unsigned int x = ptMain["patch"]["xPos"];
					unsigned int y = ptMain["patch"]["yPos"];
					unsigned int z = ptMain["patch"]["zPos"];

					vector<float> temperatureCycle = vector<float>();
					for( auto elem : ptMain["patch"]["temperatureCycle"])
					{
						temperatureCycle.push_back(elem);
					}
					vector<float> relativeHumidityCycle = vector<float>();
					bool useRelativeHumidityCycle = ptMain["patch"]["useRelativeHumidityCycle"];
					if (useRelativeHumidityCycle)
					{
						for( auto elem : ptMain["patch"]["relativeHumidityCycle"])
						{
							relativeHumidityCycle.push_back(elem);
						}
					}
					bool useRelativeHumidityDecayOverTime = ptMain["patch"]["useRelativeHumidityDecayOverTime"];
					float relativeHumidityOnRainEvent = ptMain["patch"]["relativeHumidityOnRainEvent"];
					float relativeHumidityDecayOverTime = ptMain["patch"]["relativeHumidityDecayOverTime"];
					int timeStepsBetweenRainEvents = ptMain["patch"]["timeStepsBetweenRainEvents"];
					int standardDeviationForRainEvent = ptMain["patch"]["standardDeviationForRainEvent"];
					float totalMaximumResourceCapacity = ptMain["patch"]["totalMaximumResourceCapacity"];
					bool inEnemyFreeSpace = ptMain["patch"]["inEnemyFreeSpace"];
					bool inCompetitorFreeSpace = ptMain["patch"]["inCompetitorFreeSpace"];

					setSphericalWaterPatch(patchFilename, radius, x, y, z, useRelativeHumidityCycle, temperatureCycle, relativeHumidityCycle, useRelativeHumidityDecayOverTime, relativeHumidityOnRainEvent, relativeHumidityDecayOverTime, timeStepsBetweenRainEvents, standardDeviationForRainEvent, totalMaximumResourceCapacity, inEnemyFreeSpace, inCompetitorFreeSpace);
				}
				else if(type == "cube")
				{
					// This creates a cube-like moisture patch, with center in patchCenter and dimensions
					// in patchDimensions. 
					Coordinate3D<int> patchCenter(
						ptMain["patch"]["center"]["x"], 
						ptMain["patch"]["center"]["y"], 
						ptMain["patch"]["center"]["z"]
					);
					Coordinate3D<int> patchDimensions(
						ptMain["patch"]["dimensions"]["x"], 
						ptMain["patch"]["dimensions"]["y"], 
						ptMain["patch"]["dimensions"]["z"]
					);

					vector<float> temperatureCycle = vector<float>();
					for( auto elem : ptMain["patch"]["temperatureCycle"])
					{
						temperatureCycle.push_back(elem);
					}
					vector<float> relativeHumidityCycle = vector<float>();
					bool useRelativeHumidityCycle = ptMain["patch"]["useRelativeHumidityCycle"];
					if (useRelativeHumidityCycle)
					{
						for( auto elem : ptMain["patch"]["relativeHumidityCycle"])
						{
							relativeHumidityCycle.push_back(elem);
						}
					}
					bool useRelativeHumidityDecayOverTime = ptMain["patch"]["useRelativeHumidityDecayOverTime"];
					float relativeHumidityOnRainEvent = ptMain["patch"]["relativeHumidityOnRainEvent"];
					float relativeHumidityDecayOverTime = ptMain["patch"]["relativeHumidityDecayOverTime"];
					int timeStepsBetweenRainEvents = ptMain["patch"]["timeStepsBetweenRainEvents"];
					int standardDeviationForRainEvent = ptMain["patch"]["standardDeviationForRainEvent"];
					float totalMaximumResourceCapacity = ptMain["patch"]["totalMaximumResourceCapacity"];
					bool inEnemyFreeSpace = ptMain["patch"]["inEnemyFreeSpace"];
					bool inCompetitorFreeSpace = ptMain["patch"]["inCompetitorFreeSpace"];

					setCubicWaterPatch(patchFilename, patchCenter, patchDimensions, useRelativeHumidityCycle, temperatureCycle, relativeHumidityCycle, useRelativeHumidityDecayOverTime, relativeHumidityOnRainEvent, relativeHumidityDecayOverTime, timeStepsBetweenRainEvents, standardDeviationForRainEvent, totalMaximumResourceCapacity, inEnemyFreeSpace, inCompetitorFreeSpace);

				}
				/*
				else if (type == "randomGaussian")
				{
					bool useRandomAmplitude = ptMain.get<bool>("patch.useRandomAmplitude");
					bool useRandomSigma = ptMain.get<bool>("patch.useRandomSigma");
					unsigned int numberOfPatches = ptMain.get<unsigned int>("patch.numberOfPatches");
					unsigned int patchesRadius = ptMain.get<unsigned int>("patch.patchesRadius");
					float maxSigma = ptMain.get<float>("patch.maxSigma");
					float maxAmplitude = ptMain.get<float>("patch.maxAmplitude");

					setRandomGaussianWaterPatches(numberOfPatches, patchesRadius, maxSigma, useRandomSigma, maxAmplitude,
							useRandomAmplitude);
				}
				*/
			}
		}
	}
	else
	{
		cerr << "The specified path \"" + moistureFolder.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}
}

void World::eraseTerrain()
{
	for(auto cell2D : World::terrain) {
		for(auto cell1D : cell2D) {
			for(auto cell : cell1D) {
				delete cell;
			}
		}
	}
}

bool World::getCompetitionAmongResourceSpecies()
{
	return competitionAmongResourceSpecies;
}

int World::getWidth()
{
	return width;
}

int World::getLength()
{
	return length;
}

int World::getDepth()
{
	return depth;
}

const vector<AnimalSpecies *> * World::getExistingAnimalSpecies()
{
	return &existingAnimalSpecies;
}

float World::getCellSize()
{
	return cellSize;
}

void World::setWidth(unsigned int newWidth)
{
	if (newWidth <= 0)
		throw WorldSizeException("Trying to assign wrong value to world width (must be positive)");

	width = newWidth;
}

void World::setLength(unsigned int newLength)
{
	if (newLength <= 0)
		throw WorldSizeException("Trying to assign wrong value to world length (must be positive)");

	length = newLength;
}

void World::setDepth(unsigned int newDepth)
{
	if (newDepth <= 0)
		throw WorldSizeException("Trying to assign wrong value to world depth (must be positive)");

	depth = newDepth;
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

void World::setCellSize(float newCellSize)
{
	if (newCellSize <= 0)
		throw WorldSizeException("Trying to assign wrong value to world cells size (must be positive)");

	cellSize = newCellSize;
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

TerrainCell * World::getCell(unsigned int z, unsigned int y, unsigned int x)
{
// Check dimensions are correct and return NULL in case there is an error
	if (terrain.size() <= z || terrain[z].size() <= y || terrain[z][y].size() <= x || z < 0 || y < 0 || x < 0)
	{
		return NULL;
	}

// Dimensions are correct
	return terrain[z][y][x];
}


bool surroundingTerrainCellCompare(const std::pair<double, TerrainCell*>& firstElem, const std::pair<double, TerrainCell*>& secondElem) {
  return firstElem.first < secondElem.first;
}

TerrainCell * World::getCellByBearing(TerrainCell* position, TerrainCell* bestCell)
{
	int bearingZ = (bestCell->getZ() - position->getZ() > 0)?1:((bestCell->getZ() - position->getZ() < 0)?-1:0);
	int bearingY = (bestCell->getY() - position->getY() > 0)?1:((bestCell->getY() - position->getY() < 0)?-1:0);
	int bearingX = (bestCell->getX() - position->getX() > 0)?1:((bestCell->getX() - position->getX() < 0)?-1:0);
	TerrainCell* nextCellByBearing = getCell(position->getZ() + bearingZ, position->getY() + bearingY, position->getX() + bearingX);

	//Avoid obstacles using the least bearing difference path
	if(nextCellByBearing == NULL || nextCellByBearing->isObstacle())
	{
		vector<pair<double, TerrainCell*>> surroundingTerrainCells;
		int surroundingDistance = 1;
		// Put the TerrainCells in
		for (int newBearingZ = -surroundingDistance; newBearingZ <= surroundingDistance; ++newBearingZ)
		{
			for (int newBearingY = -surroundingDistance; newBearingY <= surroundingDistance; ++newBearingY)
			{
				for (int newBearingX = -surroundingDistance; newBearingX <= surroundingDistance; ++newBearingX)
				{
					TerrainCell* currentSurroundingTerrainCell = getCell(position->getZ() + newBearingZ, position->getY() + newBearingY, position->getX() + newBearingX);

					//Not adding the current animal position
					if(currentSurroundingTerrainCell != NULL && !currentSurroundingTerrainCell->isObstacle() && position != currentSurroundingTerrainCell)
					{
						double bearingDifference = abs((bearingZ - newBearingZ) + (bearingY - newBearingY) + (bearingX - newBearingX));

						//Adding some randomness to differenciate cells with the same bearingDifference
						bearingDifference += Random::randomUniform();
						surroundingTerrainCells.push_back(make_pair(bearingDifference, currentSurroundingTerrainCell));
					}
				}
			}
		}
		//Sort surrounding cells by bearing difference using ascending order
		std::sort(surroundingTerrainCells.begin(), surroundingTerrainCells.end(), surroundingTerrainCellCompare);
		nextCellByBearing = surroundingTerrainCells.front().second;
	}

	return nextCellByBearing;
}

void World::setHomogeneousResource(ResourceSpecies* species, double value, double resourceMaximumCapacity)
{
	Output::cout("Initializing homogeneous resource value for all cells ({}) ... \n\n", species->getScientificName());

	// Initialize all cells with a minimum water content
	TerrainCell* currentTerrainCell = NULL;
	if (value >= 0)
	{
		for (unsigned int z = 0; z < depth; ++z)
		{
			for (unsigned int y = 0; y < length; y++)
			{
				for (unsigned int x = 0; x < width; x++)
				{
					currentTerrainCell = getCell(z,y,x);
					if (!currentTerrainCell->isObstacle())
					{
						/*if(value > currentTerrainCell->getTotalMaximumResourceCapacity())
						{
							std::cerr << "For the species '" << species->getScientificName() << "':" << std::endl;
							std::cerr << "TerrainCell (" << x << "," << y << "," << z << " ): Resource value must be lower than the cell totalMaximumResourceCapacity. You entered " << value << " > " << currentTerrainCell->getTotalMaximumResourceCapacity() << ". Exiting now" << std::endl;
							exit(-1);
						}*/
						Resource* aux = currentTerrainCell->getResource(species);

						if (aux == NULL)
						{
							currentTerrainCell->addResource(new Resource(species, value, resourceMaximumCapacity, getCompetitionAmongResourceSpecies(), massRatio));
							//TODO PROVISIONAL!!!!
							currentTerrainCell->setAuxInitialResourceBiomass(value, species->getId());
						}
						else
						{
							aux->setBiomass(max(value, aux->calculateWetMass()));
							//TODO PROVISIONAL!!!!
							currentTerrainCell->setAuxInitialResourceBiomass(max(value, aux->calculateWetMass()), species->getId());
						}
					}
				}
			}
		}
	}
	else
	{
		std::cerr << "For the species '" << species->getScientificName() << "':" << std::endl;
		std::cerr << "Resource value must be 0 or positive. You entered " << value << ". Exiting now" << std::endl;
		exit(-1);
	}
	Output::cout("DONE\n");
}

void World::setGaussianResourcePatch(ResourceSpecies* species, unsigned int xpos, unsigned int ypos, unsigned int zpos,
		unsigned int radius, float sigma, float amplitude, double resourceMaximumCapacity)
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
	Output::cout("DONE\n");

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
									currentTerrainCell->addResource(new Resource(species, resourceAsGauss, resourceMaximumCapacity, getCompetitionAmongResourceSpecies(), massRatio));
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

void World::setSphericalResourcePatch(ResourceSpecies* species, unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, double value, double resourceMaximumCapacity)
{
	if (value >= 0)
	{
		// Generate water patches

		// Generate coordinates for the center of the patch
		Coordinate3D<int> center(xpos, ypos, zpos);

		Output::cout("Initializing spherical resource patch ({}) ... \n", species->getScientificName());

		Output::cout(" - Position (x,y,z) = {},{},{} ... ", xpos, ypos, zpos);
		Output::cout(" - Parameters (radius, value) = {},{} ... ", radius, value);

		
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


									float distance = sqrt((center.getX() - x) * (center.getX() - x) + (center.getY() - y) * (center.getY() - y) + (center.getZ() - z) * (center.getZ() - z));

									if (distance <= radius)
									{
										/*if(value > currentTerrainCell->getTotalMaximumResourceCapacity())
										{
											std::cerr << "For the species '" << species->getScientificName() << "':" << std::endl;
											std::cerr << "TerrainCell (" << x << "," << y << "," << z << " ): Resource value must be lower than the cell totalMaximumResourceCapacity. You entered " << value << " > " << currentTerrainCell->getTotalMaximumResourceCapacity() << ". Exiting now" << std::endl;
											exit(-1);
										}*/

										Resource* aux = currentTerrainCell->getResource(species);

										if (aux == NULL)
										{
											currentTerrainCell->addResource(new Resource(species, value, resourceMaximumCapacity, getCompetitionAmongResourceSpecies(), massRatio));

											Output::cout("RECENTLY ADDED VALUE: {}\n", currentTerrainCell->getTotalResourceBiomass());
											//TODO PROVISIONAL!!!!
											currentTerrainCell->setAuxInitialResourceBiomass(value, species->getId());
																		
										}
										else
										{
											aux->setBiomass(max(value, aux->calculateWetMass()));
											//TODO PROVISIONAL!!!!
											currentTerrainCell->setAuxInitialResourceBiomass(max(value, aux->calculateWetMass()), species->getId());
										
										}

										 //double diag;
										 //diag = sqrt(2*(currentTerrainCell->getTheWorld()->getWidth()*currentTerrainCell->getTheWorld()->getWidth()));
										 //if ((radius - distance) >= diag){ // round(currentTerrainCell->getTheWorld()->getWidth()/2)){
										 currentTerrainCell->setInPatch(true);//  
										 /* cout << currentTerrainCell->isInPatch() << endl;
										 cout << currentTerrainCell->getX() << endl;
										 cout << currentTerrainCell->getY() << endl;
										 cout << currentTerrainCell->getZ() << endl; */
										 //} 

										



										
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		std::cerr << "For the species '" << species->getScientificName() << "':" << std::endl;
		std::cerr << "Resource value must be 0 or positive. You entered " << value << ". Exiting now" << std::endl;
		exit(-1);
	}
	Output::cout("DONE\n\n");
}

void World::setCubicResourcePatch(ResourceSpecies* species, Coordinate3D<int> center, Coordinate3D<int> dimensions, double value, double resourceMaximumCapacity)
{	
	if (value >= 0)
	{
		Output::cout("Initializing cubic resource patch ({}) ... \n", species->getScientificName());

		Output::cout(" - Position (x,y,z) = {},{},{} ... ", center.getX(), center.getY(), center.getZ());
		Output::cout(" - Parameters (dimensions, value) = {},{},{},{} ... ", dimensions.getX(), dimensions.getY(), dimensions.getZ(), value);

		// Now iterate around the center to fill up with new water contents
		TerrainCell* currentTerrainCell = NULL;

		// Defining limits of patch
		int minx, maxx, miny, maxy, minz, maxz;

		minx = ceil(center.getX()-dimensions.getX()/2.);
		maxx = ceil(center.getX()+dimensions.getX()/2.) - 1;
		miny = ceil(center.getY()-dimensions.getY()/2.);
		maxy = ceil(center.getY()+dimensions.getY()/2.) - 1;
		minz = ceil(center.getZ()-dimensions.getZ()/2.);
		maxz = ceil(center.getZ()+dimensions.getZ()/2.) - 1;

		for (int x = minx; x <= maxx; x++)
		{
			if (x >= 0 && x < (int) width) // Ckeck limits
			{
				for (int y = miny; y <= maxy; y++)
				{
					if (y >= 0 && y < (int) length) // Ckeck limits
					{
						for (int z = minz; z <= maxz; z++)
						{
							if (z >= 0 && z < (int) depth) // Ckeck limits
							{
								currentTerrainCell = getCell(z,y,x);
								if (!currentTerrainCell->isObstacle())
								{
										Resource* aux = currentTerrainCell->getResource(species);

										if (aux == NULL)
										{
											currentTerrainCell->addResource(new Resource(species, value, resourceMaximumCapacity, getCompetitionAmongResourceSpecies(), massRatio));

											Output::cout("RECENTLY ADDED VALUE: {}\n", currentTerrainCell->getTotalResourceBiomass());
											//TODO PROVISIONAL!!!!
											currentTerrainCell->setAuxInitialResourceBiomass(value, species->getId());
																		
										}
										else
										{
											aux->setBiomass(max(value, aux->calculateWetMass()));
											//TODO PROVISIONAL!!!!
											currentTerrainCell->setAuxInitialResourceBiomass(max(value, aux->calculateWetMass()), species->getId());
										
										}

										currentTerrainCell->setInPatch(true);//  			
									
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		std::cerr << "For the species '" << species->getScientificName() << "':" << std::endl;
		std::cerr << "Resource value must be 0 or positive. You entered " << value << ". Exiting now" << std::endl;
		exit(-1);
	}
	Output::cout("DONE\n\n");
}

void World::setRandomGaussianResourcePatches(ResourceSpecies* species, unsigned int number, float radius, float newSigma, bool useRandomSigma, float newAmplitude, bool useRandomAmplitude, double resourceMaximumCapacity)
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

		Output::cout("DONE\n");

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
										currentTerrainCell->addResource(new Resource(species, resourceAsGauss, resourceMaximumCapacity, getCompetitionAmongResourceSpecies(), massRatio));
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

void World::setHomogeneousWater(float value, float relativeHumidityDecayOverTime)
{
	Output::cout("Initializing homogeneous water ({}) for all space ... ", value);
	// Initialize all cells with a minimum water content

	if (value >= 0)
	{
		if(relativeHumidityDecayOverTime >= 0)
		{
			TerrainCell* currentTerrainCell = NULL;
			for (unsigned int z = 0; z < depth; ++z)
			{
				for (unsigned int y = 0; y < length; y++)
				{
					for (unsigned int x = 0; x < width; x++)
					{
						currentTerrainCell = getCell(z,y,x);
						if (!currentTerrainCell->isObstacle())
						{
							currentTerrainCell->setRelativeHumidityOnRainEvent(value);
						}
					}
				}
			}
		}
		else
		{
			std::cerr << "Error, relativeHumidityDecayOverTime must be a 0 or positive value. You entered "
					<< relativeHumidityDecayOverTime << " ... EXITING" << std::endl;
			exit(-1);
		}
	}
	else
	{
		std::cerr << "You have entered a negative homogeneous water value (" << value
				<< "). Please correct it and re-run." << std::endl;
		exit(0);
	}

	Output::cout("DONE\n");
}

void World::setCubicObstaclePatch(string patchFilename, unsigned int depthStartPoint, unsigned int lengthStartPoint, unsigned int widthStartPoint, unsigned int patchDepth, unsigned int patchLength,
		unsigned int patchWidth)
{
	Output::cout("Initializing cubic obstacle patch \"{}\":\n", patchFilename);
	Output::cout(" - Position (x,y,z) = ({},{},{})\n", widthStartPoint, lengthStartPoint, depthStartPoint);
	Output::cout(" - Parameters (width, length, depth) = ({},{},{}) ... ", patchWidth, patchLength, patchDepth);
	try
	{
		for (unsigned int z = depthStartPoint; z < depthStartPoint + patchDepth; z++)
		{
			if (z < depth)
			{
				for (unsigned int y = lengthStartPoint; y < lengthStartPoint + patchLength; y++)
				{
					if (y < length)
					{
						for (unsigned int x = widthStartPoint; x < widthStartPoint + patchWidth; x++)
						{
							if (x < width)
							{
								if (!getCell(z,y,x)->isObstacle())
								{
									getCell(z,y,x)->setObstacle(true);
								}
								else
								{
									Output::cout("WARNING: The patch from file \"{}\" overlaps with the previously declared obstacle\n", patchFilename);
								}
							}
							else
							{
								cerr << "The patch entered contains a region which is out of the terrain length bounds (" << x << " >= " << width << "). Please correct it and re-run." << endl;
								exit(-1);
							}
						}
					}
					else
					{
						cerr << "The patch entered contains a region which is out of the terrain length bounds (" << y << " >= " << length << "). Please correct it and re-run." << endl;
						exit(-1);
					}
				}
			}
			else
			{
				cerr << "The patch entered contains a region which is out of the terrain depth bounds (" << z << " >= " << depth << "). Please correct it and re-run." << endl;
				exit(-1);
			}
		}
	} catch (std::bad_alloc&) // Treat different exceptions
	{
		std::cerr << "caught bad alloc exception, perhaps out of memory?, exiting.." << std::endl;
		exit(-1);
	} catch (const std::exception& x)
	{
		std::cerr << typeid(x).name() << std::endl;
	} catch (...)
	{
		std::cerr << "unknown exception" << std::endl;
	}
	Output::cout("DONE\n");
}

void World::setSphericalWaterPatch(string patchFilename, unsigned int radius, unsigned int xpos, unsigned int ypos, unsigned int zpos, bool useRelativeHumidityCycle, const vector<float>& temperatureCycle, const vector<float>& relativeHumidityCycle, bool useRelativeHumidityDecayOverTime, float relativeHumidityOnRainEvent, float relativeHumidityDecayOverTime, int timeStepsBetweenRainEvents, int standardDeviationForRainEvent, float totalMaximumResourceCapacity, bool inEnemyFreeSpace, bool inCompetitorFreeSpace)
{
	if(useRelativeHumidityCycle != useRelativeHumidityDecayOverTime)
	{
		if (relativeHumidityOnRainEvent > 0)
		{
			if(relativeHumidityDecayOverTime >= 0)
			{
				// Generate water patches
				Output::cout("Initializing spherical water patch \"{}\":\n", patchFilename);
				Output::cout(" - Position (x,y,z) = ({},{},{})\n", xpos, ypos, zpos);
				Output::cout(" - Parameters (radius, value) = ({},{}) ... ", radius, relativeHumidityOnRainEvent);

				// Generate coordinates for the center of the patch
				Coordinate3D<int> center(xpos, ypos, zpos);

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
											float distance = sqrt((center.getX() - x) * (center.getX() - x) + (center.getY() - y) * (center.getY() - y) + (center.getZ() - z) * (center.getZ() - z));

											if (distance <= radius)
											{
												currentTerrainCell->setTemperatureCycle(temperatureCycle);
												currentTerrainCell->setRelativeHumidityCycle(relativeHumidityCycle);
												currentTerrainCell->setRelativeHumidityOnRainEvent(relativeHumidityOnRainEvent);
												currentTerrainCell->setRelativeHumidityDecayOverTime(relativeHumidityDecayOverTime);
												currentTerrainCell->setTimeStepsBetweenRainEvents(timeStepsBetweenRainEvents);
												currentTerrainCell->setStandardDeviationForRainEvent(standardDeviationForRainEvent);
												currentTerrainCell->setMaximumCapacities(totalMaximumResourceCapacity);
												currentTerrainCell->setInEnemyFreeSpace(inEnemyFreeSpace);
												currentTerrainCell->setInCompetitorFreeSpace(inCompetitorFreeSpace);
												
												/* cout << "before   " << endl;
												cout << currentTerrainCell->isInPatch() << endl; */
												
												//arthro and for dinos - so resource does not spread in cells that are supposed to be empty  
												//if (distance-radius) >= 0){ //so edges are not used to spread resource causing no error
											 	
												
												//cout << currentTerrainCell->isInPatch() <<endl;
												
												//cout << currentTerrainCell->getWater() << endl;
												
											}
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				std::cerr << "Error, in patch " << patchFilename << " relativeHumidityDecayOverTime must be a 0 or positive value. You entered "
						<< relativeHumidityDecayOverTime << " ... EXITING" << std::endl;
				exit(-1);
			}
		}
		else
		{
			cerr << "In patch " << patchFilename << ", you have entered a negative spherical water value (" << relativeHumidityOnRainEvent << "). Please correct it and re-run." << endl;
			exit(-1);
		}
	}
	else
	{
		cerr << "In patch " << patchFilename << ", you must use at least and maximum one humidity method (useRelativeHumidityCycle must be different from useRelativeHumidityDecayOverTime). Please correct it and re-run." << endl;
		exit(-1);
	}
	Output::cout("DONE\n\n");
}

void World::setCubicWaterPatch(string patchFilename, Coordinate3D<int> center, Coordinate3D<int> dimensions,  bool useRelativeHumidityCycle, const vector<float>& temperatureCycle, const vector<float>& relativeHumidityCycle, bool useRelativeHumidityDecayOverTime, float relativeHumidityOnRainEvent, float relativeHumidityDecayOverTime, int timeStepsBetweenRainEvents, int standardDeviationForRainEvent, float totalMaximumResourceCapacity, bool inEnemyFreeSpace, bool inCompetitorFreeSpace)
{
	if(useRelativeHumidityCycle != useRelativeHumidityDecayOverTime)
	{
		if (relativeHumidityOnRainEvent > 0)
		{
			if(relativeHumidityDecayOverTime >= 0)
			{
				// Generate water patches
				Output::cout("Initializing cubic water patch \"{}\":\n", patchFilename);
				Output::cout(" - Center position (x,y,z) = ({},{},{})\n", center.getX(), center.getY(), center.getZ());
				Output::cout(" - Dimensions (xdim, ydim, zdim, value) = ({},{},{},{}) ... ", dimensions.getX(), dimensions.getY(), dimensions.getZ(), relativeHumidityOnRainEvent);

				// Defining limits of patch
				int minx, maxx, miny, maxy, minz, maxz;

				minx = ceil(center.getX()-dimensions.getX()/2.);
				maxx = ceil(center.getX()+dimensions.getX()/2.) - 1;
				miny = ceil(center.getY()-dimensions.getY()/2.);
				maxy = ceil(center.getY()+dimensions.getY()/2.) - 1;
				minz = ceil(center.getZ()-dimensions.getZ()/2.);
				maxz = ceil(center.getZ()+dimensions.getZ()/2.) - 1;

				// Now iterate around the center to fill up with new water contents
				TerrainCell* currentTerrainCell = NULL;
				for (int x = minx; x <= maxx; x++)
				{
					if (x >= 0 && x < (int) width) // Ckeck limits
					{
						for (int y = miny; y <= maxy; y++)
						{
							if (y >= 0 && y < (int) length) // Ckeck limits
							{
								for (int z = minz; z <= maxz; z++)
								{
									if (z >= 0 && z < (int) depth) // Ckeck limits
									{
										currentTerrainCell = getCell(z,y,x);
										if (!currentTerrainCell->isObstacle())
										{
											currentTerrainCell->setTemperatureCycle(temperatureCycle);
											currentTerrainCell->setRelativeHumidityCycle(relativeHumidityCycle);
											currentTerrainCell->setRelativeHumidityOnRainEvent(relativeHumidityOnRainEvent);
											currentTerrainCell->setRelativeHumidityDecayOverTime(relativeHumidityDecayOverTime);
											currentTerrainCell->setTimeStepsBetweenRainEvents(timeStepsBetweenRainEvents);
											currentTerrainCell->setStandardDeviationForRainEvent(standardDeviationForRainEvent);
											currentTerrainCell->setMaximumCapacities(totalMaximumResourceCapacity);
											currentTerrainCell->setInEnemyFreeSpace(inEnemyFreeSpace);
											currentTerrainCell->setInCompetitorFreeSpace(inCompetitorFreeSpace);											
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				std::cerr << "Error, in patch " << patchFilename << " relativeHumidityDecayOverTime must be a 0 or positive value. You entered "
						<< relativeHumidityDecayOverTime << " ... EXITING" << std::endl;
				exit(-1);
			}
		}
		else
		{
			cerr << "In patch " << patchFilename << ", you have entered a negative spherical water value (" << relativeHumidityOnRainEvent << "). Please correct it and re-run." << endl;
			exit(-1);
		}
	}
	else
	{
		cerr << "In patch " << patchFilename << ", you must use at least and maximum one humidity method (useRelativeHumidityCycle must be different from useRelativeHumidityDecayOverTime). Please correct it and re-run." << endl;
		exit(-1);
	}
	Output::cout("DONE\n\n");
}

void World::setGaussianWaterPatch(unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, float sigma, float amplitude)
{
	// Generate water patches
	IsotropicGaussian3D gauss;

	Output::cout("Initializing Gaussian water patch ... \n\n");

	Output::cout(" - Position (x,y,z) = {},{},{} ... ", xpos, ypos, zpos);
	Output::cout(" - Parameters (Influence radius, Amplitude, Sigma) = {},{},{} ... ", radius, amplitude, sigma);

	gauss.setSigma(sigma);
	gauss.setAmplitude(amplitude);
	Output::cout("DONE\n");

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

		Output::cout("DONE\n");

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

double World::getPdfThreshold() const
{
	switch (getSimulType()) {
		case SimulType::dinosaurs: {
			return 0.0003; //dinosaurs
			break;
		}
		case SimulType::arthropods: {
			return 0.08; //arthropods
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}

void World::calculateAttackStatistics(map<AnimalSpecies*,vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells)
{
	Output::cout("Size of the Animal class: {}\n", sizeof(Animal));
	Output::cout("Size of the Genome class: {}\n", sizeof(Genome));
	Output::cout("Size of the TerrainCell class: {}\n", sizeof(TerrainCell));
	Output::cout("Creating heating code individuals... \n");

	vector<pair<Animal *, Instar> > animalAndInstarAtInitialization;

	map<AnimalSpecies*, vector<Animal*>*> * animalsPopulation = generateStatisticsPopulation(animalAndInstarAtInitialization, mapSpeciesInhabitableTerrainCells);

	//Calculating pseudoGrowthSd
	for (auto animalAndInstarAtInitializationIt = animalAndInstarAtInitialization.begin(); animalAndInstarAtInitializationIt != animalAndInstarAtInitialization.end(); animalAndInstarAtInitializationIt++)
	{
		(*animalAndInstarAtInitializationIt).first->sumPseudoGrowthSd();
	}

	for (auto speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
	{
		(*speciesIt)->calculatePseudoGrowthSd();
	}

	for (auto animalAndInstarAtInitializationIt = animalAndInstarAtInitialization.begin(); animalAndInstarAtInitializationIt != animalAndInstarAtInitialization.end(); animalAndInstarAtInitializationIt++)
	{
		//TODO Ver en CUÁNTO cambian usando los 1.000.000 animales , probando el calentamiento 2 o 3 veces
		(*animalAndInstarAtInitializationIt).first->interpolateTraits();
		//DONE Usar calculateGrowthCurves CADA DÍA , y no al nacer.
		//DONE ELIMINAR SPEED DEL ADJUST TRAITS YA QUE QUEDA INCLUIDO EN EL TUNETRAITS DE ABAJO
		(*animalAndInstarAtInitializationIt).first->adjustTraits();
		//DONE Forzar el crecimiento de forma DIARIA y utilizando growtcurves+tunetraits CADA DÍA para el ciclo de temperaturas
		float temperatureOnMolting = (*animalAndInstarAtInitializationIt).first->forceMolting(timeStepsPerDay, -1, (*animalAndInstarAtInitializationIt).second, getSimulType(), &World::getCell);
		(*animalAndInstarAtInitializationIt).first->tuneTraits(-1, timeStepsPerDay, temperatureOnMolting, 100, Output::nullFile, false, true, getSimulType(), &World::getCell);

		updateMaxSearchArea((*animalAndInstarAtInitializationIt).first->getSearchArea());

		//DONE tuneTraits tiene que ir AQUI y ADEMÁS con la temperatura final en la que mudaron CADA UNO DE ELLOS
	}
	Output::cout("A total of {} heating code individuals have been created.\n", animalAndInstarAtInitialization.size());

	//Only for predators. The experiment is carried out for every predator species and its linked species, up until the specified numberOfCombinations.

	//TODO MIRAR MEMORY LEAKS

	Output::cout("Calculating attack statistics: \n");
	for (auto speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = (*speciesIt);
		const vector<AnimalSpecies* >* currentEdibleAnimalSpecies = currentAnimalSpecies->getEdibleAnimalSpecies();

		if(!currentEdibleAnimalSpecies->empty())
		{
			Output::cout(">> Simulating {} attacks from the species \"{}\"... \n", numberOfCombinations, currentAnimalSpecies->getScientificName());
			vector<Animal* >* currentVectorOfPredators = (*animalsPopulation)[currentAnimalSpecies];
			
			vector<Animal*> auxAttacks;
			for (auto innerSpeciesIt = currentEdibleAnimalSpecies->begin(); innerSpeciesIt != currentEdibleAnimalSpecies->end(); innerSpeciesIt++)
			{
				vector<Animal*> * currentVectorOfEdibleAnimals = (*animalsPopulation)[(*innerSpeciesIt)];
				auxAttacks.insert(auxAttacks.end(), currentVectorOfEdibleAnimals->begin(), currentVectorOfEdibleAnimals->end());
			}

			float percentageForPrinting = 0.1;
			float currentPercentageForPrinting = percentageForPrinting;
			vector<pair<Animal*, Animal*>> vectorOfAttacks;
			vectorOfAttacks.reserve(numberOfCombinations);
			while(vectorOfAttacks.size() < numberOfCombinations)
			{
				Animal* hunterAnimal = currentVectorOfPredators->at(Random::randomIndex(currentVectorOfPredators->size()));
				Animal* huntedAnimal = auxAttacks.at(Random::randomIndex(auxAttacks.size()));
				//cout << "CAN EAT ANIMAL??? <" << huntedAnimal->getId() << " _ " << hunterAnimal->getId() << endl;
				
				double probabilityDensityFunction = exp(-0.5 * pow((log(hunterAnimal->calculateWetMass()/huntedAnimal->calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
				
				double pdfThreshold = getPdfThreshold(); //to avoid feeding on too small prey
				
				if(probabilityDensityFunction >= pdfThreshold){  //Dinosaurs but everything else might be - p=0.003 from Alamosaurus - Tytannsaurus in PDF_fix_in heating_code.xls
					if(hunterAnimal->canEatEdible(huntedAnimal) && hunterAnimal != huntedAnimal)
					{
						//cout << "CAN EAT ANIMAL!!! <" << huntedAnimal->getId() << " _ " << hunterAnimal->getId() << endl;
						pair<Animal*, Animal*> currentAttack = make_pair(hunterAnimal, huntedAnimal);
						if(find(vectorOfAttacks.begin(), vectorOfAttacks.end(), currentAttack) == vectorOfAttacks.end())
						{
							vectorOfAttacks.push_back(currentAttack);

							//Computing the total mean values.
							currentAnimalSpecies->sumStatisticMeans(hunterAnimal->getCurrentBodySize(), hunterAnimal->getVoracity(), hunterAnimal->getSpeed(), hunterAnimal->calculateDryMass(), huntedAnimal->getCurrentBodySize(), huntedAnimal->getVoracity(), huntedAnimal->getSpeed(), huntedAnimal->calculateDryMass(), muForPDF, sigmaForPDF);
							currentAnimalSpecies->interactionRanges(hunterAnimal->getCurrentBodySize(), hunterAnimal->getVoracity(), hunterAnimal->getSpeed(), hunterAnimal->calculateDryMass(), huntedAnimal->getCurrentBodySize(), huntedAnimal->getVoracity(), huntedAnimal->getSpeed(), huntedAnimal->calculateDryMass(), muForPDF, sigmaForPDF);
						}
					}
					if(vectorOfAttacks.size() >= (numberOfCombinations-1)*currentPercentageForPrinting)
					{
						Output::cout(">>>> {}%... \n", (int)(currentPercentageForPrinting*100));
						currentPercentageForPrinting += percentageForPrinting;
					}
				}
			}

			//Computing the means
			currentAnimalSpecies->computeStatisticMeans(vectorOfAttacks.size());

			for(unsigned int i = 0; i < vectorOfAttacks.size(); i++)
			{
				//Computing the total sd values.
				Animal* hunterAnimal = vectorOfAttacks[i].first;
				Animal* huntedAnimal = vectorOfAttacks[i].second;

				double probabilityDensityFunction = exp(-0.5 * pow((log(hunterAnimal->calculateWetMass()/huntedAnimal->calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
				
				double pdfThreshold = getPdfThreshold();
				
				if(probabilityDensityFunction >= pdfThreshold){  //Dinosaurs but everything else might be - p=0.003 from Alamosaurus - Tytannsaurus in PDF_fix_in heating_code.xls

				currentAnimalSpecies->sumStatisticSds(hunterAnimal->getCurrentBodySize(), hunterAnimal->getVoracity(), hunterAnimal->getSpeed(), hunterAnimal->calculateDryMass(), huntedAnimal->getCurrentBodySize(), huntedAnimal->getVoracity(), huntedAnimal->getSpeed(), huntedAnimal->calculateDryMass(), muForPDF, sigmaForPDF);

				}

			}

			//Computing the sds
			currentAnimalSpecies->computeStatisticSds(vectorOfAttacks.size());
			vectorOfAttacks.clear();
			auxAttacks.clear();
		}
	}

	eraseStatisticsPopulation(animalsPopulation);

	Edible::resetIds(Resource::resourceCounter);
	Animal::animalCounter = 0;
	Output::cout("Calculating attack statistics DONE\n");
}

void World::updateMaxSearchArea(double currentAnimalMaxSearchArea)
{
	if(currentAnimalMaxSearchArea > maxSearchArea)
	{
		maxSearchArea = currentAnimalMaxSearchArea;
	}
}

void World::printActualEcosystemSize(const vector<TerrainCell*> &inhabitableTerrainCells)
{
	double actualEcosystemSize = 0;

	if(initIndividualsPerDensities)
	{
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
		for (auto speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
		{
			actualEcosystemSize += (*speciesIt)->getTotalInitialPopulation();
		}
		Output::cout(" - Total initial ecosystem size from input: {} individuals.\n", actualEcosystemSize);
	}
}

void World::obtainSpeciesInhabitableTerrainCells(map<AnimalSpecies*,vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells, const vector<TerrainCell*> &inhabitableTerrainCells)
{
	for (auto speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *speciesIt;

		vector<ResourceSpecies*> * involvedResourceSpecies = currentAnimalSpecies->getInvolvedResourceSpecies();

		vector<TerrainCell*>* speciesInhabitableTerrainCells = new vector<TerrainCell*>();

		bool cellHasBeenAdded;

		TerrainCell* currentTerrainCell = NULL;
		for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
		{
			currentTerrainCell = inhabitableTerrainCells[i];
			cellHasBeenAdded = false;
			for(unsigned int j = 0; j < involvedResourceSpecies->size() && !cellHasBeenAdded; j++)
			{
				if(currentTerrainCell->containsResourceSpecies(involvedResourceSpecies->at(j)))
				{
					speciesInhabitableTerrainCells->push_back(currentTerrainCell);
					cellHasBeenAdded = true;
				}
			}
		}

		mapSpeciesInhabitableTerrainCells[currentAnimalSpecies] = speciesInhabitableTerrainCells;

		delete involvedResourceSpecies;
	}
}

map<AnimalSpecies*, vector<Animal*>*>* World::generateStatisticsPopulation(vector<pair<Animal *, Instar> > &animalAndInstarAtInitialization, map<AnimalSpecies*,vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells)
{
	map<AnimalSpecies*, vector<Animal*>*> * animalsPopulation = new map<AnimalSpecies*, vector<Animal*>*>(); 

	//TODO Separarlo TOTALMENTE del mundo de la simulación
	for (auto speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
	{
		AnimalSpecies* currentAnimalSpecies = *speciesIt;

		(*animalsPopulation)[currentAnimalSpecies] = new vector<Animal*>();

		//TODO AÑADIR que solamente haya un PORCENTAJE de individuos en cada instar (un vector solamente), a partir de un numero total (un total para simulacion, y otro total diferente para estadisticas)
		vector<unsigned long>* initialPopulation = currentAnimalSpecies->getStatisticsInitialPopulation();
		vector<TerrainCell*>* speciesInhabitableTerrainCells = mapSpeciesInhabitableTerrainCells[currentAnimalSpecies];

		float percentageForPrinting = 0.1;
		float currentPercentageForPrinting = percentageForPrinting;
		unsigned int actualPopulationInstarsSize = 0;
		for (unsigned int i = 0; i < initialPopulation->size(); ++i)
		{
			if(initialPopulation->at(i)>0)
			{
				actualPopulationInstarsSize++;
			}
		}

		unsigned int randomCellIndex;

		for (unsigned int instarAtInitialization = 1; instarAtInitialization <= initialPopulation->size(); ++instarAtInitialization)
		{
			Instar instar(instarAtInitialization);
			for (unsigned long individual = 0; individual < initialPopulation->at(instar.getValue()); individual++)
			{
				// Get a random index from this species inhabitable cells
				randomCellIndex = Random::randomIndex(speciesInhabitableTerrainCells->size());
				TerrainCell * newCell = (*speciesInhabitableTerrainCells)[randomCellIndex];

				Animal * newAnimal = new Animal(-1, newCell, 0, timeStepsPerDay, 0, 0, -1,
						-1, currentAnimalSpecies, currentAnimalSpecies->getRandomGender());

				newAnimal->updateLimits();
				newAnimal->sumPseudoGrowthMean();
				
				newCell->addAnimal(newAnimal);

				animalAndInstarAtInitialization.push_back(make_pair(newAnimal, instar));

				(*animalsPopulation)[currentAnimalSpecies]->push_back(newAnimal);
			}
			if(instarAtInitialization-1 >= (actualPopulationInstarsSize-1)*currentPercentageForPrinting)
			{
				Output::cout(">>>> {}%... \n", (int)(currentPercentageForPrinting*100));
				currentPercentageForPrinting += percentageForPrinting;
			}
		}
		currentAnimalSpecies->calculatePseudoGrowthMean();
	}

	return animalsPopulation;
}

void World::eraseStatisticsPopulation(map<AnimalSpecies*, vector<Animal*>*> * animalsPopulation)
{
	for(auto &[key, value] : *animalsPopulation)
	{
		for(Animal* currentAnimal : *value)
		{
			TerrainCell* currentAnimalPosition = currentAnimal->getPosition();

			currentAnimalPosition->eraseAnimal(currentAnimal, currentAnimal->getLifeStage());

			delete currentAnimal;
		}

		delete value;
	}
	
	animalsPopulation->clear();

	delete animalsPopulation;
}

int World::generatePopulation(AnimalSpecies* currentAnimalSpecies, const vector<TerrainCell*> &speciesInhabitableTerrainCells)
{
	int numberOfDiscardedIndividualsOutsideRestrictedRanges = 0;

	Output::cout("Creating {} individuals of the species \"{}\"...\n", currentAnimalSpecies->getTotalInitialPopulation(), currentAnimalSpecies->getScientificName());
	Output::cout(">> Number of inhabitable cells (with involved resources): {}\n", speciesInhabitableTerrainCells.size());

	float percentageForPrinting = 0.1;
	float currentPercentageForPrinting = percentageForPrinting;

	unsigned int randomCellIndex;

	for (unsigned int instarAtInitialization = 1; instarAtInitialization <= currentAnimalSpecies->getInitialPopulation().size(); ++instarAtInitialization)
	{
		Instar instar(instarAtInitialization);
		for (unsigned long individual = 0; individual < currentAnimalSpecies->getInitialPopulation().at(instar.getValue()); individual++)
		{
			// Get a random index from this species inhabitable cells
			randomCellIndex = Random::randomIndex(speciesInhabitableTerrainCells.size());
			TerrainCell * newCell = speciesInhabitableTerrainCells[randomCellIndex];

			Animal * newAnimal = new Animal(-1, newCell, 0, timeStepsPerDay, 0, 0, -1,
					-1, currentAnimalSpecies, currentAnimalSpecies->getRandomGender(), true);

			pair<bool, bool> isInsideRestrictedRangesAndIsViableOffSpring = newAnimal->interpolateTraits();
			while(!isInsideRestrictedRangesAndIsViableOffSpring.first)
			{
				numberOfDiscardedIndividualsOutsideRestrictedRanges++;
				delete newAnimal;
				newAnimal = new Animal(-1, newCell, 0, timeStepsPerDay, 0, 0, -1,
										-1, currentAnimalSpecies, currentAnimalSpecies->getRandomGender(), true);
				isInsideRestrictedRangesAndIsViableOffSpring = newAnimal->interpolateTraits();
			}
			// Indicate that the animal created is the final animal, and therefore assign a final ID to it.
			newAnimal->doDefinitive();
			//ALWAYS print the traits after interpolating and before adjusting
			//newAnimal->printGenetics(geneticsFile);

			if(saveAnimalConstitutiveTraits) {
				newAnimal->printTraits(constitutiveTraitsFile);
			}
			
			//DONE Adjust traits now includes the first CalculateGrowthCurves call. For every animal.
			newAnimal->adjustTraits();
			newAnimal->forceMolting2(timeStepsPerDay, -1, instar);
			//exit(-1);
			newCell->addAnimal(newAnimal);
		}
		if(instarAtInitialization-1 >= (currentAnimalSpecies->getInitialPopulation().size()-1)*currentPercentageForPrinting)
		{
			Output::cout(">> {}%... \n", (int)(currentPercentageForPrinting*100));
			currentPercentageForPrinting += percentageForPrinting;
		}
	}

	return numberOfDiscardedIndividualsOutsideRestrictedRanges;
}

void World::initializeAnimals()
{
	vector<TerrainCell*> inhabitableTerrainCells;
	obtainInhabitableTerrainCells(inhabitableTerrainCells);

	printActualEcosystemSize(inhabitableTerrainCells);

	map<AnimalSpecies*,vector<TerrainCell*>*> mapSpeciesInhabitableTerrainCells;
	obtainSpeciesInhabitableTerrainCells(mapSpeciesInhabitableTerrainCells, inhabitableTerrainCells);

	calculateAttackStatistics(mapSpeciesInhabitableTerrainCells);

	Output::cout("Giving life to animals... \n");

	FILE* geneticsFile = nullptr;

	createOutputFile(&geneticsFile, outputFolder, "animal_genetics", "txt");
	if(!isFileOpen(geneticsFile))
	{
		Output::cerr("Error opening the file.\n");
	}
	else
	{
		Output::print(geneticsFile, "id\tspecies\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\t\n");

		for (auto speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
		{
			AnimalSpecies* currentAnimalSpecies = *speciesIt;

			vector<TerrainCell*>* speciesInhabitableTerrainCells = mapSpeciesInhabitableTerrainCells[currentAnimalSpecies];
							
			int numberOfDiscardedIndividualsOutsideRestrictedRanges = 0;

			if(initIndividualsPerDensities)
			{
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
				numberOfDiscardedIndividualsOutsideRestrictedRanges = generatePopulation(currentAnimalSpecies, *speciesInhabitableTerrainCells);
			}

			Output::cout(">> A total of {} individuals have been discarded due to at least one trait value being outside restricted ranges. \n", numberOfDiscardedIndividualsOutsideRestrictedRanges);
		}

		fclose(geneticsFile);
	}

	for(auto &[key,value] : mapSpeciesInhabitableTerrainCells)
	{
		delete value;
	}

	Output::cout("DONE\n");
}

void World::readWaterFromVolume(string fileName)
{
	Output::cout("Reading water from volume {}... ", fileName);
	fstream binf(fileName.c_str(), ios::binary | ios::in);

	double value;
	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = 0; z < depth; ++z)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				currentTerrainCell = getCell(z,y,x);
				if (!currentTerrainCell->isObstacle())
				{
					binf.read((char*) &value, sizeof(value));
					currentTerrainCell->setRelativeHumidityOnRainEvent(value);
				}
			}
		}
	}

	Output::cout("DONE\n");
	binf.close();
}

void World::initializeOutputFiles(json * jsonTree, fs::path inputFile)
{
	fs::create_directories(outputFolder / fs::path("config"));

	// Copy simulation configuration
	fs::copy(inputFolder, outputFolder / fs::path("config") / inputFolder.filename(), fs::copy_options::recursive);

	// Copy simulation parameters into output directory
	fs::copy_file(inputFile, outputFolder / fs::path("run_params.json"), fs::copy_options::overwrite_existing);

	dailySummaryFile = nullptr;
	
	createOutputFile(&dailySummaryFile, outputFolder, "dailySummary", "txt");

	if (!isFileOpen(dailySummaryFile))
	{
		Output::cerr("Error opening the dailySummaryFile.\n");
	}
	else
	{
		Output::print(dailySummaryFile, "DAY\tBIOMASS\tPREY_UNBORN\tPREY_ACTIVE\tPREY_STARVED\tPREY_PREDATED\tPREY_REPRODUCING\tPREY_SENESCED\tPREY_SHOCKED\tPREDATOR_UNBORN\tPREDATOR_ACTIVE\tPREDATOR_STARVED\tPREDATOR_PREDATED\tPREDATOR_REPRODUCING\tPREDATOR_BACKGROUND\tPREDATOR_SENESCED\tPREDATOR_SHOCKED\n");
	}

	extendedDailySummaryFile = nullptr;

	createOutputFile(&extendedDailySummaryFile, outputFolder, "extendedDailySummary", "txt");

	if (!isFileOpen(extendedDailySummaryFile))
	{
		Output::cerr("Error opening the dailySummaryFile.\n");
	}
	else
	{
		Output::print(extendedDailySummaryFile, "day\t");

		for (auto itResourceSpecies = existingResourceSpecies.begin(); itResourceSpecies != existingResourceSpecies.end(); itResourceSpecies++)
		{
			Output::print(extendedDailySummaryFile, "{}_biomass\t", (*itResourceSpecies)->getScientificName());
		}

		for (auto itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
		{
			for(const auto &lifeStage : LifeStage::getEnumValues())
			{
				Output::print(extendedDailySummaryFile, "{}_{}\t", (*itSpecies)->getScientificName(), LifeStage::to_string(lifeStage));
			}
		}
		Output::print(extendedDailySummaryFile, "\n");
	}

	if(saveGeneticsSummaries) {
		fs::create_directories(outputFolder / fs::path("geneticsSummaries"));

		geneticsSummaryFile.reserve(existingAnimalSpecies.size());

		for (auto itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
		{
			string scientificName = (*itSpecies)->getScientificName();
			std::replace(scientificName.begin(), scientificName.end(), ' ', '_');

			geneticsSummaryFile[*itSpecies] = nullptr;

			createOutputFile(&geneticsSummaryFile[*itSpecies], outputFolder / fs::path("geneticsSummaries"), scientificName + "_geneticsSummary", "txt");

			if (!isFileOpen(geneticsSummaryFile[*itSpecies]))
			{
				throwLineInfoException("Error opening the file '" + scientificName + "_geneticsSummary.txt'");
			}

			Output::print(geneticsSummaryFile[*itSpecies], "day\tpopulation\tenergy_cr1\tgrowth_cr1\tpheno_cr1\tbody_cr1\tassim_cr1\tvor_cr1\tspeed_cr1\tsearch_cr1\tmet_cr1\tactE_vor_cr1\tactE_spd_cr1\tactE_srch_cr1\te_met_cr1\tenergy_cr2\tgrowth_cr2\tpheno_cr2\tbody_cr2\tassim_cr2\tvor_cr2\tspeed_cr2\tsearch_cr2\tmet_cr2\tactE_vor_cr2\tactE_spd_cr2\tactE_srch_cr2\te_met_cr2\t\n");
		}
	}

	edibilitiesFile = nullptr;

	if(saveEdibilitiesFile)
	{
		createOutputFile(&edibilitiesFile, outputFolder, (*jsonTree)["simulation"]["edibilitiesFilename"], "txt");
	
		Output::print(edibilitiesFile, "timeStep\tsearcherId\tsearcherSpecies\tfoodMass\tpredatorId\tpredatorSpecies\tpredatorDryMass\tpredatedId\tpredatedSpecies\tpredatedDryMass\tencounterProbability\tpredationProbability\tpreference\texperience\tedibility\n");
	}
	
	constitutiveTraitsFile = nullptr;

	if(saveAnimalConstitutiveTraits) {
		createOutputFile(&constitutiveTraitsFile, outputFolder, "animal_constitutive_traits", "txt");

		if(!isFileOpen(constitutiveTraitsFile))
		{
			throwLineInfoException("Error opening the file 'animal_constitutive_traits.txt'");
		}

		Output::print(constitutiveTraitsFile, "id\tspecies\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\t{}\n", Trait::printAvailableTraits());
	}
}

#ifdef _PTHREAD
void* World::activateAnimalsThreadMaker(void* threadArgument)
{
	struct ThreadRangerArgument* argument = static_cast<ThreadRangerArgument*>(threadArgument);
	World* world = argument->world;
	unsigned int x0 = argument->x0;
	unsigned int x1 = argument->x1;
	unsigned int y0 = argument->y0;
	unsigned int y1 = argument->y1;
	unsigned int z0 = argument->z0;
	unsigned int z1 = argument->z1;
	int day = argument->day;
	world->activateAnimalsTask(x0, x1, y0, y1, z0, z1, day);
	return NULL;
}

void* World::activateAnimalsTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1, int day)
{
	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = z0; z < z1; z++)
	{
		for (unsigned int y = y0; y < y1; y++)
		{
			for (unsigned int x = x0; x < x1; x++)
			{
				currentTerrainCell = getCell(z,y,x);
				if(!currentTerrainCell->isObstacle())
				{
					currentTerrainCell->activateAndResumeAnimals(day);
					currentTerrainCell->tuneTraits();
				}
			}
		}
	}
	return NULL;
}

void* World::growResourceThreadMaker(void* threadArgument)
{
	struct ThreadRangerArgument* argument = static_cast<ThreadRangerArgument*>(threadArgument);
	World* world = argument->world;
	unsigned int x0 = argument->x0;
	unsigned int x1 = argument->x1;
	unsigned int y0 = argument->y0;
	unsigned int y1 = argument->y1;
	unsigned int z0 = argument->z0;
	unsigned int z1 = argument->z1;
	world->growResourceTask(x0, x1, y0, y1, z0, z1);
	return NULL;
}

void* World::growResourceTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1)
{
	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = z0; z < z1; z++)
	{
		for (unsigned int y = y0; y < y1; y++)
		{
			for (unsigned int x = x0; x < x1; x++)
			{
				currentTerrainCell = getCell(z,y,x);
				if(!currentTerrainCell->isObstacle())
				{
					currentTerrainCell->growResource();
				}
			}
		}
	}
	return NULL;
}

void* World::moveAnimalsThreadMaker(void* threadArgument)
{
	struct ThreadRangerArgument* argument = static_cast<ThreadRangerArgument*>(threadArgument);
	World* world = argument->world;
	unsigned int x0 = argument->x0;
	unsigned int x1 = argument->x1;
	unsigned int y0 = argument->y0;
	unsigned int y1 = argument->y1;
	unsigned int z0 = argument->z0;
	unsigned int z1 = argument->z1;
	int day = argument->day;
	world->moveAnimalsTask(x0, x1, y0, y1, z0, z1, day);
	return NULL;
}

void* World::moveAnimalsTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1, int day)
{
	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = z0; z < z1; z++)
	{
		for (unsigned int y = y0; y < y1; y++)
		{
			for (unsigned int x = x0; x < x1; x++)
			{
				currentTerrainCell = getCell(z,y,x);
				if(!currentTerrainCell->isObstacle())
				{
					currentTerrainCell->moveAnimals(day, cout, cout);
				}
			}
		}
	}
	return NULL;
}

void* World::assimilateThreadMaker(void* threadArgument)
{
	struct ThreadRangerArgument* argument = static_cast<ThreadRangerArgument*>(threadArgument);
	World* world = argument->world;
	unsigned int x0 = argument->x0;
	unsigned int x1 = argument->x1;
	unsigned int y0 = argument->y0;
	unsigned int y1 = argument->y1;
	unsigned int z0 = argument->z0;
	unsigned int z1 = argument->z1;
	int day = argument->day;
	world->assimilateTask(x0, x1, y0, y1, z0, z1, day);
	return NULL;
}

void* World::assimilateTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1, int day)
{
	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = z0; z < z1; z++)
	{
		for (unsigned int y = y0; y < y1; y++)
		{
			for (unsigned int x = x0; x < x1; x++)
			{
				currentTerrainCell = getCell(z,y,x);
				if(!currentTerrainCell->isObstacle())
				{
					//currentTerrainCell->printAnimalsVoracities(voracitiesFile);
					currentTerrainCell->diePredatorsFromBackground(day);
					currentTerrainCell->assimilateFoodMass();
					currentTerrainCell->growAnimals(day);
					currentTerrainCell->breedAnimals(day, timeStepsPerDay, outputFolder);
				}
			}
		}
	}
	return NULL;
}

#endif //_PTHREAD
