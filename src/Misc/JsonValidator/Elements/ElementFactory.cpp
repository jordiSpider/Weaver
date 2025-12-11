#include "Misc/JsonValidator/Elements/ElementFactory.h"


using namespace std;
using json = nlohmann::json;


Element* ElementFactory::createInstance(const json& info)
{
    try
	{
		if(!info.at("type").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}

        switch(EnumClass<Element::Type>::stringToEnumValue(info["type"]))
		{
			case Element::Type::object: {
				return new Object(info);
				break;
			}
			case Element::Type::array: {
				return new Array(info);
				break;
			}
			case Element::Type::tuple: {
				return new Tuple(info);
				break;
			}
			case Element::Type::string: {
				return new String(info);
				break;
			}
			case Element::Type::integer: {
				return new Integer(info);
				break;
			}
			case Element::Type::number: {
				return new Number(info);
				break;
			}
			case Element::Type::boolean: {
				return new Boolean(info);
				break;
			}
			default: {
				throwValidatorSchemaJSONException("': Default case");
				break;
			}
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".type");
		throw;
	}
	catch(const json::out_of_range&)
	{
		throwValidatorSchemaJSONException("': Property 'type' not defined. Valid values are " + EnumClass<Element::Type>::printValues());
	}
}

Element* ElementFactory::createInstance(const Element& other, const bool nullProperty)
{
	switch(other.getType())
	{
		case Element::Type::object: {
			return new Object(static_cast<const Object&>(other), nullProperty);
			break;
		}
		case Element::Type::array: {
			return new Array(static_cast<const Array&>(other), nullProperty);
			break;
		}
		case Element::Type::tuple: {
			return new Tuple(static_cast<const Tuple&>(other), nullProperty);
			break;
		}
		case Element::Type::string: {
			return new String(static_cast<const String&>(other), nullProperty);
			break;
		}
		case Element::Type::integer: {
			return new Integer(static_cast<const Integer&>(other), nullProperty);
			break;
		}
		case Element::Type::number: {
			return new Number(static_cast<const Number&>(other), nullProperty);
			break;
		}
		case Element::Type::boolean: {
			return new Boolean(static_cast<const Boolean&>(other), nullProperty);
			break;
		}
		default: {
			throwValidatorConfigJSONException("': Default case");
			break;
		}
	}
}
