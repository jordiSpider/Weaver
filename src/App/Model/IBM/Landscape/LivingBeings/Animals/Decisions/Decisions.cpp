#include "App/Model/IBM/Landscape/LivingBeings/Animals/Decisions/Decisions.h"

#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResourceInterface.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"
#include "App/Model/IBM/Landscape/Landscape.h"


using namespace std;






Decisions::Decisions()
{

}

Decisions::Decisions(AnimalNonStatistical* owner, AnimalSpeciesDecisions* animalSpeciesDecisions, const PreciseDouble& timeStepsPerDay)
	: owner(owner), animalSpeciesDecisions(animalSpeciesDecisions), 
	  lastAssimilationEfficiency(0.0), 
	  maximumCumulativePredationProbabilityCurrentTimeStep(0.0), 
	  lastCumulativePredationProbability(0.0),
	  maximumPatchEdibilityValueIndividual(0.0),
	  maximumPatchPredationRiskIndividual(0.0),
	  maximumPatchConspecificBiomassIndividual(0.0)
{
	TimeStep memoryDepth(Day(owner->getGenetics().getBaseIndividualTraits(BaseTraitType::memoryDepth).getPhenotypicValue()), timeStepsPerDay);

	assimilationEfficiencyBuffer = RingBuffer<PreciseDouble>(memoryDepth.getValue(), 0.0);
	cumulativePredationProbabilityBuffer = RingBuffer<PreciseDouble>(memoryDepth.getValue(), 0.0);

	setInitialPreferences(timeStepsPerDay);
}

Decisions::~Decisions()
{
	
}





void Decisions::setAnimalSpeciesDecisions(AnimalSpeciesDecisions* newAnimalSpeciesDecisions)
{
	animalSpeciesDecisions = newAnimalSpeciesDecisions;
}

void Decisions::setOwner(AnimalNonStatistical* newOwner) {
	owner = newOwner;
}


PreciseDouble Decisions::calculatePredationProbability(const AnimalNonStatistical& prey)
{
	return calculatePredationProbability(prey, prey.getGrowthBuildingBlock().getCurrentTotalDryMass());
}


PreciseDouble Decisions::calculatePredationProbability(const Edible& prey, const DryMass& preyDryMass)
{
	return animalSpeciesDecisions->calculatePredationProbability(*owner, prey, preyDryMass);
}


PreciseDouble Decisions::calculatePredationRisk(AnimalNonStatistical& predator) const
{
	return predator.calculatePredationProbability(*owner);
}


PreciseDouble Decisions::calculateEdibilityValue(const AnimalNonStatistical& prey)
{
	return animalSpeciesDecisions->calculateEdibilityValue(*owner, prey, prey.getGrowthBuildingBlock().getCurrentTotalDryMass());
}


PreciseDouble Decisions::calculateEdibilityValue(const CellResourceInterface& prey, const DryMass& preyDryMass)
{
	return animalSpeciesDecisions->calculateEdibilityValue(*owner, prey, preyDryMass);
}


PreciseDouble Decisions::calculateCellQuality(const AnimalNonStatistical& prey)
{
	return animalSpeciesDecisions->calculateCellQuality(*owner, prey, prey.getGrowthBuildingBlock().getCurrentTotalDryMass());
}


PreciseDouble Decisions::calculateCellQuality(const CellResourceInterface& prey, const DryMass& preyDryMass)
{
	return animalSpeciesDecisions->calculateCellQuality(*owner, prey, preyDryMass);
}


DryMass Decisions::calculateConspecificBiomass(const AnimalNonStatistical& otherAnimal) const
{
	return otherAnimal.getGrowthBuildingBlock().getCurrentTotalDryMass();
}


void Decisions::setInitialPreferences(const PreciseDouble& timeStepsPerDay)
{
	TimeStep memoryDepth(Day(owner->getGenetics().getBaseIndividualTraits(BaseTraitType::memoryDepth).getPhenotypicValue()), timeStepsPerDay);

	const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, OntogeneticLink>>* const ontogeneticLinks = &owner->getSpecies()->getOntogeneticLinksPerInstar(owner->getGrowthBuildingBlock().getInstar());

	experienceBuffer.resize(ontogeneticLinks->size());
	preferences.resize(ontogeneticLinks->size());

	for(size_t speciesId = 0; speciesId < ontogeneticLinks->size(); speciesId++)
	{
		experienceBuffer.getValue(speciesId).resize(ontogeneticLinks->getValue(speciesId).size());
		preferences.getValue(speciesId).resize(ontogeneticLinks->getValue(speciesId).size());

		for(size_t preyInstar = 0; preyInstar < ontogeneticLinks->getValue(speciesId).size(); preyInstar++)
		{
			experienceBuffer.getValue(speciesId).getValue(preyInstar) = RingBuffer<PreciseDouble>(memoryDepth.getValue(), 0.0);
			preferences.getValue(speciesId).getValue(preyInstar) = ontogeneticLinks->getValue(speciesId).getValue(preyInstar).getPreference();
		}
	}
}


void Decisions::updatePreferences()
{
	const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, OntogeneticLink>>* const previousInstarLinks = &owner->getSpecies()->getOntogeneticLinksPerInstar(owner->getGrowthBuildingBlock().getInstar() - 1);
	const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, OntogeneticLink>>* const currentInstarLinks = &owner->getSpecies()->getOntogeneticLinksPerInstar(owner->getGrowthBuildingBlock().getInstar());

	for(size_t speciesId = 0; speciesId < preferences.size(); speciesId++)
	{
		for(size_t preyInstar = 0; preyInstar < preferences.getValue(speciesId).size(); preyInstar++)
		{
			if(
				(previousInstarLinks->getValue(speciesId).getValue(preyInstar).getPreference() == 0.0 || currentInstarLinks->getValue(speciesId).getValue(preyInstar).getPreference() == 0.0) &&
				(previousInstarLinks->getValue(speciesId).getValue(preyInstar).getPreference() != currentInstarLinks->getValue(speciesId).getValue(preyInstar).getPreference())
			)
			{
				experienceBuffer.getValue(speciesId).getValue(preyInstar).clear();
				preferences.getValue(speciesId).getValue(preyInstar) = currentInstarLinks->getValue(speciesId).getValue(preyInstar).getPreference();
			}
		}
	}
}


void Decisions::tryToEatEdible(Landscape* const landscape, vector<tuple<PreciseDouble, Edible*, DryMass>>& ediblesByEdibility, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, list<const AnimalNonStatistical*> &animalsHasTriedToPredate, const bool saveAnimalsEachDayPredationProbabilities, ostringstream& predationProbabilitiesContent, const bool saveActivity, ostringstream& activityContent, const bool competitionAmongResourceSpecies)
{
	auto ediblesIt = ediblesByEdibility.begin();

	Edible* prey = get<1>(*ediblesIt);
	DryMass preyDryMass = get<2>(*ediblesIt);


	PreciseDouble randomKillProbability = Random::randomUniform();

	owner->predateEdible(randomKillProbability, animalSpeciesDecisions->getKillProbability(), false, false, landscape, *prey, preyDryMass, numberOfTimeSteps, timeStepsPerDay, animalsHasTriedToPredate, saveAnimalsEachDayPredationProbabilities, predationProbabilitiesContent, saveActivity, activityContent, competitionAmongResourceSpecies);

	ediblesByEdibility.erase(ediblesIt);
}


const PreciseDouble& Decisions::getPreference(const Species::ID &preySpeciesId, const Instar &preyInstar) const
{
	return preferences.at(preySpeciesId).at(preyInstar);
}


PreciseDouble Decisions::getMeanExperience(const Species::ID &preySpeciesId, const Instar &preyInstar) const
{
	return experienceBuffer.at(preySpeciesId).at(preyInstar).calculateMean();
}


bool Decisions::isLastAssimilationEfficiencyLowerThanMean() const
{
	return lastAssimilationEfficiency < assimilationEfficiencyBuffer.calculateMean();
} 


bool Decisions::isLastCumulativePredationProbabilityUpperThanMean() const
{
	return lastCumulativePredationProbability > cumulativePredationProbabilityBuffer.calculateMean();
}


void Decisions::updateVariablesAssociatedWithInstar()
{
	resetMaximumPatchValuesIndividual();

	updatePreferences();
}


void Decisions::addSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay)
{
	TimeStep memoryDepth(Day(owner->getGenetics().getBaseIndividualTraits(BaseTraitType::memoryDepth).getPhenotypicValue()), timeStepsPerDay);

	experienceBuffer.emplace_back(numberOfInstars, RingBuffer<PreciseDouble>(memoryDepth.getValue(), 0.0));
	preferences.emplace_back(numberOfInstars, 0.0);
}


void Decisions::setLastIngestion(
		const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, DryMass>>& foodMassAssimilatedCurrentTimeStepPerSpecies, 
        const DryMass& foodMassAssimilatedCurrentTimeStep,
	    const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, DryMass>>& foodMassEatenCurrentTimeStepPerSpecies, 
        const DryMass& foodMassEatenCurrentTimeStep
	)
{
	if(foodMassEatenCurrentTimeStep.getValue() > 0.0)
	{
		lastAssimilationEfficiency = foodMassAssimilatedCurrentTimeStep.getValue() / foodMassEatenCurrentTimeStep.getValue();
	}
	else
	{
		lastAssimilationEfficiency = 0.0;
	}

	assimilationEfficiencyBuffer.push(lastAssimilationEfficiency);

	updatePreferences(
		foodMassAssimilatedCurrentTimeStepPerSpecies, foodMassAssimilatedCurrentTimeStep,
		foodMassEatenCurrentTimeStepPerSpecies, foodMassEatenCurrentTimeStep
	);
}


void Decisions::updatePreferences(
		const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, DryMass>>& foodMassAssimilatedCurrentTimeStepPerSpecies, 
        const DryMass& foodMassAssimilatedCurrentTimeStep,
	    const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, DryMass>>& foodMassEatenCurrentTimeStepPerSpecies, 
        const DryMass& foodMassEatenCurrentTimeStep
	)
{
	for(size_t speciesId = 0; speciesId < preferences.size(); speciesId++)
	{
		for(size_t preyInstar = 0; preyInstar < preferences.getValue(speciesId).size(); preyInstar++)
		{
			PreciseDouble experience;

			if(foodMassEatenCurrentTimeStep.getValue() > 0.0)
			{
				if(animalSpeciesDecisions->getQualityResourceAssessment())
				{
					experience = foodMassAssimilatedCurrentTimeStepPerSpecies.getValue(speciesId).getValue(preyInstar).getValue() / foodMassAssimilatedCurrentTimeStep.getValue();
				}
				else
				{
					experience = foodMassEatenCurrentTimeStepPerSpecies.getValue(speciesId).getValue(preyInstar).getValue() / foodMassEatenCurrentTimeStep.getValue();
				}
			}
			else
			{
				experience = 0.0;
			}

			experienceBuffer.getValue(speciesId).getValue(preyInstar).push(experience);

			const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, OntogeneticLink>>* const ontogeneticLinks = &owner->getSpecies()->getOntogeneticLinksPerInstar(owner->getGrowthBuildingBlock().getInstar());

			const PreciseDouble experienceInfluenceWithEdibles = owner->getGenetics().getPreferencesIndividualTraits(PreferencesTraitType::experienceInfluenceWithEdibles).getPhenotypicValue();

			preferences.getValue(speciesId).getValue(preyInstar) = experienceBuffer.getValue(speciesId).getValue(preyInstar).calculateMean() * experienceInfluenceWithEdibles + ontogeneticLinks->getValue(speciesId).getValue(preyInstar).getPreference() * (1.0 - experienceInfluenceWithEdibles);
		}
	}
}


void Decisions::setLastCumulativePredationProbability()
{
	cumulativePredationProbabilityBuffer.push(fmax(maximumCumulativePredationProbabilityCurrentTimeStep, lastCumulativePredationProbability));

	maximumCumulativePredationProbabilityCurrentTimeStep = 0.0;
	lastCumulativePredationProbability = 0.0;
}


void Decisions::addToCumulativePredationProbability(const PreciseDouble& predationProbability)
{
	lastCumulativePredationProbability += predationProbability;
}


void Decisions::setNewDestination()
{
	maximumCumulativePredationProbabilityCurrentTimeStep = fmax(maximumCumulativePredationProbabilityCurrentTimeStep, lastCumulativePredationProbability);
	lastCumulativePredationProbability = 0.0;
}


const PreciseDouble& Decisions::getMaximumPatchEdibilityValueIndividual() const
{
	return maximumPatchEdibilityValueIndividual;
}

const PreciseDouble& Decisions::getMaximumPatchPredationRiskIndividual() const
{
	return maximumPatchPredationRiskIndividual;
}

const PreciseDouble& Decisions::getMaximumPatchConspecificBiomassIndividual() const
{
	return maximumPatchConspecificBiomassIndividual;
}


PreciseDouble Decisions::getMaximumPatchEdibilityValue() const
{
	return animalSpeciesDecisions->getWeightIndividualToGlobalAssessment() * getMaximumPatchEdibilityValueIndividual() + 
		(1.0 - animalSpeciesDecisions->getWeightIndividualToGlobalAssessment()) * animalSpeciesDecisions->getMaximumPatchEdibilityValueGlobal(owner->getGrowthBuildingBlock().getInstar());
}

PreciseDouble Decisions::getMaximumPatchPredationRisk() const
{
	return animalSpeciesDecisions->getWeightIndividualToGlobalAssessment() * getMaximumPatchPredationRiskIndividual() + 
		(1.0 - animalSpeciesDecisions->getWeightIndividualToGlobalAssessment()) * animalSpeciesDecisions->getMaximumPatchPredationRiskGlobal(owner->getGrowthBuildingBlock().getInstar());
}

PreciseDouble Decisions::getMaximumPatchConspecificBiomass() const
{
	return animalSpeciesDecisions->getWeightIndividualToGlobalAssessment() * getMaximumPatchConspecificBiomassIndividual() + 
		(1.0 - animalSpeciesDecisions->getWeightIndividualToGlobalAssessment()) * animalSpeciesDecisions->getMaximumPatchConspecificBiomassGlobal(owner->getGrowthBuildingBlock().getInstar());
}

void Decisions::setMaximumPatchEdibilityValueIndividual(const PreciseDouble& newValue)
{
	maximumPatchEdibilityValueIndividual = fmax(maximumPatchEdibilityValueIndividual, newValue);

	animalSpeciesDecisions->setMaximumPatchEdibilityValueGlobal(owner->getGrowthBuildingBlock().getInstar(), getMaximumPatchEdibilityValueIndividual());
}

void Decisions::setMaximumPatchPredationRiskIndividual(const PreciseDouble& newValue)
{
	maximumPatchPredationRiskIndividual = fmax(maximumPatchPredationRiskIndividual, newValue);

	animalSpeciesDecisions->setMaximumPatchPredationRiskGlobal(owner->getGrowthBuildingBlock().getInstar(), getMaximumPatchPredationRiskIndividual());
}

void Decisions::setMaximumPatchConspecificBiomassIndividual(const PreciseDouble& newValue)
{
	maximumPatchConspecificBiomassIndividual = fmax(maximumPatchConspecificBiomassIndividual, newValue);

	animalSpeciesDecisions->setMaximumPatchConspecificBiomassGlobal(owner->getGrowthBuildingBlock().getInstar(), getMaximumPatchConspecificBiomassIndividual());
}

void Decisions::resetMaximumPatchValuesIndividual()
{
	maximumPatchEdibilityValueIndividual = 0.0;
	maximumPatchPredationRiskIndividual = 0.0;
	maximumPatchConspecificBiomassIndividual = 0.0;
}


template <class Archive>
void Decisions::serialize(Archive &ar, const unsigned int) 
{
	ar & assimilationEfficiencyBuffer;
	ar & lastAssimilationEfficiency;

	ar & cumulativePredationProbabilityBuffer;
	ar & maximumCumulativePredationProbabilityCurrentTimeStep;
	ar & lastCumulativePredationProbability;

	ar & experienceBuffer;

	ar & preferences;

	ar & maximumPatchEdibilityValueIndividual;
	ar & maximumPatchPredationRiskIndividual;
	ar & maximumPatchConspecificBiomassIndividual;
}

// Specialisation
template void Decisions::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Decisions::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Decisions::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Decisions::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
