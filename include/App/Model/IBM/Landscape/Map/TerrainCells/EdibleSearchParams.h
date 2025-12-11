/**
 * @file EdibleSearchParams.h
 * @brief Defines the EdibleSearchParams class used to filter animals and resources in terrain cells.
 */

#ifndef EDIBLE_SEARCH_PARAMS_H_
#define EDIBLE_SEARCH_PARAMS_H_


#include <boost/serialization/export.hpp>



#include <fstream>
#include <ostream>

#include "App/Model/IBM/Landscape/Map/TerrainCells/AnimalSearchParams.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/ResourceSearchParams.h"
#include "Misc/EnumClass.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Gender.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "Misc/CustomIndexedVector.h"


/**
 * @class EdibleSearchParams
 * @brief Encapsulates search parameters for both animals and resources.
 *
 * This class combines animal search parameters and resource search parameters
 * to facilitate the filtering of edibles (animals and resources) within a
 * terrain cell during simulation.
 */
class EdibleSearchParams
{
protected:
    /// Search parameters for animals
    AnimalSearchParams animalSearchParams;

    /// Search parameters for resources
    ResourceSearchParams resourceSearchParams;

public:
    /**
     * @brief Default constructor, initializes empty search parameters.
     */
    EdibleSearchParams();

    /**
     * @brief Destructor.
     */
    virtual ~EdibleSearchParams();

    /**
     * @brief Copy constructor.
     * 
     * @param other Another EdibleSearchParams object to copy from
     */
    EdibleSearchParams(const EdibleSearchParams& other);
    
    /**
     * @brief Copy assignment operator.
     * 
     * @param other Another EdibleSearchParams object to assign from
     * @return Reference to this object
     */
    EdibleSearchParams& operator=(const EdibleSearchParams& other);

    /**
     * @brief Adds animal search parameters to this object.
     * 
     * @param searchableLifeStages Vector of life stages to include
     * @param searchableAnimalSpecies Vector of animal species IDs to include
     * @param searchableInstars Vector of instars to include
     * @param searchableGenders Vector of genders to include
     */
    void addAnimalSearchParams(
        const std::vector<LifeStage> &searchableLifeStages,
        const std::vector<AnimalSpeciesID> &searchableAnimalSpecies,
        const std::vector<Instar> &searchableInstars,
        const std::vector<Gender> &searchableGenders
    );

    /**
     * @brief Adds resource search parameters to this object.
     * 
     * @param existingResourceSpecies Vector of existing resource species
     * @param searchableResourceSpecies Optional vector of resource IDs to include
     */
    void addResourceSearchParams(const std::vector<ResourceSpecies*>& existingResourceSpecies, const std::vector<ResourceSpecies::ResourceID> &searchableResourceSpecies = {});

    /**
     * @brief Returns the animal search parameters.
     * 
     * @return Reference to AnimalSearchParams object
     */
    const AnimalSearchParams& getAnimalSearchParams() const;
    
    /**
     * @brief Returns the resource search parameters.
     * 
     * @return Reference to ResourceSearchParams object
     */
    const ResourceSearchParams& getResourceSearchParams() const;

    /**
     * @brief Clears all search parameters (animals and resources).
     */
    void clear();

    /**
     * @brief Serialization function for EdibleSearchParams.
     * 
     * @tparam Archive Type of archive
     * @param ar Archive object
     * @param version Version of the serialization
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* EDIBLE_SEARCH_PARAMS_H_ */
