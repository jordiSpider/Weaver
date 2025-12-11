/**
 * @file BranchCellMoisture.h
 * @brief Defines the BranchCellMoisture class for managing moisture in branch terrain cells.
 */

#ifndef BRANCH_CELL_MOISTURE_H_
#define BRANCH_CELL_MOISTURE_H_


#include <vector>
#include <numeric>

#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Moisture/CellMoistureInterface.h"


class BranchTerrainCell;


/**
 * @class BranchCellMoisture
 * @brief Moisture implementation for branch nodes in a spatial tree.
 *
 * This class aggregates moisture information from child cells of a branch
 * terrain cell. It provides current moisture, temperature, and resource capacity
 * estimates, as well as environmental flags such as enemy-free or competitor-free space.
 */
class BranchCellMoisture : public CellMoistureInterface
{
public:
    /// Default constructor.
    BranchCellMoisture();

    /**
     * @brief Constructor with initial moisture source and patch priority.
     * @param newMoistureSource Pointer to the moisture source.
     * @param newMoisturePatchPriority Patch priority for moisture updates.
     */
    BranchCellMoisture(const MoistureSource* const newMoistureSource, const size_t newMoisturePatchPriority);
    
    /// Destructor.
    virtual ~BranchCellMoisture();

    /// Deleted copy constructor
    BranchCellMoisture(const BranchCellMoisture&) = delete;

    /// Deleted copy assignment
    BranchCellMoisture& operator=(const BranchCellMoisture&) = delete;

    /**
     * @brief Apply a moisture source to this branch cell.
     * @param newMoistureSource Pointer to the moisture source.
     * @param newMoisturePatchPriority Patch priority for the update.
     * @param cellSize Size of the cell for calculation.
     */
    void applySource(const MoistureSource* const newMoistureSource, const size_t newMoisturePatchPriority, const PreciseDouble& cellSize);

    /**
     * @brief Aggregate moisture data from child terrain cells.
     * @param newSummaryTerrainCell Pointer to the branch terrain cell to summarize.
     */
    void applyBranch(BranchTerrainCell* const newSummaryTerrainCell);

    /// @brief Get the current temperature of the branch cell.
    const Temperature& getTemperature() const;

    /// @brief Get the current moisture of the branch cell.
    const PreciseDouble& getMoisture() const;

    /// @brief Check if the cell is free of enemies.
    bool isInEnemyFreeSpace() const;

    /// @brief Check if the cell is free of competitors.
    bool isInCompetitorFreeSpace() const;

    /// @brief Get the maximum resource capacity density for this cell.
    const WetMass& getMaximumResourceCapacityDensity() const;

    /**
     * @brief Add this cell's temperature to global temperature ranges and cycles.
     * @param globalTemperatureRange Set of all temperatures.
     * @param temperatureCycle Vector for temperature cycle updates.
     */
    void addTemperatureRange(std::unordered_set<Temperature>& globalTemperatureRange, std::vector<Temperature>& temperatureCycle) const override;

    /**
     * @brief Deserialize moisture data from child cells to reconstruct the branch cell.
     * @param appliedMoisture Vector of applied moisture sources.
     * @param newTerrainCell Pointer to the terrain cell being deserialized.
     */
    void deserializeCellMoisture(std::vector<std::pair<size_t, MoistureSource*>>& appliedMoisture, TerrainCell* newTerrainCell);

    /**
     * @brief Serialization function.
     * @tparam Archive Type of the archive.
     * @param ar Archive object.
     * @param version Version of serialization.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    const MoistureSource* moistureSource; ///< Pointer to the original moisture source.
    bool isSource; ///< Flag indicating if this cell is a source.

    BranchTerrainCell* summaryTerrainCell; ///< Pointer to the branch cell being summarized.

    bool inEnemyFreeSpace; ///< Flag indicating if the cell is free of enemies.
    bool inCompetitorFreeSpace; ///< Flag indicating if the cell is free of competitors.

    Temperature temperature; ///< Current temperature.
    PreciseDouble moisture; ///< Current humidity.

    /// @brief Update the temperature of the branch cell.
    void updateTemperature();

    /// @brief Update the relative humidity of the branch cell.
    void updateRelativeHumidity();

    /// @brief Set the current temperature.
    void setTemperature(const Temperature &newTemperature);

    /// @brief Set the current moisture.
    void setMoisture(const PreciseDouble &newMoisture);

private:
    /**
     * @brief Calculate the patch priority for moisture based on child cells.
     * @param summaryTerrainCell Pointer to the branch terrain cell.
     * @return Calculated patch priority.
     */
    static size_t calculateMoisturePatchPriority(const BranchTerrainCell* const summaryTerrainCell);

    /**
     * @brief Calculate total maximum resource capacity from child cells.
     * @param summaryTerrainCell Pointer to the branch terrain cell.
     * @return Total maximum resource capacity.
     */
    static WetMass calculateTotalMaximumResourceCapacity(const BranchTerrainCell* const summaryTerrainCell);

    /**
     * @brief Determine if the branch cell is free of enemies.
     * @param summaryTerrainCell Pointer to the branch terrain cell.
     * @return True if enemy-free, false otherwise.
     */
    static bool calculateInEnemyFreeSpace(const BranchTerrainCell* const summaryTerrainCell);

    /**
     * @brief Determine if the branch cell is free of competitors.
     * @param summaryTerrainCell Pointer to the branch terrain cell.
     * @return True if competitor-free, false otherwise.
     */
    static bool calculateInCompetitorFreeSpace(const BranchTerrainCell* const summaryTerrainCell);
};

#endif /* BRANCH_CELL_MOISTURE_H_ */
