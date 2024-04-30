#include "IBM/World/LivingBeings/Animals/AnimalFactory.h"


using namespace std;



const typename AnimalFactory::Type::TypeValue AnimalFactory::Type::stringToEnumValue(const string &str) { 
	try
	{
		return stringToEnum.at(str);
	}
	catch(const out_of_range& e) 
	{
		throwLineInfoException(fmt::format("Unknown form patch type '{}'. Valid values are {}", str, printAvailableValues()));
	}
}


constexpr size_t AnimalFactory::Type::size() 
{ 
	return magic_enum::enum_count<TypeValue>(); 
}


std::string_view AnimalFactory::Type::to_string(const TypeValue& type) 
{ 
	return magic_enum::enum_name(type); 
}


const unordered_map<string_view, const typename AnimalFactory::Type::TypeValue> AnimalFactory::Type::generateMap() 
{
	unordered_map<string_view, const TypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const TypeValue type = static_cast<const TypeValue>(i);
		enumMap.insert({to_string(type), type});
	}

	return enumMap;
}


const unordered_map<string_view, const typename AnimalFactory::Type::TypeValue> AnimalFactory::Type::stringToEnum = AnimalFactory::Type::generateMap();


const string AnimalFactory::Type::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<TypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}


const string AnimalFactory::Type::enumValues = AnimalFactory::Type::generateAvailableValues();


std::string_view AnimalFactory::Type::printAvailableValues() 
{ 
	return enumValues; 
}

template <class Archive>
void AnimalFactory::Type::serialize(Archive &ar, const unsigned int version) {
	ar & stringToEnum;
	ar & enumValues;
}
