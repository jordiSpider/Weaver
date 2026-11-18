#include "Misc/JsonValidator/Elements/Integer.h"


using namespace std;
using json = nlohmann::json;


Integer::Integer(const json& info)
    : Element(info)
{
	try
	{
		minimum = info.value("minimum", numeric_limits<int>::min());

		if(info.contains("minimum") && !info.at("minimum").is_number_integer()) {
			throwValidatorSchemaJSONException("': Not an element of type 'integer'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".minimum");
		throw;
	}


	try
	{
		exclusiveMinimum = info.value("exclusiveMinimum", false);

		if(info.contains("exclusiveMinimum") && !info.at("exclusiveMinimum").is_boolean()) {
			throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".exclusiveMinimum");
		throw;
	}


	try
	{
		maximum = info.value("maximum", numeric_limits<int>::max());

		if(info.contains("maximum") && !info.at("maximum").is_number_integer()) {
			throwValidatorSchemaJSONException("': Not an element of type 'integer'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".maximum");
		throw;
	}


	try
	{
		exclusiveMaximum = info.value("exclusiveMaximum", false);
		
		if(info.contains("exclusiveMaximum") && !info.at("exclusiveMaximum").is_boolean()) {
			throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".exclusiveMaximum");
		throw;
	}


	try
	{
		multipleOf = info.value("multipleOf", make_pair<>(false, 0));

		if(info.contains("multipleOf") && !info.at("multipleOf").is_number_integer()) {
			throwValidatorSchemaJSONException("': Not an element of type 'integer'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".multipleOf");
		throw;
	}
}

Integer::Integer(const Integer& other, const bool nullProperty)
	: Element(nullProperty, other.getDescription()), minimum(other.getMinimum()), exclusiveMinimum(other.getExclusiveMinimum()),
	  maximum(other.getMaximum()), exclusiveMaximum(other.getExclusiveMaximum()), multipleOf(other.getMultipleOf())
{

}

Integer::~Integer()
{

}

void Integer::validateElement(const json& config) const
{
	if(!config.is_number_integer()) {
        throwValidatorConfigJSONException("': Not an element of type 'integer'");
    }


	
	if(exclusiveMinimum)
	{
		if(static_cast<const int&>(config) <= minimum)
		{
			throwValidatorConfigJSONException("': Value less than or equal minimum");
		}
	}
	else
	{
		if(static_cast<const int&>(config) < minimum)
		{
			throwValidatorConfigJSONException("': Value less than minimum");
		}
	}


	if(exclusiveMaximum)
	{
		if(static_cast<const int&>(config) >= maximum)
		{
			throwValidatorConfigJSONException("': Value greater than or equal maximum");
		}
	}
	else
	{
		if(static_cast<const int&>(config) > maximum)
		{
			throwValidatorConfigJSONException("': Value greater than maximum");
		}
	}
	

	if(multipleOf.first)
	{
		if(static_cast<const int&>(config) % multipleOf.second != 0) {
			throwValidatorConfigJSONException("': Value is not a multiple of " + to_string(multipleOf.second));
		}
	}
}

Element::Type Integer::getType() const
{
	return Type::integer;
}

int Integer::getMinimum() const
{
	return minimum;
}

bool Integer::getExclusiveMinimum() const
{
	return exclusiveMinimum;
}

int Integer::getMaximum() const
{
	return maximum;
}

bool Integer::getExclusiveMaximum() const
{
	return exclusiveMaximum;
}

const pair<bool, int>& Integer::getMultipleOf() const
{
	return multipleOf;
}
