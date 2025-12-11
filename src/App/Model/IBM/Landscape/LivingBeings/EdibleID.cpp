#include "App/Model/IBM/Landscape/LivingBeings/EdibleID.h"



using namespace std;




EdibleID::EdibleID()
	: set(false)
{

}

EdibleID::EdibleID(const id_type& value)
	: value(value), set(true)
{
	
}

EdibleID::~EdibleID()
{

}

EdibleID::EdibleID(const EdibleID &other)
{
    value = other.value;
	set = other.set;
}

// Overloaded copy operator
EdibleID& EdibleID::operator=(const EdibleID& other) {
    if (this != &other) {
        value = other.value;
		set = other.set;
    }
    return *this;
}

const id_type& EdibleID::getValue() const
{
	return value;
}

EdibleID::operator size_t() const {
	return static_cast<size_t>(value);
}

EdibleID::operator std::string() const {
    if(set) {
		return std::string(MAX_NUM_DIGITS_ID - std::to_string(value).length(), '0') + std::to_string(value);
	}
	else {
		return "-1";
	}
}

std::ostream& operator<<(std::ostream& os, const EdibleID& id) {
	os << string(id);
    
    return os;
}

template <class Archive>
void EdibleID::serialize(Archive &ar, const unsigned int) {
	ar & value;
	ar & set;
}

// // Specialisation
template void EdibleID::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void EdibleID::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void EdibleID::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void EdibleID::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


size_t hash<EdibleID>::operator()(const EdibleID& id) const
{
    return hash<size_t>{}(id.getValue());
}
