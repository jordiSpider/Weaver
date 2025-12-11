/**
 * @file Instar.h
 * @brief Defines the Instar class representing developmental stages of an animal.
 */

#ifndef INSTAR_H_
#define INSTAR_H_


#include <ostream>
#include <string>


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#ifdef DEBUG
#include "Exceptions/LineInfoException.h"
#endif



/**
 * @class Instar
 * @brief Represents a developmental stage (instar) of an animal in the simulation.
 *
 * This class encapsulates the concept of an instar, which is a discrete developmental
 * stage between molts or growth events in invertebrates or other species with defined life stages.
 * Provides arithmetic and comparison operations, and supports serialization.
 */
class Instar {
public:
    /**
     * @brief Converts an unsigned integer value to an Instar object.
     * @param value The numeric value to convert.
     * @return Corresponding Instar object.
     */
    static Instar convertToIndex(const unsigned int value);

    /// Default constructor, initializes to zero.
    Instar();

    /// Constructs an Instar with a specific value.
    /// @param value Numeric instar value.
    explicit Instar(unsigned int value);

    /// Copy constructor.
    Instar(const Instar &other);

    /// Copy assignment operator.
    Instar& operator=(const Instar& other);

    /// Destructor.
    ~Instar();

    /// Returns the numeric value of the instar.
    /// @return Instar value as unsigned int.
    unsigned int getValue() const;

    /// Moves to the previous instar stage, if possible.
    void moveOnPreviousInstar();

    /// Moves to the next instar stage.
    void moveOnNextInstar();

    /** @name Arithmetic operators with unsigned int.
     */
    ///@{
    /**
     * @brief Adds an unsigned integer to an Instar value.
     * 
     * @param lhs The Instar value.
     * @param rhs The integer to add.
     * @return Resulting Instar after addition.
     */
    friend Instar operator+(const Instar& lhs, const unsigned int& rhs);

    /**
     * @brief Adds an Instar value to an unsigned integer.
     * 
     * @param lhs The integer value.
     * @param rhs The Instar value.
     * @return Resulting Instar after addition.
     */
    friend Instar operator+(const unsigned int& lhs, const Instar& rhs);

    /**
     * @brief Subtracts an unsigned integer from an Instar value.
     * 
     * @param lhs The Instar value.
     * @param rhs The integer to subtract.
     * @return Resulting Instar after subtraction.
     */
    friend Instar operator-(const Instar& lhs, const unsigned int& rhs);

    /**
     * @brief Subtracts an Instar value from an unsigned integer.
     * 
     * @param lhs The integer value.
     * @param rhs The Instar value.
     * @return Resulting Instar after subtraction.
     */
    friend Instar operator-(const unsigned int& lhs, const Instar& rhs);
    ///@}

    /// Prefix increment operator.
    Instar& operator++();

    /// Postfix increment operator.
    Instar operator++(int);

    /** @name Comparison operators between Instar and Instar or unsigned int.
     */
    ///@{
    /**
     * @brief Compares two Instar values for less-than.
     * @param lhs Left-hand Instar.
     * @param rhs Right-hand Instar.
     * @return True if lhs < rhs, false otherwise.
     */
    friend bool operator<(const Instar& lhs, const Instar& rhs);

    /**
     * @brief Compares an Instar with an unsigned int for less-than.
     * @param lhs Instar value.
     * @param rhs Unsigned integer value.
     * @return True if lhs < rhs, false otherwise.
     */
    friend bool operator<(const Instar& lhs, const unsigned int& rhs);

    /**
     * @brief Compares an unsigned int with an Instar for less-than.
     * @param lhs Unsigned integer value.
     * @param rhs Instar value.
     * @return True if lhs < rhs, false otherwise.
     */
    friend bool operator<(const unsigned int& lhs, const Instar& rhs);

    /**
     * @brief Compares two Instar values for less-than-or-equal.
     * @param lhs Left-hand Instar.
     * @param rhs Right-hand Instar.
     * @return True if lhs <= rhs, false otherwise.
     */
    friend bool operator<=(const Instar& lhs, const Instar& rhs);

    /**
     * @brief Compares an Instar with an unsigned int for less-than-or-equal.
     * @param lhs Instar value.
     * @param rhs Unsigned integer value.
     * @return True if lhs <= rhs, false otherwise.
     */
    friend bool operator<=(const Instar& lhs, const unsigned int& rhs);

    /**
     * @brief Compares an unsigned int with an Instar for less-than-or-equal.
     * @param lhs Unsigned integer value.
     * @param rhs Instar value.
     * @return True if lhs <= rhs, false otherwise.
     */
    friend bool operator<=(const unsigned int& lhs, const Instar& rhs);

    /**
     * @brief Compares two Instar values for greater-than.
     * @param lhs Left-hand Instar.
     * @param rhs Right-hand Instar.
     * @return True if lhs > rhs, false otherwise.
     */
    friend bool operator>(const Instar& lhs, const Instar& rhs);

    /**
     * @brief Compares an Instar with an unsigned int for greater-than.
     * @param lhs Instar value.
     * @param rhs Unsigned integer value.
     * @return True if lhs > rhs, false otherwise.
     */
    friend bool operator>(const Instar& lhs, const unsigned int& rhs);

    /**
     * @brief Compares an unsigned int with an Instar for greater-than.
     * @param lhs Unsigned integer value.
     * @param rhs Instar value.
     * @return True if lhs > rhs, false otherwise.
     */
    friend bool operator>(const unsigned int& lhs, const Instar& rhs);

    /**
     * @brief Compares two Instar values for greater-than-or-equal.
     * @param lhs Left-hand Instar.
     * @param rhs Right-hand Instar.
     * @return True if lhs >= rhs, false otherwise.
     */
    friend bool operator>=(const Instar& lhs, const Instar& rhs);

    /**
     * @brief Compares an Instar with an unsigned int for greater-than-or-equal.
     * @param lhs Instar value.
     * @param rhs Unsigned integer value.
     * @return True if lhs >= rhs, false otherwise.
     */
    friend bool operator>=(const Instar& lhs, const unsigned int& rhs);

    /**
     * @brief Compares an unsigned int with an Instar for greater-than-or-equal.
     * @param lhs Unsigned integer value.
     * @param rhs Instar value.
     * @return True if lhs >= rhs, false otherwise.
     */
    friend bool operator>=(const unsigned int& lhs, const Instar& rhs);

    /**
     * @brief Compares two Instar values for equality.
     * @param lhs Left-hand Instar.
     * @param rhs Right-hand Instar.
     * @return True if lhs == rhs, false otherwise.
     */
    friend bool operator==(const Instar& lhs, const Instar& rhs);

    /**
     * @brief Compares an Instar with an unsigned int for equality.
     * @param lhs Instar value.
     * @param rhs Unsigned integer value.
     * @return True if lhs == rhs, false otherwise.
     */
    friend bool operator==(const Instar& lhs, const unsigned int& rhs);

    /**
     * @brief Compares an unsigned int with an Instar for equality.
     * @param lhs Unsigned integer value.
     * @param rhs Instar value.
     * @return True if lhs == rhs, false otherwise.
     */
    friend bool operator==(const unsigned int& lhs, const Instar& rhs);

    /**
     * @brief Compares two Instar values for inequality.
     * @param lhs Left-hand Instar.
     * @param rhs Right-hand Instar.
     * @return True if lhs != rhs, false otherwise.
     */
    friend bool operator!=(const Instar& lhs, const Instar& rhs);

    /**
     * @brief Compares an Instar with an unsigned int for inequality.
     * @param lhs Instar value.
     * @param rhs Unsigned integer value.
     * @return True if lhs != rhs, false otherwise.
     */
    friend bool operator!=(const Instar& lhs, const unsigned int& rhs);

    /**
     * @brief Compares an unsigned int with an Instar for inequality.
     * @param lhs Unsigned integer value.
     * @param rhs Instar value.
     * @return True if lhs != rhs, false otherwise.
     */
    friend bool operator!=(const unsigned int& lhs, const Instar& rhs);
    ///@}

    /// Converts the instar value to a string representation.
    std::string to_string() const;

    /// Stream insertion operator.
    friend std::ostream& operator<<(std::ostream& os, const Instar& instar);

    /// Converts the instar to a size_t for indexing or hashing.
    explicit operator size_t() const;

    /**
    * @brief Serializes the object for persistence.
    *
    * @tparam Archive Serialization archive type.
    * @param ar Archive instance.
    * @param version Serialization version.
    */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    unsigned int value; ///< Numeric value of the instar.
};


/**
 * @brief Hash specialization for Instar to allow usage in unordered containers.
 *
 * This enables Instar objects to be used as keys in containers such as
 * std::unordered_map or std::unordered_set by providing a hash function.
 */
template <>
struct std::hash<Instar> {
    /**
     * @brief Computes a hash value for the given Instar.
     * @param instar The Instar object to hash.
     * @return A hash value corresponding to the internal representation of Instar.
     */
    std::size_t operator()(const Instar& instar) const;
};

#endif /* INSTAR_H_ */
