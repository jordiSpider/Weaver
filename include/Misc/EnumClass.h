/**
 * @file EnumClass.h
 * @brief Template helper class for working with strongly-typed enums.
 *
 * This class provides utility functions for converting enums to strings,
 * strings to enum values, obtaining all enum values, and printing headers.
 * It uses the `magic_enum` library to handle enum reflection at compile time.
 */

#ifndef ENUM_CLASS_H_
#define ENUM_CLASS_H_


#include <sstream>
#include <string>
#include <unordered_map>
#include <magic_enum/magic_enum.hpp>

#include "Exceptions/LineInfoException.h"

/**
 * @class EnumClass
 * @brief Provides utility functions for enum reflection and conversion.
 *
 * This template class allows converting enum values to strings and vice versa,
 * retrieving all enum values, printing enum names as headers, and generating
 * a string listing all enum values.
 *
 * @tparam T The enum type.
 */
template <typename T>
class EnumClass {
public:
	/**
     * @brief Converts a string to its corresponding enum value.
     *
     * Throws a LineInfoException if the string does not match any enum value.
     * 
     * @param str The string representation of the enum.
     * @return The corresponding enum value.
     */
    static T stringToEnumValue(const std::string &str);
    
	/**
     * @brief Returns the number of enum entries.
     * @return Number of entries in the enum.
     */
	static constexpr size_t size();

	/**
     * @brief Returns a vector containing all enum values.
     * @return Vector of all enum values.
     */
	static const std::vector<T> getEnumValues();
	
	/**
     * @brief Converts an enum value to its string representation.
     * @param type The enum value.
     * @return The string name of the enum value.
     */
	static std::string to_string(const T& type);
	
	/**
     * @brief Returns a tab-separated string of all enum names (useful as a header).
     * @return Tab-separated string of enum names.
     */
	static const std::string getHeader();
	
	/**
     * @brief Returns a comma-separated string of all enum names.
     * @return Comma-separated string of enum names.
     */
	static const std::string printValues();

private:
	/**
     * @brief Generates a mapping from strings to enum values.
     * @return Unordered map from enum name strings to enum values.
     */
    static const std::unordered_map<std::string, const T> generateMap();
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
std::string EnumClass<T>::to_string(const T& type) 
{ 
	return std::string(magic_enum::enum_name(type)); 
}

template <typename T>
const std::string EnumClass<T>::getHeader()
{
	constexpr auto enumNames = magic_enum::enum_names<T>();

	std::ostringstream header;

	header << enumNames[0];
	for(size_t i = 1; i < enumNames.size(); i++) {
		header << "\t" << enumNames[i];
	}

	return header.str();
}

template <typename T>
const std::string EnumClass<T>::printValues()
{
	constexpr auto enumNames = magic_enum::enum_names<T>();

	std::ostringstream values;

	values << enumNames[0];
	for(size_t i = 1; i < enumNames.size(); i++) {
		values << ", " << enumNames[i];
	}

	return values.str();
}

template <typename T>
const std::unordered_map<std::string, const T> EnumClass<T>::generateMap() 
{
	std::unordered_map<std::string, const T> enumMap;

	for(size_t i = 0; i < EnumClass<T>::size(); i++) {
		const T type = static_cast<const T>(i);
		enumMap.insert({EnumClass<T>::to_string(type), type});
	}

	return enumMap;
}

template <typename T>
T EnumClass<T>::stringToEnumValue(const std::string &str) { 
	const std::unordered_map<std::string, const T> stringToEnum = EnumClass<T>::generateMap();
    
    try
	{
		return stringToEnum.at(str);
	}
	catch(const std::out_of_range&) 
	{
		std::ostringstream message;

		message << "Unknown enum type '" << str << "'. Valid values are " << EnumClass<T>::printValues();

		throwLineInfoException(message.str());
	}
}

#endif // ENUM_CLASS_H_
