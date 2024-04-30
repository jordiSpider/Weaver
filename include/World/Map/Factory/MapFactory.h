#ifndef MAP_FACTORY_H_
#define MAP_FACTORY_H_


#include <string>
#include <magic_enum.hpp>
#include <vector>

#include "Exceptions/LineInfoException.h"
#include "World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "World/Map/Map.h"
#include "World/Map/SpatialTree/SpatialTree.h"
#include "Misc/Macros.h"



class MapFactory
{
protected:
	class Type {
    public:
        enum TypeValue : unsigned int
		{
            SpatialTree
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

public:
	static std::unique_ptr<Map> createInstance(const nlohmann::json &mapInfo, WorldInterface* const worldInterface);
};

#endif // MAP_FACTORY_H_
