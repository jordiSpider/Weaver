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
#include <nlohmann/json.hpp>
#include "LineInfoException.h"
#include "Output.h"

#include <magic_enum.hpp>

#define FMT_HEADER_ONLY
#include <fmt/core.h>


class ValidatorJSON
{
private:
    class TypeElemJSON {
    public:
        enum TypeElemJSONValue {
            object,
            array,
            string, 
            integer, 
            unsigned_integer, 
            floater, 
            boolean
        };

        inline static const TypeElemJSONValue stringToEnumValue(const std::string &str) { 
            try
			{
				return stringToEnum.at(str);
			}
			catch(const std::out_of_range& e) 
			{
				throwLineInfoException(fmt::format("': Unknown elem type '{}'. Valid values are {}", str, printAvailableValues()));
			}
        }
        inline static constexpr size_t size() { return magic_enum::enum_count<TypeElemJSONValue>(); }
        inline static std::string_view printAvailableValues() { return enumValues; }

    private:
        static const std::unordered_map<std::string_view, const TypeElemJSONValue> stringToEnum;
        static const std::string enumValues;

        inline static std::string_view to_string(const TypeElemJSONValue& type) { return magic_enum::enum_name(type); }
        static const std::unordered_map<std::string_view, const TypeElemJSONValue> generateMap();
        static const std::string generateAvailableValues();  
    };

    class OperatorJSON {
    public:
        enum OperatorJSONType {
            if_cond,
            equal,
            not_equal,
            less_than, 
            greater_than, 
            less_equal_than, 
            greater_equal_than, 
            not_oper,
            and_oper,
            or_oper,
            property,
            property_item,
            property_size,
            property_exists,
            constant,
            subtract,
            sum,
            division,
            multiply,
            module
        };

        inline static const OperatorJSONType stringToEnumValue(const std::string &str) { 
            try
			{
				return stringToEnum.at(str);
			}
			catch(const std::out_of_range& e) 
			{
				throwLineInfoException(fmt::format("': Unknown operator type '{}'. Valid values are {}", str, printAvailableValues()));
			}
        }
        inline static constexpr size_t size() { return magic_enum::enum_count<OperatorJSONType>(); }
        inline static std::string_view printAvailableValues() { return enumValues; }

    private:
        static const std::unordered_map<std::string_view, const OperatorJSONType> stringToEnum;
        static const std::string enumValues;

        inline static std::string_view to_string(const OperatorJSONType& type) { return magic_enum::enum_name(type); }
        static const std::unordered_map<std::string_view, const OperatorJSONType> generateMap();
        static const std::string generateAvailableValues();
    };

    class ItemsOrder {
    public:
        enum ItemsOrderType {
            increasing,
            strictly_increasing,
            decreasing, 
            strictly_decreasing
        };

        inline static const ItemsOrderType stringToEnumValue(const std::string &str) { 
            try
			{
				return stringToEnum.at(str);
			}
			catch(const std::out_of_range& e) 
			{
				throwLineInfoException(fmt::format("': Unknown item order type '{}'. Valid values are {}", str, printAvailableValues()));
			}
        }
        inline static constexpr size_t size() { return magic_enum::enum_count<ItemsOrderType>(); }
    private:
        static const std::unordered_map<std::string_view, const ItemsOrderType> stringToEnum;
        static const std::string enumValues;

        inline static std::string_view to_string(const ItemsOrderType& type) { return magic_enum::enum_name(type); }
        static const std::unordered_map<std::string_view, const ItemsOrderType> generateMap();
        inline static std::string_view printAvailableValues() { return enumValues; }
        static const std::string generateAvailableValues();
    };

    class Environment {
    public:
        enum EnvironmentType {
            schema,
            config
        };

        inline static const EnvironmentType stringToEnumValue(const std::string &str) { 
            try
			{
				return stringToEnum.at(str);
			}
			catch(const std::out_of_range& e) 
			{
				throwLineInfoException(fmt::format("': Unknown environment type '{}'. Valid values are {}", str, printAvailableValues()));
			} 
        }
        inline static constexpr size_t size() { return magic_enum::enum_count<EnvironmentType>(); }
    private:
        static const std::unordered_map<std::string_view, const EnvironmentType> stringToEnum;
        static const std::string enumValues;

        inline static std::string_view to_string(const EnvironmentType& type) { return magic_enum::enum_name(type); }
        static const std::unordered_map<std::string_view, const EnvironmentType> generateMap();
        inline static std::string_view printAvailableValues() { return enumValues; }
        static const std::string generateAvailableValues();
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

    static constexpr char PATTERN_ELEM[] = "<pattern>";
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
    static bool checkIf(const nlohmann::json& config, const nlohmann::json& condition, const std::string patternElem = "");
    static void checkCondition(nlohmann::json& schema, const nlohmann::json& condition, const nlohmann::json& config, const std::string patternElem = "");
    static nlohmann::json* moveToDestination(nlohmann::json* structure, const nlohmann::json dst, Environment::EnvironmentType environment);
    static const nlohmann::json* moveToDestination(const nlohmann::json* structure, const nlohmann::json dst, Environment::EnvironmentType environment);
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
