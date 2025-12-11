/**
 * @file Object.h
 * @brief Represents a JSON object element with properties, pattern properties, and conditional validation.
 */

#ifndef OBJECT_H_
#define OBJECT_H_


#include <unordered_map>
#include <vector>
#include <string>
#include <regex>
#include <memory>

#include <nlohmann/json.hpp>

#include "Misc/JsonValidator/Elements/Element.h"
#include "Misc/JsonValidator/Elements/ElementFactory.h"
#include "Misc/JsonValidator/Operators.h"


/**
 * @class Object
 * @brief Represents a JSON object element with nested properties and conditional rules.
 *
 * Inherits from Element. Supports standard properties, pattern-based properties,
 * and conditional rules (required, warning, or pattern-based). Provides methods
 * for property retrieval and validation against JSON configuration.
 */
class Object : public Element
{
protected:
    /// Standard properties of the object mapped by property name.
    std::unordered_map<std::string, Element*> properties;

    /// Pattern-based properties mapped by regex or pattern keys.
    std::unordered_map<std::string, Element*> patternProperties;

    /// Conditional blocks applied to this object.
    std::vector<nlohmann::json> conditionals;

    /// Conditional blocks applied to pattern properties.
    std::vector<nlohmann::json> patternConditionals;

    /// Required conditions that must be satisfied for the object to be valid.
    std::vector<nlohmann::json> requiredConditions;

    /// Warning conditions for optional validation notifications.
    std::vector<nlohmann::json> warningConditions;


    /**
     * @brief Applies a conditional block to modify object properties.
     * @param conditionalBlock JSON map representing the conditional block.
     * @param modifiedProperties Map of properties to be modified by the conditional.
     * @param patternReplacement Whether to replace patterns in property names.
     * @param replacementValue Replacement string for pattern keys.
     */
    void applyConditionalBlock(const std::unordered_map<std::string, nlohmann::json>& conditionalBlock, std::unordered_map<std::string, Element*>& modifiedProperties, const bool patternReplacement, const std::string& replacementValue) const;
    
    /**
     * @brief Applies a conditional block without pattern replacement.
     * @param conditionalBlock JSON map representing the conditional block.
     * @param modifiedProperties Map of properties to be modified by the conditional.
     */
    void applyConditionalBlock(const std::unordered_map<std::string, nlohmann::json>& conditionalBlock, std::unordered_map<std::string, Element*>& modifiedProperties) const;

    /**
     * @brief Validates that the object conforms to its schema and constraints.
     * @param config JSON object to validate.
     *
     * Overrides Element::validateElement. Checks property types, required fields,
     * conditional rules, and pattern properties.
     */
    void validateElement(const nlohmann::json& config) const;

    /**
     * @brief Verifies the content of a conditional block.
     * @param content JSON object representing the content of the conditional block.
     *
     * This method checks that the content of the conditional block is valid, including
     * the structure, allowed operators, and references to existing properties. Throws
     * an exception if the content does not meet the expected schema or rules.
     */
    void verifyConditionalBlockContent(const nlohmann::json& content) const;

    /**
     * @brief Verifies an entire conditional block.
     * @param conditionBlock JSON object representing the conditional block.
     *
     * This method validates the conditional block as a whole, ensuring that all
     * conditions, property references, and logical/arithmetic operators are correctly
     * specified. It also delegates validation of the block's content to
     * verifyConditionalBlockContent().
     */
    void verifyConditionalBlock(const nlohmann::json& conditionBlock) const;

    /**
     * @brief Verifies a single conditional within a block.
     * @param conditional JSON object representing a single conditional.
     *
     * This method checks the validity of a single conditional expression, including
     * operator types, operand types, and proper syntax. Throws an exception if any
     * part of the conditional is invalid.
     */
    void verifyConditional(const nlohmann::json& conditional) const;

    /**
     * @brief Verifies that a required condition is correctly defined.
     * @param condition JSON object representing the required condition. Passed by reference
     *                  because it may be modified during validation.
     *
     * This method ensures that required conditions are valid, properly reference existing
     * properties, and adhere to the schema rules. Throws an exception if the required
     * condition is invalid or cannot be satisfied.
     */
    void verifyRequiredCondition(nlohmann::json& condition) const;

    /**
     * @brief Verifies that a warning condition is correctly defined.
     * @param condition JSON object representing the warning condition. Passed by reference
     *                  because it may be modified during validation.
     *
     * This method ensures that warning conditions are valid and properly reference existing
     * properties. Unlike required conditions, these do not prevent validation from
     * succeeding, but may trigger warnings during the validation process.
     */
    void verifyWarningCondition(nlohmann::json& condition) const;

public:
    /**
     * @brief Constructs an Object element from a JSON configuration.
     * @param info JSON object describing properties, patterns, and conditionals.
     */
    Object(const nlohmann::json& info);
    
    /**
     * @brief Copy constructor with optional null property flag.
     * @param other Object to copy from.
     * @param nullProperty Whether the new object should represent a null property.
     */
    Object(const Object& other, const bool nullProperty);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~Object();

    /**
     * @brief Verifies that a property path exists within this object.
     * @param path Vector of property names forming the path.
     * @param currentIndex Current index in the path vector.
     *
     * Overrides Element::verifyPropertyPath. Checks nested properties and pattern properties.
     */
    void verifyPropertyPath(const std::vector<std::string>& path, const unsigned int currentIndex) const override;

    /**
     * @brief Retrieves a property by name with optional pattern replacement.
     * @param propertyName Name of the property to retrieve.
     * @param patternReplacement Whether to perform pattern replacement.
     * @param replacementValue Replacement string for pattern keys.
     * @return Pair of pointer to the Element and resolved property name.
     *
     * Overrides Element::getProperty.
     */
    std::pair<const Element* const, std::string> getProperty(const std::string& propertyName, const bool patternReplacement, const std::string& replacementValue) const override;

    /**
     * @brief Returns the type of the element.
     * @return Type::object
     */
    Type getType() const;

    /**
     * @brief Returns the standard properties of the object.
     * @return Map of property names to Element pointers.
     */
    const std::unordered_map<std::string, Element*>& getProperties() const;

    /**
     * @brief Returns the pattern-based properties of the object.
     * @return Map of pattern keys to Element pointers.
     */
    const std::unordered_map<std::string, Element*>& getPatternProperties() const;

    /**
     * @brief Returns the conditional blocks applied to this object.
     * @return Vector of JSON conditionals.
     */
    const std::vector<nlohmann::json>& getConditionals() const;

    /**
     * @brief Returns the conditional blocks applied to pattern properties.
     * @return Vector of JSON pattern conditionals.
     */
    const std::vector<nlohmann::json>& getPatternConditionals() const;

    /**
     * @brief Returns the required conditions for the object.
     * @return Vector of JSON required conditions.
     */
    const std::vector<nlohmann::json>& getRequiredConditions() const;

    /**
     * @brief Returns the warning conditions for the object.
     * @return Vector of JSON warning conditions.
     */
    const std::vector<nlohmann::json>& getWarningConditions() const;
};

#endif /* OBJECT_H_ */