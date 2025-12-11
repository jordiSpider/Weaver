/**
 * @file ElementFactory.h
 * @brief Factory class for creating Element instances from JSON or existing elements.
 */

#ifndef ELEMENT_FACTORY_H_
#define ELEMENT_FACTORY_H_


#include <memory>

#include <nlohmann/json.hpp>

#include "Misc/JsonValidator/Elements/Element.h"
#include "Misc/JsonValidator/Elements/Array.h"
#include "Misc/JsonValidator/Elements/Tuple.h"
#include "Misc/JsonValidator/Elements/String.h"
#include "Misc/JsonValidator/Elements/Integer.h"
#include "Misc/JsonValidator/Elements/Number.h"
#include "Misc/JsonValidator/Elements/Boolean.h"
#include "Misc/JsonValidator/Elements/Object.h"
#include "Misc/JsonValidator/Exceptions.h"
#include "Misc/EnumClass.h"


/**
 * @class ElementFactory
 * @brief Factory for constructing Element objects.
 *
 * Provides static methods to create new Element instances either from a JSON configuration
 * or by copying an existing Element, optionally producing a null property.
 */
class ElementFactory
{
public:
    /**
     * @brief Creates a new Element instance based on JSON information.
     * @param info JSON object containing the information to configure the Element.
     * @return Pointer to the newly created Element instance.
     *
     * The created Element is dynamically allocated and should be managed appropriately
     * by the caller.
     */
    static Element* createInstance(const nlohmann::json& info);

    /**
     * @brief Creates a new Element instance by copying an existing Element.
     * @param other Reference to the Element to copy.
     * @param nullProperty If true, the created Element will represent a null property.
     * @return Pointer to the newly created Element instance.
     *
     * The created Element is dynamically allocated and should be managed appropriately
     * by the caller.
     */
    static Element* createInstance(const Element& other, const bool nullProperty);
};

#endif /* ELEMENT_FACTORY_H_ */