#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/PredationProbabilityWeightTrait.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




namespace {
    const bool registered = []() {
        Trait::registryGenerateTraits[Trait::ExecutionOrder::PredationProbabilityWeight] = &PredationProbabilityWeightTrait::generateTraits;

		Trait::registryGetTraitStrVector[Trait::ExecutionOrder::PredationProbabilityWeight] = &PredationProbabilityWeightTrait::getTraitStrVector;

		return true;
    }();
}


vector<Trait*> PredationProbabilityWeightTrait::generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& definitionConfig, const vector<json>& individualLevelTraitsOrder)
{
	vector<Trait*> predationProbabilityWeight;

	for(const PredationProbabilityWeightType type : EnumClass<PredationProbabilityWeightType>::getEnumValues())
	{
		predationProbabilityWeight.push_back(new PredationProbabilityWeightTrait(
			individualLevelTraits, type, definitionConfig[DecisionsTrait::getJsonFieldName()][PredationProbabilityWeightTrait::getJsonFieldName()].at(EnumClass<PredationProbabilityWeightType>::to_string(type)), 
			individualLevelTraitsOrder
		));
	}

	return predationProbabilityWeight;
}


vector<string> PredationProbabilityWeightTrait::getTraitStrVector()
{
	vector<string> traitStrVector;

	for(PredationProbabilityWeightType type : EnumClass<PredationProbabilityWeightType>::getEnumValues())
	{
		traitStrVector.push_back(EnumClass<PredationProbabilityWeightType>::to_string(type));
	}

	return traitStrVector;
}




TraitTemperatureSection* PredationProbabilityWeightTrait::createTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& temperatureConfig, const vector<json>& individualLevelTraitsOrder, const string& traitStr, const string& fileName)
{
	if(temperatureConfig["temperature"]["dependent"].get<bool>())
	{
		return new PawarTraitTemperatureSection(individualLevelTraits, temperatureConfig["temperature"], individualLevelTraitsOrder, false, true, traitStr, fileName);
	}
	else
	{
		return nullptr;
	}
}




PredationProbabilityWeightTrait::PredationProbabilityWeightTrait()
{
	
}

PredationProbabilityWeightTrait::PredationProbabilityWeightTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const PredationProbabilityWeightType type, const json& config, const vector<json>& individualLevelTraitsOrder)
	: DecisionsTrait(
		individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<PredationProbabilityWeightType>::to_string(type), 
		getFileName(),
		createTemperatureSection(
			individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<PredationProbabilityWeightType>::to_string(type), 
			getFileName()
		)
	  )
{
	
}

PredationProbabilityWeightTrait::~PredationProbabilityWeightTrait() 
{
	
}


string PredationProbabilityWeightTrait::getFileName() const
{
	return DecisionsTrait::getFileName() + "_" + string("predationProbabilityWeight");
}

string PredationProbabilityWeightTrait::getParentTraitStr() const
{
	return DecisionsTrait::getParentTraitStr() + "$" + string("predationProbabilityWeight");
}

string PredationProbabilityWeightTrait::getJsonFieldName()
{
	return string("predationProbabilityWeight");
}


BOOST_CLASS_EXPORT(PredationProbabilityWeightTrait)

template <class Archive>
void PredationProbabilityWeightTrait::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<DecisionsTrait>(*this);
} 

// Specialisation
template void PredationProbabilityWeightTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void PredationProbabilityWeightTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void PredationProbabilityWeightTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void PredationProbabilityWeightTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
