#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TraitTemperatureSection.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/PawarTraitTemperatureSection.h"


using namespace std;
using json = nlohmann::json;



void TraitTemperatureSection::getHeader(const string& trait, std::string& header)
{
    PawarTraitTemperatureSection::getHeader(trait, header);
}


TraitTemperatureSection::TraitTemperatureSection()
{
	
}

TraitTemperatureSection::~TraitTemperatureSection() 
{

}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(TraitTemperatureSection)

template <class Archive>
void TraitTemperatureSection::serialize(Archive &, const unsigned int) {
    
}


// Specialisation
template void TraitTemperatureSection::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void TraitTemperatureSection::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void TraitTemperatureSection::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void TraitTemperatureSection::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
