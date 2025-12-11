/**
 * @file PredationProbability.h
 * @brief Defines the PredationProbability class, which calculates the probability
 * of a predator consuming a prey, extending EdibleDecisionProbability.
 */

#ifndef PREDATION_PROBABILITY_H_
#define PREDATION_PROBABILITY_H_


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/EdibleDecisionProbability.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/EscapeProbability.h"
#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/Instar.h"


class Animal;


/**
 * @class PredationProbability
 * @brief Manages predation probability functions for species across different instars.
 *
 * This class computes the probability of predation events using multiple components,
 * including predator reach, voracity, prey traits, and probability density functions (PDFs).
 * It also manages instar-specific maximum PDF values and sensory model integration.
 */
class PredationProbability : public EdibleDecisionProbability {
public:
    /**
     * @brief Calculates a probability density function for a predator-prey pair.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @return Probability density function value as PreciseDouble.
     */
    static PreciseDouble calculateProbabilityDensityFunction(const Animal& predator, const Animal& prey);


    /**
     * @brief Default constructor.
     */
    PredationProbability();

    /**
     * @brief Constructor from JSON configuration and species genetics.
     *
     * @param config JSON object with configuration parameters.
     * @param genetics Species genetics.
     * @param numberOfInstars Number of instars.
     * @param pdfThreshold Threshold for PDF-based calculations.
     * @param sensoryModel Pointer to the sensory model of the species.
     */
    PredationProbability(const nlohmann::json& config, const AnimalSpeciesGenetics& genetics, const unsigned int numberOfInstars, const PreciseDouble& pdfThreshold, const SensoryModel* const sensoryModel);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~PredationProbability();

    // Delete copy constructor
    PredationProbability(const PredationProbability&) = delete;

    // Delete assignment operator
	PredationProbability& operator=(const PredationProbability&) = delete;

    /**
     * @brief Calculates the predation probability for a predator-prey pair.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Predation probability as PreciseDouble.
     */
    PreciseDouble calculateValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass) const override;

    /**
     * @brief Updates the maximum PDF value for a given instar.
     *
     * @param instar Instar stage.
     * @param newValue New maximum PDF value.
     */
    void updateMaximumPDF(const Instar& instar, const PreciseDouble& newValue);

    /**
     * @brief Returns the maximum PDF value for a given instar.
     *
     * @param instar Instar stage.
     * @return Maximum PDF value as PreciseDouble.
     */
    const PreciseDouble& getMaximumPDF(const Instar& instar) const;

    /**
     * @brief Deserializes the probability functions from saved state.
     */
    void deserializeProbabilityFunctions() override;

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
     * @brief Initializes all probability functions for this class.
     */
    void initProbabilityFunctions();

    /**
     * @brief Returns the individual-level traits used for weighting in predation.
     *
     * @param predator Predator individual.
     * @return Vector of IndividualTrait references.
     */
    const std::vector<IndividualTrait>& getWeightIndividualTraits(const AnimalNonStatistical& predator) const;

private:
    /// Escape probability component for predation calculations.
    EscapeProbability escapeProbability;

    /// Maximum PDF values for each instar.
    CustomIndexedVector<Instar, PreciseDouble> maximumPDF;

    /// Threshold for PDF-based decisions.
    PreciseDouble pdfThreshold;

    /// Flag indicating whether to use global maximum prey voracity.
    bool useGlobalMaximumPreyVoracity;


    /**
     * @brief Returns the PDF threshold value.
     * @return PDF threshold as PreciseDouble.
     */
    const PreciseDouble& getPdfThreshold() const;

    /**
     * @brief Calculates the reach probability of a predator attacking a prey.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Reach probability as PreciseDouble.
     */
    PreciseDouble calculateReachProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass& preyDryMass);
    
    /**
     * @brief Calculates the interpolated probability density function value for a predator-prey pair.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Interpolated PDF value as PreciseDouble.
     */
    PreciseDouble calculateInterpolatedProbabilityDensityFunction(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass& preyDryMass);
    
    /**
     * @brief Calculates the predator voracity component of predation probability.
     */
    PreciseDouble calculateVoracityPredatorProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass& preyDryMass);
    
    /**
     * @brief Calculates the prey voracity component of predation probability.
     */
    PreciseDouble calculateVoracityPreyProbability(const AnimalNonStatistical& predator, const AnimalNonStatistical& prey, const DryMass& preyDryMass);
};

#endif /* PREDATION_PROBABILITY_H_ */
