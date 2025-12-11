/**
 * @file TraitTemperatureSection.h
 * @brief Defines the TraitTemperatureSection class, representing temperature-dependent sections of traits.
 */

#ifndef TRAIT_TEMPERATURE_SECTION_H_
#define TRAIT_TEMPERATURE_SECTION_H_

#include <vector>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Genome.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/IndividualLevelTrait.h"


class IndividualTraitTemperatureSection;


/**
 * @class TraitTemperatureSection
 * @brief Represents the temperature-dependent part of a trait.
 *
 * This class provides the interface for managing temperature-specific aspects of traits,
 * including generating individual trait temperature sections and serializing/deserializing data.
 */
class TraitTemperatureSection {
public:
    /**
     * @brief Returns the CSV header for a given trait.
     *
     * @param trait Name of the trait.
     * @return Header string for CSV or data output.
     */
    static std::string getHeader(const std::string& trait);


    /**
     * @brief Default constructor.
     */
    TraitTemperatureSection();
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~TraitTemperatureSection();    

    /**
     * @brief Generates an individual-level temperature section for a trait.
     *
     * Must be implemented by derived classes. Creates an object representing
     * the temperature-dependent characteristics of an individual trait.
     *
     * @param geneticValue Genetic value of the trait.
     * @param genome Genome containing trait information.
     * @param traitsPerModule Number of traits per module.
     * @param numberOfLociPerTrait Number of loci per trait.
     * @param rhoPerModule Vector of rho values per module.
     * @param rhoRangePerModule Vector of rho ranges per module.
     * @return Pointer to an IndividualTraitTemperatureSection representing the individual's temperature-dependent trait.
     */
    virtual IndividualTraitTemperatureSection* generateIndividualTraitTemperatureSection(const PreciseDouble& geneticValue, const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule) const=0;

    /**
     * @brief Deserializes individual-level traits.
     *
     * Must be implemented by derived classes. Populates the provided vector
     * with individual-level traits from a serialized format.
     *
     * @param individualLevelTraits Vector of pointers to IndividualLevelTrait objects to populate.
     */
    virtual void deserializeIndividualLevelTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits)=0;

    /**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};


#endif // TRAIT_TEMPERATURE_SECTION_H_
