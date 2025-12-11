/**
 * @file Element.h
 * @brief Abstract base class representing a configurable element in JSON-based configurations.
 */

#ifndef ELEMENT_H_
#define ELEMENT_H_


#include <vector>
#include <string>
#include <utility>

#include <nlohmann/json.hpp>

#include "Misc/JsonValidator/Exceptions.h"


/**
 * @class Element
 * @brief Abstract base class for elements that can be validated and accessed in a configuration.
 *
 * Elements represent structured or primitive data in a JSON configuration. This class provides
 * an interface for validation, property access, and metadata management such as description
 * and modification status.
 */
class Element
{
protected:
    bool modified;          ///< Indicates whether the element has been modified.
    bool nullProperty;      ///< Indicates whether the element represents a null property.
    std::string description;///< Description of the element.

    /**
     * @brief Validates the element against a JSON configuration.
     * @param config JSON object to validate against.
     *
     * This pure virtual function must be implemented by derived classes to define
     * element-specific validation rules.
     */
    virtual void validateElement(const nlohmann::json& config) const=0;

public:
    /**
     * @brief Enum representing the type of the element.
     */
    enum class Type {
        object,   ///< JSON object
        array,    ///< JSON array
        tuple,    ///< Tuple structure
        string,   ///< String
        integer,  ///< Integer number
        number,   ///< Floating point number
        boolean   ///< Boolean value
    };

    /**
     * @brief Constructs an Element from JSON information.
     * @param info JSON object containing element data.
     */
    Element(const nlohmann::json& info);

    /**
     * @brief Constructs an Element with specified null property flag and description.
     * @param nullProperty Whether this element represents a null property.
     * @param description Description of the element.
     */
    Element(const bool nullProperty, const std::string& description);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Element();

    /**
     * @brief Checks if the element represents a null property.
     * @return True if null property, false otherwise.
     */
    bool isNullProperty() const;

    /**
     * @brief Checks if the element has been modified.
     * @return True if modified, false otherwise.
     */
    bool isModified() const;

    /**
     * @brief Returns the element description.
     * @return Description string.
     */
    const std::string& getDescription() const;

    /**
     * @brief Validates the element against a JSON configuration.
     * @param config JSON object to validate against.
     *
     * Calls the derived class's validateElement() implementation.
     */
    void validate(const nlohmann::json& config) const;

    /**
     * @brief Verifies that a property path exists in the element.
     * @param path Vector of property names representing the path.
     * @param currentIndex Current index in the path traversal.
     *
     * Can be overridden by derived classes to enforce structured property checks.
     */
    virtual void verifyPropertyPath(const std::vector<std::string>& path, const unsigned int currentIndex) const;

    /**
     * @brief Retrieves a child property by name with optional pattern replacement.
     * @param propertyName Name of the property to retrieve.
     * @param patternReplacement Whether to perform pattern replacement in strings.
     * @param replacementValue Value to replace the pattern with.
     * @return Pair of pointer to the Element and its description.
     *
     * Can be overridden by derived classes for structured access.
     */
    virtual std::pair<const Element* const, std::string> getProperty(const std::string& propertyName, const bool patternReplacement, const std::string& replacementValue) const;
    
    /**
     * @brief Retrieves a child property by name without pattern replacement.
     * @param propertyName Name of the property to retrieve.
     * @return Pair of pointer to the Element and its description.
     */
    std::pair<const Element* const, std::string> getProperty(const std::string& propertyName) const;

    /**
     * @brief Returns the type of the element.
     * @return Element type (Type enum).
     *
     * Pure virtual function to be implemented by derived classes.
     */
    virtual Type getType() const=0;
};

#endif /* ELEMENT_H_ */