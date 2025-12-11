/**
 * @file CellResource.h
 * @brief Implementation of a resource cell in a terrain.
 *
 * This class represents a concrete resource in a terrain cell. It implements
 * the CellResourceInterface and manages growth, consumption, biomass addition,
 * and resource distribution.
 */

#ifndef CELL_RESOURCE_H_
#define CELL_RESOURCE_H_


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResourceInterface.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/Growth/CellResourceGrowth.h"



/**
 * @class CellResource
 * @brief Concrete implementation of a resource cell.
 *
 * Manages the lifecycle of a resource in a terrain cell, including growth, 
 * predation, biomass addition, and serialization.
 *
 * @see CellResourceInterface
 */
class CellResource : public CellResourceInterface
{
public:
	/**
     * @brief Default constructor.
     *
     * Initializes the resource with default values.
     */
	CellResource();

	/**
     * @brief Constructor with terrain and species information.
     *
     * @param landscape Pointer to the containing landscape.
     * @param terrainCell Pointer to the terrain cell.
     * @param newResourcePatchPriority Patch priority of this resource.
     * @param resourceSpecies Reference to the species of this resource.
     * @param newResourceSource Optional pointer to the resource source.
     */
	CellResource(Landscape* const landscape, TerrainCell* terrainCell, const size_t newResourcePatchPriority, ResourceSpecies& resourceSpecies, ResourceSource* const newResourceSource);
	
	/// Virtual destructor.
	virtual ~CellResource();

	/// Copy constructor deleted to prevent copying.
	CellResource(const CellResource&) = delete;

	/// Copy assignment deleted to prevent copying.
	CellResource& operator=(const CellResource&) = delete;

	/**
     * @brief Get constant reference to the growth building block.
     *
     * @return Constant reference to the growth building block.
     */
	const CellResourceGrowth& getGrowthBuildingBlock() const override;

	/**
     * @brief Get mutable reference to the growth building block.
     *
     * @return Mutable reference to the growth building block.
     */
	CellResourceGrowth& getMutableGrowthBuildingBlock() override;

	/// Update the state of the resource, including growth and biomass.
	void update();

	/**
     * @brief Apply a resource source to this cell.
     *
     * @param landscape Pointer to the landscape.
     * @param source Pointer to the resource source.
     * @param resourcePatchPriority Priority of the resource patch.
     */
	void applySource(Landscape* const landscape, const ResourceSource* const source, const size_t resourcePatchPriority);

	/**
     * @brief Perform growth of the resource based on environmental conditions.
     *
     * @param landscape Pointer to the landscape.
     */
	void growth(Landscape* const landscape);

	/**
     * @brief Add biomass to the resource.
     *
     * @param landscape Pointer to the landscape.
     * @param biomass Amount of biomass to add.
     * @param competitionAmongResourceSpecies If true, consider competition with other resources.
     */
	void addBiomass(Landscape* const landscape, const WetMass& biomass, const bool competitionAmongResourceSpecies);

	/**
     * @brief Calculate the available dry mass for consumption.
     *
     * @param fullCoverage Whether to consider the full cell coverage.
     * @param sourcePosition Position of the consumer.
     * @param radius Radius around the source position.
     * @param radiusArea Optional area for partial coverage calculations.
     * @return Amount of dry mass available.
     */
	DryMass calculateDryMassAvailable(const bool fullCoverage, const PointContinuous* const sourcePosition, const PreciseDouble &radius, const RingModel * const radiusArea) const;

	/// Deserialize resource sources applied to this cell.
	void deserializeCellResource(std::vector<std::pair<size_t, ResourceSource*>>& appliedResource);

	/**
     * @brief Subtract biomass from the resource due to consumption or competition.
     *
     * @param dryMassToBeSubstracted Amount of dry mass to remove.
     * @param sourcePosition Position of the consumer.
     * @param radius Radius around the source position.
     * @param competitionAmongResourceSpecies Whether to consider competition with other resources.
     */
	void substractBiomass(const DryMass& dryMassToBeSubstracted, const PointContinuous &sourcePosition, const PreciseDouble &radius, const bool competitionAmongResourceSpecies);

	/**
     * @brief Subtract biomass with optional full coverage consideration.
     *
     * @param dryMassToBeSubstracted Amount of dry mass to remove.
     * @param fullCoverage Whether to consider full cell coverage.
     * @param sourcePosition Position of the consumer.
     * @param radius Radius around the source position.
     * @param radiusArea Optional area for partial coverage.
     * @param competitionAmongResourceSpecies Whether to consider competition with other resources.
     */
	void substractBiomass(const DryMass& dryMassToBeSubstracted, const bool fullCoverage, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const bool competitionAmongResourceSpecies);

	/**
     * @brief Serialization method for persistence.
     *
     * @tparam Archive Archive type.
     * @param ar Archive object.
     * @param version Version number of the class.
     */
	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
	/// Growth building block that manages resource growth internally.
	CellResourceGrowth growthBuildingBlock;

	/**
     * @brief Spread biomass across the terrain.
     *
     * @param landscape Pointer to the landscape.
     * @param massToSpread Amount of biomass to spread.
     */
	void spread(Landscape* const landscape, const WetMass& massToSpread);

	/**
     * @brief Set the species growth object for this resource.
     *
     * @param newSpeciesGrowth Pointer to the new species growth object.
     */
	void setSpeciesGrowth(SpeciesGrowth* newSpeciesGrowth);
};

#endif /* CELL_RESOURCE_H_ */
