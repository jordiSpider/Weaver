#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"

using namespace std;


DryMass::DryMass()
    : value(0.0)
{
    
}

DryMass::DryMass(const PreciseDouble& value)
    : value(value)
{
    
}

DryMass::DryMass(const WetMass& wetMass, const PreciseDouble& conversionToWetMass)
    : value(wetMass.getValue() / conversionToWetMass)
{

}

DryMass::DryMass(const Length& length, const PreciseDouble& coefficientForMassA, const PreciseDouble& scaleForMassB)
    : value(coefficientForMassA * pow(length.getValue(), scaleForMassB))
{

}

DryMass::DryMass(const DryMass &other)
    : value(other.getValue())
{
    
}

DryMass& DryMass::operator=(const DryMass& other)
{
    if (this != &other) {
        value = other.value;
    }
    return *this;
}

DryMass::~DryMass()
{

}

const PreciseDouble& DryMass::getValue() const 
{ 
    return value; 
}

DryMass& DryMass::operator+=(const DryMass& other) {
    value += other.getValue();
    return *this;
}

DryMass DryMass::operator/(const PreciseDouble& scalar) const {
    return DryMass(value / scalar);
}

DryMass operator+(const DryMass& lhs, const DryMass& rhs)
{
    return DryMass(lhs.getValue() + rhs.getValue());
}

DryMass operator+(const DryMass& lhs, const PreciseDouble& rhs)
{
    return DryMass(lhs.getValue() + rhs);
}

DryMass operator+(const PreciseDouble& lhs, const DryMass& rhs)
{
    return DryMass(lhs + rhs.getValue());
}

DryMass operator-(const DryMass& lhs, const DryMass& rhs)
{
    return DryMass(lhs.getValue() - rhs.getValue());
}

DryMass operator-(const DryMass& lhs, const PreciseDouble& rhs)
{
    return DryMass(lhs.getValue() - rhs);
}

DryMass operator-(const PreciseDouble& lhs, const DryMass& rhs)
{
    return DryMass(lhs - rhs.getValue());
}

bool operator<(const DryMass& lhs, const DryMass& rhs)
{
    return lhs.getValue() < rhs.getValue();
}

bool operator<(const DryMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() < rhs;
}

bool operator<(const PreciseDouble& lhs, const DryMass& rhs)
{
    return lhs < rhs.getValue();
}

bool operator<=(const DryMass& lhs, const DryMass& rhs)
{
    return lhs.getValue() <= rhs.getValue();
}

bool operator<=(const DryMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() <= rhs;
}

bool operator<=(const PreciseDouble& lhs, const DryMass& rhs)
{
    return lhs <= rhs.getValue();
}

bool operator>(const DryMass& lhs, const DryMass& rhs)
{
    return lhs.getValue() > rhs.getValue();
}

bool operator>(const DryMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() > rhs;
}

bool operator>(const PreciseDouble& lhs, const DryMass& rhs)
{
    return lhs > rhs.getValue();
}

bool operator>=(const DryMass& lhs, const DryMass& rhs)
{
    return lhs.getValue() >= rhs.getValue();
}

bool operator>=(const DryMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() >= rhs;
}

bool operator>=(const PreciseDouble& lhs, const DryMass& rhs)
{
    return lhs >= rhs.getValue();
}

bool operator==(const DryMass& lhs, const DryMass& rhs)
{
    return lhs.getValue() == rhs.getValue();
}

bool operator==(const DryMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() == rhs;
}

bool operator==(const PreciseDouble& lhs, const DryMass& rhs)
{
    return lhs == rhs.getValue();
}

bool operator!=(const DryMass& lhs, const DryMass& rhs)
{
    return lhs.getValue() != rhs.getValue();
}

bool operator!=(const DryMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() != rhs;
}

bool operator!=(const PreciseDouble& lhs, const DryMass& rhs)
{
    return lhs != rhs.getValue();
}

ostream& operator<<(ostream& os, const DryMass& dryMass)
{
    os << dryMass.getValue();
    return os;
}


template <class Archive>
void DryMass::serialize(Archive &ar, const unsigned int) {
    ar & value;
}

// // Specialisation
template void DryMass::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void DryMass::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void DryMass::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void DryMass::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);




WetMass::WetMass()
    : value(0.0)
{
    
}

WetMass::WetMass(const PreciseDouble& value)
    : value(value)
{
    
}

WetMass::WetMass(const DryMass& dryMass, const PreciseDouble& conversionToWetMass)
    : value(dryMass.getValue() * conversionToWetMass)
{

}

WetMass::WetMass(const WetMass &other)
    : value(other.getValue())
{
    
}

WetMass& WetMass::operator=(const WetMass& other)
{
    if (this != &other) {
        value = other.value;
    }
    return *this;
}

WetMass::~WetMass()
{

}

const PreciseDouble& WetMass::getValue() const
{ 
    return value; 
}

WetMass operator+(const WetMass& lhs, const WetMass& rhs)
{
    return WetMass(lhs.getValue() + rhs.getValue());
}

WetMass operator+(const WetMass& lhs, const PreciseDouble& rhs)
{
    return WetMass(lhs.getValue() + rhs);
}

WetMass operator+(const PreciseDouble& lhs, const WetMass& rhs)
{
    return WetMass(lhs + rhs.getValue());
}

WetMass operator-(const WetMass& lhs, const WetMass& rhs)
{
    return WetMass(lhs.getValue() - rhs.getValue());
}

WetMass operator-(const WetMass& lhs, const PreciseDouble& rhs)
{
    return WetMass(lhs.getValue() - rhs);
}

WetMass operator-(const PreciseDouble& lhs, const WetMass& rhs)
{
    return WetMass(lhs - rhs.getValue());
}

bool operator<(const WetMass& lhs, const WetMass& rhs)
{
    return lhs.getValue() < rhs.getValue();
}

bool operator<(const WetMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() < rhs;
}

bool operator<(const PreciseDouble& lhs, const WetMass& rhs)
{
    return lhs < rhs.getValue();
}

bool operator<=(const WetMass& lhs, const WetMass& rhs)
{
    return lhs.getValue() <= rhs.getValue();
}

bool operator<=(const WetMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() <= rhs;
}

bool operator<=(const PreciseDouble& lhs, const WetMass& rhs)
{
    return lhs <= rhs.getValue();
}

bool operator>(const WetMass& lhs, const WetMass& rhs)
{
    return lhs.getValue() > rhs.getValue();
}

bool operator>(const WetMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() > rhs;
}

bool operator>(const PreciseDouble& lhs, const WetMass& rhs)
{
    return lhs > rhs.getValue();
}

bool operator>=(const WetMass& lhs, const WetMass& rhs)
{
    return lhs.getValue() >= rhs.getValue();
}

bool operator>=(const WetMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() >= rhs;
}

bool operator>=(const PreciseDouble& lhs, const WetMass& rhs)
{
    return lhs >= rhs.getValue();
}

bool operator==(const WetMass& lhs, const WetMass& rhs)
{
    return lhs.getValue() == rhs.getValue();
}

bool operator==(const WetMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() == rhs;
}

bool operator==(const PreciseDouble& lhs, const WetMass& rhs)
{
    return lhs == rhs.getValue();
}

bool operator!=(const WetMass& lhs, const WetMass& rhs)
{
    return lhs.getValue() != rhs.getValue();
}

bool operator!=(const WetMass& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() != rhs;
}

bool operator!=(const PreciseDouble& lhs, const WetMass& rhs)
{
    return lhs != rhs.getValue();
}

ostream& operator<<(ostream& os, const WetMass& wetMass)
{
    os << wetMass.getValue();
    return os;
}


template <class Archive>
void WetMass::serialize(Archive &ar, const unsigned int) {
    ar & value;
}

// // Specialisation
template void WetMass::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void WetMass::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void WetMass::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void WetMass::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);




Length::Length()
    : value(0.0)
{
    
}

Length::Length(const PreciseDouble& value)
    : value(value)
{
    
}

Length::Length(const DryMass& dryMass, const PreciseDouble& coefficientForMassA, const PreciseDouble& scaleForMassB)
    : value(pow(dryMass.getValue()/coefficientForMassA, 1/scaleForMassB))
{

}

Length::Length(const Length &other)
    : value(other.getValue())
{
    
}

Length& Length::operator=(const Length& other)
{
    if (this != &other) {
        value = other.value;
    }
    return *this;
}

Length::~Length()
{

}

const PreciseDouble& Length::getValue() const 
{ 
    return value; 
}

Length operator+(const Length& lhs, const Length& rhs)
{
    return Length(lhs.getValue() + rhs.getValue());
}

Length operator+(const Length& lhs, const PreciseDouble& rhs)
{
    return Length(lhs.getValue() + rhs);
}

Length operator+(const PreciseDouble& lhs, const Length& rhs)
{
    return Length(lhs + rhs.getValue());
}

Length operator-(const Length& lhs, const Length& rhs)
{
    return Length(lhs.getValue() - rhs.getValue());
}

Length operator-(const Length& lhs, const PreciseDouble& rhs)
{
    return Length(lhs.getValue() - rhs);
}

Length operator-(const PreciseDouble& lhs, const Length& rhs)
{
    return Length(lhs - rhs.getValue());
}

bool operator<(const Length& lhs, const Length& rhs)
{
    return lhs.getValue() < rhs.getValue();
}

bool operator<(const Length& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() < rhs;
}

bool operator<(const PreciseDouble& lhs, const Length& rhs)
{
    return lhs < rhs.getValue();
}

bool operator<=(const Length& lhs, const Length& rhs)
{
    return lhs.getValue() <= rhs.getValue();
}

bool operator<=(const Length& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() <= rhs;
}

bool operator<=(const PreciseDouble& lhs, const Length& rhs)
{
    return lhs <= rhs.getValue();
}

bool operator>(const Length& lhs, const Length& rhs)
{
    return lhs.getValue() > rhs.getValue();
}

bool operator>(const Length& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() > rhs;
}

bool operator>(const PreciseDouble& lhs, const Length& rhs)
{
    return lhs > rhs.getValue();
}

bool operator>=(const Length& lhs, const Length& rhs)
{
    return lhs.getValue() >= rhs.getValue();
}

bool operator>=(const Length& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() >= rhs;
}

bool operator>=(const PreciseDouble& lhs, const Length& rhs)
{
    return lhs >= rhs.getValue();
}

bool operator==(const Length& lhs, const Length& rhs)
{
    return lhs.getValue() == rhs.getValue();
}

bool operator==(const Length& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() == rhs;
}

bool operator==(const PreciseDouble& lhs, const Length& rhs)
{
    return lhs == rhs.getValue();
}

bool operator!=(const Length& lhs, const Length& rhs)
{
    return lhs.getValue() != rhs.getValue();
}

bool operator!=(const Length& lhs, const PreciseDouble& rhs)
{
    return lhs.getValue() != rhs;
}

bool operator!=(const PreciseDouble& lhs, const Length& rhs)
{
    return lhs != rhs.getValue();
}

ostream& operator<<(ostream& os, const Length& length)
{
    os << length.getValue();
    return os;
}


template <class Archive>
void Length::serialize(Archive &ar, const unsigned int) {
    ar & value;
}

// // Specialisation
template void Length::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Length::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Length::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Length::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
