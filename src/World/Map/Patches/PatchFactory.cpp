#include "World/Map/Patches/PatchFactory.h"


using namespace std;



const typename PatchFactory::Type::TypeValue PatchFactory::Type::stringToEnumValue(const string &str) { 
	try
	{
		return stringToEnum.at(str);
	}
	catch(const out_of_range& e) 
	{
		throwLineInfoException(fmt::format("Unknown form patch type '{}'. Valid values are {}", str, printAvailableValues()));
	}
}


constexpr size_t PatchFactory::Type::size() 
{ 
	return magic_enum::enum_count<TypeValue>(); 
}


std::string_view PatchFactory::Type::to_string(const TypeValue& type) 
{ 
	return magic_enum::enum_name(type); 
}


const unordered_map<string_view, const typename PatchFactory::Type::TypeValue> PatchFactory::Type::generateMap() 
{
	unordered_map<string_view, const TypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const TypeValue type = static_cast<const TypeValue>(i);
		enumMap.insert({to_string(type), type});
	}

	return enumMap;
}


const unordered_map<string_view, const typename PatchFactory::Type::TypeValue> PatchFactory::Type::stringToEnum = PatchFactory::Type::generateMap();


const string PatchFactory::Type::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<TypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}


const string PatchFactory::Type::enumValues = PatchFactory::Type::generateAvailableValues();


std::string_view PatchFactory::Type::printAvailableValues() 
{ 
	return enumValues; 
}
