/**
 * @file Integer.h
 * @brief Represents a JSON integer element with validation and constraints.
 */

#ifndef INTEGER_H_
#define INTEGER_H_


#include <unordered_map>
#include <string>
#include <limits>

#include <nlohmann/json.hpp>

#include "Misc/JsonValidator/Elements/Element.h"
#include "Misc/JsonValidator/Exceptions.h"


/**
 * @class Integer
 * @brief Represents an integer element in a JSON configuration.
 *
 * Inherits from Element. Provides validation for integer values,
 * including minimum, maximum, exclusivity, and multiplicity constraints.
 */
class Integer : public Element
{
protected:
    /// Minimum allowed value for the integer.
    int minimum;

    /// Flag indicating if the minimum is exclusive.
    bool exclusiveMinimum;

    /// Maximum allowed value for the integer.
    int maximum;

    /// Flag indicating if the maximum is exclusive.
    bool exclusiveMaximum;

    /// Optional multiplicity constraint (value must be a multiple of the given integer).
    std::pair<bool, int> multipleOf;


    /**
     * @brief Validates that the JSON value is an integer and respects constraints.
     * @param config JSON object representing the integer value.
     *
     * Overrides Element::validateElement. Throws an exception if the value
     * does not respect the min/max/multipleOf constraints.
     */
    void validateElement(const nlohmann::json& config) const;

public:
    /**
     * @brief Constructs an Integer element from a JSON configuration.
     * @param info JSON object containing integer metadata and constraints.
     */
    Integer(const nlohmann::json& info);

    /**
     * @brief Copy constructor with optional null property flag.
     * @param other Integer instance to copy from.
     * @param nullProperty Whether the new instance should represent a null property.
     */
    Integer(const Integer& other, const bool nullProperty);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~Integer();

    /**
     * @brief Returns the type of the element.
     * @return Type::integer
     */
    Type getType() const;

    /**
     * @brief Returns the minimum allowed value.
     * @return Minimum integer.
     */
    int getMinimum() const;

    /**
     * @brief Checks if the minimum value is exclusive.
     * @return True if minimum is exclusive, false otherwise.
     */
    bool getExclusiveMinimum() const;

    /**
     * @brief Returns the maximum allowed value.
     * @return Maximum integer.
     */
    int getMaximum() const;

    /**
     * @brief Checks if the maximum value is exclusive.
     * @return True if maximum is exclusive, false otherwise.
     */
    bool getExclusiveMaximum() const;

    /**
     * @brief Returns the multiplicity constraint.
     * @return Pair of <isDefined, multipleOfValue>.
     */
    const std::pair<bool, int>& getMultipleOf() const;
};

#endif /* INTEGER_H_ */