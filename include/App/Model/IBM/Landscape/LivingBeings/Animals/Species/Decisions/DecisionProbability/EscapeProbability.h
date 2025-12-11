/**
 * @file EscapeProbability.h
 * @brief Defines the EscapeProbability class, which calculates the probability
 * that a prey escapes from a predator.
 */

#ifndef ESCAPE_PROBABILITY_H_
#define ESCAPE_PROBABILITY_H_


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/AnimalDecisionProbability.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/SensoryModel.h"


/**
 * @class EscapeProbability
 * @brief Manages the escape probability functions for a prey in predator-prey interactions.
 *
 * This class computes the probability that a prey escapes predation based on
 * velocity, attack distance, and fleeing behavior. It integrates species sensory
 * model information and individual traits.
 */
class EscapeProbability : public AnimalDecisionProbability {
public:
    /**
     * @brief Default constructor.
     */
    EscapeProbability();

    /**
     * @brief Constructor from JSON configuration and species genetics.
     *
     * @param config JSON object containing configuration parameters.
     * @param genetics Species genetics.
     * @param sensoryModel Pointer to the sensory model used for calculations.
     */
    EscapeProbability(const nlohmann::json& config, const AnimalSpeciesGenetics& genetics, const SensoryModel* const sensoryModel);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~EscapeProbability();

    // Delete copy constructor
    EscapeProbability(const EscapeProbability&) = delete;

    // Delete assignment operator
	EscapeProbability& operator=(const EscapeProbability&) = delete;

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
     * @brief Initializes all escape probability functions.
     */
    void initProbabilityFunctions();

    /**
     * @brief Returns the individual-level traits used for weighting in escape probability.
     *
     * @param predator Predator individual.
     * @return Vector of IndividualTrait references.
     */
    const std::vector<IndividualTrait>& getWeightIndividualTraits(const AnimalNonStatistical& predator) const;

private:
    /// Pointer to the species sensory model.
    const SensoryModel* sensoryModel;

    /// Velocity coefficient used in escape probability calculations.
    PreciseDouble cVelocity;


    /**
     * @brief Returns the velocity coefficient.
     * @return Velocity coefficient as PreciseDouble.
     */
    const PreciseDouble& getCVelocity() const;

    /**
     * @brief Calculates the velocity-based component of escape probability.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Probability component as PreciseDouble.
     */
    PreciseDouble calculateVelocityProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass& preyDryMass);
    
    /**
     * @brief Calculates the attack distance-based component of escape probability.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Probability component as PreciseDouble.
     */
    PreciseDouble calculateAttackDistanceProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass& preyDryMass);
    
    /**
     * @brief Calculates the fleeing behavior component of escape probability.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Probability component as PreciseDouble.
     */
    PreciseDouble calculateFleeingProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass& preyDryMass);
};

#endif /* ESCAPE_PROBABILITY_H_ */
