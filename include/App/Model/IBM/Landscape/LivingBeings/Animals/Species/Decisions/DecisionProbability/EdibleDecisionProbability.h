/**
 * @file EdibleDecisionProbability.h
 * @brief Defines the EdibleDecisionProbability class, which extends DecisionProbability
 * to compute probabilities related to predation on edible prey.
 */

#ifndef EDIBLE_DECISION_PROBABILITY_H_
#define EDIBLE_DECISION_PROBABILITY_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/DecisionProbability.h"


/**
 * @class EdibleDecisionProbability
 * @brief Base class for computing probabilities related to edible prey decisions.
 *
 * This class manages probability functions for predator-prey interactions where
 * the prey is an edible resource. It allows multiple probability functions to
 * be applied and a remaining function for unassigned indices.
 */
class EdibleDecisionProbability : public DecisionProbability {
public:
    /**
     * @brief Default constructor.
     */
    EdibleDecisionProbability();

    /**
     * @brief Constructor with configuration and probability weight traits.
     *
     * @param config JSON object containing configuration parameters.
     * @param probabilityWeightTraits Vector of traits used to weight probabilities.
     */
    EdibleDecisionProbability(const nlohmann::json& config, const std::vector<Trait*> &probabilityWeightTraits);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~EdibleDecisionProbability();

    // Delete copy constructor
    EdibleDecisionProbability(const EdibleDecisionProbability&) = delete;

    // Delete assignment operator
	EdibleDecisionProbability& operator=(const EdibleDecisionProbability&) = delete;

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
     * @brief Sets a specific probability function at a given index.
     *
     * @param index Index where the function will be applied.
     * @param newProbabilityFunction Function to compute probability for predator-prey interactions.
     */
    void setProbabilityFunction(const size_t index, const std::function<PreciseDouble(const AnimalNonStatistical&, const Edible&, const DryMass&)>& newProbabilityFunction);
    
    /**
     * @brief Sets the remaining probability function for unassigned indices.
     *
     * @param newProbabilityFunction Function to compute remaining probabilities.
     */
    void setRemainingProbabilityFunction(const std::function<PreciseDouble(const AnimalNonStatistical&, const Edible&, const DryMass&)>& newProbabilityFunction);

    /**
     * @brief Applies the probability function at a specific index.
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
    /// Vector of probability functions for specific indices.
    std::vector<std::function<PreciseDouble(const AnimalNonStatistical&, const Edible&, const DryMass&)>> probabilityFunctions;
    
    /// Probability function for remaining indices.
    std::function<PreciseDouble(const AnimalNonStatistical&, const Edible&, const DryMass&)> remainingProbabilityFunction;


    /**
     * @brief Default probability function when no specific function is assigned.
     *
     * @return Default probability as PreciseDouble.
     */
    PreciseDouble defaultProbabilityFunction();
};

#endif /* EDIBLE_DECISION_PROBABILITY_H_ */
