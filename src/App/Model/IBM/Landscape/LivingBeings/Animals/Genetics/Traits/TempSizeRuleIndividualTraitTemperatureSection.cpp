#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/TempSizeRuleIndividualTraitTemperatureSection.h"

#include "App/Model/IBM/Maths/MathFunctions.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TempSizeRuleTraitTemperatureSection.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/PawarIndividualTraitTemperatureSection.h"


using namespace std;





TempSizeRuleIndividualTraitTemperatureSection::TempSizeRuleIndividualTraitTemperatureSection() = default;

TempSizeRuleIndividualTraitTemperatureSection::TempSizeRuleIndividualTraitTemperatureSection(const TempSizeRuleTraitTemperatureSection* traitTemperatureSection)
    : traitTemperatureSection(traitTemperatureSection)
{

}

TempSizeRuleIndividualTraitTemperatureSection::~TempSizeRuleIndividualTraitTemperatureSection() 
{
	
}


PreciseDouble TempSizeRuleIndividualTraitTemperatureSection::applyTemperatureDependency(const Temperature& temperature, const PreciseDouble& traitValue, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab) const
{
	return MathFunctions::use_TSR(temperature, traitTemperatureSection->getTempSizeRuleVector(), coefficientForMassAforMature, scaleForMassBforMature, Length(traitValue), tempFromLab).getValue();
}

void TempSizeRuleIndividualTraitTemperatureSection::setTraitTemperatureSection(const TempSizeRuleTraitTemperatureSection* newTraitTemperatureSection)
{
    traitTemperatureSection = newTraitTemperatureSection;
}




template <class Archive>
void TempSizeRuleIndividualTraitTemperatureSection::serialize(Archive &ar, const unsigned int) {
	
} 

// Specialisation
template void TempSizeRuleIndividualTraitTemperatureSection::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void TempSizeRuleIndividualTraitTemperatureSection::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void TempSizeRuleIndividualTraitTemperatureSection::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void TempSizeRuleIndividualTraitTemperatureSection::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
