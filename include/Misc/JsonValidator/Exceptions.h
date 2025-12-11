/**
 * @file Exceptions.h
 * @brief Defines JSON validator exception classes with line/file information for error reporting.
 */

#ifndef JSON_VALIDATOR_EXCEPTIONS_H_
#define JSON_VALIDATOR_EXCEPTIONS_H_


#include <string>

#include "Exceptions/LineInfoException.h"


/**
 * @class ValidatorJSONException
 * @brief Base exception for JSON validation errors.
 *
 * Inherits from LineInfoException to include file and line number information.
 */
class ValidatorJSONException : public LineInfoException
{
public:
    /**
     * @brief Constructs a ValidatorJSONException.
     * @param message Error message describing the problem.
     * @param file Source file where the exception occurred.
     * @param line Line number in the source file.
     */
    ValidatorJSONException(const std::string& message, const char* file, int line);
};

/**
 * @def throwValidatorJSONException
 * @brief Convenience macro to throw ValidatorJSONException with current file and line.
 */
#define throwValidatorJSONException(msg) throw ValidatorJSONException(msg, __FILE__, __LINE__)


/**
 * @class ValidatorConfigJSONException
 * @brief Exception thrown when the JSON configuration is invalid.
 *
 * Inherits from ValidatorJSONException.
 */
class ValidatorConfigJSONException : public ValidatorJSONException
{
public:
    /**
     * @brief Constructs a ValidatorConfigJSONException.
     * @param message Error message describing the configuration issue.
     * @param file Source file where the exception occurred.
     * @param line Line number in the source file.
     */
    ValidatorConfigJSONException(const std::string& message, const char* file, int line);
};

/**
 * @def throwValidatorConfigJSONException
 * @brief Convenience macro to throw ValidatorConfigJSONException with current file and line.
 */
#define throwValidatorConfigJSONException(msg) throw ValidatorConfigJSONException(msg, __FILE__, __LINE__)


/**
 * @class ValidatorSchemaJSONException
 * @brief Exception thrown when the JSON schema is invalid.
 *
 * Inherits from ValidatorJSONException.
 */
class ValidatorSchemaJSONException : public ValidatorJSONException
{
public:
    /**
     * @brief Constructs a ValidatorSchemaJSONException.
     * @param message Error message describing the schema problem.
     * @param file Source file where the exception occurred.
     * @param line Line number in the source file.
     */
    ValidatorSchemaJSONException(const std::string& message, const char* file, int line);
};

/**
 * @def throwValidatorSchemaJSONException
 * @brief Convenience macro to throw ValidatorSchemaJSONException with current file and line.
 */
#define throwValidatorSchemaJSONException(msg) throw ValidatorSchemaJSONException(msg, __FILE__, __LINE__)

#endif /* JSON_VALIDATOR_EXCEPTIONS_H_ */
