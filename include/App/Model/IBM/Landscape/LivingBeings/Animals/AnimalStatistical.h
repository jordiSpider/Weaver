/**
 * @file AnimalStatistical.h
 * @brief Defines the AnimalStatistical class, a concrete subclass of Animal for statistical simulations.
 *
 * AnimalStatistical implements the abstract Animal class. It provides concrete
 * implementations for growth, voracity, life stage transitions, predation, and
 * evaluation of terrain cells.
 */

#ifndef ANIMAL_STATISTICAL_H_
#define ANIMAL_STATISTICAL_H_


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/AnimalStatisticalGrowth.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Animal.h"


/**
 * @class AnimalStatistical
 * @brief Concrete implementation of Animal for statistical modeling.
 *
 * This class extends Animal to provide a statistical model for animal behavior,
 * including growth, predation, and life stage transitions. It contains a specific
 * growth building block, AnimalStatisticalGrowth, and implements all pure virtual
 * methods from Animal.
 */
class AnimalStatistical : public Animal
{
public:
    /**
     * @brief Constructor for statistical animal.
     * @param landscape Pointer to the landscape where the animal exists.
     * @param instar The current instar (developmental stage) of the animal.
     * @param mySpecies Pointer to the animal's species.
     * @param terrainCell Pointer to the terrain cell occupied by the animal.
     * @param timeStepsPerDay Number of time steps per day in the simulation.
     */
    AnimalStatistical(Landscape* const landscape, const Instar &instar, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Destructor.
     */
    virtual ~AnimalStatistical();

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    AnimalStatistical(const AnimalStatistical&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent assignment.
     */
	AnimalStatistical& operator=(const AnimalStatistical&) = delete;

    /**
     * @brief Get the growth building block (const).
     * @return Const reference to AnimalStatisticalGrowth.
     */
    const AnimalStatisticalGrowth& getGrowthBuildingBlock() const override;

    /**
     * @brief Get the growth building block (mutable).
     * @return Reference to AnimalStatisticalGrowth.
     */
	AnimalStatisticalGrowth& getMutableGrowthBuildingBlock() override;

    /**
     * @brief Add animal species to the simulation.
     * @param numberOfInstars Number of instars.
     * @param timeStepsPerDay Number of time steps per day.
     */
    void addAnimalSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Add resource species to the simulation.
     * @param numberOfInstars Number of instars.
     * @param timeStepsPerDay Number of time steps per day.
     */
    void addResourceSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Get the instar to evaluate terrain cells.
     * @return Const reference to Instar.
     */
    const Instar& getInstarToEvaluateCells() const;

    /**
     * @brief Increment the count of predation encounters for the animal.
     */
    void increasePredationEncounters();

    /**
     * @brief Check if the animal is currently hunting.
     * @return True if hunting, false otherwise.
     */
    bool isHunting() const;

    /**
     * @brief Set a new life stage for the animal.
     * @param landscape Pointer to the landscape.
     * @param newLifeStage The new life stage.
     */
    void setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage);

    /**
     * @brief Set a new life stage for the animal with a given number of time steps.
     * @param landscape Pointer to the landscape.
     * @param newLifeStage The new life stage.
     * @param numberOfTimeSteps Number of time steps for the transition.
     * @param timeStepsPerDay Number of time steps per day.
     */
	void setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay);
	
    /**
     * @brief Set a new life stage for the animal considering predation.
     * @param landscape Pointer to the landscape.
     * @param newLifeStage The new life stage.
     * @param numberOfTimeSteps Number of time steps for the transition.
     * @param predatorId ID of the predator.
     * @param timeStepsPerDay Number of time steps per day.
     */
    void setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage, const TimeStep numberOfTimeSteps, EdibleID predatorId, const PreciseDouble& timeStepsPerDay);
    
    /**
     * @brief Get the remaining voracity of the animal.
     * @return Remaining voracity as PreciseDouble.
     */
    PreciseDouble getRemainingVoracity() const;
    
    /**
     * @brief Reset control variables used for statistical computations.
     */
    void resetControlVariables();
    
    /**
     * @brief Attempt to predate on another edible object.
     * @param landscape Pointer to the landscape.
     * @param edibleToBePredated Reference to the edible target.
     * @param targetDryMass Dry mass of the target.
     * @param sourcePosition Position of the predator.
     * @param radius Predation radius.
     * @param numberOfTimeSteps Number of simulation time steps.
     * @param timeStepsPerDay Number of time steps per day.
     * @param retaliation Whether the prey can retaliate.
     * @param animalsHasTriedToPredate List of animals that already attempted predation.
     * @param predationProbabilitiesContent Stream to log predation probabilities.
     * @param activityContent Stream to log activity information.
     * @param predationSpeedRatioAH Parameters for activity-hunting phase.
     * @param predationHunterVoracityAH Parameters for activity-hunting phase.
     * @param predationProbabilityDensityFunctionAH Parameters for activity-hunting phase.
     * @param predationSpeedRatioSAW Parameters for searching-activity phase.
     * @param predationHunterVoracitySAW Parameters for searching-activity phase.
     * @param predationProbabilityDensityFunctionSAW Parameters for searching-activity phase.
     * @param competitionAmongResourceSpecies Whether competition among resources is considered.
     * @return Pair containing success flag and dry mass consumed.
     */
    std::pair<bool, DryMass> predateEdible(Landscape* const landscape, Edible &edibleToBePredated, const DryMass &targetDryMass, const PointContinuous &sourcePosition, const PreciseDouble &radius, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, bool retaliation, std::list<const Edible*> &animalsHasTriedToPredate, std::ostringstream& predationProbabilitiesContent, std::ostringstream& activityContent, const PreciseDouble& predationSpeedRatioAH, const PreciseDouble& predationHunterVoracityAH, const PreciseDouble& predationProbabilityDensityFunctionAH, const PreciseDouble& predationSpeedRatioSAW, const PreciseDouble& predationHunterVoracitySAW, const PreciseDouble& predationProbabilityDensityFunctionSAW, const bool competitionAmongResourceSpecies);
    
    /**
     * @brief Compute the dry mass obtained based on target and predator voracity.
     * @param targetDryMass Dry mass of the target.
     * @param predatorRemainingVoracity Remaining voracity of the predator.
     * @return Dry mass actually obtained by the predator.
     */
    DryMass turnIntoDryMass(const DryMass &targetDryMass, const PreciseDouble& predatorRemainingVoracity) const;

protected:
    AnimalStatisticalGrowth growthBuildingBlock; /**< Growth building block for statistical modeling */

    /**
     * @brief Set the growth parameters from a new species growth object.
     * @param newSpeciesGrowth Pointer to the new species growth data.
     */
    void setSpeciesGrowth(SpeciesGrowth* newSpeciesGrowth);
};

#endif // ANIMAL_STATISTICAL_H_
