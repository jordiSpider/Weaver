/**
 * @file TimeUnits.h
 * @brief Defines the TimeStep and Day classes for representing simulation time units.
 */

#ifndef TIME_UNITS_H_
#define TIME_UNITS_H_



#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include <ostream>
#include <cmath>


#include "App/Model/IBM/Maths/PreciseDouble.h"



class TimeStep;
class Day;


/**
 * @class TimeStep
 * @brief Represents a discrete time step in the simulation.
 *
 * Encapsulates an unsigned integer value for time steps and provides arithmetic
 * and comparison operators for simulation calculations.
 */
class TimeStep {
private:
    /// Number of time steps
    unsigned int value;

public:
    /// Default constructor, initializes value to 0
    TimeStep();

    /// Constructor from unsigned int
    explicit TimeStep(const unsigned int value);

    /// Constructor from Day and number of time steps per day
    explicit TimeStep(const Day& day, const PreciseDouble& timeStepsPerDay);

    /// Destructor
    ~TimeStep();

    /// Copy constructor
    TimeStep(const TimeStep& other);

    /// Copy assignment
    TimeStep& operator=(const TimeStep& other);

    /// Returns the numeric value of the time step
    unsigned int getValue() const;

    /// Returns the numeric value of the time step
    friend std::ostream& operator<<(std::ostream& os, const TimeStep& timeStep);

    /**
    * @brief Serializes the object for persistence.
    *
    * @tparam Archive Serialization archive type.
    * @param ar Archive instance.
    * @param version Serialization version.
    */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};


/** @name Arithmetic Operators for TimeStep
 */
///@{
/**
 * @brief Adds two TimeStep objects.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Right-hand side TimeStep.
 * @return A TimeStep representing the sum of lhs and rhs.
 */
TimeStep operator+(const TimeStep& lhs, const TimeStep& rhs);

/**
 * @brief Subtracts one TimeStep from another.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Right-hand side TimeStep.
 * @return A TimeStep representing the difference lhs - rhs.
 */
TimeStep operator-(const TimeStep& lhs, const TimeStep& rhs);

/**
 * @brief Multiplies a TimeStep by an unsigned integer.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Multiplier.
 * @return A TimeStep scaled by rhs.
 */
TimeStep operator*(const TimeStep& lhs, const unsigned int& rhs);

/**
 * @brief Multiplies an unsigned integer by a TimeStep.
 * @param lhs Multiplier.
 * @param rhs Right-hand side TimeStep.
 * @return A TimeStep scaled by lhs.
 */
TimeStep operator*(const unsigned int& lhs, const TimeStep& rhs);

/**
 * @brief Multiplies a TimeStep by a PreciseDouble.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Multiplier.
 * @return A TimeStep scaled by rhs.
 */
TimeStep operator*(const TimeStep& lhs, const PreciseDouble& rhs);

/**
 * @brief Multiplies a PreciseDouble by a TimeStep.
 * @param lhs Multiplier.
 * @param rhs Right-hand side TimeStep.
 * @return A TimeStep scaled by lhs.
 */
TimeStep operator*(const PreciseDouble& lhs, const TimeStep& rhs);

/**
 * @brief Divides one TimeStep by another TimeStep.
 * @param lhs Numerator TimeStep.
 * @param rhs Denominator TimeStep.
 * @return A TimeStep representing the quotient of lhs / rhs.
 */
TimeStep operator/(const TimeStep& lhs, const TimeStep& rhs);

/**
 * @brief Divides a TimeStep by an unsigned integer.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Divisor.
 * @return A TimeStep representing lhs / rhs.
 */
TimeStep operator/(const TimeStep& lhs, const unsigned int& rhs);

/**
 * @brief Computes the modulo of two TimeStep objects.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Right-hand side TimeStep.
 * @return The remainder of lhs divided by rhs as an unsigned int.
 */
unsigned int operator%(const TimeStep& lhs, const TimeStep& rhs);

/**
 * @brief Computes the modulo of a TimeStep by an unsigned integer.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Right-hand side unsigned integer.
 * @return The remainder of lhs divided by rhs as an unsigned int.
 */
unsigned int operator%(const TimeStep& lhs, const unsigned int& rhs);
///@}


/** @name Comparison Operators for TimeStep
 */
///@{
/**
 * @brief Checks if one TimeStep is less than another.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Right-hand side TimeStep.
 * @return True if lhs is less than rhs, false otherwise.
 */
bool operator<(const TimeStep& lhs, const TimeStep& rhs);

/**
 * @brief Checks if one TimeStep is less than or equal to another.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Right-hand side TimeStep.
 * @return True if lhs is less than or equal to rhs, false otherwise.
 */
bool operator<=(const TimeStep& lhs, const TimeStep& rhs);

/**
 * @brief Checks if one TimeStep is greater than another.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Right-hand side TimeStep.
 * @return True if lhs is greater than rhs, false otherwise.
 */
bool operator>(const TimeStep& lhs, const TimeStep& rhs);

/**
 * @brief Checks if one TimeStep is greater than or equal to another.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Right-hand side TimeStep.
 * @return True if lhs is greater than or equal to rhs, false otherwise.
 */
bool operator>=(const TimeStep& lhs, const TimeStep& rhs);

/**
 * @brief Checks if two TimeStep objects are equal.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Right-hand side TimeStep.
 * @return True if lhs is equal to rhs, false otherwise.
 */
bool operator==(const TimeStep& lhs, const TimeStep& rhs);

/**
 * @brief Checks if two TimeStep objects are not equal.
 * @param lhs Left-hand side TimeStep.
 * @param rhs Right-hand side TimeStep.
 * @return True if lhs is not equal to rhs, false otherwise.
 */
bool operator!=(const TimeStep& lhs, const TimeStep& rhs);
///@}


/**
 * @class Day
 * @brief Represents time in days using high-precision floating point.
 *
 * Encapsulates a PreciseDouble value for fractional days and provides arithmetic
 * and comparison operators for simulation calculations.
 */
class Day {
private:
    /// Number of days
    PreciseDouble value;

public:
    /// Default constructor, initializes value to 0
    Day();

    /// Constructor from a PreciseDouble value
    explicit Day(const PreciseDouble& value);

    /// Constructor from TimeStep and number of time steps per day
    explicit Day(const TimeStep& timeStep, const PreciseDouble& timeStepsPerDay);

    /// Destructor
    ~Day();

    /// Copy constructor
    Day(const Day& other);

    /// Copy assignment
    Day& operator=(const Day& other);

    /// Returns the numeric value of the day
    const PreciseDouble& getValue() const;
    
    /// Stream output operator
    friend std::ostream& operator<<(std::ostream& os, const Day& day);

    /**
    * @brief Serializes the object for persistence.
    *
    * @tparam Archive Serialization archive type.
    * @param ar Archive instance.
    * @param version Serialization version.
    */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};


/** @name Arithmetic Operators for Day
 */
///@{
/**
 * @brief Adds two Day objects.
 * @param lhs Left-hand side Day.
 * @param rhs Right-hand side Day.
 * @return A new Day representing the sum of lhs and rhs.
 */
Day operator+(const Day& lhs, const Day& rhs);

/**
 * @brief Subtracts one Day from another.
 * @param lhs Left-hand side Day.
 * @param rhs Right-hand side Day.
 * @return A new Day representing the difference lhs - rhs.
 */
Day operator-(const Day& lhs, const Day& rhs);

/**
 * @brief Multiplies a Day by a scalar.
 * @param lhs Day object.
 * @param rhs Scalar multiplier.
 * @return A new Day representing lhs multiplied by rhs.
 */
Day operator*(const Day& lhs, const PreciseDouble& rhs);

/**
 * @brief Multiplies a scalar by a Day.
 * @param lhs Scalar multiplier.
 * @param rhs Day object.
 * @return A new Day representing lhs multiplied by rhs.
 */
Day operator*(const PreciseDouble& lhs, const Day& rhs);

/**
 * @brief Divides a Day by a scalar.
 * @param lhs Day object.
 * @param rhs Scalar divisor.
 * @return A new Day representing lhs divided by rhs.
 */
Day operator/(const Day& lhs, const PreciseDouble& rhs);
///@}


/** @name Comparison Operators for Day
 */
///@{
/**
 * @brief Checks if one Day is less than another.
 * @param lhs Left-hand side Day.
 * @param rhs Right-hand side Day.
 * @return true if lhs is less than rhs, false otherwise.
 */
bool operator<(const Day& lhs, const Day& rhs);

/**
 * @brief Checks if one Day is less than or equal to another.
 * @param lhs Left-hand side Day.
 * @param rhs Right-hand side Day.
 * @return true if lhs is less than or equal to rhs, false otherwise.
 */
bool operator<=(const Day& lhs, const Day& rhs);

/**
 * @brief Checks if one Day is greater than another.
 * @param lhs Left-hand side Day.
 * @param rhs Right-hand side Day.
 * @return true if lhs is greater than rhs, false otherwise.
 */
bool operator>(const Day& lhs, const Day& rhs);

/**
 * @brief Checks if one Day is greater than or equal to another.
 * @param lhs Left-hand side Day.
 * @param rhs Right-hand side Day.
 * @return true if lhs is greater than or equal to rhs, false otherwise.
 */
bool operator>=(const Day& lhs, const Day& rhs);

/**
 * @brief Checks if two Days are equal.
 * @param lhs Left-hand side Day.
 * @param rhs Right-hand side Day.
 * @return true if lhs is equal to rhs, false otherwise.
 */
bool operator==(const Day& lhs, const Day& rhs);

/**
 * @brief Checks if two Days are not equal.
 * @param lhs Left-hand side Day.
 * @param rhs Right-hand side Day.
 * @return true if lhs is not equal to rhs, false otherwise.
 */
bool operator!=(const Day& lhs, const Day& rhs);
///@}

#endif /* TIME_UNITS_H_ */
