#include "App/Model/IBM/Landscape/LivingBeings/Animals/Generation.h"



using namespace std;




Generation::Generation()
{

}

Generation::Generation(const unsigned int& value)
	: value(value)
{
	
}

Generation::~Generation()
{

}

Generation::Generation(const Generation &other)
{
    value = other.value;
}

// Overloaded copy operator
Generation& Generation::operator=(const Generation& other) {
    if (this != &other) {
        value = other.value;
    }
    return *this;
}

unsigned int Generation::getValue() const
{
	return value;
}

Generation::operator size_t() const {
	return static_cast<size_t>(value);
}

template <class Archive>
void Generation::serialize(Archive &ar, const unsigned int) {
	ar & value;
}

// // Specialisation
template void Generation::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Generation::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Generation::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Generation::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
