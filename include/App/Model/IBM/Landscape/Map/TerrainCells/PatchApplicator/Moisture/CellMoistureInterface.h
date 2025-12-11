/**
 * @file CellMoistureInterface.h
 * @brief Abstract interface for managing moisture, temperature, and resource
 *        capacity in a terrain cell within the ecosystem simulation.
 *
 * This interface defines the environmental behavior associated with a terrain
 * cell, including hydration processes, temperature updates, humidity dynamics,
 * and resource capacity computation. Any concrete moisture model in Weaver
 * must derive from this interface.
 */

#ifndef CELL_MOISTURE_INTERFACE_H_
#define CELL_MOISTURE_INTERFACE_H_


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

#include "App/Model/IBM/Physics/Temperature.h"
#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/MoistureSource.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/CellElement.h"



class TerrainCell;


/**
 * @class CellMoistureInterface
 * @brief Base interface representing the hydrological and thermal properties
 *        of a terrain cell.
 *
 * This class specifies:
 * - how moisture sources are applied to a cell,
 * - how temperature and relative humidity are updated,
 * - how the maximum resource capacity of a cell is computed and retrieved,
 * - how the cell integrates into global environmental cycles.
 *
 * Implementations must define all pure virtual functions, encapsulating the
 * specific behavior of the moisture model.
 *
 * @see MoistureSource
 * @see TerrainCell
 */
class CellMoistureInterface : public CellElement
{
public:
    /**
     * @brief Default constructor.
     *
     * Initializes the cell moisture component with neutral environmental
     * conditions and no defined patch priority.
     */
    CellMoistureInterface();

    /**
     * @brief Constructor with moisture patch priority.
     * @param newMoisturePatchPriority Priority used for moisture source application.
     *
     * This constructor allows integrating the cell into a structured
     * environmental patch system, where multiple sources may affect the same cell.
     */
    CellMoistureInterface(const size_t newMoisturePatchPriority);
    
    /// Virtual destructor.
    virtual ~CellMoistureInterface();

    /// Copy construction is disabled.
    CellMoistureInterface(const CellMoistureInterface&) = delete;
    
    /// Copy assignment is disabled.
    CellMoistureInterface& operator=(const CellMoistureInterface&) = delete;

    /**
     * @brief Applies a moisture source to the cell.
     *
     * @param newMoistureSource Pointer to the moisture source being applied.
     * @param newMoisturePatchPriority Priority of the moisture patch.
     * @param cellSize Physical size of the cell.
     *
     * This function is responsible for computing how a moisture source affects
     * the thermal and hydration state of the cell. Derived classes must implement
     * the complete logic for hydration, temperature influence, and resulting
     * resource capacity changes.
     */
    virtual void applySource(const MoistureSource* const newMoistureSource, const size_t newMoisturePatchPriority, const PreciseDouble& cellSize)=0;

    // -------------------------------------------------------------------------
    // ABSTRACT GETTERS
    // -------------------------------------------------------------------------

    /// @return The current temperature of the cell.
    virtual const Temperature& getTemperature() const=0;
    
    /// @return The current moisture value (absolute or relative depending on implementation).
    virtual const PreciseDouble& getMoisture() const=0;

    /// @return true if the cell lies within predator-free space.
    virtual bool isInEnemyFreeSpace() const=0;

    /// @return true if the cell lies within competitor-free space.
    virtual bool isInCompetitorFreeSpace() const=0;

    /// @return Maximum resource capacity density of the cell.
    virtual const WetMass& getMaximumResourceCapacityDensity() const=0;

    /**
     * @brief Integrates the cell's temperature range into global and local cycles.
     *
     * @param globalTemperatureRange A set collecting all temperature values across the terrain.
     * @param temperatureCycle Local temperature cycle to which this cell contributes.
     *
     * Used to model daily or seasonal temperature patterns across the simulation.
     */
    virtual void addTemperatureRange(std::unordered_set<Temperature>& globalTemperatureRange, std::vector<Temperature>& temperatureCycle) const=0;

    /**
     * @brief Returns the total maximum resource capacity of the cell.
     *
     * This corresponds to the maximum wet mass of resources the cell can hold,
     * derived from the moisture source and cell size.
     */
    const WetMass& getTotalMaximumResourceCapacity() const;

    /**
     * @brief Updates the internal environmental state of the cell.
     *
     * Calls updateTemperature() and updateRelativeHumidity(), which must be
     * implemented by derived classes.
     */
    void update();

    /**
     * @brief Deserializes moisture state and re-applies moisture sources.
     *
     * @param appliedMoisture List of pairs (patch priority, applied moisture source).
     * @param newTerrainCell Pointer to the terrain cell to which this moisture
     *                       component now belongs.
     *
     * Allows reconstruction of the moisture state after loading from disk.
     * Derived classes must restore internal state consistency.
     */
    virtual void deserializeCellMoisture(std::vector<std::pair<size_t, MoistureSource*>>& appliedMoisture, TerrainCell* newTerrainCell)=0;

    /**
     * @brief Boost serialization function.
     *
     * Serializes the total maximum resource capacity and any other data required
     * by the concrete implementation.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    /**
     * @brief Computes total maximum resource capacity from a moisture source and cell size.
     *
     * @param source Moisture source affecting the cell.
     * @param cellSize Physical size of the cell.
     * @return Total maximum wet mass of resources supported by this cell.
     */
    static WetMass calculateTotalMaximumResourceCapacity(const MoistureSource* source, const PreciseDouble& cellSize);

    /// Total maximum resource capacity available in the cell.
    WetMass totalMaximumResourceCapacity;

    /// Sets the total maximum resource capacity.
    void setTotalMaximumResourceCapacity(const WetMass& newValue);

    // -------------------------------------------------------------------------
    // ABSTRACT UPDATE METHODS
    // -------------------------------------------------------------------------

    /// Updates the cell temperature based on current environmental factors.
    virtual void updateTemperature()=0;

    /// Updates the relative humidity of the cell.
    virtual void updateRelativeHumidity()=0;
};

#endif /* CELL_MOISTURE_INTERFACE_H_ */
