#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/EscapeProbabilityWeightTrait.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




namespace {
    const bool registered = []() {
        Trait::registryGenerateTraits[Trait::ExecutionOrder::EscapeProbabilityWeight] = &EscapeProbabilityWeightTrait::generateTraits;

		Trait::registryGetTraitStrVector[Trait::ExecutionOrder::EscapeProbabilityWeight] = &EscapeProbabilityWeightTrait::getTraitStrVector;

		return true;
    }();
}


vector<Trait*> EscapeProbabilityWeightTrait::generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& definitionConfig, const vector<json>& individualLevelTraitsOrder)
{
	vector<Trait*> escapeProbabilityWeightTraits;

	for(const EscapeProbabilityWeightType type : EnumClass<EscapeProbabilityWeightType>::getEnumValues())
	{
		escapeProbabilityWeightTraits.push_back(new EscapeProbabilityWeightTrait(
			individualLevelTraits, type, definitionConfig[DecisionsTrait::getJsonFieldName()][EscapeProbabilityWeightTrait::getJsonFieldName()].at(EnumClass<EscapeProbabilityWeightType>::to_string(type)), 
			individualLevelTraitsOrder
		));
	}

	return escapeProbabilityWeightTraits;
}


vector<string> EscapeProbabilityWeightTrait::getTraitStrVector()
{
	vector<string> traitStrVector;

	for(EscapeProbabilityWeightType type : EnumClass<EscapeProbabilityWeightType>::getEnumValues())
	{
		traitStrVector.push_back(EnumClass<EscapeProbabilityWeightType>::to_string(type));
	}

	return traitStrVector;
}




TraitTemperatureSection* EscapeProbabilityWeightTrait::createTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& temperatureConfig, const vector<json>& individualLevelTraitsOrder, const string& traitStr, const string& fileName)
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




EscapeProbabilityWeightTrait::EscapeProbabilityWeightTrait()
{
	
}

EscapeProbabilityWeightTrait::EscapeProbabilityWeightTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const EscapeProbabilityWeightType type, const json& config, const vector<json>& individualLevelTraitsOrder)
	: DecisionsTrait(
		individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<EscapeProbabilityWeightType>::to_string(type), 
		getFileName(),
		createTemperatureSection(
			individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<EscapeProbabilityWeightType>::to_string(type), 
			getFileName()
		)
	  )
{
	
}

EscapeProbabilityWeightTrait::~EscapeProbabilityWeightTrait() 
{
	
}


string EscapeProbabilityWeightTrait::getFileName() const
{
	return DecisionsTrait::getFileName() + "_" + string("escapeProbabilityWeight");
}

string EscapeProbabilityWeightTrait::getParentTraitStr() const
{
	return DecisionsTrait::getParentTraitStr() + "$" + string("escapeProbabilityWeight");
}

string EscapeProbabilityWeightTrait::getJsonFieldName()
{
	return string("escapeProbabilityWeight");
}


BOOST_CLASS_EXPORT(EscapeProbabilityWeightTrait)

template <class Archive>
void EscapeProbabilityWeightTrait::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<DecisionsTrait>(*this);
} 

// Specialisation
template void EscapeProbabilityWeightTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void EscapeProbabilityWeightTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void EscapeProbabilityWeightTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void EscapeProbabilityWeightTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
