/**
 * @file IndividualTrait.h
 * @brief Defines the IndividualTrait class representing a trait of an individual animal.
 * 
 * IndividualTrait stores the constitutive and phenotypic values of a trait for a
 * specific individual, allows tuning based on environmental conditions (temperature),
 * checks if the trait is within restricted ranges, and provides access to trait type.
 */

#ifndef INDIVIDUAL_TRAIT_H_
#define INDIVIDUAL_TRAIT_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Trait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTraitTemperatureSection.h"


/**
 * @class IndividualTrait
 * @brief Represents an individual trait of an animal.
 * 
 * This class encapsulates a single trait of an individual animal. It holds both the
 * constitutive (genetic) and phenotypic (expressed) values. Traits can be tuned according
 * to environmental conditions, such as temperature, and can be checked against
 * restricted ranges defined for the species.
 */
class IndividualTrait {
public:
    /**
     * @brief Default constructor.
     */
    IndividualTrait();

    /**
     * @brief Constructor for initializing an individual trait with environmental tuning.
     * 
     * @param trait Pointer to the trait definition.
     * @param genome Reference to the individual's genome.
     * @param traitsPerModule Number of traits per module.
     * @param numberOfLociPerTrait Number of loci per trait.
     * @param rhoPerModule Vector of correlation values per module.
     * @param rhoRangePerModule Vector of correlation range indices per module.
     * @param temperature Current environmental temperature.
     * @param actualTimeStep Current simulation timestep.
     * @param coefficientForMassAforMature Coefficient A for mature mass calculation.
     * @param scaleForMassBforMature Scale B for mature mass calculation.
     * @param tempFromLab Temperature data from lab experiments.
     */
    IndividualTrait(Trait* trait, const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule, const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab);
    
    /**
     * @brief Constructor for testing without environmental tuning.
     * 
     * @param trait Pointer to the trait definition.
     * @param genome Reference to the individual's genome.
     * @param traitsPerModule Number of traits per module.
     * @param numberOfLociPerTrait Number of loci per trait.
     * @param rhoPerModule Vector of correlation values per module.
     * @param rhoRangePerModule Vector of correlation range indices per module.
     */
    IndividualTrait(Trait* trait, const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule);

    /**
     * @brief Destructor.
     */
    virtual ~IndividualTrait();

    /**
     * @brief Get the constitutive (genetic) value of the trait.
     * @return Reference to the constitutive value.
     */
    const PreciseDouble& getConstitutiveValue() const;

    /**
     * @brief Get the phenotypic (expressed) value of the trait.
     * @return Reference to the phenotypic value.
     */
    const PreciseDouble& getPhenotypicValue() const;

    /**
     * @brief Tune the trait according to environmental conditions.
     * @param temperature Current environmental temperature.
     * @param actualTimeStep Current simulation timestep.
     * @param coefficientForMassAforMature Coefficient A for mature mass calculation.
     * @param scaleForMassBforMature Scale B for mature mass calculation.
     * @param tempFromLab Temperature from lab experiments.
     */
    void tune(const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab);

    /**
     * @brief Convert the trait to a string representation.
     * @return String containing trait information.
     */
    std::string to_string() const;

    /**
     * @brief Get the trait type.
     * @return Type of the individual trait.
     */
    IndividualLevelTrait::Type getType() const;

    /**
     * @brief Set a new trait definition.
     * @param newTrait Pointer to the new trait definition.
     */
    void setTrait(Trait* newTrait);

    #ifdef DEBUG
        /**
         * @brief Deserialize debug-specific variables.
         */
        void deserializeDebugVariables();
    #endif

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
    Trait* trait; /**< Pointer to the trait definition. */
    PreciseDouble constitutiveValue; /**< Genetic value of the trait. */
    PreciseDouble phenotypicValue; /**< Expressed (phenotypic) value of the trait. */
    IndividualTraitTemperatureSection* temperatureSection; /**< Temperature-dependent tuning section. */

    #ifdef DEBUG
        TimeStep phenotypicValueLastTimeStep; /**< Last timestep when phenotypic value was updated (debug). */

        /**
         * @brief Debug function to manually set phenotypic value.
         * @param actualTimeStep Current simulation timestep.
         */
        void testSetPhenotypicValue(const TimeStep actualTimeStep);
    #endif

    /**
     * @brief Set the phenotypic value.
     * @param newValue New phenotypic value.
     * @param actualTimeStep Current simulation timestep.
     */
    void setPhenotypicValue(const PreciseDouble& newValue, const TimeStep actualTimeStep);
};

#endif // INDIVIDUAL_TRAIT_H_
