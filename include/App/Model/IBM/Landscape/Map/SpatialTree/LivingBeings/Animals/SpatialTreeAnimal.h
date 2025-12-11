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
     * @brief Searches for a target cell to travel to within a given scope area.
     * @param scopeArea Maximum area the animal can travel.
     * @param animalsHasTriedToPredate List of animals already attempted for predation.
     */
    void searchTargetToTravelTo(const PreciseDouble &scopeArea, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate);
    
    /**
     * @brief Searches for a target cell to travel to, with an output flag indicating
     *        if no destinations are available.
     * @param scopeArea Maximum area the animal can travel.
     * @param animalsHasTriedToPredate List of animals already attempted for predation.
     * @param withoutDestinations Flag set to true if no valid destinations are found.
     */
    void searchTargetToTravelTo(const PreciseDouble &scopeArea, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate, bool& withoutDestinations);
    
    /**
     * @brief Moves the animal one step in the landscape.
     * @param landscape Pointer to the landscape.
     * @param saveActivity Whether to save activity logs.
     * @param activityContent Stream to store activity data.
     * @param saveMovements Whether to save movement logs.
     * @param movementsContent Stream to store movement data.
     * @param actualTimeStep Current time step.
     * @param timeStepsPerDay Number of time steps per day.
     * @param edibilitiesContent Stream to store edible-related logs.
     * @param saveAnimalsEachDayPredationProbabilities Whether to save predation probability logs.
     * @param predationProbabilitiesContent Stream to store predation probabilities.
     * @param competitionAmongResourceSpecies Whether to consider competition among resources.
     */
    void moveOneStep(Landscape* const landscape, const bool saveActivity, std::ostringstream& activityContent, const bool saveMovements, std::ostringstream& movementsContent, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay, std::ostringstream& edibilitiesContent, const bool saveAnimalsEachDayPredationProbabilities, std::ostringstream& predationProbabilitiesContent, const bool competitionAmongResourceSpecies);
    
    /**
     * @brief Creates an offspring from two parent gametes.
     * @param firstParentGamete First parent's gamete.
     * @param secondParentGamete Second parent's gamete.
     * @param parentTerrainCell Terrain cell where the offspring is born.
     * @param factorEggMassFromMom Factor of egg mass contributed by the mother.
     * @param g_numb_prt_female Generation number of the female parent.
     * @param g_numb_prt_male Generation number of the male parent.
     * @param ID_prt_female EdibleID of the female parent.
     * @param ID_prt_male EdibleID of the male parent.
     * @param parentSpecies Pointer to the parent species.
     * @param genderValue Gender of the offspring.
     * @param actualTimeStep Current time step.
     * @param timeStepsPerDay Number of time steps per day.
     * @return Pointer to the newly created offspring.
     */
    AnimalNonStatistical* createOffspring(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& factorEggMassFromMom, const Generation& g_numb_prt_female,
		const Generation& g_numb_prt_male, EdibleID ID_prt_female, EdibleID ID_prt_male, AnimalSpecies* const parentSpecies, Gender genderValue, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);

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
	
    /**
     * @brief Constructor for an animal from two parent gametes.
     * @param firstParentGamete First parent's gamete.
     * @param secondParentGamete Second parent's gamete.
     * @param parentTerrainCell Terrain cell where the animal is born.
     * @param eggMassAtBirth Egg mass at birth.
     * @param g_numb_prt_female Generation number of the female parent.
     * @param g_numb_prt_male Generation number of the male parent.
     * @param ID_prt_female EdibleID of the female parent.
     * @param ID_prt_male EdibleID of the male parent.
     * @param mySpecies Pointer to the species.
     * @param genderValue Gender of the offspring.
     * @param actualTimeStep Current time step.
     * @param timeStepsPerDay Number of time steps per day.
     */
    SpatialTreeAnimal(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& eggMassAtBirth, const Generation& g_numb_prt_female,
			const Generation& g_numb_prt_male, EdibleID ID_prt_female, EdibleID ID_prt_male, AnimalSpecies* const mySpecies, Gender genderValue, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);
    
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
