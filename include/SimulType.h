#ifndef SIMUL_TYPE_H_
#define SIMUL_TYPE_H_

#include <string>
#include <unordered_map>
#include "LineInfoException.h"

#include <magic_enum.hpp>

#define FMT_HEADER_ONLY
#include <fmt/core.h>

class SimulType {
public:
	enum SimulTypeValue {
		arthropods, 
		dinosaurs
	};

	SimulType() {};
	SimulType(const std::string& typeStr);
	~SimulType() {};

	inline const SimulTypeValue& getValue() const { return value; }
	inline operator int() const { return value; }
	inline SimulType& operator=(const SimulType& other) {
        this->value = other.value;
        return *this;
    }

private:
	static const std::unordered_map<std::string_view, const SimulTypeValue> stringToEnum;
	static const std::string enumValues;

	inline static SimulTypeValue stringToEnumValue(const std::string &str) { 
		try
		{
			return stringToEnum.at(str);
		}
		catch(const std::out_of_range& e) 
		{
			throwLineInfoException(fmt::format("Unknown simulation type '{}'. Valid values are {}", str, printAvailableValues()));
		}
	}
	inline static std::string_view printAvailableValues() { return enumValues; }
	inline static std::string_view to_string(const SimulTypeValue& simulType) { return magic_enum::enum_name(simulType); }
	static const std::unordered_map<std::string_view, const SimulTypeValue> generateMap();
	inline static constexpr size_t size() { return magic_enum::enum_count<SimulTypeValue>(); }
	static const std::string generateAvailableValues();

	SimulTypeValue value;
};

#endif // SIMUL_TYPE_H_
