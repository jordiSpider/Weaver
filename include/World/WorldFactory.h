#ifndef WORLD_FACTORY_H_
#define WORLD_FACTORY_H_


#include <memory>
#include <string>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include "Exceptions/LineInfoException.h"
#include "Misc/Types.h"
#include "World/WorldInterface.h"
#include "World/ArthropodsWorld.h"
#include "World/DinosaursWorld.h"
#include "World/AquaticWorld.h"
#include "Misc/Macros.h"


class WorldFactory
{
public:
    class Type {
    public:
        enum TypeValue : unsigned int
		{
            Arthropods,
            Dinosaurs,
            Aquatic
		};

        static const TypeValue stringToEnumValue(const std::string &str);
        static constexpr size_t size();

    private:
        static const std::unordered_map<std::string_view, const TypeValue> stringToEnum;
        static const std::string enumValues;

        static std::string_view to_string(const TypeValue& type);
        static const std::unordered_map<std::string_view, const TypeValue> generateMap();
        static const std::string generateAvailableValues();
        static std::string_view printAvailableValues();
    };


	static std::unique_ptr<WorldInterface> createInstance(nlohmann::json* jsonTree, nlohmann::json &worldConfig, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath, int burnIn);
};

#endif // WORLD_FACTORY_H_
