#include "Patch.h"


using namespace std;


const unordered_map<string_view, const Patch::PatchType> Patch::generateMap() 
{
	unordered_map<string_view, const PatchType> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const PatchType patch = static_cast<const PatchType>(i);
		enumMap.insert({to_string(patch), patch});
	}

	return enumMap;
}

const unordered_map<string_view, const Patch::PatchType> Patch::stringToEnum = Patch::generateMap();

const string Patch::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<PatchType>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string Patch::enumValues = Patch::generateAvailableValues();
