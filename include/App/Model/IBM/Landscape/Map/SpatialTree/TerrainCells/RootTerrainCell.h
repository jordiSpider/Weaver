/**
 * @file RootTerrainCell.h
 * @brief Defines the RootTerrainCell class, the top-level node of the spatial tree in the landscape.
 */

#ifndef ROOT_TERRAINCELL_H_
#define ROOT_TERRAINCELL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <cmath>
#include <memory>


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/Points/PointSpatialTree.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Animal.h"
#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"



/**
 * @class RootTerrainCell
 * @brief Represents the root node of the spatial tree in the landscape.
 *
 * The RootTerrainCell is the top-level branch of the spatial tree.
 * It inherits from BranchTerrainCell and represents the starting point
 * for all recursive operations on the spatial tree, including:
 * - Animal insertion, migration, and evaluation.
 * - Resource and moisture deserialization and updates.
 * - Applying functions to animals and resources recursively.
 *
 * Unlike other branches, the root cell has no parent, so getParent() returns nullptr.
 * It overrides certain methods to handle operations starting from the top of the tree.
 */
class RootTerrainCell : public BranchTerrainCell
{
public:
    /// Default constructor.
    RootTerrainCell();

    /**
     * @brief Constructor initializing the root cell with cell sizes, tree depth, position, and moisture source.
     * @param cellSizes Vector of hierarchical cell sizes.
     * @param mapDepth Depth of the spatial tree.
     * @param position Pointer to the spatial position of the root.
     * @param moistureBaseSource Pointer to the base moisture source.
     */
    RootTerrainCell(const std::vector<PreciseDouble>& cellSizes, const unsigned int& mapDepth, PointSpatialTree *position, MoistureSource* const moistureBaseSource);
    
    /// Destructor.
    virtual ~RootTerrainCell();

    /**
     * @brief Returns the parent branch of this cell (always nullptr for root).
     * @return Pointer to BranchTerrainCell (nullptr).
     */
    const BranchTerrainCell* getParent() const override;
    
    /**
     * @brief Returns a mutable parent branch (always nullptr for root).
     * @return Pointer to BranchTerrainCell (nullptr).
     */
    BranchTerrainCell* getMutableParent() const override;

    /**
     * @brief Retrieves neighboring terrain cells within a radius starting from the root.
     * @param bestEvaluations Vector to store evaluated cells.
     * @param sourcePosition Center of the search.
     * @param radius Radius of the search area.
     * @param radiusArea Optional area model for partial coverage.
     * @param searchDepth Depth limit for recursive search.
     * @param searchNeighborsWithFemales Whether to include neighbors with females.
     * @param animalWhoIsEvaluating Pointer to the animal performing the evaluation.
     * @param animalsHasTriedToPredate List of animals that have already tried predation.
     */
    void getNeighboursCellsOnRadius(
        std::vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const size_t searchDepth, const bool searchNeighborsWithFemales, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate
    ) override;

    /**
     * @brief Retrieves a child cell by spatial tree position starting from the root.
     * @param cellPos Position in the spatial tree.
     * @return Pointer to the SpatialTreeTerrainCell at that position.
     */
    SpatialTreeTerrainCell* getCell(const PointSpatialTree &cellPos) override;

    /**
     * @brief Applies functions upward to animals starting from the root.
     * @param checker Function to check which animals to apply operations to.
     * @param animalFunctions Vector of animal functions and their search parameters.
     */
    void applyUpFunctionToAnimals(
        std::function<bool(Animal&)> checker,
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    ) override;

    /**
     * @brief Applies functions upward to animals in random order starting from the root.
     * @param checker Function to check which animals to apply operations to.
     * @param animalFunctions Vector of animal functions and their search parameters.
     */
    void randomApplyUpFunctionToAnimals(
        std::function<bool(Animal&)> checker,
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    ) override;

    /**
     * @brief Applies functions to edibles within a given radius starting from the root.
     * @param sourcePosition Center of the search.
     * @param radius Radius of the search area.
     * @param radiusArea Optional area model for partial coverage.
     * @param animalFunctions Vector of animal functions with their search parameters.
     * @param resourceFunctions Vector of resource functions with their search parameters.
     */
    void applyFunctionToEdiblesInRadius(
        const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const std::vector<std::pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    ) override;

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

#endif /* ROOT_TERRAINCELL_H_ */
