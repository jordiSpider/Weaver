#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/EdibilityValueWeightTrait.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




namespace {
    const bool registered = []() {
        Trait::registryGenerateTraits[Trait::ExecutionOrder::EdibilityValueWeight] = &EdibilityValueWeightTrait::generateTraits;

		Trait::registryGetTraitStrVector[Trait::ExecutionOrder::EdibilityValueWeight] = &EdibilityValueWeightTrait::getTraitStrVector;

		return true;
    }();
}


vector<Trait*> EdibilityValueWeightTrait::generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& definitionConfig, const vector<json>& individualLevelTraitsOrder)
{
	vector<Trait*> edibilityValueWeightTraits;

	for(const EdibilityValueWeightType type : EnumClass<EdibilityValueWeightType>::getEnumValues())
	{
		edibilityValueWeightTraits.push_back(new EdibilityValueWeightTrait(
			individualLevelTraits, type, definitionConfig[DecisionsTrait::getJsonFieldName()][EdibilityValueWeightTrait::getJsonFieldName()].at(EnumClass<EdibilityValueWeightType>::to_string(type)), 
			individualLevelTraitsOrder
		));
	}

	return edibilityValueWeightTraits;
}


vector<string> EdibilityValueWeightTrait::getTraitStrVector()
{
	vector<string> traitStrVector;

	for(EdibilityValueWeightType type : EnumClass<EdibilityValueWeightType>::getEnumValues())
	{
		traitStrVector.push_back(EnumClass<EdibilityValueWeightType>::to_string(type));
	}

	return traitStrVector;
}




TraitTemperatureSection* EdibilityValueWeightTrait::createTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& temperatureConfig, const vector<json>& individualLevelTraitsOrder, const string& traitStr, const string& fileName)
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




EdibilityValueWeightTrait::EdibilityValueWeightTrait()
{
	
}

EdibilityValueWeightTrait::EdibilityValueWeightTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const EdibilityValueWeightType type, const json& config, const vector<json>& individualLevelTraitsOrder)
	: DecisionsTrait(
		individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<EdibilityValueWeightType>::to_string(type), 
		getFileName(),
		createTemperatureSection(
			individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<EdibilityValueWeightType>::to_string(type), 
			getFileName()
		)
	  )
{
	
}

EdibilityValueWeightTrait::~EdibilityValueWeightTrait() 
{
	
}


string EdibilityValueWeightTrait::getFileName() const
{
	return DecisionsTrait::getFileName() + "_" + string("edibilityValueWeight");
}

string EdibilityValueWeightTrait::getParentTraitStr() const
{
	return DecisionsTrait::getParentTraitStr() + "$" + string("edibilityValueWeight");
}

string EdibilityValueWeightTrait::getJsonFieldName()
{
	return string("edibilityValueWeight");
}


BOOST_CLASS_EXPORT(EdibilityValueWeightTrait)

template <class Archive>
void EdibilityValueWeightTrait::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<DecisionsTrait>(*this);
} 

// Specialisation
template void EdibilityValueWeightTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void EdibilityValueWeightTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void EdibilityValueWeightTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void EdibilityValueWeightTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
