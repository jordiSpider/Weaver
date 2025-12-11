/**
 * @file SpatialTreePatchApplicator.h
 * @brief Defines the SpatialTreePatchApplicator class for applying patches in a spatial tree of terrain cells.
 */

#ifndef SPATIAL_TREE_PATCH_APPLICATOR_H_
#define SPATIAL_TREE_PATCH_APPLICATOR_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/PatchApplicator.h"



/**
 * @class SpatialTreePatchApplicator
 * @brief Abstract class for applying environmental patches to a spatial tree of terrain cells.
 *
 * This class extends PatchApplicator and adds functionality specific to the
 * spatial tree hierarchy (SpatialTreeTerrainCell). It provides an interface
 * for applying patches recursively down the tree.
 */
class SpatialTreePatchApplicator : public PatchApplicator
{
public:
    /// Default constructor.
    SpatialTreePatchApplicator();

    /**
     * @brief Constructor initializing the patch applicator with pointers to cell interfaces.
     * @param cell Pointer to the terrain cell that owns this patch applicator.
     * @param cellMoisture Pointer to the cell's moisture interface.
     * @param cellObstacle Pointer to the cell's obstacle interface.
     * @param cellHabitatDomain Pointer to the cell's habitat domain interface.
     */
    SpatialTreePatchApplicator(
        TerrainCell* cell, CellMoistureInterface* cellMoisture, CellObstacleInterface* cellObstacle, 
        CellHabitatDomainInterface* cellHabitatDomain
    );

    /// Destructor.
    virtual ~SpatialTreePatchApplicator();

    /// Deleted copy constructor
    SpatialTreePatchApplicator(const SpatialTreePatchApplicator&) = delete;

    /// Deleted copy assignment
    SpatialTreePatchApplicator& operator=(const SpatialTreePatchApplicator&) = delete;

    /**
     * @brief Applies a patch recursively downward in the spatial tree.
     * 
     * This method must be implemented by derived classes. It handles applying
     * the patch to this cell and potentially propagating it to child cells.
     * 
     * @param landscape Pointer to the landscape containing the terrain cells.
     * @param source The patch source to apply.
     * @param newPatchPriority Priority of the patch to apply.
     */
    virtual void applyDown(Landscape* const landscape, const PathSource &source, const size_t newPatchPriority)=0;

    /**
     * @brief Serialization method for persistence.
     * @tparam Archive Type of archive used.
     * @param ar Archive object.
     * @param version Version of the serialized object.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    /**
     * @brief Applies a patch with partial coverage.
     * 
     * This method must be implemented by derived classes. It handles patches
     * that do not fully cover the cell area and may require more complex logic
     * to update the cell properties.
     * 
     * @param landscape Pointer to the landscape.
     * @param patch The patch to apply partially.
     * @return A pair of bool values indicating the status of the patch application.
     */
    virtual std::pair<bool, bool> applyPartialCoveragePatch(Landscape* const landscape, const Patch &patch)=0;
};


#endif /* SPATIAL_TREE_PATCH_APPLICATOR_H_ */
