#ifndef VALIDATOR_JSON_H_
#define VALIDATOR_JSON_H_


#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <regex>
#include <fstream>
#include <stdexcept>
#include "nlohmann/json.h"
#include "LineInfoException.h"




class TypeElemJSONConverter {
public:
    enum TypeElemJSON {
        Object,
        Array,
        String, 
        Integer, 
        Unsigned_Integer, 
        Float, 
        Boolean
    };

    inline static TypeElemJSON stringToEnumValue(const std::string &str) { return stringToEnum.at(str); }
    static const std::string printValues();
private:
    static std::unordered_map<std::string, TypeElemJSON> stringToEnum;
};


#define PATTERN_ELEM "<pattern>"

class OperatorsJSONConverter {
public:
    enum OperatorsJSON {
        If,
        Equal,
        Not_Equal,
        Less_Than, 
        Greater_Than, 
        Less_Equal_Than, 
        Greater_Equal_Than, 
        Not,
        And,
        Or,
        Property,
        Property_Item,
        Property_Size,
        Property_Exists,
        Constant,
        Subtract,
        Sum,
        Division,
        Multiply,
        Module
    };

    inline static OperatorsJSON stringToEnumValue(const std::string &str) { return stringToEnum.at(str); }
    static const std::string printValues();
private:
    static std::unordered_map<std::string, OperatorsJSON> stringToEnum;
};


class ItemsOrderConverter {
public:
    enum ItemsOrder {
        Increasing,
        Strictly_Increasing,
        Decreasing, 
        Strictly_Decreasing
    };

    inline static ItemsOrder stringToEnumValue(const std::string &str) { return stringToEnum.at(str); }
    static const std::string printValues();
private:
    static std::unordered_map<std::string, ItemsOrder> stringToEnum;
};




class RegexKey
{
private:
    std::string regexValue;
    std::regex pattern;
public:
    RegexKey(const std::string& regexValue);
    bool operator==(const std::string &other) const;
    inline std::string getRegexValue() const { return regexValue; }
};


class ListRegexKey
{
private:
    std::list<RegexKey> listRegex;
public:
    ListRegexKey() {}
    ListRegexKey(const nlohmann::json* list);
    void findRegexKey(const std::string& key) const;
};


class MapRegexKey
{
private:
    const nlohmann::json* schema;
    std::list<RegexKey> listRegex;
public:
    MapRegexKey() {}
    MapRegexKey(const nlohmann::json* schema);
    const nlohmann::json& at(const std::string& key, std::string& regexKey) const;
    const nlohmann::json& at(const std::string& key) const;
};




class ValidatorJSON
{
private:
    static std::ofstream warningsFile;

    static bool isRequired(std::map<std::string, bool>& required, const std::string& property);
    static void addObject(nlohmann::json& object, const std::map<std::string, nlohmann::json>& newObject, const std::string patternElem = "");
    static void removeObject(nlohmann::json& object, const nlohmann::json& objectToRemove, const std::string patternElem = "");
    static void addArray(nlohmann::json& array, const nlohmann::json& newArray);
    static void addRequired(nlohmann::json& properties, const nlohmann::json& requiredToAdd, const std::string patternElem = "");
    static void addPatternRequired(std::map<std::string,nlohmann::json>& properties, const nlohmann::json& requiredToAdd);
    static void removeRequired(nlohmann::json& properties, const nlohmann::json& requiredToRemove, const std::string patternElem = "");
    static void removePatternRequired(std::map<std::string,nlohmann::json>& properties, const nlohmann::json& requiredToRemove);
    static void doConditionBlock(nlohmann::json& schema, const nlohmann::json& block, const nlohmann::json& config, const std::string patternElem = "");
    static void checkConditionals(nlohmann::json& schema, const nlohmann::json& conditionals, const nlohmann::json& config);
    static void checkPatternConditionals(nlohmann::json& schema, const nlohmann::json& conditionals, const nlohmann::json& config);
    static void addSchema(nlohmann::json& schema, const nlohmann::json& newSchema, const nlohmann::json& config, const std::string patternElem = "");
    static void removeSchema(nlohmann::json& schema, const nlohmann::json& schemaToRemove, const std::string patternElem = "");
    static const nlohmann::json solveIfOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static const nlohmann::json solveEqualOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static const nlohmann::json solveNotEqualOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static const nlohmann::json solveLessThanOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static const nlohmann::json solveGreaterThanOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static const nlohmann::json solveLessEqualThanOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static const nlohmann::json solveGreaterEqualThanOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static const nlohmann::json solveNotOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static const nlohmann::json solveAndOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static const nlohmann::json solveOrOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    template <typename T>
    static const T solveSubtractOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    template <typename T>
    static const T solveSumOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    template <typename T>
    static const T solveDivisionOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    template <typename T>
    static const T solveMultiplyOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static const int solveModuleOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static const nlohmann::json* getProperty(const nlohmann::json& config, const nlohmann::json& listProperties, const std::string patternElem = "");
    static const nlohmann::json solveOperator(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static bool checkCondition(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static std::vector<std::string> generateRegexElems(const std::string& regex);
    static void addPatternProperties(nlohmann::json& schema, const nlohmann::json& patternProperties);
    static void removePatternProperties(std::map<std::string,nlohmann::json>& properties, const nlohmann::json& patternProperties);
    static void validateObject(const nlohmann::json& config, nlohmann::json& schema);
    static void validateArray(const nlohmann::json& config, nlohmann::json& schema);
    static void validateString(const nlohmann::json& config, nlohmann::json& schema);
    static void validateIntegerNumber(const nlohmann::json& config, nlohmann::json& schema);
    static void validateInteger(const nlohmann::json& config, nlohmann::json& schema);
    static void validateUnsignedInteger(const nlohmann::json& config, nlohmann::json& schema);
    static void validateFloat(const nlohmann::json& config, nlohmann::json& schema);
    static void validateBoolean(const nlohmann::json& config, nlohmann::json& schema);
    static void validateElem(const nlohmann::json& config, nlohmann::json& schema);
    
public:
    static void validateJSON(const nlohmann::json& config, nlohmann::json& schema);
};


class ValidatorJSONException : public LineInfoException
{
public:
    ValidatorJSONException(const std::string& message, const char* file, int line) : LineInfoException(message, file, line) {}
};

#define throwValidatorJSONException(msg) throw ValidatorJSONException(msg, __FILE__, __LINE__)

class ValidatorConfigJSONException : public ValidatorJSONException
{
public:
    ValidatorConfigJSONException(const std::string& message, const char* file, int line) : ValidatorJSONException(message, file, line) {}
};

#define throwValidatorConfigJSONException(msg) throw ValidatorConfigJSONException(msg, __FILE__, __LINE__)

class ValidatorSchemaJSONException : public ValidatorJSONException
{
public:
    ValidatorSchemaJSONException(const std::string& message, const char* file, int line) : ValidatorJSONException(message, file, line) {}
};

#define throwValidatorSchemaJSONException(msg) throw ValidatorSchemaJSONException(msg, __FILE__, __LINE__)


#endif /* VALIDATOR_JSON_H_ */
