#include "Misc/JsonValidator/Elements/Boolean.h"


using namespace std;
using json = nlohmann::json;


Boolean::Boolean(const json& info)
    : Element(info)
{
    
}

Boolean::Boolean(const Boolean& other, const bool nullProperty)
    : Element(nullProperty, other.getDescription())
{

}

Boolean::~Boolean()
{

}

void Boolean::validateElement(const json& config) const
{
    if(!config.is_boolean()) {
        throwValidatorConfigJSONException("': Not an element of type 'boolean'");
    }
}

Element::Type Boolean::getType() const
{
	return Type::boolean;
}
