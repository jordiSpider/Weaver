/**
 * @file HabitatDomainSource.h
 * @brief Defines the HabitatDomainSource class representing habitat domain information for animal species.
 *
 * This file contains the declaration of the HabitatDomainSource class, which is
 * a specialized PathSource that provides information about the habitat domains
 * of animal species within a landscape.
 */

#ifndef HABITAT_DOMAIN_SOURCE_H
#define HABITAT_DOMAIN_SOURCE_H



#include <nlohmann/json.hpp>

#include <vector>
#include <string>

#include <sstream>


#include "App/Model/IBM/Landscape/Map/Patches/Source/PathSource.h"

#include "Misc/CustomIndexedVector.h"
#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/Instar.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpecies.h"


/**
 * @class HabitatDomainSource
 * @brief Represents habitat domain information for animals in the landscape.
 *
 * HabitatDomainSource extends PathSource and contains data on which habitat cells
 * are suitable for each species and life stage. It can provide a status map and
 * a textual description of the source.
 */
class HabitatDomainSource : public PathSource {
public:
    /**
     * @brief Constructs a HabitatDomainSource object from JSON configuration.
     *
     * @param config JSON configuration describing the habitat domain.
     * @param existingAnimalSpecies Vector of pointers to existing animal species in the landscape.
     */
    HabitatDomainSource(const nlohmann::json &config, const std::vector<AnimalSpecies*>& existingAnimalSpecies);

    /**
     * @brief Destructor.
     */
    ~HabitatDomainSource();

    /**
     * @brief Returns a string describing this habitat domain source.
     *
     * @return Description of the habitat domain source.
     */
    std::string showInfo() const;

    /**
     * @brief Returns the type of this path source.
     *
     * @return PathSource::Type::HabitatDomain
     */
    Type getType() const;

    /**
     * @brief Returns the habitat domain status for each animal species and life stage.
     *
     * @return Reference to a nested CustomIndexedVector mapping species ID and instar to suitability.
     */
    const CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, bool>>& getHabitatDomainStatus() const;

private:
    CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, bool>> habitatDomainStatus; /**< Maps species and instar to habitat suitability */
    std::vector<std::pair<std::string, std::vector<Instar>>> affectedAnimalSpecies; /**< List of species affected by this habitat domain source */
};


#endif // HABITAT_DOMAIN_SOURCE_H

