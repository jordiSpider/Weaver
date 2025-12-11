/**
 * @file String.h
 * @brief Defines the String class, a JSON Schema validator element representing string types.
 *
 * The String class models a JSON Schema element of type *string*.  
 * It supports validation of enumerations, length constraints, and regular expression patterns.
 * During validation, it ensures that the provided JSON value strictly adheres to the schema
 * rules defined in the associated configuration.
 */

#ifndef STRING_H_
#define STRING_H_


#include <vector>
#include <string>
#include <utility>
#include <regex>
#include <limits>
#include <sstream>

#include <nlohmann/json.hpp>

#include "Misc/JsonValidator/Elements/Element.h"
#include "Misc/JsonValidator/Exceptions.h"


/**
 * @class String
 * @brief Represents a string-type element in the JSON schema validation system.
 *
 * This class models a JSON string element and provides mechanisms for validating
 * configuration values according to constraints such as enumeration of allowed
 * values, minimum and maximum length, and regular expression patterns.
 *
 * It inherits from Element and implements the required validation logic for
 * string-based schema elements.
 */
class String : public Element
{
protected:
    /**
     * @brief Optional list of allowed string values.
     *
     * - first  → true if the enum constraint is defined.
     * - second → vector containing the allowed string values.
     */
    std::pair<bool, std::vector<std::string>> enumValues;

    /**
     * @brief Minimum allowed length for the string.
     */
    size_t minLength;

    /**
     * @brief Maximum allowed length for the string.
     */
    size_t maxLength;

    /**
     * @brief Optional regular expression pattern constraint.
     *
     * - first  → true if a regex pattern constraint is defined.
     * - second → the regex pattern string.
     */
    std::pair<bool, std::string> patternStr;


    /**
     * @brief Validates a configuration entry against the string constraints.
     *
     * @param config JSON configuration entry to validate.
     *
     * This method enforces:
     * - type checking (must be a string),
     * - minimum and maximum length,
     * - allowed enum values,
     * - optional regex pattern validation.
     *
     * Throws an exception if validation fails.
     */
    void validateElement(const nlohmann::json& config) const;

public:
    /**
     * @brief Constructs a String element from its schema definition.
     * @param info JSON object containing the schema definition for this string.
     */
    String(const nlohmann::json& info);

    /**
     * @brief Copy constructor used when cloning or overriding properties.
     * @param other Original String element.
     * @param nullProperty Whether the cloned element should behave as null/ignored.
     */
    String(const String& other, const bool nullProperty);

    /**
     * @brief Destructor.
     */
    virtual ~String();

    /**
     * @brief Returns the type of this element.
     * @return Type::string.
     */
    Type getType() const;

    /**
     * @brief Returns the enum constraint for the string.
     *
     * @return Pair where:
     *         - first  → indicates if enum constraint is active,
     *         - second → list of allowed string values.
     */
    const std::pair<bool, std::vector<std::string>>& getEnumValues() const;

    /**
     * @brief Returns the minimum allowed length.
     */
    size_t getMinLength() const;

    /**
     * @brief Returns the maximum allowed length.
     */
    size_t getMaxLength() const;

    /**
     * @brief Returns the optional regex pattern constraint.
     *
     * @return Pair where:
     *         - first  → indicates if a pattern constraint is active,
     *         - second → regex pattern string.
     */
    const std::pair<bool, std::string>& getPatternStr() const;
};

#endif /* STRING_H_ */