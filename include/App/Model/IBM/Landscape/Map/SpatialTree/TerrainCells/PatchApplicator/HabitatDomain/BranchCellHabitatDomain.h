/**
 * @file BranchCellHabitatDomain.h
 * @brief Defines the BranchCellHabitatDomain class for managing habitat domains in branch terrain cells.
 */

#ifndef BRANCH_CELL_HABITAT_DOMAIN_H_
#define BRANCH_CELL_HABITAT_DOMAIN_H_


#include <vector>
#include <numeric>

#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/HabitatDomain/CellHabitatDomainInterface.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/HabitatDomainSource.h"


class BranchTerrainCell;


/**
 * @class BranchCellHabitatDomain
 * @brief Habitat domain implementation for branch nodes in a spatial tree.
 *
 * This class manages habitat domain information for a branch terrain cell.
 * It can aggregate habitat data from its children and apply a combined
 * habitat domain status to the branch node.
 */
class BranchCellHabitatDomain : public CellHabitatDomainInterface
{
public:
    /// Default constructor.
    BranchCellHabitatDomain();

    /**
     * @brief Constructor with a specified patch priority.
     * @param newHabitatDomainPatchPriority Patch priority for habitat domain updates.
     */
    BranchCellHabitatDomain(const size_t newHabitatDomainPatchPriority);

    /// Destructor.
    virtual ~BranchCellHabitatDomain();

    /// Deleted copy constructor
    BranchCellHabitatDomain(const BranchCellHabitatDomain&) = delete;

    /// Deleted copy assignment
    BranchCellHabitatDomain& operator=(const BranchCellHabitatDomain&) = delete;

    /**
     * @brief Aggregates habitat domain information from the children of the branch cell.
     * @param summaryTerrainCell Pointer to the branch terrain cell to summarize.
     *
     * This method updates the habitat domain of the branch node based on the
     * habitat domain status of its child terrain cells.
     */
    void applyBranch(BranchTerrainCell* const summaryTerrainCell);

    /**
     * @brief Serialization function.
     * @tparam Archive Type of the archive.
     * @param ar Archive object.
     * @param version Version of serialization.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    /**
     * @brief Calculates the patch priority for the habitat domain based on child cells.
     * @param summaryTerrainCell Pointer to the branch terrain cell.
     * @return Calculated patch priority.
     *
     * This static method computes the patch priority to use when updating
     * the habitat domain for a branch cell based on its children's priorities.
     */
    static size_t calculateHabitatDomainPatchPriority(BranchTerrainCell* const summaryTerrainCell);
};

#endif /* BRANCH_CELL_HABITAT_DOMAIN_H_ */
