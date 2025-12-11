/**
 * @file AnimalSearchParams.h
 * @brief Defines the AnimalSearchParams class used to filter animals in terrain cells.
 */

#ifndef ANIMAL_SEARCH_PARAMS_H_
#define ANIMAL_SEARCH_PARAMS_H_


#include <vector>
#include <algorithm>


#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>


#include <fstream>
#include <ostream>

#include "Misc/Types.h"
#include "Misc/CustomIndexedVector.h"
#include "Misc/EnumClass.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Gender.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"
#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/Instar.h"


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
    /// Vector of life stages that can be searched
    std::vector<LifeStage> searchableLifeStages;

    /// Indexed vector mapping life stage to searchable animal species IDs
    CustomIndexedVector<LifeStage, std::vector<AnimalSpeciesID>> searchableAnimalSpecies;

    /// Indexed vector mapping life stage to searchable instars per species
    CustomIndexedVector<LifeStage, std::vector<std::vector<Instar>>> searchableInstars;

    /// Indexed vector mapping life stage to searchable genders per species and instar
    CustomIndexedVector<LifeStage, std::vector<CustomIndexedVector<Instar, std::vector<Gender>>>> searchableGenders;

    /**
     * @brief Helper function to insert an element into a vector if it is not already present.
     * 
     * @tparam T Type of element
     * @param uniqueVector Vector to insert into
     * @param elementToInsert Element to insert
     */
    template<typename T>
    void insertElement(std::vector<T>& uniqueVector, const T& elementToInsert);

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

    /**
     * @brief Returns the list of searchable life stages.
     * 
     * @return Vector of searchable life stages
     */
    const std::vector<LifeStage>& getSearchableLifeStages() const;

    /**
     * @brief Returns the list of searchable species IDs for a given life stage.
     * 
     * @param lifeStage Life stage to query
     * @return Vector of searchable species IDs
     */
    const std::vector<AnimalSpeciesID>& getSearchableAnimalSpecies(
        const LifeStage &lifeStage
    ) const;

    /**
     * @brief Returns the list of searchable instars for a given life stage and species.
     * 
     * @param lifeStage Life stage to query
     * @param animalSpeciesId Species ID to query
     * @return Vector of searchable instars
     */
    const std::vector<Instar>& getSearchableInstars(
        const LifeStage &lifeStage,
        const AnimalSpeciesID &animalSpeciesId
    ) const;

    /**
     * @brief Returns the list of searchable genders for a given life stage, species, and instar.
     * 
     * @param lifeStage Life stage to query
     * @param animalSpeciesId Species ID to query
     * @param instar Instar to query
     * @return Vector of searchable genders
     */
    const std::vector<Gender>& getSearchableGenders(
        const LifeStage &lifeStage,
        const AnimalSpeciesID &animalSpeciesId, const Instar &instar
    ) const;

    /**
     * @brief Clears all stored searchable parameters.
     */
    void clear();

    /**
     * @brief Serialization function for AnimalSearchParams.
     * 
     * @tparam Archive Type of archive
     * @param ar Archive object
     * @param version Version of the serialization
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* ANIMAL_SEARCH_PARAMS_H_ */
