#ifndef MAP_FACTORY_H_
#define MAP_FACTORY_H_


#include <string>
#include <magic_enum.hpp>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Exceptions/LineInfoException.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/Map/Map.h"
#include "IBM/World/Map/SpatialTree/SpatialTree.h"
#include "Misc/Macros.h"



class MapFactory
{
private:
    friend class boost::serialization::access;

protected:
    class Type {
        public:
            enum TypeValue : unsigned int
            {
                SpatialTree
            };

            static const TypeValue stringToEnumValue(const std::string &str);
            static constexpr size_t size();

            /**
             * @brief Serialize the MapFactory object.
             * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
             * @param ar The archive to use.
             * @param version The version of the serialization format.
             */
            template <class Archive>
            void serialize(Archive &ar, const unsigned int version);

        private:
            static const std::unordered_map<std::string_view, const TypeValue> stringToEnum;
            static const std::string enumValues;

            friend class boost::serialization::access;

            static std::string_view to_string(const TypeValue& type);
            static const std::unordered_map<std::string_view, const TypeValue> generateMap();
            static const std::string generateAvailableValues();
            static std::string_view printAvailableValues();
    };

public:
    static std::unique_ptr<Map> createInstance(const nlohmann::json &mapInfo, WorldInterface* const worldInterface);

    /**
     * @brief Serialize the MapFactory object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif // MAP_FACTORY_H_
