/**
 * @file AnimalNonStatisticalGrowth.h
 * @brief Defines the AnimalNonStatisticalGrowth class, which extends AnimalGrowth for non-statistical animal simulations.
 */

#ifndef ANIMAL_NON_STATISTICAL_GROWTH_MODULE_H_
#define ANIMAL_NON_STATISTICAL_GROWTH_MODULE_H_


#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/AnimalGrowth.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/SexualType.h"



class Landscape;
class AnimalNonStatistical;


/**
 * @class AnimalNonStatisticalGrowth
 * @brief Represents the growth dynamics of individual animals in a non-statistical simulation.
 * 
 * This class handles growth, maturation, reproduction, molting, diapause, and energy tank management
 * for individual animals. It extends AnimalGrowth and adds functionality specific to non-statistical simulations.
 */
class AnimalNonStatisticalGrowth : public AnimalGrowth {
public:
    /**
     * @brief Default constructor.
     */
    AnimalNonStatisticalGrowth();

    /**
     * @brief Constructor initializing all relevant growth parameters.
     * 
     * @param speciesGrowth Pointer to species-specific growth information.
     * @param instar Initial instar.
     * @param temperatureDependent Whether growth is temperature-dependent.
     * @param actualTimeSteps Current time step of the simulation.
     * @param gender Gender of the individual.
     * @param baseIndividualTraits Traits used for growth calculations.
     * @param factorEggMassValue Factor to scale egg mass.
     * @param femaleMaxReproductionEvents Maximum reproduction events for females.
     * @param actualTemperature Current temperature of the environment.
     * @param tempFromLab Laboratory reference temperature.
     * @param timeStepsPerDay Number of simulation time steps per day.
     */
    AnimalNonStatisticalGrowth(AnimalSpeciesGrowth* const speciesGrowth, const Instar &instar, const bool temperatureDependent, const TimeStep& actualTimeSteps, Gender gender, const std::vector<IndividualTrait>& baseIndividualTraits, const PreciseDouble& factorEggMassValue, const unsigned int femaleMaxReproductionEvents, const Temperature& actualTemperature, const Temperature& tempFromLab, const PreciseDouble& timeStepsPerDay);
    
    /**
     * @brief Destructor.
     */
    virtual ~AnimalNonStatisticalGrowth();

    // Delete copy constructor 
    AnimalNonStatisticalGrowth(const AnimalNonStatisticalGrowth&) = delete;

    // Delete  assignment operator
    AnimalNonStatisticalGrowth& operator=(const AnimalNonStatisticalGrowth&) = delete;

    /**
     * @brief Tunes growth parameters based on individual traits, reproduction events, and temperature.
     */
    void tune(const std::vector<IndividualTrait>& baseIndividualTraits, const TimeStep actualTimeStep, const unsigned int femaleMaxReproductionEvents, const Temperature& actualTemperature, const Temperature& tempFromLab, const PreciseDouble &timeStepsPerDay);

    /**
     * @brief Gets the complete vector of lengths for each instar.
     * 
     * @return Constant reference to the indexed vector of lengths per instar.
     */
    const CustomIndexedVector<Instar, Length>& getInstarLengthsVector() const;

    /**
     * @brief Gets the length corresponding to a specific instar.
     * 
     * @param instarValue The instar for which the length is requested.
     * @return Constant reference to the length of the specified instar.
     */
    const Length& getInstarLength(const Instar& instarValue) const;
    
    /**
     * @brief Gets the complete vector of body masses for each instar.
     * 
     * @return Constant reference to the indexed vector of masses per instar.
     */
    const CustomIndexedVector<Instar, DryMass>& getInstarMassesVector() const;

    /**
     * @brief Gets the body mass corresponding to a specific instar.
     * 
     * @param instarValue The instar for which the body mass is requested.
     * @return Constant reference to the mass of the specified instar.
     */
    const DryMass& getInstarMass(const Instar& instarValue) const;

    /**
     * @brief Calculates the age of first maturation after pupation.
     */
    const TimeStep getAgeOfFirstMaturation(const TimeStep& pupaPeriodTime) const;

    /**
     * @brief Executes the growth step for the animal.
     * 
     * @return A pair of status flags and life stage information.
     */
    std::pair<std::pair<bool, TimeStep>, std::pair<bool, LifeStage>> grow(Landscape* const landscape, AnimalNonStatistical* animal, Gender gender, const TimeStep& pupaPeriodTime, const unsigned int reproCounter, const unsigned int femaleMaxReproductionEvents, const PreciseDouble& growthValue, const PreciseDouble& totFec, const SexualType sexualType, const bool isMated, const TimeStep actualTimeStep, const PreciseDouble& energyTankFinalTraitValue, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Checks if the animal can resume growth from pupation or needs to decrease pupation timer.
     */
    bool isReadyToResumeFromPupaOrDecreasePupaTimer();

    /**
     * @brief Checks if the animal can resume from diapause or needs to increase diapause timer.
     * 
     * @param relativeHumidity Current relative humidity.
     * @param minRelativeHumidityThreshold Minimum relative humidity threshold for growth.
     * @return Pair of booleans indicating readiness.
     */
    std::pair<bool, bool> isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(const PreciseDouble& relativeHumidity, const PreciseDouble& minRelativeHumidityThreshold);

    /**
     * @brief Accessor for diapause time steps.
     */
    const TimeStep getDiapauseTimeSteps() const;

    /**
     * @brief Accessor for animal longevity.
     */
    const TimeStep& getLongevity() const;

    /**
     * @brief Calculates the previous target body mass for reproduction or moulting.
     * 
     * @param gender The gender of the animal.
     * @param reproCounter The reproduction event number.
     * @return The previous target mass as a DryMass object.
     */
    DryMass calculatePreviousTarget(const Gender gender, const unsigned int reproCounter) const;

    /**
     * @brief Calculates the next target body mass for reproduction or moulting.
     * 
     * @param gender The gender of the animal.
     * @return The next target mass as a DryMass object.
     */
    DryMass calculateNextTarget(const Gender gender) const;

    /**
     * @brief Calculates the total mass load for the animal considering its gender and reproduction status.
     * 
     * @param gender The gender of the animal.
     * @param reproCounter The reproduction event number.
     * @return The calculated mass load as a DryMass object.
     */
    DryMass calculateMassLoad(const Gender gender, const unsigned int reproCounter) const;

    /**
     * @brief Forces animals to molt to a specified instar at the beginning of the simulation.
     */
    std::pair<TimeStep, std::pair<bool, LifeStage>> forceMolting(const std::vector<IndividualTrait>& baseIndividualTraits, const TimeStep actualTimeStep, Gender gender, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Forces reproduction and updates reproduction-related mass targets.
     */
    void forceReproduction(const unsigned int reproCounter, const unsigned int femaleMaxReproductionEvents, const TimeStep& timeOfReproEvent, const TimeStep ageOfLastMoultOrReproduction, const TimeStep& pupaPeriodTime, const PreciseDouble& factorEggMassValue, const TimeStep actualTimeStep, const PreciseDouble& growthValue, const PreciseDouble& totFec, const bool lastEventIsMoult, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Determines whether the animal uses capital breeding.
     */
    bool hasCapitalBreeding() const;

    /**
     * @brief Disables capital breeding for the individual.
     */
	void disableCapitalBreeding();

    /**
     * @brief Gets the predicted mass that the animal should reach for its next reproduction event.
     * 
     * @return A constant reference to the DryMass representing the mass for the next reproduction.
     */
    const DryMass& getMassForNextReproduction() const;

    /**
     * @brief Sets the predicted mass that the animal should reach for its next reproduction event.
     * 
     * @param newMassForNextReproduction The new DryMass value to set for the next reproduction.
     */
    void setMassForNextReproduction(const DryMass& newMassForNextReproduction);

    /**
     * @brief Computes the egg batch number based on scaling factors.
     */
    unsigned int computeEggBatchNumber(const PreciseDouble& factorEggMassValue) const;

    /**
     * @brief Accessor for the date of egg laying.
     */
    const Day& getDateEgg() const;

    /**
     * @brief Calculates predation and growth-related mass changes.
     */
    const DryMass calculatePredVor(const DryMass& basalMetabolicDryMassLoss, const PreciseDouble& assim, const DryMass& previousNetGrowth, const TimeStep numberOfTimeSteps);

    /**
     * @brief Gets the total reproductive mass accumulated by the animal.
     * 
     * @return A constant reference to the DryMass representing the total reproductive mass.
     */
    const DryMass& getTotalReproMass() const;

    /**
     * @brief Gets the current energy tank mass of the animal during growth.
     * 
     * @return A constant reference to the DryMass representing the energy tank at growth.
     */
    const DryMass& getTankAtGrowth() const;

    /**
     * @brief Gets the target mass the animal had for its previous reproduction event.
     * 
     * @return A constant reference to the DryMass representing the previous reproduction mass target.
     */
    const DryMass& getPreviousReproMassTarget() const;

    /**
     * @brief Gets the target mass the animal had for its previous moulting event.
     * 
     * @return A constant reference to the DryMass representing the previous moulting mass target.
     */
    const DryMass& getPreviousMoultingMassTarget() const;

    /**
     * @brief Overrides AnimalGrowth to set the mature status.
     */
    void setMatureValue(const bool isMature, const Gender& gender) override;

    /**
     * @brief Calculates growth curves based on traits, reproduction events, and time steps.
     */
    void calculateGrowthCurves(const std::vector<IndividualTrait>& baseIndividualTraits, const unsigned int femaleMaxReproductionEvents, const PreciseDouble& timeStepsPerDay) override;

    /**
     * @brief Accessor for age at next reproduction.
     */
    const TimeStep& getAgeForNextReproduction() const;

    /**
     * @brief Checks whether the animal is currently in a habitat shift phase.
     * 
     * @return True if the animal is in habitat shift, false otherwise.
     */
    bool isInHabitatShift() const;

    /**
     * @brief Sets the habitat shift status of the animal.
     * 
     * @param newInHabitatShiftValue Boolean value indicating the new habitat shift state.
     */
    void setInHabitatShift(const bool newInHabitatShiftValue);

    /**
     * @brief Gets the predicted mass of the animal for its next growth or reproduction event.
     * 
     * @return A constant reference to the DryMass representing the next predicted mass.
     */
    const DryMass& getNextMassPredicted() const;

    /**
     * @brief Calculates the predicted mass for the animal's next growth or reproduction event.
     * 
     * @param reproCounter The current reproduction event counter.
     * @param maxReproductionEvents The maximum number of reproduction events allowed.
     * @param growthValue The growth rate or factor to apply.
     * @param gender The gender of the animal.
     * @param timeStepsPerDay The number of simulation time steps per day.
     */
    void calculateNextMassPredicted(const unsigned int reproCounter, const unsigned int maxReproductionEvents, const PreciseDouble& growthValue, const Gender gender, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Gets the previously calculated predicted mass for the next growth or reproduction event.
     * 
     * @return A constant reference to the DryMass representing the previous next predicted mass.
     */
    const DryMass& getPreviousNextMassPredicted() const;

    /**
     * @brief Sets the animal's instar and updates relevant growth parameters.
     */
    void setInstar(Landscape* const landscape, const Instar& newInstar, AnimalNonStatistical* animal, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Calculates targets for the next reproduction event after maturation.
     */
    void calculateMatureNextReproductionTargets(const TimeStep& pupaPeriodTime, const unsigned int femaleMaxReproductionEvents, const unsigned int reproCounter, const PreciseDouble& growthValue, const PreciseDouble& totFec, const bool updatePreviousReproMassTarget, const PreciseDouble& timeStepsPerDay);
    
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
    TimeStep longevity; /**< Total lifespan in time steps. */
    TimeStep pupaTimer; /**< Timer for pupation stage. */
    TimeStep diapauseTimeSteps; /**< Duration of diapause in time steps. */
    DryMass massForNextReproduction; /**< Predicted mass at next reproduction. */
    DryMass lastMassBeforeMaturationOrOviposition; /**< Last body mass before maturation or egg-laying. */
    DryMass clutchDryMass; /**< Dry mass of current clutch of eggs. */
    DryMass tankAtGrowth; /**< Energy tank during growth. */
    DryMass nextMassPredicted; /**< Predicted body mass for next step. */
    DryMass previousNextMassPredicted; /**< Previously predicted body mass. */
    bool calculatedFirstReproductionTargets; /**< Flag indicating whether first reproduction targets were calculated. */
    bool calculatedMassAtMaturation; /**< Flag indicating whether mass at maturation was calculated. */
    DryMass massAtMaturation; /**< Body mass at maturation. */
    std::vector<TimeStep> actualMoltingTimeVector; /**< Time steps when molting occurred. */
    std::vector<DryMass> actualMoltingMassVector; /**< Masses at each molting event. */
    Day dateEgg; /**< Date of egg-laying. */
    DryMass totalReproMass; /**< Total reproductive mass accumulated. */
    bool capitalBreeding; /**< Whether animal uses capital breeding strategy. */
    TimeStep ageForNextReproduction; /**< Age for next reproduction event. */
    bool inHabitatShift; /**< Whether the animal is in habitat shift. */
    DryMass previousReproMassTarget; /**< Previous reproductive mass target. */
    DryMass previousMoultingMassTarget; /**< Previous moulting mass target. */


    /**
     * @brief Computes longevity based on growth parameters and pupation period.
     */
    void calculateLongevity(const PreciseDouble& longevitySinceMaturationValue, const TimeStep& pupaPeriodTime);

    /**
     * @brief Computes total reproductive mass based on growth and fecundity.
     */
    void calculateTotalReproMass();

    /**
     * @brief Gets the last recorded mass of the animal before maturation or oviposition.
     * 
     * @return A constant reference to the DryMass representing the last mass before maturation or oviposition.
     */
    const DryMass& getLastMassBeforeMaturationOrOviposition() const;

    /**
     * @brief Sets the last recorded mass of the animal before maturation or oviposition.
     * 
     * @param newLastMassBeforeMaturationOrOviposition The new DryMass value to set.
     */
    void setLastMassBeforeMaturationOrOviposition(const DryMass& newLastMassBeforeMaturationOrOviposition);

    /**
     * @brief Calculates the predicted mass for the next growth or reproduction event using the given parameters.
     * 
     * @param dryMass Current dry mass of the animal.
     * @param age Current age of the animal.
     * @param reproCounter Current reproduction event counter.
     * @param maxReproductionEvents Maximum number of reproduction events.
     * @param growthValue Growth factor or rate to apply.
     * @param gender Gender of the animal.
     * @param timeStepsPerDay Number of simulation time steps per day.
     */
    void calculateNextMassPredicted(const DryMass& dryMass, const TimeStep& age, const unsigned int reproCounter, const unsigned int maxReproductionEvents, const PreciseDouble& growthValue, const Gender gender, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Checks whether the animal has met its reproduction targets.
     * 
     * @param gender Gender of the animal.
     * @param sexualType Sexual type of the animal.
     * @param isMated Boolean indicating if the animal is currently mated.
     * @return A pair containing:
     *   - first: a pair of (bool, TimeStep) indicating whether reproduction targets are met and relevant time information.
     *   - second: a pair of (bool, LifeStage) indicating success and current life stage.
     */
    std::pair<std::pair<bool, TimeStep>, std::pair<bool, LifeStage>> checkReproductionTargets(Gender gender, const SexualType sexualType, const bool isMated);

    /**
     * @brief Checks if the animal has met its reproduction targets.
     * 
     * @return True if reproduction targets are met, false otherwise.
     */
    bool meetReproductionTargets() const;

    /**
     * @brief Checks if the animal has met its molting targets.
     * 
     * @return True if molting targets are met, false otherwise.
     */
    bool meetMoltingTargets() const;

    /**
     * @brief Applies the molting process to the animal.
     * 
     * @param actualTimeStep The current simulation time step.
     * @param energyTankFinalTraitValue The final energy tank value used for growth calculations.
     * @param gender Gender of the animal.
     */
    void applyMoulting(const TimeStep actualTimeStep, const PreciseDouble& energyTankFinalTraitValue, Gender gender);

    /**
     * @brief Calculates the clutch dry mass based on the current individual dry mass.
     * 
     * @param individualDryMass The dry mass of the individual.
     */
    void calculateClutchDryMass(const DryMass& individualDryMass);

    /**
     * @brief Calculates the clutch dry mass based on the number of current eggs.
     * 
     * @param currentEggs The number of eggs currently present.
     */
    void calculateClutchDryMass(const PreciseDouble& currentEggs);

    /**
     * @brief Gets the dry mass of the current clutch of eggs.
     * 
     * @return A constant reference to the DryMass representing the clutch dry mass.
     */
    const DryMass& getClutchDryMass() const;

    /**
     * @brief Sets the date of egg laying.
     * 
     * @param newDateEgg The new Day value representing the date of egg laying.
     */
    void setDateEgg(Day newDateEgg);

    /**
     * @brief Moves the animal to the next instar stage.
     * 
     * @param landscape Pointer to the landscape object.
     * @param animal Pointer to the animal object.
     * @param timeStepsPerDay Number of simulation time steps per day.
     */
    void moveOnNextInstar(Landscape* const landscape, AnimalNonStatistical* animal, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Sets the age at which the animal will perform its next reproduction event.
     * 
     * @param newAgeForNextReproduction The TimeStep value for the next reproduction age.
     */
    void setAgeForNextReproduction(const TimeStep& newAgeForNextReproduction);
};

#endif /* ANIMAL_NON_STATISTICAL_GROWTH_MODULE_H_ */
