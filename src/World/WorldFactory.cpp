#include "World/WorldFactory.h"

using namespace std;



const typename WorldFactory::Type::TypeValue WorldFactory::Type::stringToEnumValue(const string &str) { 
	try
	{
		return stringToEnum.at(str);
	}
	catch(const out_of_range& e) 
	{
		throwLineInfoException(fmt::format("Unknown moisture type '{}'. Valid values are {}", str, printAvailableValues()));
	}
}


constexpr size_t WorldFactory::Type::size() 
{ 
	return magic_enum::enum_count<TypeValue>(); 
}


std::string_view WorldFactory::Type::to_string(const TypeValue& type) 
{ 
	return magic_enum::enum_name(type); 
}


const unordered_map<string_view, const typename WorldFactory::Type::TypeValue> WorldFactory::Type::generateMap() 
{
	unordered_map<string_view, const TypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const TypeValue type = static_cast<const TypeValue>(i);
		enumMap.insert({to_string(type), type});
	}

	return enumMap;
}


const unordered_map<string_view, const typename WorldFactory::Type::TypeValue> WorldFactory::Type::stringToEnum = WorldFactory::Type::generateMap();


const string WorldFactory::Type::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<TypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}


const string WorldFactory::Type::enumValues = WorldFactory::Type::generateAvailableValues();


std::string_view WorldFactory::Type::printAvailableValues() 
{ 
	return enumValues; 
}




unique_ptr<WorldInterface> WorldFactory::createInstance(json* jsonTree, json &worldConfig, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath) {
    switch(Type::stringToEnumValue(worldConfig["world"]["simulationType"])) {
        case Type::Arthropods: {
            return make_unique<ArthropodsWorld>(jsonTree, worldConfig, outputFolder, WeaverFolder, configPath);
            break;
        }
        case Type::Dinosaurs: {
            return make_unique<DinosaursWorld>(jsonTree, worldConfig, outputFolder, WeaverFolder, configPath);
            break;
        }
		case Type::Aquatic: {
            return make_unique<AquaticWorld>(jsonTree, worldConfig, outputFolder, WeaverFolder, configPath);
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}
