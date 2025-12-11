/**
 * @file Tuple.h
 * @brief Defines the Tuple class, a JSON Schema validator element representing fixed-length arrays.
 *
 * The Tuple class models a JSON Schema element of type *array* where each position
 * is validated against a distinct schema element, following the JSON Schema concept of
 * “tuple validation” (i.e., arrays whose items are heterogeneous and position-dependent).
 *
 * Unlike homogeneous arrays, a Tuple enforces:
 *  - A fixed number of items.
 *  - Validation of each index using a specific Element instance.
 *
 * This is used when the schema defines `"items"` as an array rather than an object.
 */

#ifndef TUPLE_H_
#define TUPLE_H_


#include <vector>
#include <string>
#include <utility>
#include <limits>

#include <nlohmann/json.hpp>

#include "Misc/JsonValidator/Elements/Element.h"
#include "Misc/JsonValidator/Elements/ElementFactory.h"
#include "Misc/JsonValidator/Exceptions.h"


/**
 * @class Tuple
 * @brief Represents a JSON Schema tuple-type element with position-dependent validation.
 *
 * The Tuple class models JSON arrays whose elements are heterogeneous and must match
 * different schema definitions depending on their index (“tuple validation” in JSON Schema).
 * Unlike homogeneous arrays, a Tuple enforces:
 *  - A fixed length determined by the number of schema items.
 *  - Validation of each position using its corresponding Element schema.
 *
 * This is used when the schema defines `"items"` as an array instead of an object.
 */
class Tuple : public Element
{
protected:
    /**
     * @brief List of element validators corresponding to each position in the tuple.
     *
     * Each index of the JSON array must match the schema element stored
     * at the same position inside this vector.
     */
    std::vector<Element*> items;


    /**
     * @brief Validates the provided JSON value against the tuple structure.
     *
     * Validation steps include:
     *  - Checking that the value is an array.
     *  - Ensuring that the array length matches the tuple size.
     *  - Validating each element at index *i* with `items[i]`.
     *
     * @param config JSON value to validate.
     *
     * @throws ValidatorConfigJSONException If the JSON value violates tuple constraints.
     */
    void validateElement(const nlohmann::json& config) const;

public:
    /**
     * @brief Constructs a Tuple element from its JSON schema definition.
     *
     * Reads the `"items"` field when it is defined as an array of schemas and
     * creates one Element child for each tuple position.
     *
     * @param info JSON schema definition for this tuple.
     */
    Tuple(const nlohmann::json& info);

    /**
     * @brief Copy constructor used during pattern instantiation.
     *
     * Creates a new Tuple instance by shallow-copying each Element child,
     * optionally marking the tuple as a nullified property when produced through
     * pattern replacement.
     *
     * @param other The Tuple instance to copy.
     * @param nullProperty Whether this copy should be marked as a null property.
     */
    Tuple(const Tuple& other, const bool nullProperty);

    /**
     * @brief Destructor.
     *
     * Frees all dynamically allocated child Element instances stored in `items`.
     */
    virtual ~Tuple();

    /**
     * @brief Returns the element type.
     * @return Type::tuple
     */
    Type getType() const;

    /**
     * @brief Returns the list of child element validators.
     *
     * @return Vector of Element* representing schema for each tuple index.
     */
    const std::vector<Element*>& getItems() const;
};

#endif /* TUPLE_H_ */