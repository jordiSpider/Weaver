#include "IBM/World/LivingBeings/Animals/Genetics/Traits/FixedTrait.h"


using namespace std;
using json = nlohmann::json;


FixedTrait::FixedTrait(const string& typeStr, const json& traitInfo)
	: Trait(typeStr, traitInfo), value(traitInfo["valueUpdateMethod"]["fixedParams"]["value"])
{

}

const double& FixedTrait::getValue() const
{ 
	return value; 
}

void FixedTrait::setValue(const double& value) 
{ 
	this->value = value; 
}

Trait::ValueUpdateMethod::MethodType FixedTrait::getValueUpdateMethodType() const
{
	return ValueUpdateMethod::Fixed;
}

template <class Archive>
void FixedTrait::serialize(Archive &ar, const unsigned int version) {
	ar & value; 
}