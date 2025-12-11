#include "Misc/JsonValidator/Exceptions.h"


using namespace std;



ValidatorJSONException::ValidatorJSONException(const std::string& message, const char* file, int line) 
    : LineInfoException(message, file, line) 
{

}

ValidatorConfigJSONException::ValidatorConfigJSONException(const std::string& message, const char* file, int line) 
    : ValidatorJSONException(message, file, line) 
{

}

ValidatorSchemaJSONException::ValidatorSchemaJSONException(const std::string& message, const char* file, int line) 
    : ValidatorJSONException(message, file, line) 
{
    
}

