/**
 * @file StructuralUnits.h
 * @brief Defines physical units used in the simulation: DryMass, WetMass, and Length.
 */

#ifndef STRUCTURAL_UNITS_H_
#define STRUCTURAL_UNITS_H_



#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include <ostream>
#include <cmath>


#include "App/Model/IBM/Maths/PreciseDouble.h"


class DryMass;
class WetMass;
class Length;


/**
 * @class DryMass
 * @brief Represents dry mass in the ecosystem simulation.
 *
 * Encapsulates a PreciseDouble value for dry mass, provides arithmetic operations,
 * comparisons, and conversion from WetMass or Length using species-specific coefficients.
 */
class DryMass {
private:
    /// Dry mass value
    PreciseDouble value;

public:
    /// Default constructor, initializes value to 0
    DryMass();
    
    /// Constructor from a PreciseDouble
    explicit DryMass(const PreciseDouble& value);

    /// Constructor from WetMass and conversion coefficient
    explicit DryMass(const WetMass& wetMass, const PreciseDouble& conversionToWetMass);

    /// Constructor from Length and species-specific mass coefficients
    explicit DryMass(const Length& length, const PreciseDouble& coefficientForMassA, const PreciseDouble& scaleForMassB);
    
    /// Copy constructor
    DryMass(const DryMass &other);
    
    /// Copy assignment
    DryMass& operator=(const DryMass& other);

    /// Destructor
    ~DryMass();

    /// Returns the numeric value of the dry mass
    const PreciseDouble& getValue() const;

    /// Adds another DryMass to this one
    DryMass& operator+=(const DryMass& other);

    /// Division by scalar
    DryMass operator/(const PreciseDouble& scalar) const;
    
    /// Stream output operator
    friend std::ostream& operator<<(std::ostream& os, const DryMass& dryMass);

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

/** @name Arithmetic Operators for DryMass
 */
///@{
/**
 * @brief Adds two DryMass objects.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side DryMass.
 * @return Resulting DryMass from addition.
 */
DryMass operator+(const DryMass& lhs, const DryMass& rhs);

/**
 * @brief Adds a DryMass and a PreciseDouble.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return Resulting DryMass from addition.
 */
DryMass operator+(const DryMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Adds a PreciseDouble and a DryMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side DryMass.
 * @return Resulting DryMass from addition.
 */
DryMass operator+(const PreciseDouble& lhs, const DryMass& rhs);

/**
 * @brief Subtracts one DryMass from another.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side DryMass.
 * @return Resulting DryMass from subtraction.
 */
DryMass operator-(const DryMass& lhs, const DryMass& rhs);

/**
 * @brief Subtracts a PreciseDouble from a DryMass.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return Resulting DryMass from subtraction.
 */
DryMass operator-(const DryMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Subtracts a DryMass from a PreciseDouble.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side DryMass.
 * @return Resulting DryMass from subtraction.
 */
DryMass operator-(const PreciseDouble& lhs, const DryMass& rhs);
///@}

/** @name Comparison Operators for DryMass
 */
///@{
/**
 * @brief Checks if one DryMass is less than another.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs < rhs, false otherwise.
 */
bool operator<(const DryMass& lhs, const DryMass& rhs);

/**
 * @brief Checks if a DryMass is less than a PreciseDouble.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs < rhs, false otherwise.
 */
bool operator<(const DryMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is less than a DryMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs < rhs, false otherwise.
 */
bool operator<(const PreciseDouble& lhs, const DryMass& rhs);

/**
 * @brief Checks if one DryMass is less than or equal to another.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs <= rhs, false otherwise.
 */
bool operator<=(const DryMass& lhs, const DryMass& rhs);

/**
 * @brief Checks if a DryMass is less than or equal to a PreciseDouble.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs <= rhs, false otherwise.
 */
bool operator<=(const DryMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is less than or equal to a DryMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs <= rhs, false otherwise.
 */
bool operator<=(const PreciseDouble& lhs, const DryMass& rhs);

/**
 * @brief Checks if one DryMass is greater than another.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs > rhs, false otherwise.
 */
bool operator>(const DryMass& lhs, const DryMass& rhs);

/**
 * @brief Checks if a DryMass is greater than a PreciseDouble.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs > rhs, false otherwise.
 */
bool operator>(const DryMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is greater than a DryMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs > rhs, false otherwise.
 */
bool operator>(const PreciseDouble& lhs, const DryMass& rhs);

/**
 * @brief Checks if one DryMass is greater than or equal to another.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs >= rhs, false otherwise.
 */
bool operator>=(const DryMass& lhs, const DryMass& rhs);

/**
 * @brief Checks if a DryMass is greater than or equal to a PreciseDouble.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs >= rhs, false otherwise.
 */
bool operator>=(const DryMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is greater than or equal to a DryMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs >= rhs, false otherwise.
 */
bool operator>=(const PreciseDouble& lhs, const DryMass& rhs);

/**
 * @brief Checks if two DryMass objects are equal.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs == rhs, false otherwise.
 */
bool operator==(const DryMass& lhs, const DryMass& rhs);

/**
 * @brief Checks if a DryMass is equal to a PreciseDouble.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs == rhs, false otherwise.
 */
bool operator==(const DryMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is equal to a DryMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs == rhs, false otherwise.
 */
bool operator==(const PreciseDouble& lhs, const DryMass& rhs);

/**
 * @brief Checks if two DryMass objects are not equal.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs != rhs, false otherwise.
 */
bool operator!=(const DryMass& lhs, const DryMass& rhs);

/**
 * @brief Checks if a DryMass is not equal to a PreciseDouble.
 * @param lhs Left-hand side DryMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs != rhs, false otherwise.
 */
bool operator!=(const DryMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is not equal to a DryMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side DryMass.
 * @return True if lhs != rhs, false otherwise.
 */
bool operator!=(const PreciseDouble& lhs, const DryMass& rhs);
///@}


/**
 * @class WetMass
 * @brief Represents wet mass in the ecosystem simulation.
 *
 * Encapsulates a PreciseDouble value for wet mass and provides arithmetic operations,
 * comparisons, and conversion from DryMass using conversion coefficients.
 */
class WetMass {
private:
    /// Wet mass value
    PreciseDouble value;

public:
    /// Default constructor, initializes value to 0
    WetMass();

    /// Constructor from a PreciseDouble
    explicit WetMass(const PreciseDouble& value);

    /// Constructor from DryMass and conversion coefficient
    explicit WetMass(const DryMass& dryMass, const PreciseDouble& conversionToWetMass);

    /// Copy constructor
    WetMass(const WetMass &other);

    /// Copy assignment
    WetMass& operator=(const WetMass& other);

    /// Destructor
    ~WetMass();

    /// Returns the numeric value of the wet mass
    const PreciseDouble& getValue() const;
    
    /// Stream output operator
    friend std::ostream& operator<<(std::ostream& os, const WetMass& wetMass);

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


/** @name Arithmetic Operators for WetMass
 */
///@{
/**
 * @brief Adds two WetMass objects.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side WetMass.
 * @return The sum as a new WetMass.
 */
WetMass operator+(const WetMass& lhs, const WetMass& rhs);

/**
 * @brief Adds a WetMass and a PreciseDouble.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return The sum as a new WetMass.
 */
WetMass operator+(const WetMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Adds a PreciseDouble and a WetMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side WetMass.
 * @return The sum as a new WetMass.
 */
WetMass operator+(const PreciseDouble& lhs, const WetMass& rhs);

/**
 * @brief Subtracts one WetMass from another.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side WetMass.
 * @return The difference as a new WetMass.
 */
WetMass operator-(const WetMass& lhs, const WetMass& rhs);

/**
 * @brief Subtracts a PreciseDouble from a WetMass.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return The difference as a new WetMass.
 */
WetMass operator-(const WetMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Subtracts a WetMass from a PreciseDouble.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side WetMass.
 * @return The difference as a new WetMass.
 */
WetMass operator-(const PreciseDouble& lhs, const WetMass& rhs);
///@}

/** @name Comparison Operators for WetMass
 */
///@{
/**
 * @brief Checks if one WetMass is less than another WetMass.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs < rhs, false otherwise.
 */
bool operator<(const WetMass& lhs, const WetMass& rhs);

/**
 * @brief Checks if a WetMass is less than a PreciseDouble.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs < rhs, false otherwise.
 */
bool operator<(const WetMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is less than a WetMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs < rhs, false otherwise.
 */
bool operator<(const PreciseDouble& lhs, const WetMass& rhs);

/**
 * @brief Checks if one WetMass is less than or equal to another WetMass.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs <= rhs, false otherwise.
 */
bool operator<=(const WetMass& lhs, const WetMass& rhs);

/**
 * @brief Checks if a WetMass is less than or equal to a PreciseDouble.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs <= rhs, false otherwise.
 */
bool operator<=(const WetMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is less than or equal to a WetMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs <= rhs, false otherwise.
 */
bool operator<=(const PreciseDouble& lhs, const WetMass& rhs);

/**
 * @brief Checks if one WetMass is greater than another WetMass.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs > rhs, false otherwise.
 */
bool operator>(const WetMass& lhs, const WetMass& rhs);

/**
 * @brief Checks if a WetMass is greater than a PreciseDouble.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs > rhs, false otherwise.
 */
bool operator>(const WetMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is greater than a WetMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs > rhs, false otherwise.
 */
bool operator>(const PreciseDouble& lhs, const WetMass& rhs);

/**
 * @brief Checks if one WetMass is greater than or equal to another WetMass.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs >= rhs, false otherwise.
 */
bool operator>=(const WetMass& lhs, const WetMass& rhs);

/**
 * @brief Checks if a WetMass is greater than or equal to a PreciseDouble.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs >= rhs, false otherwise.
 */
bool operator>=(const WetMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is greater than or equal to a WetMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs >= rhs, false otherwise.
 */
bool operator>=(const PreciseDouble& lhs, const WetMass& rhs);

/**
 * @brief Checks if two WetMass objects are equal.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs == rhs, false otherwise.
 */
bool operator==(const WetMass& lhs, const WetMass& rhs);

/**
 * @brief Checks if a WetMass is equal to a PreciseDouble.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs == rhs, false otherwise.
 */
bool operator==(const WetMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is equal to a WetMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs == rhs, false otherwise.
 */
bool operator==(const PreciseDouble& lhs, const WetMass& rhs);

/**
 * @brief Checks if two WetMass objects are not equal.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs != rhs, false otherwise.
 */
bool operator!=(const WetMass& lhs, const WetMass& rhs);

/**
 * @brief Checks if a WetMass is not equal to a PreciseDouble.
 * @param lhs Left-hand side WetMass.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs != rhs, false otherwise.
 */
bool operator!=(const WetMass& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is not equal to a WetMass.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side WetMass.
 * @return True if lhs != rhs, false otherwise.
 */
bool operator!=(const PreciseDouble& lhs, const WetMass& rhs);
///@}


/**
 * @class Length
 * @brief Represents length in the ecosystem simulation.
 *
 * Encapsulates a PreciseDouble value for length and provides arithmetic operations,
 * comparisons, and conversion from DryMass using species-specific coefficients.
 */
class Length {
private:
    /// Length value
    PreciseDouble value;

public:
    /// Default constructor, initializes value to 0
    Length();

    /// Constructor from a PreciseDouble
    explicit Length(const PreciseDouble& value);

    /// Constructor from DryMass and species-specific coefficients
    explicit Length(const DryMass& dryMass, const PreciseDouble& coefficientForMassA, const PreciseDouble& scaleForMassB);

    /// Copy constructor
    Length(const Length &other);

    /// Copy assignment
    Length& operator=(const Length& other);

    /// Destructor
    ~Length();

    /// Returns the numeric value of the length
    const PreciseDouble& getValue() const;
    
    /// Stream output operator
    friend std::ostream& operator<<(std::ostream& os, const Length& length);

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


/** @name Arithmetic Operators for Length
 */
///@{
/**
 * @brief Adds two Length objects.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side Length.
 * @return A Length representing the sum of lhs and rhs.
 */
Length operator+(const Length& lhs, const Length& rhs);

/**
 * @brief Adds a Length and a PreciseDouble.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side PreciseDouble.
 * @return A Length representing the sum of lhs and rhs.
 */
Length operator+(const Length& lhs, const PreciseDouble& rhs);

/**
 * @brief Adds a PreciseDouble and a Length.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side Length.
 * @return A Length representing the sum of lhs and rhs.
 */
Length operator+(const PreciseDouble& lhs, const Length& rhs);

/**
 * @brief Subtracts one Length from another.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side Length.
 * @return A Length representing lhs minus rhs.
 */
Length operator-(const Length& lhs, const Length& rhs);

/**
 * @brief Subtracts a PreciseDouble from a Length.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side PreciseDouble.
 * @return A Length representing lhs minus rhs.
 */
Length operator-(const Length& lhs, const PreciseDouble& rhs);

/**
 * @brief Subtracts a Length from a PreciseDouble.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side Length.
 * @return A Length representing lhs minus rhs.
 */
Length operator-(const PreciseDouble& lhs, const Length& rhs);
///@}


/** @name Comparison Operators for Length
 */
///@{
/**
 * @brief Checks if one Length is less than another Length.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side Length.
 * @return True if lhs < rhs, false otherwise.
 */
bool operator<(const Length& lhs, const Length& rhs);

/**
 * @brief Checks if a Length is less than a PreciseDouble.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs < rhs, false otherwise.
 */
bool operator<(const Length& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is less than a Length.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side Length.
 * @return True if lhs < rhs, false otherwise.
 */
bool operator<(const PreciseDouble& lhs, const Length& rhs);

/**
 * @brief Checks if one Length is less than or equal to another Length.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side Length.
 * @return True if lhs <= rhs, false otherwise.
 */
bool operator<=(const Length& lhs, const Length& rhs);

/**
 * @brief Checks if a Length is less than or equal to a PreciseDouble.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs <= rhs, false otherwise.
 */
bool operator<=(const Length& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is less than or equal to a Length.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side Length.
 * @return True if lhs <= rhs, false otherwise.
 */
bool operator<=(const PreciseDouble& lhs, const Length& rhs);

/**
 * @brief Checks if one Length is greater than another Length.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side Length.
 * @return True if lhs > rhs, false otherwise.
 */
bool operator>(const Length& lhs, const Length& rhs);

/**
 * @brief Checks if a Length is greater than a PreciseDouble.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs > rhs, false otherwise.
 */
bool operator>(const Length& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is greater than a Length.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side Length.
 * @return True if lhs > rhs, false otherwise.
 */
bool operator>(const PreciseDouble& lhs, const Length& rhs);

/**
 * @brief Checks if one Length is greater than or equal to another Length.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side Length.
 * @return True if lhs >= rhs, false otherwise.
 */
bool operator>=(const Length& lhs, const Length& rhs);

/**
 * @brief Checks if a Length is greater than or equal to a PreciseDouble.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs >= rhs, false otherwise.
 */
bool operator>=(const Length& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is greater than or equal to a Length.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side Length.
 * @return True if lhs >= rhs, false otherwise.
 */
bool operator>=(const PreciseDouble& lhs, const Length& rhs);

/**
 * @brief Checks if two Length objects are equal.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side Length.
 * @return True if lhs == rhs, false otherwise.
 */
bool operator==(const Length& lhs, const Length& rhs);

/**
 * @brief Checks if a Length is equal to a PreciseDouble.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs == rhs, false otherwise.
 */
bool operator==(const Length& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is equal to a Length.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side Length.
 * @return True if lhs == rhs, false otherwise.
 */
bool operator==(const PreciseDouble& lhs, const Length& rhs);

/**
 * @brief Checks if two Length objects are not equal.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side Length.
 * @return True if lhs != rhs, false otherwise.
 */
bool operator!=(const Length& lhs, const Length& rhs);

/**
 * @brief Checks if a Length is not equal to a PreciseDouble.
 * @param lhs Left-hand side Length.
 * @param rhs Right-hand side PreciseDouble.
 * @return True if lhs != rhs, false otherwise.
 */
bool operator!=(const Length& lhs, const PreciseDouble& rhs);

/**
 * @brief Checks if a PreciseDouble is not equal to a Length.
 * @param lhs Left-hand side PreciseDouble.
 * @param rhs Right-hand side Length.
 * @return True if lhs != rhs, false otherwise.
 */
bool operator!=(const PreciseDouble& lhs, const Length& rhs);
///@}

#endif /* STRUCTURAL_UNITS_H_ */
