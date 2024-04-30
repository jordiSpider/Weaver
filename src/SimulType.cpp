#include "SimulType.h"

using namespace std;


SimulType::SimulType(const string& typeStr) : value(stringToEnumValue(typeStr)) {};

const unordered_map<string_view, const SimulType::SimulTypeValue> SimulType::generateMap() 
{
	unordered_map<string_view, const SimulType::SimulTypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const SimulType::SimulTypeValue simulType = static_cast<const SimulType::SimulTypeValue>(i);
		enumMap.insert({to_string(simulType), simulType});
	}

	return enumMap;
}

const unordered_map<string_view, const SimulType::SimulTypeValue> SimulType::stringToEnum = SimulType::generateMap();

const string SimulType::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<SimulTypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string SimulType::enumValues = SimulType::generateAvailableValues();
