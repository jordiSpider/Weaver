#include "IBM/World/LivingBeings/Animals/Genetics/Traits/Trait.h"

using namespace std;
using json = nlohmann::json;



const typename Trait::ValueUpdateMethod::MethodType Trait::ValueUpdateMethod::stringToEnumValue(const string &str) { 
	try
	{
		return stringToEnum.at(str);
	}
	catch(const out_of_range& e) 
	{
		throwLineInfoException(fmt::format("Unknown form patch type '{}'. Valid values are {}", str, printAvailableValues()));
	}
}


constexpr size_t Trait::ValueUpdateMethod::size() 
{ 
	return magic_enum::enum_count<MethodType>(); 
}


std::string_view Trait::ValueUpdateMethod::to_string(const MethodType& type) 
{ 
	return magic_enum::enum_name(type); 
}


const unordered_map<string_view, const typename Trait::ValueUpdateMethod::MethodType> Trait::ValueUpdateMethod::generateMap() 
{
	unordered_map<string_view, const MethodType> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const MethodType type = static_cast<const MethodType>(i);
		enumMap.insert({to_string(type), type});
	}

	return enumMap;
}


const unordered_map<string_view, const typename Trait::ValueUpdateMethod::MethodType> Trait::ValueUpdateMethod::stringToEnum = Trait::ValueUpdateMethod::generateMap();


const string Trait::ValueUpdateMethod::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<MethodType>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}


const string Trait::ValueUpdateMethod::enumValues = Trait::ValueUpdateMethod::generateAvailableValues();


std::string_view Trait::ValueUpdateMethod::printAvailableValues() 
{ 
	return enumValues; 
}



Trait::Trait(const string& name, const json& traitInfo)
	: type(stringToEnumValue(name)), lowerThreshold(traitInfo["temperatureThreshold"]["lower"]), 
	  upperThreshold(traitInfo["temperatureThreshold"]["upper"])
{

}

const unordered_map<string_view, const Trait::Type> Trait::generateMap() 
{
	unordered_map<string_view, const Type> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const Type trait = static_cast<const Type>(i);
		enumMap.insert({to_string(trait), trait});
	}

	return enumMap;
}

const unordered_map<string_view, const Trait::Type> Trait::stringToEnum = Trait::generateMap();

const string Trait::generateHeader()
{
	constexpr auto traitNames = magic_enum::enum_names<Type>();

	auto header = fmt::format("{}", traitNames[0]);
	for(size_t i = 1; i < traitNames.size(); i++) {
		header += fmt::format("\t{}", traitNames[i]);
	}

	return header;
}

const string Trait::enumHeader = Trait::generateHeader();

const string Trait::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<Type>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string Trait::enumValues = Trait::generateAvailableValues();


const Temperature& Trait::getLowerThreshold() const
{
	return lowerThreshold;
}

const Temperature& Trait::getUpperThreshold() const
{
	return upperThreshold;
}

template <class Archive>
void Trait::serialize(Archive &ar, const unsigned int version) {
	ar & type; 
	ar & stringToEnum;
	ar & enumHeader;
	ar & enumValues;
} 
