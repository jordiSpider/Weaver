#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/SpeciesLevelTrait.h"


using namespace std;
using json = nlohmann::json;


SpeciesLevelTrait::SpeciesLevelTrait()
	: TraitDefinitionSection()
{

}

SpeciesLevelTrait::SpeciesLevelTrait(const json& info)
	: TraitDefinitionSection()
{
	if(info["value"].is_null())
	{
		null = true;
		value = 0.0;
	}
	else
	{
		null = false;
		value = info["value"].get<double>();
	}
}

SpeciesLevelTrait::~SpeciesLevelTrait() 
{

}

PreciseDouble SpeciesLevelTrait::getValue(const Genome&, const size_t, const size_t, const vector<PreciseDouble>&, const vector<size_t>&) const
{ 
	return value; 
}

void SpeciesLevelTrait::setValue(const PreciseDouble& newValue) 
{ 
	value = newValue; 
}

SpeciesLevelTrait::Type SpeciesLevelTrait::getType() const
{
	return Type::SpeciesLevel;
}

const PreciseDouble& SpeciesLevelTrait::getMinTraitRestrictedRange() const
{
	return value;
}

const PreciseDouble& SpeciesLevelTrait::getMaxTraitRestrictedRange() const
{
	return value;
}

void SpeciesLevelTrait::deserializeIndividualLevelTraits(std::vector<IndividualLevelTrait*>&)
{

}

bool SpeciesLevelTrait::isNull() const
{
	return null;
}



BOOST_CLASS_EXPORT(SpeciesLevelTrait)

template <class Archive>
void SpeciesLevelTrait::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<TraitDefinitionSection>(*this);
	
	ar & value; 
	ar & null;
}

// Specialisation
template void SpeciesLevelTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SpeciesLevelTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SpeciesLevelTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SpeciesLevelTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
