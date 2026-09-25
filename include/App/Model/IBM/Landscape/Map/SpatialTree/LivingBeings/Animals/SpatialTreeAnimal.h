/**
 * @file SpatialTreeAnimal.h
 * @brief Defines the SpatialTreeAnimal class for animals in a spatial tree terrain.
 */

#ifndef SPATIAL_TREE_ANIMAL_H_
#define SPATIAL_TREE_ANIMAL_H_


#include <memory>

#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/Points/PointSpatialTree.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/CellValue.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Gender.h"


/**
 * @class SpatialTreeAnimal
 * @brief Represents a non-statistical animal located in a spatial tree terrain.
 *
 * Extends AnimalNonStatistical to provide movement, target search, and reproduction
 * functionality specifically adapted to spatial tree structured landscapes.
 */
class SpatialTreeAnimal : public AnimalNonStatistical
{
protected:
    /**
     * @brief Searches for a target cell to travel to, with an output flag indicating
     *        if no destinations are available.
     * @param scopeArea Maximum area the animal can travel.
     */
    bool searchTargetToTravelTo(const PreciseDouble &scopeArea, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchEdibilityValueGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchPredationRiskGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchConspecificBiomassGlobal);
    
    void move(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, 
        const bool saveMovements, fmt::memory_buffer& movementsText, const bool saveActivity,
        fmt::memory_buffer& activitiesText) override;
    
    /**
     * @brief Creates an offspring from two parent gametes.
     * @param firstParentGamete First parent's gamete.
     * @param secondParentGamete Second parent's gamete.
     * @param parentTerrainCell Terrain cell where the offspring is born.
     * @param factorEggMassFromMom Factor of egg mass contributed by the mother.
     * @param g_numb_prt_female Generation number of the female parent.
     * @param g_numb_prt_male Generation number of the male parent.
     * @param ID_prt_female id_type of the female parent.
     * @param ID_prt_male id_type of the male parent.
     * @param parentSpecies Pointer to the parent species.
     * @param genderValue Gender of the offspring.
     * @param actualTimeStep Current time step.
     * @param timeStepsPerDay Number of time steps per day.
     * @return Pointer to the newly created offspring.
     */
    AnimalNonStatistical* createOffspring(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& factorEggMassFromMom, const Generation& g_numb_prt_female,
		const Generation& g_numb_prt_male, id_type ID_prt_female, id_type ID_prt_male, AnimalSpecies* const parentSpecies, Gender genderValue, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);

public:
    /// Default constructor
    SpatialTreeAnimal();

    /**
     * @brief Constructor for an animal at a given instar stage.
     * @param instar Current instar of the animal.
     * @param mySpecies Pointer to the species.
     * @param terrainCell Pointer to the terrain cell where the animal is located.
     * @param actualTimeStep Current time step.
     * @param timeStepsPerDay Number of time steps per day.
     */
    SpatialTreeAnimal(const Instar &instar, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);
	
    SpatialTreeAnimal(const Instar &instar, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, const Genome* const genome, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Constructor for an animal from two parent gametes.
     * @param firstParentGamete First parent's gamete.
     * @param secondParentGamete Second parent's gamete.
     * @param parentTerrainCell Terrain cell where the animal is born.
     * @param eggMassAtBirth Egg mass at birth.
     * @param g_numb_prt_female Generation number of the female parent.
     * @param g_numb_prt_male Generation number of the male parent.
     * @param ID_prt_female id_type of the female parent.
     * @param ID_prt_male id_type of the male parent.
     * @param mySpecies Pointer to the species.
     * @param genderValue Gender of the offspring.
     * @param actualTimeStep Current time step.
     * @param timeStepsPerDay Number of time steps per day.
     */
    SpatialTreeAnimal(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& eggMassAtBirth, const Generation& g_numb_prt_female,
			const Generation& g_numb_prt_male, id_type ID_prt_female, id_type ID_prt_male, AnimalSpecies* const mySpecies, Gender genderValue, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);
    
    /// Destructor
    virtual ~SpatialTreeAnimal();

    /// Deleted copy constructor
    SpatialTreeAnimal(const SpatialTreeAnimal&) = delete;

    /// Deleted copy assignment
    SpatialTreeAnimal& operator=(const SpatialTreeAnimal&) = delete;

    /**
     * @brief Serialization function.
     * @tparam Archive Type of archive.
     * @param ar Archive object.
     * @param version Serialization version.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* SPATIAL_TREE_ANIMAL_H_ */
