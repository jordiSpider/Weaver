/**
 * @file CellMoisture.h
 * @brief Concrete implementation of CellMoistureInterface representing
 *        the moisture and thermal state of a terrain cell.
 *
 * This class models how a single moisture source affects a terrain cell,
 * including temperature, humidity, and the resulting maximum resource capacity.
 * It provides the concrete logic for environmental updates required by the
 * CellMoistureInterface base class.
 */

#ifndef CELL_MOISTURE_H_
#define CELL_MOISTURE_H_


#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Moisture/CellMoistureInterface.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/MoistureSource.h"


/**
  * @class CellMoisture
  * @brief Concrete moisture model for a terrain cell.
  *
  * CellMoisture represents a cell influenced by a single MoistureSource.
  * It manages:
  * - the current moisture source,
  * - the temperature and relative humidity derived from that source,
  * - whether the cell is predator-free or competitor-free space,
  * - the cell's moisture level,
  * - the maximum resource capacity density.
  *
  * This class is used by the simulation to compute environmental effects
  * impacting resource availability and animal behavior.
  *
  * @see CellMoistureInterface
  * @see MoistureSource
  */
class CellMoisture : public CellMoistureInterface
{
public:
    /**
     * @brief Default constructor.
     *
     * Creates an empty moisture component with no source assigned.
     */
    CellMoisture();

    /**
     * @brief Constructor assigning an initial moisture source.
     *
     * @param newMoistureSource Pointer to the moisture source affecting the cell.
     * @param newMoisturePatchPriority Patch priority for source application.
     */
    CellMoisture(const MoistureSource* const newMoistureSource, const size_t newMoisturePatchPriority);
    
    /// Virtual destructor.
    virtual ~CellMoisture();

    /// Copy construction disabled.
    CellMoisture(const CellMoisture&) = delete;

    /// Copy assignment disabled.
    CellMoisture& operator=(const CellMoisture&) = delete;

    /**
     * @brief Applies a moisture source to this cell.
     *
     * Replaces the cell's current moisture source if the new source has
     * sufficient patch priority. Updates temperature, humidity, and maximum
     * resource capacity according to the applied source.
     *
     * @param newMoistureSource Source being applied to the cell.
     * @param newMoisturePatchPriority Patch priority of the source.
     * @param cellSize Physical size of the cell.
     */
    void applySource(const MoistureSource* const newMoistureSource, const size_t newMoisturePatchPriority, const PreciseDouble& cellSize);

    /**
     * @brief Returns the currently assigned moisture source.
     * @return Pointer to the MoistureSource assigned to this cell.
     */
    const MoistureSource* getMoistureSource() const;

    /// @return Current temperature of the cell.
    const Temperature& getTemperature() const;

    /// @return Moisture value assigned to the cell.
    const PreciseDouble& getMoisture() const;

    /// @return true if the cell lies inside predator-free space.
    bool isInEnemyFreeSpace() const;

    /// @return true if the cell lies inside competitor-free space.
    bool isInCompetitorFreeSpace() const;

    /// @return Maximum resource capacity density determined by the moisture source.
    const WetMass& getMaximumResourceCapacityDensity() const;
    
    /**
     * @brief Adds temperature values from the cell to both global and local cycles.
     *
     * @param globalTemperatureRange A global set of all temperature values.
     * @param temperatureCycle Local temperature cycle for this cell.
     */
    virtual void addTemperatureRange(std::unordered_set<Temperature>& globalTemperatureRange, std::vector<Temperature>& temperatureCycle) const;

    /**
     * @brief Restores moisture state during deserialization.
     *
     * @param appliedMoisture List of (patch priority, moisture source) pairs.
     * @param newTerrainCell Pointer to the new parent terrain cell.
     */
    void deserializeCellMoisture(std::vector<std::pair<size_t, MoistureSource*>>& appliedMoisture, TerrainCell* newTerrainCell);

    /**
     * @brief Boost serialization method.
     *
     * Saves and loads the moisture source pointer and inherited data.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    /**
     * @brief The moisture source influencing this cell.
     *
     * May be nullptr if no source is currently assigned.
     */
    const MoistureSource* moistureSource;

    /**
     * @brief Updates the cell's temperature according to the moisture source.
     *
     * Implements the abstract method from CellMoistureInterface.
     */
    void updateTemperature();

    /**
     * @brief Updates relative humidity based on the moisture source.
     *
     * Implements the abstract method from CellMoistureInterface.
     */
    void updateRelativeHumidity();
};

#endif /* CELL_MOISTURE_H_ */
