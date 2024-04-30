#include "World.h"
#include "TerrainCell.h"
/*
#include "tbb/tbb.h"
*/
#include "SimulationConstants.h"
#include <thread>
#include <algorithm> // transform
#include "Utilities.h"
#include "GlobalVariable.h"
#include "AnimalSpecies.h"

/*
using namespace tbb;
*/

World::World(json * jsonTree, fs::path outputDirectory)
{

	std::string simulTypeString = (*jsonTree)["simulation"]["simulationType"];
	if (simulTypeString == "arthropods")
	{
			simulType = 0;
	}
	else if (simulTypeString == "dinosaurs")
	{
			simulType = 1;
	}
	exitAtFirstExtinction = (*jsonTree)["simulation"]["exitAtFirstExtinction"];
	runDays = (*jsonTree)["simulation"]["runDays"];
	recordEach = (*jsonTree)["simulation"]["recordEach"];
	numberOfCombinations = (*jsonTree)["simulation"]["numberOfCombinations"];
	timeStepsPerDay = (*jsonTree)["simulation"]["timeStepsPerDay"];
	//TODO tunetraits and metab every X STEPS
	//TODO growth every X STEPS

	// Set output directory
	setOutputDirectory(outputDirectory);

	// Read space configuration
	string configDirectory;
	try
	{
		configDirectory = (*jsonTree)["simulation"].at("configDirectory");
	}
	catch(const json::out_of_range& e)
	{
		configDirectory = CONFIG_FOLDER;
	}
	string configName = (*jsonTree)["simulation"]["configName"];
	inputDirectory = fs::path(configDirectory) / fs::path(configName);

	json worldConfig = readConfigFile(inputDirectory / fs::path("world_params.json"));


	setFungiToPreysCapacityTransference(worldConfig["world"]["spaceConfiguration"]["fungiToPreysCapacityTransference"]);
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
	setObstacleDirectoryName(fs::path(worldConfig["world"]["spaceConfiguration"]["obstacleDirectory"]));
	setMoistureDirectoryName(fs::path(worldConfig["world"]["spaceConfiguration"]["moistureDirectory"]));
	setFungiDirectoryName(fs::path(worldConfig["world"]["life"]["fungiDirectory"]));
	setSpeciesDirectoryName(fs::path(worldConfig["world"]["life"]["speciesDirectory"]));
	
	initIndividualsPerDensities = worldConfig["world"]["life"]["initIndividualsPerDensities"]["enabled"];

	competitionAmongFungiSpecies = worldConfig["world"]["life"]["competitionAmongFungiSpecies"];

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

	readFungusSpeciesFromJSONFiles();
	readAnimalSpeciesFromJSONFiles();

	initializeTerrainDimensions();

	readObstaclePatchesFromJSONFiles();
	readMoisturePatchesFromJSONFiles();

	initializeFungi();
	//initializeAnimals();

	encountersMatrixFilename = (*jsonTree)["simulation"]["encountersMatrixFilename"];
	predationsMatrixFilename = (*jsonTree)["simulation"]["predationsMatrixFilename"];
	nodesMatrixFilename = (*jsonTree)["simulation"]["nodesMatrixFilename"];
	predationEventsOnOtherSpeciesFilename = (*jsonTree)["simulation"]["predationEventsOnOtherSpeciesFilename"];
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

	createOutputFile(edibilitiesFile, outputDirectory, (*jsonTree)["simulation"]["edibilitiesFilename"], "txt", std::ofstream::out | std::ofstream::trunc);
	
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

void World::setOutputDirectory(fs::path outputDirectory)
{
  this->outputDirectory = outputDirectory;

  fs::create_directories(outputDirectory / fs::path("Snapshots"));
  fs::create_directories(outputDirectory / fs::path("Matrices"));
  fs::create_directories(outputDirectory / fs::path("animals_each_day_start"));
  fs::create_directories(outputDirectory / fs::path("animals_each_day_end"));
  fs::create_directories(outputDirectory / fs::path("cells_each_day"));
  fs::create_directories(outputDirectory / fs::path("animals_each_day_growth"));
  fs::create_directories(outputDirectory / fs::path("animals_each_day_voracities"));
  fs::create_directories(outputDirectory / fs::path("animals_each_day_encounterProbabilities"));
  fs::create_directories(outputDirectory / fs::path("animals_each_day_predationProbabilities"));
  fs::create_directories(outputDirectory / fs::path("geneticsSummaries"));
}

void World::setFungiToPreysCapacityTransference(double fungiToPreysCapacityTransference)
{
	if (fungiToPreysCapacityTransference > 0)
	{
		this->fungiToPreysCapacityTransference = fungiToPreysCapacityTransference;
	}
	else
	{
		std::cerr << "Error, fungiToPreysCapacityTransference must be a positive value. You entered " << fungiToPreysCapacityTransference << " ... EXITING" << std::endl;
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

	for(auto elem : existingFungiSpecies) {
		delete elem;
	}
	existingFungiSpecies.clear();

	for(auto elem : existingAnimalSpecies) {
		delete elem;
	}
	existingAnimalSpecies.clear();
}

void World::readAnimalSpeciesFromJSONFiles()
{
	cout << "Reading all animal species from JSON files ... " << endl;
	
	fs::path speciesDirectory = inputDirectory / speciesDirectoryName;
	fs::directory_iterator end_iter;
	if (fs::exists(speciesDirectory) && fs::is_directory(speciesDirectory))
	{
		vector<fs::path> filePaths;
		copy(fs::directory_iterator(speciesDirectory), fs::directory_iterator(), back_inserter(filePaths));
		sort(filePaths.begin(), filePaths.end());             // directory iteration is not ordered on some file systems, so we sort them
		json ptMain;
		for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, fs::path(SCHEMA_FOLDER) / fs::path(SPECIES_SCHEMA));

				std::cout << " - Animal scientific name: " << ptMain["animal"]["name"] << endl;

				AnimalSpecies* newSpecies = new AnimalSpecies(
					ptMain["animal"]["name"], ptMain["animal"]["genetics"]["numberOfLociPerTrait"], 
					ptMain["animal"]["genetics"]["numberOfAllelesPerLocus"], ptMain["animal"]["genetics"]["restrictPerTrait"], 
					ptMain["animal"]["genetics"]["correlationCoefficientRhoPerModule"], ptMain["animal"]["genetics"]["traitsPerModule"], 
					ptMain["animal"]["genetics"]["numberOfChromosomes"], ptMain["animal"]["genetics"]["numberOfChiasmasPerChromosome"],
					ptMain["animal"]["traits"], ptMain["animal"]["traits"]["minTraitsRanges"], 
					ptMain["animal"]["traits"]["maxTraitsRanges"], ptMain["animal"]["traits"]["minTraitLimits"], 
					ptMain["animal"]["traits"]["maxTraitLimits"], ptMain["animal"]["traits"]["order"]
				);
				
				std::string growthTypeString = ptMain["animal"]["growthType"];
				unsigned int growthType;
				if (growthTypeString == "VonBertalanffy")
				{
					growthType = 0;
				}
				else if (growthTypeString == "logistic")
				{
					growthType = 1;
				}
                newSpecies->setGrowthType(growthType);

				std::string huntingModeString = ptMain["animal"]["huntingMode"];
				unsigned int huntingMode;
				if (huntingModeString == "does_not_hunt")
				{
					huntingMode = Animal::HUNTING_MODES::DOES_NOT_HUNT;
				}
				else if (huntingModeString == "sit_and_wait")
				{
					huntingMode = Animal::HUNTING_MODES::SIT_AND_WAIT;
				}
				else if (huntingModeString == "active_hunting")
				{
					huntingMode = Animal::HUNTING_MODES::ACTIVE_HUNTING;
				}
				newSpecies->setDefaultHuntingMode(huntingMode);

				std::string sexualTypeString = ptMain["animal"]["sexualType"];
				unsigned int sexualType;
				if (sexualTypeString == "diploid")
				{
					sexualType = AnimalSpecies::SEXUAL_TYPES::DIPLOID;
				}
				else if (sexualTypeString == "haplodiploid")
				{
					sexualType = AnimalSpecies::SEXUAL_TYPES::HAPLODIPLOID;
				}
				else if (sexualTypeString == "asexual")
				{
					sexualType = AnimalSpecies::SEXUAL_TYPES::ASEXUAL;
				}
				newSpecies->setSexualType(sexualType);

				newSpecies->computeRestrictedTraits();

				//Added for new growth_curves
				if(initIndividualsPerDensities == false)
				{
					newSpecies->setInitialPopulation(ptMain["animal"]["individualsPerInstar"]);
				}
				newSpecies->setStatisticsInitialPopulation(ptMain["animal"]["statisticsIndividualsPerInstar"]);

				unsigned int numberOfInstars = newSpecies->getInitialPopulation().size();
				cout << "numberOfInstars: " << numberOfInstars << endl;

				newSpecies->setAssignedForMolt(ptMain["animal"]["assignedForMolt"]);
				newSpecies->setBetaScaleTank(ptMain["animal"]["betaScaleTank"]);
				newSpecies->setExcessInvestInSize(ptMain["animal"]["excessInvestInSize"]);
				newSpecies->setNumberOfInstars(numberOfInstars);
				newSpecies->setPupaPeriodLength(ptMain["animal"]["pupaPeriodLength"]);
				newSpecies->setMinRelativeHumidityThreshold(ptMain["animal"]["minRelativeHumidityThreshold"]);
				newSpecies->setMaxEncountersT(ptMain["animal"]["maxEncountersT"]);

				//Added for new growth_curves
				newSpecies->setDevTimeVector(ptMain["animal"]["devTimeVector"]);
				newSpecies->setVonBertTime0(ptMain["animal"]["vonBertTime0"]);
				newSpecies->setLinfKcorr(ptMain["animal"]["LinfKcorr"]);
				newSpecies->setDevTimeConstant(ptMain["animal"]["devTimeConstant"]);
				newSpecies->setVonBertLdistanceMin(ptMain["animal"]["vonBertLdistanceMin"]);
				newSpecies->setVonBertLdistanceMax(ptMain["animal"]["vonBertLdistanceMax"]);
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

		cout << "DONE" << endl << endl;
		cout << "Assigning links between species from JSON files edible species information ... " << endl;

		//Who eats whom
		for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, fs::path(SCHEMA_FOLDER) / fs::path(SPECIES_SCHEMA));

				string scientificName = ptMain["animal"]["name"];
				cout << "Animal species " << scientificName << " eats: " << endl;

				for( auto elem : ptMain["animal"]["edibleFungusSpecies"])
				{
					string speciesToBeAddedName = elem;
					Species* fungusSpeciesToBeAdded = getFungusSpecies(speciesToBeAddedName);
					if (fungusSpeciesToBeAdded == NULL)
					{
						cerr << "Trying to add an edible fungus species that does not exist. Please check the species name or contact developers." << endl;
						exit(-1);
					}
					cout << " - Fungus: " << speciesToBeAddedName << endl;
					getAnimalSpecies(scientificName)->addEdibleFungusSpecies(fungusSpeciesToBeAdded);
				}

				int i = 0;
				for( auto elem : ptMain["animal"]["edibleFungusPreferences"])
				{
					float edibleFungusPreferenceToBeAdded = elem;
					Species* fungusSpeciesToBeAdded = getAnimalSpecies(scientificName)->getEdibleFungusSpecies()->at(i);

					cout << " - Fungus: " << fungusSpeciesToBeAdded->getScientificName() << " -> Preference: " << edibleFungusPreferenceToBeAdded << endl;
					getAnimalSpecies(scientificName)->addEdiblePreference(fungusSpeciesToBeAdded, edibleFungusPreferenceToBeAdded);
					++i;
				}

				i = 0;
				for( auto elem : ptMain["animal"]["edibleFungusProfitability"])
				{
					float edibleFungusProfitabilityToBeAdded = elem;
					Species* fungusSpeciesToBeAdded = getAnimalSpecies(scientificName)->getEdibleFungusSpecies()->at(i);

					cout << " - Fungus: " << fungusSpeciesToBeAdded->getScientificName() << " -> Profitability: " << edibleFungusProfitabilityToBeAdded << endl;
					getAnimalSpecies(scientificName)->addEdibleProfitability(fungusSpeciesToBeAdded, edibleFungusProfitabilityToBeAdded);
					++i;
				}

				for( auto elem : ptMain["animal"]["edibleAnimalSpecies"])
				{
					string speciesToBeAddedName = elem;
					Species* animalSpeciesToBeAdded = getAnimalSpecies(speciesToBeAddedName);
					if (animalSpeciesToBeAdded == NULL)
					{
						cerr << "Trying to add an edible animal species that does not exist. Please check the species name or contact developers." << endl;
						exit(-1);
					}
					cout << " - Animal: " << speciesToBeAddedName << endl;
					getAnimalSpecies(scientificName)->addEdibleAnimalSpecies(animalSpeciesToBeAdded);
				}

				i = 0;
				for( auto elem : ptMain["animal"]["edibleAnimalPreferences"])
				{
					float edibleAnimalPreferenceToBeAdded = elem;
					Species* animalSpeciesToBeAdded = getAnimalSpecies(scientificName)->getEdibleAnimalSpecies()->at(i);

					cout << " - Animal: " << animalSpeciesToBeAdded->getScientificName() << " -> Preference: " << edibleAnimalPreferenceToBeAdded << endl;
					getAnimalSpecies(scientificName)->addEdiblePreference(animalSpeciesToBeAdded, edibleAnimalPreferenceToBeAdded);
					++i;
				}

				i = 0;
				for( auto elem : ptMain["animal"]["edibleAnimalProfitability"])
				{
					float edibleAnimalProfitabilityToBeAdded = elem;
					Species* animalSpeciesToBeAdded = getAnimalSpecies(scientificName)->getEdibleAnimalSpecies()->at(i);

					cout << " - Animal: " << animalSpeciesToBeAdded->getScientificName() << " -> Profitability: " << edibleAnimalProfitabilityToBeAdded << endl;
					getAnimalSpecies(scientificName)->addEdibleProfitability(animalSpeciesToBeAdded, edibleAnimalProfitabilityToBeAdded);
					++i;
				}

				cout << endl;
			}
		}
	}
	else
	{
		cerr << "The specified path \"" + speciesDirectory.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}

	cout << "DONE" << endl << endl;

	for (vector<Species *>::iterator speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
	{
		(*speciesIt)->setInitialPredationEventsOnOtherSpecies(existingAnimalSpecies.size() + existingFungiSpecies.size());
	}

}

void World::readFungusSpeciesFromJSONFiles()
{
	cout << "Reading all fungus species from JSON files ... " << endl;
	
	fs::path fungiDirectory = inputDirectory / fungiDirectoryName;
	fs::directory_iterator end_iter;
	if (fs::exists(fungiDirectory) && fs::is_directory(fungiDirectory))
	{
		vector<fs::path> filePaths;
		copy(fs::directory_iterator(fungiDirectory), fs::directory_iterator(), back_inserter(filePaths));
		sort(filePaths.begin(), filePaths.end());             // directory iteration is not ordered on some file systems, so we sort them
		json ptMain;
		for(vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, fs::path(SCHEMA_FOLDER) / fs::path(FUNGI_SCHEMA));

				std::cout << " - Fungus scientific name: " << ptMain["fungus"]["name"] << endl << endl;

				FungusSpecies * newSpecies = new FungusSpecies(ptMain["fungus"]["name"], ptMain["fungus"]["ACTIVATION_ENERGY"], ptMain["fungus"]["NORMALIZATION_B"], ptMain["fungus"]["patchesDirectory"]);
				newSpecies->setCellMass(ptMain["fungus"]["cellMass"]);
				newSpecies->setConversionToWetMass(ptMain["fungus"]["conversionToWetMass"]);
				newSpecies->setGrowingRateParams(ptMain["fungus"]["minHR"], ptMain["fungus"]["maxHR"], ptMain["fungus"]["maxRScale"]);
				newSpecies->setMinimumEdibleBiomass(ptMain["fungus"]["minimumNumberOfEdibleSpores"], ptMain["fungus"]["cellMass"]);

				addFungusSpecies(newSpecies);
			}
		}
	}
	else
	{
		cerr << "The specified path \"" + fungiDirectory.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}
}

void World::initializeFungi()
{
	cout << "Reading all fungus patches from JSON files ... " << endl;
	
	fs::directory_iterator end_iter;

	for (vector<Species *>::iterator itFungiSpecies = existingFungiSpecies.begin(); itFungiSpecies != existingFungiSpecies.end(); itFungiSpecies++)
	{
		Species* currentFungusSpecies = *itFungiSpecies;
		cout << " - Fungus scientific name: " << currentFungusSpecies->getScientificName() << endl << endl;
		fs::path patchesDirectory = inputDirectory / fungiDirectoryName / fs::path(currentFungusSpecies->getPatchesDirectoryName());
		json ptMain;
		if (fs::exists(patchesDirectory) && fs::is_directory(patchesDirectory))
		{
			vector<fs::path> filePaths;
			copy(fs::directory_iterator(patchesDirectory), fs::directory_iterator(), back_inserter(filePaths));
			sort(filePaths.begin(), filePaths.end());             // directory iteration is not ordered on some file systems, so we sort them

			for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
			{
				if (it->extension() == ".json")
				{
					string patchFilename = it->string();
					cout << patchFilename << endl;

					ptMain.clear();

					// Read configuration file
					ptMain = readConfigFile(*it, fs::path(SCHEMA_FOLDER) / fs::path(PATCH_SCHEMA));

					string type = ptMain["patch"]["type"];

					if (type == "homogeneous")
					{
						float value = ptMain["patch"]["value"];
						setHomogeneousFungus(currentFungusSpecies, value);
					}
					else if (type == "gaussian")
					{
						unsigned int radius = ptMain["patch"]["radius"];
						unsigned int x = ptMain["patch"]["xPos"];
						unsigned int y = ptMain["patch"]["yPos"];
						unsigned int z = ptMain["patch"]["zPos"];
						float sigma = ptMain["patch"]["sigma"];
						float amplitude = ptMain["patch"]["amplitude"];
						setGaussianFungusPatch(currentFungusSpecies, x, y, z, radius, sigma, amplitude);
					}
					else if (type == "sphere")
					{
						unsigned int radius = ptMain["patch"]["radius"];
						unsigned int x = ptMain["patch"]["xPos"];
						unsigned int y = ptMain["patch"]["yPos"];
						unsigned int z = ptMain["patch"]["zPos"];
						float value = ptMain["patch"]["value"];
						setSphericalFungusPatch(currentFungusSpecies, x, y, z, radius, value);
					}
					else if (type == "cubic")
					{
std::cout << "CUBICOOOOO\n\n" << std::endl;

						Coordinate3D<int> patchCenter;
						Coordinate3D<int> patchDimensions;

						patchCenter.setX(ptMain["patch"]["center"]["x"]);
						patchCenter.setY(ptMain["patch"]["center"]["y"]);
						patchCenter.setZ(ptMain["patch"]["center"]["z"]);

						patchDimensions.setX(ptMain["patch"]["dimensions"]["x"]);
						patchDimensions.setY(ptMain["patch"]["dimensions"]["y"]);
						patchDimensions.setZ(ptMain["patch"]["dimensions"]["z"]);

						float value = ptMain["patch"]["value"];
						setCubicFungusPatch(currentFungusSpecies, patchCenter, patchDimensions, value);

					}
					else if (type == "randomGaussian")
					{
						bool useRandomAmplitude = ptMain["patch"]["useRandomAmplitude"];
						bool useRandomSigma = ptMain["patch"]["useRandomSigma"];
						unsigned int numberOfPatches = ptMain["patch"]["numberOfPatches"];
						unsigned int patchesRadius = ptMain["patch"]["patchesRadius"];
						float maxSigma = ptMain["patch"]["maxSigma"];
						float maxAmplitude = ptMain["patch"]["maxAmplitude"];

						setRandomGaussianFungusPatches(currentFungusSpecies, numberOfPatches, patchesRadius, maxSigma, useRandomSigma, maxAmplitude, useRandomAmplitude);
					}
				}
			}
		}
		else
		{
			cerr << "The specified path \"" + patchesDirectory.string() + "\" does not exist or it is not a directory" << endl;
			exit(-1);
		}
	}
}

void World::addFungusSpecies(FungusSpecies * newSpecies)
{
	if (std::find(existingFungiSpecies.begin(), existingFungiSpecies.end(), newSpecies) == existingFungiSpecies.end())
	{
		existingFungiSpecies.push_back(newSpecies);
	}
	else
	{
		std::cerr << "Error, the " << newSpecies->getScientificName() << " was already added to this world ... EXITING"
				<< std::endl;
		exit(-1);
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
		std::cerr << "Error, the " << newSpecies->getScientificName() << " was already added to this world ... EXITING"
				<< std::endl;
		exit(-1);
	}
}

Species * World::getFungusSpecies(string name)
{
	for (vector<Species*>::iterator it = existingFungiSpecies.begin(); it != existingFungiSpecies.end(); it++)
	{
		if ((*it)->getScientificName() == name)
		{
			return (*it);
		}
	}

	return NULL;
}

Species * World::getAnimalSpecies(string name)
{
	for (vector<Species*>::iterator it = existingAnimalSpecies.begin(); it != existingAnimalSpecies.end(); it++)
	{
		if ((*it)->getScientificName() == name)
		{
			return (*it);
		}
	}

	return NULL;
}

void World::addPlantSpecies(PlantSpecies * newSpecies, unsigned int initialPopulationSize)
{
	if (existingPlantSpecies.find(newSpecies) == existingPlantSpecies.end())
	{
		existingPlantSpecies[newSpecies] = initialPopulationSize;
	}
	else
	{
		std::cerr << "Error, the " << newSpecies->getScientificName() << " was already added to this world ... EXITING"
				<< std::endl;
		exit(-1);
	}
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
		
		ofstream file;
		createOutputFile(file, outputDirectory / fs::path(pathBySimulationPoint) / fs::path("animals_day_"), "txt", day, recordEach);
		if (!file.is_open())
		{
			cerr << "Error opening the file." << endl;
		}
		else
		{
			file
					<< "id\tspecies\tgender\tx\ty\tz\tstate\tinstar\tpheno_ini\tdate_egg\tage_first_rep\trep_count\tfecundity\tdate_death\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\tencounters_pred\tglobal_pred_encs\tdays_digest\tvor_ini\tsearch_ini\tspeed_ini\ttank_ini\tpheno_ini\tcurrentBodySize\tcurrentDryMass\t" << TraitConverter::printAvailableTraits() //\t1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\t16\t17\t18\t19\t20\t21\t22\t23\t24\t25\t26\t27\t28\t29\t30\t31\t32\t33\t34\t35\t36\t37\t38\t39\t40\t41\t42\t43\t44\t45\t46\t47\t48\t49\t50\t51\t52\t53\t54\t55\t56\t57\t58\t59\t60\t61\t62\t63\t64\t65\t66\t67\t68\t69\t70\t71\t72"
					<< endl;
			TerrainCell* cellToPrintFrom;
			for (unsigned int z = 0; z < depth; z++)
			{
				for (unsigned int y = 0; y < length; y++)
				{
					for (unsigned int x = 0; x < width; x++)
					{
						cellToPrintFrom = terrain[z][y][x];
						if(!cellToPrintFrom->isObstacle())
						{
							terrain[z][y][x]->printAnimals(file);
						}
					}
				}
			}
			file.close();
		}
	}
}

 void World::printCellAlongCells(int day)
 {
	if(((day%recordEach)==0) | (day==0)){

		string pathBySimulationPoint = "cells_each_day";
		
		ofstream file;
		createOutputFile(file, outputDirectory / fs::path(pathBySimulationPoint) / fs::path("cells_day_"), "txt", day, recordEach);
		if(!file.is_open())
		{
			cerr << "Error opening the file." << endl;
		}
		else
		{
			file << "x\ty\tz\t";

			for (vector<Species *>::iterator itFungiSpecies = existingFungiSpecies.begin(); itFungiSpecies != existingFungiSpecies.end(); itFungiSpecies++)
			{
				file << (*itFungiSpecies)->getScientificName() << "\t";
			}

			for (vector<Species *>::iterator itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
			{
				file << (*itSpecies)->getScientificName() << "\t";
			}
			file << endl;

			TerrainCell* cellToPrintFrom;
			for (unsigned int z = 0; z < depth; z++)
			{
				for (unsigned int y = 0; y < length; y++)
				{
					for (unsigned int x = 0; x < width; x++)
					{
						cellToPrintFrom = terrain[z][y][x];
						if(!cellToPrintFrom->isObstacle())
						{
							terrain[z][y][x]->printCell(file);
						}
					}
				}
			}
			file.close();
		}
	}
 }


ostream& World::printDailySummary(ostream& os, int day)
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
				biomass += terrain[z][y][x]->getTotalFungusBiomass();

				numberOfUnbornPreys += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::UNBORN,
						Animal::HUNTING_MODES::DOES_NOT_HUNT);
				numberOfActivePreys += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::ACTIVE,
						Animal::HUNTING_MODES::DOES_NOT_HUNT);
				numberOfStarvedPreys += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::STARVED,
						Animal::HUNTING_MODES::DOES_NOT_HUNT);
				numberOfPredatedPreys += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::PREDATED,
						Animal::HUNTING_MODES::DOES_NOT_HUNT);
				numberOfReproducingPreys += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::REPRODUCING,
						Animal::HUNTING_MODES::DOES_NOT_HUNT);
				numberOfSenescedPreys += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::SENESCED,
						Animal::HUNTING_MODES::DOES_NOT_HUNT);
				numberOfShockedPreys += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::SHOCKED,
						Animal::HUNTING_MODES::DOES_NOT_HUNT);

				numberOfUnbornPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::UNBORN,
						Animal::HUNTING_MODES::ACTIVE_HUNTING);
				numberOfUnbornPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::UNBORN,
						Animal::HUNTING_MODES::SIT_AND_WAIT);
				numberOfActivePredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::ACTIVE,
						Animal::HUNTING_MODES::ACTIVE_HUNTING);
				numberOfActivePredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::ACTIVE,
						Animal::HUNTING_MODES::SIT_AND_WAIT);
				numberOfStarvedPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::STARVED,
						Animal::HUNTING_MODES::ACTIVE_HUNTING);
				numberOfStarvedPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::STARVED,
						Animal::HUNTING_MODES::SIT_AND_WAIT);
				numberOfPredatedPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::PREDATED,
						Animal::HUNTING_MODES::ACTIVE_HUNTING);
				numberOfPredatedPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::PREDATED,
						Animal::HUNTING_MODES::SIT_AND_WAIT);
				numberOfReproducingPredators += terrain[z][y][x]->getLifeStagePopulation(
						Animal::LIFE_STAGES::REPRODUCING, Animal::HUNTING_MODES::ACTIVE_HUNTING);
				numberOfReproducingPredators += terrain[z][y][x]->getLifeStagePopulation(
						Animal::LIFE_STAGES::REPRODUCING, Animal::HUNTING_MODES::SIT_AND_WAIT);
				numberOfBackgroundPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::BACKGROUND,
						Animal::HUNTING_MODES::ACTIVE_HUNTING);
				numberOfBackgroundPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::BACKGROUND,
						Animal::HUNTING_MODES::SIT_AND_WAIT);
				numberOfSenescedPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::SENESCED,
						Animal::HUNTING_MODES::ACTIVE_HUNTING);
				numberOfSenescedPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::SENESCED,
						Animal::HUNTING_MODES::SIT_AND_WAIT);
				numberOfShockedPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::SHOCKED,
						Animal::HUNTING_MODES::ACTIVE_HUNTING);
				numberOfShockedPredators += terrain[z][y][x]->getLifeStagePopulation(Animal::LIFE_STAGES::SHOCKED,
						Animal::HUNTING_MODES::SIT_AND_WAIT);

			}
		}
	}

	os << day << "\t" << biomass << "\t" << numberOfUnbornPreys << "\t" << numberOfActivePreys << "\t"
			<< numberOfStarvedPreys << "\t" << numberOfPredatedPreys << "\t" << numberOfReproducingPreys << "\t"
			<< numberOfSenescedPreys << "\t" << numberOfShockedPreys << "\t" << numberOfUnbornPredators << "\t" << numberOfActivePredators << "\t"
			<< numberOfStarvedPredators << "\t" << numberOfPredatedPredators << "\t" << numberOfReproducingPredators
			<< "\t" << numberOfBackgroundPredators << "\t" << numberOfSenescedPredators << "\t" 
			<< numberOfShockedPredators << endl;
			
	return os;
}

ostream& World::printExtendedDailySummary(ostream& os, int day)
{
	map<Species*, double>* worldFungiBiomass = new map<Species*, double>();
	for (vector<Species *>::iterator itFungiSpecies = existingFungiSpecies.begin(); itFungiSpecies != existingFungiSpecies.end(); itFungiSpecies++)
	{
		worldFungiBiomass->insert(std::pair<Species*, double>((*itFungiSpecies),0));
	}

	map<Species*, vector<unsigned int>* >* worldAnimalsPopulation = new map<Species*, vector<unsigned int>* >();
	for (vector<Species *>::iterator itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
	{
		worldAnimalsPopulation->insert(std::pair<Species*, vector<unsigned int>*>((*itSpecies), new vector<unsigned int>(Animal::LIFE_STAGES::SHOCKED+1, 0)));
	}

	TerrainCell* cellToUpdateFrom;
	for (unsigned int z = 0; z < depth; z++)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				cellToUpdateFrom = terrain[z][y][x];
				if(!cellToUpdateFrom->isObstacle())
				{
					cellToUpdateFrom->updateWorldFungiBiomassAndAnimalsPopulation(worldFungiBiomass, worldAnimalsPopulation);
				}
			}
		}
	}

	os << day << "\t";

	for (vector<Species *>::iterator itFungiSpecies = existingFungiSpecies.begin(); itFungiSpecies != existingFungiSpecies.end(); itFungiSpecies++)
	{
		os << worldFungiBiomass->at(*itFungiSpecies) << "\t";
	}

	for (vector<Species *>::iterator itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
	{
		for (unsigned int lifeStage = 0; lifeStage <= Animal::LIFE_STAGES::SHOCKED; ++lifeStage)
		{
			os << worldAnimalsPopulation->at(*itSpecies)->at(lifeStage) << "\t";
		}
	}
	os << endl;

/*
	os << day << "\t" << biomass << "\t" << numberOfUnbornPreys << "\t" << numberOfActivePreys << "\t"
			<< numberOfStarvedPreys << "\t" << numberOfPredatedPreys << "\t" << numberOfReproducingPreys << "\t"
			<< numberOfSenescedPreys << "\t" << numberOfUnbornPredators << "\t" << numberOfActivePredators << "\t"
			<< numberOfStarvedPredators << "\t" << numberOfPredatedPredators << "\t" << numberOfReproducingPredators
			<< "\t" << numberOfBackgroundPredators << "\t" << numberOfSenescedPredators << endl;
			*/

	worldFungiBiomass->clear();
	delete worldFungiBiomass;
	
	for (vector<Species *>::iterator itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
	{
	
		delete worldAnimalsPopulation->at(*itSpecies);
		
	}
	worldAnimalsPopulation->clear();
	delete worldAnimalsPopulation;

	return os;
}

void World::printGeneticsSummaries(int day)
{

	map<Species*, vector<unsigned int>* >* worldAnimalsPopulation = new map<Species*, vector<unsigned int>* >();
	map<Species*, vector<set<double>* >* >* worldGeneticsFrequencies = new map<Species*, vector<set<double>* >* >();
	for (vector<Species *>::iterator itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
	{
		worldAnimalsPopulation->insert(std::pair<Species*, vector<unsigned int>*>((*itSpecies), new vector<unsigned int>(Animal::LIFE_STAGES::SHOCKED+1, 0)));
		//We multiply x2 because there are PAIRS of chromosomes. We store this data this way: T1_CR1, T2_CR1... T13_CR1.. T1_CR2, T2_CR2... T13_CR2
		unsigned int totalNumberOfLoci = (*itSpecies)->getNumberOfTraits() * (*itSpecies)->getNumberOfLociPerTrait() * 2;
		worldGeneticsFrequencies->insert(std::pair<Species*, vector<set<double>* >*>((*itSpecies), new vector<set<double>* >(totalNumberOfLoci)));
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
				cellToUpdateFrom = terrain[z][y][x];
				if(!cellToUpdateFrom->isObstacle())
				{
					cellToUpdateFrom->updateAnimalsPopulationAndGeneticsFrequencies(worldAnimalsPopulation, worldGeneticsFrequencies);
				}

			}

		}
	}

	double sum, mean;
	for (vector<Species *>::iterator itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
	{
		string scientificName = (*itSpecies)->getScientificName();
		
		ofstream geneticsSummaryFile;
		createOutputFile(geneticsSummaryFile, outputDirectory / fs::path("geneticsSummaries"), scientificName + "_geneticsSummary", "txt", ofstream::app);
		if (!geneticsSummaryFile.is_open())
		{
			cerr << "Error opening the file." << endl;
		}
		else
		{
			geneticsSummaryFile << day << "\t";
			geneticsSummaryFile << worldAnimalsPopulation->at(*itSpecies)->at(Animal::LIFE_STAGES::UNBORN) + worldAnimalsPopulation->at(*itSpecies)->at(Animal::LIFE_STAGES::ACTIVE) + worldAnimalsPopulation->at(*itSpecies)->at(Animal::LIFE_STAGES::REPRODUCING) << "\t";

			//We multiply x2 because there are PAIRS of chromosomes. We store this data this way: T1_CR1, T2_CR1... T13_CR1.. T1_CR2, T2_CR2... T13_CR2
			for(unsigned int selectedChromosome = 0; selectedChromosome < (*itSpecies)->getNumberOfTraits()*2; ++selectedChromosome)
			{
				sum = 0;
				mean = 0;
				for(unsigned int selectedLoci = 0; selectedLoci < (*itSpecies)->getNumberOfLociPerTrait(); ++selectedLoci)
				{
					sum += worldGeneticsFrequencies->at((*itSpecies))->at(selectedChromosome*(*itSpecies)->getNumberOfLociPerTrait()+selectedLoci)->size();
				}
				mean = sum / (*itSpecies)->getNumberOfLociPerTrait();
				geneticsSummaryFile << mean << "\t";
			}
			geneticsSummaryFile << endl;
			geneticsSummaryFile.close();
		}

	}
}

void World::saveAnimalSpeciesSnapshot(fs::path filenameRoot, int day, Species* species)
{
	if (species->getTotalInitialPopulation() > 0)
	{
		string scientificName = species->getScientificName();
		std::replace(scientificName.begin(), scientificName.end(), ' ', '_');

		ofstream file;
		string filename = createOutputFile(file, fs::path(filenameRoot.string() + "_" + scientificName + "_day_"), "dat", day, recordEach, ios::out | ios::binary);

		std::cout << "Saving Animal as " << filename << "... ";

		float value;
		for (unsigned int z = 0; z < depth; z++)
		{
			for (unsigned int y = 0; y < length; y++)
			{
				for (unsigned int x = 0; x < width; x++)
				{
					TerrainCell* currentTerrainCell = terrain[z][y][x];
					if(!currentTerrainCell->isObstacle())
					{
						value = currentTerrainCell->getLifeStagePopulation(Animal::LIFE_STAGES::ACTIVE, species);
					}
					else
					{
						value = 0.0;
					}
					file.write((char *) &value, sizeof(float));
				}
			}
		}

		std::cout << "DONE" << std::endl;

		file.close();
	}
}

void World::saveFungusSpeciesSnapshot(fs::path filenameRoot, int day, Species* species)
{
	string scientificName = species->getScientificName();
	std::replace(scientificName.begin(), scientificName.end(), ' ', '_');

	ofstream file;
	string filename = createOutputFile(file, fs::path(filenameRoot.string() + "_" + scientificName + "_day_"), "dat", day, recordEach, ios::out | ios::binary);

	std::cout << "Saving Fungus as " << filename << "... ";

	float value;

	TerrainCell * currentTerrainCell;
	for (unsigned int z = 0; z < depth; ++z)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				currentTerrainCell = terrain[z][y][x];
				if(!currentTerrainCell->isObstacle())
				{
					value = currentTerrainCell->getFungusBiomass(species);
				}
				else
				{
					value = 0.0;
				}
				file.write((char *) &value, sizeof(float));
			}
		}
	}

	std::cout << "DONE" << std::endl;

	file.close();
}

void World::saveWaterSnapshot(fs::path filenameRoot, int day)
{
	ofstream file;
	string filename = createOutputFile(file, fs::path(filenameRoot.string() + "_day_"), "dat", day, recordEach, ios::out | ios::binary);

	std::cout << "Saving Water volume as " << filename << "... ";

	float value;

	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = 0; z < depth; z++)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				currentTerrainCell = terrain[z][y][x];
				if(!currentTerrainCell->isObstacle())
				{
					value = currentTerrainCell->getWater();
					file.write((const char*) &value, sizeof(float));
				}
			}
		}
	}

	file.close();
	std::cout << "DONE" << std::endl;
}

void World::printPredationEventsOnOtherSpeciesMatrix(ostream& predationEventsOnOtherSpeciesFile)
{
	predationEventsOnOtherSpeciesFile << "\t";
	for (unsigned int i = 0; i < existingAnimalSpecies.size(); i++)
	{
		predationEventsOnOtherSpeciesFile << existingAnimalSpecies[i]->getScientificName() << "\t";
	}

	predationEventsOnOtherSpeciesFile << endl;

	vector<int>* currentPredationEventsOnOtherSpecies;
	for (unsigned int i = 0; i < existingAnimalSpecies.size(); i++)
	{
		predationEventsOnOtherSpeciesFile << existingAnimalSpecies[i]->getScientificName() << "\t";
		currentPredationEventsOnOtherSpecies = existingAnimalSpecies[i]->getPredationEventsOnOtherSpecies();
		for (unsigned int j = 0; j < existingAnimalSpecies.size(); j++)
		{
			predationEventsOnOtherSpeciesFile
					<< currentPredationEventsOnOtherSpecies->at(existingAnimalSpecies[j]->getId()) << "\t";
		}
		predationEventsOnOtherSpeciesFile << endl;
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
				currentTerrainCell = terrain[z][y][x];
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
				TerrainCell* currentTerrainCell = terrain[z][y][x];
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
	initializeOutputFiles();

	saveWaterSnapshot(outputDirectory / fs::path("Snapshots") / fs::path("Water_initial"), 0);

	for (std::vector<Species *>::iterator fungiSpeciesIt = existingFungiSpecies.begin(); fungiSpeciesIt != existingFungiSpecies.end(); fungiSpeciesIt++)
	{
		saveFungusSpeciesSnapshot(outputDirectory / fs::path("Snapshots") / fs::path("Fungus_initial"), 0, *fungiSpeciesIt);
	}

	// Next is intentionally commented because it would give empty volumes (animals are unborn)
	/*
	 std::vector<AnimalSpecies *>::iterator animIt;

	 for (animIt = existingAnimalSpecies.begin(); animIt != existingAnimalSpecies.end(); animIt++)
	 {
	 saveAnimalSpeciesSnapshot(outputDirectory + "/Snapshots/" + "Animal_initial", 0, **animIt);
	 }
	 */

	vector<TerrainCell*> inhabitableTerrainCells;
	obtainInhabitableTerrainCells(inhabitableTerrainCells);

	ofstream timeSpentFile;
	createOutputFile(timeSpentFile, outputDirectory, "time_spent", "txt");
	if (!timeSpentFile.is_open())
	{
		cerr << "Error opening the file." << endl;
	}

	for (unsigned int timeStep = 0; timeStep < runDays*timeStepsPerDay; timeStep++)
	{
		std::cout << "Running on day " << timeStep << " out of " << runDays*timeStepsPerDay << std::endl;

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

		ofstream tuneTraitsFile;
		createOutputFile(tuneTraitsFile, outputDirectory / fs::path(pathBySimulationPointTuneTraits) / fs::path("animals_growth_day_"), "txt", timeStep, recordEach);
		if (!tuneTraitsFile.is_open())
		{
			cerr << "Error opening the file." << endl;
		}
		else
		{
			
				tuneTraitsFile << "growth\tLinf\tid\tspecies\tstate\tcurrent_age\tinstar\tbody_size\tenergy_tank\ttankAtGrowth\tbody_mass\tmature\tmin_mass_for_death\tfinalJMinVB\tfinalJMaxVB\tvoracity_ini\texpectedDryMassFromMinVor\texpectedDryMassFromMaxVor\tmaxMassNextInstarPlasticity\tcurrentWetMass\tpreT_search\tpreT_speed\tpostT_search\tpostT_speed\tini_mass_instar\ttarget_next_mass\tminimum_met_loss\tcondition_search\tcondition_speed\tnon_condition_voracity\tcondition_voracity\tafter_encounters_voracity\tafter_encounters_search\tfinal_speed\tdeath_date" << endl;
				cout << " - Activating animals and tuning traits ... " << endl << flush;
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
				currentTerrainCell->tuneTraits(timeStep, timeStepsPerDay, tuneTraitsFile);
			}
/*
#endif
*/
			auto t1 = clock();
			cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
			timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";
			cout << "DONE" << endl << flush;

			tuneTraitsFile.close();
		}

//#####################################################################
//#######################     GROWING FUNGI     #######################
//#####################################################################

		cout << " - Growing fungi ... " <<  endl << flush;
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
		//			currentTerrainCell->growFungi();
		//		}
		//	});
		//}
		//taskGroup.wait();
		//
		GrowFungiRangerTask& growFungiRangerTask = *new(task::allocate_root()) GrowFungiRangerTask(width, length, depth, terrain);
		task::spawn_root_and_wait(growFungiRangerTask);
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

					errorCreatingThread = pthread_create(&workerThreads[i][j][k], NULL, &World::growFungiThreadMaker, static_cast<void*>(&toDoArguments[i][j][k]));
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
			currentTerrainCell->growFungi(timeStep, currentTerrainCell->getTheWorld()->getCompetitionAmongFungiSpecies(), currentTerrainCell->getMaximumFungiCapacity(), currentTerrainCell->getTheWorld()->existingFungiSpecies.size());
		}
/*
#endif
*/
		auto t1 = clock();
		cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
		timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";
		cout << "DONE" << endl << flush;
		
#ifndef _TEST
		// After a fungus has grown, it can expand to other cells. In this case,
		// when the new cell is processed, a wrong amount (in excess) of fungus would
		// be found. Therefore, new amounts are added to a temporary variable that
		// must be processed upon completion of all cells processing

		cout << " - Spreading fungi ... " << endl << flush;

		// TODO ROMAN All this can be done in parallel
		t0 = clock();
		/*
		parallel_for(size_t(0), size_t(inhabitableTerrainCells.size()), size_t(1) , [=](size_t i)
		{
			inhabitableTerrainCells[i]->commitFungiSpread();
		});
		*/
		for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
		{
			//cout << "Cell: (" << inhabitableTerrainCells[i]->getX() << "," << inhabitableTerrainCells[i]->getY() << "," << inhabitableTerrainCells[i]->getZ() << ")" << endl;
			inhabitableTerrainCells[i]->commitFungiSpread(); //OK			// Grows and spreads excess to neighbors
		}

		t1 = clock();
		cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
		timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";
		cout << "DONE" << endl << flush;
		
		/*
		cout << " - Applying humidity decay over time effect ... " << endl << flush;
		// Moisture decays each day
		//TODO Should moisture decay depending on the amount consumed by fungi?
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

		ofstream encounterProbabilitiesFile, predationProbabilitiesFile;
		createOutputFile(encounterProbabilitiesFile, outputDirectory / fs::path(pathBySimulationPoint) / fs::path("animals_encounterProbabilities_day_"), "txt", timeStep, recordEach);
		if (!encounterProbabilitiesFile.is_open())
		{
			cerr << "Error opening the file." << endl;
		}
		else
		{
			pathBySimulationPoint = "animals_each_day_predationProbabilities";

			createOutputFile(predationProbabilitiesFile, outputDirectory / fs::path(pathBySimulationPoint) / fs::path("animals_predationProbabilities_day_"), "txt", timeStep, recordEach);
			if (!predationProbabilitiesFile.is_open())
			{
				cerr << "Error opening the file." << endl;
			}
			else
			{
				encounterProbabilitiesFile << "idSearcher\tidSearched\tspeciesSearcher\tspeciesSearched\tsearchedIsPredator\tmassSearcher\tmassSearched\tprobRandomEncounter\tprobLogisticEncounter\tprobAttack\tsuccessfulEncounter" << endl; 
				predationProbabilitiesFile << "idHunter\tidHunted\tspeciesHunter\tspeciesHunted\thuntedIsPredator\tmassHunter\tmassHunted\tprobRandomPredation\tprobLogisticPredation\tsuccessfulPredation" << endl;

				cout << " - Moving animals ... " << endl << flush;
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

		    	cout << "0%|";
		    	for(currentPercentage = printBarEach; currentPercentage < 1.0; currentPercentage+=printBarEach)
		    	{
		    		cout << "  ";
		    	}
		    	cout << "|100%" << endl;

		    	currentPercentage = printBarEach;
				cout << "   ";
		    	for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
				{
					TerrainCell* currentTerrainCell = inhabitableTerrainCells[i];
					currentTerrainCell->moveAnimals(timeStep, timeStepsPerDay, encounterProbabilitiesFile, predationProbabilitiesFile, edibilitiesFile, exitTimeThreshold);
					if(i >= currentPercentage * inhabitableTerrainCells.size())
					{
						cout << "==" << flush;
						currentPercentage += printBarEach;
					}
				}
				cout << endl;
/*
#endif
*/
				t1 = clock();
				cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
				timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";
				cout << "DONE" << endl << flush;

				encounterProbabilitiesFile.close();
				predationProbabilitiesFile.close();
			}
		}

		#ifdef _DEBUG
		cout << "Animals searched for by the same animal at least twice the same day: " << multipleSameSearchedAnimalToday << endl;
		cout << "Animals encountered by the same animal at least twice the same day: " << multipleSameEncounteredAnimalToday << endl;
		cout << "Animals hunted off by the same animal at least twice the same day: " << multipleSameHuntedAnimalToday << endl;
		cout << "Animals predated by the same animal at least twice the same day: " << multipleSamePredatedAnimalToday << endl;
		#endif

		string pathBySimulationPointVoracities = "animals_each_day_voracities";

		ofstream voracitiesFile;
		createOutputFile(voracitiesFile, outputDirectory / fs::path(pathBySimulationPointVoracities) / fs::path("animals_voracities_day_"), "txt", timeStep, recordEach);
		if (!voracitiesFile.is_open())
		{
			cerr << "Error opening the file." << endl;
		}
		else
		{
			voracitiesFile << "id\tspecies\tstate\tcurrentAge\tinstar\tbody_size\tenergy_tank\tdryMass\tnextDinoMass\tmin_mass_for_death\tafter_encounters_voracity\tfood_mass\tdryMassAfterAssim\ttotalMetabolicDryMassLossAfterAssim\teatenToday\tsteps\tstepsAttempted\tafter_encounters_search\tsated\tpercentMoving\tpercentHandling\tvoracity_body_mass_ratio\tgender\tmated\teggDryMass\tK\tL\tpseudoK\tpseudoL\tfactorEggMass\teggDryMassAtBirth\tdeath_date\tageOfFirstMaturation\trep_count" << endl;
			cout << " - Background, assimilating food and reproducing ... " << endl << flush;
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
				currentTerrainCell->printAnimalsVoracities(timeStep, timeStepsPerDay, voracitiesFile);
				currentTerrainCell->diePredatorsFromBackground(timeStep);
				currentTerrainCell->assimilateFoodMass(timeStep);
				currentTerrainCell->metabolizeAnimals(timeStep, timeStepsPerDay);
				currentTerrainCell->growAnimals(timeStep, timeStepsPerDay);
				currentTerrainCell->breedAnimals(timeStep, timeStepsPerDay, outputDirectory);
			}
/*
#endif
*/
			t1 = clock();
			cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
			timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";
			cout << "DONE" << endl << flush;

			voracitiesFile.close();
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
		cout << " - Printing animals along cells ... " << endl << flush;
		t0 = clock();

		printAnimalsAlongCells(timeStep, 1);

		t1 = clock();
		cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
		timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";
		cout << "DONE" << endl << flush;

		cout << " - Printing summary file ... " << endl << flush;
		t0 = clock();

		//printDailySummary(file, day);
		printExtendedDailySummary(extendedDailySummaryFile, timeStep);

		t1 = clock();
		cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
		timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t";
		cout << "DONE" << endl << flush;

		cout << " - Purging dead animals ... " << endl << flush;
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
		cout << "Time: " << (double(t1-t0)/CLOCKS_PER_SEC) << " secs." << endl;
		timeSpentFile << (double(t1-t0)/CLOCKS_PER_SEC) << "\t" << endl;
		cout << "DONE" << endl << flush;


		//ALWAYS print this genetics after purguing dead animals or before the whole day
		if(saveGeneticsSummaries == true)
		{
			//TODO FIX THIS INSIDE according to new Genome classes... (use debugging)
			printGeneticsSummaries(timeStep);
		}

		printCellAlongCells(timeStep);

		ofstream predationEventsOnOtherSpeciesFile;
		createOutputFile(predationEventsOnOtherSpeciesFile, outputDirectory / fs::path("Matrices"), predationEventsOnOtherSpeciesFilename, "txt");
		if (!predationEventsOnOtherSpeciesFile.is_open())
		{
			cerr << "Error opening the file." << endl;
		}
		else
		{
			printPredationEventsOnOtherSpeciesMatrix(predationEventsOnOtherSpeciesFile);
			predationEventsOnOtherSpeciesFile.close();
		}

		if (saveIntermidiateVolumes && (((timeStep + 1) % saveIntermidiateVolumesPeriodicity) == 0))
		{
			saveWaterSnapshot(outputDirectory / fs::path("Snapshots") / fs::path("Water"), timeStep);

			for (std::vector<Species *>::iterator fungIt = existingFungiSpecies.begin(); fungIt != existingFungiSpecies.end(); fungIt++)
			{
				saveFungusSpeciesSnapshot(outputDirectory / fs::path("Snapshots") / fs::path("Fungus"), timeStep, *fungIt);
			}

			for (std::vector<Species *>::iterator animIt = existingAnimalSpecies.begin(); animIt != existingAnimalSpecies.end(); animIt++)
			{
				saveAnimalSpeciesSnapshot(outputDirectory / fs::path("Snapshots") / fs::path("Animal"), timeStep, *animIt);
			}
		}
		if(exitAtFirstExtinction){
			if (isExtinguished())
			{
				break;
			}
		}
	}

	saveWaterSnapshot(outputDirectory / fs::path("Snapshots") / fs::path("Water_final"), runDays*timeStepsPerDay);

	for (std::vector<Species *>::iterator fungiSpeciesIt = existingFungiSpecies.begin(); fungiSpeciesIt != existingFungiSpecies.end(); fungiSpeciesIt++)
	{
		saveFungusSpeciesSnapshot(outputDirectory / fs::path("Snapshots") / fs::path("Fungus_final"), runDays*timeStepsPerDay, *fungiSpeciesIt);
	}

	for (std::vector<Species *>::iterator animalSpeciesIt = existingAnimalSpecies.begin(); animalSpeciesIt != existingAnimalSpecies.end(); animalSpeciesIt++)
	{
		saveAnimalSpeciesSnapshot(outputDirectory / fs::path("Snapshots") / fs::path("Animal_final"), runDays*timeStepsPerDay, *animalSpeciesIt);
	}

	/*
	encountersMatrixFilename = outputDirectory + "/Matrices/" + encountersMatrixFilename;
	predationsMatrixFilename = outputDirectory + "/Matrices/" + predationsMatrixFilename;
	nodesMatrixFilename = outputDirectory + "/Matrices/" + nodesMatrixFilename;

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
						currentTerrainCell = terrain[z][y][x];
						if(!currentTerrainCell->isObstacle())
						{
							animalPopulation[k] += currentTerrainCell->getLifeStagePopulation(Animal::LIFE_STAGES::ACTIVE, existingAnimalSpecies[k]);
							animalPopulation[k] += currentTerrainCell->getLifeStagePopulation(Animal::LIFE_STAGES::UNBORN, existingAnimalSpecies[k]);
							animalPopulation[k] += currentTerrainCell->getLifeStagePopulation(Animal::LIFE_STAGES::SATIATED, existingAnimalSpecies[k]);
							animalPopulation[k] += currentTerrainCell->getLifeStagePopulation(Animal::LIFE_STAGES::HANDLING, existingAnimalSpecies[k]);
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
						currentTerrainCell = terrain[z][y][x];
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
	std::vector<double> fungusBiomass(existingFungiSpecies.size());

	unsigned int i = 0;

	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = 0; z < depth; z++)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				currentTerrainCell = terrain[z][y][x];
				if (!currentTerrainCell->isObstacle())
				{
					i = 0;
					for (std::vector<Species *>::iterator fungiSpeciesIt = existingFungiSpecies.begin(); fungiSpeciesIt != existingFungiSpecies.end(); fungiSpeciesIt++)
					{
						fungusBiomass[i] += currentTerrainCell->getFungusBiomass(*fungiSpeciesIt);
						i++;
					}

					i = 0;
					for (std::vector<Species *>::iterator animalSpeciesIt = existingAnimalSpecies.begin(); animalSpeciesIt != existingAnimalSpecies.end(); animalSpeciesIt++)
					{
						animalPopulation[i] += currentTerrainCell->getLifeStagePopulation(Animal::LIFE_STAGES::ACTIVE, *animalSpeciesIt);
						animalPopulation[i] += currentTerrainCell->getLifeStagePopulation(Animal::LIFE_STAGES::UNBORN, *animalSpeciesIt);
						i++;
					}
				}
			}
		}
	}

	for (i = 0; i < fungusBiomass.size(); i++)
	{
		if (fungusBiomass[i] == 0)
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
	ifstream temperatureFile(inputDirectory + "/temperature/" + temperatureFilename);
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

void World::initializeTerrainDimensions()
{
	std::cout << "Initializing terrain voxels ... " << flush;
	try
	{
		terrain.resize(depth); // Create depth

		for (unsigned int z = 0; z < depth; ++z)
		{
			terrain[z].resize(length);

			for (unsigned int y = 0; y < length; y++)
			{
				terrain[z][y].resize(width);

				// Now data can be accesed this way:
				// terrain[i][j][k] = 6.0;

				for (unsigned int x = 0; x < width; x++)
				{
					terrain[z][y][x] = new TerrainCell(this, x, y, z, cellSize);
					terrain[z][y][x]->updateTemperature(0);
					terrain[z][y][x]->setRelativeHumidityOnRainEvent(0);
				}

			}
		}
	} catch (std::bad_alloc&) // Treat different exceptions
	{
		std::cout << "caught bad alloc exception, perhaps out of memory?, exiting.." << std::endl;
		exit(-1);
	} catch (const std::exception& x)
	{
		std::cerr << typeid(x).name() << std::endl;
	} catch (...)
	{
		std::cerr << "unknown exception" << std::endl;
	}

	std::cout << "DONE" << std::endl;
}

void World::readObstaclePatchesFromJSONFiles()
{
	cout << "Reading obstacle patches from JSON files ... " << endl;

	fs::path obstacleDirectory = inputDirectory / obstacleDirectoryName;
	fs::directory_iterator end_iter;

	if (fs::exists(obstacleDirectory) && fs::is_directory(obstacleDirectory))
	{
		vector<fs::path> filePaths;
		copy(fs::directory_iterator(obstacleDirectory), fs::directory_iterator(), back_inserter(filePaths));
		sort(filePaths.begin(), filePaths.end());             // directory iteration is not ordered on some file systems, so we sort them
		json ptMain;
		for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				string patchFilename = it->string();

				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, fs::path(SCHEMA_FOLDER) / fs::path(OBSTACLE_SCHEMA));

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
		cerr << "The specified path \"" + obstacleDirectory.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}
}

void World::readMoisturePatchesFromJSONFiles()
{
	cout << "Reading moisture patches from JSON files ... " << endl;
	
	fs::path moistureDirectory = inputDirectory / moistureDirectoryName;
	fs::directory_iterator end_iter;

	if (fs::exists(moistureDirectory) && fs::is_directory(moistureDirectory))
	{
		vector<fs::path> filePaths;
		copy(fs::directory_iterator(moistureDirectory), fs::directory_iterator(), back_inserter(filePaths));
		sort(filePaths.begin(), filePaths.end());             // directory iteration is not ordered on some file systems, so we sort them
		json ptMain;
		for (vector<fs::path>::const_iterator it(filePaths.begin()); it != filePaths.end(); ++it)
		{
			if (it->extension() == ".json")
			{
				string patchFilename = it->string();

				ptMain.clear();

				// Read configuration file
				ptMain = readConfigFile(*it, fs::path(SCHEMA_FOLDER) / fs::path(MOISTURE_SCHEMA));

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
					float maximumFungiCapacity = ptMain["patch"]["maximumFungiCapacity"];
					bool inEnemyFreeSpace = ptMain["patch"]["inEnemyFreeSpace"];
					bool inCompetitorFreeSpace = ptMain["patch"]["inCompetitorFreeSpace"];

					setSphericalWaterPatch(patchFilename, radius, x, y, z, useRelativeHumidityCycle, temperatureCycle, relativeHumidityCycle, useRelativeHumidityDecayOverTime, relativeHumidityOnRainEvent, relativeHumidityDecayOverTime, timeStepsBetweenRainEvents, standardDeviationForRainEvent, maximumFungiCapacity, inEnemyFreeSpace, inCompetitorFreeSpace);
				}
				else if(type == "cube")
				{
					// This creates a cube-like moisture patch, with center in patchCenter and dimensions
					// in patchDimensions. 
					Coordinate3D<int> patchCenter;
					Coordinate3D<int> patchDimensions;

					patchCenter.setX(ptMain["patch"]["center"]["x"]);
					patchCenter.setY(ptMain["patch"]["center"]["y"]);
					patchCenter.setZ(ptMain["patch"]["center"]["z"]);

					patchDimensions.setX(ptMain["patch"]["dimensions"]["x"]);
					patchDimensions.setY(ptMain["patch"]["dimensions"]["y"]);
					patchDimensions.setZ(ptMain["patch"]["dimensions"]["z"]);

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
					float maximumFungiCapacity = ptMain["patch"]["maximumFungiCapacity"];
					bool inEnemyFreeSpace = ptMain["patch"]["inEnemyFreeSpace"];
					bool inCompetitorFreeSpace = ptMain["patch"]["inCompetitorFreeSpace"];

					setCubicWaterPatch(patchFilename, patchCenter, patchDimensions, useRelativeHumidityCycle, temperatureCycle, relativeHumidityCycle, useRelativeHumidityDecayOverTime, relativeHumidityOnRainEvent, relativeHumidityDecayOverTime, timeStepsBetweenRainEvents, standardDeviationForRainEvent, maximumFungiCapacity, inEnemyFreeSpace, inCompetitorFreeSpace);

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
		cerr << "The specified path \"" + moistureDirectory.string() + "\" does not exist or it is not a directory" << endl;
		exit(-1);
	}
}

void World::eraseTerrain()
{
	for(auto cell2D : terrain) {
		for(auto cell1D : cell2D) {
			for(auto cell : cell1D) {
				delete cell;
			}
		}
	}
}

bool World::getCompetitionAmongFungiSpecies()
{
	return competitionAmongFungiSpecies;
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

const vector<Species *> * World::getExistingAnimalSpecies()
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

void World::setObstacleDirectoryName(fs::path newObstacleDirectoryName)
{
	obstacleDirectoryName = newObstacleDirectoryName;
}

void World::setMoistureDirectoryName(fs::path newMoistureDirectoryName)
{
	moistureDirectoryName = newMoistureDirectoryName;
}

void World::setFungiDirectoryName(fs::path newFungiDirectoryName)
{
	fungiDirectoryName = newFungiDirectoryName;
}

void World::setSpeciesDirectoryName(fs::path newSpeciesDirectoryName)
{
	speciesDirectoryName = newSpeciesDirectoryName;
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

//this is a public version for InEnemyFreeSpace - so animals can access it
TerrainCell * World::getCell2(unsigned int z, unsigned int y, unsigned int x)
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

void World::setHomogeneousFungus(Species* species, double value)
{
	std::cout << "Initializing homogeneous fungus value for all cells (" << species->getScientificName() << ") ... \n"
			<< std::endl;

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
					currentTerrainCell = terrain[z][y][x];
					if (!currentTerrainCell->isObstacle())
					{
						/*if(value > currentTerrainCell->getMaximumFungiCapacity())
						{
							std::cerr << "For the species '" << species->getScientificName() << "':" << std::endl;
							std::cerr << "TerrainCell (" << x << "," << y << "," << z << " ): Fungus value must be lower than the cell maximumFungiCapacity. You entered " << value << " > " << currentTerrainCell->getMaximumFungiCapacity() << ". Exiting now" << std::endl;
							exit(-1);
						}*/
						Edible* aux = currentTerrainCell->getFungus(species);

						if (aux == NULL)
						{
							currentTerrainCell->addFungus(new Fungus(species, value, currentTerrainCell));
							//TODO PROVISIONAL!!!!
							currentTerrainCell->setAuxInitialFungiBiomass(value, species->getId());
						}
						else
						{
							aux->setBiomass(max(value, aux->calculateWetMass()));
							//TODO PROVISIONAL!!!!
							currentTerrainCell->setAuxInitialFungiBiomass(max(value, aux->calculateWetMass()), species->getId());
						}
					}
				}
			}
		}
	}
	else
	{
		std::cerr << "For the species '" << species->getScientificName() << "':" << std::endl;
		std::cerr << "Fungus value must be 0 or positive. You entered " << value << ". Exiting now" << std::endl;
		exit(-1);
	}
	cout << "DONE" << endl;
}

void World::setGaussianFungusPatch(Species* species, unsigned int xpos, unsigned int ypos, unsigned int zpos,
		unsigned int radius, float sigma, float amplitude)
{
	// Generate water patches
	Coordinate3D<int> center;
	IsotropicGaussian3D gauss;

	cout << "Initializing Gaussian fungus patch (" << species->getScientificName() << ") ... \n" << endl;

	cout << " - Position (x,y,z) = " << xpos << "," << ypos << "," << zpos << " ... ";
	cout << " - Parameters (Influence radius, Amplitude, Sigma) = " << radius << "," << amplitude << "," << sigma
			<< "... ";

	gauss.setSigma(sigma);
	gauss.setAmplitude(amplitude);
	cout << "DONE" << endl;

	// Generate random coordinates for the center of the patch
	center.setX(xpos);
	center.setY(ypos);
	center.setZ(zpos);

	double fungusAsGauss;

	// Now iterate around the center to fill up with new water contents
	TerrainCell* currentTerrainCell = NULL;
	for (int x = (center.X() - (int) radius); x <= (center.X() + (int) radius); x++)
	{
		if (x >= 0 && x < (int) width) // Ckeck limits
		{
			for (int y = (center.Y() - (int) radius); y <= (center.Y() + (int) radius); y++)
			{
				if (y >= 0 && y < (int) length) // Ckeck limits
				{
					for (int z = (center.Z() - (int) radius); z <= (center.Z() + (int) radius); z++)
					{
						if (z >= 0 && z < (int) depth) // Ckeck limits
						{
							currentTerrainCell = terrain[z][y][x];
							if (!currentTerrainCell->isObstacle())
							{
								fungusAsGauss = gauss.getValueAtDistance(center.X() - x, center.Y() - y, center.Z() - z); // Value obtained from Gaussian

								Edible* aux = currentTerrainCell->getFungus(species);

								if (aux == NULL)
								{
									currentTerrainCell->addFungus(new Fungus(species, fungusAsGauss, currentTerrainCell));
									//TODO PROVISIONAL!!!!
									currentTerrainCell->setAuxInitialFungiBiomass(fungusAsGauss, species->getId());
								}
								else
								{
									aux->setBiomass(max(fungusAsGauss, aux->calculateWetMass()));
									//TODO PROVISIONAL!!!!
									currentTerrainCell->setAuxInitialFungiBiomass(max(fungusAsGauss, aux->calculateWetMass()), species->getId());
								}
							}
						}
					}
				}
			}
		}
	}
}

void World::setSphericalFungusPatch(Species* species, unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, double value)
{
	if (value >= 0)
	{
		// Generate water patches
		Coordinate3D<int> center;

		cout << "Initializing spherical fungus patch (" << species->getScientificName() << ") ... " << endl;

		cout << " - Position (x,y,z) = " << xpos << "," << ypos << "," << zpos << " ... ";
		cout << " - Parameters (radius, value) = " << radius << "," << value << "... ";

		// Generate coordinates for the center of the patch
		center.setX((int) xpos);
		center.setY((int) ypos);
		center.setZ((int) zpos);

		// Now iterate around the center to fill up with new water contents
		TerrainCell* currentTerrainCell = NULL;
		for (int x = (center.X() - (int) radius); x <= (center.X() + (int) radius); x++)
		{
			if (x >= 0 && x < (int) width) // Ckeck limits
			{
				for (int y = (center.Y() - (int) radius); y <= (center.Y() + (int) radius); y++)
				{
					if (y >= 0 && y < (int) length) // Ckeck limits
					{
						for (int z = (center.Z() - (int) radius); z <= (center.Z() + (int) radius); z++)
						{
							if (z >= 0 && z < (int) depth) // Ckeck limits
							{
								currentTerrainCell = terrain[z][y][x];
								if (!currentTerrainCell->isObstacle())
								{


									float distance = sqrt((center.X() - x) * (center.X() - x) + (center.Y() - y) * (center.Y() - y) + (center.Z() - z) * (center.Z() - z));

									if (distance <= radius)
									{
										/*if(value > currentTerrainCell->getMaximumFungiCapacity())
										{
											std::cerr << "For the species '" << species->getScientificName() << "':" << std::endl;
											std::cerr << "TerrainCell (" << x << "," << y << "," << z << " ): Fungus value must be lower than the cell maximumFungiCapacity. You entered " << value << " > " << currentTerrainCell->getMaximumFungiCapacity() << ". Exiting now" << std::endl;
											exit(-1);
										}*/

										Edible* aux = currentTerrainCell->getFungus(species);

										if (aux == NULL)
										{
											currentTerrainCell->addFungus(new Fungus(species, value, currentTerrainCell));

											cout << "RECENTLY ADDED VALUE: " << currentTerrainCell->getTotalFungusBiomass() << endl;
											//TODO PROVISIONAL!!!!
											currentTerrainCell->setAuxInitialFungiBiomass(value, species->getId());
																		
										}
										else
										{
											aux->setBiomass(max(value, aux->calculateWetMass()));
											//TODO PROVISIONAL!!!!
											currentTerrainCell->setAuxInitialFungiBiomass(max(value, aux->calculateWetMass()), species->getId());
										
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
		std::cerr << "Fungus value must be 0 or positive. You entered " << value << ". Exiting now" << std::endl;
		exit(-1);
	}
	cout << "DONE" << endl << endl;
}

void World::setCubicFungusPatch(Species* species, Coordinate3D<int> center, Coordinate3D<int> dimensions, double value)
{	
	if (value >= 0)
	{
		cout << "Initializing cubic fungus patch (" << species->getScientificName() << ") ... " << endl;

		cout << " - Position (x,y,z) = " << center.X() << "," << center.Y() << "," << center.Z() << " ... ";
		cout << " - Parameters (dimensions, value) = " << dimensions.X() << "," << dimensions.Y() << "," << dimensions.Z() << "," << value << "... ";

		// Now iterate around the center to fill up with new water contents
		TerrainCell* currentTerrainCell = NULL;

		// Defining limits of patch
		int minx, maxx, miny, maxy, minz, maxz;

		minx = ceil(center.X()-dimensions.X()/2.);
		maxx = ceil(center.X()+dimensions.X()/2.) - 1;
		miny = ceil(center.Y()-dimensions.Y()/2.);
		maxy = ceil(center.Y()+dimensions.Y()/2.) - 1;
		minz = ceil(center.Z()-dimensions.Z()/2.);
		maxz = ceil(center.Z()+dimensions.Z()/2.) - 1;

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
								currentTerrainCell = terrain[z][y][x];
								if (!currentTerrainCell->isObstacle())
								{
										Edible* aux = currentTerrainCell->getFungus(species);

										if (aux == NULL)
										{
											currentTerrainCell->addFungus(new Fungus(species, value, currentTerrainCell));

											cout << "RECENTLY ADDED VALUE: " << currentTerrainCell->getTotalFungusBiomass() << endl;
											//TODO PROVISIONAL!!!!
											currentTerrainCell->setAuxInitialFungiBiomass(value, species->getId());
																		
										}
										else
										{
											aux->setBiomass(max(value, aux->calculateWetMass()));
											//TODO PROVISIONAL!!!!
											currentTerrainCell->setAuxInitialFungiBiomass(max(value, aux->calculateWetMass()), species->getId());
										
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
		std::cerr << "Fungus value must be 0 or positive. You entered " << value << ". Exiting now" << std::endl;
		exit(-1);
	}
	cout << "DONE" << endl << endl;
}

void World::setRandomGaussianFungusPatches(Species* species, unsigned int number, float radius, float newSigma, bool useRandomSigma, float newAmplitude, bool useRandomAmplitude)
{
	// Generate water patches
	Coordinate3D<int> center;
	IsotropicGaussian3D gauss;

	std::cout << "Initializing random Gaussian fungus patches (" << species->getScientificName() << ") ... \n" << std::endl;

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

		std::cout << " - Creating random Gaussian shape with (Influence, Amplitude, Sigma) = " << radius << "," << amplitude << "," << sigma << "... ";
		gauss.setSigma(sigma);
		gauss.setAmplitude(amplitude);

		// Generate random coordinates for the center of the patch
		center.setX(Random::randomIntegerInRange(0, width - 1));
		center.setY(Random::randomIntegerInRange(0, length - 1));
		center.setZ(Random::randomIntegerInRange(0, depth - 1));
		std::cout << " - Position (x,y,z) = " << center.X() << "," << center.Y() << "," << center.Z() << " ... ";

		std::cout << "DONE" << std::endl;

		double fungusAsGauss;

		// Now iterate around the center to fill up with new water contents
		TerrainCell* currentTerrainCell = NULL;
		for (int x = (center.X() - (int) radius); x <= (center.X() + (int) radius); x++)
		{
			if (x >= 0 && x < (int) width) // Ckeck limits
			{
				for (int y = (center.Y() - (int) radius); y <= (center.Y() + (int) radius); y++)
				{
					if (y >= 0 && y < (int) length) // Ckeck limits
					{
						for (int z = (center.Z() - (int) radius); z <= (center.Z() + (int) radius); z++)
						{
							if (z >= 0 && z < (int) depth) // Ckeck limits
							{
								currentTerrainCell = terrain[z][y][x];
								if (!currentTerrainCell->isObstacle())
								{
									fungusAsGauss = gauss.getValueAtDistance(center.X() - x, center.Y() - y, center.Z() - z); // Value obtained from Gaussian

									Edible* aux = currentTerrainCell->getFungus(species);

									if (aux == NULL)
									{
										currentTerrainCell->addFungus(new Fungus(species, fungusAsGauss, currentTerrainCell));
										//TODO PROVISIONAL!!!!
										currentTerrainCell->setAuxInitialFungiBiomass(fungusAsGauss, species->getId());
									}
									else
									{
										aux->setBiomass(max(fungusAsGauss, aux->calculateWetMass()));
										//TODO PROVISIONAL!!!!
										currentTerrainCell->setAuxInitialFungiBiomass(max(fungusAsGauss, aux->calculateWetMass()), species->getId());
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
	std::cout << "Initializing homogeneous water (" << value << ") for all space ... ";
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
						currentTerrainCell = terrain[z][y][x];
						if (!currentTerrainCell->isObstacle())
						{
							currentTerrainCell->setRelativeHumidityOnRainEvent(max(currentTerrainCell->getWater(), value));
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

	std::cout << "DONE" << std::endl;
}

void World::setCubicObstaclePatch(string patchFilename, unsigned int depthStartPoint, unsigned int lengthStartPoint, unsigned int widthStartPoint, unsigned int patchDepth, unsigned int patchLength,
		unsigned int patchWidth)
{
	cout << "Initializing cubic obstacle patch \"" << patchFilename << "\":" << endl;
	cout << " - Position (x,y,z) = (" << widthStartPoint << "," << lengthStartPoint << "," << depthStartPoint << ")" << endl;
	cout << " - Parameters (width, length, depth) = (" << patchWidth << "," << patchLength << "," << patchDepth << ") ... ";
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
								if (!terrain[z][y][x]->isObstacle())
								{
									terrain[z][y][x]->setObstacle(true);
								}
								else
								{
									cout << "WARNING: The patch from file \"" << patchFilename << "\" overlaps with the previously declared obstacle" << endl;
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
	cout << "DONE" << endl;
}

void World::setSphericalWaterPatch(string patchFilename, unsigned int radius, unsigned int xpos, unsigned int ypos, unsigned int zpos, bool useRelativeHumidityCycle, const vector<float>& temperatureCycle, const vector<float>& relativeHumidityCycle, bool useRelativeHumidityDecayOverTime, float relativeHumidityOnRainEvent, float relativeHumidityDecayOverTime, int timeStepsBetweenRainEvents, int standardDeviationForRainEvent, float maximumFungiCapacity, bool inEnemyFreeSpace, bool inCompetitorFreeSpace)
{
	if(useRelativeHumidityCycle != useRelativeHumidityDecayOverTime)
	{
		if (relativeHumidityOnRainEvent > 0)
		{
			if(relativeHumidityDecayOverTime >= 0)
			{
				// Generate water patches
				Coordinate3D<int> center;

				std::cout << "Initializing spherical water patch \"" << patchFilename << "\":" << endl;
				std::cout << " - Position (x,y,z) = (" << xpos << "," << ypos << "," << zpos << ")" << endl;
				std::cout << " - Parameters (radius, value) = (" << radius << "," << relativeHumidityOnRainEvent << ") ... ";

				// Generate coordinates for the center of the patch
				center.setX((int) xpos);
				center.setY((int) ypos);
				center.setZ((int) zpos);

				// Now iterate around the center to fill up with new water contents
				TerrainCell* currentTerrainCell = NULL;
				for (int x = (center.X() - (int) radius); x <= (center.X() + (int) radius); x++)
				{
					if (x >= 0 && x < (int) width) // Ckeck limits
					{
						for (int y = (center.Y() - (int) radius); y <= (center.Y() + (int) radius); y++)
						{
							if (y >= 0 && y < (int) length) // Ckeck limits
							{
								for (int z = (center.Z() - (int) radius); z <= (center.Z() + (int) radius); z++)
								{
									if (z >= 0 && z < (int) depth) // Ckeck limits
									{
										currentTerrainCell = terrain[z][y][x];
										if (!currentTerrainCell->isObstacle())
										{
											float distance = sqrt((center.X() - x) * (center.X() - x) + (center.Y() - y) * (center.Y() - y) + (center.Z() - z) * (center.Z() - z));

											if (distance <= radius)
											{
												currentTerrainCell->setTemperatureCycle(temperatureCycle);
												currentTerrainCell->setRelativeHumidityCycle(relativeHumidityCycle);
												currentTerrainCell->setRelativeHumidityOnRainEvent(max(currentTerrainCell->getWater(), relativeHumidityOnRainEvent));
												currentTerrainCell->setRelativeHumidityDecayOverTime(relativeHumidityDecayOverTime);
												currentTerrainCell->setTimeStepsBetweenRainEvents(timeStepsBetweenRainEvents);
												currentTerrainCell->setStandardDeviationForRainEvent(standardDeviationForRainEvent);
												currentTerrainCell->setMaximumCapacities(maximumFungiCapacity);
												currentTerrainCell->setInEnemyFreeSpace(inEnemyFreeSpace);
												currentTerrainCell->setInCompetitorFreeSpace(inCompetitorFreeSpace);
												
												/* cout << "before   " << endl;
												cout << currentTerrainCell->isInPatch() << endl; */
												
												//arthro and for dinos - so fungi does not spread in cells that are supposed to be empty  
												//if (distance-radius) >= 0){ //so edges are not used to spread fungi causing no error
											 	
												
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
	cout << "DONE" << endl << endl;
}

void World::setCubicWaterPatch(string patchFilename, Coordinate3D<int> center, Coordinate3D<int> dimensions,  bool useRelativeHumidityCycle, const vector<float>& temperatureCycle, const vector<float>& relativeHumidityCycle, bool useRelativeHumidityDecayOverTime, float relativeHumidityOnRainEvent, float relativeHumidityDecayOverTime, int timeStepsBetweenRainEvents, int standardDeviationForRainEvent, float maximumFungiCapacity, bool inEnemyFreeSpace, bool inCompetitorFreeSpace)
{
	if(useRelativeHumidityCycle != useRelativeHumidityDecayOverTime)
	{
		if (relativeHumidityOnRainEvent > 0)
		{
			if(relativeHumidityDecayOverTime >= 0)
			{
				// Generate water patches
				Coordinate3D<int> center;

				std::cout << "Initializing cubic water patch \"" << patchFilename << "\":" << endl;
				std::cout << " - Center position (x,y,z) = (" << center.X() << "," << center.Y() << "," << center.Z() << ")" << endl;
				std::cout << " - Dimensions (xdim, ydim, zdim, value) = (" << dimensions.X() << "," << dimensions.Y() << "," << dimensions.Z() << "," << relativeHumidityOnRainEvent << ") ... ";

				// Defining limits of patch
				int minx, maxx, miny, maxy, minz, maxz;

				minx = ceil(center.X()-dimensions.X()/2.);
				maxx = ceil(center.X()+dimensions.X()/2.) - 1;
				miny = ceil(center.Y()-dimensions.Y()/2.);
				maxy = ceil(center.Y()+dimensions.Y()/2.) - 1;
				minz = ceil(center.Z()-dimensions.Z()/2.);
				maxz = ceil(center.Z()+dimensions.Z()/2.) - 1;

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
										currentTerrainCell = terrain[z][y][x];
										if (!currentTerrainCell->isObstacle())
										{
											currentTerrainCell->setTemperatureCycle(temperatureCycle);
											currentTerrainCell->setRelativeHumidityCycle(relativeHumidityCycle);
											currentTerrainCell->setRelativeHumidityOnRainEvent(max(currentTerrainCell->getWater(), relativeHumidityOnRainEvent));
											currentTerrainCell->setRelativeHumidityDecayOverTime(relativeHumidityDecayOverTime);
											currentTerrainCell->setTimeStepsBetweenRainEvents(timeStepsBetweenRainEvents);
											currentTerrainCell->setStandardDeviationForRainEvent(standardDeviationForRainEvent);
											currentTerrainCell->setMaximumCapacities(maximumFungiCapacity);
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
	cout << "DONE" << endl << endl;
}

void World::setGaussianWaterPatch(unsigned int xpos, unsigned int ypos, unsigned int zpos, unsigned int radius, float sigma, float amplitude)
{
	// Generate water patches
	Coordinate3D<int> center;
	IsotropicGaussian3D gauss;

	cout << "Initializing Gaussian water patch ... \n" << endl;

	cout << " - Position (x,y,z) = " << xpos << "," << ypos << "," << zpos << " ... ";
	cout << " - Parameters (Influence radius, Amplitude, Sigma) = " << radius << "," << amplitude << "," << sigma << "... ";

	gauss.setSigma(sigma);
	gauss.setAmplitude(amplitude);
	cout << "DONE" << endl;

	// Generate random coordinates for the center of the patch
	center.setX(xpos);
	center.setY(ypos);
	center.setZ(zpos);

	float waterAsGauss;

	// Now iterate around the center to fill up with new water contents
	TerrainCell* currentTerrainCell = NULL;
	for (int x = (center.X() - (int) radius); x <= (center.X() + (int) radius); x++)
	{
		if (x >= 0 && x < (int) width) // Ckeck limits
		{
			for (int y = (center.Y() - (int) radius); y <= (center.Y() + (int) radius); y++)
			{
				if (y >= 0 && y < (int) length) // Ckeck limits
				{
					for (int z = (center.Z() - (int) radius); z <= (center.Z() + (int) radius); z++)
					{
						if (z >= 0 && z < (int) depth) // Ckeck limits
						{
							currentTerrainCell = terrain[z][y][x];
							if (!currentTerrainCell->isObstacle())
							{
								waterAsGauss = gauss.getValueAtDistance(center.X() - x, center.Y() - y, center.Z() - z); // Value obtained from Gaussian
								currentTerrainCell->setRelativeHumidityOnRainEvent(max(currentTerrainCell->getWater(), waterAsGauss));
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
	Coordinate3D<int> center;
	IsotropicGaussian3D gauss;

	cout << "Initializing random Gaussian water patches ... \n" << endl;

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

		cout << " - Creating random Gaussian shape with (Influence, Amplitude, Sigma) = " << radius << ","
				<< amplitude << "," << sigma << "... ";
		gauss.setSigma(sigma);
		gauss.setAmplitude(amplitude);

		// Generate random coordinates for the center of the patch
		center.setX(Random::randomIntegerInRange(0, width - 1));
		center.setY(Random::randomIntegerInRange(0, length - 1));
		center.setZ(Random::randomIntegerInRange(0, depth - 1));
		cout << " - Position (x,y,z) = " << center.X() << "," << center.Y() << "," << center.Z() << " ... ";

		cout << "DONE" << endl;

		float waterAsGauss;

		// Now iterate around the center to fill up with new water contents
		TerrainCell* currentTerrainCell = NULL;
		for (int x = (center.X() - (int) radius); x <= (center.X() + (int) radius); x++)
		{
			if (x >= 0 && x < (int) width) // Ckeck limits
			{
				for (int y = (center.Y() - (int) radius); y <= (center.Y() + (int) radius); y++)
				{
					if (y >= 0 && y < (int) length) // Ckeck limits
					{
						for (int z = (center.Z() - (int) radius); z <= (center.Z() + (int) radius); z++)
						{
							if (z >= 0 && z < (int) depth) // Ckeck limits
							{
								currentTerrainCell = terrain[z][y][x];
								if (!currentTerrainCell->isObstacle())
								{
									waterAsGauss = gauss.getValueAtDistance(center.X() - x, center.Y() - y, center.Z() - z); // Value obtained from Gaussian
									currentTerrainCell->setRelativeHumidityOnRainEvent(max(currentTerrainCell->getWater(), waterAsGauss));
								}
							}
						}
					}
				}
			}
		}
	}
}

void World::calculateAttackStatistics(map<Species,vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells)
{
	cout << "Size of the Animal class: " << sizeof(Animal) << endl;
	cout << "Size of the Genome class: " << sizeof(Genome) << endl;
	cout << "Size of the TerrainCell class: " << sizeof(TerrainCell) << endl;
	cout << "Creating heating code individuals... " << endl;

	vector<pair<Animal *, Instar> > animalAndInstarAtInitialization;

	map<Species, vector<Edible*>*> * animalsPopulation = generateStatisticsPopulation(animalAndInstarAtInitialization, mapSpeciesInhabitableTerrainCells);

	//Calculating pseudoGrowthSd
	for (auto animalAndInstarAtInitializationIt = animalAndInstarAtInitialization.begin(); animalAndInstarAtInitializationIt != animalAndInstarAtInitialization.end(); animalAndInstarAtInitializationIt++)
	{
		(*animalAndInstarAtInitializationIt).first->sumPseudoGrowthSd();
	}

	for (vector<Species *>::iterator speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
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
		float temperatureOnMolting = (*animalAndInstarAtInitializationIt).first->forceMolting(timeStepsPerDay, -1, (*animalAndInstarAtInitializationIt).second);
		ofstream noStream;
		(*animalAndInstarAtInitializationIt).first->tuneTraits(-1, timeStepsPerDay, temperatureOnMolting, 100, noStream, false, true);

		updateMaxSearchArea((*animalAndInstarAtInitializationIt).first->getSearchArea());

		//DONE tuneTraits tiene que ir AQUI y ADEMÁS con la temperatura final en la que mudaron CADA UNO DE ELLOS
	}
	cout << "A total of " << animalAndInstarAtInitialization.size() << " heating code individuals have been created." << endl;

	//Only for predators. The experiment is carried out for every predator species and its linked species, up until the specified numberOfCombinations.

	//TODO MIRAR MEMORY LEAKS

	cout << "Calculating attack statistics: " << endl;
	for (vector<Species *>::iterator speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
	{
		Species* currentAnimalSpecies = (*speciesIt);
		vector<Species* >* currentEdibleAnimalSpecies = currentAnimalSpecies->getEdibleAnimalSpecies();

		if(!currentEdibleAnimalSpecies->empty())
		{
			cout << ">> Simulating " << numberOfCombinations << " attacks from the species \"" << currentAnimalSpecies->getScientificName() << "\"... " << endl;
			vector<Edible* >* currentVectorOfPredators = (*animalsPopulation)[*currentAnimalSpecies];
			
			vector<Edible*> auxAttacks;
			for (vector<Species *>::iterator innerSpeciesIt = currentEdibleAnimalSpecies->begin(); innerSpeciesIt != currentEdibleAnimalSpecies->end(); innerSpeciesIt++)
			{
				vector<Edible*> * currentVectorOfEdibleAnimals = (*animalsPopulation)[*(*innerSpeciesIt)];
				auxAttacks.insert(auxAttacks.end(), currentVectorOfEdibleAnimals->begin(), currentVectorOfEdibleAnimals->end());
			}

			float percentageForPrinting = 0.1;
			float currentPercentageForPrinting = percentageForPrinting;
			vector<pair<Edible*, Edible*>> vectorOfAttacks;
			vectorOfAttacks.reserve(numberOfCombinations);
			while(vectorOfAttacks.size() < numberOfCombinations)
			{
				Edible* hunterAnimal = currentVectorOfPredators->at(Random::randomIndex(currentVectorOfPredators->size()));
				Edible* huntedAnimal = auxAttacks.at(Random::randomIndex(auxAttacks.size()));
				//cout << "CAN EAT ANIMAL??? <" << huntedAnimal->getId() << " _ " << hunterAnimal->getId() << endl;
				
				double probabilityDensityFunction = exp(-0.5 * pow((log(hunterAnimal->calculateWetMass()/huntedAnimal->calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
				
				if(probabilityDensityFunction >= 0.08){  //Dinosaurs but everything else might be - p=0.003 from Alamosaurus - Tytannsaurus in PDF_fix_in heating_code.xls
					if(hunterAnimal->canEatEdible(huntedAnimal) && hunterAnimal != huntedAnimal)
					{
						//cout << "CAN EAT ANIMAL!!! <" << huntedAnimal->getId() << " _ " << hunterAnimal->getId() << endl;
						pair<Edible*, Edible*> currentAttack = make_pair(hunterAnimal, huntedAnimal);
						if(find(vectorOfAttacks.begin(), vectorOfAttacks.end(), currentAttack) == vectorOfAttacks.end())
						{
							vectorOfAttacks.push_back(currentAttack);

							//Computing the total mean values.
							currentAnimalSpecies->sumStatisticMeans(hunterAnimal, huntedAnimal, muForPDF, sigmaForPDF);
							currentAnimalSpecies->interactionRanges(hunterAnimal, huntedAnimal, muForPDF, sigmaForPDF);
						}
					}
					if(vectorOfAttacks.size() >= (numberOfCombinations-1)*currentPercentageForPrinting)
					{
						cout << ">>>> " << (int)(currentPercentageForPrinting*100) << "%... " << endl;
						currentPercentageForPrinting += percentageForPrinting;
					}
				}
			}

			//Computing the means
			currentAnimalSpecies->computeStatisticMeans(vectorOfAttacks.size());

			for(unsigned int i = 0; i < vectorOfAttacks.size(); i++)
			{
				//Computing the total sd values.
				Edible* hunterAnimal = vectorOfAttacks[i].first;
				Edible* huntedAnimal = vectorOfAttacks[i].second;

				double probabilityDensityFunction = exp(-0.5 * pow((log(hunterAnimal->calculateWetMass()/huntedAnimal->calculateWetMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
				
				if(probabilityDensityFunction >= 0.08){  //Dinosaurs but everything else might be - p=0.003 from Alamosaurus - Tytannsaurus in PDF_fix_in heating_code.xls

				currentAnimalSpecies->sumStatisticSds(hunterAnimal, huntedAnimal, muForPDF, sigmaForPDF);

				}

			}

			//Computing the sds
			currentAnimalSpecies->computeStatisticSds(vectorOfAttacks.size());
			vectorOfAttacks.clear();
			auxAttacks.clear();
		}
	}

	eraseStatisticsPopulation(animalsPopulation);

	Edible::resetIds();
	cout << "Calculating attack statistics DONE" << endl;
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

		double totalFungiWetBiomass = 0.0;
		//Initializing trophic level biomass based on Hatton et al 2015
		for(vector<TerrainCell*>::const_iterator inhabitableTerrainCellsIt = inhabitableTerrainCells.cbegin(); inhabitableTerrainCellsIt != inhabitableTerrainCells.cend(); inhabitableTerrainCellsIt++)
		{
			totalFungiWetBiomass += (*inhabitableTerrainCellsIt)->getTotalFungusBiomass();
		}

		double totalPreyWetBiomass = 0.08*pow(totalFungiWetBiomass,0.75);
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
			if((*speciesIt)->canEatAnyFungusSpecies())
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
			if((*speciesIt)->canEatAnyFungusSpecies())
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
		for (vector<Species *>::iterator speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
		{
			actualEcosystemSize += (*speciesIt)->getTotalInitialPopulation();
		}
		cout << " - Total initial ecosystem size from input: " << actualEcosystemSize << " individuals." << endl;
	}
}

void World::obtainSpeciesInhabitableTerrainCells(map<Species,vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells, const vector<TerrainCell*> &inhabitableTerrainCells)
{
	for (vector<Species *>::iterator speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
	{
		Species* currentAnimalSpecies = *speciesIt;

		vector<Species*> * involvedFungusSpecies = currentAnimalSpecies->getInvolvedFungusSpecies();

		vector<TerrainCell*>* speciesInhabitableTerrainCells = new vector<TerrainCell*>();

		bool cellHasBeenAdded;

		TerrainCell* currentTerrainCell = NULL;
		for (unsigned int i = 0; i < inhabitableTerrainCells.size(); i++)
		{
			currentTerrainCell = inhabitableTerrainCells[i];
			cellHasBeenAdded = false;
			for(unsigned int j = 0; j < involvedFungusSpecies->size() && !cellHasBeenAdded; j++)
			{
				if(currentTerrainCell->containsFungusSpecies(involvedFungusSpecies->at(j)))
				{
					speciesInhabitableTerrainCells->push_back(currentTerrainCell);
					cellHasBeenAdded = true;
				}
			}
		}

		mapSpeciesInhabitableTerrainCells[*currentAnimalSpecies] = speciesInhabitableTerrainCells;

		delete involvedFungusSpecies;
	}
}

map<Species, vector<Edible*>*>* World::generateStatisticsPopulation(vector<pair<Animal *, Instar> > &animalAndInstarAtInitialization, map<Species,vector<TerrainCell*>*> &mapSpeciesInhabitableTerrainCells)
{
	map<Species, vector<Edible*>*> * animalsPopulation = new map<Species, vector<Edible*>*>(); 

	//TODO Separarlo TOTALMENTE del mundo de la simulación
	for (vector<Species *>::iterator speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
	{
		Species* currentAnimalSpecies = *speciesIt;

		(*animalsPopulation)[*currentAnimalSpecies] = new vector<Edible*>();

		//TODO AÑADIR que solamente haya un PORCENTAJE de individuos en cada instar (un vector solamente), a partir de un numero total (un total para simulacion, y otro total diferente para estadisticas)
		vector<unsigned long>* initialPopulation = currentAnimalSpecies->getStatisticsInitialPopulation();
		vector<TerrainCell*>* speciesInhabitableTerrainCells = mapSpeciesInhabitableTerrainCells[*currentAnimalSpecies];

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

				Animal * newAnimal = new Animal(currentAnimalSpecies->getChromosomesGenerator()->getNewGenome(), -1, newCell, 0, timeStepsPerDay, 0, 0, -1,
						-1, currentAnimalSpecies, currentAnimalSpecies->getRandomGender(), currentAnimalSpecies->getDefaultHuntingMode());

				newAnimal->updateLimits();
				newAnimal->sumPseudoGrowthMean();
				
				newCell->addAnimal(newAnimal);

				animalAndInstarAtInitialization.push_back(make_pair(newAnimal, instar));

				(*animalsPopulation)[*currentAnimalSpecies]->push_back(newAnimal);
			}
			if(instarAtInitialization-1 >= (actualPopulationInstarsSize-1)*currentPercentageForPrinting)
			{
				cout << ">>>> " << (int)(currentPercentageForPrinting*100) << "%... " << endl;
				currentPercentageForPrinting += percentageForPrinting;
			}
		}
		currentAnimalSpecies->calculatePseudoGrowthMean();
	}

	return animalsPopulation;
}

void World::eraseStatisticsPopulation(map<Species, vector<Edible*>*> * animalsPopulation)
{
	for(auto &[key, value] : *animalsPopulation)
	{
		for(Edible* currentAnimal : *value)
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

int World::generatePopulation(Species* currentAnimalSpecies, const vector<TerrainCell*> &speciesInhabitableTerrainCells, ofstream &constitutiveTraitsFile)
{
	int numberOfDiscardedIndividualsOutsideRestrictedRanges = 0;

	cout << "Creating " << currentAnimalSpecies->getTotalInitialPopulation() << " individuals of the species \"" << currentAnimalSpecies->getScientificName() << "\"..." << endl;
	cout << ">> Number of inhabitable cells (with involved fungus resources): " << speciesInhabitableTerrainCells.size() << endl;

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

			Animal * newAnimal = new Animal(currentAnimalSpecies->getChromosomesGenerator()->getNewGenome(), -1, newCell, 0, timeStepsPerDay, 0, 0, -1,
					-1, currentAnimalSpecies, currentAnimalSpecies->getRandomGender(), currentAnimalSpecies->getDefaultHuntingMode(), true);

			pair<bool, bool> isInsideRestrictedRangesAndIsViableOffSpring = newAnimal->interpolateTraits();
			while(!isInsideRestrictedRangesAndIsViableOffSpring.first)
			{
				numberOfDiscardedIndividualsOutsideRestrictedRanges++;
				delete newAnimal;
				newAnimal = new Animal(currentAnimalSpecies->getChromosomesGenerator()->getNewGenome(), -1, newCell, 0, timeStepsPerDay, 0, 0, -1,
										-1, currentAnimalSpecies, currentAnimalSpecies->getRandomGender(), currentAnimalSpecies->getDefaultHuntingMode(), true);
				isInsideRestrictedRangesAndIsViableOffSpring = newAnimal->interpolateTraits();
			}
			// Indicate that the animal created is the final animal, and therefore assign a final ID to it.
			newAnimal->doDefinitive();
			//ALWAYS print the traits after interpolating and before adjusting
			//newAnimal->printGenetics(geneticsFile);
			newAnimal->printTraits(constitutiveTraitsFile);
			//DONE Adjust traits now includes the first CalculateGrowthCurves call. For every animal.
			newAnimal->adjustTraits();
			newAnimal->forceMolting2(timeStepsPerDay, -1, instar);
			//exit(-1);
			newCell->addAnimal(newAnimal);
		}
		if(instarAtInitialization-1 >= (currentAnimalSpecies->getInitialPopulation().size()-1)*currentPercentageForPrinting)
		{
			cout << ">> " << (int)(currentPercentageForPrinting*100) << "%... " << endl;
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

	map<Species,vector<TerrainCell*>*> mapSpeciesInhabitableTerrainCells;
	obtainSpeciesInhabitableTerrainCells(mapSpeciesInhabitableTerrainCells, inhabitableTerrainCells);

	calculateAttackStatistics(mapSpeciesInhabitableTerrainCells);

	cout << "Giving life to animals... " << endl;

	ofstream geneticsFile, constitutiveTraitsFile;
	createOutputFile(geneticsFile, outputDirectory, "animal_genetics", "txt", ofstream::app);
	if(!geneticsFile.is_open())
	{
		cerr << "Error opening the file." << endl;
	}
	else
	{
		createOutputFile(constitutiveTraitsFile, outputDirectory, "animal_constitutive_traits", "txt", ofstream::app);
		if(!constitutiveTraitsFile.is_open())
		{
			cerr << "Error opening the file." << endl;
		}
		else
		{
			geneticsFile << "id\tspecies\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\t" << endl;
			constitutiveTraitsFile << "id\tspecies\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2\t" << TraitConverter::printAvailableTraits() << endl;

			for (vector<Species *>::iterator speciesIt = existingAnimalSpecies.begin(); speciesIt != existingAnimalSpecies.end(); speciesIt++)
			{
				Species* currentAnimalSpecies = *speciesIt;

				vector<TerrainCell*>* speciesInhabitableTerrainCells = mapSpeciesInhabitableTerrainCells[*currentAnimalSpecies];
							 
				int numberOfDiscardedIndividualsOutsideRestrictedRanges = 0;

				if(initIndividualsPerDensities)
				{
					// TODO Mario Initialize animals with initIndividualsPerDensities
					/*
					initialPopulation = currentAnimalSpecies->getInitialPopulation();

					cout << "Creating " << currentAnimalSpecies->getTotalInitialPopulation() << " biomass worth of the species \"" << currentAnimalSpecies->getScientificName() << "\"..." << endl;
					cout << ">> Number of inhabitable cells (with involved fungus resources): " << speciesInhabitableTerrainCells->size() << endl;
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

							Animal * newAnimal = new Animal(currentAnimalSpecies->getChromosomesGenerator()->getNewGenome(), -1, newCell, 0, timeStepsPerDay, 0, 0, -1,
									-1, currentAnimalSpecies, currentAnimalSpecies->getRandomGender(), currentAnimalSpecies->getDefaultHuntingMode());

							pair<bool, bool> isInsideRestrictedRangesAndIsViableOffSpring = newAnimal->interpolateTraits();
							while(!isInsideRestrictedRangesAndIsViableOffSpring.first)
							{
								numberOfDiscardedIndividualsOutsideRestrictedRanges++;
								delete newAnimal;
								newAnimal = new Animal(currentAnimalSpecies->getChromosomesGenerator()->getNewGenome(), -1, newCell, 0, timeStepsPerDay, 0, 0, -1,
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
					numberOfDiscardedIndividualsOutsideRestrictedRanges = generatePopulation(currentAnimalSpecies, *speciesInhabitableTerrainCells, constitutiveTraitsFile);
				}

				cout << ">> A total of " << numberOfDiscardedIndividualsOutsideRestrictedRanges << " individuals have been discarded due to at least one trait value being outside restricted ranges. " << endl;
			}

			geneticsFile.close();
			constitutiveTraitsFile.close();
		}
	}

	for(auto &[key,value] : mapSpeciesInhabitableTerrainCells)
	{
		delete value;
	}

	cout << "DONE" << endl;
}

void World::readWaterFromVolume(string fileName)
{
	cout << "Reading water from volume " << fileName << "... ";
	fstream binf(fileName.c_str(), ios::binary | ios::in);

	double value;
	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = 0; z < depth; ++z)
	{
		for (unsigned int y = 0; y < length; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				currentTerrainCell = terrain[z][y][x];
				if (!currentTerrainCell->isObstacle())
				{
					binf.read((char*) &value, sizeof(value));
					currentTerrainCell->setRelativeHumidityOnRainEvent(value);
				}
			}
		}
	}

	cout << "DONE" << endl;
	binf.close();
}

string World::translateLifeStage(unsigned int lifeStage)
{
	switch(lifeStage) {
	case 0:
		return "unborn";
	case 1:
		return "active";
	case 2:
		return "starved";
	case 3:
		return "predated";
	case 4:
		return "reproducing";
	case 5:
		return "pupa";
	case 6:
		return "satiated";
	case 7:
		return "handling";
	case 8:
		return "diapause";
	case 9:
		return "background";
	case 10:
		return "senesced";
 	case 11:
        return "shocked";
	default:
		return "not a lifestage";
	}
}

void World::initializeOutputFiles()
{
	//string command = string("cp ") + string("run_params.json ") + outputDirectory;
	//system(command.c_str());
	
	// Copy simulation parameters into output directory
	fs::path run_params_source_path = fs::path("run_params.json");
	fs::path run_params_destination_path = outputDirectory / fs::path("run_params.json");
	fs::copy_file(run_params_source_path, run_params_destination_path, fs::copy_options::overwrite_existing);

	// Copy world parameters into output directory
	fs::path world_params_source_path = inputDirectory / fs::path("world_params.json");
	fs::path world_params_destination_path = outputDirectory / fs::path("world_params.json");
	fs::copy_file(world_params_source_path, world_params_destination_path, fs::copy_options::overwrite_existing);

	fs::path dailySummariFilename = outputDirectory / fs::path("dailySummary.txt");
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

	fs::path extendedDailySummaryFilename = outputDirectory / fs::path("extendedDailySummary.txt");
	extendedDailySummaryFile.open(extendedDailySummaryFilename);

	if (!extendedDailySummaryFile.is_open())
	{
		cerr << "Error opening the dailySummaryFile." << endl;
	}
	else
	{
		extendedDailySummaryFile << "day" << "\t";

		for (vector<Species *>::iterator itFungiSpecies = existingFungiSpecies.begin(); itFungiSpecies != existingFungiSpecies.end(); itFungiSpecies++)
		{
			extendedDailySummaryFile << (*itFungiSpecies)->getScientificName() << "_biomass" << "\t";
		}

		for (vector<Species *>::iterator itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
		{
			for (unsigned int lifeStage = 0; lifeStage <= Animal::LIFE_STAGES::SHOCKED; ++lifeStage)
			{
				extendedDailySummaryFile << (*itSpecies)->getScientificName() << "_" << translateLifeStage(lifeStage) << "\t";
			}
		}
		extendedDailySummaryFile << endl;
	}

	for (vector<Species *>::iterator itSpecies = existingAnimalSpecies.begin(); itSpecies != existingAnimalSpecies.end(); itSpecies++)
	{
		string scientificName = (*itSpecies)->getScientificName();
		std::replace(scientificName.begin(), scientificName.end(), ' ', '_');
		fs::path geneticsSummaryFilename = outputDirectory / fs::path("geneticsSummaries") / fs::path(scientificName + "_geneticsSummary.txt");
		geneticsSummaryFile.open(geneticsSummaryFilename);
		if (!geneticsSummaryFile.is_open())
		{
			cerr << "Error opening the file." << endl;
		}
		else
		{
			geneticsSummaryFile << "day" << "\t";
			geneticsSummaryFile << "population\t";
			geneticsSummaryFile << "energy_cr1\t";
			geneticsSummaryFile << "growth_cr1\t";
			geneticsSummaryFile << "pheno_cr1\t";
			geneticsSummaryFile << "body_cr1\t";
			geneticsSummaryFile << "assim_cr1\t";
			geneticsSummaryFile << "vor_cr1\t";
			geneticsSummaryFile << "speed_cr1\t";
			geneticsSummaryFile << "search_cr1\t";
			geneticsSummaryFile << "met_cr1\t";
			geneticsSummaryFile << "actE_vor_cr1\t";
			geneticsSummaryFile << "actE_spd_cr1\t";
			geneticsSummaryFile << "actE_srch_cr1\t";
			geneticsSummaryFile << "e_met_cr1\t";
			geneticsSummaryFile << "energy_cr2\t";
			geneticsSummaryFile << "growth_cr2\t";
			geneticsSummaryFile << "pheno_cr2\t";
			geneticsSummaryFile << "body_cr2\t";
			geneticsSummaryFile << "assim_cr2\t";
			geneticsSummaryFile << "vor_cr2\t";
			geneticsSummaryFile << "speed_cr2\t";
			geneticsSummaryFile << "search_cr2\t";
			geneticsSummaryFile << "met_cr2\t";
			geneticsSummaryFile << "actE_vor_cr2\t";
			geneticsSummaryFile << "actE_spd_cr2\t";
			geneticsSummaryFile << "actE_srch_cr2\t";
			geneticsSummaryFile << "e_met_cr2\t";
			geneticsSummaryFile << endl;
			geneticsSummaryFile.close();
		}

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
				currentTerrainCell = terrain[z][y][x];
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

void* World::growFungiThreadMaker(void* threadArgument)
{
	struct ThreadRangerArgument* argument = static_cast<ThreadRangerArgument*>(threadArgument);
	World* world = argument->world;
	unsigned int x0 = argument->x0;
	unsigned int x1 = argument->x1;
	unsigned int y0 = argument->y0;
	unsigned int y1 = argument->y1;
	unsigned int z0 = argument->z0;
	unsigned int z1 = argument->z1;
	world->growFungiTask(x0, x1, y0, y1, z0, z1);
	return NULL;
}

void* World::growFungiTask(unsigned int x0, unsigned int x1, unsigned int y0, unsigned int y1, unsigned int z0, unsigned int z1)
{
	TerrainCell* currentTerrainCell = NULL;
	for (unsigned int z = z0; z < z1; z++)
	{
		for (unsigned int y = y0; y < y1; y++)
		{
			for (unsigned int x = x0; x < x1; x++)
			{
				currentTerrainCell = terrain[z][y][x];
				if(!currentTerrainCell->isObstacle())
				{
					currentTerrainCell->growFungi();
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
				currentTerrainCell = terrain[z][y][x];
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
				currentTerrainCell = terrain[z][y][x];
				if(!currentTerrainCell->isObstacle())
				{
					//currentTerrainCell->printAnimalsVoracities(voracitiesFile);
					currentTerrainCell->diePredatorsFromBackground(day);
					currentTerrainCell->assimilateFoodMass();
					currentTerrainCell->growAnimals(day);
					currentTerrainCell->breedAnimals(day, timeStepsPerDay, outputDirectory);
				}
			}
		}
	}
	return NULL;
}

#endif //_PTHREAD
