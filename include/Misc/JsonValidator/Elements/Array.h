/**
 * @file Array.h
 * @brief Represents a JSON array element with validation and ordering constraints.
 */

#ifndef ARRAY_H_
#define ARRAY_H_


#include <vector>
#include <string>
#include <utility>
#include <limits>

#include <nlohmann/json.hpp>

#include "Misc/JsonValidator/Elements/Element.h"
#include "Misc/JsonValidator/Elements/ElementFactory.h"
#include "Misc/JsonValidator/Exceptions.h"


/**
 * @class Array
 * @brief Represents an array element in a JSON configuration.
 *
 * Inherits from Element. Provides mechanisms to define constraints on array size,
 * validate contained items, and specify ordering rules (increasing, decreasing, etc.).
 */
class Array : public Element
{
protected:
    /**
     * @brief Enum representing ordering constraints for array items.
     */
    enum class ItemsOrder {
        increasing,          ///< Items must be in non-decreasing order.
        strictly_increasing, ///< Items must be in strictly increasing order.
        decreasing,          ///< Items must be in non-increasing order.
        strictly_decreasing  ///< Items must be in strictly decreasing order.
    };


    size_t minItems; ///< Minimum number of items allowed in the array.
    size_t maxItems; ///< Maximum number of items allowed in the array.

    Element* items;  ///< Pointer to the element describing the type/structure of array items.

    std::pair<bool, ItemsOrder> itemsOrder; ///< Pair indicating whether ordering is required and its type.


    /**
     * @brief Validates the array against its configuration constraints.
     * @param config JSON object representing the array.
     *
     * Checks item type, array length, and ordering constraints. Overrides Element::validateElement.
     */
    void validateElement(const nlohmann::json& config) const;

public:
    /**
     * @brief Constructs an Array from JSON configuration.
     * @param info JSON object containing array metadata and constraints.
     */
    Array(const nlohmann::json& info);

    /**
     * @brief Copy constructor with optional null property flag.
     * @param other Array instance to copy from.
     * @param nullProperty Whether the new instance should represent a null property.
     */
    Array(const Array& other, const bool nullProperty);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~Array();

    /**
     * @brief Returns the type of the element.
     * @return Type::array
     */
    Type getType() const;

    /**
     * @brief Returns the minimum number of items allowed in the array.
     * @return Minimum items.
     */
    size_t getMinItems() const;

    /**
     * @brief Returns the maximum number of items allowed in the array.
     * @return Maximum items.
     */
    size_t getMaxItems() const;

    /**
     * @brief Returns the element type descriptor for the array items.
     * @return Pointer to the items element.
     */
    Element* const& getItems() const;

    /**
     * @brief Returns the ordering constraint for array items.
     * @return Pair indicating if ordering is enforced and the type of order.
     */
    const std::pair<bool, ItemsOrder>& getItemsOrder() const;
};

#endif /* ARRAY_H_ */