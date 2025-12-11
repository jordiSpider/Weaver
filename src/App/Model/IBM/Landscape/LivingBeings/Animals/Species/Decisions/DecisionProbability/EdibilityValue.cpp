
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/EdibilityValue.h"


#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"


using namespace std;
using json = nlohmann::json;


EdibilityValue::EdibilityValue()
    : EdibleDecisionProbability()
{

}

EdibilityValue::EdibilityValue(const json& config, const AnimalSpeciesGenetics& genetics, const PredationProbability* predationProbability)
    : EdibleDecisionProbability(config, genetics.getEdibilityValueWeightTraits()), predationProbability(predationProbability),
      qualityResourceAssessment(config["qualityResourceAssessment"].get<bool>())
{
    initProbabilityFunctions();
}

EdibilityValue::~EdibilityValue()
{
    
}


PreciseDouble EdibilityValue::calculateValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const
{
    try
    {
        return DecisionProbability::calculateValue(predator, prey, preyDryMass) * predator.getPreference(prey.getSpecies()->getId(), prey.getGrowthBuildingBlock().getInstar());
    }
    catch(const LineInfoException&)
    {
        return 0.0;
    }
}


void EdibilityValue::setPredationProbability(const PredationProbability* newPredationProbability)
{
    predationProbability = newPredationProbability;
}


void EdibilityValue::initProbabilityFunctions()
{
    setProbabilityFunction(
        static_cast<size_t>(EdibilityValueWeightType::Pp),
        [this](const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) { 
            return calculatePredationProbability(predator, prey, preyDryMass); 
        }
    );

    setRemainingProbabilityFunction(
        [this](const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) { 
            return calculateRelativeResourceValue(predator, prey, preyDryMass); 
        }
    );
}


const std::vector<IndividualTrait>& EdibilityValue::getWeightIndividualTraits(const AnimalNonStatistical& predator) const
{
    return predator.getGenetics().getEdibilityValueWeightIndividualTraits();
}


PreciseDouble EdibilityValue::calculatePredationProbability(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const
{
    const PreciseDouble value = predationProbability->calculateValue(predator, prey, preyDryMass);

    if(value <= 0.0)
    {
        throwLineInfoException("Predation probability is zero.");
    }

    return value;
}


PreciseDouble EdibilityValue::calculateRelativeResourceValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const
{
    DryMass relativeResourceValue;

    if(getQualityResourceAssessment())
    {
        relativeResourceValue = predator.calculateAssimilatedMass(preyDryMass, prey.getSpecies()->getId(), prey.getGrowthBuildingBlock().getInstar());
    }
    else
    {
        relativeResourceValue = preyDryMass;
    }

    return relativeResourceValue.getValue() / (relativeResourceValue + predator.getVoracity()).getValue();
}


bool EdibilityValue::getQualityResourceAssessment() const
{
    return qualityResourceAssessment;
}



BOOST_CLASS_EXPORT(EdibilityValue)

template <class Archive>
void EdibilityValue::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<EdibleDecisionProbability>(*this);

    ar & qualityResourceAssessment;
}

// Specialisation
template void EdibilityValue::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void EdibilityValue::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void EdibilityValue::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void EdibilityValue::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
