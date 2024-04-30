#ifndef PATCH_H_
#define PATCH_H_

#include <string>
#include <unordered_map>
#include "LineInfoException.h"

#include <magic_enum.hpp>

#define FMT_HEADER_ONLY
#include <fmt/core.h>

class Patch {
public:
	enum PatchType {
        homogeneous,
        gaussian,
		spherical,
        cubic,
        randomGaussian
	};

	inline static const PatchType stringToEnumValue(const std::string &str) { 
        try
        {
            return stringToEnum.at(str);
        }
        catch(const std::out_of_range& e) 
        {
            throwLineInfoException(fmt::format("Unknown patch type '{}'. Valid values are {}", str, printAvailableValues()));
        }
    }
	inline static constexpr size_t size() { return magic_enum::enum_count<PatchType>(); }

private:
	static const std::unordered_map<std::string_view, const PatchType> stringToEnum;
    static const std::string enumValues;

	inline static std::string_view to_string(const PatchType& patch) { return magic_enum::enum_name(patch); }
	static const std::unordered_map<std::string_view, const PatchType> generateMap();
	static const std::string generateHeader(); 
    inline static std::string_view printAvailableValues() { return enumValues; }
    static const std::string generateAvailableValues();   
};

#endif // PATCH_H_
