/**
 * @file AnimalSpeciesID.h
 * @brief Defines the AnimalSpeciesID class for uniquely identifying animal species.
 */

#ifndef ANIMAL_SPECIES_ID_H_
#define ANIMAL_SPECIES_ID_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "Misc/Types.h"


/**
 * @class AnimalSpeciesID
 * @brief Represents a unique identifier for an animal species.
 */
class AnimalSpeciesID
{
public:
	/**
     * @brief Default constructor, initializes the ID to a default value.
     */
	AnimalSpeciesID();

	/**
     * @brief Constructor with an initial ID value.
     * @param value The value to assign to the species ID.
     */
	AnimalSpeciesID(const id_type& value);

	/**
     * @brief Get the underlying ID value.
     * @return The stored ID value.
     */
	const id_type& getValue() const;

	/**
     * @brief Conversion operator to size_t.
     * @return The ID value as size_t.
     */
	operator size_t() const;

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
	id_type value; /**< The underlying value of the species ID. */
};

/**
 * @brief Hash specialization for AnimalSpeciesID to allow usage in unordered containers.
 */
template <>
struct std::hash<AnimalSpeciesID> {
	/**
     * @brief Computes a hash value for an AnimalSpeciesID.
     * @param animalSpeciesID The AnimalSpeciesID to hash.
     * @return The computed hash value.
     */
	std::size_t operator()(const AnimalSpeciesID& animalSpeciesID) const;
};

#endif /* ANIMAL_SPECIES_ID_H_ */
