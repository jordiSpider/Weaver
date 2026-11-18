#include "Misc/JsonValidator/Elements/Element.h"


using namespace std;
using json = nlohmann::json;


Element::Element(const json& info)
	: modified(false)
{
    try
	{
		nullProperty = info.value("nullProperty", false);

		if (info.contains("nullProperty") && !info.at("nullProperty").is_boolean()) {
            throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
        }
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".nullProperty");
		throw;
	}

	try
	{
		description = info.value("description", "");

		if(info.contains("description") && !info.at("description").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".description");
		throw;
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
	if (config.is_null()) {
		if (isNullProperty()) return;
		throwValidatorConfigJSONException("': Cannot be null");
	}

	// nullProperty means a value may be absent/null. If it is supplied, validate
	// it normally so optional arrays and their object items cannot bypass checks.
	validateElement(config);
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
