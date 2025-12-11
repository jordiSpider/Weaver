/**
 * @file Decisions.h
 * @brief Defines the Decisions class which handles predation, feeding, and movement decisions for animals.
 */

#ifndef DECISIONS_H_
#define DECISIONS_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/AnimalSpeciesDecisions.h"
#include "Misc/RingBuffer.h"
#include "App/Model/IBM/Landscape/LivingBeings/Species/Species.h"


class Edible;
class CellResourceInterface;
class AnimalNonStatistical;
class Landscape;


/**
 * @class Decisions
 * @brief Encapsulates the decision-making logic of an animal, including predation probabilities, feeding, and movement.
 * 
 * This class stores information about the animal's experience, preferences, and recent feeding activity.
 * It calculates probabilities of predation, edibility values, and cell quality for target prey.
 */
class Decisions
{
public:
    /**
     * @brief Default constructor.
     */
    Decisions();

    /**
     * @brief Constructs a Decisions object for a specific animal.
     * @param owner Pointer to the animal owner of this decision object.
     * @param animalSpeciesDecisions Pointer to species-specific decision rules.
     * @param timeStepsPerDay Number of time steps per day for scaling purposes.
     */
    Decisions(AnimalNonStatistical* owner, AnimalSpeciesDecisions* animalSpeciesDecisions, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Destructor.
     */
    virtual ~Decisions();

    /// Deleted copy constructor
    Decisions(const Decisions&) = delete;

    /// Deleted copy assignment
	Decisions& operator=(const Decisions&) = delete;

    /**
     * @brief Sets the species-specific decision rules for the animal.
     * @param newAnimalSpeciesDecisions Pointer to new species decision rules.
     */
    void setAnimalSpeciesDecisions(AnimalSpeciesDecisions* newAnimalSpeciesDecisions);

    /**
     * @brief Sets the animal owner of this Decisions object.
     * @param newOwner Pointer to the animal owner.
     */
    void setOwner(AnimalNonStatistical* newOwner);

    /**
     * @brief Calculates predation probability for a prey animal.
     * @param prey Reference to the prey animal.
     * @return Predation probability as PreciseDouble.
     */
    PreciseDouble calculatePredationProbability(const AnimalNonStatistical& prey);

    /**
     * @brief Calculates predation probability for an edible resource.
     * @param prey Reference to the Edible object.
     * @param preyDryMass Dry mass of the prey.
     * @return Predation probability as PreciseDouble.
     */
    PreciseDouble calculatePredationProbability(const Edible& prey, const DryMass& preyDryMass);

    /**
     * @brief Calculates the predation risk of being predated by a specific predator.
     * @param predator Reference to the predator animal.
     * @return Predation risk as PreciseDouble.
     */
    PreciseDouble calculatePredationRisk(AnimalNonStatistical& predator) const;

    /**
     * @brief Calculates edibility value for a prey animal.
     * @param prey Reference to the prey animal.
     * @return Edibility value as PreciseDouble.
     */
    PreciseDouble calculateEdibilityValue(const AnimalNonStatistical& prey);

    /**
     * @brief Calculates edibility value for a resource cell.
     * @param prey Reference to the resource cell.
     * @param preyDryMass Dry mass of the resource.
     * @return Edibility value as PreciseDouble.
     */
    PreciseDouble calculateEdibilityValue(const CellResourceInterface& prey, const DryMass& preyDryMass);

    /**
     * @brief Calculates the quality of a cell for a prey animal.
     * @param prey Reference to the prey animal.
     * @return Cell quality as PreciseDouble.
     */
    PreciseDouble calculateCellQuality(const AnimalNonStatistical& prey);

    /**
     * @brief Calculates the quality of a resource cell.
     * @param prey Reference to the resource cell.
     * @param preyDryMass Dry mass of the resource.
     * @return Cell quality as PreciseDouble.
     */
    PreciseDouble calculateCellQuality(const CellResourceInterface& prey, const DryMass& preyDryMass);

    /**
     * @brief Calculates the biomass of conspecific animals in the vicinity.
     * @param otherAnimal Reference to another animal of the same species.
     * @return Dry mass of conspecific biomass.
     */
    DryMass calculateConspecificBiomass(const AnimalNonStatistical& otherAnimal) const;

    /**
     * @brief Checks if the last assimilation efficiency is lower than the mean.
     * @return True if lower than mean, false otherwise.
     */
    bool isLastAssimilationEfficiencyLowerThanMean() const;

    /**
     * @brief Checks if the last cumulative predation probability is higher than the mean.
     * @return True if higher than mean, false otherwise.
     */
    bool isLastCumulativePredationProbabilityUpperThanMean() const;

    /**
     * @brief Updates ingestion data for the current time step.
     * @param foodMassAssimilatedCurrentTimeStepPerSpecies Assimilated food mass per species/instar.
     * @param foodMassAssimilatedCurrentTimeStep Total food mass assimilated.
     * @param foodMassEatenCurrentTimeStepPerSpecies Eaten food mass per species/instar.
     * @param foodMassEatenCurrentTimeStep Total food mass eaten.
     */
    void setLastIngestion(
        const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, DryMass>>& foodMassAssimilatedCurrentTimeStepPerSpecies, 
        const DryMass& foodMassAssimilatedCurrentTimeStep,
	    const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, DryMass>>& foodMassEatenCurrentTimeStepPerSpecies, 
        const DryMass& foodMassEatenCurrentTimeStep
    );

    /**
     * @brief Stores the last cumulative predation probability for the animal.
     */
    void setLastCumulativePredationProbability();

    /**
     * @brief Adds a predation probability to the cumulative buffer.
     * @param predationProbability Predation probability to add.
     */
    void addToCumulativePredationProbability(const PreciseDouble& predationProbability);

    /**
     * @brief Sets a new movement destination for the animal based on its preferences and environment.
     */
    void setNewDestination();

    /**
     * @brief Attempts to eat edibles in the landscape according to the animal's decisions.
     * @param landscape Pointer to the landscape.
     * @param ediblesByEdibility Vector of tuples (edibility value, Edible*, dry mass) sorted by preference.
     * @param numberOfTimeSteps Number of time steps.
     * @param timeStepsPerDay Number of time steps per day.
     * @param animalsHasTriedToPredate List of animals that have already been targeted.
     * @param saveAnimalsEachDayPredationProbabilities Flag to save daily predation probabilities.
     * @param predationProbabilitiesContent Stream to store predation probability data.
     * @param saveActivity Flag to save activity data.
     * @param activityContent Stream to store activity information.
     * @param competitionAmongResourceSpecies Flag indicating competition among resource species.
     */
    void tryToEatEdible(Landscape* const landscape, std::vector<std::tuple<PreciseDouble, Edible*, DryMass>>& ediblesByEdibility, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate, const bool saveAnimalsEachDayPredationProbabilities, std::ostringstream& predationProbabilitiesContent, const bool saveActivity, std::ostringstream& activityContent, const bool competitionAmongResourceSpecies);

    /**
     * @brief Returns the preference value for a given species and instar.
     * @param preySpeciesId ID of the prey species.
     * @param preyInstar Instar of the prey.
     * @return Preference value.
     */
    const PreciseDouble& getPreference(const Species::ID &preySpeciesId, const Instar &preyInstar) const;

    /**
     * @brief Returns the mean experience for a given species and instar.
     * @param preySpeciesId ID of the prey species.
     * @param preyInstar Instar of the prey.
     * @return Mean experience value.
     */
    PreciseDouble getMeanExperience(const Species::ID &preySpeciesId, const Instar &preyInstar) const;

    /**
     * @brief Returns the maximum edibility value of the patch for this animal.
     */
    PreciseDouble getMaximumPatchEdibilityValue() const;

    /**
     * @brief Returns the maximum predation risk in the patch for this animal.
     */
	PreciseDouble getMaximumPatchPredationRisk() const;

    /**
     * @brief Returns the maximum conspecific biomass in the patch for this animal.
     */
	PreciseDouble getMaximumPatchConspecificBiomass() const;

    /**
     * @brief Sets the maximum edibility value for the individual.
     */
    void setMaximumPatchEdibilityValueIndividual(const PreciseDouble& newValue);

    /**
     * @brief Sets the maximum predation risk for the individual.
     */
    void setMaximumPatchPredationRiskIndividual(const PreciseDouble& newValue);

    /**
     * @brief Sets the maximum conspecific biomass for the individual.
     */
    void setMaximumPatchConspecificBiomassIndividual(const PreciseDouble& newValue);

    /**
     * @brief Updates variables associated with the current instar.
     */
    void updateVariablesAssociatedWithInstar();
    
    /**
     * @brief Adds a new species to the decision-making system.
     * @param numberOfInstars Number of instars for the species.
     * @param timeStepsPerDay Number of time steps per day.
     */
    void addSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay);

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
    AnimalNonStatistical* owner; /**< Pointer to the animal owning this Decisions object. */
    AnimalSpeciesDecisions* animalSpeciesDecisions; /**< Pointer to species-specific decision rules. */

    RingBuffer<PreciseDouble> assimilationEfficiencyBuffer; /**< Circular buffer of recent assimilation efficiencies. */
    PreciseDouble lastAssimilationEfficiency; /**< Last assimilation efficiency value. */

    RingBuffer<PreciseDouble> cumulativePredationProbabilityBuffer; /**< Circular buffer of cumulative predation probabilities. */
    PreciseDouble maximumCumulativePredationProbabilityCurrentTimeStep; /**< Maximum cumulative predation probability at current time step. */
    PreciseDouble lastCumulativePredationProbability; /**< Last cumulative predation probability. */

    CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, RingBuffer<PreciseDouble>>> experienceBuffer; /**< Experience buffers per species and instar. */
    CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, PreciseDouble>> preferences; /**< Preferences per species and instar. */

    /**
     * @brief Maximum edibility value of the patch for the individual animal.
     * 
     * This value represents the highest edibility observed in the surrounding patch
     * for the animal based on its preferences and experience. It is updated every time
     * the animal evaluates its environment.
     */
    PreciseDouble maximumPatchEdibilityValueIndividual;

    /**
     * @brief Maximum predation risk of the patch for the individual animal.
     * 
     * This value represents the highest predation risk observed in the surrounding patch
     * for the animal. It helps to assess safety when choosing feeding or movement targets.
     */
    PreciseDouble maximumPatchPredationRiskIndividual;

    /**
     * @brief Maximum conspecific biomass in the patch for the individual animal.
     * 
     * Represents the maximum biomass of same-species animals observed in the patch,
     * which can affect competition for resources and social interactions.
     */
    PreciseDouble maximumPatchConspecificBiomassIndividual;


    /**
     * @brief Returns the maximum edibility value of the patch for the individual.
     * @return Constant reference to the maximum patch edibility value.
     */
    const PreciseDouble& getMaximumPatchEdibilityValueIndividual() const;

    /**
     * @brief Returns the maximum predation risk in the patch for the individual.
     * @return Constant reference to the maximum patch predation risk.
     */
	const PreciseDouble& getMaximumPatchPredationRiskIndividual() const;

    /**
     * @brief Returns the maximum conspecific biomass in the patch for the individual.
     * @return Constant reference to the maximum patch conspecific biomass.
     */
	const PreciseDouble& getMaximumPatchConspecificBiomassIndividual() const;

    /**
     * @brief Resets all individual maximum patch values to initial state.
     * 
     * Typically called before evaluating a new environment or time step to ensure
     * that maximum values reflect the current state and observations.
     */
    void resetMaximumPatchValuesIndividual();

    /**
     * @brief Updates the animal's feeding and movement preferences.
     * 
     * Preferences are updated based on the experience buffer, assimilation efficiency,
     * cumulative predation probability, and other ecological factors.
     */
    void updatePreferences();

    /**
     * @brief Initializes the animal's preferences at the start of simulation or after reset.
     * @param timeStepsPerDay Number of time steps per day to scale initial preferences.
     */
    void setInitialPreferences(const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Updates preferences based on the most recent feeding events.
     * 
     * This method incorporates the food mass assimilated and eaten during the current
     * time step for each species and instar, adjusting the animal's preferences accordingly.
     * 
     * @param foodMassAssimilatedCurrentTimeStepPerSpecies Assimilated food mass per species and instar.
     * @param foodMassAssimilatedCurrentTimeStep Total food mass assimilated in this time step.
     * @param foodMassEatenCurrentTimeStepPerSpecies Eaten food mass per species and instar.
     * @param foodMassEatenCurrentTimeStep Total food mass eaten in this time step.
     */
    void updatePreferences(
        const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, DryMass>>& foodMassAssimilatedCurrentTimeStepPerSpecies, 
        const DryMass& foodMassAssimilatedCurrentTimeStep,
	    const CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, DryMass>>& foodMassEatenCurrentTimeStepPerSpecies, 
        const DryMass& foodMassEatenCurrentTimeStep
    );
};

#endif // DECISIONS_H_
