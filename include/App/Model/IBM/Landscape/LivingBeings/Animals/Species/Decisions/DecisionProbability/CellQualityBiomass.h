/**
 * @file CellQualityBiomass.h
 * @brief Defines the CellQualityBiomass class, which calculates the quality of a cell
 * based on prey biomass and edibility.
 */

#ifndef CELL_QUALITY_H_
#define CELL_QUALITY_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/EdibilityValue.h"


/**
 * @class CellQualityBiomass
 * @brief Computes the quality of a cell for a predator based on prey biomass and edibility.
 *
 * This class uses the EdibilityValue object to assess how valuable the resources in a cell
 * are for a predator, taking into account the proximity and biomass of prey.
 */
class CellQualityBiomass {
public:
    /**
     * @brief Default constructor.
     */
    CellQualityBiomass();

    /**
     * @brief Constructor with an EdibilityValue object.
     *
     * @param edibilityValue Pointer to the EdibilityValue object used for calculations.
     */
    CellQualityBiomass(const EdibilityValue* edibilityValue);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~CellQualityBiomass();

    // Delete copy constructor
    CellQualityBiomass(const CellQualityBiomass&) = delete;

    // Delete assignment operator
	CellQualityBiomass& operator=(const CellQualityBiomass&) = delete;

	/**
     * @brief Calculates the quality of a cell for a predator.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Cell quality value as PreciseDouble.
     */
    PreciseDouble calculateValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const;

    /**
     * @brief Sets the EdibilityValue object used for cell quality calculations.
     *
     * @param newEdibilityValue Pointer to the EdibilityValue object.
     */
    void setEdibilityValue(const EdibilityValue* newEdibilityValue);

    /**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    /// Pointer to the EdibilityValue object used for calculations.
    const EdibilityValue* edibilityValue;


    /**
     * @brief Calculates the probability of a prey being available based on proximity.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @return Proximity probability as PreciseDouble.
     */
    PreciseDouble calculateProximityProbability(const AnimalNonStatistical& predator, const Edible& prey) const;
};

#endif /* CELL_QUALITY_H_ */
