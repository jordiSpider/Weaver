#ifndef LINE_INFO_EXCEPTION_H_
#define LINE_INFO_EXCEPTION_H_

#include <exception>
#include <string>

class LineInfoException : public std::exception
{
private:
    std::string file;
    int line;
    std::string message;
    std::string what_text;
public:
    LineInfoException(const std::string& message, const char* file, int line);
    virtual const char* what() const noexcept;
    void addPreMessage(const std::string& newMessage);
    void addPostMessage(const std::string& newMessage);
};

#define throwLineInfoException(msg) throw LineInfoException(msg, __FILE__, __LINE__)

#endif // LINE_INFO_EXCEPTION_H_
