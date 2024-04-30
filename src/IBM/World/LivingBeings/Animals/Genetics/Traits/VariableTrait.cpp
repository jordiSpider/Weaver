#include "IBM/World/LivingBeings/Animals/Genetics/Traits/VariableTrait.h"


using namespace std;
using json = nlohmann::json;


VariableTrait::VariableTrait()
	: Trait()
{

}

VariableTrait::VariableTrait(const string& typeStr, const json& traitInfo, const unsigned int& order)
	: Trait(typeStr, traitInfo), order(order),
      minTraitRange(traitInfo["valueUpdateMethod"]["variableParams"]["ranges"]["min"]), 
	  maxTraitRange(traitInfo["valueUpdateMethod"]["variableParams"]["ranges"]["max"]),
	  restrictValue(traitInfo["valueUpdateMethod"]["variableParams"]["restrictValue"]),
	  minRestrictedRange(minTraitRange + ((maxTraitRange - minTraitRange) * 0.5) * restrictValue),
	  maxRestrictedRange(maxTraitRange - ((maxTraitRange - minTraitRange) * 0.5) * restrictValue),
	  minTraitLimit(traitInfo["valueUpdateMethod"]["variableParams"]["limits"]["min"]), 
	  maxTraitLimit(traitInfo["valueUpdateMethod"]["variableParams"]["limits"]["max"]),
      minObservedPseudoValue(DBL_MAX), maxObservedPseudoValue(0.0)
{

}

VariableTrait::~VariableTrait() 
{

}

const unsigned int& VariableTrait::getOrder() const
{ 
	return order; 
}

const double& VariableTrait::getMinTraitRange() const
{ 
	return minTraitRange; 
}

const double& VariableTrait::getMaxTraitRange() const 
{ 
	return maxTraitRange; 
}

const double& VariableTrait::getMinRestrictedRange() const 
{ 
	return minRestrictedRange; 
}

const double& VariableTrait::getMaxRestrictedRange() const 
{ 
	return maxRestrictedRange; 
}

const double& VariableTrait::getMinTraitLimit() const 
{ 
	return minTraitLimit; 
}

const double& VariableTrait::getMaxTraitLimit() const 
{ 
	return maxTraitLimit; 
}

const double& VariableTrait::getMinObservedPseudoValue() const 
{ 
	return minObservedPseudoValue; 
}

const double& VariableTrait::getMaxObservedPseudoValue() const 
{ 
	return maxObservedPseudoValue; 
}

void VariableTrait::setMinObservedPseudoValue(const double& newMinObservedPseudoValue) 
{ 
	minObservedPseudoValue = newMinObservedPseudoValue; 
}

void VariableTrait::setMaxObservedPseudoValue(const double& newMaxObservedPseudoValue) 
{ 
	maxObservedPseudoValue = newMaxObservedPseudoValue; 
}

Trait::ValueUpdateMethod VariableTrait::getValueUpdateMethodType() const
{
	return ValueUpdateMethod::Variable;
}

template <class Archive>
void VariableTrait::serialize(Archive &ar, const unsigned int version) {
	ar & boost::serialization::base_object<Trait>(*this);

	ar & order;
	ar & minTraitRange;
	ar & maxTraitRange;
	ar & minRestrictedRange;
	ar & maxRestrictedRange;
	ar & minTraitLimit;
	ar & maxTraitLimit;
	ar & minObservedPseudoValue;
	ar & maxObservedPseudoValue;
	ar & restrictValue;
}

// Specialisation
template void VariableTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void VariableTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void VariableTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void VariableTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
