#ifndef PATCH_FACTORY_H_
#define PATCH_FACTORY_H_


#include <string>
#include <magic_enum.hpp>

#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"



class PatchFactory
{
protected:
	class Type {
    public:
        enum TypeValue : unsigned int
		{
            Homogeneous,
            Gaussian,
            Spherical,
            Cubic,
            RandomGaussian
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
};

#endif // PATCH_FACTORY_H_
