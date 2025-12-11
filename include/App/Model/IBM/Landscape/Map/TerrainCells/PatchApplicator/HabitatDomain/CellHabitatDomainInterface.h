/**
 * @file CellHabitatDomainInterface.h
 * @brief Declares the CellHabitatDomainInterface class, which stores and manages
 *        habitat-domain information for a terrain cell.
 *
 * This interface keeps track of which animal species and instars consider
 * the cell to be part of their habitat domain. Patch sources can dynamically
 * modify this status during landscape generation or simulation updates.
 */

#ifndef CELL_HABITAT_DOMAIN_INTERFACE_H_
#define CELL_HABITAT_DOMAIN_INTERFACE_H_


#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include <fstream>
#include <ostream>
#include <unordered_set>
#include <memory>

#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/CellElement.h"

#include "Misc/CustomIndexedVector.h"
#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/Instar.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/HabitatDomainSource.h"



/**
 * @class CellHabitatDomainInterface
 * @brief Represents the habitat-domain information associated with a terrain cell.
 *
 * This class stores and manages whether each combination of animal species and
 * instar belongs to the habitat domain of the cell. Habitat-domain status affects
 * the behaviour of animals when evaluating a cell, influencing decisions such as
 * occupancy, movement, or suitability assessments.
 *
 * Like all cell-modifying components, it inherits from CellElement and uses patch
 * priorities to control whether incoming patches may update habitat-domain values.
 */
class CellHabitatDomainInterface : public CellElement
{
public:
    /**
     * @brief Default constructor.
     *
     * Initializes an empty habitat-domain structure with default patch priority.
     */
    CellHabitatDomainInterface();

    /**
     * @brief Constructs the habitat-domain interface with a given patch priority.
     *
     * Useful during deserialization or controlled initialization when the element
     * already has a known patch state.
     *
     * @param newHabitatDomainPatchPriority The priority of the patch that last modified the habitat domain.
     */
    CellHabitatDomainInterface(const size_t newHabitatDomainPatchPriority);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~CellHabitatDomainInterface();

    /**
     * @brief Copy constructor (deleted).
     *
     * Preventing copying guarantees safe management of patch priority and avoids
     * duplicating cell-state elements in inconsistent ways.
     */
    CellHabitatDomainInterface(const CellHabitatDomainInterface&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted).
     */
    CellHabitatDomainInterface& operator=(const CellHabitatDomainInterface&) = delete;

    /**
     * @brief Applies a habitat-domain patch to the cell.
     *
     * Updates the habitat-domain values according to the provided source, if the
     * incoming patch's priority is high enough to overwrite the current one.
     *
     * @param source The habitat-domain source containing the new values.
     * @param habitatDomainPatchPriority The priority of the patch being applied.
     */
    void applySource(const HabitatDomainSource &source, const size_t habitatDomainPatchPriority);

    /**
     * @brief Ensures capacity for a newly introduced animal species.
     *
     * Called when new species are added to the ecosystem. Expands internal
     * structures so that habitat-domain status can be stored for all instars of
     * the new species.
     *
     * @param animalSpecies The species to register within the habitat-domain data.
     */
    void addAnimalSpecies(const AnimalSpecies& animalSpecies);

    /**
     * @brief Returns the full habitat-domain status matrix.
     *
     * The structure is indexed first by AnimalSpeciesID, and then by Instar.
     * Each boolean value indicates whether that instar of that species is
     * considered to belong to the habitat domain of this cell.
     *
     * @return A read-only reference to the species–instar habitat-domain mapping.
     */
    const CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, bool>>& getHabitatDomainStatus() const;

    /**
     * @brief Checks whether a given species and instar belongs to the habitat domain.
     *
     * @param id The species identifier.
     * @param instar The instar/stage of the species.
     * @return True if the species–instar combination is marked as habitat domain; false otherwise.
     */
    bool isHabitatDomain(const AnimalSpeciesID& id, const Instar& instar) const;

    /**
     * @brief Serializes the habitat-domain interface.
     *
     * Saves and loads habitat-domain status and patch priority.
     *
     * @tparam Archive A Boost.Serialization archive type.
     * @param ar The archive used for serialization.
     * @param version Serialization version number (unused).
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    /**
     * @brief Matrix storing habitat-domain flags for all species and instars.
     *
     * Structure:
     *   - First index: AnimalSpeciesID
     *   - Second index: Instar
     *   - Value: bool indicating habitat-domain membership
     *
     * This structure grows when new species are added via addAnimalSpecies().
     */
    CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, bool>> habitatDomainStatus;
};

#endif /* CELL_HABITAT_DOMAIN_INTERFACE_H_ */
