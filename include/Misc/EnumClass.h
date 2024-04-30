#ifndef ENUM_CLASS_H_
#define ENUM_CLASS_H_


#include <string>
#include <unordered_map>
#include <magic_enum.hpp>

#include "Misc/Macros.h"
#include "Exceptions/LineInfoException.h"


template <typename T>
class EnumClass {
public:
    static const T stringToEnumValue(const std::string &str);
    static constexpr size_t size();
	static const std::vector<T> getEnumValues();
	static std::string_view to_string(const T& type);
	static const std::string getHeader();
	static const std::string printValues();

private:
    static const std::unordered_map<std::string_view, const T> generateMap();
};


template <typename T>
constexpr size_t EnumClass<T>::size() 
{ 
	return magic_enum::enum_count<T>(); 
}

template <typename T>
const std::vector<T> EnumClass<T>::getEnumValues() 
{ 
	std::vector<T> values;

	for(const auto &value : magic_enum::enum_values<T>())
	{
		values.push_back(value);
	}

	return values;
}

template <typename T>
std::string_view EnumClass<T>::to_string(const T& type) 
{ 
	return magic_enum::enum_name(type); 
}

template <typename T>
const std::string EnumClass<T>::getHeader()
{
	constexpr auto enumNames = magic_enum::enum_names<T>();

	std::string header = fmt::format("{}", enumNames[0]);
	for(size_t i = 1; i < enumNames.size(); i++) {
		header += fmt::format("\t{}", enumNames[i]);
	}

	return header;
}

template <typename T>
const std::string EnumClass<T>::printValues()
{
	constexpr auto enumNames = magic_enum::enum_names<T>();

	std::string values = fmt::format("{}", enumNames[0]);
	for(size_t i = 1; i < enumNames.size(); i++) {
		values += fmt::format(", {}", enumNames[i]);
	}

	return values;
}

template <typename T>
const std::unordered_map<std::string_view, const T> EnumClass<T>::generateMap() 
{
	std::unordered_map<std::string_view, const T> enumMap;

	for(size_t i = 0; i < EnumClass<T>::size(); i++) {
		const T type = static_cast<const T>(i);
		enumMap.insert({EnumClass<T>::to_string(type), type});
	}

	return enumMap;
}

template <typename T>
const T EnumClass<T>::stringToEnumValue(const std::string &str) { 
	const std::unordered_map<std::string_view, const T> stringToEnum = EnumClass<T>::generateMap();
    
    try
	{
		return stringToEnum.at(str);
	}
	catch(const std::out_of_range& e) 
	{
		throwLineInfoException(fmt::format("Unknown moisture type '{}'. Valid values are {}", str, EnumClass<T>::printValues()));
	}
}

#endif // ENUM_CLASS_H_
