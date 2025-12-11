#include "Misc/JsonValidator/Elements/String.h"


using namespace std;
using json = nlohmann::json;


String::String(const json& info)
    : Element(info)
{
    try
	{
		if(!info.at("enum").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		for(size_t i = 0; i < info["enum"].size(); i++) {
			try
			{
				if(!info["enum"][i].is_string()) {
					throwValidatorSchemaJSONException("': Not an element of type 'string'");
				}
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}

        enumValues = make_pair<>(true, info["enum"]);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".enum");
		throw;
	}
	catch(const json::out_of_range&) 
    {
        enumValues = make_pair<>(false, vector<string>());
    }


	try
	{
		if(!info.at("minLength").is_number_integer()) {
			throwValidatorSchemaJSONException("': Not an element of type 'integer'");
		}

		minLength = info["minLength"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".minLength");
		throw;
	}
	catch(const json::out_of_range&) 
    {
        minLength = 0;
    }


	try
	{
		if(!info.at("maxLength").is_number_integer()) {
			throwValidatorSchemaJSONException("': Not an element of type 'integer'");
		}

		maxLength = info["maxLength"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".maxLength");
		throw;
	}
	catch(const json::out_of_range&) 
    {
        maxLength = numeric_limits<size_t>::max();
    }


	try
	{
		if(!info.at("pattern").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}

        patternStr = make_pair<>(true, info["pattern"]);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".pattern");
		throw;
	}
	catch(const json::out_of_range&) 
    {
        patternStr = make_pair<>(false, "");
    }
}

String::String(const String& other, const bool nullProperty)
	: Element(nullProperty, other.getDescription()), enumValues(other.getEnumValues()), minLength(other.getMinLength()),
	  maxLength(other.getMaxLength()), patternStr(other.getPatternStr())
{

}

String::~String()
{

}

void String::validateElement(const json& config) const
{
	if(!config.is_string()) {
        throwValidatorConfigJSONException("': Not an element of type 'string'");
    }


	
	if(enumValues.first)
    {
        bool found = false;
        for(const string& value : enumValues.second)
        {
            if(value == config) {
                found = true;
            }
        }

        if(!found) {
            ostringstream enum_values;
            for(size_t i = 0; i < enumValues.second.size(); i++) {
                enum_values << enumValues.second[i];
                if (i != enumValues.second.size() - 1) {
                    enum_values << ", ";
                }
            }
            
            throwValidatorConfigJSONException("': Value is not among the allowed values: [" + enum_values.str() + "]");
        }
    }


    if(static_cast<const string&>(config).length() < minLength) {
		throwValidatorConfigJSONException("': String length less than minimum");
	}


    if(static_cast<const string&>(config).length() > maxLength) {
		throwValidatorConfigJSONException("': String length greater than maximum");
	}


    if(patternStr.first)
    {
        regex pattern(patternStr.second);
        if(!regex_match(static_cast<const string&>(config), pattern)) {
            throwValidatorConfigJSONException("': String does not meet defined pattern");
        }
    }
}

Element::Type String::getType() const
{
	return Type::string;
}

const pair<bool, vector<string>>& String::getEnumValues() const
{
	return enumValues;
}

size_t String::getMinLength() const
{
	return minLength;
}

size_t String::getMaxLength() const
{
	return maxLength;
}

const pair<bool, string>& String::getPatternStr() const
{
	return patternStr;
}
