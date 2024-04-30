#ifndef LINE_INFO_EXCEPTION_H_
#define LINE_INFO_EXCEPTION_H_

#include <exception>
#include <string>

/**
 * @class LineInfoException
 * @brief An exception class that includes information about the file and line where the exception occurred.
 */
class LineInfoException : public std::exception
{
private:
    std::string file;    ///< File where the exception occurred.
    int line;            ///< Line number where the exception occurred.
    std::string message; ///< Custom error message.
    std::string what_text; ///< Full exception description including file, line, and custom message.

public:
    /**
     * @brief Constructs a LineInfoException.
     * @param message Custom error message.
     * @param file File where the exception occurred.
     * @param line Line number where the exception occurred.
     */
    LineInfoException(const std::string& message, const char* file, int line);
    
    /**
     * @brief Returns a C-style character string describing the exception.
     * @return A C-style character string describing the exception.
     */
    virtual const char* what() const noexcept;
    
    /**
     * @brief Adds a message prefix to the custom error message.
     * @param newMessage The message to add as a prefix.
     */
    void addPreMessage(const std::string& newMessage);

    /**
     * @brief Adds a message suffix to the custom error message.
     * @param newMessage The message to add as a suffix.
     */
    void addPostMessage(const std::string& newMessage);
};

/**
 * @def throwLineInfoException
 * @brief Macro to throw a LineInfoException with file and line information.
 * @param msg The custom error message.
 */
#define throwLineInfoException(msg) throw LineInfoException(msg, __FILE__, __LINE__)

#endif // LINE_INFO_EXCEPTION_H_
