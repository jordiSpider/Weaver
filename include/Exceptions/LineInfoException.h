/**
 * @file LineInfoException.h
 * @brief Exception class with detailed file and line information for error reporting.
 *
 * This file defines the `LineInfoException` class, which extends `std::exception` to include
 * the source file and line number where the exception was thrown. It also provides
 * utilities to prepend or append messages to the exception description.
 */

#ifndef LINE_INFO_EXCEPTION_H_
#define LINE_INFO_EXCEPTION_H_

#include <exception>
#include <string>

/**
 * @class LineInfoException
 * @brief Exception class that stores file, line, and custom error message.
 *
 * The `LineInfoException` class extends `std::exception` and adds
 * information about the source file and line number where the exception
 * occurred. This is useful for debugging and detailed error reporting
 * in the Weaver framework.
 *
 * ### Features
 * - Stores the file name and line number.
 * - Stores a custom message.
 * - Automatically formats a full description including file, line, and message.
 * - Supports prepending and appending additional messages dynamically.
 *
 * ### Example usage
 * @code
 * if(condition) {
 *     throwLineInfoException("Unexpected condition occurred");
 * }
 * @endcode
 */
class LineInfoException : public std::exception
{
private:
    std::string file;      /**< Source file where the exception occurred */
    int line;              /**< Line number where the exception occurred */
    std::string message;   /**< Custom error message */
    std::string what_text; /**< Complete exception description including file, line, and message */

public:
    /**
     * @brief Constructs a LineInfoException with a message, file, and line number.
     * @param message Custom error message describing the exception.
     * @param file Source file where the exception occurred (usually __FILE__).
     * @param line Line number where the exception occurred (usually __LINE__).
     */
    LineInfoException(const std::string& message, const char* file, int line);
    
    /**
     * @brief Returns a C-style character string describing the exception.
     *
     * Overrides `std::exception::what()` to return the full exception description
     * including file, line, and custom message.
     *
     * @return const char* C-string with the full exception description.
     */
    virtual const char* what() const noexcept;
    
    /**
     * @brief Adds a prefix message to the current exception message.
     *
     * Useful to provide additional context before the main exception message.
     *
     * @param newMessage The message to prepend.
     */
    void addPreMessage(const std::string& newMessage);

    /**
     * @brief Adds a suffix message to the current exception message.
     *
     * Useful to provide additional context after the main exception message.
     *
     * @param newMessage The message to append.
     */
    void addPostMessage(const std::string& newMessage);
};

/**
 * @def throwLineInfoException(msg)
 * @brief Macro to throw a LineInfoException including the current file and line.
 *
 * Simplifies throwing `LineInfoException` with automatic file and line insertion.
 *
 * @param msg Custom message describing the error.
 *
 * @code
 * throwLineInfoException("Division by zero detected");
 * @endcode
 */
#define throwLineInfoException(msg) throw LineInfoException(msg, __FILE__, __LINE__)

#endif // LINE_INFO_EXCEPTION_H_
