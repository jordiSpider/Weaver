/**
 * @file Generation.h
 * @brief Defines the Generation class, which encapsulates a generation value.
 *
 * The Generation class stores an unsigned integer representing a generation.
 * It provides basic operations, such as retrieving the value, conversion
 * to standard types, and serialization.
 */

#ifndef GENERATION_H_
#define GENERATION_H_


#include <ostream>
#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


/**
 * @class Generation
 * @brief Represents a generation with a numeric value.
 *
 * The Generation class encapsulates an unsigned integer representing a
 * generation. It supports copying, assignment, conversion to size_t and
 * string, output streaming, and serialization.
 */
class Generation
{
public:
	/**
     * @brief Default constructor.
     *
     * Initializes the generation value to 0 and marks it as unset.
     */
	Generation();

	/**
     * @brief Constructor with initial value.
     * @param value The initial generation value.
     *
     * Sets the generation value and marks it as set.
     */
	explicit Generation(const unsigned int& value);

	/**
     * @brief Virtual destructor.
     */
	virtual ~Generation();

	/**
     * @brief Copy constructor.
     * @param other The Generation object to copy from.
     */
	Generation(const Generation& other);

	/**
     * @brief Copy assignment operator.
     * @param other The Generation object to assign from.
     * @return Reference to this Generation object.
     */
	Generation& operator=(const Generation& other);

	/**
     * @brief Get the stored generation value.
     * @return The unsigned integer value of the generation.
     */
	unsigned int getValue() const;

	/**
     * @brief Conversion operator to size_t.
     * @return The generation value as size_t.
     */
	operator size_t() const;

	/**
     * @brief Conversion operator to string.
     * @return The generation value as a string.
     */
	explicit operator std::string() const;

	/**
     * @brief Output stream operator.
     * @param os The output stream.
     * @param gen The Generation object to output.
     * @return Reference to the output stream.
     */
	friend std::ostream& operator<<(std::ostream& os, const Generation& gen);

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
	unsigned int value; /**< The numeric value of the generation */
    bool set;           /**< Flag indicating whether the value has been set */
};

#endif /* GENERATION_H_ */
