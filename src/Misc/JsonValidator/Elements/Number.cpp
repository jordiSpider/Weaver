#include "Misc/JsonValidator/Elements/Number.h"


using namespace std;
using json = nlohmann::json;


Number::Number(const json& info)
    : Element(info)
{
	try
	{
		if(!info.at("minimum").is_number_float()) {
			throwValidatorSchemaJSONException("': Not an element of type 'number'");
		}

		minimum = info["minimum"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".minimum");
		throw;
	}
	catch(const json::out_of_range&)
	{
		minimum = -numeric_limits<double>::max();
	}


	try
	{
		if(!info.at("exclusiveMinimum").is_boolean()) {
			throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
		}

		exclusiveMinimum = info["exclusiveMinimum"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".exclusiveMinimum");
		throw;
	}
	catch(const json::out_of_range&)
	{
		exclusiveMinimum = false;
	}


	try
	{
		if(!info.at("maximum").is_number_float()) {
			throwValidatorSchemaJSONException("': Not an element of type 'number'");
		}

		maximum = info["maximum"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".maximum");
		throw;
	}
	catch(const json::out_of_range&)
	{
		maximum = numeric_limits<double>::max();
	}


	try
	{
		if(!info.at("exclusiveMaximum").is_boolean()) {
			throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
		}

		exclusiveMaximum = info["exclusiveMaximum"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".exclusiveMaximum");
		throw;
	}
	catch(const json::out_of_range&)
	{
		exclusiveMaximum = false;
	}
}

Number::Number(const Number& other, const bool nullProperty)
	: Element(nullProperty, other.getDescription()), minimum(other.getMinimum()), exclusiveMinimum(other.getExclusiveMinimum()),
	  maximum(other.getMaximum()), exclusiveMaximum(other.getExclusiveMaximum())
{

}

Number::~Number()
{

}

void Number::validateElement(const json& config) const
{
	if(!config.is_number_float() && !config.is_number_integer()) {
        throwValidatorConfigJSONException("': Not an element of type 'number'");
    }


	
	if(exclusiveMinimum)
	{
		if(static_cast<const double&>(config) <= minimum)
		{
			throwValidatorConfigJSONException("': Value less than or equal minimum");
		}
	}
	else
	{
		if(static_cast<const double&>(config) < minimum)
		{
			throwValidatorConfigJSONException("': Value less than minimum");
		}
	}
		

	if(exclusiveMaximum)
	{
		if(static_cast<const double&>(config) >= maximum)
		{
			throwValidatorConfigJSONException("': Value greater than or equal maximum");
		}
	}
	else
	{
		if(static_cast<const double&>(config) > maximum)
		{
			throwValidatorConfigJSONException("': Value greater than maximum");
		}
	} 
}

Element::Type Number::getType() const
{
	return Type::number;
}

const double& Number::getMinimum() const
{
	return minimum;
}

bool Number::getExclusiveMinimum() const
{
	return exclusiveMinimum;
}

const double& Number::getMaximum() const
{
	return maximum;
}

bool Number::getExclusiveMaximum() const
{
	return exclusiveMaximum;
}
