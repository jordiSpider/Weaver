#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"


using namespace std;


AnimalSpeciesID::AnimalSpeciesID()
{

}

AnimalSpeciesID::AnimalSpeciesID(const id_type& value)
	: value(value)
{

}

const id_type& AnimalSpeciesID::getValue() const
{
	return value;
}

AnimalSpeciesID::operator size_t() const {
	return static_cast<size_t>(value);
}

size_t hash<AnimalSpeciesID>::operator()(const AnimalSpeciesID& animalSpeciesID) const
{
    return hash<size_t>{}(animalSpeciesID.getValue());
}

template <class Archive>
void AnimalSpeciesID::serialize(Archive &ar, const unsigned int) {
	ar & value;
}

// // Specialisation
template void AnimalSpeciesID::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalSpeciesID::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalSpeciesID::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalSpeciesID::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
