
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/EdibleDecisionProbability.h"


#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"


using namespace std;
using json = nlohmann::json;


EdibleDecisionProbability::EdibleDecisionProbability()
    : DecisionProbability()
{

}

EdibleDecisionProbability::EdibleDecisionProbability(const json& config, const vector<Trait*> &probabilityWeightTraits)
    : DecisionProbability(config, probabilityWeightTraits)
{
    probabilityFunctions.resize(probabilityWeightTraits.size(), [this](const AnimalNonStatistical&, const Edible&, const DryMass&) { return defaultProbabilityFunction(); });
    remainingProbabilityFunction = [this](const AnimalNonStatistical&, const Edible&, const DryMass&) { return defaultProbabilityFunction(); };
}

EdibleDecisionProbability::~EdibleDecisionProbability()
{
    
}


void EdibleDecisionProbability::setProbabilityFunction(const size_t index, const std::function<PreciseDouble(const AnimalNonStatistical&, const Edible&, const DryMass&)>& newProbabilityFunction)
{
    probabilityFunctions[index] = newProbabilityFunction;
}


void EdibleDecisionProbability::setRemainingProbabilityFunction(const std::function<PreciseDouble(const AnimalNonStatistical&, const Edible&, const DryMass&)>& newProbabilityFunction)
{
    remainingProbabilityFunction = newProbabilityFunction;
}


PreciseDouble EdibleDecisionProbability::applyProbabilityFunctions(const size_t index, const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const
{
    return probabilityFunctions[index](predator, prey, preyDryMass);
}


PreciseDouble EdibleDecisionProbability::applyRemainingProbabilityFunctions(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const
{
    return remainingProbabilityFunction(predator, prey, preyDryMass);
}


PreciseDouble EdibleDecisionProbability::defaultProbabilityFunction()
{
	throwLineInfoException("Error: Method not defined for any of the weights.");
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(EdibleDecisionProbability)

template <class Archive>
void EdibleDecisionProbability::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<DecisionProbability>(*this);
}

// Specialisation
template void EdibleDecisionProbability::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void EdibleDecisionProbability::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void EdibleDecisionProbability::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void EdibleDecisionProbability::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
