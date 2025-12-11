/**
 * @file Species.h
 * @brief Defines the base class for all species in the simulation, including animals and resources.
 */

#ifndef SPECIES_H_
#define SPECIES_H_

#include <string>
#include <cmath>


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>



#include "Misc/string/trim.h"
#include "Misc/Types.h"
#include "Misc/CustomIndexedVector.h"
#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/SpeciesGrowth.h"
#include "App/Model/IBM/Landscape/LivingBeings/Species/SpeciesType.h"
#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"



/**
 * @class Species
 * @brief Represents a generic species in the ecosystem.
 *
 * The Species class encapsulates common properties of all species, including a unique identifier,
 * a scientific name, growth information, and extinction status. It serves as a base class for more
 * specialized species types such as animals and resources.
 */
class Species
{
public:
	/**
     * @class Species::ID
     * @brief Unique identifier for a Species.
     *
     * Provides type-safe identification for species instances within the simulation.
     */
	class ID
	{
	public:
		/// Default constructor, creates an empty ID.
		ID();

		/// Constructs an ID with a specific value.
        /// @param value The identifier value.
		ID(const id_type& value);

		/// Returns the underlying ID value.
        /// @return The stored id_type.
		const id_type& getValue() const;

		/// Converts the ID to a size_t for hashing or indexing.
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
		id_type value; ///< The underlying identifier value.
	};

	/// Default constructor.
	Species();

	/// Constructs a Species with a given ID and scientific name.
    /// @param speciesId The unique species identifier.
    /// @param scientificName The scientific name of the species.
	Species(const Species::ID& speciesId, const std::string& scientificName);

	/// Virtual destructor for proper cleanup of derived classes.
	virtual ~Species();

	/// Returns the unique ID of the species.
    /// @return The Species::ID.
	const ID& getId() const;

	/// Returns the scientific name of the species.
    /// @return Scientific name string.
	const std::string& getScientificName() const;

	/// Returns the scientific name with replacements (e.g., for display or formatting).
    /// @return Formatted scientific name string.
	std::string getScientificNameReplaced() const;

	/// Returns a read-only reference to the species' growth building block.
	virtual const SpeciesGrowth& getGrowthBuildingBlock() const=0;

	/// Returns a mutable reference to the species' growth building block.
	virtual SpeciesGrowth& getMutableGrowthBuildingBlock()=0;

	/// Returns whether the species is extinct.
    /// @return True if extinct, false otherwise.
	virtual bool isExtinguished() const { return extinguished; };

	/// Sets the extinction status of the species.
    /// @param extinguishedValue True if the species is now extinct.
	virtual void setExtinguished(bool extinguishedValue) { extinguished = extinguishedValue; };

	/// Comparison operator based on species ID.
	bool operator<(const Species& other) const;

	/// Equality operator based on species ID.
	bool operator==(const Species& other) const;

	/// Returns whether the species is mobile (e.g., animals vs. plants).
	virtual bool isMobile() const=0;

	/// Returns the type of species (Resource or Animal).
	virtual SpeciesType getType() const=0;

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
	ID id;                     	///< Unique identifier of the species.
    std::string scientificName; ///< Scientific name of the species.
    bool extinguished;         	///< True if the species is extinct.
};

/**
 * @brief Specialization of std::hash for Species::ID.
 *
 * This allows Species::ID objects to be used as keys in unordered containers
 * such as `std::unordered_map` and `std::unordered_set`.
 */
template <>
struct std::hash<Species::ID> {
	/**
     * @brief Computes a hash value for a Species::ID.
     * @param id The Species::ID object to hash.
     * @return A size_t hash value corresponding to the given Species::ID.
     */
	std::size_t operator()(const Species::ID& id) const;
};

#endif /* SPECIES_H_ */
