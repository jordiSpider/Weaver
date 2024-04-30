#include "WorldInterface.h"

using namespace std;



const unordered_map<string_view, const WorldType::WorldTypeValue> WorldType::generateMap() 
{
	unordered_map<string_view, const WorldType::WorldTypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const WorldType::WorldTypeValue worldType = static_cast<const WorldType::WorldTypeValue>(i);
		enumMap.insert({to_string(worldType), worldType});
	}

	return enumMap;
}

const unordered_map<string_view, const WorldType::WorldTypeValue> WorldType::stringToEnum = WorldType::generateMap();

const string WorldType::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<WorldTypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string WorldType::enumValues = WorldType::generateAvailableValues();
