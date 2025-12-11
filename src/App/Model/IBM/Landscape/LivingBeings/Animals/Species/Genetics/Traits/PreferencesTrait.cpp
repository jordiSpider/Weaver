#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/PreferencesTrait.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




namespace {
    const bool registered = []() {
        Trait::registryGenerateTraits[Trait::ExecutionOrder::Preferences] = &PreferencesTrait::generateTraits;

		Trait::registryGetTraitStrVector[Trait::ExecutionOrder::Preferences] = &PreferencesTrait::getTraitStrVector;

		return true;
    }();
}


vector<Trait*> PreferencesTrait::generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& definitionConfig, const vector<json>& individualLevelTraitsOrder)
{
	vector<Trait*> preferencesTraits;

	for(const PreferencesTraitType type : EnumClass<PreferencesTraitType>::getEnumValues())
	{
		preferencesTraits.push_back(new PreferencesTrait(
			individualLevelTraits, type, definitionConfig[DecisionsTrait::getJsonFieldName()][PreferencesTrait::getJsonFieldName()].at(EnumClass<PreferencesTraitType>::to_string(type)), 
			individualLevelTraitsOrder
		));
	}

	return preferencesTraits;
}


vector<string> PreferencesTrait::getTraitStrVector()
{
	vector<string> traitStrVector;

	for(PreferencesTraitType type : EnumClass<PreferencesTraitType>::getEnumValues())
	{
		traitStrVector.push_back(EnumClass<PreferencesTraitType>::to_string(type));
	}

	return traitStrVector;
}




TraitTemperatureSection* PreferencesTrait::createTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& temperatureConfig, const vector<json>& individualLevelTraitsOrder, const string& traitStr, const string& fileName)
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




PreferencesTrait::PreferencesTrait()
{
	
}

PreferencesTrait::PreferencesTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const PreferencesTraitType type, const json& config, const vector<json>& individualLevelTraitsOrder)
	: DecisionsTrait(
		individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<PreferencesTraitType>::to_string(type), 
		getFileName(),
		createTemperatureSection(
			individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<PreferencesTraitType>::to_string(type), 
			getFileName()
		)
	  )
{
	
}

PreferencesTrait::~PreferencesTrait() 
{
	
}


string PreferencesTrait::getFileName() const
{
	return DecisionsTrait::getFileName() + "_" + string("preferences");
}

string PreferencesTrait::getParentTraitStr() const
{
	return DecisionsTrait::getParentTraitStr() + "$" + string("preferences");
}

string PreferencesTrait::getJsonFieldName()
{
	return string("preferences");
}


BOOST_CLASS_EXPORT(PreferencesTrait)

template <class Archive>
void PreferencesTrait::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<DecisionsTrait>(*this);
} 

// Specialisation
template void PreferencesTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void PreferencesTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void PreferencesTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void PreferencesTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
