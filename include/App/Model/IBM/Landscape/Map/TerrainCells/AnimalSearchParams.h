/**
 * @file AnimalSearchParams.h
 * @brief Defines the AnimalSearchParams class used to filter animals in terrain cells.
 */

#ifndef ANIMAL_SEARCH_PARAMS_H_
#define ANIMAL_SEARCH_PARAMS_H_


#include <vector>
#include <algorithm>


#include <fstream>
#include <ostream>

#include "Misc/Types.h"
#include "Misc/CustomIndexedVector.h"
#include "Misc/EnumClass.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Gender.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"
#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/Instar.h"


class Animal;


/**
 * @class AnimalSearchParams
 * @brief Stores parameters for searching animals within a terrain cell.
 *
 * This class encapsulates life stages, species, instars, and genders
 * that are considered searchable. It allows filtering animals according
 * to these attributes during simulations.
 */
class AnimalSearchParams
{
protected:
    std::vector<bool> validSignaturesBitmask;

public:
    /**
     * @brief Default constructor, initializes empty search parameters.
     */
    AnimalSearchParams();
    
    /**
     * @brief Constructor with explicit searchable parameters.
     * 
     * @param newSearchableLifeStages Life stages to include
     * @param newSearchableAnimalSpecies Species IDs to include
     * @param newSearchableInstars Instars to include
     * @param newSearchableGenders Genders to include
     */
    AnimalSearchParams(
        const std::vector<LifeStage> &newSearchableLifeStages,
        const std::vector<AnimalSpeciesID> &newSearchableAnimalSpecies,
        const std::vector<Instar> &newSearchableInstars,
        const std::vector<Gender> &newSearchableGenders
    );

    /**
     * @brief Destructor.
     */
    virtual ~AnimalSearchParams();

    /**
     * @brief Copy constructor.
     * 
     * @param other Another AnimalSearchParams object to copy from
     */
    AnimalSearchParams(const AnimalSearchParams& other);

    /**
     * @brief Copy assignment operator.
     * 
     * @param other Another AnimalSearchParams object to assign from
     * @return Reference to this object
     */
    AnimalSearchParams& operator=(const AnimalSearchParams& other);

    /**
     * @brief Adds new searchable parameters to the current object.
     * 
     * @param newSearchableLifeStages Life stages to add
     * @param newSearchableAnimalSpecies Species IDs to add
     * @param newSearchableInstars Instars to add
     * @param newSearchableGenders Genders to add
     */
    void addSearchParams(
        const std::vector<LifeStage> &newSearchableLifeStages,
        const std::vector<AnimalSpeciesID> &newSearchableAnimalSpecies,
        const std::vector<Instar> &newSearchableInstars,
        const std::vector<Gender> &newSearchableGenders
    );

    void init();

    bool matches(const Animal& animal) const noexcept;
};

#endif /* ANIMAL_SEARCH_PARAMS_H_ */
