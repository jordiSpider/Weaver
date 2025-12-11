#include "Misc/JsonValidator/Elements/Element.h"


using namespace std;
using json = nlohmann::json;


Element::Element(const json& info)
	: modified(false)
{
    try
	{
		if(!info.at("nullProperty").is_boolean()) {
			throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
		}

		nullProperty = info["nullProperty"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".nullProperty");
		throw;
	}
	catch(const json::out_of_range&)
	{
		nullProperty = false;
	}

	try
	{
		if(!info.at("description").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}

		description = info["description"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".description");
		throw;
	}
	catch(const json::out_of_range&)
	{
		description = "";
	}
}

Element::Element(const bool nullProperty, const string& description)
	: modified(true), nullProperty(nullProperty), description(description)
{

}

Element::~Element()
{

}

bool Element::isNullProperty() const
{
    return nullProperty;
}

bool Element::isModified() const
{
	return modified;
}

const string& Element::getDescription() const
{
	return description;
}

void Element::validate(const json& config) const
{
    if(!isNullProperty())
	{
        if(config.is_null())
		{
			throwValidatorConfigJSONException("': Cannot be null");
		}

        validateElement(config);
    }
}

void Element::verifyPropertyPath(const vector<string>& path, const unsigned int currentIndex) const
{
	if(currentIndex < path.size())
	{
		throwValidatorSchemaJSONException("': Incorrect property path");
	}
}

pair<const Element* const, string> Element::getProperty(const string&, const bool, const string&) const
{
	throwValidatorConfigJSONException("': Incorrect property");
}

pair<const Element* const, string> Element::getProperty(const string& propertyName) const
{
	return getProperty(propertyName, false, "");
}
