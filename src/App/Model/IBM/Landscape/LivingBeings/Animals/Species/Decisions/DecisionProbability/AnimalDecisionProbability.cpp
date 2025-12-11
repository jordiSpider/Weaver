
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/AnimalDecisionProbability.h"


#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"


using namespace std;
using json = nlohmann::json;


AnimalDecisionProbability::AnimalDecisionProbability()
    : DecisionProbability()
{

}

AnimalDecisionProbability::AnimalDecisionProbability(const json& config, const vector<Trait*> &probabilityWeightTraits)
    : DecisionProbability(config, probabilityWeightTraits)
{
    probabilityFunctions.resize(probabilityWeightTraits.size(), [this](const AnimalNonStatistical&, const AnimalNonStatistical&, const DryMass&) { return defaultProbabilityFunction(); });
    remainingProbabilityFunction = [this](const AnimalNonStatistical&, const AnimalNonStatistical&, const DryMass&) { return defaultProbabilityFunction(); };
}

AnimalDecisionProbability::~AnimalDecisionProbability()
{
    
}


void AnimalDecisionProbability::setProbabilityFunction(const size_t index, const std::function<PreciseDouble(const AnimalNonStatistical&, const AnimalNonStatistical&, const DryMass&)>& newProbabilityFunction)
{
    probabilityFunctions[index] = newProbabilityFunction;
}


void AnimalDecisionProbability::setRemainingProbabilityFunction(const std::function<PreciseDouble(const AnimalNonStatistical&, const AnimalNonStatistical&, const DryMass&)>& newProbabilityFunction)
{
    remainingProbabilityFunction = newProbabilityFunction;
}


PreciseDouble AnimalDecisionProbability::applyProbabilityFunctions(const size_t index, const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const
{
    return probabilityFunctions[index](predator, static_cast<const AnimalNonStatistical&>(prey), preyDryMass);
}


PreciseDouble AnimalDecisionProbability::applyRemainingProbabilityFunctions(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const
{
    return remainingProbabilityFunction(predator, static_cast<const AnimalNonStatistical&>(prey), preyDryMass);
}


PreciseDouble AnimalDecisionProbability::defaultProbabilityFunction()
{
	throwLineInfoException("Error: Method not defined for any of the weights.");
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(AnimalDecisionProbability)

template <class Archive>
void AnimalDecisionProbability::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<DecisionProbability>(*this);
}

// Specialisation
template void AnimalDecisionProbability::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalDecisionProbability::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalDecisionProbability::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalDecisionProbability::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
