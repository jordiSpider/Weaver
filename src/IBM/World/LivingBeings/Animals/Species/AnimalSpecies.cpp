/*
 * AnimalSpecies.cpp
 *
 *  Created on: Jul 6, 2013
 *      Author: jrbcast
 */

#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"

#include "IBM/World/Map/SpatialTree.h"
#include "IBM/World/Map/TerrainCells/EdibleSearchParams.h"
#include "IBM/World/Map/TerrainCells/AnimalSearchParams.h"
#include "IBM/World/World.h"


using namespace std;
using json = nlohmann::json;


const std::vector<AnimalSpecies::Gender::GenderValue> AnimalSpecies::Gender::createEnumValues()
{
	std::vector<GenderValue> values;

	for(const auto &value : magic_enum::enum_values<GenderValue>())
	{
		values.push_back(value);
	}

	return values;
}

const vector<AnimalSpecies::Gender::GenderValue> AnimalSpecies::Gender::enumValues = AnimalSpecies::Gender::createEnumValues();


SexualType::SexualType(const string& typeStr) : value(stringToEnumValue(typeStr)) {};

const unordered_map<string_view, const SexualType::SexualTypeValue> SexualType::generateMap() 
{
	unordered_map<string_view, const SexualType::SexualTypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const SexualTypeValue sexualType = static_cast<const SexualTypeValue>(i);
		enumMap.insert({to_string(sexualType), sexualType});
	}

	return enumMap;
}

const unordered_map<string_view, const SexualType::SexualTypeValue> SexualType::stringToEnum = SexualType::generateMap();

const string SexualType::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<SexualTypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string SexualType::enumValues = SexualType::generateAvailableValues();

template <class Archive>
void SexualType::serialize(Archive &ar, const unsigned int version) {
	ar & value; 
	ar & enumValues;
	ar & stringToEnum;
}


HuntingMode::HuntingMode(const string& typeStr) : value(stringToEnumValue(typeStr)) {};

const unordered_map<string_view, const HuntingMode::HuntingModeValue> HuntingMode::generateMap() 
{
	unordered_map<string_view, const HuntingMode::HuntingModeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const HuntingModeValue huntingMode = static_cast<const HuntingModeValue>(i);
		enumMap.insert({to_string(huntingMode), huntingMode});
	}

	return enumMap;
}

const unordered_map<string_view, const HuntingMode::HuntingModeValue> HuntingMode::stringToEnum = HuntingMode::generateMap();

const string HuntingMode::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<HuntingModeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string HuntingMode::enumValues = HuntingMode::generateAvailableValues();

template <class Archive>
void HuntingMode::serialize(Archive &ar, const unsigned int version) {
	ar & value; 
	ar & enumValues;
	ar & stringToEnum;
}


unsigned int AnimalSpecies::animalSpeciesCounter = 0;

const id_type& AnimalSpecies::getAnimalSpeciesCounter()
{
	return animalSpeciesCounter;
}


AnimalSpecies::AnimalSpecies(const json &info, bool initIndividualsPerDensities, World* const world) 
	: Species(info["name"], info["conversionToWetMass"], info["individualsPerInstar"].size(), world), 
	  animalSpeciesId(animalSpeciesCounter++), 
	  defaultHuntingMode(info["defaultHuntingMode"]), 
	  edibleSpecies(getNumberOfInstars(), vector<unordered_map<Species*, vector<Instar>>>(Species::Type::size())),
	  edibleSearchParams(getNumberOfInstars()), 
	  breedSearchParams(Gender::size()), 
	  cellEvaluationSearchParams(getNumberOfInstars()), matureFemalesSearchParams(make_unique<EdibleSearchParams>()), 
	  populationSearchParams(make_unique<AnimalSearchParams>()), lifeStageSearchParams(LifeStage::size()), 
	  traits(EnumClass<Trait::Type>::size(), nullptr), numberOfLociPerTrait(info["genetics"]["numberOfLociPerTrait"]),
	  numberOfAllelesPerLocus(info["genetics"]["numberOfAllelesPerLocus"]), 
	  traitsPerModule(info["genetics"]["modules"]["traitsPerModule"]),
	  numberOfChiasmasPerChromosome(info["genetics"]["numberOfChiasmasPerChromosome"]),
	  rhoPerModule(info["genetics"]["modules"]["correlationCoefficientRhoPerModule"]),
	  tempFromLab(info["tempFromLab"]), maximumDryMassObserved(0.0),
	  sexualType(info["sexualType"]), involvedResourceSpecies(getNumberOfInstars()), 
	  cellSizePerInstar(static_cast<vector<double>>(info["cellSizePerInstar"])), 
	  forcePresenceAllResourcesInvolved(info["forcePresenceAllResourcesInvolved"]),
	  preserveLeftovers(info["preserveLeftovers"]), newAAdult(info["newAAdult"]), newB(info["newB"])
	  
{
	this->rhoRangePerModule.reserve(rhoPerModule.size());
	for(size_t i = 0; i < rhoPerModule.size(); ++i)
	{
		this->rhoRangePerModule.push_back(round((1.0 - fabs(rhoPerModule[i])) * numberOfLociPerTrait));
	}

	setTraits(info["genetics"]["traits"]);

	// setHunterInteractionRanges();

	if (numberOfChiasmasPerChromosome % 2 != 0 || numberOfChiasmasPerChromosome < 0)
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "numberOfChiasmasPerChromosome must be multiple of 2. You entered " << numberOfChiasmasPerChromosome << " = " << numberOfChiasmasPerChromosome << ". Exiting now" << std::endl;
		exit(-1);
	}


	maxEncountersPerDay = 0;


	//Constants for interactions
	minSizeHunted = 10000000.000;
	maxSizeHunted = 0;
	minVorHunted = 10000000.000;
	maxVorHunted = 0;

	minSizeHunter = 10000000.000;
	maxSizeHunter = 0.0;
	minVorHunter = 10000000.000;
	maxVorHunter = 0.0;
	minSearchAreaHunter = 10000000.000;
	maxSearchAreaHunter = 0.0;

	minProbabilityDensityFunction = 10000000.000;
	maxProbabilityDensityFunction = 0.0;
	minVorXVor = 0.0;
	maxVorXVor = 0.0;
	minSpeedRatio = 0.0;
	maxSpeedRatio = 0.0;

	maxPredationProbability = 0.0;
	maxPredationIndividualWetMass = 0.0;

	setInstarDevTimeVector(static_cast<vector<double>>(info["devTimeVector"]));

	//Added for new growth_curves
	if(initIndividualsPerDensities == false)
	{
		setInitialPopulation(info["individualsPerInstar"]);
	}
	setStatisticsIndividualsPerInstar(info["statisticsIndividualsPerInstar"]);

	setAssignedForMolt(info["assignedForMolt"]);
	setBetaScaleTank(info["betaScaleTank"]);
	setExcessInvestInSize(info["excessInvestInSize"]);
	setHabitatShiftBeforeBreeding(info["habitatShiftBeforeBreeding"], info["habitatShiftBeforeBreedingFactor"]);
	setHabitatShiftAfterBreeding(info["habitatShiftAfterBreeding"], info["habitatShiftAfterBreedingFactor"]);
	setHabitatShift(info["habitatShift"], info["habitatShiftFactor"]);
	setMinRelativeHumidityThreshold(info["minRelativeHumidityThreshold"]);
	setMaxEncountersT(info["maxEncountersT"]);

	//Added for new growth_curves
	setLongevitySinceMaturation(info["longevitySinceMaturation"]);
	setReproTimeFactor(info["reproTimeFactor"]);
	setDevInter(info["devInter"]);
	setFractSearchExtremeT(info["fractSearchExtremeT"]);
	setFractSpeedExtremeT(info["fractSpeedExtremeT"]);

	setExperienceInfluencePerDay(info["experienceInfluencePerDay"]);

	setMassInfo(info["eggClutchFromEquation"], info["forClutchMassCoefficient"], info["forClutchMassScale"]);

	setScaleForVoracity(info["scaleForVoracity"]);
	setScaleForSearchArea(info["scaleForSearchArea"]);
	setScaleForSpeed(info["scaleForSpeed"]);
	setMaxPlasticityKVonBertalanffy(info["maxPlasticityKVonBertalanffy"]);
	setMinPlasticityKVonBertalanffy(info["minPlasticityKVonBertalanffy"]);
	setPlasticityDueToConditionVor(info["plasticityDueToConditionVor"]);
	setPlasticityDueToConditionSearch(info["plasticityDueToConditionSearch"]);
	setPlasticityDueToConditionSpeed(info["plasticityDueToConditionSpeed"]);

	setAttackProbability(info["attackProbability"]);
	setExposedAttackProbability(info["exposedAttackProbability"]);
	setKillProbability(info["killProbability"]);
	setOptimalTemperatureModifier(info["optimalTemperatureModifier"]);

	setDaysWithoutFoodForMetabolicDownregulation(info["daysWithoutFoodForMetabolicDownregulation"]);
	setPercentageMetabolicDownregulation(info["percentageMetabolicDownregulation"]);
	setPercentageCostForMetabolicDownregulationVoracity(info["percentageCostForMetabolicDownregulationVoracity"]);
	setPercentageCostForMetabolicDownregulationSearchArea(info["percentageCostForMetabolicDownregulationSearchArea"]);
	setPercentageCostForMetabolicDownregulationSpeed(info["percentageCostForMetabolicDownregulationSpeed"]);

	setCellEvaluationBiomass(info["cellEvaluationBiomass"]);
	setCellEvaluationRisk(info["cellEvaluationRisk"]);
	setCellEvaluationProConspecific(info["cellEvaluationProConspecific"]);
	setCellEvaluationAntiConspecific(info["cellEvaluationAntiConspecific"]);
	setConspecificWeighing(info["conspecificWeighing"]);

	setCoefficientForMassA(info["coefficientForMassA"]);
	setScaleForMassB(info["scaleForMassB"]);

	setCoefficientForMassAforMature(info["coefficientForMassAforMature"]);
	setScaleForMassBforMature(info["scaleForMassBforMature"]);

	setForDensitiesA(info["forDensitiesA"]);
	setForDensitiesB(info["forDensitiesB"]);

	setInstarsForNextReproduction(info["instarsForNextReproduction"]);

	setSexRatio(info["sexRatio"]);
	setSize(info["size"]);
	setFemaleMaxReproductionEvents(info["femaleMaxReproductionEvents"]);
	setEggsPerBatch(info["eggsPerBatch"]);
	setMaleMaxReproductionEvents(info["maleMaxReproductionEvents"]);
	setMaleMobility(info["maleMobility"]);
	setSurviveWithoutFood(info["debug"]["surviveWithoutFood"]);
	setActivityUnderPredationRisk(info["activityUnderPredationRisk"]);
	setMaleReproductionFactor(info["maleReproductionFactor"]);
	setProbabilityDeathFromBackground(info["probabilityDeathFromBackground"]);

	setCapitalBreeding(info["capitalBreeding"]);


	if(getNumberOfIndividualLevelTraits()/getTraitsPerModule() + (getNumberOfIndividualLevelTraits()%getTraitsPerModule() == 0) ? 0 : 1)
	{
		throwLineInfoException("Error: Item 'correlationCoefficientRhoPerModule' must have a size equal to the value of the arithmetic expression 'numberOfIndividualLevelTraits/traitsPerModule + (numberOfIndividualLevelTraits%traitsPerModule==0?0:1)'.");
	}

	totFec = getFemaleMaxReproductionEvents() * getEggsPerBatch();

	growthModule = new GrowthModule(this, info["growthModule"]);
}

AnimalSpecies::~AnimalSpecies()
{
	for(auto& trait : traits)
	{	
		delete trait;
	}

	for(auto elem : loci) {
		delete elem;
	}
	loci.clear();

	delete growthModule;
}

const id_type& AnimalSpecies::getAnimalSpeciesId() const 
{ 
	return animalSpeciesId; 
}

void AnimalSpecies::initEdibleOntogeneticLink(const vector<const Species*>& existingSpecies)
{
	edibleOntogeneticLink.resize(existingSpecies.size());

	for(unsigned int i = 0; i < existingSpecies.size(); i++)
	{
		edibleOntogeneticLink[i].resize(getNumberOfInstars());

		for(vector<OntogeneticLink> &predatorInstar : edibleOntogeneticLink[i])
		{
			predatorInstar.resize(existingSpecies[i]->getNumberOfInstars());
		}
	}
}

const OntogeneticLink& AnimalSpecies::getEdibleOntogeneticLink(const id_type &preySpeciesId, const Instar &predator, const Instar &prey) const
{
	return edibleOntogeneticLink.at(preySpeciesId).at(predator.getValue()).at(prey.getValue());
}

void AnimalSpecies::setEdibleOntogeneticLink(const id_type &preySpeciesId, const Instar &predator, const Instar &prey, const json &linkInfo)
{
	edibleOntogeneticLink[preySpeciesId][predator.getValue()][prey.getValue()].setInfo(linkInfo);
}

void AnimalSpecies::checkOntogeneticLinksPreference() const
{
	for(const Instar &predatorInstar : getInstarsRange())
	{
		double instarPreference = 0.0;

		for(const auto &preySpeciesType : getInstarEdibleSpecies(predatorInstar))
		{
			for(const auto &[preySpecies, preySpeciesInstars] : preySpeciesType)
			{
				for(const Instar &preyInstar : preySpeciesInstars)
				{
					instarPreference += getEdiblePreference(preySpecies->getId(), predatorInstar, preyInstar);
				}
			}
		}
		

		if(instarPreference > 1.0)
		{
			throwLineInfoException("The total preference of instar " + to_string(predatorInstar.getValue() + 1) + " of '" + getScientificName() + "' animal species must less or equal to 1");
		}
	}
}

void AnimalSpecies::addEdibleOntogeneticLink(Species::Type::TypeValue speciesType, Species* newSpecies, const json& ontogeneticLink)
{
	for(const auto &link : ontogeneticLink["links"])
	{
		vector<unsigned int> resourceSpeciesInstarVector = {newSpecies->getNumberOfInstars()};

		vector<unsigned int> predatorInstar = link["predatorInstar"];
		vector<Instar>(link["predatorInstar"].begin(), link["predatorInstar"].end());

		try
		{
			addEdibleLink(
				speciesType, newSpecies, 
				vector<Instar>(link["predatorInstar"].begin(), link["predatorInstar"].end()), 
				vector<Instar>(link.at("preyInstar").begin(), link.at("preyInstar").end()),
				link
			);
		}
		catch(const json::out_of_range& e)
		{
			addEdibleLink(
				speciesType, newSpecies, 
				vector<Instar>(link["predatorInstar"].begin(), link["predatorInstar"].end()), 
				vector<Instar>(resourceSpeciesInstarVector.begin(), resourceSpeciesInstarVector.end()),
				link
			);
		}
	}
}

void AnimalSpecies::addEdibleLink(Species::Type::TypeValue speciesType, Species* newSpecies, const vector<Instar> &predatorInstarVector, 
		const vector<Instar> &preyInstarVector, const json& linkValue)
{
	for(const Instar &predatorInstar : predatorInstarVector)
	{
		// Esto se puede sustituir por una condicción en los schemas
		if(predatorInstar > getNumberOfInstars())
		{
			throwLineInfoException("The instar value is not in the instar range of the predator species");
		}

		for(const Instar &preyInstar : preyInstarVector)
		{
			if(preyInstar > newSpecies->getNumberOfInstars())
			{
				throwLineInfoException("The instar value is not in the instar range of the prey species");
			}

			try
			{
				if(find(edibleSpecies[predatorInstar][speciesType].at(newSpecies).begin(), edibleSpecies[predatorInstar][speciesType].at(newSpecies).end(), preyInstar) != edibleSpecies[predatorInstar][speciesType].at(newSpecies).end())
				{
					throwLineInfoException("Trying to add an edible species twice, please contact developers");
				}

				edibleSpecies[predatorInstar][speciesType][newSpecies].push_back(preyInstar);
			}
			catch(const std::out_of_range& e)
			{
				edibleSpecies[predatorInstar][speciesType][newSpecies] = vector<Instar>();
				edibleSpecies[predatorInstar][speciesType][newSpecies].push_back(preyInstar);
			}

			setEdibleOntogeneticLink(newSpecies->getId(), predatorInstar, preyInstar, linkValue);
		}
	}
}

void AnimalSpecies::calculateCellDepthPerInstar(const Map* const map)
{
	cellDepthPerInstar.reserve(cellSizePerInstar.size());
	
	for(const double &cellSize : cellSizePerInstar)
	{
		unsigned int actualDepth = static_cast<const SpatialTree* const>(map)->getMapDepth()-1;

		while(cellSize > static_cast<const SpatialTree* const>(map)->getCellSize(actualDepth))
		{
			actualDepth--;
		}

		cellDepthPerInstar.push_back(actualDepth);
	}
}

const InstarVector<unsigned int>& AnimalSpecies::getCellDepthPerInstar() const
{
	return cellDepthPerInstar;
}

const unsigned int AnimalSpecies::getInstarCellDepth(const Instar &instar) const
{
	return cellDepthPerInstar[instar];
}

double AnimalSpecies::getEdiblePreference(const id_type &preySpeciesId, const Instar &predator) const
{
	double preference = 0.0;

	for(const auto &elem : edibleOntogeneticLink[preySpeciesId][predator.getValue()])
	{
		preference += elem.getPreference();
	}

	return preference;
}

const double& AnimalSpecies::getEdiblePreference(const id_type &preySpeciesId, const Instar &predator, const Instar &prey) const
{
	return getEdibleOntogeneticLink(preySpeciesId, predator, prey).getPreference();
}

const double& AnimalSpecies::getEdibleProfitability(const id_type &preySpeciesId, const Instar &predator, const Instar &prey) const
{
	return getEdibleOntogeneticLink(preySpeciesId, predator, prey).getProfitability();
}

bool AnimalSpecies::canEatEdible(const id_type &preySpeciesId, const Instar &predator, const Instar &prey) const
{
	return getEdibleOntogeneticLink(preySpeciesId, predator, prey).getEdible();
}

const double& AnimalSpecies::getMaximumDryMassObserved() const
{
	return maximumDryMassObserved;
}

void AnimalSpecies::setNumberOfLinks(const unsigned int numberOfLinksAsPredator, const unsigned int numberOfLinksAsPrey)
{
	this->numberOfLinksAsPredator = numberOfLinksAsPredator;
	this->numberOfLinksAsPrey = numberOfLinksAsPrey;
}

const unsigned int AnimalSpecies::getNumberOfLinksAsPredator() const
{
	return numberOfLinksAsPredator;
}

const unsigned int AnimalSpecies::getNumberOfLinksAsPrey() const
{
	return numberOfLinksAsPrey;
}

void AnimalSpecies::calculateK_Density(vector<InstarVector<bool>> &checkedAnimalSpecies, 
		vector<InstarVector<bool>> &cannibalismAnimalSpecies)
{
	for(const Instar& instar : getInstarsRange())
	{
		calculateInstarK_Density(instar, checkedAnimalSpecies, cannibalismAnimalSpecies);
	}
}

double AnimalSpecies::calculateInstarK_Density(const Instar &instar, vector<InstarVector<bool>> &checkedAnimalSpecies, 
		vector<InstarVector<bool>> &cannibalismAnimalSpecies)
{
	if(!isCalculatedInstarK_Density(instar))
	{
		if(checkedAnimalSpecies[getAnimalSpeciesId()][instar])
		{
			cannibalismAnimalSpecies[getAnimalSpeciesId()][instar] = true;
			return 0.0;
		}

		checkedAnimalSpecies[getAnimalSpeciesId()][instar] = true;


		double K_Density = 0.0;

		// Calculate the K of animal species without taking into account cannibalism.
		for(const auto &speciesType : getInstarEdibleSpecies(instar))
		{
			for(const auto &[species, speciesInstars] : speciesType)
			{
				for(const Instar &targetInstar : speciesInstars)
				{
					K_Density += species->calculateInstarK_Density(targetInstar, checkedAnimalSpecies, cannibalismAnimalSpecies);
				}
			}
		}

		// If cannibalism exists
		if(cannibalismAnimalSpecies[getAnimalSpeciesId()][instar]) {
			// Calculate K by adding cannibalism
			K_Density += K_Density * 0.1;
		}

		setInstarK_Density(instar, K_Density);
	}

	return getInstarK_Density(instar) * 0.1;
}

void AnimalSpecies::updateMaximumDryMassObserved(double newMaximumDryMass)
{
	maximumDryMassObserved = (maximumDryMassObserved < newMaximumDryMass) ? newMaximumDryMass : maximumDryMassObserved;
}

void AnimalSpecies::setInitialPredationEventsOnOtherSpecies(unsigned int numberOfSpecies)
{
	for (unsigned int i = 0; i < numberOfSpecies; ++i) {
		predationEventsOnOtherSpecies.push_back(0);
	}
}

const vector<int>& AnimalSpecies::getPredationEventsOnOtherSpecies() const
{
	return predationEventsOnOtherSpecies;
}

bool AnimalSpecies::eatsAnimalSpecies(const Instar &instar) const
{
	return !getInstarEdibleAnimalSpecies(instar).empty();
}

void AnimalSpecies::setHabitatShift(const std::vector<unsigned int> &habitatShift, const json &newHabitatShiftFactor)
{
	habitatShiftPerInstar.resize(getNumberOfInstars(), false);
	habitatShiftFactor = 0.0;

	if(!habitatShift.empty())
	{
		for(const unsigned int instarValue : habitatShift)
		{
			Instar instar(instarValue);
			habitatShiftPerInstar[instar.getValue()] = true;
		}

		habitatShiftFactor = newHabitatShiftFactor;
	}
}

const bool AnimalSpecies::habitatShiftOccurs(const Instar &instar) const
{
	return habitatShiftPerInstar[instar.getValue()];
}

const double& AnimalSpecies::getHabitatShiftBeforeBreedingFactor() const
{
	return habitatShiftBeforeBreedingFactor;
}

const double& AnimalSpecies::getHabitatShiftAfterBreedingFactor() const
{
	return habitatShiftAfterBreedingFactor;
}

const double& AnimalSpecies::getHabitatShiftFactor() const
{
	return habitatShiftFactor;
}

void AnimalSpecies::generateInvolvedResourceSpecies(const vector<const Species*> &existingSpecies)
{
	for(const Instar &instar : getInstarsRange())
	{
		vector<InstarVector<bool>> alreadyCheckedSpecies(existingSpecies.size());

		for(const auto &species : existingSpecies)
		{
			alreadyCheckedSpecies[species->getId()] = InstarVector<bool>(species->getNumberOfInstars(), false);
		}

		generateInstarInvolvedResourceSpecies(instar, involvedResourceSpecies[instar], alreadyCheckedSpecies);
		
		if(getInstarInvolvedResourceSpecies(instar).empty())
		{
			throwLineInfoException("Each instar must have at least one resource species involved");
		}
	}
}

void AnimalSpecies::generateInstarInvolvedResourceSpecies(const Instar &instar, vector<ResourceSpecies*> &instarInvolvedResourceSpecies, vector<InstarVector<bool>> &alreadyCheckedSpecies)
{
	auto isAlreadyCheckedSpecies = alreadyCheckedSpecies[getId()][instar];

	if(!isAlreadyCheckedSpecies)
	{
		alreadyCheckedSpecies[getId()][instar] = true;

		if(getInstarInvolvedResourceSpecies(instar).empty())
		{
			for(const auto &preySpeciesType : getInstarEdibleSpecies(instar))
			{
				for(const auto& [preySpecies, preyInstarsVector] : preySpeciesType)
				{
					for(const Instar &preyInstar : preyInstarsVector)
					{
						preySpecies->generateInstarInvolvedResourceSpecies(preyInstar, instarInvolvedResourceSpecies, alreadyCheckedSpecies);
					}
				}
			}
		}
		else
		{
			instarInvolvedResourceSpecies.insert(
				instarInvolvedResourceSpecies.end(), 
				getInstarInvolvedResourceSpecies(instar).begin(), getInstarInvolvedResourceSpecies(instar).end()
			);
		}
	}
} 

const EdibleSearchParams& AnimalSpecies::getEdibleSearchParams(const Instar &instar) const
{
	return edibleSearchParams[instar];
}

void AnimalSpecies::setHabitatShiftBeforeBreeding(const bool newHabitatShiftBeforeBreedingValue, const json &newHabitatShiftBeforeBreedingFactor)
{
	habitatShiftBeforeBreeding = newHabitatShiftBeforeBreedingValue;

	habitatShiftBeforeBreedingFactor = 0.0;

	if(habitatShiftBeforeBreeding)
	{
		habitatShiftBeforeBreedingFactor = newHabitatShiftBeforeBreedingFactor;
	}
}

void AnimalSpecies::setInitialPopulation(const vector<unsigned int>& initialPopulation)
{ 
	this->initialPopulation.reserve(initialPopulation.size());

	for(const unsigned int instarInitialPopulation : initialPopulation)
	{
		this->initialPopulation.push_back(instarInitialPopulation);
	}
}

void AnimalSpecies::setHabitatShiftAfterBreeding(const bool newHabitatShiftAfterBreedingValue, const json &newHabitatShiftAfterBreedingFactor)
{
	habitatShiftAfterBreeding = newHabitatShiftAfterBreedingValue;

	habitatShiftAfterBreedingFactor = 0.0;

	if(habitatShiftAfterBreeding)
	{
		habitatShiftAfterBreedingFactor = newHabitatShiftAfterBreedingFactor;
	}
}

const bool AnimalSpecies::occursHabitatShiftBeforeBreeding() const
{
	return habitatShiftBeforeBreeding;
}

const bool AnimalSpecies::occursHabitatShiftAfterBreeding() const
{
	return habitatShiftAfterBreeding;
}

void AnimalSpecies::obtainEdibleSearchParams(World* const world)
{
	for(const Instar &instar : getInstarsRange())
	{
		for(const AnimalSpecies* const &otherAnimalSpecies : world->getExistingAnimalSpecies())
		{
			for(const Instar &otherAnimalSpeciesInstar : otherAnimalSpecies->getInstarsRange())
			{
				vector<LifeStage::LifeStageValue> searchableLifeStages;

				// Check if the animal is prey for the species
				bool isSpeciePrey = otherAnimalSpecies->canEatEdible(getId(), otherAnimalSpeciesInstar, instar);
				// Check if the species is prey for the animal
				bool isSpeciePredator = canEatEdible(otherAnimalSpecies->getId(), instar, otherAnimalSpeciesInstar);
				
				if(isSpeciePrey)
				{
					// Identify interactions with active animals of the species
					searchableLifeStages = {LifeStage::ACTIVE};
				}
				else if(!isSpeciePrey && isSpeciePredator)
				{
					// Identify interactions with all animals of the species with which it can interact
					searchableLifeStages = {LifeStage::ACTIVE, LifeStage::SATIATED, LifeStage::HANDLING, LifeStage::DIAPAUSE};
				}
				else
				{
					// No interaction with an animal
					searchableLifeStages = {};
				}

				if(!searchableLifeStages.empty())
				{
					edibleSearchParams[instar].addAnimalSearchParams(
						world, searchableLifeStages, {otherAnimalSpecies->getAnimalSpeciesId()},
						{otherAnimalSpeciesInstar}
					);
				}
			}
		}

		for(const auto &[resourceSpecies, resourceSpeciesInstars] : getInstarEdibleResourceSpecies(instar))
		{
			edibleSearchParams[instar].addResourceSearchParams(
				{static_cast<ResourceSpecies* const>(resourceSpecies)->getResourceSpeciesId()}
			);
		}	
	}
}

const EdibleSearchParams& AnimalSpecies::getBreedSearchParams(const Gender::GenderValue &gender) const
{
	return breedSearchParams[gender];
}

void AnimalSpecies::obtainBreedSearchParams(World* const world)
{
	auto searchableLifeStages = {LifeStage::ACTIVE, LifeStage::SATIATED, LifeStage::HANDLING, LifeStage::DIAPAUSE};

	// Get all the animals that can be mated
	vector<Instar> searchableInstars(find(getInstarsRange().begin(), getInstarsRange().end(), getGrowthModule()->getInstarFirstReproduction()), getInstarsRange().end());

	vector<Gender::GenderValue> searchableGender;

	for(const auto &gender : Gender::getEnumValues())
	{
		switch(gender)
		{
			case AnimalSpecies::Gender::MALE:
				searchableGender = {AnimalSpecies::Gender::FEMALE};
				break;
			case AnimalSpecies::Gender::FEMALE:
				searchableGender = {AnimalSpecies::Gender::MALE};
				break;
			case AnimalSpecies::Gender::HERMAPHRODITE:
				searchableGender = {AnimalSpecies::Gender::HERMAPHRODITE};
				break;
			default:
				throwLineInfoException("Default case");
				break;
		}

		breedSearchParams[gender].addAnimalSearchParams(
			world, searchableLifeStages, {getAnimalSpeciesId()}, searchableInstars, searchableGender
		);
	}
}

void AnimalSpecies::setEggsPerBatch(const json& info)
{
	eggsPerBatchFromEquation = info["eggsPerBatchFromEquation"];

	if(eggsPerBatchFromEquation)
	{
		interceptForEggBatchFromEquation = info["equationParameters"]["interceptForEggBatchFromEquation"];
		slopeForEggBatchFromEquation = info["equationParameters"]["slopeForEggBatchFromEquation"];
	}
	else
	{
		eggsPerBatch = info["value"];
	}
}

const bool AnimalSpecies::getEggsPerBatchFromEquation() const
{
	return eggsPerBatchFromEquation;
}

const double& AnimalSpecies::getEggsPerBatch() const 
{ 
	return eggsPerBatch; 
}

const double& AnimalSpecies::getInterceptForEggBatchFromEquation() const
{
	return interceptForEggBatchFromEquation;
}

const double& AnimalSpecies::getSlopeForEggBatchFromEquation() const
{
	return slopeForEggBatchFromEquation;
}

double AnimalSpecies::calculateEggsPerBatch(const double& individualDryMass) const
{
	if(getEggsPerBatchFromEquation())
	{
		return getInterceptForEggBatchFromEquation() + getSlopeForEggBatchFromEquation()*convertToWetMass(individualDryMass);
	}
	else
	{
		return getEggsPerBatch();
	}
}

const EdibleSearchParams& AnimalSpecies::getCellEvaluationSearchParams(const Instar &instar) const
{
    return cellEvaluationSearchParams[instar];
}

void AnimalSpecies::obtainCellEvaluationSearchParams(World* const world)
{
	for(const Instar &instar : getInstarsRange())
	{
		for(const AnimalSpecies* const &otherAnimalSpecies : world->getExistingAnimalSpecies())
		{
			if(getAnimalSpeciesId() == otherAnimalSpecies->getAnimalSpeciesId())
			{
				cellEvaluationSearchParams[instar].addAnimalSearchParams(
					world, {LifeStage::ACTIVE}, {otherAnimalSpecies->getAnimalSpeciesId()}
				);
			}
			else
			{
				for(const Instar &otherAnimalSpeciesInstar : otherAnimalSpecies->getInstarsRange())
				{
					vector<LifeStage::LifeStageValue> searchableLifeStages;

					// Check if the animal is prey for the species
					bool isSpeciePrey = otherAnimalSpecies->canEatEdible(getId(), otherAnimalSpeciesInstar, instar);
					// Check if the species is prey for the animal
					bool isSpeciePredator = canEatEdible(otherAnimalSpecies->getId(), instar, otherAnimalSpeciesInstar);
					
					if(isSpeciePrey || isSpeciePredator)
					{
						cellEvaluationSearchParams[instar].addAnimalSearchParams(
							world, {LifeStage::ACTIVE}, {otherAnimalSpecies->getAnimalSpeciesId()},
							{otherAnimalSpeciesInstar}
						);
					}
				}
			}
		}

		for(const auto &[resourceSpecies, resourceSpeciesInstars] : getInstarEdibleResourceSpecies(instar))
		{
			cellEvaluationSearchParams[instar].addResourceSearchParams(
				{static_cast<ResourceSpecies* const>(resourceSpecies)->getResourceSpeciesId()}
			);
		}	
	}
}

const EdibleSearchParams& AnimalSpecies::getMatureFemalesSearchParams() const
{
	return *matureFemalesSearchParams;
}

void AnimalSpecies::obtainMatureFemalesSearchParams(World* const world)
{
	matureFemalesSearchParams->addAnimalSearchParams(
		world, {LifeStage::ACTIVE, LifeStage::SATIATED, LifeStage::HANDLING, LifeStage::DIAPAUSE},
		{getAnimalSpeciesId()}, {}, {AnimalSpecies::Gender::FEMALE}
	);
}

const AnimalSearchParams& AnimalSpecies::getPopulationSearchParams() const
{
	return *populationSearchParams;
}

void AnimalSpecies::obtainPopulationSearchParams(World* const world)
{
	populationSearchParams->addSearchParams(world, LifeStage::getEnumValues(), {getAnimalSpeciesId()});
}

const AnimalSearchParams& AnimalSpecies::getLifeStageSearchParams(const LifeStage::LifeStageValue &lifeStage) const
{
    return lifeStageSearchParams[lifeStage];
}

void AnimalSpecies::obtainLifeStageSearchParams(World* const world)
{
    for(const auto &lifeStage : LifeStage::getEnumValues())
    {
        lifeStageSearchParams[lifeStage].addSearchParams(world, {lifeStage}, {getAnimalSpeciesId()});
    }
}

const vector<ResourceSpecies*>& AnimalSpecies::getInstarInvolvedResourceSpecies(const Instar &instar) const
{
	return involvedResourceSpecies[instar];
}

const vector<unordered_map<Species*, vector<Instar>>>& AnimalSpecies::getInstarEdibleSpecies(const Instar &instar) const
{
	return edibleSpecies[instar];
}

const double AnimalSpecies::calculateDryMass(const double& length, const bool mature) const
{
	if(mature)
	{
		return convertLengthToDryMass(length, getCoefficientForMassAforMature(), getScaleForMassBforMature());
	}
	else
	{
		return convertLengthToDryMass(length, getCoefficientForMassA(), getScaleForMassB());
	}
}

const GrowthModule* const AnimalSpecies::getGrowthModule() const
{
	return growthModule;
}

GrowthModule* const AnimalSpecies::getMutableGrowthModule()
{
	return growthModule;
}

const double AnimalSpecies::calculateDryLength(const double& dryMass, const bool mature) const
{
	if(mature)
	{
		return convertDryMassToLength(dryMass, getCoefficientForMassAforMature(), getScaleForMassBforMature());
	}
	else
	{
		return convertDryMassToLength(dryMass, getCoefficientForMassA(), getScaleForMassB());
	}
}

const unordered_map<Species*, vector<Instar>>& AnimalSpecies::getInstarEdibleAnimalSpecies(const Instar &instar) const
{
	return edibleSpecies[instar][Species::Type::Animal];
}

const unordered_map<Species*, vector<Instar>>& AnimalSpecies::getInstarEdibleResourceSpecies(const Instar &instar) const
{
	return edibleSpecies[instar][Species::Type::Resource];
}

void AnimalSpecies::setAssignedForMolt(const double& assignedForMolt)
{
	if (assignedForMolt > 0)
	{
		this->assignedForMolt = assignedForMolt;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "assignedForMolt must be a positive value. You entered " << assignedForMolt << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMinRelativeHumidityThreshold(const double& minRelativeHumidityThreshold)
{
	if (minRelativeHumidityThreshold >= 0)
	{
		this->minRelativeHumidityThreshold = minRelativeHumidityThreshold;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "minRelativeHumidityThreshold must be 0 or a positive value. You entered " << minRelativeHumidityThreshold << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::calculateMinSizeHunted(double minSizeHunted)
{
	this->minSizeHunted = min(this->minSizeHunted, minSizeHunted);

}

void AnimalSpecies::calculateMaxSizeHunted(double maxSizeHunted)
{
	this->maxSizeHunted = max(this->maxSizeHunted, maxSizeHunted);

}

void AnimalSpecies::calculateMinVorHunted(double minVorHunted)
{
	this->minVorHunted = min(this->minVorHunted, minVorHunted);

}

void AnimalSpecies::calculateMaxVorHunted(double maxVorHunted)
{
	this->maxVorHunted = max(this->maxVorHunted, maxVorHunted);

}

void AnimalSpecies::calculateMinSizeHunter(double minSizeHunter)
{
	this->minSizeHunter = min(this->minSizeHunter, minSizeHunter);

}

void AnimalSpecies::calculateMaxSizeHunter(double maxSizeHunter)
{
	this->maxSizeHunter = max(this->maxSizeHunter, maxSizeHunter);

}

void AnimalSpecies::calculateMinVorHunter(double minVorHunter)
{
	this->minVorHunter = min(this->minVorHunter, minVorHunter);

}

void AnimalSpecies::calculateMaxVorHunter(double maxVorHunter)
{
	this->maxVorHunter = max(this->maxVorHunter, maxVorHunter);

}

void AnimalSpecies::calculateMinSearchAreaHunter(double minSearchAreaHunter)
{
	this->minSearchAreaHunter = min(this->minSearchAreaHunter, minSearchAreaHunter);

}

void AnimalSpecies::calculateMaxSearchAreaHunter(double maxSearchAreaHunter)
{
	this->maxSearchAreaHunter = max(this->maxSearchAreaHunter, maxSearchAreaHunter);

}

void AnimalSpecies::calculateMinProbabilityDensityFunction(double minProbabilityDensityFunction)
{
	this->minProbabilityDensityFunction = min(this->minProbabilityDensityFunction, minProbabilityDensityFunction);

}

void AnimalSpecies::calculateMaxProbabilityDensityFunction(double maxProbabilityDensityFunction)
{
	this->maxProbabilityDensityFunction = max(this->maxProbabilityDensityFunction, maxProbabilityDensityFunction);

}

void AnimalSpecies::calculateMinVorXVor(double minVorXVor)
{
	this->minVorXVor = min(this->minVorXVor, minVorXVor);

}

void AnimalSpecies::calculateMaxVorXVor(double maxVorXVor)
{
	this->maxVorXVor = max(this->maxVorXVor, maxVorXVor);

}


void AnimalSpecies::calculateMinSpeedRatio(double minSpeedRatio)
{
	this->minSpeedRatio = min(this->minSpeedRatio, minSpeedRatio);

}

void AnimalSpecies::calculateMaxSpeedRatio(double maxSpeedRatio)
{
	this->maxSpeedRatio = max(this->maxSpeedRatio, maxSpeedRatio);

}

void AnimalSpecies::setCapitalBreeding(const json &info)
{
	capitalBreeding = info["enabled"];

	if(capitalBreeding)
	{
		timeOfReproEventDuringCapitalBreeding = info["capitalBreedingParams"]["timeOfReproEventDuringCapitalBreeding"];
		numberOfCapitalBreeds = info["capitalBreedingParams"]["numberOfCapitalBreeds"];
	}
}

const bool AnimalSpecies::hasCapitalBreeding() const
{
	return capitalBreeding;
}

const double& AnimalSpecies::getNewAAdult() const
{
	return newAAdult;
}

const double& AnimalSpecies::getNewB() const
{
	return newB;
}

const double& AnimalSpecies::getH_Enhancement() const
{
	return h_enhancement;
}

const unsigned int AnimalSpecies::getTimeOfReproEventDuringCapitalBreeding() const
{
	return timeOfReproEventDuringCapitalBreeding;
}

const unsigned int AnimalSpecies::getNumberOfCapitalBreeds() const
{
	return numberOfCapitalBreeds;
}

void AnimalSpecies::setVonBertLinf(double vonBertLinf)
{
	this->vonBertLinf = vonBertLinf;
}

void AnimalSpecies::setVonBertKini(double vonBertKini)
{
	this->vonBertKini = vonBertKini;
}

void AnimalSpecies::setPlasticityDueToConditionVor(const double& plasticityDueToConditionVor)
{
	if (plasticityDueToConditionVor > 0)
	{
		this->plasticityDueToConditionVor = plasticityDueToConditionVor;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "plasticityDueToConditionVor must be a positive value. You entered " << plasticityDueToConditionVor << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setPlasticityDueToConditionSearch(const double& plasticityDueToConditionSearch)
{
	if (plasticityDueToConditionSearch > 0)
	{
		this->plasticityDueToConditionSearch = plasticityDueToConditionSearch;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "plasticityDueToConditionSearch must be a positive value. You entered " << plasticityDueToConditionSearch << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setPlasticityDueToConditionSpeed(const double& plasticityDueToConditionSpeed)
{
	if (plasticityDueToConditionSpeed > 0)
	{
		this->plasticityDueToConditionSpeed = plasticityDueToConditionSpeed;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "plasticityDueToConditionSpeed must be a positive value. You entered " << plasticityDueToConditionSpeed << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::updateMaxEncountersPerDay(const unsigned int newMaxEncountersPerDay)
{
	maxEncountersPerDay = max(maxEncountersPerDay, newMaxEncountersPerDay);
}

const unsigned int AnimalSpecies::getMaxEncountersPerDay() const
{
	return maxEncountersPerDay;
}

AnimalSpecies::Gender::GenderValue AnimalSpecies::getRandomGender() const
{
	AnimalSpecies::Gender::GenderValue newAnimalGender = AnimalSpecies::Gender::MALE;
	if(Random::randomUniform() < sexRatio || getSexualType() == SexualType::asexual)
	{
		newAnimalGender = AnimalSpecies::Gender::FEMALE;
	}
	return newAnimalGender;
}

void AnimalSpecies::setExperienceInfluencePerDay(const float& experienceInfluencePerDay)
{
	if (experienceInfluencePerDay >= 0.0 && experienceInfluencePerDay < 1.0)
	{
		this->experienceInfluencePerDay = experienceInfluencePerDay;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "experienceInfluencePerDay must be in the range [0.0 - 1.0). You entered " << experienceInfluencePerDay << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMassInfo(const bool& eggClutchFromEquation, const double& forClutchMassCoefficient,
					 			const double& forClutchMassScale)
{
	setEggClutchFromEquation(eggClutchFromEquation);
	setForClutchMassCoefficient(forClutchMassCoefficient);
	setForClutchMassScale(forClutchMassScale);
}

/*
void AnimalSpecies::initWetBiomassDensitiesPerAge(Temperature temperature, int timeStepsPerDay)
{
	//double tmax = 273 + tempFromLab;

	double preGrowth = getMinTraitRange(Trait::Type::growth) + ((getMaxTraitRange(Trait::Type::growth) - getMinTraitRange(Trait::Type::growth))/2);

	double thisAnimalTempSizeRuleConstant = tempSizeRuleConstant;
	double degreesDifference = fabs(temperature.getTemperatureCelsius() - tempFromLab.getTemperatureCelsius());
	
	if(temperature > tempFromLab)
	{
		thisAnimalTempSizeRuleConstant = (-thisAnimalTempSizeRuleConstant);
	}

	double LdistanceMin, LdistanceMax;
	switch(getGrowthCurve()->getType()) {
		case CurveType::VonBertalanffy: {
			LdistanceMin = static_cast<const VonBertalanffyCurve* const>(getGrowthCurve())->getLdistanceMin();
			LdistanceMax = static_cast<const VonBertalanffyCurve* const>(getGrowthCurve())->getLdistanceMax();
			break;
		}
		case CurveType::Logistic: {
			LdistanceMin = static_cast<const LogisticCurve* const>(getGrowthCurve())->getLdistanceMin();
			LdistanceMax = static_cast<const LogisticCurve* const>(getGrowthCurve())->getLdistanceMax();
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
	double Linf = LdistanceMin+((LdistanceMax - LdistanceMin)/2);

	//Here the TSR rule is improved by directly affecting asymptotic body mass, not length.
	 //Here to calculate the new dev time we need to calculate the mass of the adult after TSR has been applied
	double ageLastInstar = instarDevTimeVector.back();

	VonBertalanffyCurveParams params;
	params.setAsymptoticSize(Linf);
	params.setGrowthCoefficient(preGrowth);
	params.setTime(ageLastInstar);

	double maxLfromSpeciesLastInstar = getGrowthCurve()->getValue(params);

	double massLastInstarForDevT = getCoefficientForMassAforMature()*pow(maxLfromSpeciesLastInstar,getScaleForMassBforMature());

	double postTSRMassLastInstarForDevT = massLastInstarForDevT + thisAnimalTempSizeRuleConstant*degreesDifference*massLastInstarForDevT;

	//transform to g in wetMass to adjust to the equation by Gillooly et al. 2012
	double massLastInstarForDevTinG = getConversionToWetMass() * postTSRMassLastInstarForDevT * 1000;

	double devTime = exp(-0.11 * (temperature.getTemperatureCelsius() / (1 + temperature.getTemperatureCelsius() / 273)) + getDevInter()) * pow(massLastInstarForDevTinG, 0.25);

	double finalDevTimeProportion = devTime / ageLastInstar;

	//Fixing t0 in BV

	//Now we do anchor t0 regardless of temperature

	double finalMatureAge = ageLastInstar*finalDevTimeProportion;

	for(double currentAge = 0; currentAge <= finalMatureAge; currentAge += 1.0/timeStepsPerDay)
	{
		params.setTime(currentAge);

		double lengthAtCurrentAge = getGrowthCurve()->getValue(params);
		double massAtCurrentAge;
		if(currentAge<finalMatureAge){
		massAtCurrentAge = getCoefficientForMassA()*pow(lengthAtCurrentAge,getScaleForMassB());
		}else{
		massAtCurrentAge = getCoefficientForMassAforMature()*pow(lengthAtCurrentAge,getScaleForMassBforMature());
		}

		//TSR is applied directly here so the k value is effectively changed at the individual level
		massAtCurrentAge = massAtCurrentAge + massAtCurrentAge*thisAnimalTempSizeRuleConstant*degreesDifference;


		double wetMassAtCurrentAge = massAtCurrentAge*getConversionToWetMass();
		double density = forDensitiesA*pow(wetMassAtCurrentAge,forDensitiesB);
		density = wetMassAtCurrentAge * density;
		initialPopulation.push_back(density);
	}
}
*/

void AnimalSpecies::scaleInitialPopulation(double totalPopulationDensity, double initialEcosystemSize)
{
	for(double &elem : initialPopulation)
	{
		elem = (elem/totalPopulationDensity)*initialEcosystemSize;
	}
}

unsigned int AnimalSpecies::getTotalInitialPopulation() const
{
	unsigned int totalInitialPopulation = 0;

	for(const unsigned int instarPopulation : initialPopulation)
	{
		totalInitialPopulation += instarPopulation;
	}

	return totalInitialPopulation;
}

unsigned int AnimalSpecies::getTotalStatisticsInitialPopulation()
{
	return getStatisticsIndividualsPerInstar() * getNumberOfInstars();
}

const double& AnimalSpecies::getBetaScaleTank() const 
{ 
	return betaScaleTank; 
}

std::pair<double, double> AnimalSpecies::decomposeMassElements(const double &dryMass, const double &energyTankTraitValue) const
{
	double energyTank = energyTankTraitValue * pow(dryMass, getBetaScaleTank());
	double bodySize = dryMass - energyTank;

	return make_pair<>(energyTank, bodySize);
}

std::pair<double, double> AnimalSpecies::decomposeMassElements(const double &dryMass, const double& investment, const double &energyTankTraitValue) const
{
	double energyTank = energyTankTraitValue * pow(dryMass, getBetaScaleTank());
	double bodySize = dryMass - energyTank;

	double excessInvestment = investment-dryMass;

	double energyTankWithExcess = energyTank + excessInvestment*(1-getExcessInvestInSize());
	double bodySizeWithExcess = bodySize + excessInvestment*getExcessInvestInSize();

	return make_pair<>(energyTankWithExcess, bodySizeWithExcess);
}

// std::pair<double, double> AnimalSpecies::calculateHunterSizeRanges(const double& minDryMassHunter, const double& maxDryMassHunter) const
// {
// 	double tmpEnergyTank, minCurrentBodySizeHunter, maxCurrentBodySizeHunter;

// 	tie(tmpEnergyTank, minCurrentBodySizeHunter) = decomposeMassElements(
// 		minDryMassHunter, 
// 		getTrait(Trait::Type::energy_tank).getElements().at(TraitDefinitionSection::Elements::TraitValue)->getMaxRestrictedRange()
// 	);

// 	tie(tmpEnergyTank, maxCurrentBodySizeHunter) = decomposeMassElements(
// 		maxDryMassHunter, 
// 		getTrait(Trait::Type::energy_tank).getElements().at(TraitDefinitionSection::Elements::TraitValue)->getMinRestrictedRange()
// 	);

// 	return make_pair<>(minCurrentBodySizeHunter, maxCurrentBodySizeHunter);
// }

// void AnimalSpecies::setHunterInteractionRanges()
// {
	
// }

void AnimalSpecies::interactionRanges(AnimalInterface& predator, AnimalInterface& prey, double muForPDF, double sigmaForPDF)
{
	calculateMinSizeHunted(prey.getAnimalGrowth()->getCurrentBodySize());
	calculateMaxSizeHunted(prey.getAnimalGrowth()->getCurrentBodySize());
	
    calculateMinVorHunted(prey.getVoracity());
	calculateMaxVorHunted(prey.getVoracity());

	calculateMinSizeHunter(predator.getAnimalGrowth()->getCurrentBodySize());
	calculateMaxSizeHunter(predator.getAnimalGrowth()->getCurrentBodySize());
	
    calculateMinVorHunter(predator.getVoracity());
	calculateMaxVorHunter(predator.getVoracity());

	double probabilityDensityFunction = exp(-0.5 * pow((log(predator.calculateDryMass()/prey.calculateDryMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
	calculateMinProbabilityDensityFunction(probabilityDensityFunction);
	calculateMaxProbabilityDensityFunction(probabilityDensityFunction);

	calculateMinVorXVor(predator.getVoracity()*prey.getVoracity());
	calculateMaxVorXVor(predator.getVoracity()*prey.getVoracity());



	if(prey.getSpeed()<0.001){

		calculateMinSpeedRatio(predator.getSpeed()/prey.getSpeed());
		calculateMaxSpeedRatio(predator.getSpeed()/prey.getSpeed());

	}else{

		calculateMinSpeedRatio(predator.getSpeed()/prey.getSpeed());
		calculateMaxSpeedRatio(predator.getSpeed()/prey.getSpeed());

	}
}

const double& AnimalSpecies::getTotFec() const
{
	return totFec;
}

double AnimalSpecies::getMaxPredationProbability()
{
	return maxPredationProbability;
}

void AnimalSpecies::setMaxPredationProbability(double maxPredationProbability)
{
	this->maxPredationProbability = maxPredationProbability;
}

double AnimalSpecies::getMaxPredationIndividualWetMass()
{
	return maxPredationIndividualWetMass;
}

void AnimalSpecies::setMaxPredationIndividualWetMass(double maxPredationIndividualWetMass)
{
	this->maxPredationIndividualWetMass = maxPredationIndividualWetMass;
}

const std::vector<std::pair<Trait::Type, TraitDefinitionSection::Elements>>& AnimalSpecies::getIndividualLevelTraitElements() const
{
	return individualLevelTraitElements;
}

bool AnimalSpecies::isInsideRestrictedRanges(const CustomIndexedVector<Trait::Type, CustomIndexedVector<TraitDefinitionSection::Elements, double>>& baseTraitElementVector) const
{
	bool isInsideRestrictedRanges = true;

	Trait::Type traitType;
	TraitDefinitionSection::Elements elementType;

	for(unsigned int i = 0; i < getIndividualLevelTraitElements().size() && isInsideRestrictedRanges; i++)
	{
		tie(traitType, elementType) = getIndividualLevelTraitElements().at(i);

		isInsideRestrictedRanges = isInsideRestrictedRanges && static_cast<IndividualLevelTrait* const>(getTrait(traitType).getElements().at(elementType))->isInsideRestrictedRanges(baseTraitElementVector[traitType][elementType]);
	}

	return isInsideRestrictedRanges;
}

bool AnimalSpecies::checkInsideRestrictedRange() const
{
	const unsigned int numberOfTestIndividuals = 1000;

	unsigned int numberOfIndividualsInsideRestrictedRange = 0;

	CustomIndexedVector<Trait::Type, CustomIndexedVector<TraitDefinitionSection::Elements, double>> baseTraitElementVector(EnumClass<Trait::Type>::size());

	for(const Trait::Type& traitType : EnumClass<Trait::Type>::getEnumValues())
	{
		baseTraitElementVector[traitType].resize(getTrait(traitType).getElements().size());
	}


	for(unsigned int i = 0; i < numberOfTestIndividuals; i++)
	{
		Genome genome(getLoci(), getRandomlyCreatedPositionsForChromosomes(), getNumberOfChromosomes(), 
			getNumberOfLociPerChromosome(), getNumberOfChiasmasPerChromosome()
		);

		for(const Trait::Type& traitType : EnumClass<Trait::Type>::getEnumValues())
		{
			for(unsigned int i = 0; i < getTrait(traitType).getElements().size(); i++)
			{
				TraitDefinitionSection::Elements traitElement = static_cast<TraitDefinitionSection::Elements>(i);

				baseTraitElementVector[traitType][traitElement] = getTrait(traitType).getElements().at(traitElement)->getValue(
					genome, getTraitsPerModule(), getNumberOfLociPerTrait(), getRhoPerModuleVector(), getRhoRangePerModuleVector()
				);
			}
		}

		if(isInsideRestrictedRanges(baseTraitElementVector))
		{
			numberOfIndividualsInsideRestrictedRange++;
		}
	}


	return (static_cast<double>(numberOfIndividualsInsideRestrictedRange)/static_cast<const double>(numberOfTestIndividuals) >= 0.02);
}

void AnimalSpecies::setTraits(const unordered_map<string,json>& traitsInfo)
{
	loci.reserve(getNumberOfLociPerTrait());
	for (unsigned int i = 0; i < getNumberOfLociPerTrait(); ++i)
	{
		loci.push_back(new Locus(getNumberOfAllelesPerLocus()));
	}



	individualLevelTraitElements.reserve(traitsInfo.at("individualLevelTraitsOrder").size());

	for(const auto& [traitType, info] : traitsInfo.at("definition").items()) {
		Trait* newTrait = new Trait(traitType, info, traitsInfo.at("individualLevelTraitsOrder"), getTempFromLab(), getLoci(), getTraitsPerModule(), getNumberOfLociPerTrait(), getRhoPerModuleVector(), getRhoRangePerModuleVector());

		traits[newTrait->getType()] = newTrait;

		for(unsigned int element = static_cast<unsigned int>(TraitDefinitionSection::Elements::TraitValue); element < traits[newTrait->getType()]->getElements().size(); element++)
		{
			if(traits[newTrait->getType()]->getElements().at(static_cast<const TraitDefinitionSection::Elements>(element))->getType() == TraitDefinitionSection::Type::IndividualLevel)
			{
				individualLevelTraitElements.emplace_back(traits[newTrait->getType()]->getType(), static_cast<TraitDefinitionSection::Elements>(element));
			}
		}

		if(traits[newTrait->getType()]->isThermallyDependent())
		{
			temperatureDependentTraits.push_back(traits[newTrait->getType()]->getType());
		}
	}


	numberOfChromosomes = getNumberOfIndividualLevelTraits();


	randomlyCreatedPositionsForChromosomes.reserve(getNumberOfIndividualLevelTraits()*getNumberOfLociPerTrait());
	for(unsigned int i = 0; i < getNumberOfIndividualLevelTraits()*getNumberOfLociPerTrait(); ++i)
	{
		randomlyCreatedPositionsForChromosomes.emplace_back(i);
	}
	random_shuffle(randomlyCreatedPositionsForChromosomes.begin(), randomlyCreatedPositionsForChromosomes.end());


	//TODO Hay alguna restricción más para el parametrizador? Cual es el máximo de numberOfChromosomes??
	if ((getNumberOfIndividualLevelTraits()*numberOfLociPerTrait) % numberOfChromosomes == 0)
	{
		this->numberOfLociPerChromosome = (getNumberOfIndividualLevelTraits()*numberOfLociPerTrait) / numberOfChromosomes;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "numberOfTraits*numberOfLoci must be multiple of numberOfChromosomes. You entered " << getNumberOfIndividualLevelTraits() << "*" << numberOfLociPerTrait << " = " << getNumberOfIndividualLevelTraits()*numberOfLociPerTrait << " --> " << getNumberOfIndividualLevelTraits()*numberOfLociPerTrait << " % " << numberOfChromosomes << " = " << ((getNumberOfIndividualLevelTraits()*numberOfLociPerTrait) % numberOfChromosomes) << ". Exiting now" << std::endl;
		exit(-1);
	}



	while(!checkInsideRestrictedRange())
	{
		for(auto& elem : loci)
		{
			delete elem;
		}


		for (unsigned int i = 0; i < getNumberOfLociPerTrait(); ++i)
		{
			loci[i] = new Locus(getNumberOfAllelesPerLocus());
		}


		
		for(auto& [traitType, traitElement] : getIndividualLevelTraitElements())
		{
			static_cast<IndividualLevelTrait*>(getTrait(traitType).getElements().at(traitElement))->setPseudoValueLimits(
				getLoci(), getTraitsPerModule(), getNumberOfLociPerTrait(), getRhoPerModuleVector(), 
				getRhoRangePerModuleVector()
			);
		}
	}
}

const std::vector<Trait::Type>& AnimalSpecies::getTemperatureDependentTraits() const
{
	return temperatureDependentTraits;
}

const Trait& AnimalSpecies::getTrait(const Trait::Type name) const
{
	return *traits[name];
}

Trait& AnimalSpecies::getMutableTrait(const Trait::Type name)
{
	return *traits[name];
}

template <class Archive>
void AnimalSpecies::Gender::serialize(Archive &ar, const unsigned int version) {
	ar & enumValues;
}

template <class Archive>
void AnimalSpecies::serialize(Archive &ar, const unsigned int version) {
    ar & animalSpeciesCounter;

	ar & animalSpeciesId;
	ar & extinguished;
	ar & defaultHuntingMode;
	ar & edibleSpecies;
	ar & edibleOntogeneticLink;
	ar & edibleSearchParams;
	ar & breedSearchParams;
	ar & cellEvaluationSearchParams;
	ar & matureFemalesSearchParams;
	ar & populationSearchParams;
	ar & lifeStageSearchParams;
    ar & experienceInfluencePerDay;
	ar & predationEventsOnOtherSpecies;
    ar & traits;

	ar & numberOfLinksAsPredator;
	ar & numberOfLinksAsPrey;

	ar & initialPopulation;
	ar & statisticsIndividualsPerInstar;

	ar & numberOfLociPerTrait;
	ar & numberOfAllelesPerLocus;
	ar & traitsPerModule;

	ar & numberOfChromosomes;
	ar & numberOfChiasmasPerChromosome;
	ar & numberOfLociPerChromosome;

	ar & rhoPerModule;
	ar & rhoRangePerModule;
	ar & weightPerLocus;

	ar & assignedForMolt;
	ar & betaScaleTank;
	ar & excessInvestInSize;
	ar & minRelativeHumidityThreshold;

	ar & maxEncountersT;

	ar & minSizeHunted;
	ar & maxSizeHunted;
	ar & minVorHunted;
	ar & maxVorHunted;

	ar & minSizeHunter;
	ar & maxSizeHunter;
	ar & minVorHunter;
	ar & maxVorHunter;
	ar & minSearchAreaHunter;
	ar & maxSearchAreaHunter;

	ar & minProbabilityDensityFunction;
	ar & maxProbabilityDensityFunction;
	ar & minVorXVor;
	ar & maxVorXVor;
	ar & minSpeedRatio;
	ar & maxSpeedRatio;

	ar & maxPredationProbability;
	ar & maxPredationIndividualWetMass;

	ar & cellEvaluationBiomass;
	ar & cellEvaluationRisk;
	ar & cellEvaluationProConspecific;
	ar & cellEvaluationAntiConspecific;
	ar & conspecificWeighing;

	ar & instarDevTimeVector;
	ar & vonBertLinf;
	ar & vonBertKini;
	ar & longevitySinceMaturation;
	ar & reproTimeFactor;
	ar & devInter;
	ar & fractSearchExtremeT;
	ar & fractSpeedExtremeT;
	ar & tempFromLab;

	ar & eggClutchFromEquation;
	ar & forClutchMassCoefficient;
	ar & forClutchMassScale;

	ar & scaleForVoracity;
	ar & scaleForSearchArea;
	ar & scaleForSpeed;
	ar & maxPlasticityKVonBertalanffy;
	ar & minPlasticityKVonBertalanffy;
	ar & plasticityDueToConditionVor;
	ar & plasticityDueToConditionSearch;
	ar & plasticityDueToConditionSpeed;

	ar & attackProbability;
	ar & exposedAttackProbability;
	ar & killProbability;
	ar & optimalTemperatureModifier;
	ar & daysWithoutFoodForMetabolicDownregulation;
	ar & percentageMetabolicDownregulation;
	ar & percentageCostForMetabolicDownregulationVoracity;
	ar & percentageCostForMetabolicDownregulationSearchArea;
	ar & percentageCostForMetabolicDownregulationSpeed;

	ar & coefficientForMassA;
	ar & scaleForMassB;

	ar & maximumDryMassObserved;

	ar & coefficientForMassAforMature;
	ar & scaleForMassBforMature;

	ar & forDensitiesA;
	ar & forDensitiesB;
	ar & instarsForNextReproduction;
	ar & sexualType;
	ar & sexRatio;
	ar & size;
	ar & femaleMaxReproductionEvents;
	ar & eggsPerBatchFromEquation;
	ar & eggsPerBatch;
	ar & interceptForEggBatchFromEquation;
	ar & slopeForEggBatchFromEquation;
	ar & maleMaxReproductionEvents;
	ar & maleReproductionFactor;
	ar & maleMobility;
	ar & surviveWithoutFood;
	ar & activityUnderPredationRisk;
	ar & probabilityDeathFromBackground;
	ar & involvedResourceSpecies;

	ar & habitatShiftBeforeBreeding;
	ar & habitatShiftAfterBreeding;

	ar & habitatShiftBeforeBreedingFactor;
	ar & habitatShiftAfterBreedingFactor;

	ar & habitatShiftPerInstar;
	ar & habitatShiftFactor;

	ar & cellSizePerInstar;
	ar &cellDepthPerInstar;

	ar & forcePresenceAllResourcesInvolved;
	ar & preserveLeftovers;

	ar &loci;
	ar & randomlyCreatedPositionsForChromosomes;
}
