
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/PredationProbability.h"


#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"


using namespace std;
using json = nlohmann::json;


PredationProbability::PredationProbability()
    : EdibleDecisionProbability()
{

}

PredationProbability::PredationProbability(const json& config, const AnimalSpeciesGenetics& genetics, const unsigned int numberOfInstars, const PreciseDouble& pdfThreshold, const SensoryModel* const sensoryModel)
    : EdibleDecisionProbability(config, genetics.getPredationProbabilityWeightTraits()), 
      escapeProbability(config["escapeProbability"], genetics, sensoryModel), maximumPDF(numberOfInstars, 0.0),
      pdfThreshold(pdfThreshold), useGlobalMaximumPreyVoracity(config["useGlobalMaximumPreyVoracity"].get<bool>())
{
    initProbabilityFunctions();
}

PredationProbability::~PredationProbability()
{
    
}


void PredationProbability::deserializeProbabilityFunctions()
{
    DecisionProbability::deserializeProbabilityFunctions();

    escapeProbability.deserializeProbabilityFunctions();
}


void PredationProbability::initProbabilityFunctions()
{
    setProbabilityFunction(
        static_cast<size_t>(PredationProbabilityWeightType::Preach),
        [this](const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) { 
            return calculateReachProbability(predator, static_cast<const AnimalNonStatistical&>(prey), preyDryMass); 
        }
    );

    setProbabilityFunction(
        static_cast<size_t>(PredationProbabilityWeightType::Ppdf),
        [this](const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) { 
            return calculateInterpolatedProbabilityDensityFunction(predator, static_cast<const AnimalNonStatistical&>(prey), preyDryMass); 
        }
    );

    setProbabilityFunction(
        static_cast<size_t>(PredationProbabilityWeightType::PvorPred),
        [this](const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) { 
            return calculateVoracityPredatorProbability(predator, static_cast<const AnimalNonStatistical&>(prey), preyDryMass); 
        }
    );

    setRemainingProbabilityFunction(
        [this](const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) { 
            return calculateVoracityPreyProbability(predator, static_cast<const AnimalNonStatistical&>(prey), preyDryMass); 
        }
    );
}


const std::vector<IndividualTrait>& PredationProbability::getWeightIndividualTraits(const AnimalNonStatistical& predator) const
{
    return predator.getGenetics().getPredationProbabilityWeightIndividualTraits();
}


PreciseDouble PredationProbability::calculateValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const
{
    if(prey.getSpecies()->isMobile())
    {
        try
        {
            return DecisionProbability::calculateValue(predator, prey, preyDryMass);
        }
        catch(const LineInfoException&)
        {
            return 0.0;
        }
    }
    else
    {
        return 1.0;
    }
}


PreciseDouble PredationProbability::calculateReachProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass& preyDryMass)
{
    return 1.0 - escapeProbability.calculateValue(predator, prey, preyDryMass);
}

PreciseDouble PredationProbability::calculateInterpolatedProbabilityDensityFunction(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass&)
{
    PreciseDouble probabilityDensityFunction = PredationProbability::calculateProbabilityDensityFunction(predator, prey);

    if(probabilityDensityFunction < getPdfThreshold())
    {
        throwLineInfoException("Below the threshold.");
    }

    updateMaximumPDF(predator.getInstarToEvaluateCells(), probabilityDensityFunction);

    return probabilityDensityFunction / getMaximumPDF(predator.getInstarToEvaluateCells());
}

PreciseDouble PredationProbability::calculateVoracityPredatorProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical&, const DryMass&)
{
	return predator.getVoracity() / predator.getSpecies()->getDecisionsBuildingBlock()->getMaximumVoracity(predator.getGrowthBuildingBlock().getInstar());
}

PreciseDouble PredationProbability::calculateVoracityPreyProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass&)
{
    PreciseDouble maximumVoracity;

    if(useGlobalMaximumPreyVoracity)
    {
        maximumVoracity = predator.getSpecies()->getDecisionsBuildingBlock()->getMaximumPreyVoracity(predator.getGrowthBuildingBlock().getInstar());
    }
    else
    {
        maximumVoracity = prey.getSpecies()->getDecisionsBuildingBlock()->getMaximumVoracity(prey.getGrowthBuildingBlock().getInstar());
    }

    return prey.getVoracity() / maximumVoracity;
}



PreciseDouble PredationProbability::calculateProbabilityDensityFunction(const Animal& predator, const Animal& prey)
{
    DryMass predatorDryMass, preyDryMass;

    if(predator.getGrowthBuildingBlock().getInstar() != predator.getInstarToEvaluateCells())
    {
        predatorDryMass = predator.getSpecies()->getGrowthBuildingBlock().getEggDryMass() + predator.getSpecies()->getGrowthBuildingBlock().getEggDryMass().getValue()*predator.getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue();
    }
    else
    {
        predatorDryMass = predator.getGrowthBuildingBlock().getCurrentTotalDryMass();
    }

    if(prey.getGrowthBuildingBlock().getInstar() != prey.getInstarToEvaluateCells())
    {
        preyDryMass = prey.getSpecies()->getGrowthBuildingBlock().getEggDryMass() + prey.getSpecies()->getGrowthBuildingBlock().getEggDryMass().getValue()*prey.getGenetics().getBaseIndividualTraits(BaseTraitType::factorEggMass).getPhenotypicValue();
    }
    else
    {
        preyDryMass = prey.getGrowthBuildingBlock().getCurrentTotalDryMass();
    }


    PreciseDouble muForPDF = predator.getGenetics().getProbabilityDensityFunctionIndividualTraits(ProbabilityDensityFunctionTraitType::muForPDF).getPhenotypicValue();
    PreciseDouble sigmaForPDF = predator.getGenetics().getProbabilityDensityFunctionIndividualTraits(ProbabilityDensityFunctionTraitType::sigmaForPDF).getPhenotypicValue();


    return exp(-0.5 * pow((log(predatorDryMass.getValue()/preyDryMass.getValue()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
}



void PredationProbability::updateMaximumPDF(const Instar& instar, const PreciseDouble& newValue)
{
    maximumPDF[instar] = fmax(maximumPDF[instar], newValue);
}

const PreciseDouble& PredationProbability::getMaximumPDF(const Instar& instar) const
{
    return maximumPDF[instar];
}

const PreciseDouble& PredationProbability::getPdfThreshold() const
{
    return pdfThreshold;
}



BOOST_CLASS_EXPORT(PredationProbability)

template <class Archive>
void PredationProbability::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<EdibleDecisionProbability>(*this);

    ar & escapeProbability;
    
    ar & maximumPDF;

    ar & pdfThreshold;

    ar & useGlobalMaximumPreyVoracity;
}

// Specialisation
template void PredationProbability::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void PredationProbability::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void PredationProbability::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void PredationProbability::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
