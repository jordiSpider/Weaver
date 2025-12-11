
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/DecisionProbability.h"



using namespace std;
using json = nlohmann::json;


DecisionProbability::DecisionProbability()
{

}

DecisionProbability::DecisionProbability(const json& config, const vector<Trait*> &probabilityWeightTraits)
    : additiveMechanism(config["additiveMechanism"].get<bool>()), computedRemainingWeight(!config["isRemainingWeightNull"].get<bool>())
{
    computedWeights.resize(probabilityWeightTraits.size());
    for(size_t i = 0; i < probabilityWeightTraits.size(); i++)
    {
        computedWeights[i] = !probabilityWeightTraits[i]->isNull();
    }
}

DecisionProbability::~DecisionProbability()
{
    
}



PreciseDouble DecisionProbability::calculateValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const
{
    PreciseDouble remainingWeight = 1.0;

	PreciseDouble probability;

	if(additiveMechanism)
	{
		probability = 0.0;
	}
	else
	{
		probability = 1.0;
	}


	for(size_t i = 0; i < getWeightIndividualTraits(predator).size(); ++i)
	{
		if(computedWeights[i])
		{
			const PreciseDouble weight = getWeightIndividualTraits(predator).at(i).getPhenotypicValue();

			if(additiveMechanism)
			{
				probability += weight * applyProbabilityFunctions(i, predator, prey, preyDryMass);
			}
			else
			{
				probability *= weight * applyProbabilityFunctions(i, predator, prey, preyDryMass);
			}

			remainingWeight -= weight;
		}
	}


	if(computedRemainingWeight)
	{
		if(additiveMechanism)
		{
			probability += remainingWeight * applyRemainingProbabilityFunctions(predator, prey, preyDryMass);
		}
		else
		{
			probability *= remainingWeight * applyRemainingProbabilityFunctions(predator, prey, preyDryMass);
		}
	}


	return probability;
}


void DecisionProbability::deserializeProbabilityFunctions()
{
    initProbabilityFunctions();
}





BOOST_SERIALIZATION_ASSUME_ABSTRACT(DecisionProbability)

template <class Archive>
void DecisionProbability::serialize(Archive &ar, const unsigned int) {
    ar & additiveMechanism;
    ar & computedWeights;
    ar & computedRemainingWeight;
}

// Specialisation
template void DecisionProbability::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void DecisionProbability::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void DecisionProbability::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void DecisionProbability::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
