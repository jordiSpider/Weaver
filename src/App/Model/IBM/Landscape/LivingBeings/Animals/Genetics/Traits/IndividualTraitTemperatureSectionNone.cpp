#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTraitTemperatureSectionNone.h"

#include "Exceptions/LineInfoException.h"


using namespace std;



IndividualTraitTemperatureSectionNone::IndividualTraitTemperatureSectionNone() = default;

IndividualTraitTemperatureSectionNone::~IndividualTraitTemperatureSectionNone() 
{
	
}


PreciseDouble IndividualTraitTemperatureSectionNone::applyTemperatureDependency(const Temperature& temperature, const PreciseDouble& traitValue,
        const PreciseDouble &, const PreciseDouble &, const Temperature&
    ) const
{
	throwLineInfoException("Temperature section is not available for this trait.");
}

string IndividualTraitTemperatureSectionNone::to_string() const
{
    throwLineInfoException("Temperature section is not available for this trait.");
}




template <class Archive>
void IndividualTraitTemperatureSectionNone::serialize(Archive &ar, const unsigned int) {
	
} 

// Specialisation
template void IndividualTraitTemperatureSectionNone::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void IndividualTraitTemperatureSectionNone::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void IndividualTraitTemperatureSectionNone::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void IndividualTraitTemperatureSectionNone::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
