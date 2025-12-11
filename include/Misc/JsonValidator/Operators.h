/**
 * @file Operators.h
 * @brief Defines operator types, comparison/logical/arithmetic enums, and verification/check functions for JSON-based configuration logic.
 */

#ifndef OPERATORS_H_
#define OPERATORS_H_


#include <string>

#include <nlohmann/json.hpp>

#include "Misc/JsonValidator/Exceptions.h"
#include "Misc/EnumClass.h"
#include "Misc/JsonValidator/Elements/Element.h"


/**
 * @brief Key used to identify pattern placeholders in JSON configurations.
 *
 * This constant string represents the special key "<pattern>" that is
 * used throughout the validation and operator logic to mark pattern-based
 * properties or values.
 */
constexpr char PATTERN_KEY[] = "<pattern>";

/**
 * @enum OperatorType
 * @brief Types of operators used in JSON expressions.
 */
enum class OperatorType {
    if_else,        /**< Conditional operator (if-else). */
    comparison,     /**< Comparison operator (==, !=, <, <=, >, >=). */
    logical,        /**< Logical operator (AND, OR, NOT). */
    property,       /**< Accessing a property of an element. */
    constant,       /**< Constant value. */
    arithmetical    /**< Arithmetic operations (+, -, *, /, %). */
};

/**
 * @enum ComparisonType
 * @brief Comparison operator types.
 */
enum class ComparisonType {
    equal,              /**< Equal to (==). */
    not_equal,          /**< Not equal (!=). */
    less_than,          /**< Less than (<). */
    less_equal_than,    /**< Less than or equal to (<=). */
    greater_than,       /**< Greater than (>). */
    greater_equal_than  /**< Greater than or equal to (>=). */
};

/**
 * @enum LogicalType
 * @brief Logical operator types.
 */
enum class LogicalType {
    negation,       /**< Logical NOT. */
    conjunction,    /**< Logical AND. */
    disjunction     /**< Logical OR. */
};

/**
 * @enum PropertyElement
 * @brief Types of property elements that can be accessed.
 */
enum class PropertyElement {
    value,          /**< The value of the property. */
    item,           /**< An item in a collection property. */
    size,           /**< Size of a collection property. */
    nullProperty    /**< Represents a missing property. */
};

/**
 * @enum ArithmeticalType
 * @brief Arithmetic operation types.
 */
enum class ArithmeticalType {
    subtract,   /**< Subtraction (-). */
    addition,   /**< Addition (+). */
    division,   /**< Division (/). */
    multiply,   /**< Multiplication (*). */
    module      /**< Modulo (%). */
};


/**
 * @brief Verifies that the given JSON represents a valid if-else operator.
 * @param condition JSON representing the if-else operator.
 * @param properties Map of property names to Element pointers for property resolution.
 */
void verifyIfElseOperator(const nlohmann::json& condition, const std::unordered_map<std::string, Element*>& properties);

/**
 * @brief Verifies that the given JSON represents a valid comparison operator.
 */
void verifyComparisonOperator(const nlohmann::json& condition, const std::unordered_map<std::string, Element*>& properties);

/**
 * @brief Verifies that the given JSON represents a valid logical operator.
 */
void verifyLogicalOperator(const nlohmann::json& condition, const std::unordered_map<std::string, Element*>& properties);

/**
 * @brief Verifies that the given JSON represents a valid property access operator.
 */
void verifyPropertyOperator(const nlohmann::json& condition, const std::unordered_map<std::string, Element*>& properties);

/**
 * @brief Verifies that the given JSON represents a valid constant operator.
 */
void verifyConstantOperator(const nlohmann::json& condition);

/**
 * @brief Verifies that the given JSON represents a valid arithmetic operator.
 */
void verifyArithmeticalOperator(const nlohmann::json& condition, const std::unordered_map<std::string, Element*>& properties);

/**
 * @brief Verifies that the given JSON represents a valid operator of any type.
 */
void verifyOperator(const nlohmann::json& condition, const std::unordered_map<std::string, Element*>& properties);

/**
 * @brief Retrieves a property value from a JSON object using a relative path.
 * @param config JSON object to query.
 * @param relativePath Vector of keys representing the path to the property.
 * @param patternReplacement If true, replaces the PATTERN_KEY with replacementValue.
 * @param replacementValue Value to replace pattern placeholders with.
 * @return JSON value at the requested property.
 */
const nlohmann::json getProperty(const nlohmann::json& config, const std::vector<std::string>& relativePath, const bool patternReplacement, const std::string& replacementValue);

/**
 * @brief Evaluates an if-else operator and returns the result as JSON.
 */
const nlohmann::json checkIfElseOperator(const std::unordered_map<std::string, nlohmann::json>& condition, const nlohmann::json& config, const std::unordered_map<std::string, Element*>& properties, const bool patternReplacement, const std::string& replacementValue);

/**
 * @brief Evaluates a comparison operator and returns the result as JSON.
 */
const nlohmann::json checkComparisonOperator(const std::unordered_map<std::string, nlohmann::json>& condition, const nlohmann::json& config, const std::unordered_map<std::string, Element*>& properties, const bool patternReplacement, const std::string& replacementValue);

/**
 * @brief Evaluates a logical operator and returns the result as JSON.
 */
const nlohmann::json checkLogicalOperator(const std::unordered_map<std::string, nlohmann::json>& condition, const nlohmann::json& config, const std::unordered_map<std::string, Element*>& properties, const bool patternReplacement, const std::string& replacementValue);

/**
 * @brief Evaluates a property operator and returns the result as JSON.
 */
const nlohmann::json checkPropertyOperator(const std::unordered_map<std::string, nlohmann::json>& condition, const nlohmann::json& config, const std::unordered_map<std::string, Element*>& properties, const bool patternReplacement, const std::string& replacementValue);

/**
 * @brief Evaluates a constant operator and returns the result as JSON.
 */
const nlohmann::json checkConstantOperator(const std::unordered_map<std::string, nlohmann::json>& condition);

/**
 * @brief Evaluates an arithmetic operator and returns the result as JSON.
 */
const nlohmann::json checkArithmeticalOperator(const std::unordered_map<std::string, nlohmann::json>& condition, const nlohmann::json& config, const std::unordered_map<std::string, Element*>& properties, const bool patternReplacement, const std::string& replacementValue);

/**
 * @brief Evaluates any operator type and returns the result as JSON.
 */
const nlohmann::json checkOperator(const std::unordered_map<std::string, nlohmann::json>& condition, const nlohmann::json& config, const std::unordered_map<std::string, Element*>& properties, const bool patternReplacement, const std::string& replacementValue);

/**
 * @brief Overload: evaluates an operator without pattern replacement.
 */
const nlohmann::json checkOperator(const std::unordered_map<std::string, nlohmann::json>& condition, const nlohmann::json& config, const std::unordered_map<std::string, Element*>& properties);

#endif /* OPERATORS_H_ */