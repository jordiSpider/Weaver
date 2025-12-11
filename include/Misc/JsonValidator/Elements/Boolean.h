/**
 * @file Boolean.h
 * @brief Represents a JSON boolean element with validation.
 */

#ifndef BOOLEAN_H_
#define BOOLEAN_H_


#include <unordered_map>
#include <string>

#include <nlohmann/json.hpp>

#include "Misc/JsonValidator/Elements/Element.h"
#include "Misc/JsonValidator/Exceptions.h"


/**
 * @class Boolean
 * @brief Represents a boolean element in a JSON configuration.
 *
 * Inherits from Element. Provides validation for JSON boolean values.
 */
class Boolean : public Element
{
protected:
    /**
     * @brief Validates that the JSON value is a boolean.
     * @param config JSON object representing the boolean value.
     *
     * Overrides Element::validateElement. Throws an exception if the value is not a boolean.
     */
    void validateElement(const nlohmann::json& config) const;

public:
    /**
     * @brief Constructs a Boolean element from a JSON configuration.
     * @param info JSON object containing boolean metadata.
     */
    Boolean(const nlohmann::json& info);

    /**
     * @brief Copy constructor with optional null property flag.
     * @param other Boolean instance to copy from.
     * @param nullProperty Whether the new instance should represent a null property.
     */
    Boolean(const Boolean& other, const bool nullProperty);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Boolean();

    /**
     * @brief Returns the type of the element.
     * @return Type::boolean
     */
    Type getType() const;
};

#endif /* BOOLEAN_H_ */