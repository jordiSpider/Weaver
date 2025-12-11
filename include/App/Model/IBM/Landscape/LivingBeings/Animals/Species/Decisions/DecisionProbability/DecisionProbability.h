/**
 * @file DecisionProbability.h
 * @brief Defines the DecisionProbability class, which provides a base interface
 * for computing predator-prey decision probabilities.
 */

#ifndef DECISION_PROBABILITY_H_
#define DECISION_PROBABILITY_H_


#include <nlohmann/json.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>


#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTrait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/DecisionsTraitType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Genetics.h"


class Edible;
class AnimalNonStatistical;


/**
 * @class DecisionProbability
 * @brief Abstract base class for probability calculations in predator-prey decisions.
 *
 * This class provides a common interface for all decision probability computations,
 * such as escape probability, predation probability, and edibility value. It manages
 * probability weight traits and the additive mechanism for combining probabilities.
 */
class DecisionProbability {
public:
    /**
     * @brief Default constructor.
     */
    DecisionProbability();

    /**
     * @brief Constructor with configuration and probability weight traits.
     *
     * @param config JSON object containing configuration parameters.
     * @param probabilityWeightTraits Vector of traits used to weight probabilities.
     */
    DecisionProbability(const nlohmann::json& config, const std::vector<Trait*> &probabilityWeightTraits);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~DecisionProbability();

    // Delete copy constructor
    DecisionProbability(const DecisionProbability&) = delete;

    // Delete assignment operator
	DecisionProbability& operator=(const DecisionProbability&) = delete;

    /**
     * @brief Calculates the probability value for a predator-prey interaction.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Computed probability as PreciseDouble.
     */
    virtual PreciseDouble calculateValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const;

    /**
     * @brief Deserializes the probability functions after loading from persistence.
     */
    virtual void deserializeProbabilityFunctions();
	
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
    /**
     * @brief Initializes all probability functions.
     *
     * Must be implemented by derived classes.
     */
    virtual void initProbabilityFunctions()=0;

    /**
     * @brief Applies the probability function at a given index.
     *
     * Must be implemented by derived classes.
     *
     * @param index Index of the probability function.
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Computed probability as PreciseDouble.
     */
    virtual PreciseDouble applyProbabilityFunctions(const size_t index, const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const=0;
    
    /**
     * @brief Applies the remaining probability function for unassigned indices.
     *
     * Must be implemented by derived classes.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Computed probability as PreciseDouble.
     */
    virtual PreciseDouble applyRemainingProbabilityFunctions(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const=0;

    /**
     * @brief Returns the individual-level traits used for weighting the probabilities.
     *
     * Must be implemented by derived classes.
     *
     * @param predator Predator individual.
     * @return Vector of IndividualTrait references.
     */
    virtual const std::vector<IndividualTrait>& getWeightIndividualTraits(const AnimalNonStatistical& predator) const=0;

private:
    /// Flag indicating if an additive mechanism is used for combining probabilities.
    bool additiveMechanism;

    /// Vector tracking which probability weights have been computed.
    std::vector<bool> computedWeights;

    /// Flag indicating if the remaining probability weight has been computed.
    bool computedRemainingWeight;
};

#endif /* DECISION_PROBABILITY_H_ */
