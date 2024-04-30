#include "IBM/World/LivingBeings/Animals/Genetics/Traits/DefinitionSection/SpeciesLevelTrait.h"


using namespace std;
using json = nlohmann::json;


SpeciesLevelTrait::SpeciesLevelTrait(const json& info)
	: TraitDefinitionSection(), value(info["value"])
{

}

SpeciesLevelTrait::~SpeciesLevelTrait() 
{

}

const double SpeciesLevelTrait::getValue(const Genome& genome, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const vector<double>& rhoPerModule, const vector<unsigned int>& rhoRangePerModule) const
{ 
	return value; 
}

void SpeciesLevelTrait::setValue(const double& value) 
{ 
	this->value = value; 
}

const SpeciesLevelTrait::Type SpeciesLevelTrait::getType() const
{
	return Type::SpeciesLevel;
}

const double& SpeciesLevelTrait::getMinTraitRestrictedRange() const
{
	return value;
}

const double& SpeciesLevelTrait::getMaxTraitRestrictedRange() const
{
	return value;
}

template <class Archive>
void SpeciesLevelTrait::serialize(Archive &ar, const unsigned int version) {
	ar & value; 
}