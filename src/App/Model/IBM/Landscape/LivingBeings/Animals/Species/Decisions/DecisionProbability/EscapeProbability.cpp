
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/EscapeProbability.h"


#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"


using namespace std;
using json = nlohmann::json;


EscapeProbability::EscapeProbability()
    : AnimalDecisionProbability()
{

}

EscapeProbability::EscapeProbability(const json& config, const AnimalSpeciesGenetics& genetics, const SensoryModel* const sensoryModel)
    : AnimalDecisionProbability(config, genetics.getEscapeProbabilityWeightTraits()), sensoryModel(sensoryModel),
      cVelocity(config["cVelocity"].get<double>())
{
    initProbabilityFunctions();
}

EscapeProbability::~EscapeProbability()
{
    
}


void EscapeProbability::initProbabilityFunctions()
{
    setProbabilityFunction(
        static_cast<size_t>(EscapeProbabilityWeightType::Pvelocity),
        [this](const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass& preyDryMass) { 
            return calculateVelocityProbability(predator, prey, preyDryMass); 
        }
    );

    setProbabilityFunction(
        static_cast<size_t>(EscapeProbabilityWeightType::PattackDistance),
        [this](const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass& preyDryMass) { 
            return calculateAttackDistanceProbability(predator, prey, preyDryMass); 
        }
    );

    setRemainingProbabilityFunction(
        [this](const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass& preyDryMass) { 
            return calculateFleeingProbability(predator, prey, preyDryMass); 
        }
    );
}


const std::vector<IndividualTrait>& EscapeProbability::getWeightIndividualTraits(const AnimalNonStatistical& predator) const
{
    return predator.getGenetics().getEscapeProbabilityWeightIndividualTraits();
}


const PreciseDouble& EscapeProbability::getCVelocity() const
{
    return cVelocity;
}


PreciseDouble EscapeProbability::calculateVelocityProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass&)
{
    PreciseDouble tunedPreyVelocity = prey.getSpeed() - getCVelocity() * prey.calculateMassLoad().getValue() * prey.getSpeed();
    PreciseDouble tunedPredatorVelocity = predator.getSpeed() - getCVelocity() * predator.calculateMassLoad().getValue() * predator.getSpeed();

    return tunedPreyVelocity / (tunedPreyVelocity + tunedPredatorVelocity);
}

PreciseDouble EscapeProbability::calculateAttackDistanceProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass&)
{
    PreciseDouble predatorPreyDistance = Geometry::calculateDistanceBetweenPoints(predator.getPosition(), prey.getPosition());

    PreciseDouble attackDistanceProbability = predatorPreyDistance / predator.getInteractionAreaRadius();

    attackDistanceProbability = fmin(1.0, attackDistanceProbability);

    return attackDistanceProbability;
}

PreciseDouble EscapeProbability::calculateFleeingProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass&)
{
    PreciseDouble predatorPreyDistance = Geometry::calculateDistanceBetweenPoints(predator.getPosition(), prey.getPosition());

    PreciseDouble distance = predatorPreyDistance - predator.getInteractionAreaRadius();

    return 1.0 - sensoryModel->calculateProbability(distance, prey.getScopeAreaRadius());
}




BOOST_CLASS_EXPORT(EscapeProbability)

template <class Archive>
void EscapeProbability::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<AnimalDecisionProbability>(*this);

    ar & cVelocity;
}

// Specialisation
template void EscapeProbability::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void EscapeProbability::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void EscapeProbability::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void EscapeProbability::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
