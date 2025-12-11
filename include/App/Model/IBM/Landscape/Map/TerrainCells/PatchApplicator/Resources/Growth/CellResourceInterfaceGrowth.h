/**
 * @file CellResourceInterfaceGrowth.h
 * @brief Growth logic for a resource cell, extending EdibleGrowth.
 *
 * This class manages the growth state of a resource in a terrain cell.
 * It tracks biomass and interacts with the resource species growth object.
 */

#ifndef CELL_RESOURCE_INTERFACE_GROWTH_H_
#define CELL_RESOURCE_INTERFACE_GROWTH_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Growth/EdibleGrowth.h"
#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/Growth/ResourceSpeciesGrowth.h"
#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Landscape/Map/Patches/Source/ResourceSource.h"


class CellResourceInterface;


/**
 * @class CellResourceInterfaceGrowth
 * @brief Manages growth and biomass for a resource cell.
 *
 * This class extends EdibleGrowth and provides methods to update,
 * consume, and manage the biomass of a resource cell. It maintains a
 * link to the owning CellResourceInterface and the corresponding
 * ResourceSpeciesGrowth object.
 */
class CellResourceInterfaceGrowth : public EdibleGrowth
{
public:
	/**
     * @brief Default constructor.
     *
     * Initializes an empty growth object without owner or species growth reference.
     */
	CellResourceInterfaceGrowth();

	/**
     * @brief Constructor with owner and species growth.
     *
     * @param owner Pointer to the owning CellResourceInterface.
     * @param newSpeciesGrowth Pointer to the species growth object for this resource.
     */
	CellResourceInterfaceGrowth(CellResourceInterface* owner, ResourceSpeciesGrowth* const newSpeciesGrowth);
	
	/// Virtual destructor.
	virtual ~CellResourceInterfaceGrowth();

	/**
     * @brief Set the owner of this growth object.
     *
     * @param newOwner Pointer to the owning CellResourceInterface.
     */
	void setOwner(CellResourceInterface* newOwner);

	/**
     * @brief Subtract dry biomass from the resource.
     *
     * @param dryMassToBeSubstracted Amount of biomass to subtract.
     */
	void substractBiomass(const DryMass& dryMassToBeSubstracted);

	/**
     * @brief Get the minimum edible biomass that can be consumed.
     *
     * @return Constant reference to the minimum biomass value.
     */
	virtual const WetMass& getMinimumEdibleBiomass() const=0;

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
	/// Pointer to the owning CellResourceInterface.
	CellResourceInterface* owner;

	/**
     * @brief Get constant pointer to the species growth object.
     *
     * @return Pointer to ResourceSpeciesGrowth.
     */
	const ResourceSpeciesGrowth* getSpeciesGrowth() const;

	/**
     * @brief Get mutable pointer to the species growth object.
     *
     * @return Pointer to ResourceSpeciesGrowth.
     */
    ResourceSpeciesGrowth* getMutableSpeciesGrowth();

	/**
     * @brief Get constant pointer to the owner cell.
     *
     * @return Pointer to the owning CellResourceInterface.
     */
	const CellResourceInterface* getOwner() const;

	/**
     * @brief Get mutable pointer to the owner cell.
     *
     * @return Pointer to the owning CellResourceInterface.
     */
	CellResourceInterface* getMutableOwner();

	/**
     * @brief Set the current total dry mass of the resource.
     *
     * Overrides EdibleGrowth method.
     *
     * @param newCurrentTotalDryMass New total dry mass value.
     */
	void setCurrentTotalDryMass(const DryMass& newCurrentTotalDryMass) override;
};

#endif /* CELL_RESOURCE_INTERFACE_GROWTH_H_ */
