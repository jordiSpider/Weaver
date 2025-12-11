/**
 * @file Genetics.h
 * @brief Defines the Genetics class which manages an individual's genome and traits.
 * 
 * The Genetics class represents the genetic information of an individual animal.
 * It contains the genome, individual traits, and provides methods for tuning,
 * cloning, accessing traits, and evaluating the genetic state.
 */

#ifndef GENETICS_H_
#define GENETICS_H_



#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "Misc/CustomIndexedVector.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/BaseTraitType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/ProbabilityDensityFunctionTraitType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/CellValueTraitType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/PreferencesTraitType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTrait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/AnimalSpeciesGenetics.h"


/**
 * @class Genetics
 * @brief Represents the genetic information and traits of an individual.
 * 
 * The Genetics class manages an individual's genome, traits, and provides
 * functions to tune, clone, and evaluate traits based on environmental
 * conditions such as temperature. It also allows access to base traits,
 * weights for predation, escape, and edibility, as well as preferences.
 */
class Genetics
{
public:
    /**
     * @brief Default constructor.
     */
    Genetics();

    /**
     * @brief Constructor for a new individual with environmental tuning.
     * 
     * @param speciesGenetics Pointer to the species genetics configuration.
     * @param temperature Current temperature.
     * @param actualTimeStep Current timestep in the simulation.
     * @param coefficientForMassAforMature Coefficient A for mature mass calculation.
     * @param scaleForMassBforMature Scale B for mature mass calculation.
     * @param tempFromLab Temperature data from lab experiments.
     */
    Genetics(AnimalSpeciesGenetics* speciesGenetics, const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab);
    
    /**
     * @brief Constructor for an individual from parental gametes.
     * 
     * @param speciesGenetics Pointer to the species genetics configuration.
     * @param firstParentGamete Pointer to the first parent's gamete.
     * @param secondParentGamete Pointer to the second parent's gamete.
     * @param temperature Current temperature.
     * @param actualTimeStep Current timestep in the simulation.
     * @param coefficientForMassAforMature Coefficient A for mature mass calculation.
     * @param scaleForMassBforMature Scale B for mature mass calculation.
     * @param tempFromLab Temperature data from lab experiments.
     */
    Genetics(AnimalSpeciesGenetics* speciesGenetics, Gamete* const firstParentGamete, Gamete* const secondParentGamete, const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab);

    /**
     * @brief Destructor for the Genetics class.
     */
    virtual ~Genetics();

    /**
     * @brief Checks if all traits are within restricted ranges.
     * @return True if all traits are within allowed ranges, false otherwise.
     */
    bool isInsideRestrictedRanges() const;

    /**
     * @brief Get all base individual traits.
     * @return Vector of base individual traits.
     */
    const std::vector<IndividualTrait>& getBaseIndividualTraits() const;

    /**
     * @brief Get a specific base individual trait by type.
     * @param type Type of the base trait.
     * @return Reference to the requested IndividualTrait.
     */
    const IndividualTrait& getBaseIndividualTraits(const BaseTraitType type) const;

    /**
     * @brief Get traits used as weights for predation probability.
     */
    const std::vector<IndividualTrait>& getPredationProbabilityWeightIndividualTraits() const;

    /**
     * @brief Get traits used as weights for escape probability.
     */
    const std::vector<IndividualTrait>& getEscapeProbabilityWeightIndividualTraits() const;

    /**
     * @brief Get traits used as weights for edibility value.
     */
    const std::vector<IndividualTrait>& getEdibilityValueWeightIndividualTraits() const;

    /**
     * @brief Get trait representing a probability density function.
     * @param type Type of PDF trait.
     */
    const IndividualTrait& getProbabilityDensityFunctionIndividualTraits(const ProbabilityDensityFunctionTraitType type) const;

    /**
     * @brief Get trait representing cell value.
     * @param type Type of cell value trait.
     */
    const IndividualTrait& getCellValueIndividualTraits(const CellValueTraitType type) const;

    /**
     * @brief Get trait representing preference values.
     * @param type Type of preference trait.
     */
    const IndividualTrait& getPreferencesIndividualTraits(const PreferencesTraitType type) const;

    /**
     * @brief Delete homologous correlosomes from the genome.
     */
    void deleteHomologousCorrelosomes();

    /**
     * @brief Tune the individual's traits based on environmental conditions.
     * 
     * @param temperature Current temperature.
     * @param actualTimeStep Current simulation timestep.
     * @param coefficientForMassAforMature Coefficient A for mature mass calculation.
     * @param scaleForMassBforMature Scale B for mature mass calculation.
     * @param tempFromLab Temperature from lab data.
     */
    void tune(const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab);

    /**
     * @brief Convert the genetics information to a string representation.
     * @return String describing traits and genome.
     */
    std::string to_string() const;

    /**
     * @brief Set the species genetics pointer.
     * @param newSpeciesGenetics Pointer to the new species genetics configuration.
     */
    void setAnimalSpeciesGenetics(AnimalSpeciesGenetics* newSpeciesGenetics);

    /**
     * @brief Print all traits as a string.
     * @return String with traits information.
     */
    std::string printTraits() const;

    /**
     * @brief Clone the genome.
     * @return Pointer to a cloned Genome object.
     */
    Genome* cloneGenome() const;

    /**
     * @brief Get the genome (const access).
     * @return Reference to the genome.
     */
    const Genome& getGenome() const;

    /**
     * @brief Get the genome (mutable access).
     * @return Reference to the genome.
     */
    Genome& getMutableGenome();

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
    AnimalSpeciesGenetics* speciesGenetics; /**< Pointer to the species genetics. */
    Genome genome; /**< Genome of the individual. */

    CustomIndexedVector<Trait::ExecutionOrder, std::vector<IndividualTrait>> allIndividualTraits; /**< All individual traits indexed by execution order. */


    /**
     * @brief Initialize individual traits based on environmental and biological parameters.
     * @param temperature Current temperature.
     * @param actualTimeStep Current simulation timestep.
     * @param coefficientForMassAforMature Coefficient A for mature mass calculation.
     * @param scaleForMassBforMature Scale B for mature mass calculation.
     * @param tempFromLab Temperature from lab data.
     */
    void initTraits(const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab);
};

#endif // GENETICS_H_
