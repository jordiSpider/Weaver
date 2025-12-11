
#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/Instar.h"

using namespace std;



Instar Instar::convertToIndex(const unsigned int value)
{
    return Instar(value+1);
}



Instar::Instar()
{
    this->value = 0;
}

Instar::Instar(unsigned int value)
{
    #ifdef DEBUG
        if(value == 0)
        {
            throwLineInfoException("Instar value must be greater than or equal to 1.");
        }
    #endif

    this->value = value - 1;
}

Instar::Instar(const Instar &other)
{
    this->value = other.getValue();
}

Instar& Instar::operator=(const Instar& other)
{
    if (this != &other) {
        value = other.value;
    }
    return *this;
}

Instar::~Instar()
{

}

unsigned int Instar::getValue() const 
{ 
    return value; 
}

void Instar::moveOnPreviousInstar() 
{ 
    value--; 
}

void Instar::moveOnNextInstar() 
{ 
    value++; 
}

Instar operator+(const Instar& lhs, const unsigned int& rhs)
{
    return Instar(lhs.value + rhs + 1);
}

Instar operator+(const unsigned int& lhs, const Instar& rhs)
{
    return Instar(lhs + rhs.value + 1);
}

Instar& Instar::operator++()
{
    ++value;
    return *this;
}

Instar Instar::operator++(int)
{
    Instar tmp(*this);
    ++value;
    return tmp;
}

Instar operator-(const Instar& lhs, const unsigned int& rhs)
{
    return Instar(lhs.value - rhs + 1);
}

Instar operator-(const unsigned int& lhs, const Instar& rhs)
{
    return Instar(lhs - rhs.value + 1);
}

bool operator<(const Instar& lhs, const Instar& rhs)
{
    return lhs.value < rhs.value;
}

bool operator<(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) < rhs;
}

bool operator<(const unsigned int& lhs, const Instar& rhs)
{
    return lhs < (rhs.value+1);
}

bool operator<=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value <= rhs.value;
}

bool operator<=(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) <= rhs;
}

bool operator<=(const unsigned int& lhs, const Instar& rhs)
{
    return lhs <= (rhs.value+1);
}

bool operator>(const Instar& lhs, const Instar& rhs)
{
    return lhs.value > rhs.value;
}

bool operator>(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) > rhs;
}

bool operator>(const unsigned int& lhs, const Instar& rhs)
{
    return lhs > (rhs.value+1);
}

bool operator>=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value >= rhs.value;
}

bool operator>=(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) >= rhs;
}

bool operator>=(const unsigned int& lhs, const Instar& rhs)
{
    return lhs >= (rhs.value+1);
}

bool operator==(const Instar& lhs, const Instar& rhs)
{
    return lhs.value == rhs.value;
}

bool operator==(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) == rhs;
}

bool operator==(const unsigned int& lhs, const Instar& rhs)
{
    return lhs == (rhs.value+1);
}

bool operator!=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value != rhs.value;
}

bool operator!=(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) != rhs;
}

bool operator!=(const unsigned int& lhs, const Instar& rhs)
{
    return lhs != (rhs.value+1);
}

string Instar::to_string() const 
{
    return std::to_string(value + 1);
}

ostream& operator<<(ostream& os, const Instar& instar)
{
    os << instar.to_string();
    return os;
}

Instar::operator size_t() const 
{
    return static_cast<size_t>(value);
}

size_t hash<Instar>::operator()(const Instar& instar) const
{
    return hash<unsigned int>{}(instar.getValue());
}


template <class Archive>
void Instar::serialize(Archive &ar, const unsigned int) {
    ar & value;
}

// // Specialisation
template void Instar::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Instar::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Instar::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Instar::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
