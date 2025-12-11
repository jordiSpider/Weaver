/**
 * @file EdibleGrowth.h
 * @brief Defines the EdibleGrowth class, which represents the growth state of an edible entity.
 */

#ifndef EDIBLE_GROWTH_H_
#define EDIBLE_GROWTH_H_



#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/SpeciesGrowth.h"
#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"



/**
 * @class EdibleGrowth
 * @brief Represents the growth characteristics and current biomasses of an edible object.
 *
 * This class models the growth state of an edible entity (resource or animal) by storing:
 * - Its current instar (developmental stage).
 * - The associated species growth model.
 * - Its current total dry and wet mass.
 *
 * It provides utilities to update the instar, set masses, and compute wet mass from dry mass
 * according to the species-specific growth rules.
 */
class EdibleGrowth
{
public:
	/// Default constructor.
	EdibleGrowth();

	/**
     * @brief Constructs an EdibleGrowth object.
     * @param speciesGrowth Pointer to the species growth model.
     * @param instar Initial instar of the edible entity.
     */
	EdibleGrowth(SpeciesGrowth* const speciesGrowth, const Instar &instar);

	/// Destructor.
	virtual ~EdibleGrowth();

	/**
     * @brief Returns the current instar of the edible entity.
     * @return Instar object.
     */
	const Instar& getInstar() const;

	/**
     * @brief Updates the instar of the edible entity.
     * @param newInstar The new instar value.
     */
	void setInstar(const Instar& newInstar);

	/**
     * @brief Sets a new species growth model.
     * @param newSpeciesGrowth Pointer to the new species growth data.
     */
	void setSpeciesGrowth(SpeciesGrowth* newSpeciesGrowth);

	/**
     * @brief Sets the current total wet mass of the edible entity.
     *
     * The dry mass is usually updated separately via setCurrentTotalDryMass().
     *
     * @param newCurrentTotalWetMass Wet mass value.
     */
	virtual void setCurrentTotalWetMass(const WetMass& newCurrentTotalWetMass);

	/**
     * @brief Returns the current total dry mass.
     * @return DryMass object.
     */
	const DryMass& getCurrentTotalDryMass() const;

	/**
     * @brief Returns the current total wet mass.
     * @return WetMass object.
     */
	const WetMass& getCurrentTotalWetMass() const;

	/**
     * @brief Computes the wet mass corresponding to a given dry mass.
     *
     * The conversion factor depends on the current instar and the species' growth model.
     *
     * @param dryMass Dry mass to convert.
     * @return Computed wet mass.
     */
    WetMass calculateWetMass(const DryMass& dryMass) const;

	/**
    * @brief Serializes the object for persistence.
    *
    * @tparam Archive Serialization archive type.
    * @param ar Archive instance.
    * @param version Serialization version.
    */
	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
	Instar instar; ///< Current instar of the edible entity.
    SpeciesGrowth* speciesGrowth; ///< Pointer to the species' growth information.

    DryMass currentTotalDryMass; ///< Current dry mass of the entity.
    WetMass currentTotalWetMass; ///< Current wet mass of the entity.

	/**
     * @brief Returns the associated species growth model (const version).
     * @return Pointer to the species growth model.
     */
	virtual const SpeciesGrowth* getSpeciesGrowth() const;

	/**
     * @brief Returns a mutable pointer to the species growth model.
     * @return Mutable species growth pointer.
     */
    virtual SpeciesGrowth* getMutableSpeciesGrowth();

	/**
     * @brief Sets the current total dry mass.
     * @param newCurrentTotalDryMass Dry mass value.
     */
	virtual void setCurrentTotalDryMass(const DryMass& newCurrentTotalDryMass);
};

#endif /* EDIBLE_GROWTH_H_ */
