/**
 * @file EdibilityValue.h
 * @brief Defines the EdibilityValue class, which computes the edibility value
 * of a prey for a predator based on multiple probability functions and predation risk.
 */

#ifndef EDIBILITY_VALUE_H_
#define EDIBILITY_VALUE_H_


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/EdibleDecisionProbability.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/PredationProbability.h"


/**
 * @class EdibilityValue
 * @brief Calculates the edibility value of prey for a predator.
 *
 * This class extends EdibleDecisionProbability and integrates predation probability
 * information to estimate how valuable a prey is for a predator. It also provides
 * assessment of resource quality for decision-making.
 */
class EdibilityValue : public EdibleDecisionProbability {
public:
    /**
     * @brief Default constructor.
     */
    EdibilityValue();

    /**
     * @brief Constructor with configuration, species genetics, and predation probability.
     *
     * @param config JSON object containing configuration parameters.
     * @param genetics Species genetics.
     * @param predationProbability Pointer to a PredationProbability object used for calculations.
     */
    EdibilityValue(const nlohmann::json& config, const AnimalSpeciesGenetics& genetics, const PredationProbability* predationProbability);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~EdibilityValue();

    // Delete copy constructor
    EdibilityValue(const EdibilityValue&) = delete;

    // Delete assignment operator
	EdibilityValue& operator=(const EdibilityValue&) = delete;

    /**
     * @brief Calculates the edibility value of a prey for a given predator.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Edibility value as PreciseDouble.
     */
    virtual PreciseDouble calculateValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const override;

    /**
     * @brief Sets the predation probability object used for calculations.
     *
     * @param newPredationProbability Pointer to the PredationProbability object.
     */
    void setPredationProbability(const PredationProbability* newPredationProbability);

    /**
     * @brief Returns whether the resource quality assessment is enabled.
     *
     * @return True if resource quality is assessed, false otherwise.
     */
    bool getQualityResourceAssessment() const;

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
     * @brief Initializes all probability functions for edibility value.
     */
    void initProbabilityFunctions();

    /**
     * @brief Returns the individual-level traits used for weighting the edibility value.
     *
     * @param predator Predator individual.
     * @return Vector of IndividualTrait references.
     */
    const std::vector<IndividualTrait>& getWeightIndividualTraits(const AnimalNonStatistical& predator) const;

private:
    /// Pointer to the predation probability object used in calculations.
    const PredationProbability* predationProbability;

    /// Flag indicating if resource quality assessment is active.
    bool qualityResourceAssessment;


    /**
     * @brief Calculates the predation probability component of edibility.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Predation probability as PreciseDouble.
     */
    PreciseDouble calculatePredationProbability(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const;
    
    /**
     * @brief Calculates the relative resource value component of edibility.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Relative resource value as PreciseDouble.
     */
    PreciseDouble calculateRelativeResourceValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const;
};

#endif /* EDIBILITY_VALUE_H_ */
