
#include "Exceptions/LineInfoException.h"

using namespace std;

LineInfoException::LineInfoException(const std::string& message, const char* file, int line)
    : line(line), message(message)
{
    this->file = string(file);
    what_text = this->file + ": Exception thrown at line " + to_string(line) + ": " + message;
}

const char* LineInfoException::what() const noexcept
{
    return what_text.c_str();
}

void LineInfoException::addPreMessage(const string& newMessage)
{
    message = newMessage + message;
    what_text = file + ": Exception thrown at line " + to_string(line) + ": " + message;
}

void LineInfoException::addPostMessage(const string& newMessage)
{
    message += newMessage;
    what_text = file + ": Exception thrown at line " + to_string(line) + ": " + message;
}
