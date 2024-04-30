#include "World/Map/Factory/MapFactory.h"


using namespace std;



const typename MapFactory::Type::TypeValue MapFactory::Type::stringToEnumValue(const string &str) { 
	try
	{
		return stringToEnum.at(str);
	}
	catch(const out_of_range& e) 
	{
		throwLineInfoException(fmt::format("Unknown map type '{}'. Valid values are {}", str, printAvailableValues()));
	}
}


constexpr size_t MapFactory::Type::size() 
{ 
	return magic_enum::enum_count<TypeValue>(); 
}


string_view MapFactory::Type::to_string(const TypeValue& type) 
{ 
	return magic_enum::enum_name(type); 
}


const unordered_map<string_view, const typename MapFactory::Type::TypeValue> MapFactory::Type::generateMap() 
{
	unordered_map<string_view, const TypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const TypeValue type = static_cast<const TypeValue>(i);
		enumMap.insert({to_string(type), type});
	}

	return enumMap;
}


const unordered_map<string_view, const typename MapFactory::Type::TypeValue> MapFactory::Type::stringToEnum = MapFactory::Type::generateMap();


const string MapFactory::Type::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<TypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}


const string MapFactory::Type::enumValues = MapFactory::Type::generateAvailableValues();


std::string_view MapFactory::Type::printAvailableValues() 
{ 
	return enumValues; 
}



unique_ptr<Map> MapFactory::createInstance(const json &mapInfo, WorldInterface* const worldInterface)
{
	switch(Type::stringToEnumValue(mapInfo["mapType"])) {
		case Type::SpatialTree: {
			return make_unique<SpatialTree>(mapInfo["worldWideParams"], mapInfo["spatialTreeParams"], worldInterface);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}
