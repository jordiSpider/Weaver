#ifndef WORLD_FACTORY_H_
#define WORLD_FACTORY_H_


#include <memory>
#include <string>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Exceptions/LineInfoException.h"
#include "Misc/Types.h"
#include "IBM/World/WorldInterface.h"
#include "IBM/World/ArthropodsWorld.h"
#include "IBM/World/DinosaursWorld.h"
#include "IBM/World/AquaticWorld.h"
#include "Misc/Macros.h"


class WorldFactory
{
private:
    friend class boost::serialization::access;

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

        /**
         * @brief Serialize the Type object.
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


	static std::unique_ptr<WorldInterface> createInstance(nlohmann::json* jsonTree, nlohmann::json &worldConfig, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath, int burnIn);

    /**
     * @brief Serialize the WorldFactory object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif // WORLD_FACTORY_H_
