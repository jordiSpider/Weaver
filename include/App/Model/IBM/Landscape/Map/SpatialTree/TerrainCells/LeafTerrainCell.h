/**
 * @file LeafTerrainCell.h
 * @brief Leaf node implementation of a spatial tree terrain cell.
 *
 * This class represents a leaf cell in a spatial tree, which contains actual
 * animals and resources. It provides methods for localized operations,
 * radius-based searches, and applying functions to animals and edibles.
 */

#ifndef LEAF_TERRAINCELL_H_
#define LEAF_TERRAINCELL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <cmath>


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/LeafPatchApplicator.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/Points/PointSpatialTree.h"
#include "Misc/Types.h"



class BranchTerrainCell;


/**
 * @class LeafTerrainCell
 * @brief Represents a leaf terrain cell in a spatial tree.
 *
 * Leaf cells are the terminal nodes of the spatial tree. They store animals
 * and resources and handle operations such as radius-based evaluations,
 * registering edibles, and applying functions to contained entities.
 */
class LeafTerrainCell : public SpatialTreeTerrainCell
{
public:
    /// Default constructor.
    LeafTerrainCell();

    /**
     * @brief Constructor with sizes, parent, position, and moisture source.
     *
     * @param cellSizes Vector of cell sizes for hierarchical division.
     * @param parentTerrainCell Pointer to the parent branch cell.
     * @param position Pointer to the spatial position.
     * @param moistureBaseSource Pointer to the moisture source of the cell.
     */
    LeafTerrainCell(const std::vector<PreciseDouble>& cellSizes, BranchTerrainCell* const parentTerrainCell, PointSpatialTree* const position, MoistureSource* const moistureBaseSource);
    
    /// Virtual destructor.
    virtual ~LeafTerrainCell();

    /**
     * @brief Get the child terrain cell by spatial position.
     *
     * @param cellPos Position of the child cell.
     * @return Pointer to the child cell (leaf returns itself).
     */
    SpatialTreeTerrainCell* getCell(const PointSpatialTree &cellPos) override;

    /**
     * @brief Get a reference to a child terrain cell by position (const).
     *
     * @param cellPos Position of the child cell.
     * @return Const reference to the child cell pointer.
     */
    SpatialTreeTerrainCell* const& getChildTerrainCell(const PointSpatialTree &cellPos) const;
    
    /**
     * @brief Retrieve terrain cells within a radius for evaluation.
     *
     * @param bestEvaluations Vector to store evaluated cell values.
     * @param sourcePosition Center position for the radius search.
     * @param radius Search radius.
     * @param radiusArea Optional ring model area.
     * @param searchDepth Depth of search (ignored for leaf).
     * @param searchNeighborsWithFemales Whether to prioritize cells with females.
     * @param parentFullCoverage Flag indicating if parent has full coverage.
     * @param animalWhoIsEvaluating Pointer to the animal performing evaluation.
     * @param animalsHasTriedToPredate List of animals already considered.
     */
    void getRadiusTerrainCells(
        std::vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const size_t searchDepth, const bool searchNeighborsWithFemales, const bool parentFullCoverage, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate
    );

    /**
     * @brief Check if a given position corresponds to a child.
     *
     * @param childPos Spatial position to check.
     * @return True if the position is within this leaf cell (always true for leaf).
     */
    bool isChild(const PointSpatialTree &childPos) const;

    /// Get mutable leaf patch applicator.
    LeafPatchApplicator& getMutablePatchApplicator() override;

    /// Get patch applicator (const).
    const LeafPatchApplicator& getPatchApplicator() const override;

    /**
     * @brief Register edibles and animals present in this leaf cell.
     *
     * @param landscapeResources 3D vector of landscape resources.
     * @param landscapeAnimals Vector of all animals in the landscape.
     */
    void registerEdibles(std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, std::vector<AnimalNonStatistical*>& landscapeAnimals);

    /// Check if this cell is a leaf.
    bool isLeaf() const;

    /** @name Downward traversal functions
     */
    ///@{
    void applyDownFunctionToAnimals(const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions) override;
    void applyDownFunctionToAnimals(
        std::function<bool(Animal&)> checker, const PointContinuous &sourcePosition, const PreciseDouble &radius,
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    ) override;
    void randomApplyDownFunctionToAnimals(const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions) override;
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

#endif /* LEAF_TERRAINCELL_H_ */
