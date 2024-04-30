/*
 * Allele.h
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#ifndef ALLELE_H_
#define ALLELE_H_

#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>
#include <ostream>


/**
 * @class Allele
 * @brief Represents a genetic allele with a unique identifier, a numerical value, and an alphabetic order.
 */
class Allele
{
private:
	static unsigned int alleleId; /**< Unique identificator for each allele in the genetic pool */

	unsigned int id; /**< Id used for computational tasks */
    double value; /**< Value for the allele */
    unsigned int alphabeticOrder; /**< Alphabetic order used for ordering tasks */

public:
    explicit Allele();

    /**
     * @brief Constructor for the Allele class.
     * @param value The numerical value of the allele.
     * @param alphabeticOrder The alphabetic order of the allele.
     */
    explicit Allele(const double &value, const unsigned int alphabeticOrder);
	
	/**
     * @brief Destructor for the Allele class.
     */
	~Allele();

	// Getters
    /**
     * @brief Get the unique identifier of the allele.
     * @return The unique identifier.
     */
	inline const unsigned int getId() const { return id; }
	
	/**
     * @brief Get the numerical value of the allele.
     * @return The numerical value.
     */
	inline const double& getValue() const { return value; }
	
	/**
     * @brief Get the alphabetic order of the allele.
     * @return The alphabetic order.
     */
	inline const unsigned int getAlphabeticOrder() const { return alphabeticOrder; }
	
	/**
     * @brief Overloaded output stream operator for easy printing of Allele objects.
     * @param os The output stream.
     * @param allele The Allele object to be printed.
     * @return The modified output stream.
     */
	friend std::ostream& operator<<(std::ostream& os, const Allele& chromosome);

	/**
     * @brief Serialize the object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, Allele* &allelePtr, const unsigned int version);
    }
}


#endif /* ALLELE_H_ */
