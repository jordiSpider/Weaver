#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>


/**
 * @brief Class representing a temperature and allowing conversion between different scales.
 */
class Temperature {
private:
    double temperatureCelsius; /**< Temperature stored in Celsius degrees */
    double temperatureFahrenheit; /**< Temperature stored in Fahrenheit degrees */
    double temperatureKelvin; /**< Temperature stored in Kelvin */

    friend class boost::serialization::access;

public:
    /**
     * @brief Constructor of the class.
     * @param tempCelsius Temperature in Celsius degrees (default is 0.0).
     */
    explicit Temperature(double tempCelsius = 0.0);

    /**
     * @brief Sets the temperature in Celsius degrees.
     * @param tempCelsius Temperature in Celsius degrees.
     */
    void setTemperatureCelsius(double tempCelsius);

    /**
     * @brief Sets the temperature in Fahrenheit degrees.
     * @param tempFahrenheit Temperature in Fahrenheit degrees.
     */
    void setTemperatureFahrenheit(double tempFahrenheit);

    /**
     * @brief Sets the temperature in Kelvin.
     * @param tempKelvin Temperature in Kelvin.
     */
    void setTemperatureKelvin(double tempKelvin);

    /**
     * @brief Gets the temperature in Celsius degrees.
     * @return Temperature in Celsius degrees.
     */
    double getTemperatureCelsius() const;

    /**
     * @brief Gets the temperature in Fahrenheit degrees.
     * @return Temperature in Fahrenheit degrees.
     */
    double getTemperatureFahrenheit() const;

    /**
     * @brief Gets the temperature in Kelvin.
     * @return Temperature in Kelvin.
     */
    double getTemperatureKelvin() const;
    
    /**
     * @brief Overloaded assignment operator.
     *
     * This operator assigns the values of another Temperature object to the current object.
     *
     * @param other The Temperature object to copy values from.
     * @return A reference to the modified Temperature object.
     */
    Temperature& operator=(const Temperature& other);
    
    /**
     * @brief Overloaded addition-assignment operator.
     *
     * This operator adds the values of another Temperature object to the current object.
     *
     * @param other The Temperature object to add values from.
     * @return A reference to the modified Temperature object.
     */
    Temperature& operator+=(const Temperature& other);
    
    /**
     * @brief Overloaded less-than operator.
     *
     * Compares two Temperature objects based on their Celsius values.
     *
     * @param other The Temperature object to compare with.
     * @return True if the current object is less than the other, false otherwise.
     */
    bool operator<(const Temperature& other) const;
    
    /**
     * @brief Overloaded greater-than operator.
     *
     * Compares two Temperature objects based on their Celsius values.
     *
     * @param other The Temperature object to compare with.
     * @return True if the current object is less than the other, false otherwise.
     */
    bool operator>(const Temperature& other) const;
  
  
    /**
     * @brief Overloaded less-or-equal-than operator.
     *
     * Compares two Temperature objects based on their Celsius values.
     *
     * @param other The Temperature object to compare with.
     * @return True if the current object is less than the other, false otherwise.
     */
    bool operator<=(const Temperature& other) const;
    
    /**
     * @brief Overloaded greater-or-equal-than operator.
     *
     * Compares two Temperature objects based on their Celsius values.
     *
     * @param other The Temperature object to compare with.
     * @return True if the current object is less than the other, false otherwise.
     */
    bool operator>=(const Temperature& other) const;
    
    /**
     * @brief Serialize the object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

// Non-member operator overloads

/**
 * @brief Overloaded addition operator for temperatures.
 * @param temp1 First temperature.
 * @param temp2 Second temperature.
 * @return Result of the addition.
 */
Temperature operator+(const Temperature& temp1, const Temperature& temp2);

/**
 * @brief Overloaded subtraction operator for temperatures.
 * @param temp1 First temperature.
 * @param temp2 Second temperature.
 * @return Result of the subtraction.
 */
Temperature operator-(const Temperature& temp1, const Temperature& temp2);

/**
 * @brief Overloaded multiplication operator for temperatures.
 * @param temp Temperature.
 * @param scalar Scaling factor.
 * @return Result of the multiplication.
 */
Temperature operator*(const Temperature& temp, double scalar);

/**
 * @brief Overloaded division operator for temperatures.
 * @param temp Temperature.
 * @param divisor Divisor.
 * @return Result of the division.
 */
Temperature operator/(const Temperature& temp, double divisor);


#endif // TEMPERATURE_H

