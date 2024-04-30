/*
 * AnimalSpecies.cpp
 *
 *  Created on: Jul 6, 2013
 *      Author: jrbcast
 */

#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/Map/SpatialTreeInterface.h"
#include "IBM/World/Map/TerrainCells/EdibleSearchParams.h"
#include "IBM/World/Map/TerrainCells/AnimalSearchParams.h"
#include "IBM/World/WorldInterface.h"


using namespace std;
using json = nlohmann::json;



AnimalSpecies::AnimalID::AnimalID()
{

}

AnimalSpecies::AnimalID::AnimalID(const id_type& value)
	: value(value)
{

}

const id_type& AnimalSpecies::AnimalID::getValue() const
{
	return value;
}

AnimalSpecies::AnimalID::operator size_t() const {
	return static_cast<size_t>(value);
}

size_t hash<AnimalSpecies::AnimalID>::operator()(const AnimalSpecies::AnimalID& animalID) const
{
    return hash<unsigned int>{}(animalID.getValue());
}

template <class Archive>
void AnimalSpecies::AnimalID::serialize(Archive &ar, const unsigned int version) {
	ar & value;
}




AnimalSpecies::AnimalSpecies(WorldInterface* const worldInterface)
	: Species(worldInterface)
{
	
}

AnimalSpecies::AnimalSpecies(const AnimalSpecies::ID& speciesId, const AnimalSpecies::AnimalID& animalSpeciesId, const json &info, bool initIndividualsPerDensities, WorldInterface* const worldInterface) 
	: Species(speciesId, info["name"], info["individualsPerInstar"].size(), worldInterface), 
	  animalSpeciesId(animalSpeciesId), 
	  defaultHuntingMode(EnumClass<HuntingMode>::stringToEnumValue(info["defaultHuntingMode"])),
	  edibleAnimalSpecies(getNumberOfInstars()),
	  edibleResourceSpecies(getNumberOfInstars()),
	  edibleSearchParams(getNumberOfInstars()), 
	  breedSearchParams(EnumClass<Gender>::size()), 
	  cellEvaluationSearchParams(getNumberOfInstars()), matureFemalesSearchParams(new EdibleSearchParams()), 
	  populationSearchParams(new AnimalSearchParams()), lifeStageSearchParams(EnumClass<LifeStage>::size()), 
	  traits(EnumClass<Trait::Type>::size(), nullptr), numberOfLociPerTrait(info["genetics"]["numberOfLociPerTrait"]),
	  numberOfAllelesPerLocus(info["genetics"]["numberOfAllelesPerLocus"]), 
	  traitsPerModule(info["genetics"]["modules"]["traitsPerModule"]),
	  numberOfChiasmasPerChromosome(info["genetics"]["numberOfChiasmasPerChromosome"]),
	  rhoPerModule(info["genetics"]["modules"]["correlationCoefficientRhoPerModule"]), 
	  tempOptGrowth(info["tempOptGrowth"]), tempOptSearch(info["tempOptSearch"]),
	  tempOptVoracity(info["tempOptVoracity"]), tempOptSpeed(info["tempOptSpeed"]),
	  tempFromLab(info["tempFromLab"]), maximumDryMassObserved(0.0),
	  indeterminateGrowth(info["indeterminateGrowth"]),
	  instarFirstReproduction((hasIndeterminateGrowth()) ? Instar(info["instarFirstReproduction"]) : Instar(info["individualsPerInstar"].size())), 
	  sexualType(EnumClass<SexualType>::stringToEnumValue(info["sexualType"])), involvedResourceSpecies(getNumberOfInstars()), 
	  cellSizePerInstar(static_cast<vector<double>>(info["cellSizePerInstar"])), 
	  forcePresenceAllResourcesInvolved(info["forcePresenceAllResourcesInvolved"]),
	  preserveLeftovers(info["preserveLeftovers"])
{
	setTraits(info["genetics"]["traits"]);

	this->numberOfChromosomes = getNumberOfVariableTraits();

	if (numberOfChiasmasPerChromosome % 2 != 0 || numberOfChiasmasPerChromosome < 0)
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "numberOfChiasmasPerChromosome must be multiple of 2. You entered " << numberOfChiasmasPerChromosome << " = " << numberOfChiasmasPerChromosome << ". Exiting now" << std::endl;
		exit(-1);
	}

	//TODO Hay alguna restricción más para el parametrizador? Cual es el máximo de numberOfChromosomes??
	if ((getNumberOfVariableTraits()*numberOfLociPerTrait) % numberOfChromosomes == 0)
	{
		this->numberOfLociPerChromosome = (getNumberOfVariableTraits()*numberOfLociPerTrait) / numberOfChromosomes;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "numberOfTraits*numberOfLoci must be multiple of numberOfChromosomes. You entered " << getNumberOfVariableTraits() << "*" << numberOfLociPerTrait << " = " << getNumberOfVariableTraits()*numberOfLociPerTrait << " --> " << getNumberOfVariableTraits()*numberOfLociPerTrait << " % " << numberOfChromosomes << " = " << ((getNumberOfVariableTraits()*numberOfLociPerTrait) % numberOfChromosomes) << ". Exiting now" << std::endl;
		exit(-1);
	}

	this->rhoRangePerModule.reserve(rhoPerModule.size());
	for(size_t i = 0; i < rhoPerModule.size(); ++i)
	{
		this->rhoRangePerModule.push_back(round((1.0 - fabs(rhoPerModule[i])) * numberOfLociPerTrait));
	}

	loci.reserve(numberOfLociPerTrait);
	for (unsigned int i = 0; i < numberOfLociPerTrait; ++i)
	{
		loci.emplace_back(i, getNumberOfAllelesPerLocus());
	}

	randomlyCreatedPositionsForChromosomes.reserve(getNumberOfVariableTraits()*getNumberOfLociPerTrait());
	for(unsigned int i = 0; i < getNumberOfVariableTraits()*getNumberOfLociPerTrait(); ++i)
	{
		randomlyCreatedPositionsForChromosomes.emplace_back(i);
	}
	random_shuffle(randomlyCreatedPositionsForChromosomes.begin(), randomlyCreatedPositionsForChromosomes.end());



	normal_distribution<double> normalDistributionX (0.0,1.0);
	normal_distribution<double> normalDistributionY (0.0,1.0);
	pseudoGrowthMean = 0.0;
	pseudoGrowthSd = 0.0;

	//Constants for interactions
	minSizeHunted = 10000000.000;
	maxSizeHunted = 0;
	meanSizeHunted = 0.0;
	sdSizeHunted = 0.0;
	minVorHunted = 10000000.000;
	maxVorHunted = 0;
	meanVorHunted = 0.0;
	sdVorHunted = 0.0;
	minSpeedHunted = 10000000.00;
	maxSpeedHunted = 0.0;
	meanSpeedHunted = 0.0;
	sdSpeedHunted = 0.0;

	minSizeHunter = 10000000.000;
	maxSizeHunter = 0.0;
	meanSizeHunter = 0.0;
	sdSizeHunter = 0.0;
	minVorHunter = 10000000.000;
	maxVorHunter = 0.0;
	meanVorHunter = 0.0;
	sdVorHunter = 0.0;
	minSpeedHunter = 10000000.000;
	maxSpeedHunter = 0.0;
	meanSpeedHunter = 0.0;
	sdSpeedHunter = 0.0;
	minSearchAreaHunter = 10000000.000;
	maxSearchAreaHunter = 0.0;
	meanSearchAreaHunter = 0.0;
	sdSearchAreaHunter = 0.0;

	minProbabilityDensityFunction = 10000000.000;
	maxProbabilityDensityFunction = 0.0;
	meanProbabilityDensityFunction = 0.0;
	sdProbabilityDensityFunction = 0.0;
	minVorXVor = 0.0;
	maxVorXVor = 0.0;
	meanVorXVor = 0.0;
	sdVorXVor = 0.0;
	minSpeedRatio = 0.0;
	maxSpeedRatio = 0.0;
	meanSpeedRatio = 0.0;
	sdSpeedRatio = 0.0;
	
	meanSizeRatio = 0.0;
	sdSizeRatio = 0.0;

	maxPredationProbability = 0.0;
	maxPredationIndividualWetMass = 0.0;

	setDevTimeVector(info["devTimeVector"]);

	//Added for new growth_curves
	if(initIndividualsPerDensities == false)
	{
		setInitialPopulation(info["individualsPerInstar"]);
	}
	setStatisticsIndividualsPerInstar(info["statisticsIndividualsPerInstar"]);

	setAssignedForMolt(info["assignedForMolt"]);
	setBetaScaleTank(info["betaScaleTank"]);
	setExcessInvestInSize(info["excessInvestInSize"]);
	setPupaPeriodLength(info["pupaPeriodLength"]);
	setHabitatShiftBeforeBreeding(info["habitatShiftBeforeBreeding"], info["habitatShiftBeforeBreedingFactor"]);
	setHabitatShiftAfterBreeding(info["habitatShiftAfterBreeding"], info["habitatShiftAfterBreedingFactor"]);
	setHabitatShift(info["habitatShift"], info["habitatShiftFactor"]);
	setMinRelativeHumidityThreshold(info["minRelativeHumidityThreshold"]);
	setMaxEncountersT(info["maxEncountersT"]);

	//Added for new growth_curves
	setLinfKcorr(info["LinfKcorr"]);
	setDevTimeConstant(info["devTimeConstant"]);
	setLongevitySinceMaturation(info["longevitySinceMaturation"]);
	setReproTimeFactor(info["reproTimeFactor"]);
	setEdGrowth(info["EdGrowth"]);
	setEdVoracity(info["EdVoracity"]);
	setEdSearch(info["EdSearch"]);
	setEdSpeed(info["EdSpeed"]);
	setDevInter(info["devInter"]);
	setFractSearchExtremeT(info["fractSearchExtremeT"]);
	setFractSpeedExtremeT(info["fractSpeedExtremeT"]);

	setTempSizeRuleConstant(info["tempSizeRuleConstant"]);

	setExperienceInfluencePerDay(info["experienceInfluencePerDay"]);

	setMassInfo(info["conversionToWetMass"], info["eggClutchFromEquation"],
							info["forClutchMassCoefficient"], info["forClutchMassScale"],
							info["forEggMassCoefficient"], info["forEggMassScale"],
							info["eggDryMass"], info["femaleWetMass"], 
							info["eggMassFromEquation"]);

	setScaleForVoracity(info["scaleForVoracity"]);
	setScaleForSearchArea(info["scaleForSearchArea"]);
	setScaleForSpeed(info["scaleForSpeed"]);
	setMaxPlasticityKVonBertalanffy(info["maxPlasticityKVonBertalanffy"]);
	setMinPlasticityKVonBertalanffy(info["minPlasticityKVonBertalanffy"]);
	setMaxPlasticityDueToConditionVor(info["maxPlasticityDueToConditionVor"]);
	setMinPlasticityDueToConditionVor(info["minPlasticityDueToConditionVor"]);
	setMaxPlasticityDueToConditionSearch(info["maxPlasticityDueToConditionSearch"]);
	setMinPlasticityDueToConditionSearch(info["minPlasticityDueToConditionSearch"]);
	setMaxPlasticityDueToConditionSpeed(info["maxPlasticityDueToConditionSpeed"]);
	setMinPlasticityDueToConditionSpeed(info["minPlasticityDueToConditionSpeed"]);

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
	setDecreaseOnTraitsDueToEncounters(info["decreaseOnTraitsDueToEncounters"]);
	setMaleReproductionFactor(info["maleReproductionFactor"]);
	setProbabilityDeathFromBackground(info["probabilityDeathFromBackground"]);

	growthCurve = Curve::createInstance(info).release();


	if(getNumberOfVariableTraits()/getTraitsPerModule() + (getNumberOfVariableTraits()%getTraitsPerModule() == 0) ? 0 : 1)
	{
		throwLineInfoException("Error: Item 'correlationCoefficientRhoPerModule' must have a size equal to the value of the arithmetic expression 'numberOfVariableTraits/traitsPerModule + (numberOfVariableTraits%traitsPerModule==0?0:1)'.");
	}
}

AnimalSpecies::~AnimalSpecies()
{
	delete growthCurve;

	for(Trait* elem : traits)
	{
		delete elem;
	}

	delete matureFemalesSearchParams;
	
	delete populationSearchParams;
}

const AnimalSpecies::AnimalID& AnimalSpecies::getAnimalSpeciesId() const 
{ 
	return animalSpeciesId; 
}

void AnimalSpecies::initEdibleOntogeneticLink(const vector<Species*>& existingSpecies)
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

const OntogeneticLink& AnimalSpecies::getEdibleOntogeneticLink(const Species::ID &preySpeciesId, const Instar &predator, const Instar &prey) const
{
	return edibleOntogeneticLink.at(preySpeciesId).at(predator.getValue()).at(prey.getValue());
}

void AnimalSpecies::setEdibleOntogeneticLink(const Species::ID &preySpeciesId, const Instar &predator, const Instar &prey, const json &linkInfo)
{
	edibleOntogeneticLink[preySpeciesId][predator.getValue()][prey.getValue()].setInfo(linkInfo);
}

void AnimalSpecies::checkOntogeneticLinksPreference() const
{
	for(const Instar &predatorInstar : getInstarsRange())
	{
		double instarPreference = 0.0;

		for(const auto &[preyAnimalSpecies, preySpeciesInstars] : getInstarEdibleAnimalSpecies(predatorInstar))
		{
			for(const Instar &preyInstar : preySpeciesInstars)
			{
				instarPreference += getEdiblePreference(getWorldInterface()->getExistingAnimalSpecies().at(preyAnimalSpecies)->getId(), predatorInstar, preyInstar);
			}
		}

		for(const auto &[preyResourceSpecies, preySpeciesInstars] : getInstarEdibleResourceSpecies(predatorInstar))
		{
			for(const Instar &preyInstar : preySpeciesInstars)
			{
				instarPreference += getEdiblePreference(getWorldInterface()->getExistingResourceSpecies().at(preyResourceSpecies)->getId(), predatorInstar, preyInstar);
			}
		}
		

		if(instarPreference > 1.0)
		{
			throwLineInfoException("The total preference of instar " + to_string(predatorInstar.getValue() + 1) + " of '" + getScientificName() + "' animal species must less or equal to 1");
		}
	}
}

void AnimalSpecies::addEdibleOntogeneticLink(Species::Type speciesType, Species* newSpecies, const json& ontogeneticLink)
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

void AnimalSpecies::addEdibleLink(Species::Type speciesType, Species* newSpecies, const vector<Instar> &predatorInstarVector, 
		const vector<Instar> &preyInstarVector, const json& linkValue)
{
	for(const Instar &predatorInstar : predatorInstarVector)
	{
		// Esto se puede sustituir por una condicción en los schemas
		if(predatorInstar > Instar(getNumberOfInstars()))
		{
			throwLineInfoException("The instar value is not in the instar range of the predator species");
		}

		for(const Instar &preyInstar : preyInstarVector)
		{
			if(preyInstar > Instar(newSpecies->getNumberOfInstars()))
			{
				throwLineInfoException("The instar value is not in the instar range of the prey species");
			}

			vector<Instar>* actualPreyInstarVector;

			switch(speciesType)
			{
			case Species::Type::Animal:
				actualPreyInstarVector = &edibleAnimalSpecies[predatorInstar][static_cast<AnimalSpecies*>(newSpecies)->getAnimalSpeciesId()];
				break;
			case Species::Type::Resource:
				actualPreyInstarVector = &edibleResourceSpecies[predatorInstar][static_cast<ResourceSpecies*>(newSpecies)->getResourceSpeciesId()];
				break;
			default:
				throwLineInfoException("Default case");
				break;
			}

			try
			{
				if(find(actualPreyInstarVector->begin(), actualPreyInstarVector->end(), preyInstar) != actualPreyInstarVector->end())
				{
					throwLineInfoException("Trying to add an edible species twice, please contact developers");
				}

				actualPreyInstarVector->push_back(preyInstar);
			}
			catch(const std::out_of_range& e)
			{
				*actualPreyInstarVector = vector<Instar>();
				actualPreyInstarVector->push_back(preyInstar);
			}

			setEdibleOntogeneticLink(newSpecies->getId(), predatorInstar, preyInstar, linkValue);
		}
	}
}

void AnimalSpecies::calculateCellDepthPerInstar(const MapInterface* const mapInterface)
{
	cellDepthPerInstar.reserve(cellSizePerInstar.size());
	
	for(const double &cellSize : cellSizePerInstar)
	{
		unsigned int actualDepth = static_cast<const SpatialTreeInterface* const>(mapInterface)->getMapDepth()-1;

		while(cellSize > static_cast<const SpatialTreeInterface* const>(mapInterface)->getCellSize(actualDepth))
		{
			actualDepth--;
		}

		cellDepthPerInstar.push_back(actualDepth);
	}
}

const CustomIndexedVector<Instar, unsigned int>& AnimalSpecies::getCellDepthPerInstar() const
{
	return cellDepthPerInstar;
}

const unsigned int AnimalSpecies::getInstarCellDepth(const Instar &instar) const
{
	return cellDepthPerInstar[instar];
}

double AnimalSpecies::getEdiblePreference(const Species::ID &preySpeciesId, const Instar &predator) const
{
	double preference = 0.0;

	for(const auto &elem : edibleOntogeneticLink[preySpeciesId][predator.getValue()])
	{
		preference += elem.getPreference();
	}

	return preference;
}

const double& AnimalSpecies::getEdiblePreference(const Species::ID &preySpeciesId, const Instar &predator, const Instar &prey) const
{
	return getEdibleOntogeneticLink(preySpeciesId, predator, prey).getPreference();
}

const double& AnimalSpecies::getEdibleProfitability(const Species::ID &preySpeciesId, const Instar &predator, const Instar &prey)
{
	return getEdibleOntogeneticLink(preySpeciesId, predator, prey).getProfitability();
}

bool AnimalSpecies::canEatEdible(const Species::ID &preySpeciesId, const Instar &predator, const Instar &prey) const
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

void AnimalSpecies::calculateK_Density(vector<CustomIndexedVector<Instar, bool>> &checkedAnimalSpecies, 
		vector<CustomIndexedVector<Instar, bool>> &cannibalismAnimalSpecies)
{
	for(const Instar& instar : getInstarsRange())
	{
		calculateInstarK_Density(instar, checkedAnimalSpecies, cannibalismAnimalSpecies);
	}
}

double AnimalSpecies::calculateInstarK_Density(const Instar &instar, vector<CustomIndexedVector<Instar, bool>> &checkedAnimalSpecies, 
		vector<CustomIndexedVector<Instar, bool>> &cannibalismAnimalSpecies)
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
		for(const auto &[animalSpecies, animalSpeciesInstars] : getInstarEdibleAnimalSpecies(instar))
		{
			for(const Instar &targetInstar : animalSpeciesInstars)
			{
				K_Density += getMutableWorldInterface()->getMutableExistingAnimalSpecies().at(animalSpecies)->calculateInstarK_Density(targetInstar, checkedAnimalSpecies, cannibalismAnimalSpecies);
			}
		}

		for(const auto &[resourceSpecies, resourceSpeciesInstars] : getInstarEdibleResourceSpecies(instar))
		{
			for(const Instar &targetInstar : resourceSpeciesInstars)
			{
				K_Density += getMutableWorldInterface()->getMutableExistingResourceSpecies().at(resourceSpecies)->calculateInstarK_Density(targetInstar, checkedAnimalSpecies, cannibalismAnimalSpecies);
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

const vector<Trait::Type>* const AnimalSpecies::getVariableTraits() const
{
	return &variableTraits;
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

void AnimalSpecies::generateInvolvedResourceSpecies(const vector<Species*> &existingSpecies)
{
	for(const Instar &instar : getInstarsRange())
	{
		vector<CustomIndexedVector<Instar, bool>> alreadyCheckedSpecies(existingSpecies.size());

		for(const auto &species : existingSpecies)
		{
			alreadyCheckedSpecies[species->getId()] = CustomIndexedVector<Instar, bool>(species->getNumberOfInstars(), false);
		}

		generateInstarInvolvedResourceSpecies(instar, involvedResourceSpecies[instar], alreadyCheckedSpecies);
		
		if(getInstarInvolvedResourceSpecies(instar).empty())
		{
			throwLineInfoException("Each instar must have at least one resource species involved");
		}
	}
}

void AnimalSpecies::generateInstarInvolvedResourceSpecies(const Instar &instar, vector<ResourceSpecies::ResourceID> &instarInvolvedResourceSpecies, vector<CustomIndexedVector<Instar, bool>> &alreadyCheckedSpecies)
{
	auto isAlreadyCheckedSpecies = alreadyCheckedSpecies[getId()][instar];

	if(!isAlreadyCheckedSpecies)
	{
		alreadyCheckedSpecies[getId()][instar] = true;

		if(getInstarInvolvedResourceSpecies(instar).empty())
		{
			for(const auto& [preyAnimalSpecies, preyInstarsVector] : getInstarEdibleAnimalSpecies(instar))
			{
				for(const Instar &preyInstar : preyInstarsVector)
				{
					getMutableWorldInterface()->getMutableExistingAnimalSpecies().at(preyAnimalSpecies)->generateInstarInvolvedResourceSpecies(preyInstar, instarInvolvedResourceSpecies, alreadyCheckedSpecies);
				}
			}

			for(const auto& [preyResourceSpecies, preyInstarsVector] : getInstarEdibleResourceSpecies(instar))
			{
				for(const Instar &preyInstar : preyInstarsVector)
				{
					getMutableWorldInterface()->getMutableExistingResourceSpecies().at(preyResourceSpecies)->generateInstarInvolvedResourceSpecies(preyInstar, instarInvolvedResourceSpecies, alreadyCheckedSpecies);
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

void AnimalSpecies::obtainEdibleSearchParams()
{
	for(const Instar &instar : getInstarsRange())
	{
		for(const AnimalSpecies* const &otherAnimalSpecies : getWorldInterface()->getExistingAnimalSpecies())
		{
			for(const Instar &otherAnimalSpeciesInstar : otherAnimalSpecies->getInstarsRange())
			{
				vector<LifeStage> searchableLifeStages;

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
						getWorldInterface(), searchableLifeStages, {otherAnimalSpecies->getAnimalSpeciesId()},
						{otherAnimalSpeciesInstar}
					);
				}
			}
		}

		for(const auto &[resourceSpecies, resourceSpeciesInstars] : getInstarEdibleResourceSpecies(instar))
		{
			edibleSearchParams[instar].addResourceSearchParams(
				getWorldInterface(), {resourceSpecies}
			);
		}	
	}
}

const EdibleSearchParams& AnimalSpecies::getBreedSearchParams(const Gender &gender) const
{
	return breedSearchParams[gender];
}

void AnimalSpecies::obtainBreedSearchParams()
{
	auto searchableLifeStages = {LifeStage::ACTIVE, LifeStage::SATIATED, LifeStage::HANDLING, LifeStage::DIAPAUSE};

	// Get all the animals that can be mated
	vector<Instar> searchableInstars(find(getInstarsRange().begin(), getInstarsRange().end(), getInstarFirstReproduction()), getInstarsRange().end());

	vector<Gender> searchableGender;

	for(const auto &gender : EnumClass<Gender>::getEnumValues())
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
			getWorldInterface(), searchableLifeStages, {getAnimalSpeciesId()}, searchableInstars, searchableGender
		);
	}
}

const EdibleSearchParams& AnimalSpecies::getCellEvaluationSearchParams(const Instar &instar) const
{
    return cellEvaluationSearchParams[instar];
}

void AnimalSpecies::obtainCellEvaluationSearchParams()
{
	for(const Instar &instar : getInstarsRange())
	{
		for(const AnimalSpecies* const &otherAnimalSpecies : getWorldInterface()->getExistingAnimalSpecies())
		{
			if(getAnimalSpeciesId() == otherAnimalSpecies->getAnimalSpeciesId())
			{
				cellEvaluationSearchParams[instar].addAnimalSearchParams(
					getWorldInterface(), {LifeStage::ACTIVE}, {otherAnimalSpecies->getAnimalSpeciesId()}
				);
			}
			else
			{
				for(const Instar &otherAnimalSpeciesInstar : otherAnimalSpecies->getInstarsRange())
				{
					vector<LifeStage> searchableLifeStages;

					// Check if the animal is prey for the species
					bool isSpeciePrey = otherAnimalSpecies->canEatEdible(getId(), otherAnimalSpeciesInstar, instar);
					// Check if the species is prey for the animal
					bool isSpeciePredator = canEatEdible(otherAnimalSpecies->getId(), instar, otherAnimalSpeciesInstar);
					
					if(isSpeciePrey || isSpeciePredator)
					{
						cellEvaluationSearchParams[instar].addAnimalSearchParams(
							getWorldInterface(), {LifeStage::ACTIVE}, {otherAnimalSpecies->getAnimalSpeciesId()},
							{otherAnimalSpeciesInstar}
						);
					}
				}
			}
		}

		for(const auto &[resourceSpecies, resourceSpeciesInstars] : getInstarEdibleResourceSpecies(instar))
		{
			cellEvaluationSearchParams[instar].addResourceSearchParams(
				getWorldInterface(), {resourceSpecies}
			);
		}	
	}
}

const EdibleSearchParams& AnimalSpecies::getMatureFemalesSearchParams() const
{
	return *matureFemalesSearchParams;
}

void AnimalSpecies::obtainMatureFemalesSearchParams()
{
	matureFemalesSearchParams->addAnimalSearchParams(
		getWorldInterface(), {LifeStage::ACTIVE, LifeStage::SATIATED, LifeStage::HANDLING, LifeStage::DIAPAUSE},
		{getAnimalSpeciesId()}, {}, {AnimalSpecies::Gender::FEMALE}
	);
}

const AnimalSearchParams& AnimalSpecies::getPopulationSearchParams() const
{
	return *populationSearchParams;
}

void AnimalSpecies::obtainPopulationSearchParams()
{
	populationSearchParams->addSearchParams(getWorldInterface(), EnumClass<LifeStage>::getEnumValues(), {getAnimalSpeciesId()});
}

const AnimalSearchParams& AnimalSpecies::getLifeStageSearchParams(const LifeStage &lifeStage) const
{
    return lifeStageSearchParams[lifeStage];
}

void AnimalSpecies::obtainLifeStageSearchParams()
{
    for(const auto &lifeStage : EnumClass<LifeStage>::getEnumValues())
    {
        lifeStageSearchParams[lifeStage].addSearchParams(getWorldInterface(), {lifeStage}, {getAnimalSpeciesId()});
    }
}

const vector<ResourceSpecies::ResourceID>& AnimalSpecies::getInstarInvolvedResourceSpecies(const Instar &instar) const
{
	return involvedResourceSpecies[instar];
}

const unordered_map<AnimalSpecies::AnimalID, vector<Instar>>& AnimalSpecies::getInstarEdibleAnimalSpecies(const Instar &instar) const
{
	return edibleAnimalSpecies[instar];
}

const unordered_map<ResourceSpecies::ResourceID, vector<Instar>>& AnimalSpecies::getInstarEdibleResourceSpecies(const Instar &instar) const
{
	return edibleResourceSpecies[instar];
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



void AnimalSpecies::calculateMinSpeedHunted(double minSpeedHunted)
{
	this->minSpeedHunted = min(this->minSpeedHunted, minSpeedHunted);

}

void AnimalSpecies::calculateMaxSpeedHunted(double maxSpeedHunted)
{
	this->maxSpeedHunted = max(this->maxSpeedHunted, maxSpeedHunted);

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

void AnimalSpecies::calculateMinSpeedHunter(double minSpeedHunter)
{
	this->minSpeedHunter = min(this->minSpeedHunter, minSpeedHunter);

}

void AnimalSpecies::calculateMaxSpeedHunter(double maxSpeedHunter)
{
	this->maxSpeedHunter = max(this->maxSpeedHunter, maxSpeedHunter);

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

void AnimalSpecies::sumMeanSizeHunted(double meanSizeHunted)
{
	this->meanSizeHunted += meanSizeHunted;
}

void AnimalSpecies::sumMeanVorHunted(double meanVorHunted)
{
	this->meanVorHunted += meanVorHunted;
}

void AnimalSpecies::sumSdSizeHunted(double sdSizeHunted)
{
	this->sdSizeHunted += pow(sdSizeHunted - meanSizeHunted, 2);
}

void AnimalSpecies::sumSdVorHunted(double sdVorHunted)
{
	this->sdVorHunted += pow(sdVorHunted - meanVorHunted, 2);
}

void AnimalSpecies::sumMeanSpeedHunted(double meanSpeedHunted)
{
	this->meanSpeedHunted += meanSpeedHunted;
}

void AnimalSpecies::sumSdSpeedHunted(double sdSpeedHunted)
{
	this->sdSpeedHunted += pow(sdSpeedHunted - meanSpeedHunted, 2);
}

void AnimalSpecies::sumMeanSizeHunter(double meanSizeHunter)
{
	this->meanSizeHunter += meanSizeHunter;
}

void AnimalSpecies::sumSdSizeHunter(double sdSizeHunter)
{
	this->sdSizeHunter += pow(sdSizeHunter - meanSizeHunter, 2);
}

void AnimalSpecies::sumMeanVorHunter(double meanVorHunter)
{
	this->meanVorHunter += meanVorHunter;
}

void AnimalSpecies::sumSdVorHunter(double sdVorHunter)
{
	this->sdVorHunter += pow(sdVorHunter - meanVorHunter, 2);
}

void AnimalSpecies::sumMeanSpeedHunter(double meanSpeedHunter)
{
	this->meanSpeedHunter += meanSpeedHunter;
}

void AnimalSpecies::sumSdSpeedHunter(double sdSpeedHunter)
{
	this->sdSpeedHunter += pow(sdSpeedHunter - meanSpeedHunter, 2);
}

void AnimalSpecies::sumMeanSearchAreaHunter(double meanSearchAreaHunter)
{
	this->meanSearchAreaHunter += meanSearchAreaHunter;
}

void AnimalSpecies::sumSdSearchAreaHunter(double sdSearchAreaHunter)
{
	this->sdSearchAreaHunter += pow(sdSearchAreaHunter - meanSearchAreaHunter, 2);
}

void AnimalSpecies::sumMeanProbabilityDensityFunction(double meanProbabilityDensityFunction)
{
	this->meanProbabilityDensityFunction += meanProbabilityDensityFunction;
}

void AnimalSpecies::sumSdProbabilityDensityFunction(double sdProbabilityDensityFunction)
{
	this->sdProbabilityDensityFunction += pow(sdProbabilityDensityFunction - meanProbabilityDensityFunction, 2);
}

void AnimalSpecies::sumMeanVorXVor(double meanVorXVor)
{
	this->meanVorXVor += meanVorXVor;
}

void AnimalSpecies::sumSdVorXVor(double sdVorXVor)
{
	this->sdVorXVor += pow(sdVorXVor - meanVorXVor, 2);
}

void AnimalSpecies::sumMeanSpeedRatio(double meanSpeedRatio)
{

	this->meanSpeedRatio += meanSpeedRatio;

}

void AnimalSpecies::sumSdSpeedRatio(double sdSpeedRatio)
{
	this->sdSpeedRatio += pow(sdSpeedRatio - meanSpeedRatio, 2);
}

void AnimalSpecies::sumMeanSizeRatio(double meanSizeRatio)
{
	this->meanSizeRatio += meanSizeRatio;
}

void AnimalSpecies::sumSdSizeRatio(double sdSizeRatio)
{
	this->sdSizeRatio += pow(sdSizeRatio - meanSizeRatio, 2);
}

void AnimalSpecies::calculateMeanSizeHunted(int population)
{
	this->meanSizeHunted = meanSizeHunted / population;
}

void AnimalSpecies::calculateSdSizeHunted(int population)
{
	this->sdSizeHunted = sqrt(sdSizeHunted / population);
}

void AnimalSpecies::calculateMeanVorHunted(int population)
{
	this->meanVorHunted = meanVorHunted / population;
}

void AnimalSpecies::calculateSdVorHunted(int population)
{
	this->sdVorHunted = sqrt(sdVorHunted / population);
}

void AnimalSpecies::calculateMeanSpeedHunted(int population)
{
	this->meanSpeedHunted = meanSpeedHunted / population;
}

void AnimalSpecies::calculateSdSpeedHunted(int population)
{
	this->sdSpeedHunted = sqrt(sdSpeedHunted / population);
}

void AnimalSpecies::calculateMeanSizeHunter(int population)
{
	this->meanSizeHunter = meanSizeHunter / population;
}

void AnimalSpecies::calculateSdSizeHunter(int population)
{
	this->sdSizeHunter = sqrt(sdSizeHunter / population);
}

void AnimalSpecies::calculateMeanVorHunter(int population)
{
	this->meanVorHunter = meanVorHunter / population;
}

void AnimalSpecies::calculateSdVorHunter(int population)
{
	this->sdVorHunter = sqrt(sdVorHunter / population);
}

void AnimalSpecies::calculateMeanSpeedHunter(int population)
{
	this->meanSpeedHunter = meanSpeedHunter / population;
}

void AnimalSpecies::calculateSdSpeedHunter(int population)
{
	this->sdSpeedHunter = sqrt(sdSpeedHunter / population);
}

void AnimalSpecies::calculateMeanSearchAreaHunter(int population)
{
	this->meanSearchAreaHunter = meanSearchAreaHunter / population;
}

void AnimalSpecies::calculateSdSearchAreaHunter(int population)
{
	this->sdSearchAreaHunter = sqrt(sdSearchAreaHunter / population);
}

void AnimalSpecies::calculateMeanProbabilityDensityFunction(int population)
{
	this->meanProbabilityDensityFunction = meanProbabilityDensityFunction / population;
}

void AnimalSpecies::calculateSdProbabilityDensityFunction(int population)
{
	this->sdProbabilityDensityFunction = sqrt(sdProbabilityDensityFunction / population);
}

void AnimalSpecies::calculateMeanVorXVor(int population)
{
	this->meanVorXVor = meanVorXVor / population;
}

void AnimalSpecies::calculateSdVorXVor(int population)
{
	this->sdVorXVor = sqrt(sdVorXVor / population);
}

void AnimalSpecies::calculateMeanSpeedRatio(int population)
{
	this->meanSpeedRatio = meanSpeedRatio / population;

	cout << "mean" << meanSpeedRatio << endl;
	cout << "pop" << population << endl;
}

void AnimalSpecies::calculateSdSpeedRatio(int population)
{
	this->sdSpeedRatio = sqrt(sdSpeedRatio / population);
}

void AnimalSpecies::calculateMeanSizeRatio(int population)
{
	this->meanSizeRatio = meanSizeRatio / population;
}

void AnimalSpecies::calculateSdSizeRatio(int population)
{
	this->sdSizeRatio = sqrt(sdSizeRatio / population);
}

void AnimalSpecies::setVonBertLinf(double vonBertLinf)
{
	this->vonBertLinf = vonBertLinf;
}

void AnimalSpecies::setVonBertKini(double vonBertKini)
{
	this->vonBertKini = vonBertKini;
}

void AnimalSpecies::setLinfKcorr(const double& LinfKcorr)
{
	this->LinfKcorr = LinfKcorr;
	cholMat = MathFunctions::cholesky(LinfKcorr);
}

void AnimalSpecies::setEggMassFromEquation(const bool& eggMassFromEquation)
{
	this->eggMassFromEquation = eggMassFromEquation;
	if(eggMassFromEquation)
	{
		eggDryMass = (forEggMassCoefficient * pow(femaleWetMass, forEggMassScale)) / getConversionToWetMass();
	}
}

void AnimalSpecies::setMaxPlasticityDueToConditionVor(const double& maxPlasticityDueToConditionVor)
{
	if (maxPlasticityDueToConditionVor > 0)
	{
		this->maxPlasticityDueToConditionVor = maxPlasticityDueToConditionVor;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "maxPlasticityDueToConditionVor must be a positive value. You entered " << maxPlasticityDueToConditionVor << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMinPlasticityDueToConditionVor(const double& minPlasticityDueToConditionVor)
{
	if (minPlasticityDueToConditionVor > 0)
	{
		this->minPlasticityDueToConditionVor = minPlasticityDueToConditionVor;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "minPlasticityDueToConditionVor must be a positive value. You entered " << minPlasticityDueToConditionVor << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMaxPlasticityDueToConditionSearch(const double& maxPlasticityDueToConditionSearch)
{
	if (maxPlasticityDueToConditionSearch > 0)
	{
		this->maxPlasticityDueToConditionSearch = maxPlasticityDueToConditionSearch;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "maxPlasticityDueToConditionSearch must be a positive value. You entered " << maxPlasticityDueToConditionSearch << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMinPlasticityDueToConditionSearch(const double& minPlasticityDueToConditionSearch)
{
	if (minPlasticityDueToConditionSearch > 0)
	{
		this->minPlasticityDueToConditionSearch = minPlasticityDueToConditionSearch;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "minPlasticityDueToConditionSearch must be a positive value. You entered " << minPlasticityDueToConditionSearch << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMaxPlasticityDueToConditionSpeed(const double& maxPlasticityDueToConditionSpeed)
{
	if (maxPlasticityDueToConditionSpeed > 0)
	{
		this->maxPlasticityDueToConditionSpeed = maxPlasticityDueToConditionSpeed;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "maxPlasticityDueToConditionSpeed must be a positive value. You entered " << maxPlasticityDueToConditionSpeed << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMinPlasticityDueToConditionSpeed(const double& minPlasticityDueToConditionSpeed)
{
	if (minPlasticityDueToConditionSpeed > 0)
	{
		this->minPlasticityDueToConditionSpeed = minPlasticityDueToConditionSpeed;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "minPlasticityDueToConditionSpeed must be a positive value. You entered " << minPlasticityDueToConditionSpeed << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::initializeFixedTraits(CustomIndexedVector<Trait::Type, double>& animalTraits) const
{
	for(size_t i = 0; i < getNumberOfFixedTraits(); i++)
	{
		animalTraits[fixedTraits[i]] = static_cast<FixedTrait*>(traits[fixedTraits[i]])->getValue();
	}
}

AnimalSpecies::Gender AnimalSpecies::getRandomGender() const
{
	AnimalSpecies::Gender newAnimalGender = AnimalSpecies::Gender::MALE;
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

void AnimalSpecies::setMassInfo(const float& conversionToWetMass, const bool& eggClutchFromEquation, const double& forClutchMassCoefficient,
					 			const double& forClutchMassScale, const double& forEggMassCoefficient, const double& forEggMassScale,
					 			const double& eggDryMass, const double& femaleWetMass, const bool& eggMassFromEquation)
{
	setConversionToWetMass(conversionToWetMass);
	setEggClutchFromEquation(eggClutchFromEquation);
	setForClutchMassCoefficient(forClutchMassCoefficient);
	setForClutchMassScale(forClutchMassScale);
	setForEggMassCoefficient(forEggMassCoefficient);
	setForEggMassScale(forEggMassScale);
	setEggDryMass(eggDryMass);
	setFemaleWetMass(femaleWetMass);
	//This setEggMassFromEquation must be used AFTER setting these previous 5 values!!
	setEggMassFromEquation(eggMassFromEquation);
}

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
		case Curve::Type::VonBertalanffy: {
			LdistanceMin = static_cast<const VonBertalanffyCurve* const>(getGrowthCurve())->getLdistanceMin();
			LdistanceMax = static_cast<const VonBertalanffyCurve* const>(getGrowthCurve())->getLdistanceMax();
			break;
		}
		case Curve::Type::Logistic: {
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
	double ageLastInstar = devTimeVector.back();

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

void AnimalSpecies::calculatePseudoGrowthMean()
{
	pseudoGrowthMean = pseudoGrowthMean / getTotalStatisticsInitialPopulation();
	cout << getScientificName() << "_pseudoGrowthMean=" << pseudoGrowthMean << endl;
}

void AnimalSpecies::calculatePseudoGrowthSd()
{
	pseudoGrowthSd = sqrt(pseudoGrowthSd / getTotalStatisticsInitialPopulation());
	cout << getScientificName() << "_pseudoGrowthSd=" << pseudoGrowthSd << endl;
}



void AnimalSpecies::interactionRanges(double hunterAnimalBodySize, double hunterAnimalVoracity, double hunterAnimalSpeed, double hunterAnimalDryMass, double huntedAnimalBodySize, double huntedAnimalVoracity, double huntedAnimalSpeed, double huntedAnimalDryMass, double muForPDF, double sigmaForPDF)
{
	calculateMinSizeHunted(huntedAnimalBodySize);
	calculateMaxSizeHunted(huntedAnimalBodySize);
	
    calculateMinVorHunted(huntedAnimalVoracity);
	calculateMaxVorHunted(huntedAnimalVoracity);
	
	calculateMinSpeedHunted(huntedAnimalSpeed);
	calculateMaxSpeedHunted(huntedAnimalSpeed);

	calculateMinSizeHunter(hunterAnimalBodySize);
	calculateMaxSizeHunter(hunterAnimalBodySize);
	
    calculateMinVorHunter(hunterAnimalVoracity);
	calculateMaxVorHunter(hunterAnimalVoracity);
	
	calculateMinSpeedHunter(hunterAnimalSpeed);
	calculateMaxSpeedHunter(hunterAnimalSpeed);

	double probabilityDensityFunction = exp(-0.5 * pow((log(hunterAnimalDryMass/huntedAnimalDryMass) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
	calculateMinProbabilityDensityFunction(probabilityDensityFunction);
	calculateMaxProbabilityDensityFunction(probabilityDensityFunction);

	calculateMinVorXVor(hunterAnimalVoracity*huntedAnimalVoracity);
	calculateMaxVorXVor(hunterAnimalVoracity*huntedAnimalVoracity);



	if(huntedAnimalSpeed<0.001){

		calculateMinSpeedRatio(hunterAnimalSpeed/huntedAnimalSpeed);
		calculateMaxSpeedRatio(hunterAnimalSpeed/huntedAnimalSpeed);

	}else{

		calculateMinSpeedRatio(hunterAnimalSpeed/huntedAnimalSpeed);
		calculateMaxSpeedRatio(hunterAnimalSpeed/huntedAnimalSpeed);

	}

	sumMeanSizeRatio(hunterAnimalBodySize / huntedAnimalBodySize);
}
 

void AnimalSpecies::sumStatisticMeans(double hunterAnimalBodySize, double hunterAnimalVoracity, double hunterAnimalSpeed, double hunterAnimalDryMass, double huntedAnimalBodySize, double huntedAnimalVoracity, double huntedAnimalSpeed, double huntedAnimalDryMass, double muForPDF, double sigmaForPDF)
{
	sumMeanSizeHunted(huntedAnimalBodySize);
	sumMeanVorHunted(huntedAnimalVoracity);
	sumMeanSpeedHunted(huntedAnimalSpeed);
	sumMeanSizeHunter(hunterAnimalBodySize);
	sumMeanVorHunter(hunterAnimalVoracity);
	sumMeanSpeedHunter(hunterAnimalSpeed);
	double probabilityDensityFunction = exp(-0.5 * pow((log(hunterAnimalDryMass/huntedAnimalDryMass) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
	sumMeanProbabilityDensityFunction(probabilityDensityFunction);
	sumMeanVorXVor(hunterAnimalVoracity * huntedAnimalVoracity);

	//this is to avoid zeroes in the denominator
	if(huntedAnimalSpeed<0.001){

		sumMeanSpeedRatio(hunterAnimalSpeed / hunterAnimalSpeed);

	}else{

		sumMeanSpeedRatio(hunterAnimalSpeed / huntedAnimalSpeed);

	}

	//double test=hunterAnimal->getSpeed() / huntedAnimal->getSpeed();
	//if(std::isnan(test)){
	//if(huntedAnimal->getSpeed()<0.0000000001){
	/*if(huntedAnimal->getId()==24600){
		cout << "hunter vs hunted speed" << endl;
		cout << hunterAnimal->getSpeed() << endl;
		cout << "id: " << huntedAnimal->getId() << endl;
		cout << huntedAnimal->getSpeed() << endl;
		cout << "dry mass :"<< huntedAnimal->calculateDryMass() << endl;
		cout << huntedAnimal->speed_ini << endl;
		cout << huntedAnimal->getSpeed() << endl;
		cout << "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ" << endl;
	}*/

	sumMeanSizeRatio(hunterAnimalBodySize / huntedAnimalBodySize);
}

void AnimalSpecies::computeStatisticMeans(unsigned int numberOfAttacks)
{
	calculateMeanSizeHunted(numberOfAttacks);
	calculateMeanVorHunted(numberOfAttacks);
	calculateMeanSpeedHunted(numberOfAttacks);
	calculateMeanSizeHunter(numberOfAttacks);
	calculateMeanVorHunter(numberOfAttacks);
	calculateMeanSpeedHunter(numberOfAttacks);
	calculateMeanSearchAreaHunter(numberOfAttacks);
	calculateMeanProbabilityDensityFunction(numberOfAttacks);
	calculateMeanVorXVor(numberOfAttacks);
	calculateMeanSpeedRatio(numberOfAttacks);
	calculateMeanSizeRatio(numberOfAttacks);
}

void AnimalSpecies::sumStatisticSds(double hunterAnimalBodySize, double hunterAnimalVoracity, double hunterAnimalSpeed, double hunterAnimalDryMass, double huntedAnimalBodySize, double huntedAnimalVoracity, double huntedAnimalSpeed, double huntedAnimalDryMass, double muForPDF, double sigmaForPDF)
{
	sumSdSizeHunted(huntedAnimalBodySize);
	sumSdVorHunted(huntedAnimalVoracity);
	sumSdSpeedHunted(huntedAnimalSpeed);
	sumSdSizeHunter(hunterAnimalBodySize);
	sumSdVorHunter(hunterAnimalVoracity);
	sumSdSpeedHunter(hunterAnimalSpeed);
	double probabilityDensityFunction = exp(-0.5 * pow((log(hunterAnimalDryMass/huntedAnimalDryMass) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
	sumSdProbabilityDensityFunction(probabilityDensityFunction);
	sumSdVorXVor(hunterAnimalVoracity * huntedAnimalVoracity);
	sumSdSpeedRatio(hunterAnimalSpeed / huntedAnimalSpeed);
	sumSdSizeRatio(hunterAnimalBodySize / huntedAnimalBodySize);
}

void AnimalSpecies::computeStatisticSds(unsigned int numberOfAttacks)
{
	calculateSdSizeHunted(numberOfAttacks);
	calculateSdVorHunted(numberOfAttacks);
	calculateSdSpeedHunted(numberOfAttacks);
	calculateSdSizeHunter(numberOfAttacks);
	calculateSdVorHunter(numberOfAttacks);
	calculateSdSpeedHunter(numberOfAttacks);
	calculateSdSearchAreaHunter(numberOfAttacks);
	calculateSdProbabilityDensityFunction(numberOfAttacks);
	calculateSdVorXVor(numberOfAttacks);
	calculateSdSpeedRatio(numberOfAttacks);
	calculateSdSizeRatio(numberOfAttacks);
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

void AnimalSpecies::setTraits(const unordered_map<string,json>& traitsInfo)
{
	variableTraits.resize(traitsInfo.at("variableTraitsOrder").size());

	for(const auto& [traitName, traitInfo] : traitsInfo.at("definition").items()) {
		unique_ptr<Trait> newTrait = Trait::createInstance(traitName, traitInfo, traitsInfo.at("variableTraitsOrder"));

		switch(newTrait->getValueUpdateMethodType())
		{
			case Trait::ValueUpdateMethod::Fixed: {
				fixedTraits.push_back(newTrait->getType());
				break;
			}
			case Trait::ValueUpdateMethod::Variable: {
				variableTraits[static_cast<VariableTrait*>(newTrait.get())->getOrder()] = newTrait->getType();
				break;
			}
			default: {
				throwLineInfoException("Default case");
				break;
			}
		}

		Trait* newTraitPtr = newTrait.release();
		traits[newTraitPtr->getType()] = newTraitPtr;
	}
}

const Trait* const AnimalSpecies::getTrait(const Trait::Type name) const
{
	return traits[name];
}

void AnimalSpecies::resetLimits()
{
	for(size_t i = 0; i < getNumberOfVariableTraits(); i++)
	{
		setMinObservedPseudoValue(variableTraits[i], DBL_MAX);
		setMaxObservedPseudoValue(variableTraits[i], NEG_DBL_MAX);
	}
}

void AnimalSpecies::resetPseudoGrowthMean()
{
	pseudoGrowthMean = 0.0;
}

void AnimalSpecies::resetPseudoGrowthSd()
{
	pseudoGrowthSd = 0.0;
}

template <class Archive>
void AnimalSpecies::serialize(Archive &ar, const unsigned int version) {
	ar & boost::serialization::base_object<Species>(*this);

	ar & animalSpeciesId;
	ar & defaultHuntingMode;

	ar & edibleAnimalSpecies;
	ar & edibleResourceSpecies;

	ar & edibleOntogeneticLink;

	ar & edibleSearchParams;
	ar & breedSearchParams;
	ar & cellEvaluationSearchParams;
	ar & matureFemalesSearchParams;
	ar & populationSearchParams;
	ar & lifeStageSearchParams;

	ar & experienceInfluencePerDay;

	ar & predationEventsOnOtherSpecies;

	unsigned int numberOfTraits;
	if (Archive::is_loading::value)
	{
		ar & numberOfTraits;
		traits.resize(numberOfTraits);
	}
	else
	{
		numberOfTraits = traits.size();
		ar & numberOfTraits;
	}

	for(Trait*& newTrait : traits)
	{
		boost::serialization::serialize(ar, newTrait, version);
	}

	ar & fixedTraits;
	ar & variableTraits;

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
	ar & pupaPeriodLength;
	ar & minRelativeHumidityThreshold;

	ar & maxEncountersT;

	ar & minSizeHunted;
	ar & maxSizeHunted;
	ar & meanSizeHunted;
	ar & sdSizeHunted;
	ar & minVorHunted;
	ar & maxVorHunted;
	ar & meanVorHunted;
	ar & sdVorHunted;
	ar & minSpeedHunted;
	ar & maxSpeedHunted;
	ar & meanSpeedHunted;
	ar & sdSpeedHunted;

	ar & minSizeHunter;
	ar & maxSizeHunter;
	ar & meanSizeHunter;
	ar & sdSizeHunter;
	ar & minVorHunter;
	ar & maxVorHunter;
	ar & meanVorHunter;
	ar & sdVorHunter;
	ar & minSpeedHunter;
	ar & maxSpeedHunter;
	ar & meanSpeedHunter;
	ar & sdSpeedHunter;
	ar & minSearchAreaHunter;
	ar & maxSearchAreaHunter;
	ar & meanSearchAreaHunter;
	ar & sdSearchAreaHunter;

	ar & minProbabilityDensityFunction;
	ar & maxProbabilityDensityFunction;
	ar & meanProbabilityDensityFunction;
	ar & sdProbabilityDensityFunction;
	ar & minVorXVor;
	ar & maxVorXVor;
	ar & meanVorXVor;
	ar & sdVorXVor;
	ar & minSpeedRatio;
	ar & maxSpeedRatio;
	ar & meanSpeedRatio;
	ar & sdSpeedRatio;
	
	ar & meanSizeRatio;
	ar & sdSizeRatio;

	ar & maxPredationProbability;
	ar & maxPredationIndividualWetMass;

	ar & cellEvaluationBiomass;
	ar & cellEvaluationRisk;
	ar & cellEvaluationProConspecific;
	ar & cellEvaluationAntiConspecific;
	ar & conspecificWeighing;

	ar & devTimeVector;
	ar & vonBertLinf;
	ar & vonBertKini;
	ar & LinfKcorr;
	ar & devTimeConstant;
	ar & longevitySinceMaturation;
	ar & reproTimeFactor;
	ar & tempOptGrowth;
	ar & tempOptSearch;
	ar & tempOptVoracity;
	ar & tempOptSpeed;
	ar & EdGrowth;
	ar & EdSearch;
	ar & EdVoracity;
	ar & EdSpeed;
	ar & devInter;
	ar & fractSearchExtremeT;
	ar & fractSpeedExtremeT;
	ar & tempFromLab;
	ar & tempSizeRuleConstant;

	ar & eggClutchFromEquation;
	ar & forClutchMassCoefficient;
	ar & forClutchMassScale;
	ar & forEggMassCoefficient;
	ar & forEggMassScale;
	ar & eggMassFromEquation;
	ar & eggDryMass;
	ar & femaleWetMass;

	ar & scaleForVoracity;
	ar & scaleForSearchArea;
	ar & scaleForSpeed;
	ar & maxPlasticityKVonBertalanffy;
	ar & minPlasticityKVonBertalanffy;
	ar & maxPlasticityDueToConditionVor;
	ar & minPlasticityDueToConditionVor;
	ar & maxPlasticityDueToConditionSearch;
	ar & minPlasticityDueToConditionSearch;
	ar & maxPlasticityDueToConditionSpeed;
	ar & minPlasticityDueToConditionSpeed;

	ar & attackProbability;
	ar & exposedAttackProbability;
	ar & killProbability;
	ar & optimalTemperatureModifier;
	ar & daysWithoutFoodForMetabolicDownregulation;
	ar & percentageMetabolicDownregulation;
	ar & percentageCostForMetabolicDownregulationVoracity;
	ar & percentageCostForMetabolicDownregulationSearchArea;
	ar & percentageCostForMetabolicDownregulationSpeed;

	ar & normalDistributionX;
	ar & normalDistributionY;
	ar & cholMat;
	ar & pseudoGrowthMean;
	ar & pseudoGrowthSd;

	ar & coefficientForMassA;
	ar & scaleForMassB;

	ar & maximumDryMassObserved;

	ar & coefficientForMassAforMature;
	ar & scaleForMassBforMature;

	ar & forDensitiesA;
	ar & forDensitiesB;
	ar & indeterminateGrowth;
	ar & instarFirstReproduction;
	ar & instarsForNextReproduction;
	ar & sexualType;
	boost::serialization::serialize(ar, growthCurve, version);
	ar & sexRatio;
	ar & size;
	ar & femaleMaxReproductionEvents;
	ar & eggsPerBatch;
	ar & maleMaxReproductionEvents;
	ar & maleReproductionFactor;
	ar & maleMobility;
	ar & surviveWithoutFood;
	ar & decreaseOnTraitsDueToEncounters;
	ar & probabilityDeathFromBackground;
	ar & involvedResourceSpecies;

	ar & habitatShiftBeforeBreeding;
	ar & habitatShiftAfterBreeding;

	ar & habitatShiftBeforeBreedingFactor;
	ar & habitatShiftAfterBreedingFactor;

	ar & habitatShiftPerInstar;
	ar & habitatShiftFactor;

	ar & cellSizePerInstar;
	ar & cellDepthPerInstar;

	ar & forcePresenceAllResourcesInvolved;
	ar & preserveLeftovers;

	ar & loci;
	//TODO This has room for efficiency improvements. We could just save the final calculations of the actual positions.
	ar & randomlyCreatedPositionsForChromosomes;
}



namespace boost {
    namespace serialization {
		template <class Archive>
        void serialize(Archive &ar, AnimalSpecies* &animalSpeciesPtr, const unsigned int version, WorldInterface* const newWorldInterface) {
            // For loading
            if (Archive::is_loading::value) {
				animalSpeciesPtr = new AnimalSpecies(newWorldInterface);
            }

			animalSpeciesPtr->serialize(ar, version);
        }

		// Specialisation
		template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, AnimalSpecies*&, const unsigned int, WorldInterface* const);
		template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, AnimalSpecies*&, const unsigned int, WorldInterface* const);

		template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, AnimalSpecies*&, const unsigned int, WorldInterface* const);
		template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, AnimalSpecies*&, const unsigned int, WorldInterface* const);
	}
}

namespace boost {
	namespace serialization {
		template<class Archive>
		void serialize(Archive & ar, normal_distribution<double>& normalDist, const unsigned int version)
		{
			double mean;
			double stddev;

			if (Archive::is_loading::value)
			{
				ar & mean;
				ar & stddev;

				normalDist = normal_distribution<double>(mean, stddev);
			}
			else
			{
				mean = normalDist.mean();
				stddev = normalDist.stddev();

				ar & mean;
				ar & stddev;
			}
		}
	}
}
