/**
 * @file CellResourceGrowth.h
 * @brief Concrete implementation of resource growth for a terrain cell.
 *
 * This class manages the growth dynamics of a resource cell, including
 * biomass addition, consumption, and capacity tracking. It extends
 * CellResourceInterfaceGrowth.
 */

#ifndef CELL_RESOURCE_GROWTH_H_
#define CELL_RESOURCE_GROWTH_H_


#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/Growth/CellResourceInterfaceGrowth.h"


class Landscape;


/**
 * @class CellResourceGrowth
 * @brief Concrete growth handler for a resource in a terrain cell.
 *
 * This class implements growth logic for a resource cell. It tracks
 * wet biomass, maximum capacity, and interacts with the ResourceSource.
 */

class CellResourceGrowth : public CellResourceInterfaceGrowth
{
public:
	/// Default constructor.
	CellResourceGrowth();

	/**
     * @brief Constructor with owner, species growth, and resource source.
     *
     * @param owner Pointer to the owning CellResourceInterface.
     * @param newSpeciesGrowth Pointer to the species growth object.
     * @param newResourceSource Pointer to the resource source for this cell.
     */
	CellResourceGrowth(CellResourceInterface* owner, ResourceSpeciesGrowth* const newSpeciesGrowth, ResourceSource* const newResourceSource);
	
	/// Virtual destructor.
	virtual ~CellResourceGrowth();

	/**
     * @brief Apply a new resource source to this growth object.
     *
     * @param competitionAmongResourceSpecies Whether to account for competition among species.
     * @param source Pointer to the resource source to apply.
     */
	void applySource(const bool competitionAmongResourceSpecies, const ResourceSource* const source);

	/**
     * @brief Check if the resource has reached full capacity.
     *
     * @param competitionAmongResourceSpecies Whether to account for competition among species.
     * @return True if full capacity, false otherwise.
     */
	bool isFullCapacity(const bool competitionAmongResourceSpecies) const;

	/**
     * @brief Perform growth update for this resource cell.
     *
     * @param landscape Pointer to the landscape object.
     * @param moisture Current moisture level in the cell.
     * @param temperature Current temperature in the cell.
     * @return WetMass gained during growth.
     */
	WetMass growth(Landscape* const landscape, const PreciseDouble& moisture, const Temperature& temperature);

	/**
     * @brief Add biomass to the resource.
     *
     * @param addedMass Amount of wet biomass to add.
     * @param competitionAmongResourceSpecies Whether to consider competition.
     * @return Actual wet biomass added.
     */
	WetMass addBiomass(const WetMass& addedMass, const bool competitionAmongResourceSpecies);

	/**
     * @brief Subtract dry biomass from the resource.
     *
     * @param dryMassToBeSubstracted Amount of dry biomass to remove.
     * @param competitionAmongResourceSpecies Whether to consider competition.
     */
	void substractBiomass(const DryMass& dryMassToBeSubstracted, const bool competitionAmongResourceSpecies);

	/**
     * @brief Get the minimum edible biomass that can be consumed.
     *
     * @return Constant reference to minimum biomass.
     */
	const WetMass& getMinimumEdibleBiomass() const;

	/**
     * @brief Get the current resource source associated with this growth.
     *
     * @return Reference to the resource source.
     */
	const ResourceSource& getSource() const;

	/**
     * @brief Set a new resource source for this growth object.
     *
     * @param source Pointer to the new resource source.
     */
	void setResourceSource(const ResourceSource* const source);

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
	/// Pointer to the resource source for this cell.
    const ResourceSource* resourceSource;

    /// Whether the resource is at full capacity.
    bool fullCapacity;

	/**
     * @brief Set the current total wet mass of the resource.
     *
     * Overrides base class method.
     *
     * @param newCurrentTotalWetMass New total wet mass.
     */
	void setCurrentTotalWetMass(const WetMass& newCurrentTotalWetMass) override;

	/**
     * @brief Update the full capacity status based on competition.
     *
     * @param competitionAmongResourceSpecies Whether to consider competition.
     */
	void updateFullCapacityStatus(const bool competitionAmongResourceSpecies);

	/**
     * @brief Set the full capacity flag manually.
     *
     * @param newFullCapacityValue New value of fullCapacity flag.
     */
	void setFullCapacity(const bool newFullCapacityValue);

	/**
     * @brief Calculate the available capacity for additional biomass.
     *
     * @param competitionAmongResourceSpecies Whether to account for competition.
     * @return Available WetMass capacity.
     */
	WetMass calculateAvailableCapacity(const bool competitionAmongResourceSpecies) const;
};

#endif /* CELL_RESOURCE_GROWTH_H_ */
