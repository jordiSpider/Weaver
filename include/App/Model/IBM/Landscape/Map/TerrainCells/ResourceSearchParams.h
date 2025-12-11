/**
 * @file ResourceSearchParams.h
 * @brief Defines the ResourceSearchParams class used to filter resources in terrain cells.
 */

#ifndef RESOURCE_SEARCH_PARAMS_H_
#define RESOURCE_SEARCH_PARAMS_H_


#include <vector>
#include <unordered_set>


#include <boost/serialization/export.hpp>
#include <boost/serialization/unordered_set.hpp>


#include <fstream>
#include <ostream>

#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/ResourceSpecies.h"


/**
 * @typedef SearchableResourceSpecies
 * @brief Set of resource IDs that can be searched.
 */
typedef std::unordered_set<ResourceSpecies::ResourceID> SearchableResourceSpecies;


/**
 * @class ResourceSearchParams
 * @brief Stores parameters for searching resources within a terrain cell.
 *
 * This class encapsulates a set of searchable resource species. It allows
 * filtering resources based on their species during simulation operations.
 */
class ResourceSearchParams
{
protected:
    /// Set of resource species IDs that are searchable
    SearchableResourceSpecies searchParams;

public:
    /**
     * @brief Default constructor, initializes empty search parameters.
     */
    ResourceSearchParams();

    /**
     * @brief Destructor.
     */
    virtual ~ResourceSearchParams();

    /**
     * @brief Copy constructor.
     * 
     * @param other Another ResourceSearchParams object to copy from
     */
    ResourceSearchParams(const ResourceSearchParams& other);

    /**
     * @brief Copy assignment operator.
     * 
     * @param other Another ResourceSearchParams object to assign from
     * @return Reference to this object
     */
    ResourceSearchParams& operator=(const ResourceSearchParams& other);

    /**
     * @brief Adds new searchable resource species to the current object.
     * 
     * @param existingResourceSpecies Vector of existing ResourceSpecies objects
     * @param searchableResourceSpecies Optional vector of resource IDs to add explicitly
     */
    void addSearchParams(const std::vector<ResourceSpecies*>& existingResourceSpecies, const std::vector<ResourceSpecies::ResourceID> &searchableResourceSpecies = {});

    /**
     * @brief Returns the set of searchable resource species IDs.
     * 
     * @return Reference to the set of searchable resource species
     */
    const SearchableResourceSpecies& getSearchParams() const;

    /**
     * @brief Clears all stored searchable parameters.
     */
    void clear();

    /**
     * @brief Serialization function for ResourceSearchParams.
     * 
     * @tparam Archive Type of archive
     * @param ar Archive object
     * @param version Version of the serialization
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* RESOURCE_SEARCH_PARAMS_H_ */
