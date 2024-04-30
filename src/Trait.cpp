#include "Trait.h"

#include <cfloat>

using namespace std;


Trait::Trait(const string& typeStr) : type(stringToEnumValue(typeStr)) {};

const unordered_map<string_view, const TraitType> Trait::generateMap() 
{
	unordered_map<string_view, const TraitType> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const TraitType trait = static_cast<const TraitType>(i);
		enumMap.insert({to_string(trait), trait});
	}

	return enumMap;
}

const unordered_map<string_view, const TraitType> Trait::stringToEnum = Trait::generateMap();

const string Trait::generateHeader()
{
	constexpr auto traitNames = magic_enum::enum_names<TraitType>();

	auto header = fmt::format("{}", traitNames[0]);
	for(size_t i = 1; i < traitNames.size(); i++) {
		header += fmt::format("\t{}", traitNames[i]);
	}

	return header;
}

const string Trait::enumHeader = Trait::generateHeader();

const string Trait::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<TraitType>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string Trait::enumValues = Trait::generateAvailableValues();


FixedTrait::FixedTrait(const string& typeStr, const double& value) 
    : Trait(typeStr), value(value) {} 


VariableTrait::VariableTrait(const string& typeStr, const unsigned int& order, 
              const double& minTraitRange, const double& maxTraitRange, 
              const double& minTraitLimit, const double& maxTraitLimit,
              const double& restrictPerTrait)
    : Trait(typeStr), order(order),
      minTraitRange(minTraitRange), maxTraitRange(maxTraitRange),
      minObservedPseudoValue(DBL_MAX), maxObservedPseudoValue(0.0),
      minTraitLimit(minTraitLimit), maxTraitLimit(maxTraitLimit),
      restrictPerTrait(restrictPerTrait)
{
    minRestrictedRange = minTraitRange + ((maxTraitRange - minTraitRange) * 0.5) * restrictPerTrait;
    maxRestrictedRange = maxTraitRange - ((maxTraitRange - minTraitRange) * 0.5) * restrictPerTrait;
}
