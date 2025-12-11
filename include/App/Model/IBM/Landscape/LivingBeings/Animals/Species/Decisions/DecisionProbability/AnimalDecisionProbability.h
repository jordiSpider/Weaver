/**
 * @file AnimalDecisionProbability.h
 * @brief Defines the AnimalDecisionProbability class, a specialization of DecisionProbability
 * for predator-prey interactions using animal traits.
 */

#ifndef ANIMAL_DECISION_PROBABILITY_H_
#define ANIMAL_DECISION_PROBABILITY_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/DecisionProbability.h"


/**
 * @class AnimalDecisionProbability
 * @brief Abstract class for decision probabilities specific to animal interactions.
 *
 * This class extends DecisionProbability to handle probabilities that depend on
 * both predator and prey animals. It manages probability functions indexed
 * by traits and provides a mechanism to apply them.
 */
class AnimalDecisionProbability : public DecisionProbability {
public:
    /**
     * @brief Default constructor.
     */
    AnimalDecisionProbability();

    /**
     * @brief Constructor with configuration and probability weight traits.
     *
     * @param config JSON object containing configuration parameters.
     * @param probabilityWeightTraits Vector of traits used to weight probabilities.
     */
    AnimalDecisionProbability(const nlohmann::json& config, const std::vector<Trait*> &probabilityWeightTraits);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~AnimalDecisionProbability();

    // Delete copy constructor
    AnimalDecisionProbability(const AnimalDecisionProbability&) = delete;

    // Delete assignment operator
	AnimalDecisionProbability& operator=(const AnimalDecisionProbability&) = delete;

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
     * @brief Sets a probability function at a given index.
     *
     * @param index Index of the probability function.
     * @param newProbabilityFunction Function to compute probability based on predator, prey, and prey dry mass.
     */
    void setProbabilityFunction(const size_t index, const std::function<PreciseDouble(const AnimalNonStatistical&, const AnimalNonStatistical&, const DryMass&)>& newProbabilityFunction);
    
    /**
     * @brief Sets the remaining probability function for indices not explicitly assigned.
     *
     * @param newProbabilityFunction Function to compute remaining probabilities.
     */
    void setRemainingProbabilityFunction(const std::function<PreciseDouble(const AnimalNonStatistical&, const AnimalNonStatistical&, const DryMass&)>& newProbabilityFunction);

    /**
     * @brief Applies the probability function at a given index.
     *
     * @param index Index of the probability function.
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Computed probability as PreciseDouble.
     */
    PreciseDouble applyProbabilityFunctions(const size_t index, const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const;
    
    /**
     * @brief Applies the remaining probability function for unassigned indices.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Computed probability as PreciseDouble.
     */
    PreciseDouble applyRemainingProbabilityFunctions(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const;

private:
    /// Vector of probability functions indexed by trait.
    std::vector<std::function<PreciseDouble(const AnimalNonStatistical&, const AnimalNonStatistical&, const DryMass&)>> probabilityFunctions;
    
    /// Function to handle remaining probabilities for unassigned indices.
    std::function<PreciseDouble(const AnimalNonStatistical&, const AnimalNonStatistical&, const DryMass&)> remainingProbabilityFunction;


    /**
     * @brief Default probability function returning a base probability.
     *
     * @return Default probability value.
     */
    PreciseDouble defaultProbabilityFunction();
};

#endif /* ANIMAL_DECISION_PROBABILITY_H_ */
