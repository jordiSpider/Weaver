#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"

using namespace std;


TimeStep::TimeStep()
    : value(0)
{
    
}

TimeStep::TimeStep(const unsigned int value)
    : value(value)
{
    
}

TimeStep::TimeStep(const Day& day, const PreciseDouble& timeStepsPerDay)
    : value(static_cast<unsigned int>(round(day.getValue() / timeStepsPerDay).getValue()))
{

}

TimeStep::~TimeStep()
{

}

TimeStep::TimeStep(const TimeStep &other)
    : value(other.getValue())
{
    
}

TimeStep& TimeStep::operator=(const TimeStep& other)
{
    if (this != &other) {
        value = other.value;
    }
    return *this;
}

unsigned int TimeStep::getValue() const 
{ 
    return value; 
}

TimeStep operator+(const TimeStep& lhs, const TimeStep& rhs)
{
    return TimeStep(lhs.getValue() + rhs.getValue());
}

TimeStep operator-(const TimeStep& lhs, const TimeStep& rhs)
{
    return TimeStep(lhs.getValue() - rhs.getValue());
}

TimeStep operator*(const TimeStep& lhs, const unsigned int& rhs)
{
    return TimeStep(lhs.getValue() * rhs);
}

TimeStep operator*(const unsigned int& lhs, const TimeStep& rhs)
{
    return TimeStep(lhs * rhs.getValue());
}

TimeStep operator*(const TimeStep& lhs, const PreciseDouble& rhs)
{
    return TimeStep(static_cast<unsigned int>(round(static_cast<double>(lhs.getValue()) * rhs).getValue()));
}

TimeStep operator*(const PreciseDouble& lhs, const TimeStep& rhs)
{
    return TimeStep(static_cast<unsigned int>(round(lhs * static_cast<double>(rhs.getValue())).getValue()));
}

TimeStep operator/(const TimeStep& lhs, const TimeStep& rhs)
{
    return TimeStep(static_cast<unsigned int>(floor(lhs.getValue() / rhs.getValue())));
}

TimeStep operator/(const TimeStep& lhs, const unsigned int& rhs)
{
    return TimeStep(static_cast<unsigned int>(floor(lhs.getValue() / rhs)));
}

unsigned int operator%(const TimeStep& lhs, const TimeStep& rhs)
{
    return lhs.getValue() % rhs.getValue();
}

unsigned int operator%(const TimeStep& lhs, const unsigned int& rhs)
{
    return lhs.getValue() % rhs;
}

bool operator<(const TimeStep& lhs, const TimeStep& rhs)
{
    return lhs.getValue() < rhs.getValue();
}

bool operator<=(const TimeStep& lhs, const TimeStep& rhs)
{
    return lhs.getValue() <= rhs.getValue();
}

bool operator>(const TimeStep& lhs, const TimeStep& rhs)
{
    return lhs.getValue() > rhs.getValue();
}

bool operator>=(const TimeStep& lhs, const TimeStep& rhs)
{
    return lhs.getValue() >= rhs.getValue();
}

bool operator==(const TimeStep& lhs, const TimeStep& rhs)
{
    return lhs.getValue() == rhs.getValue();
}

bool operator!=(const TimeStep& lhs, const TimeStep& rhs)
{
    return lhs.getValue() != rhs.getValue();
}

ostream& operator<<(ostream& os, const TimeStep& timeStep)
{
    os << timeStep.getValue();
    return os;
}


template <class Archive>
void TimeStep::serialize(Archive &ar, const unsigned int) {
    ar & value;
}

// // Specialisation
template void TimeStep::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void TimeStep::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void TimeStep::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void TimeStep::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);




Day::Day()
    : value(0.0)
{
    
}

Day::Day(const PreciseDouble& value)
    : value(value)
{
    
}

Day::Day(const TimeStep& timeStep, const PreciseDouble& timeStepsPerDay)
    : value(timeStep.getValue() * timeStepsPerDay)
{

}

Day::~Day()
{

}

Day::Day(const Day &other)
    : value(other.getValue())
{
    
}

Day& Day::operator=(const Day& other)
{
    if (this != &other) {
        value = other.value;
    }
    return *this;
}

const PreciseDouble& Day::getValue() const 
{ 
    return value; 
}

Day operator+(const Day& lhs, const Day& rhs)
{
    return Day(lhs.getValue() + rhs.getValue());
}

Day operator-(const Day& lhs, const Day& rhs)
{
    return Day(lhs.getValue() - rhs.getValue());
}

Day operator*(const Day& lhs, const PreciseDouble& rhs)
{
    return Day(lhs.getValue() * rhs);
}

Day operator*(const PreciseDouble& lhs, const Day& rhs)
{
    return Day(lhs * rhs.getValue());
}

Day operator/(const Day& lhs, const PreciseDouble& rhs)
{
    return Day(lhs.getValue() / rhs);
}

bool operator<(const Day& lhs, const Day& rhs)
{
    return lhs.getValue() < rhs.getValue();
}

bool operator<=(const Day& lhs, const Day& rhs)
{
    return lhs.getValue() <= rhs.getValue();
}

bool operator>(const Day& lhs, const Day& rhs)
{
    return lhs.getValue() > rhs.getValue();
}

bool operator>=(const Day& lhs, const Day& rhs)
{
    return lhs.getValue() >= rhs.getValue();
}

bool operator==(const Day& lhs, const Day& rhs)
{
    return lhs.getValue() == rhs.getValue();
}

bool operator!=(const Day& lhs, const Day& rhs)
{
    return lhs.getValue() != rhs.getValue();
}

ostream& operator<<(ostream& os, const Day& day)
{
    os << day.getValue();
    return os;
}


template <class Archive>
void Day::serialize(Archive &ar, const unsigned int) {
    ar & value;
}

// // Specialisation
template void Day::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Day::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Day::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Day::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);

