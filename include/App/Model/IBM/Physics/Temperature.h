/**
 * @file Temperature.h
 * @brief Definition of the Temperature class and related operations.
 *
 * This file defines the Temperature class, which represents a temperature value
 * and allows conversion and arithmetic operations between different temperature scales
 * (Celsius, Fahrenheit, Kelvin). It also includes serialization support through Boost.
 */

#ifndef TEMPERATURE_H
#define TEMPERATURE_H


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include <fstream>
#include <functional>
#include <stdexcept>

#include "App/Model/IBM/Maths/PreciseDouble.h"


/**
 * @class Temperature
 * @brief Represents a temperature and provides conversions and operations between scales.
 *
 * The Temperature class manages a temperature value expressed in three scales:
 * Celsius, Fahrenheit, and Kelvin. It supports arithmetic and comparison operators,
 * as well as serialization via Boost archives.
 *
 * Example usage:
 * @code
 * Temperature t1(25.0);
 * Temperature t2(10.0);
 * Temperature result = t1 + t2;
 * std::cout << result.getTemperatureCelsius() << std::endl;
 * @endcode
 */
class Temperature {
private:
    PreciseDouble temperatureCelsius;    /**< Temperature stored in Celsius degrees. */
    PreciseDouble temperatureFahrenheit; /**< Temperature stored in Fahrenheit degrees. */
    PreciseDouble temperatureKelvin;     /**< Temperature stored in Kelvin. */

public:
    /**
     * @brief Constructs a Temperature object.
     * @param tempCelsius Temperature in Celsius degrees (default is 0.0).
     */
    explicit Temperature(const PreciseDouble& tempCelsius = 0.0);

    /**
     * @brief Destroys the Temperature object.
     */
    ~Temperature();

    /**
     * @brief Copy constructor.
     * @param other The Temperature object to copy values from.
     */
    Temperature(const Temperature& other);

    /**
     * @brief Assignment operator.
     * @param other The Temperature object to copy values from.
     * @return A reference to the modified Temperature object.
     */
    Temperature& operator=(const Temperature& other);

    /**
     * @brief Sets the temperature in Celsius degrees.
     * @param tempCelsius Temperature in Celsius degrees.
     */
    void setTemperatureCelsius(const PreciseDouble& tempCelsius);

    /**
     * @brief Sets the temperature in Fahrenheit degrees.
     * @param tempFahrenheit Temperature in Fahrenheit degrees.
     */
    void setTemperatureFahrenheit(const PreciseDouble& tempFahrenheit);

    /**
     * @brief Sets the temperature in Kelvin.
     * @param tempKelvin Temperature in Kelvin.
     */
    void setTemperatureKelvin(const PreciseDouble& tempKelvin);

    /**
     * @brief Gets the temperature in Celsius degrees.
     * @return Temperature in Celsius degrees.
     */
    const PreciseDouble& getTemperatureCelsius() const;

    /**
     * @brief Gets the temperature in Fahrenheit degrees.
     * @return Temperature in Fahrenheit degrees.
     */
    const PreciseDouble& getTemperatureFahrenheit() const;

    /**
     * @brief Gets the temperature in Kelvin.
     * @return Temperature in Kelvin.
     */
    const PreciseDouble& getTemperatureKelvin() const;
    
    /**
     * @brief Adds another Temperature to this one.
     * @param other The Temperature object to add values from.
     * @return A reference to the modified Temperature object.
     */
    Temperature& operator+=(const Temperature& other);
    
    /**
     * @brief Checks if this Temperature is less than another.
     * @param other The Temperature object to compare with.
     * @return True if this object is less than the other, false otherwise.
     */
    bool operator<(const Temperature& other) const;
    
    /**
     * @brief Checks if this Temperature is greater than another.
     * @param other The Temperature object to compare with.
     * @return True if this object is greater than the other, false otherwise.
     */
    bool operator>(const Temperature& other) const;
  
  
    /**
     * @brief Checks if this Temperature is less than or equal to another.
     * @param other The Temperature object to compare with.
     * @return True if this object is less than or equal to the other, false otherwise.
     */
    bool operator<=(const Temperature& other) const;
    
    /**
     * @brief Checks if this Temperature is greater than or equal to another.
     * @param other The Temperature object to compare with.
     * @return True if this object is greater than or equal to the other, false otherwise.
     */
    bool operator>=(const Temperature& other) const;

    /**
     * @brief Checks equality between two Temperature objects.
     * @param other The Temperature object to compare with.
     * @return True if both Temperature objects are equal, false otherwise.
     */
    bool operator==(const Temperature& other) const;
    
    /**
     * @brief Serializes the Temperature object using Boost.
     *
     * This function allows saving or loading the Temperature object
     * to/from a binary or text archive.
     *
     * @tparam Archive Type of the Boost archive.
     * @param ar Archive object.
     * @param version Serialization version (currently unused).
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

/**
 * @brief Adds two Temperature objects.
 * @param temp1 First temperature.
 * @param temp2 Second temperature.
 * @return Resulting Temperature.
 */
Temperature operator+(const Temperature& temp1, const Temperature& temp2);

/**
 * @brief Subtracts one Temperature from another.
 * @param temp1 First temperature.
 * @param temp2 Second temperature.
 * @return Resulting Temperature.
 */
Temperature operator-(const Temperature& temp1, const Temperature& temp2);

/**
 * @brief Multiplies a Temperature by a scalar.
 * @param temp Temperature value.
 * @param scalar Scaling factor.
 * @return Resulting Temperature.
 */
Temperature operator*(const Temperature& temp, const PreciseDouble& scalar);

/**
 * @brief Divides a Temperature by a scalar.
 * @param temp Temperature value.
 * @param divisor Divisor value.
 * @return Resulting Temperature.
 */
Temperature operator/(const Temperature& temp, const PreciseDouble& divisor);

/**
 * @brief Stream insertion operator for Temperature.
 * @param os Output stream.
 * @param temp Temperature to output.
 * @return Reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, const Temperature& temp);


/**
 * @namespace std
 * @brief Provides a specialization of std::hash for the custom type Temperature.
 *
 * This specialization allows Temperature objects to be used as keys
 * in standard unordered containers (e.g., std::unordered_map, std::unordered_set)
 * by defining a hash function based on the internal representation of Temperature.
 */
namespace std {
    /**
     * @brief Hash specialization for Temperature.
     *
     * Implements the hash function required by unordered containers to handle
     * Temperature objects. The hash is computed from the internal value of Temperature.
     */
    template <>
    struct hash<Temperature> {
        /**
         * @brief Computes a hash value for a Temperature object.
         * @param temp Temperature object to hash.
         * @return The hash value.
         */
        size_t operator()(const Temperature& temp) const;
    };
}

#endif // TEMPERATURE_H

