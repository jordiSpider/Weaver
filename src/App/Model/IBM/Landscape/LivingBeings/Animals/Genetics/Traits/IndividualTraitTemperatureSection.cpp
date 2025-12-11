#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTraitTemperatureSection.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/PawarIndividualTraitTemperatureSection.h"


using namespace std;




string IndividualTraitTemperatureSection::to_string_NA()
{
    ostringstream content;

    content << PawarIndividualTraitTemperatureSection::to_string_NA();

    return content.str();
}



IndividualTraitTemperatureSection::IndividualTraitTemperatureSection()
{
	
}

IndividualTraitTemperatureSection::IndividualTraitTemperatureSection(const TraitTemperatureSection* traitTemperatureSection)
    : traitTemperatureSection(traitTemperatureSection)
{
	
}

IndividualTraitTemperatureSection::~IndividualTraitTemperatureSection() 
{
	
}

void IndividualTraitTemperatureSection::setTraitTemperatureSection(const TraitTemperatureSection* newTraitTemperatureSection)
{
    traitTemperatureSection = newTraitTemperatureSection;
}

const TraitTemperatureSection* IndividualTraitTemperatureSection::getTraitTemperatureSection() const
{
    return traitTemperatureSection;
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(IndividualTraitTemperatureSection)

template <class Archive>
void IndividualTraitTemperatureSection::serialize(Archive &, const unsigned int) {
	
} 

// Specialisation
template void IndividualTraitTemperatureSection::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void IndividualTraitTemperatureSection::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void IndividualTraitTemperatureSection::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void IndividualTraitTemperatureSection::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);

