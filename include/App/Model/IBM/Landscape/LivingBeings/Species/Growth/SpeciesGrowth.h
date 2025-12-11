/**
 * @file SpeciesGrowth.h
 * @brief Defines the SpeciesGrowth class representing the growth characteristics of a species.
 */

#ifndef SPECIES_GROWTH_H_
#define SPECIES_GROWTH_H_


#include <vector>


#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/Instar.h"
#include "Misc/CustomIndexedVector.h"

#include "App/Model/IBM/Maths/PreciseDouble.h"

#ifdef DEBUG
#include "Exceptions/LineInfoException.h"
#endif


/**
 * @class SpeciesGrowth
 * @brief Represents the growth characteristics of a species across its instars.
 *
 * This class encapsulates information about the number of instars, the range of instars,
 * and the conversion ratios to wet mass for each instar. It provides methods to access
 * the last instar, the range of instars, and the conversion coefficients.
 */
class SpeciesGrowth
{
public:
	/// Default constructor.
	SpeciesGrowth();

	/**
     * @brief Constructs a SpeciesGrowth object.
     * @param numberOfInstars Total number of instars for the species.
     * @param newConversionToWetMass Vector of conversion coefficients from dry to wet mass for each instar.
     */
	SpeciesGrowth(const unsigned int numberOfInstars, const std::vector<double>& newConversionToWetMass);
	
	/// Destructor.
	virtual ~SpeciesGrowth();

	/**
     * @brief Returns the total number of instars of the species.
     * @return Number of instars as unsigned int.
     */
	const unsigned int& getNumberOfInstars() const;

	/**
     * @brief Returns the last instar of the species.
     * @return Last instar.
     */
	const Instar& getLastInstar() const;

	/**
     * @brief Returns the range of instars of the species.
     * @return Vector containing all instars from the first to the last.
     */
	const std::vector<Instar>& getInstarsRange() const;

	/**
     * @brief Returns the conversion coefficient to wet mass for a specific instar.
     * @param instar Instar for which to get the conversion coefficient.
     * @return Conversion coefficient as PreciseDouble.
     */
	const PreciseDouble& getConversionToWetMass(const Instar& instar) const;

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
	unsigned int numberOfInstars; ///< Total number of instars.
    std::vector<Instar> instarsRange; ///< Range of instars for this species.
    Instar lastInstar; ///< Last instar of the species.

    /// Conversion ratio from dry to wet mass for each instar.
    CustomIndexedVector<Instar, PreciseDouble> conversionToWetMass;

	/**
     * @brief Calculates the range of instars from the first to the last.
     * @return Vector containing all instars.
     */
	std::vector<Instar> calculateInstarsRange();

	/**
     * @brief Sets the conversion coefficients from dry to wet mass.
     * @param newConversionToWetMass Vector of new conversion coefficients.
     */
	void setConversionToWetMass(const std::vector<double>& newConversionToWetMass);
};


#endif /* SPECIES_GROWTH_H_ */
