/**
 * @file EdibleID.h
 * @brief Represents a unique identifier for edible entities in the simulation.
 */

#ifndef EDIBLE_ID_H_
#define EDIBLE_ID_H_


#include <ostream>
#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "Misc/Types.h"


/**
 * @class EdibleID
 * @brief Encapsulates a unique identifier for an edible object or resource.
 *
 * This class provides type safety for edible IDs and supports conversions to 
 * size_t and string for compatibility with containers and debugging.
 */
class EdibleID
{
public:
	/// Default constructor, creates an invalid EdibleID
	EdibleID();

	/**
     * @brief Constructs an EdibleID with a specific value.
     * @param value The unique identifier value
     */
	explicit EdibleID(const id_type& value);

	/// Virtual destructor
	virtual ~EdibleID();

	/// Copy constructor
	EdibleID(const EdibleID&);

	/// Copy assignment operator
	EdibleID& operator=(const EdibleID&);

	/**
     * @brief Returns the underlying ID value.
     * @return Reference to the ID value
     */
	const id_type& getValue() const;

	/**
     * @brief Conversion operator to size_t.
     * Allows usage in hashed containers like std::unordered_map.
     */
	operator size_t() const;

	/**
     * @brief Conversion operator to std::string.
     * Allows printing and serialization as string.
     */
	explicit operator std::string() const;

	/**
     * @brief Stream output operator for debugging
     * @param os Output stream
     * @param id EdibleID to output
     * @return Reference to the output stream
     */
	friend std::ostream& operator<<(std::ostream& os, const EdibleID& id);

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
	/// The underlying ID value
    id_type value;

    /// Indicates if the ID has been set
    bool set;
};

namespace std {
	/**
     * @brief Hash specialization for EdibleID to allow use in unordered containers
     */
    template<>
    struct hash<EdibleID> {
		/**
         * @brief Computes a hash value for an EdibleID
         * @param id The EdibleID to hash
         * @return Hash value as size_t
         */
        size_t operator()(const EdibleID& id) const;
    };
}

#endif /* EDIBLE_ID_H_ */
