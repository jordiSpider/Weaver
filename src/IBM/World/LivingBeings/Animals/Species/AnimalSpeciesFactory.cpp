#include "IBM/World/LivingBeings/Animals/Species/AnimalSpeciesFactory.h"


using namespace std;
using json = nlohmann::json;


constexpr size_t AnimalSpeciesFactory::Type::size() 
{ 
	return magic_enum::enum_count<TypeValue>(); 
}

std::string_view AnimalSpeciesFactory::Type::to_string(const AnimalSpeciesFactory::Type::TypeValue& type) 
{ 
	return magic_enum::enum_name(type); 
}

const unordered_map<string_view, const AnimalSpeciesFactory::Type::TypeValue> AnimalSpeciesFactory::Type::generateMap() 
{
	unordered_map<string_view, const TypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const TypeValue type = static_cast<const TypeValue>(i);
		enumMap.insert({to_string(type), type});
	}

	return enumMap;
}

const unordered_map<string_view, const AnimalSpeciesFactory::Type::TypeValue> AnimalSpeciesFactory::Type::stringToEnum = AnimalSpeciesFactory::Type::generateMap();

const string AnimalSpeciesFactory::Type::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<TypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string AnimalSpeciesFactory::Type::enumValues = AnimalSpeciesFactory::Type::generateAvailableValues();

std::string_view AnimalSpeciesFactory::Type::printAvailableValues() 
{ 
	return enumValues; 
}

AnimalSpeciesFactory::Type::TypeValue AnimalSpeciesFactory::Type::stringToEnumValue(const std::string &str) 
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

template <class Archive>
void AnimalSpeciesFactory::Type::serialize(Archive &ar, const unsigned int version) {
	ar & stringToEnum;
	ar & enumValues;

}


unique_ptr<AnimalSpecies> AnimalSpeciesFactory::createInstance(const json &animalSpeciesInfo, const bool initIndividualsPerDensities, World* const world)
{
	switch(Type::Default) {
        case Type::Default: {
            return make_unique<AnimalSpecies>(animalSpeciesInfo, initIndividualsPerDensities, world);
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}

template <class Archive>
void AnimalSpeciesFactory::serialize(Archive &ar, const unsigned int version) 
{

}
