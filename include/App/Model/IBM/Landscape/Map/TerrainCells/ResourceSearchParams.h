/**
 * @file ResourceSearchParams.h
 * @brief Defines the ResourceSearchParams class used to filter resources in terrain cells.
 */

#ifndef RESOURCE_SEARCH_PARAMS_H_
#define RESOURCE_SEARCH_PARAMS_H_


#include <vector>


#include <fstream>
#include <ostream>

#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResourceInterface.h"


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
    std::vector<bool> validSignaturesBitmask;

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
    void addSearchParams(size_t numberExistingResourceSpecies, const std::vector<ResourceSpecies::ResourceID> &searchableResourceSpecies = {});

    void init();

    bool matches(const CellResourceInterface& resource) const noexcept;
};

#endif /* RESOURCE_SEARCH_PARAMS_H_ */
