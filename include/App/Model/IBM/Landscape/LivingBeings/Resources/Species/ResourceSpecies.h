/**
 * @file ResourceSpecies.h
 * @brief Declaration of the ResourceSpecies class, representing a resource-type species in the ecosystem.
 *
 * This file defines ResourceSpecies, a specialization of Species that models
 * non-mobile biological resources in the ecosystem simulation. It includes
 * their specific identifier, their growth block, and basic properties such as
 * species type and mobility.
 */

#ifndef RESOURCE_SPECIES_H_
#define RESOURCE_SPECIES_H_


#include <cmath>
#include <nlohmann/json.hpp>


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Species/Species.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/CustomIndexedVector.h"
#include "Misc/EnumClass.h"
#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/Growth/ResourceSpeciesGrowth.h"


/**
 * @class ResourceSpecies
 * @brief Represents a species that acts as a resource in the ecosystem.
 *
 * ResourceSpecies describes biological resources available to consumers in the simulation.
 * It stores its specific resource identifier, its growth information, and implements
 * Species interface methods such as mobility, species type, and growth accessors.
 */
class ResourceSpecies: public Species
{
public:
	/**
	 * @class ResourceSpecies::ResourceID
	 * @brief Unique identifier for a resource species.
	 *
	 * This class encapsulates a value of type id_type used to distinguish resource
	 * species from each other. It provides access to the identifier value, support
	 * for hashing, and serialization capabilities.
	 */
	class ResourceID
	{
	public:
		/**
		 * @brief Default constructor.
		 *
		 * Initializes the identifier with a default value.
		 */
		ResourceID();

		/**
		 * @brief Constructs a ResourceID with a specific value.
		 * @param value Identifier value.
		 */
		ResourceID(const id_type& value);

		/**
		 * @brief Returns the underlying identifier value.
		 * @return Constant reference to the identifier value.
		 */
		const id_type& getValue() const;

		/**
		 * @brief Implicit conversion operator to size_t.
		 *
		 * Enables use of ResourceID as a key in hash-based containers.
		 * @return Identifier value converted to size_t.
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
		id_type value; ///< Internal identifier value.
	};


	/**
     * @brief Default constructor.
     */
	ResourceSpecies();

	/**
     * @brief Full constructor for a resource species.
     *
     * @param speciesId Global species identifier (inherited from Species).
     * @param resourceSpeciesId Resource-specific identifier.
     * @param resourceSpeciesInfo JSON object containing species configuration data.
     */
	ResourceSpecies(const ResourceSpecies::ID& speciesId, const ResourceSpecies::ResourceID& resourceSpeciesId, const nlohmann::json &resourceSpeciesInfo);
	
	/**
     * @brief Virtual destructor.
     */
	virtual ~ResourceSpecies();

	/**
     * @brief Returns the resource-specific identifier of the species.
     * @return Constant reference to the ResourceID.
     */
	const ResourceID& getResourceSpeciesId() const;

	/**
     * @brief Retrieves the growth structure associated with this resource species.
     * @return Constant reference to ResourceSpeciesGrowth.
     */
	const ResourceSpeciesGrowth& getGrowthBuildingBlock() const;

	/**
     * @brief Retrieves the mutable growth structure associated with this resource species.
     * @return Mutable reference to ResourceSpeciesGrowth.
     */
	ResourceSpeciesGrowth& getMutableGrowthBuildingBlock();

	/**
     * @brief Returns the species type.
     *
     * For ResourceSpecies, this always returns SpeciesType::Resource.
     *
     * @return SpeciesType::Resource.
     */
	SpeciesType getType() const;

	/**
     * @brief Indicates whether the species is mobile.
     *
     * Resource species are immobile, so this always returns false.
     *
     * @return false.
     */
	virtual bool isMobile() const;

	/**
     * @brief Serializes the object for persistence.
     *
     * @tparam Archive Serialization archive type.
     * @param ar Archive instance.
     * @param version Serialization version.
     */
	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
	/**
     * @brief Internal categorization of resource types.
     *
     * - BasalResource: A base-level resource (e.g., vegetation).
     * - NoBasalResource: A derived or secondary resource.
     */
	enum class Type : unsigned char {
		BasalResource,   ///< Base ecological resource.
        NoBasalResource  ///< Non-basal resource.
	};


	ResourceID resourceSpeciesId;               ///< Resource-specific species identifier.
    ResourceSpeciesGrowth resourceSpeciesGrowth; ///< Growth structure for the resource species.
};


/**
 * @brief Hash specialization for ResourceSpecies::ResourceID.
 *
 * Allows the use of ResourceID in std::unordered_map or std::unordered_set.
 */
template <>
struct std::hash<ResourceSpecies::ResourceID> {
	/**
     * @brief Computes the hash value of a ResourceID.
     * @param resourceID The ResourceID to hash.
     * @return Hash value.
     */
	std::size_t operator()(const ResourceSpecies::ResourceID& resourceID) const;
};

#endif /* RESOURCE_SPECIES_H_ */
