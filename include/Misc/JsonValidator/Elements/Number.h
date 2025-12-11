/**
 * @file Number.h
 * @brief Represents a JSON number element with validation and constraints.
 */

#ifndef NUMBER_H_
#define NUMBER_H_


#include <unordered_map>
#include <string>
#include <limits>

#include <nlohmann/json.hpp>

#include "Misc/JsonValidator/Elements/Element.h"
#include "Misc/JsonValidator/Exceptions.h"


/**
 * @class Number
 * @brief Represents a floating-point number element in a JSON configuration.
 *
 * Inherits from Element. Provides validation for numeric values,
 * including minimum, maximum, and exclusivity constraints.
 */
class Number : public Element
{
protected:
    /// Minimum allowed value for the number.
    double minimum;

    /// Flag indicating if the minimum is exclusive.
    bool exclusiveMinimum;

    /// Maximum allowed value for the number.
    double maximum;

    /// Flag indicating if the maximum is exclusive.
    bool exclusiveMaximum;


    /**
     * @brief Validates that the JSON value is a number and respects constraints.
     * @param config JSON object representing the number value.
     *
     * Overrides Element::validateElement. Throws an exception if the value
     * does not respect the min/max constraints.
     */
    void validateElement(const nlohmann::json& config) const;

public:
    /**
     * @brief Constructs a Number element from a JSON configuration.
     * @param info JSON object containing numeric metadata and constraints.
     */
    Number(const nlohmann::json& info);

    /**
     * @brief Copy constructor with optional null property flag.
     * @param other Number instance to copy from.
     * @param nullProperty Whether the new instance should represent a null property.
     */
    Number(const Number& other, const bool nullProperty);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Number();

    /**
     * @brief Returns the type of the element.
     * @return Type::number
     */
    Type getType() const;

    /**
     * @brief Returns the minimum allowed value.
     * @return Minimum number.
     */
    const double& getMinimum() const;

    /**
     * @brief Checks if the minimum value is exclusive.
     * @return True if minimum is exclusive, false otherwise.
     */
    bool getExclusiveMinimum() const;

    /**
     * @brief Returns the maximum allowed value.
     * @return Maximum number.
     */
    const double& getMaximum() const;

    /**
     * @brief Checks if the maximum value is exclusive.
     * @return True if maximum is exclusive, false otherwise.
     */
    bool getExclusiveMaximum() const;
};

#endif /* NUMBER_H_ */