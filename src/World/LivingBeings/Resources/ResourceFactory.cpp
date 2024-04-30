#include "World/LivingBeings/Resources/ResourceFactory.h"


using namespace std;


constexpr size_t ResourceFactory::Type::size() 
{ 
	return magic_enum::enum_count<TypeValue>(); 
}

std::string_view ResourceFactory::Type::to_string(const ResourceFactory::Type::TypeValue& type) 
{ 
	return magic_enum::enum_name(type); 
}

const unordered_map<string_view, const ResourceFactory::Type::TypeValue> ResourceFactory::Type::generateMap() 
{
	unordered_map<string_view, const TypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const TypeValue type = static_cast<const TypeValue>(i);
		enumMap.insert({to_string(type), type});
	}

	return enumMap;
}

const unordered_map<string_view, const ResourceFactory::Type::TypeValue> ResourceFactory::Type::stringToEnum = ResourceFactory::Type::generateMap();

const string ResourceFactory::Type::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<TypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string ResourceFactory::Type::enumValues = ResourceFactory::Type::generateAvailableValues();

std::string_view ResourceFactory::Type::printAvailableValues() 
{ 
	return enumValues; 
}

ResourceFactory::Type::TypeValue ResourceFactory::Type::stringToEnumValue(const std::string &str) 
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


unique_ptr<ResourceInterface> ResourceFactory::createInstance(ResourceSpecies* const mySpecies, 
		TerrainCellInterface* terrainCellInterface, double biomass, double resourceMaximumCapacity, 
		double massRatio, bool patchSpread)
{
    switch(Type::Default) {
        case Type::Default: {
            return make_unique<Resource>(mySpecies, terrainCellInterface, biomass, resourceMaximumCapacity, 
				massRatio, patchSpread
			);
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}
