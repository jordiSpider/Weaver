#include "IBM/World/LivingBeings/Resources/Species/Factory/ResourceSpeciesFactory.h"


using namespace std;
using json = nlohmann::json;


constexpr size_t ResourceSpeciesFactory::Type::size() 
{ 
	return magic_enum::enum_count<TypeValue>(); 
}

std::string_view ResourceSpeciesFactory::Type::to_string(const ResourceSpeciesFactory::Type::TypeValue& type) 
{ 
	return magic_enum::enum_name(type); 
}

const unordered_map<string_view, const ResourceSpeciesFactory::Type::TypeValue> ResourceSpeciesFactory::Type::generateMap() 
{
	unordered_map<string_view, const TypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const TypeValue type = static_cast<const TypeValue>(i);
		enumMap.insert({to_string(type), type});
	}

	return enumMap;
}

const unordered_map<string_view, const ResourceSpeciesFactory::Type::TypeValue> ResourceSpeciesFactory::Type::stringToEnum = ResourceSpeciesFactory::Type::generateMap();

const string ResourceSpeciesFactory::Type::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<TypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string ResourceSpeciesFactory::Type::enumValues = ResourceSpeciesFactory::Type::generateAvailableValues();

std::string_view ResourceSpeciesFactory::Type::printAvailableValues() 
{ 
	return enumValues; 
}

ResourceSpeciesFactory::Type::TypeValue ResourceSpeciesFactory::Type::stringToEnumValue(const std::string &str) 
{ 
	try
	{
		return stringToEnum.at(str);
	}
	catch(const std::out_of_range& e) 
	{
		throwLineInfoException(fmt::format("Unknown simulation type '{}'. Valid values are {}", str, printAvailableValues()));
	}
}


unique_ptr<ResourceSpecies> ResourceSpeciesFactory::createInstance(const json &resourceSpeciesInfo)
{
    switch(Type::stringToEnumValue(resourceSpeciesInfo["resourceType"])) {
        case Type::BasalResource: {
            return make_unique<BasalResourceSpecies>(resourceSpeciesInfo);
            break;
        }
        case Type::NoBasalResource: {
            return make_unique<NoBasalResourceSpecies>(resourceSpeciesInfo);
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}
