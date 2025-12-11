#include "App/Model/IBM/Physics/Temperature.h"


using namespace std;


Temperature::Temperature(const PreciseDouble& tempCelsius) : temperatureCelsius(tempCelsius) {

    if (tempCelsius < -273.15) {
        throw std::out_of_range("Temperature below absolute zero in Celsius");
    }
    
    // Initialize the other scales at the time of object creation
    temperatureFahrenheit = (tempCelsius * 9.0 / 5.0) + 32.0;
    temperatureKelvin = temperatureCelsius + 273.15;
}

Temperature::~Temperature()
{

}

Temperature::Temperature(const Temperature &other)
{
    temperatureCelsius = other.temperatureCelsius;
    temperatureFahrenheit = other.temperatureFahrenheit;
    temperatureKelvin = other.temperatureKelvin;
}

// Overloaded copy operator
Temperature& Temperature::operator=(const Temperature& other) {
    if (this != &other) {
        temperatureCelsius = other.temperatureCelsius;
        temperatureFahrenheit = other.temperatureFahrenheit;
        temperatureKelvin = other.temperatureKelvin;
    }
    return *this;
}

void Temperature::setTemperatureCelsius(const PreciseDouble& tempCelsius) {

    if (tempCelsius < -273.15) {
        throw std::out_of_range("Temperature below absolute zero in Celsius");
    }
    
    temperatureCelsius = tempCelsius;
    temperatureFahrenheit = (tempCelsius * 9.0 / 5.0) + 32.0;
    temperatureKelvin = temperatureCelsius + 273.15;
}

void Temperature::setTemperatureFahrenheit(const PreciseDouble& tempFahrenheit) {

    if (tempFahrenheit < -459.67) {
        throw std::out_of_range("Temperature below absolute zero in Fahrenheit");
    }
    
    temperatureFahrenheit = tempFahrenheit;
    temperatureCelsius = (tempFahrenheit - 32.0) * 5.0 / 9.0;
    temperatureKelvin = temperatureCelsius + 273.15;
}

void Temperature::setTemperatureKelvin(const PreciseDouble& tempKelvin) {

    if (tempKelvin < 0.0) {
        throw std::out_of_range("Temperature below absolute zero in Kelvin");
    }

    temperatureKelvin = tempKelvin;
    temperatureCelsius = tempKelvin - 273.15;
    temperatureFahrenheit = temperatureCelsius * 9.0 / 5.0 + 32.0;
}

const PreciseDouble& Temperature::getTemperatureCelsius() const {
    return temperatureCelsius;
}

const PreciseDouble& Temperature::getTemperatureFahrenheit() const {
    return temperatureFahrenheit;
}

const PreciseDouble& Temperature::getTemperatureKelvin() const {
    return temperatureKelvin;
}

// Overloaded addition operator
Temperature operator+(const Temperature& temp1, const Temperature& temp2) {
    Temperature result;
    result.setTemperatureCelsius(temp1.getTemperatureCelsius() + temp2.getTemperatureCelsius());
    return result;
}

// Overloaded subtraction operator
Temperature operator-(const Temperature& temp1, const Temperature& temp2) {
    Temperature result;
    result.setTemperatureCelsius(temp1.getTemperatureCelsius() - temp2.getTemperatureCelsius());
    return result;
}

// Overloaded multiplication operator
Temperature operator*(const Temperature& temp, const PreciseDouble& scalar) {
    Temperature result;
    result.setTemperatureCelsius(temp.getTemperatureCelsius() * scalar);
    return result;
}

// Overloaded division operator
Temperature operator/(const Temperature& temp, const PreciseDouble& divisor) {
    Temperature result;
    result.setTemperatureCelsius(temp.getTemperatureCelsius() / divisor);
    return result;
}

// Overloaded assignment operator
Temperature& Temperature::operator+=(const Temperature& other) {
    // Add the corresponding values from the other Temperature object
    temperatureCelsius += other.temperatureCelsius;
    temperatureFahrenheit += other.temperatureFahrenheit;
    temperatureKelvin += other.temperatureKelvin;

    return *this; // Return a reference to the modified Temperature object
}

/**
 * @brief Overloaded less-than operator.
 *
 * Compares two Temperature objects based on their Celsius values.
 *
 * @param other The Temperature object to compare with.
 * @return True if the current object is less than the other, false otherwise.
 */
bool Temperature::operator<(const Temperature& other) const {
    return temperatureCelsius < other.temperatureCelsius;
}

/**
 * @brief Overloaded less-or-equal-than operator.
 *
 * Compares two Temperature objects based on their Celsius values.
 *
 * @param other The Temperature object to compare with.
 * @return True if the current object is less than the other, false otherwise.
 */
bool Temperature::operator<=(const Temperature& other) const {
    return temperatureCelsius <= other.temperatureCelsius;
}

/**
 * @brief Overloaded greater-than operator.
 *
 * Compares two Temperature objects based on their Celsius values.
 *
 * @param other The Temperature object to compare with.
 * @return True if the current object is less than the other, false otherwise.
 */
bool Temperature::operator>(const Temperature& other) const {
    return temperatureCelsius > other.temperatureCelsius;
}

/**
 * @brief Overloaded greater-or-equal-than operator.
 *
 * Compares two Temperature objects based on their Celsius values.
 *
 * @param other The Temperature object to compare with.
 * @return True if the current object is less than the other, false otherwise.
 */
bool Temperature::operator>=(const Temperature& other) const {
    return temperatureCelsius >= other.temperatureCelsius;
}

bool Temperature::operator==(const Temperature& other) const {
    return temperatureCelsius == other.temperatureCelsius;
}


ostream& operator<<(ostream& os, const Temperature& temp) {
    os << temp.getTemperatureCelsius();
    return os;
}


size_t hash<Temperature>::operator()(const Temperature& temp) const 
{
    return hash<PreciseDouble>()(temp.getTemperatureCelsius());
}


template<class Archive>
void Temperature::serialize(Archive & ar, const unsigned int) {
    ar & temperatureCelsius; 
    ar & temperatureFahrenheit; 
    ar & temperatureKelvin; 
}

// Specialisation
template void Temperature::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Temperature::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Temperature::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Temperature::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
