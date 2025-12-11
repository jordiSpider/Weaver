#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/BaseTrait.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




namespace {
    const bool registered = []() {
        Trait::registryGenerateTraits[Trait::ExecutionOrder::Base] = &BaseTrait::generateTraits;

		Trait::registryGetTraitStrVector[Trait::ExecutionOrder::Base] = &BaseTrait::getTraitStrVector;

		return true;
    }();
}



vector<Trait*> BaseTrait::generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& definitionConfig, const vector<json>& individualLevelTraitsOrder)
{
	vector<Trait*> baseTraits;

	for(const BaseTraitType type : EnumClass<BaseTraitType>::getEnumValues())
	{
		baseTraits.push_back(new BaseTrait(
			individualLevelTraits, type, definitionConfig[BaseTrait::getJsonFieldName()].at(EnumClass<BaseTraitType>::to_string(type)), 
			individualLevelTraitsOrder
		));
	}

	return baseTraits;
}



vector<string> BaseTrait::getTraitStrVector()
{
	vector<string> traitStrVector;

	for(BaseTraitType type : EnumClass<BaseTraitType>::getEnumValues())
	{
		traitStrVector.push_back(EnumClass<BaseTraitType>::to_string(type));
	}

	return traitStrVector;
}



TraitTemperatureSection* BaseTrait::createTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const BaseTraitType type, const json& temperatureConfig, const vector<json>& individualLevelTraitsOrder, const string& traitStr, const string& fileName)
{
	if(type == BaseTraitType::energy_tank)
	{
		return nullptr;
	}
	else
	{
		if(temperatureConfig["temperature"]["dependent"].get<bool>())
		{
			if(type == BaseTraitType::lengthAtMaturation)
			{
				return new TempSizeRuleTraitTemperatureSection(temperatureConfig["temperature"]);
			}
			else
			{
				return new PawarTraitTemperatureSection(individualLevelTraits, temperatureConfig["temperature"], individualLevelTraitsOrder, isInverse(type), isStrictlyPositive(type), traitStr, fileName);
			}
		}
		else
		{
			return nullptr;
		}
	}
}




BaseTrait::BaseTrait()
{
	
}

BaseTrait::BaseTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const BaseTraitType type, const json& config, const vector<json>& individualLevelTraitsOrder)
	: Trait(
		individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<BaseTraitType>::to_string(type), 
		getFileName(),
		createTemperatureSection(
			individualLevelTraits, type, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<BaseTraitType>::to_string(type), 
			getFileName()
		)
	  ),
	  type(type)
{
	
}

BaseTrait::~BaseTrait() 
{
	
}

bool BaseTrait::isInverse(const BaseTraitType type)
{
	if(type == BaseTraitType::eggDevTime ||
	   type == BaseTraitType::actE_met ||
	   type == BaseTraitType::devTime ||
	   type == BaseTraitType::pupaPeriodTime)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool BaseTrait::isStrictlyPositive(const BaseTraitType type)
{
	if(type == BaseTraitType::factorEggMass)
	{
		return false;
	}
	else
	{
		return true;
	}
}



string BaseTrait::getFileName() const
{
	return string("base");
}

string BaseTrait::getParentTraitStr() const
{
	return string("base");
}

string BaseTrait::getJsonFieldName()
{
	return string("base");
}


bool BaseTrait::canBeNegative() const
{
	return type == BaseTraitType::factorEggMass || type == BaseTraitType::eggDevTime;
}



BOOST_CLASS_EXPORT(BaseTrait)

template <class Archive>
void BaseTrait::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<Trait>(*this);

	ar & type;
} 

// Specialisation
template void BaseTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void BaseTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void BaseTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void BaseTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
