#include "../include/Physics/Temperature.h"
#include <stdexcept>

Temperature::Temperature(double tempCelsius) : temperatureCelsius(tempCelsius) {

    if (tempCelsius < -273.15) {
        throw std::out_of_range("Temperature below absolute zero in Celsius");
    }
    
    // Initialize the other scales at the time of object creation
    temperatureFahrenheit = (tempCelsius * 9.0 / 5.0) + 32.0;
    temperatureKelvin = temperatureCelsius + 273.15;
}

void Temperature::setTemperatureCelsius(double tempCelsius) {

    if (tempCelsius < -273.15) {
        throw std::out_of_range("Temperature below absolute zero in Celsius");
    }
    
    temperatureCelsius = tempCelsius;
    temperatureFahrenheit = (tempCelsius * 9.0 / 5.0) + 32.0;
    temperatureKelvin = temperatureCelsius + 273.15;
}

void Temperature::setTemperatureFahrenheit(double tempFahrenheit) {

    if (tempFahrenheit < -459.67) {
        throw std::out_of_range("Temperature below absolute zero in Fahrenheit");
    }
    
    temperatureFahrenheit = tempFahrenheit;
    temperatureCelsius = (tempFahrenheit - 32.0) * 5.0 / 9.0;
    temperatureKelvin = temperatureCelsius + 273.15;
}

void Temperature::setTemperatureKelvin(double tempKelvin) {

    if (tempKelvin < 0.0) {
        throw std::out_of_range("Temperature below absolute zero in Kelvin");
    }

    temperatureKelvin = tempKelvin;
    temperatureCelsius = tempKelvin - 273.15;
    temperatureFahrenheit = temperatureCelsius * 9.0 / 5.0 + 32.0;
}

double Temperature::getTemperatureCelsius() const {
    return temperatureCelsius;
}

double Temperature::getTemperatureFahrenheit() const {
    return temperatureFahrenheit;
}

double Temperature::getTemperatureKelvin() const {
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
Temperature operator*(const Temperature& temp, double scalar) {
    Temperature result;
    result.setTemperatureCelsius(temp.getTemperatureCelsius() * scalar);
    return result;
}

// Overloaded division operator
Temperature operator/(const Temperature& temp, double divisor) {
    Temperature result;
    result.setTemperatureCelsius(temp.getTemperatureCelsius() / divisor);
    return result;
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
    return temperatureCelsius > other.temperatureCelsius;
}


