/**
 * @file CellResourceInterface.h
 * @brief Interface for resources in a terrain cell.
 *
 * This class represents a resource (edible) within a terrain cell. It inherits from
 * Edible and CellElement and defines the interface for resource-specific behavior,
 * including growth, consumption, and serialization.
 */

#ifndef CELL_RESOURCE_INTERFACE_H_
#define CELL_RESOURCE_INTERFACE_H_


#include <vector>


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "App/Model/IBM/Landscape/LivingBeings/Edible.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/Points/PointSpatialTree.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/Growth/CellResourceInterfaceGrowth.h"
#include "App/Model/IBM/Landscape/Map/Geometry/Geometry.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/CellElement.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/ResourceSource.h"



/**
 * @class CellResourceInterface
 * @brief Interface for a resource in a terrain cell.
 *
 * Provides methods to manage resource growth, consumption by animals, and 
 * interactions with the terrain. All concrete resource implementations should
 * inherit from this class.
 *
 * @see Edible, CellElement
 */
class CellResourceInterface : public Edible, public CellElement
{
public:
	/**
     * @brief Default constructor.
     *
     * Initializes the resource with default values.
     */
	CellResourceInterface();

	/**
     * @brief Constructor with resource ID, terrain cell, patch priority, and species.
     *
     * @param id Identifier of the resource.
     * @param terrainCell Pointer to the terrain cell that contains this resource.
     * @param newResourcePatchPriority Patch priority of this resource.
     * @param resourceSpecies Reference to the species of this resource.
     */
	CellResourceInterface(const EdibleID id, TerrainCell* terrainCell, const size_t newResourcePatchPriority, ResourceSpecies& resourceSpecies);
	
	/// Virtual destructor.
	virtual ~CellResourceInterface();

	/// Copy constructor deleted to prevent copying.
	CellResourceInterface(const CellResourceInterface&) = delete;

	/// Copy assignment deleted to prevent copying.
	CellResourceInterface& operator=(const CellResourceInterface&) = delete;

	/**
     * @brief Get constant reference to the growth building block.
     *
     * @return Reference to the growth building block.
     */
	virtual const CellResourceInterfaceGrowth& getGrowthBuildingBlock() const=0;

	/**
     * @brief Get mutable reference to the growth building block.
     *
     * @return Reference to the growth building block.
     */
	virtual CellResourceInterfaceGrowth& getMutableGrowthBuildingBlock()=0;

	/// Update the resource state.
	virtual void update()=0;

	/**
     * @brief Check if this resource is currently being hunted by any animal.
     *
     * @return True if resource is under predation.
     */
	bool isHunting() const;

	/**
     * @brief Get a constant pointer to the resource species.
     *
     * @return Pointer to the resource species.
     */
	const ResourceSpecies* getSpecies() const;

	/**
     * @brief Get a mutable pointer to the resource species.
     *
     * @return Pointer to the resource species.
     */
	ResourceSpecies* getMutableSpecies();

	/**
     * @brief Get the voracity of the resource (how much can be consumed by animals).
     *
     * @return Voracity value.
     */
	PreciseDouble getVoracity() const;

	/// Increase the number of predation encounters on this resource.
	void increasePredationEncounters();

	/**
     * @brief Convert a target biomass to dry mass based on predator's remaining voracity.
     *
     * @param targetDryMass Target dry mass to convert.
     * @param predatorRemainingVoracity Remaining voracity of the predator.
     * @return Dry mass that can be consumed.
     */
	DryMass turnIntoDryMass(const DryMass &targetDryMass, const PreciseDouble& predatorRemainingVoracity) const;

	/**
     * @brief Calculate the available dry mass for consumption.
     *
     * @param fullCoverage Whether to consider the full cell coverage.
     * @param sourcePosition Position from which consumption is measured.
     * @param radius Radius around the source position.
     * @param radiusArea Optional area for partial coverage calculations.
     * @return Amount of dry mass available.
     */
	virtual DryMass calculateDryMassAvailable(const bool fullCoverage, const PointContinuous* const sourcePosition, const PreciseDouble &radius, const RingModel * const radiusArea) const=0;

	/// Deserialize applied resource sources for this cell.
	virtual void deserializeCellResource(std::vector<std::pair<size_t, ResourceSource*>>& appliedResource)=0;

	/**
     * @brief Subtract biomass from the resource due to consumption or competition.
     *
     * @param dryMassToBeSubstracted Amount of dry mass to remove.
     * @param sourcePosition Position of the consumer.
     * @param radius Radius around the source position.
     * @param competitionAmongResourceSpecies If true, consider competition with other resources.
     */
	virtual void substractBiomass(const DryMass& dryMassToBeSubstracted, const PointContinuous &sourcePosition, const PreciseDouble &radius, const bool competitionAmongResourceSpecies)=0;

	/**
     * @brief Subtract biomass from the resource with full coverage consideration.
     *
     * @param dryMassToBeSubstracted Amount of dry mass to remove.
     * @param fullCoverage Whether to consider the full cell coverage.
     * @param sourcePosition Position of the consumer.
     * @param radius Radius around the source position.
     * @param radiusArea Optional area for partial coverage calculations.
     * @param competitionAmongResourceSpecies If true, consider competition with other resources.
     */
	virtual void substractBiomass(const DryMass& dryMassToBeSubstracted, const bool fullCoverage, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const bool competitionAmongResourceSpecies)=0;

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

#endif /* CELL_RESOURCE_INTERFACE_H_ */
