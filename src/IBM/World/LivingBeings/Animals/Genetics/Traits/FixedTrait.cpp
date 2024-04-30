#include "IBM/World/LivingBeings/Animals/Genetics/Traits/FixedTrait.h"


using namespace std;
using json = nlohmann::json;


FixedTrait::FixedTrait()
	: Trait()
{

}

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

Trait::ValueUpdateMethod FixedTrait::getValueUpdateMethodType() const
{
	return ValueUpdateMethod::Fixed;
}

template <class Archive>
void FixedTrait::serialize(Archive &ar, const unsigned int version) {
	ar & boost::serialization::base_object<Trait>(*this);

	ar & value; 
}

// Specialisation
template void FixedTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void FixedTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void FixedTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void FixedTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
