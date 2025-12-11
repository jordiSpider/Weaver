
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/AnimalSpeciesDecisions.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"


using namespace std;
using json = nlohmann::json;


AnimalSpeciesDecisions::AnimalSpeciesDecisions()
{
    
}

AnimalSpeciesDecisions::AnimalSpeciesDecisions(const json& config, const AnimalSpeciesGenetics& genetics, const unsigned int numberOfInstars, const PreciseDouble& pdfThreshold)
    : sensoryModel(config["sensoryModel"]),
      predationProbability(config["predationProbability"], genetics, numberOfInstars, pdfThreshold, &sensoryModel), 
      edibilityValue(config["edibilityValue"], genetics, &predationProbability),
      cellQuality(&edibilityValue),
      maximumVoracity(numberOfInstars, 0.0),
      maximumPredatorInteractionArea(numberOfInstars, 0.0),
      maximumPreyVoracity(numberOfInstars, 0.0),
      killProbability(config["decisionWithinPatch"]["killProbability"].get<double>()),
      weightIndividualToGlobalAssessment(config["decisionAmongPatches"]["weightIndividualToGlobalAssessment"].get<double>()),
      maximumPatchEdibilityValueGlobal(numberOfInstars, 0.0),
	  maximumPatchPredationRiskGlobal(numberOfInstars, 0.0),
	  maximumPatchConspecificBiomassGlobal(numberOfInstars, 0.0)
{
    
}

AnimalSpeciesDecisions::~AnimalSpeciesDecisions()
{
    
}



void AnimalSpeciesDecisions::deserializeProbabilityFunctions()
{
    predationProbability.deserializeProbabilityFunctions();
    edibilityValue.deserializeProbabilityFunctions();
}



void AnimalSpeciesDecisions::updateMaximumVoracity(const Instar& instar, const PreciseDouble& newValue)
{
    maximumVoracity[instar] = fmax(maximumVoracity[instar], newValue);
}


void AnimalSpeciesDecisions::updateMaximumPDF(const Instar& instar, const PreciseDouble& newValue)
{
    predationProbability.updateMaximumPDF(instar, newValue);
}


const PreciseDouble& AnimalSpeciesDecisions::getMaximumVoracity(const Instar& instar) const
{
    return maximumVoracity[instar];
}


const PreciseDouble& AnimalSpeciesDecisions::getKillProbability() const 
{ 
    return killProbability; 
}


const PreciseDouble& AnimalSpeciesDecisions::getWeightIndividualToGlobalAssessment() const
{
    return weightIndividualToGlobalAssessment;
}


const PreciseDouble& AnimalSpeciesDecisions::getMaximumPatchEdibilityValueGlobal(const Instar& instar) const
{
    return maximumPatchEdibilityValueGlobal[instar];
}

const PreciseDouble& AnimalSpeciesDecisions::getMaximumPatchPredationRiskGlobal(const Instar& instar) const
{
    return maximumPatchPredationRiskGlobal[instar];
}

const PreciseDouble& AnimalSpeciesDecisions::getMaximumPatchConspecificBiomassGlobal(const Instar& instar) const
{
    return maximumPatchConspecificBiomassGlobal[instar];
}


void AnimalSpeciesDecisions::setMaximumPatchEdibilityValueGlobal(const Instar& instar, const PreciseDouble& newValue)
{
    maximumPatchEdibilityValueGlobal[instar] = fmax(maximumPatchEdibilityValueGlobal[instar], newValue);
}

void AnimalSpeciesDecisions::setMaximumPatchPredationRiskGlobal(const Instar& instar, const PreciseDouble& newValue)
{
    maximumPatchPredationRiskGlobal[instar] = fmax(maximumPatchPredationRiskGlobal[instar], newValue);
}

void AnimalSpeciesDecisions::setMaximumPatchConspecificBiomassGlobal(const Instar& instar, const PreciseDouble& newValue)
{
    maximumPatchConspecificBiomassGlobal[instar] = fmax(maximumPatchConspecificBiomassGlobal[instar], newValue);
}


PreciseDouble AnimalSpeciesDecisions::calculatePredationProbability(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass)
{
    return predationProbability.calculateValue(predator, prey, preyDryMass);
}


PreciseDouble AnimalSpeciesDecisions::calculateEdibilityValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass)
{
    return edibilityValue.calculateValue(predator, prey, preyDryMass);
}


PreciseDouble AnimalSpeciesDecisions::calculateCellQuality(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass)
{
    return cellQuality.calculateValue(predator, prey, preyDryMass);
}

void AnimalSpeciesDecisions::updateMaximumPredatorInteractionArea(const Instar& instar, const PreciseDouble& newValue)
{
    maximumPredatorInteractionArea[instar] = fmax(maximumPredatorInteractionArea[instar], newValue);
}

void AnimalSpeciesDecisions::updateMaximumPreyVoracity(const Instar& instar, const PreciseDouble& newValue)
{
    maximumPreyVoracity[instar] = fmax(maximumPreyVoracity[instar], newValue);
}

const PreciseDouble& AnimalSpeciesDecisions::getMaximumPredatorInteractionArea(const Instar& instar) const
{
    return maximumPredatorInteractionArea[instar];
}

const PreciseDouble& AnimalSpeciesDecisions::getMaximumPreyVoracity(const Instar& instar) const
{
    return maximumPreyVoracity[instar];
}

bool AnimalSpeciesDecisions::getQualityResourceAssessment() const
{
    return edibilityValue.getQualityResourceAssessment();
}

const SensoryModel& AnimalSpeciesDecisions::getSensoryModel() const
{
    return sensoryModel;
}



template <class Archive>
void AnimalSpeciesDecisions::serialize(Archive &ar, const unsigned int) {
    ar & sensoryModel;

    ar & predationProbability;
    ar & edibilityValue;
    ar & cellQuality;

    if (Archive::is_loading::value) {
		edibilityValue.setPredationProbability(&predationProbability);
        cellQuality.setEdibilityValue(&edibilityValue);
	}

    ar & maximumVoracity;

    ar & maximumPredatorInteractionArea;

    ar & maximumPreyVoracity;

    ar & killProbability;

    ar & weightIndividualToGlobalAssessment;

    ar & maximumPatchEdibilityValueGlobal;
	ar & maximumPatchPredationRiskGlobal;
	ar & maximumPatchConspecificBiomassGlobal;
}

// Specialisation
template void AnimalSpeciesDecisions::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalSpeciesDecisions::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalSpeciesDecisions::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalSpeciesDecisions::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
