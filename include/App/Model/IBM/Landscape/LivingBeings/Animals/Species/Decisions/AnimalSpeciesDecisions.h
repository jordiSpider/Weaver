/**
 * @file AnimalSpeciesDecisions.h
 * @brief Defines the AnimalSpeciesDecisions class, representing species-level decision traits.
 */

#ifndef ANIMAL_SPECIES_DECISIONS_H_
#define ANIMAL_SPECIES_DECISIONS_H_


#include <nlohmann/json.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <functional>
#include <vector>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Trait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/DecisionsTraitType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/ProbabilityDensityFunctionTraitType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTrait.h"
#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Landscape/Map/Geometry/Geometry.h"




#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/PredationProbability.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/EdibilityValue.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/DecisionProbability/CellQualityBiomass.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/SensoryModel.h"



class Edible;
class Animal;
class AnimalNonStatistical;


/**
 * @class AnimalSpeciesDecisions
 * @brief Represents the decision-making traits of a species across different instars.
 *
 * This class manages species-level behavioral traits such as predation probability,
 * maximum voracity, sensory detection, and patch quality assessment. It handles
 * updates, calculations, and serialization of these traits.
 */
class AnimalSpeciesDecisions {
public:
    /**
     * @brief Default constructor.
     */
    AnimalSpeciesDecisions();

    /**
     * @brief Constructor from JSON configuration and species genetics.
     *
     * @param config JSON object containing configuration parameters.
     * @param genetics Genetics of the species.
     * @param numberOfInstars Number of instar stages for the species.
     * @param pdfThreshold Threshold for probability density functions.
     */
    AnimalSpeciesDecisions(const nlohmann::json& config, const AnimalSpeciesGenetics& genetics, const unsigned int numberOfInstars, const PreciseDouble& pdfThreshold);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~AnimalSpeciesDecisions();

    // Delete copy constructor
    AnimalSpeciesDecisions(const AnimalSpeciesDecisions&) = delete;

    // Delete assignment operator
	AnimalSpeciesDecisions& operator=(const AnimalSpeciesDecisions&) = delete;

    /**
     * @brief Updates the maximum voracity for a given instar.
     *
     * @param instar Instar stage.
     * @param newValue New maximum voracity.
     */
    void updateMaximumVoracity(const Instar& instar, const PreciseDouble& newValue);

    /**
     * @brief Updates the maximum PDF value for a given instar.
     *
     * @param instar Instar stage.
     * @param newValue New maximum PDF value.
     */
    void updateMaximumPDF(const Instar& instar, const PreciseDouble& newValue);

    /**
     * @brief Returns the maximum voracity for a given instar.
     *
     * @param instar Instar stage.
     * @return Maximum voracity.
     */
    const PreciseDouble& getMaximumVoracity(const Instar& instar) const;

    /**
     * @brief Returns the maximum predator interaction area for a given instar.
     *
     * @param instar Instar stage.
     * @return Maximum predator interaction area.
     */
    const PreciseDouble& getMaximumPredatorInteractionArea(const Instar& instar) const;

    /**
     * @brief Returns the maximum prey voracity for a given instar.
     *
     * @param instar Instar stage.
     * @return Maximum prey voracity.
     */
    const PreciseDouble& getMaximumPreyVoracity(const Instar& instar) const;

    /**
     * @brief Returns the kill probability.
     * @return Kill probability.
     */
    const PreciseDouble& getKillProbability() const;

    /**
     * @brief Returns the weight of individual to global assessment.
     * @return Weight factor.
     */
    const PreciseDouble& getWeightIndividualToGlobalAssessment() const;

    /**
     * @brief Returns the sensory model.
     * @return SensoryModel instance.
     */
    const SensoryModel& getSensoryModel() const;

    /**
     * @brief Calculates predation probability of a predator on a prey.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Predation probability.
     */
    PreciseDouble calculatePredationProbability(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass);

    /**
     * @brief Calculates edibility value of a prey for a predator.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Edibility value.
     */
    PreciseDouble calculateEdibilityValue(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass);
    
    /**
     * @brief Calculates the cell quality for a predator-prey interaction.
     *
     * @param predator Predator individual.
     * @param prey Prey individual.
     * @param preyDryMass Dry mass of the prey.
     * @return Cell quality value.
     */
    PreciseDouble calculateCellQuality(const AnimalNonStatistical& predator, const Edible& prey, const DryMass& preyDryMass);

    /**
     * @brief Deserializes probability functions from saved state.
     */
    void deserializeProbabilityFunctions();

    /**
     * @brief Updates the maximum predator interaction area for a given instar.
     *
     * @param instar Instar stage.
     * @param newValue New maximum value.
     */
    void updateMaximumPredatorInteractionArea(const Instar& instar, const PreciseDouble& newValue);

    /**
     * @brief Updates the maximum prey voracity for a given instar.
     *
     * @param instar Instar stage.
     * @param newValue New maximum value.
     */
    void updateMaximumPreyVoracity(const Instar& instar, const PreciseDouble& newValue);

    /**
     * @brief Returns whether the species assesses resource quality.
     * @return True if quality assessment is active.
     */
    bool getQualityResourceAssessment() const;

    /**
     * @brief Returns the maximum global patch edibility value for an instar.
     *
     * @param instar Instar stage.
     * @return Maximum global patch edibility value.
     */
    const PreciseDouble& getMaximumPatchEdibilityValueGlobal(const Instar& instar) const;

    /**
     * @brief Returns the maximum global patch predation risk for an instar.
     *
     * @param instar Instar stage.
     * @return Maximum global patch predation risk.
     */
	const PreciseDouble& getMaximumPatchPredationRiskGlobal(const Instar& instar) const;

    /**
     * @brief Returns the maximum global patch conspecific biomass for an instar.
     *
     * @param instar Instar stage.
     * @return Maximum global patch conspecific biomass.
     */
	const PreciseDouble& getMaximumPatchConspecificBiomassGlobal(const Instar& instar) const;

    /**
     * @brief Sets the maximum global patch edibility value for an instar.
     *
     * @param instar Instar stage.
     * @param newValue New maximum value.
     */
    void setMaximumPatchEdibilityValueGlobal(const Instar& instar, const PreciseDouble& newValue);

    /**
     * @brief Sets the maximum global patch predation risk for an instar.
     *
     * @param instar Instar stage.
     * @param newValue New maximum value.
     */
    void setMaximumPatchPredationRiskGlobal(const Instar& instar, const PreciseDouble& newValue);

    /**
     * @brief Sets the maximum global patch conspecific biomass for an instar.
     *
     * @param instar Instar stage.
     * @param newValue New maximum value.
     */
    void setMaximumPatchConspecificBiomassGlobal(const Instar& instar, const PreciseDouble& newValue);

	/**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    /// Sensory model for the species.
    SensoryModel sensoryModel;

    /// Predation probability functions.
    PredationProbability predationProbability;

    /// Edibility value functions.
    EdibilityValue edibilityValue;

    /// Cell quality and biomass functions.
    CellQualityBiomass cellQuality;

    /// Maximum voracity per instar.
    CustomIndexedVector<Instar, PreciseDouble> maximumVoracity;

    /// Maximum predator interaction area per instar.
    CustomIndexedVector<Instar, PreciseDouble> maximumPredatorInteractionArea;

    /// Maximum prey voracity per instar.
    CustomIndexedVector<Instar, PreciseDouble> maximumPreyVoracity;

    /// Kill probability for the species.
    PreciseDouble killProbability;

    /// Weight factor for individual to global assessment.
    PreciseDouble weightIndividualToGlobalAssessment;

    /// Maximum global patch edibility value per instar.
    CustomIndexedVector<Instar, PreciseDouble> maximumPatchEdibilityValueGlobal;

    /// Maximum global patch predation risk per instar.
    CustomIndexedVector<Instar, PreciseDouble> maximumPatchPredationRiskGlobal;

    /// Maximum global patch conspecific biomass per instar.
    CustomIndexedVector<Instar, PreciseDouble> maximumPatchConspecificBiomassGlobal;
};

#endif /* ANIMAL_SPECIES_DECISIONS_H_ */
