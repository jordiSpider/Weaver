#include "App/Model/IBM/Landscape/LivingBeings/Species/Species.h"


using namespace std;


Species::ID::ID()
{

}

Species::ID::ID(const id_type& value)
	: value(value)
{

}

const id_type& Species::ID::getValue() const
{
	return value;
}

Species::ID::operator size_t() const {
	return static_cast<size_t>(value);
}

size_t hash<Species::ID>::operator()(const Species::ID& id) const
{
    return hash<size_t>{}(id.getValue());
}

template <class Archive>
void Species::ID::serialize(Archive &ar, const unsigned int) {
	ar & value;
}

// Specialisation
template void Species::ID::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Species::ID::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Species::ID::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Species::ID::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);





Species::Species()
{

}

Species::Species(const Species::ID& speciesId, const string& scientificName)
	: id(speciesId), scientificName(trim(scientificName)), extinguished(false) 
	  
{
	
}

Species::~Species()
{
	
}

bool Species::operator<(const Species& other) const
{
	return this->id < other.id;
}

bool Species::operator==(const Species& other) const {
	return (this->scientificName == other.scientificName);
}

const Species::ID& Species::getId() const 
{ 
	return id; 
}

const std::string& Species::getScientificName() const 
{ 
	return scientificName; 
}

std::string Species::getScientificNameReplaced() const
{
	string scientificNameReplaced = getScientificName();
	std::replace(scientificNameReplaced.begin(), scientificNameReplaced.end(), ' ', '_');
	return scientificNameReplaced;
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(Species)

template <class Archive>
void Species::serialize(Archive &ar, const unsigned int) {
	ar & id;
	ar & scientificName;
	ar & extinguished;
}

// // Specialisation
template void Species::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Species::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Species::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Species::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
