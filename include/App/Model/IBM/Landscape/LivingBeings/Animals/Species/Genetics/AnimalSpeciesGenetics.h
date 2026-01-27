/**
 * @file AnimalSpeciesGenetics.h
 * @brief Defines the AnimalSpeciesGenetics class for managing the genetics of animal species.
 */

#ifndef ANIMAL_SPECIES_GENETICS_H_
#define ANIMAL_SPECIES_GENETICS_H_


#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>


#include "Misc/EnumClass.h"
#include "Misc/CustomIndexedVector.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Locus.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/IndividualLevelTrait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTrait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/BaseTraitType.h"


/**
 * @class AnimalSpeciesGenetics
 * @brief Manages the genetic architecture of an animal species.
 *
 * This class handles loci, chromosomes, traits, alleles, and related genetic operations.
 * It allows reading from JSON configuration files and serializing for persistence.
 */
class AnimalSpeciesGenetics
{
public:
     /**
     * @brief Default constructor. Initializes empty genetics.
     */
     AnimalSpeciesGenetics();

     /**
     * @brief Constructor from JSON configuration.
     * @param config JSON containing genetic parameters, traits, and loci information.
     */
	AnimalSpeciesGenetics(const nlohmann::json &config);

     /**
     * @brief Destructor.
     */
	virtual ~AnimalSpeciesGenetics();

     /** @brief Get the number of loci per trait. */
     size_t getNumberOfLociPerTrait() const;

     /** @brief Get the number of traits per module. */
     size_t getTraitsPerModule() const;

     /** @brief Get the number of chiasmas per chromosome. */
     size_t getNumberOfChiasmasPerChromosome() const;

     /** @brief Get the number of loci per chromosome. */
     size_t getNumberOfLociPerChromosome() const;

     /** @brief Get the total number of chromosomes. */
     size_t getNumberOfChromosomes() const;

     /** @brief Get the range of rho coefficients per module. */
     const std::vector<size_t>& getRhoRangePerModuleVector() const;

     /** @brief Get the rho coefficients per module. */
     const std::vector<PreciseDouble>& getRhoPerModuleVector() const;

     /** @brief Get all loci organized per trait. */
     const std::vector<std::vector<Locus>>& getLociPerTrait() const;

     /** @brief Get randomly created positions for chromosomes. */
     const std::vector<size_t>& getRandomlyCreatedPositionsForChromosomes() const;

     /** @brief Get all individual-level traits. */
     const std::vector<IndividualLevelTrait*>& getIndividualLevelTraits() const;

     /** @brief Get the number of alleles per locus. */
     size_t getNumberOfAllelesPerLocus() const;

     /**
      * @brief Update pseudo-value ranges for all individual-level traits.
      * 
      * @param genome Genome used to calculate pseudo-value ranges.
      */
     void updatePseudoValueRanges(const Genome& genome);

     /** 
     * @brief Get all traits indexed by execution order.
     * @return Reference to all traits.
     */
     CustomIndexedVector<Trait::ExecutionOrder, std::vector<Trait*>>& getAllTraits();

     /** @brief Get all base traits. */
     const std::vector<Trait*>& getBaseTraits() const;

     /** @brief Get a specific base trait by type. */
     const Trait* getBaseTraits(const BaseTraitType type) const;

     /** @brief Get traits used to weight edibility values. */
     const std::vector<Trait*>& getEdibilityValueWeightTraits() const;

     /** @brief Get traits used to weight escape probability. */
     const std::vector<Trait*>& getEscapeProbabilityWeightTraits() const;

     /** @brief Get traits used to weight predation probability. */
     const std::vector<Trait*>& getPredationProbabilityWeightTraits() const;

     /** @brief Check if growth traits are temperature dependent. */
     bool isGrowthTraitsThermallyDependent() const;

     /**
     * @brief Initialize genetic files on disk for this species.
     * @param geneticsFolder Path to the folder where genetics files are stored.
     */
     void initializeGeneticFiles(const std::filesystem::path& geneticsFolder);

     /**
     * @brief Print the genetics of a genome to an output stream.
     * @param animalInfo Output stream for animal information.
     * @param genome Genome to print.
     */
     void printGenetics(const std::ostringstream& animalInfo, const Genome& genome) const;

     /**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
	template<class Archive>
     void serialize(Archive & ar, const unsigned int version);

private:
     /**
     * @brief Generate the genetic header string for a file.
     * @param numberOfLociPerTrait Number of loci per trait.
     * @return Header string.
     */
     static std::string generateGeneticHeader(const size_t numberOfLociPerTrait);


     size_t numberOfLociPerTrait; /**< Number of loci per trait */
     size_t numberOfAllelesPerLocus; /**< Number of alleles per locus */
     size_t traitsPerModule; /**< Number of traits per module */

     size_t numberOfChromosomes; /**< Number of chromosomes */
     size_t numberOfChiasmasPerChromosome; /**< Number of chiasmas per chromosome */
     size_t numberOfLociPerChromosome; /**< Number of loci per chromosome */

     std::vector<PreciseDouble> rhoPerModule; /**< Correlation coefficient per module */
     std::vector<size_t> rhoRangePerModule; /**< Range of rho per module */

     CustomIndexedVector<Trait::ExecutionOrder, std::vector<Trait*>> allTraits; /**< All traits organized by execution order */

     std::vector<IndividualLevelTrait*> individualLevelTraits; /**< Traits at the individual level */

     std::vector<std::vector<Locus>> lociPerTrait; /**< Loci organized per trait */
     std::vector<size_t> randomlyCreatedPositionsForChromosomes; /**< Positions randomly created for chromosomes */


     /** @brief Set traits using JSON configuration and modify alleles. */
     void setTraits(const nlohmann::json& traitsConfig, const nlohmann::json& modifyAlleles);

     /** @brief Generate loci for each trait based on JSON configuration. */
     void generateLociPerTrait(const nlohmann::json& modifyAlleles);

     /** @brief Set correlation coefficients per module. */
     void setRhoPerModule(const std::vector<double>& correlationCoefficientRhoPerModule);

     /** @brief Get the number of individual-level traits. */
     size_t getNumberOfIndividualLevelTraits() const;

     /**
     * @brief Deserialize individual-level traits from all traits.
     * @param allTraits Reference to all traits organized by execution order.
     * @param individualLevelTraits Vector to fill with deserialized traits.
     */
     static void deserializeIndividualLevelTraits(CustomIndexedVector<Trait::ExecutionOrder, std::vector<Trait*>>& allTraits, std::vector<IndividualLevelTrait*>& individualLevelTraits);
};

#endif /* ANIMAL_SPECIES_GENETICS_H_ */
