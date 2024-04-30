#ifndef ANIMAL_FACTORY_H_
#define ANIMAL_FACTORY_H_


#include <string>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"



class AnimalFactory
{
private:
    friend class boost::serialization::access;

protected:
	class Type {
    public:
        enum TypeValue : unsigned int
		{
            Default
		};

        static const TypeValue stringToEnumValue(const std::string &str);
        static constexpr size_t size();

        /**
         * @brief Serialize the Trait object.
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
};

#endif // ANIMAL_FACTORY_H_
