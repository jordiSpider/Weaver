/**
 * @file PreciseDouble.h
 * @brief Defines the PreciseDouble class for safe and precise double-precision arithmetic.
 * 
 * This class wraps a double value and provides arithmetic operations with tolerance-based comparisons,
 * safe mathematical functions, and debugging validation for invalid results (NaN, Inf).
 */

#ifndef PRECISE_DOUBLE_H_
#define PRECISE_DOUBLE_H_



#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <limits>
#include <functional>

#ifdef DEBUG
#include "Exceptions/LineInfoException.h"
#endif




/**
 * @brief Class for precise double-precision floating-point arithmetic.
 * 
 * Provides a wrapper around the native double type with strict validation,
 * tolerance-based comparison, and extended arithmetic operators.
 */
class PreciseDouble {
public:
    static constexpr double REL_TOL = 1e-9; /**< Relative tolerance for comparisons */
    static constexpr double ABS_TOL = 1e-12; /**< Absolute tolerance for comparisons */
    static constexpr double EPS     = 1e-9; /**< Small epsilon value for strict comparisons */

    /**
     * @brief Default constructor initializing to zero.
     * 
     */
    constexpr PreciseDouble() noexcept : value(0.0) {}

    /**
     * @brief Constructor initializing from a double value.
     * @param v The initial double value.
     */
    constexpr PreciseDouble(double v) noexcept : value(v) {}

    /**
     * @brief Returns the stored double value.
     * @return The encapsulated double value.
     */
    constexpr double getValue() const noexcept { return value; }

    /**
     * @brief Checks if two double values are approximately equal.
     * 
     * The comparison considers both relative and absolute tolerances.
     * 
     * @param a First value.
     * @param b Second value.
     * @return true if the values are nearly equal, false otherwise.
     */
    static inline bool almost_equal(double a, double b) noexcept {
        return std::fabs(a - b) <= std::max(REL_TOL * std::max(std::fabs(a), std::fabs(b)), ABS_TOL);
    }

    
    /** @name Arithmetic Operators (PreciseDouble)
     *  Basic arithmetic operations with validation (in DEBUG mode).
     */
    ///@{
    /** @brief Addition operator. */
    inline PreciseDouble operator+(const PreciseDouble& other) const noexcept {
        double r = value + other.value;

		#ifdef DEBUG
		validate(r);
		#endif

        return PreciseDouble(r);
    }

    /** @brief Subtraction operator. */
    inline PreciseDouble operator-(const PreciseDouble& other) const noexcept {
        double r = value - other.value;
        
		#ifdef DEBUG
		validate(r);
		#endif

        return PreciseDouble(r);
    }

    /** @brief Multiplication operator. */
    inline PreciseDouble operator*(const PreciseDouble& other) const noexcept {
        double r = value * other.value;
        
		#ifdef DEBUG
		validate(r);
		#endif

        return PreciseDouble(r);
    }

    /** @brief Unary negation operator. */
    inline PreciseDouble operator-() const noexcept {
        double r = -value;
        
		#ifdef DEBUG
		validate(r);
		#endif

        return PreciseDouble(r);
    }

    /**
     * @brief Division operator.
     * @throws Exception in DEBUG mode if dividing by zero.
     */
    inline PreciseDouble operator/(const PreciseDouble& other) const {
        #ifdef DEBUG
		if(other.value == 0.0)
		{
			throwLineInfoException("Division by zero in PreciseDouble.");
		}
		#endif
        
		double r = value / other.value;
        
		#ifdef DEBUG
		validate(r);
		#endif

        return PreciseDouble(r);
    }
    ///@}

    /** @name Compound Assignment Operators (PreciseDouble)
     *  Performs operation and updates the stored value.
     */
    ///@{
    /**
     * @brief Adds another PreciseDouble to this one.
     * @param other The PreciseDouble to add.
     * @return Reference to the updated PreciseDouble.
     * @note In debug mode, validates the internal value after addition.
     */
    inline PreciseDouble& operator+=(const PreciseDouble& other) noexcept {
        value += other.value;
        
        #ifdef DEBUG
        validate(value);
        #endif

        return *this;
    }

    /**
     * @brief Subtracts another PreciseDouble from this one.
     * @param other The PreciseDouble to subtract.
     * @return Reference to the updated PreciseDouble.
     * @note In debug mode, validates the internal value after subtraction.
     */
    inline PreciseDouble& operator-=(const PreciseDouble& other) noexcept {
        value -= other.value;
        
        #ifdef DEBUG
        validate(value);
        #endif

        return *this;
    }

    /**
     * @brief Multiplies this PreciseDouble by another.
     * @param other The PreciseDouble to multiply with.
     * @return Reference to the updated PreciseDouble.
     * @note In debug mode, validates the internal value after multiplication.
     */
    inline PreciseDouble& operator*=(const PreciseDouble& other) noexcept {
        value *= other.value;
        
        #ifdef DEBUG
        validate(value);
        #endif

        return *this;
    }

    /**
     * @brief Divides this PreciseDouble by another.
     * @param other The PreciseDouble to divide by.
     * @return Reference to the updated PreciseDouble.
     * @throws LineInfoException in debug mode if dividing by zero.
     * @note In debug mode, validates the internal value after division.
     */
    inline PreciseDouble& operator/=(const PreciseDouble& other) {
        #ifdef DEBUG
        if(other.value == 0.0)
        {
            throwLineInfoException("Division by zero in PreciseDouble.");
        }
        #endif

        value /= other.value;
        
        #ifdef DEBUG
        validate(value);
        #endif

        return *this;
    }
    ///@}

    /** @name Arithmetic Operators (double)
     *  Overloads to work directly with double values.
     */
    ///@{
    /**
     * @brief Adds a double value to this PreciseDouble and returns the result.
     * @param other The double value to add.
     * @return A new PreciseDouble with the result.
     */
    constexpr PreciseDouble operator+(double other) const noexcept { return PreciseDouble(value + other); }

    /**
     * @brief Subtracts a double value from this PreciseDouble and returns the result.
     * @param other The double value to subtract.
     * @return A new PreciseDouble with the result.
     */
    constexpr PreciseDouble operator-(double other) const noexcept { return PreciseDouble(value - other); }

    /**
     * @brief Multiplies this PreciseDouble by a double value and returns the result.
     * @param other The double value to multiply with.
     * @return A new PreciseDouble with the result.
     */
    constexpr PreciseDouble operator*(double other) const noexcept { return PreciseDouble(value * other); }

    /**
     * @brief Divides this PreciseDouble by a double value and returns the result.
     * @param other The double value to divide by.
     * @return A new PreciseDouble with the result.
     */
    constexpr PreciseDouble operator/(double other) const noexcept { return PreciseDouble(value / other); }

    /**
     * @brief Adds a double value to this PreciseDouble in-place.
     * @param other The double value to add.
     * @return Reference to the updated PreciseDouble.
     */
    constexpr PreciseDouble& operator+=(double other) noexcept { value += other; return *this; }

    /**
     * @brief Subtracts a double value from this PreciseDouble in-place.
     * @param other The double value to subtract.
     * @return Reference to the updated PreciseDouble.
     */
    constexpr PreciseDouble& operator-=(double other) noexcept { value -= other; return *this; }

    /**
     * @brief Multiplies this PreciseDouble by a double value in-place.
     * @param other The double value to multiply with.
     * @return Reference to the updated PreciseDouble.
     */
    constexpr PreciseDouble& operator*=(double other) noexcept { value *= other; return *this; }

    /**
     * @brief Divides this PreciseDouble by a double value in-place.
     * @param other The double value to divide by.
     * @return Reference to the updated PreciseDouble.
     */
    constexpr PreciseDouble& operator/=(double other) noexcept { value /= other; return *this; }
    ///@}
    
    /** @name Comparison Operators
     *  Comparison with tolerance handling to avoid floating-point issues.
     */
    ///@{
    /**
     * @brief Checks if this PreciseDouble is equal to another PreciseDouble.
     * @param other The PreciseDouble to compare with.
     * @return True if the values are approximately equal, false otherwise.
     */
    inline bool operator==(const PreciseDouble& other) const noexcept { return almost_equal(value, other.value); }

    /**
     * @brief Checks if this PreciseDouble is not equal to another PreciseDouble.
     * @param other The PreciseDouble to compare with.
     * @return True if the values are not approximately equal, false otherwise.
     */
    inline bool operator!=(const PreciseDouble& other) const noexcept { return !(*this == other); }

    /**
     * @brief Checks if this PreciseDouble is less than another PreciseDouble.
     * @param other The PreciseDouble to compare with.
     * @return True if this value is less than the other, accounting for EPS tolerance.
     */
    constexpr bool operator<(const PreciseDouble& other) const noexcept { return value < other.value - EPS; }

    /**
     * @brief Checks if this PreciseDouble is greater than another PreciseDouble.
     * @param other The PreciseDouble to compare with.
     * @return True if this value is greater than the other, accounting for EPS tolerance.
     */
    constexpr bool operator>(const PreciseDouble& other) const noexcept { return value > other.value + EPS; }

    /**
     * @brief Checks if this PreciseDouble is less than or equal to another PreciseDouble.
     * @param other The PreciseDouble to compare with.
     * @return True if this value is less than or equal to the other, accounting for EPS tolerance.
     */
    constexpr bool operator<=(const PreciseDouble& other) const noexcept { return !(*this > other); }

    /**
     * @brief Checks if this PreciseDouble is greater than or equal to another PreciseDouble.
     * @param other The PreciseDouble to compare with.
     * @return True if this value is greater than or equal to the other, accounting for EPS tolerance.
     */
    constexpr bool operator>=(const PreciseDouble& other) const noexcept { return !(*this < other); }

    /**
     * @brief Checks if this PreciseDouble is equal to a double value.
     * @param other The double value to compare with.
     * @return True if the values are approximately equal, false otherwise.
     */
    inline bool operator==(double other) const noexcept { return almost_equal(value, other); }

    /**
     * @brief Checks if this PreciseDouble is not equal to a double value.
     * @param other The double value to compare with.
     * @return True if the values are not approximately equal, false otherwise.
     */
    inline bool operator!=(double other) const noexcept { return !(*this == other); }

    /**
     * @brief Checks if this PreciseDouble is less than a double value.
     * @param other The double value to compare with.
     * @return True if this value is less than the double, accounting for EPS tolerance.
     */
    constexpr bool operator<(double other) const noexcept { return value < other - EPS; }

    /**
     * @brief Checks if this PreciseDouble is greater than a double value.
     * @param other The double value to compare with.
     * @return True if this value is greater than the double, accounting for EPS tolerance.
     */
    constexpr bool operator>(double other) const noexcept { return value > other + EPS; }

    /**
     * @brief Checks if this PreciseDouble is less than or equal to a double value.
     * @param other The double value to compare with.
     * @return True if this value is less than or equal to the double, accounting for EPS tolerance.
     */
    constexpr bool operator<=(double other) const noexcept { return !(*this > other); }

    /**
     * @brief Checks if this PreciseDouble is greater than or equal to a double value.
     * @param other The double value to compare with.
     * @return True if this value is greater than or equal to the double, accounting for EPS tolerance.
     */
    constexpr bool operator>=(double other) const noexcept { return !(*this < other); }
    ///@}
    
    /** @name Mathematical Friend Functions
     *  Common math operations compatible with PreciseDouble.
     */
    ///@{
    /**
     * @brief Computes the square root of a PreciseDouble.
     * @param r The input value.
     * @return A PreciseDouble containing the square root.
     */
    friend constexpr PreciseDouble sqrt(const PreciseDouble& r) noexcept {
        return PreciseDouble(std::sqrt(r.value));
    }

    /**
     * @brief Computes the absolute value of a PreciseDouble.
     * @param r The input value.
     * @return A PreciseDouble containing the absolute value.
     */
    friend constexpr PreciseDouble fabs(const PreciseDouble& r) noexcept {
        return PreciseDouble(std::fabs(r.value));
    }

    /**
     * @brief Computes r raised to the power of p.
     * @param r The base value.
     * @param p The exponent value.
     * @return A PreciseDouble containing r^p.
     */
    friend constexpr PreciseDouble pow(const PreciseDouble& r, const PreciseDouble& p) noexcept {
        return PreciseDouble(std::pow(r.value, p.value));
    }

    /**
     * @brief Computes the exponential function of a PreciseDouble.
     * @param r The input value.
     * @return A PreciseDouble containing e^r.
     */
    friend constexpr PreciseDouble exp(const PreciseDouble& r) noexcept {
        return PreciseDouble(std::exp(r.value));
    }

    /**
     * @brief Computes the natural logarithm of a PreciseDouble.
     * @param r The input value.
     * @return A PreciseDouble containing ln(r).
     */
    friend constexpr PreciseDouble log(const PreciseDouble& r) noexcept {
        return PreciseDouble(std::log(r.value));
    }

    /**
     * @brief Computes the floor of a PreciseDouble.
     * @param r The input value.
     * @return A PreciseDouble containing the largest integer not greater than r.
     */
    friend constexpr PreciseDouble floor(const PreciseDouble& r) noexcept {
        return PreciseDouble(std::floor(r.value));
    }

    /**
     * @brief Computes the nearest integer to a PreciseDouble, rounding halfway cases away from zero.
     * @param r The input value.
     * @return A PreciseDouble containing the rounded value.
     */
    friend constexpr PreciseDouble round(const PreciseDouble& r) noexcept {
        return PreciseDouble(std::round(r.value));
    }

    /**
     * @brief Computes the sine of a PreciseDouble (angle in radians).
     * @param r The input value in radians.
     * @return A PreciseDouble containing sin(r).
     */
    friend constexpr PreciseDouble sin(const PreciseDouble& r) noexcept {
        return PreciseDouble(std::sin(r.value));
    }

    /**
     * @brief Computes the cosine of a PreciseDouble (angle in radians).
     * @param r The input value in radians.
     * @return A PreciseDouble containing cos(r).
     */
    friend constexpr PreciseDouble cos(const PreciseDouble& r) noexcept {
        return PreciseDouble(std::cos(r.value));
    }

    /**
     * @brief Computes the tangent of a PreciseDouble (angle in radians).
     * @param r The input value in radians.
     * @return A PreciseDouble containing tan(r).
     */
    friend constexpr PreciseDouble tan(const PreciseDouble& r) noexcept {
        return PreciseDouble(std::tan(r.value));
    }
    ///@}

    /**
     * @brief Stream output operator.
     * @param os Output stream.
     * @param r The PreciseDouble instance to output.
     * @return Reference to the output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const PreciseDouble& r) {
        os << r.value;
        return os;
    }

    /**
     * @brief Converts the value to a string.
     * @return String representation of the value.
     */
    inline std::string to_string() const noexcept {
        return std::to_string(value);
    }

    /**
     * @brief Serialization function for archives.
     * @tparam Archive Type of the serialization archive.
     * @param ar The archive to serialize to/from.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int) {
        ar & value;
    }

private:
    double value; /**< Stored double value. */


	#ifdef DEBUG
    /**
     * @brief Validates a double value in debug mode.
     * @param v The value to validate.
     * @throws Exception if the value is NaN or infinity.
     */
	static void validate(double v) {
        if(std::isnan(v))
            throwLineInfoException("Invalid value: NaN detected in PreciseDouble.");
        if(std::isinf(v))
            throwLineInfoException("Invalid value: infinity detected in PreciseDouble.");
    }
	#endif
	
};

/** @name Global Arithmetic Operators
 *  Enables arithmetic between double and PreciseDouble in any order.
 */
///@{
/**
 * @brief Adds a double to a PreciseDouble.
 * @param a Left-hand side double value.
 * @param b Right-hand side PreciseDouble value.
 * @return A PreciseDouble representing the sum of a and b.
 */
constexpr PreciseDouble operator+(double a, const PreciseDouble& b) noexcept { return PreciseDouble(a + b.getValue()); }

/**
 * @brief Subtracts a PreciseDouble from a double.
 * @param a Left-hand side double value.
 * @param b Right-hand side PreciseDouble value.
 * @return A PreciseDouble representing the result of a - b.
 */
constexpr PreciseDouble operator-(double a, const PreciseDouble& b) noexcept { return PreciseDouble(a - b.getValue()); }

/**
 * @brief Multiplies a double by a PreciseDouble.
 * @param a Left-hand side double value.
 * @param b Right-hand side PreciseDouble value.
 * @return A PreciseDouble representing the product of a and b.
 */
constexpr PreciseDouble operator*(double a, const PreciseDouble& b) noexcept { return PreciseDouble(a * b.getValue()); }

/**
 * @brief Divides a double by a PreciseDouble.
 * @param a Left-hand side double value.
 * @param b Right-hand side PreciseDouble value.
 * @return A PreciseDouble representing the result of a / b.
 */
constexpr PreciseDouble operator/(double a, const PreciseDouble& b) noexcept { return PreciseDouble(a / b.getValue()); }
///@}

/** @name Global Comparison Operators
 *  Allows comparing double values with PreciseDouble instances.
 */
///@{
/**
 * @brief Checks if a double is approximately equal to a PreciseDouble.
 * @param a Left-hand side double value.
 * @param b Right-hand side PreciseDouble value.
 * @return true if a is approximately equal to b, false otherwise.
 */
inline bool operator==(double a, const PreciseDouble& b) noexcept { return PreciseDouble::almost_equal(a, b.getValue()); }

/**
 * @brief Checks if a double is not approximately equal to a PreciseDouble.
 * @param a Left-hand side double value.
 * @param b Right-hand side PreciseDouble value.
 * @return true if a is not approximately equal to b, false otherwise.
 */
inline bool operator!=(double a, const PreciseDouble& b) noexcept { return !PreciseDouble::almost_equal(a, b.getValue()); }

/**
 * @brief Checks if a double is less than a PreciseDouble, considering a small epsilon.
 * @param a Left-hand side double value.
 * @param b Right-hand side PreciseDouble value.
 * @return true if a is less than b minus epsilon, false otherwise.
 */
constexpr bool operator<(double a, const PreciseDouble& b) noexcept { return a < b.getValue() - PreciseDouble::EPS; }

/**
 * @brief Checks if a double is greater than a PreciseDouble, considering a small epsilon.
 * @param a Left-hand side double value.
 * @param b Right-hand side PreciseDouble value.
 * @return true if a is greater than b plus epsilon, false otherwise.
 */
constexpr bool operator>(double a, const PreciseDouble& b) noexcept { return a > b.getValue() + PreciseDouble::EPS; }

/**
 * @brief Checks if a double is less than or approximately equal to a PreciseDouble.
 * @param a Left-hand side double value.
 * @param b Right-hand side PreciseDouble value.
 * @return true if a is less than or approximately equal to b, false otherwise.
 */
inline bool operator<=(double a, const PreciseDouble& b) noexcept { return !(a > b.getValue()); }

/**
 * @brief Checks if a double is greater than or approximately equal to a PreciseDouble.
 * @param a Left-hand side double value.
 * @param b Right-hand side PreciseDouble value.
 * @return true if a is greater than or approximately equal to b, false otherwise.
 */
inline bool operator>=(double a, const PreciseDouble& b) noexcept { return !(a < b.getValue()); }
///@}

/**
 * @brief Returns the minimum of two PreciseDouble values.
 * @param a First value.
 * @param b Second value.
 * @return The smaller of the two.
 */
constexpr PreciseDouble fmin(const PreciseDouble& a, const PreciseDouble& b) noexcept {
    return (a < b) ? a : b;
}

/**
 * @brief Returns the maximum of two PreciseDouble values.
 * @param a First value.
 * @param b Second value.
 * @return The larger of the two.
 */
constexpr PreciseDouble fmax(const PreciseDouble& a, const PreciseDouble& b) noexcept {
    return (a > b) ? a : b;
}


/**
 * @namespace std
 * @brief Provides a specialization of std::hash for the custom type PreciseDouble.
 *
 * This namespace specialization allows PreciseDouble objects to be used as keys
 * in standard unordered containers (e.g., std::unordered_map, std::unordered_set)
 * by defining a hash function based on the internal double value.
 */
namespace std {
    /**
     * @brief Hash specialization for PreciseDouble.
     *
     * Implements the hash function required by unordered containers to handle
     * PreciseDouble objects. Uses the internal double representation of PreciseDouble
     * to compute the hash.
     */
    template<> struct hash<PreciseDouble> {
        /**
         * @brief Computes the hash of a PreciseDouble.
         * @param r The value to hash.
         * @return A hash value corresponding to the internal double.
         */
        std::size_t operator()(const PreciseDouble& r) const noexcept {
            return std::hash<double>{}(r.getValue());
        }
    };
}

#endif /* PRECISE_DOUBLE_H_ */
