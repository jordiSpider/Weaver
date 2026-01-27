/**
 * @file SpatialTreeTerrainCell.h
 * @brief Spatial tree implementation of a terrain cell.
 *
 * This class represents a cell in a spatial tree used for efficient 
 * management of animals and resources in the landscape. It extends TerrainCell
 * and provides functionality for hierarchical cell structures, migrations,
 * and localized operations.
 */

#ifndef SPATIAL_TREE_TERRAINCELL_H_
#define SPATIAL_TREE_TERRAINCELL_H_


#include <vector>
#include <cmath>


#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/Points/PointSpatialTree.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/SpatialTreePatchApplicator.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Moisture/CellMoisture.h"



class BranchTerrainCell;


/**
 * @class SpatialTreeTerrainCell
 * @brief Represents a terrain cell in a spatial tree structure.
 *
 * Each spatial tree cell may have a parent branch and contains methods 
 * to manage animals, resources, and hierarchical traversal.
 */
class SpatialTreeTerrainCell : public TerrainCell
{
protected:
    /// Pointer to the parent branch terrain cell.
    BranchTerrainCell* parentTerrainCell;

    /**
     * @brief Create a new animal in the cell.
     *
     * @param landscape Pointer to the landscape.
     * @param instar Life stage of the animal.
     * @param animalSpecies Pointer to the species of the animal.
     * @param saveGenetics Whether to save genetics data.
     * @param saveMassInfo Whether to save mass information.
     * @param actualTimeStep Current timestep.
     * @param timeStepsPerDay Number of timesteps per day.
     * @return Created animal.
     */
    AnimalNonStatistical* createAnimal(Landscape* const landscape, const Instar &instar, AnimalSpecies* animalSpecies, const Genome* const genome, const bool saveGenetics, const bool saveMassInfo, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);

public:
    /// Number of children per spatial tree cell (static constant).
    static const unsigned int numberOfChildren;

    /// Default constructor.
    SpatialTreeTerrainCell();

    /**
     * @brief Constructor with parent, position, and size.
     *
     * @param parentTerrainCell Pointer to the parent branch terrain cell.
     * @param position Pointer to the spatial position.
     * @param size Size of the cell.
     */
    SpatialTreeTerrainCell(BranchTerrainCell* const parentTerrainCell, PointSpatialTree* const position, const PreciseDouble &size);
    
    /// Virtual destructor.
    virtual ~SpatialTreeTerrainCell();

    /**
     * @brief Set the parent branch of this cell.
     *
     * @param newParentTerrainCell Pointer to the new parent cell.
     */
    void setParent(BranchTerrainCell* newParentTerrainCell);

    /// Get the parent branch (const).
    virtual const BranchTerrainCell* getParent() const;

    /// Get the parent branch (mutable).
    virtual BranchTerrainCell* getMutableParent() const;

    /**
     * @brief Get a cell based on a bearing from a position.
     *
     * @param map Pointer to the map.
     * @param targetNeighborToTravelTo Pair of map and continuous position.
     * @param animalPosition Position of the animal.
     * @param remainingSearchArea Remaining search area.
     * @return Pair indicating if a cell was found and its pointer and center.
     */
    std::pair<bool, std::pair<TerrainCell*, PointContinuous>> getCellByBearing(
        Map* const map, const std::pair<PointMap, PointContinuous> &targetNeighborToTravelTo, const PointContinuous &animalPosition, const PreciseDouble& remainingSearchArea
    );

    /**
     * @brief Migrate an animal from this cell to a new terrain cell.
     *
     * @param landscape Pointer to the landscape.
     * @param animalToMigrate Pointer to the animal to migrate.
     * @param newTerrainCell Pointer to the destination cell.
     * @param newPosition New position in the destination cell.
     */
    void migrateAnimalTo(Landscape* const landscape, Animal* animalToMigrate, TerrainCell* newTerrainCell, const PointContinuous &newPosition);

    /// Get the spatial position (const).
    const PointSpatialTree& getPosition() const override;

    /// Get the spatial position (mutable).
    PointSpatialTree& getMutablePosition() const override;

    /// Get mutable patch applicator for this cell.
    virtual SpatialTreePatchApplicator& getMutablePatchApplicator() override;

    /// Get patch applicator (const).
    virtual const SpatialTreePatchApplicator& getPatchApplicator() const override;

    /**
     * @brief Get a child cell by map position.
     *
     * @param dstPos Destination map position.
     * @return Pointer to the terrain cell at that position.
     */
    TerrainCell* getCell(const PointMap &dstPos);

    /**
     * @brief Get a child cell by spatial position (pure virtual).
     *
     * @param dstPos Destination spatial position.
     * @return Pointer to the spatial tree terrain cell at that position.
     */
    virtual SpatialTreeTerrainCell* getCell(const PointSpatialTree &dstPos)=0;

    /**
     * @brief Check if a position corresponds to a child of this cell.
     *
     * @param childPos Spatial position to check.
     * @return True if the position is a child, false otherwise.
     */
    virtual bool isChild(const PointSpatialTree &childPos) const=0;

    /// Check if this cell is a leaf (pure virtual).
    virtual bool isLeaf() const=0;

    /**
     * @brief Retrieve terrain cells within a radius for evaluation.
     *
     * @param bestEvaluations Vector to store evaluated cell values.
     * @param sourcePosition Position from which to measure the radius.
     * @param radius Radius of search.
     * @param radiusArea Optional ring model area.
     * @param searchDepth Depth of search in the tree.
     * @param searchNeighborsWithFemales Whether to prioritize neighbors with females.
     * @param parentFullCoverage Parent coverage flag.
     * @param animalWhoIsEvaluating Pointer to the animal performing evaluation.
     * @param animalsHasTriedToPredate List of animals already considered.
     */
    virtual void getRadiusTerrainCells(
        std::vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const size_t searchDepth, const bool searchNeighborsWithFemales, const bool parentFullCoverage, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate
    )=0;

    /**
     * @brief Retrieve neighbor cells within a radius.
     *
     * @param bestEvaluations Vector to store evaluated cell values.
     * @param sourcePosition Position from which to measure the radius.
     * @param radius Radius of search.
     * @param radiusArea Optional ring model area.
     * @param searchDepth Depth of search in the tree.
     * @param searchNeighborsWithFemales Whether to prioritize neighbors with females.
     * @param animalWhoIsEvaluating Pointer to the animal performing evaluation.
     * @param animalsHasTriedToPredate List of animals already considered.
     */
    virtual void getNeighboursCellsOnRadius(
        std::vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const size_t searchDepth, const bool searchNeighborsWithFemales, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate
    );

    /// Overloaded neighbor search using default radiusArea.
    void getNeighboursCellsOnRadius(
        std::vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const size_t searchDepth, const bool searchNeighborsWithFemales, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate
    );

    /** @name Downward traversal functions (pure virtual)
     */
    ///@{
    /**
     * @brief Applies a set of functions to all animals that match given search parameters.
     *
     * This is a pure virtual method. Derived classes must implement how the
     * functions are applied to animals within the object or space.
     *
     * @param animalFunctions A vector of pairs containing search parameters and
     * corresponding functions to apply to matching animals.
     */
    virtual void applyDownFunctionToAnimals(const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions)=0;
    
    /**
     * @brief Applies a set of functions to animals filtered by a custom checker within a radius.
     *
     * This is a pure virtual method. Derived classes must implement the logic
     * to apply functions to animals that satisfy the `checker` predicate and are
     * within a circular area defined by `sourcePosition` and `radius`.
     *
     * @param checker A function that returns true for animals to be affected.
     * @param sourcePosition The center position for the search area.
     * @param radius The radius of the search area.
     * @param animalFunctions A vector of pairs containing search parameters and
     * corresponding functions to apply to the filtered animals.
     */
    virtual void applyDownFunctionToAnimals(
        std::function<bool(Animal&)> checker, const PointContinuous &sourcePosition, const PreciseDouble &radius,
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    )=0;

    /**
     * @brief Applies functions randomly to animals that match the given search parameters.
     *
     * This is a pure virtual method. Derived classes must implement how to randomly
     * select animals and apply the specified functions to them.
     *
     * @param animalFunctions A vector of pairs containing search parameters and
     * corresponding functions to apply to randomly selected animals.
     */
    virtual void randomApplyDownFunctionToAnimals(const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions)=0;
    ///@}

    /** @name Upward traversal functions
     */
    ///@{
    /**
     * @brief Applies a set of functions to all animals that match the given search parameters.
     *
     * Overrides the base implementation. Functions are applied deterministically
     * to all animals that satisfy the search criteria.
     *
     * @param animalFunctions A vector of pairs containing search parameters and
     * the corresponding functions to apply.
     */
    void applyFunctionToAnimals(const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions) override;
    
    /**
     * @brief Randomly applies a set of functions to animals matching the search parameters.
     *
     * Overrides the base implementation. Functions are applied to a randomly
     * selected subset of animals that satisfy the search criteria.
     *
     * @param animalFunctions A vector of pairs containing search parameters and
     * the corresponding functions to apply.
     */
    void randomApplyFunctionToAnimals(const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions) override;
    
    /**
     * @brief Applies functions to animals that pass a custom checker predicate.
     *
     * This method allows upward propagation of effects in hierarchical structures,
     * applying functions only to animals for which `checker` returns true.
     *
     * @param checker A function that returns true for animals to be affected.
     * @param animalFunctions A vector of pairs containing search parameters and
     * the corresponding functions to apply.
     */
    virtual void applyUpFunctionToAnimals(
        std::function<bool(Animal&)> checker,
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    );

    /**
     * @brief Randomly applies functions to animals that pass a custom checker predicate.
     *
     * This method allows upward propagation of effects in hierarchical structures,
     * applying functions randomly to animals for which `checker` returns true.
     *
     * @param checker A function that returns true for animals to be affected.
     * @param animalFunctions A vector of pairs containing search parameters and
     * the corresponding functions to apply.
     */
    virtual void randomApplyUpFunctionToAnimals(
        std::function<bool(Animal&)> checker,
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    );
    ///@}

    /** @name Edibles operations
     */
    ///@{
    /**
     * @brief Applies functions to all edible animals and resources.
     *
     * Overrides the base implementation. Functions are applied deterministically
     * to all animals and resources that match the provided search parameters.
     *
     * @param animalFunctions Vector of pairs containing animal search parameters
     *                        and the functions to apply.
     * @param resourceFunctions Vector of pairs containing resource search parameters
     *                          and the functions to apply.
     */
    void applyFunctionToEdibles(
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const std::vector<std::pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    ) override;

    /**
     * @brief Applies functions to edible animals and resources within a specific radius
     *        and optionally filtered by a custom checker.
     *
     * Overrides the base implementation. Functions are applied to all animals and resources
     * within the specified radius that satisfy the `checker` predicate.
     *
     * @param checker Function returning true for animals to be affected.
     * @param sourcePosition Center point for the radius check.
     * @param radius Radius around the source position to consider.
     * @param radiusArea Optional ring model defining the effective area.
     * @param animalFunctions Vector of pairs containing animal search parameters
     *                        and the functions to apply.
     * @param resourceFunctions Vector of pairs containing resource search parameters
     *                          and the functions to apply.
     */
    void applyFunctionToEdibles(
        std::function<bool(Animal&)> checker, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const std::vector<std::pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    ) override;

    /**
     * @brief Applies functions to edible animals and resources within a radius.
     *
     * This method allows applying effects to all animals and resources within a
     * given radius around `sourcePosition`. Unlike the previous methods, it does
     * not require a custom checker.
     *
     * @param sourcePosition Center point for the radius check.
     * @param radius Radius around the source position to consider.
     * @param radiusArea Optional ring model defining the effective area.
     * @param animalFunctions Vector of pairs containing animal search parameters
     *                        and the functions to apply.
     * @param resourceFunctions Vector of pairs containing resource search parameters
     *                          and the functions to apply.
     */
    virtual void applyFunctionToEdiblesInRadius(
        const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const std::vector<std::pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    );
    ///@}

    /**
     * @brief Serialization method for persistence.
     *
     * @tparam Archive Archive type.
     * @param ar Archive object.
     * @param version Version number of the class.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* SPATIAL_TREE_TERRAINCELL_H_ */
