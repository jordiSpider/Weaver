#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/TempSizeRuleIndividualTraitTemperatureSection.h"


using namespace std;





TempSizeRuleIndividualTraitTemperatureSection::TempSizeRuleIndividualTraitTemperatureSection()
    : IndividualTraitTemperatureSection()
{
	
}

TempSizeRuleIndividualTraitTemperatureSection::TempSizeRuleIndividualTraitTemperatureSection(const TraitTemperatureSection* traitTemperatureSection)
    : IndividualTraitTemperatureSection(traitTemperatureSection)
{

}

TempSizeRuleIndividualTraitTemperatureSection::~TempSizeRuleIndividualTraitTemperatureSection() 
{
	
}


PreciseDouble TempSizeRuleIndividualTraitTemperatureSection::applyTemperatureDependency(const Temperature& temperature, const PreciseDouble& traitValue, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab) const
{
	return MathFunctions::use_TSR(temperature, getTraitTemperatureSection()->getTempSizeRuleVector(), coefficientForMassAforMature, scaleForMassBforMature, Length(traitValue), tempFromLab).getValue();
}

const TempSizeRuleTraitTemperatureSection* TempSizeRuleIndividualTraitTemperatureSection::getTraitTemperatureSection() const
{
    return static_cast<const TempSizeRuleTraitTemperatureSection*>(IndividualTraitTemperatureSection::getTraitTemperatureSection());
}

string TempSizeRuleIndividualTraitTemperatureSection::to_string() const
{
    ostringstream content;

    content << IndividualTraitTemperatureSection::to_string_NA();

    return content.str();
}



BOOST_CLASS_EXPORT(TempSizeRuleIndividualTraitTemperatureSection)

template <class Archive>
void TempSizeRuleIndividualTraitTemperatureSection::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<IndividualTraitTemperatureSection>(*this);
} 

// Specialisation
template void TempSizeRuleIndividualTraitTemperatureSection::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void TempSizeRuleIndividualTraitTemperatureSection::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void TempSizeRuleIndividualTraitTemperatureSection::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void TempSizeRuleIndividualTraitTemperatureSection::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
