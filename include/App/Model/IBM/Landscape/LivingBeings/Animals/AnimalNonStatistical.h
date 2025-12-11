/**
 * @file AnimalNonStatistical.h
 * @brief Defines the AnimalNonStatistical class, a non-statistical concrete implementation of Animal.
 *
 * AnimalNonStatistical represents individual animals with explicit simulation of
 * their behavior, movement, predation, reproduction, and energy dynamics in the landscape.
 */

#ifndef ANIMAL_NON_STATISTICAL_H_
#define ANIMAL_NON_STATISTICAL_H_


#include <cmath>
#include <vector>
#include <list>
#include <tuple>
#include <ostream>


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/list.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Animal.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Gamete.h"
#include "App/Model/IBM/Maths/MathFunctions.h"
#include "App/Model/IBM/Maths/Random.h"
#include "App/Model/IBM/Landscape/Map/Points/PointMap.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/AnimalNonStatisticalGrowth.h"
#include "Misc/EnumClass.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Generation.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/HuntingMode.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/SexualType.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Decisions/Decisions.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Prey.h"


class TerrainCell;


/**
 * @class AnimalNonStatistical
 * @brief Concrete subclass of Animal implementing detailed individual-based behavior.
 *
 * This class models each animal explicitly rather than statistically. It handles
 * predation, movement, breeding, growth, metabolism, life stage transitions, and
 * energy assimilation, tracking detailed attributes such as voracity, speed,
 * predation encounters, and reproductive status.
 */
class AnimalNonStatistical : public Animal
{
public:
	/**
     * @brief Default constructor.
     */
	explicit AnimalNonStatistical();

	/**
     * @brief Constructor for a newly created animal with given instar and species.
     * @param instar Current instar (developmental stage) of the animal.
     * @param mySpecies Pointer to the animal's species.
     * @param terrainCell Pointer to the terrain cell the animal occupies.
     * @param actualTimeStep Current simulation time step.
     * @param timeStepsPerDay Number of time steps per day in the simulation.
     */
	explicit AnimalNonStatistical(const Instar &instar, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);
	
	/**
     * @brief Constructor for an animal created through sexual reproduction.
     * @param firstParentGamete Pointer to first parent's gamete.
     * @param secondParentGamete Pointer to second parent's gamete.
     * @param parentTerrainCell Terrain cell of the parent.
     * @param newFactorEggMassFromMom Scaling factor from maternal egg mass.
     * @param g_numb_prt_female Generation of female parent.
     * @param g_numb_prt_male Generation of male parent.
     * @param ID_prt_female Female parent's ID.
     * @param ID_prt_male Male parent's ID.
     * @param mySpecies Pointer to the species.
     * @param gender Gender of the offspring.
     * @param actualTimeStep Current simulation time step.
     * @param timeStepsPerDay Number of time steps per day.
     */
	explicit AnimalNonStatistical(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& newFactorEggMassFromMom, const Generation& g_numb_prt_female,
			const Generation& g_numb_prt_male, EdibleID ID_prt_female, EdibleID ID_prt_male, AnimalSpecies* const mySpecies, Gender gender, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);
	
	/**
     * @brief Destructor.
     */
	virtual ~AnimalNonStatistical();

	/**
     * @brief Deleted copy constructor to prevent copying.
     */
	AnimalNonStatistical(const AnimalNonStatistical&) = delete;

	/**
     * @brief Deleted copy assignment operator to prevent assignment.
     */
    AnimalNonStatistical& operator=(const AnimalNonStatistical&) = delete;

	/**
     * @brief Get the remaining voracity.
     * @return Remaining voracity as PreciseDouble.
     */
	PreciseDouble getRemainingVoracity() const;

	/**
     * @brief Get the current hunting mode.
     * @return Const reference to HuntingMode.
     */
	inline const HuntingMode& getHuntingMode() const { return huntingMode; }

	/**
     * @brief Check if the animal is currently hunting.
     * @return True if hunting, false otherwise.
     */
	inline virtual bool isHunting() const { return getHuntingMode() != HuntingMode::does_not_hunt; }

	/**
     * @brief Increment the count of predation encounters.
     */
	void increasePredationEncounters();

	/**
     * @brief Get the growth building block (const).
     * @return Const reference to AnimalNonStatisticalGrowth.
     */
	const AnimalNonStatisticalGrowth& getGrowthBuildingBlock() const override;

	/**
     * @brief Get the growth building block (mutable).
     * @return Reference to AnimalNonStatisticalGrowth.
     */
	AnimalNonStatisticalGrowth& getMutableGrowthBuildingBlock() override;

	/**
     * @brief Mark whether the animal is in a breeding zone.
     * @param newInBreedingZoneValue True if in breeding zone.
     */
	void setInBreedingZone(const bool newInBreedingZoneValue);

	/**
     * @brief Get mass of the animal at first reproduction.
     * @return Const reference to DryMass.
     */
	const DryMass& getMassesFirstReproduction() const;

	/**
     * @brief Get mass of the animal at last instar.
     * @return Const reference to DryMass.
     */
	const DryMass& getMassesLastInstar() const;

	/**
     * @brief Prepare mass information file header for results.
     * @param resultFolder Path to the result folder.
     * @param saveMassInfo Whether to save mass information.
     */
	void setInfoMassFileHeader(const std::filesystem::path& resultFolder, const bool saveMassInfo) const;

	/**
     * @brief Calculate total mass load of the animal.
     * @return DryMass representing the mass load.
     */
	DryMass calculateMassLoad() const;

	/**
     * @brief Get the preference for a given prey species and instar.
     * @param preySpeciesId ID of the prey species.
     * @param preyInstar Instar of the prey.
     * @return Const reference to the preference value.
     */
	const PreciseDouble& getPreference(const Species::ID &preySpeciesId, const Instar &preyInstar) const;

	/**
     * @brief Update internal variables associated with the animal's instar.
     */
	void updateVariablesAssociatedWithInstar();

	/**
     * @brief Set maximum values for evaluating a cell.
     * @param totalEdibilityValue Total edibility value of the cell.
     * @param totalPredatoryRiskEdibilityValue Total predatory risk in the cell.
     * @param totalConspecificBiomass Total biomass of conspecifics in the cell.
     */
	void setMaximumCellEvaluation(const PreciseDouble& totalEdibilityValue, const PreciseDouble& totalPredatoryRiskEdibilityValue, const PreciseDouble& totalConspecificBiomass);

	/**
     * @brief Get maximum patch edibility value.
     * @return PreciseDouble representing maximum edibility.
     */
	PreciseDouble getMaximumPatchEdibilityValue() const;

	/**
     * @brief Get maximum patch predation risk.
     * @return PreciseDouble representing predation risk.
     */
	PreciseDouble getMaximumPatchPredationRisk() const;

	/**
     * @brief Get maximum patch conspecific biomass.
     * @return PreciseDouble representing conspecific biomass.
     */
	PreciseDouble getMaximumPatchConspecificBiomass() const;

	/**
     * @brief Calculate predation risk from a predator.
     * @param predator Reference to predator.
     * @return Predation risk as PreciseDouble.
     */
	PreciseDouble calculatePredationRisk(AnimalNonStatistical& predator) const;

	/**
     * @brief Calculate predation probability for a prey.
     * @param prey Reference to the prey.
     * @return Probability as PreciseDouble.
     */
	PreciseDouble calculatePredationProbability(const AnimalNonStatistical& prey);

	/**
     * @brief Calculate biomass of conspecific animal.
     * @param otherAnimal Reference to other animal.
     * @return DryMass representing conspecific biomass.
     */
	DryMass calculateConspecificBiomass(const AnimalNonStatistical& otherAnimal) const;

	/**
     * @brief Calculate cell quality for a prey considering other animals.
     * @param prey Reference to prey.
     * @param animalsHasTriedToPredate List of animals that have already attempted predation.
     * @return Cell quality as PreciseDouble.
     */
	PreciseDouble calculateCellQuality(const AnimalNonStatistical& prey, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate);
    
	/**
     * @brief Calculate cell quality for a resource.
     * @param prey Reference to cell resource interface.
     * @param preyDryMass Dry mass of the resource.
     * @return Cell quality as PreciseDouble.
     */
	PreciseDouble calculateCellQuality(const CellResourceInterface& prey, const DryMass& preyDryMass);

	/**
     * @brief Calculate edibility value for a prey considering other animals.
     * @param prey Reference to prey.
     * @param animalsHasTriedToPredate List of animals that already tried predation.
     * @return Edibility value as PreciseDouble.
     */
	PreciseDouble calculateEdibilityValue(const AnimalNonStatistical& prey, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate);
    
	/**
     * @brief Calculate edibility value for a resource.
     * @param prey Reference to cell resource interface.
     * @param preyDryMass Dry mass of the resource.
     * @return Edibility value as PreciseDouble.
     */
	PreciseDouble calculateEdibilityValue(const CellResourceInterface& prey, const DryMass& preyDryMass);

	/**
     * @brief Print preference info for prey species and instar.
     */
	std::ostringstream printPreferenceInfo(const Species::ID &preySpeciesId, const Instar &preyInstar) const;

	/**
     * @brief Assimilate food mass from prey.
     */
	DryMass calculateAssimilatedMass(const DryMass& nonAssimilatedMass, const Species::ID& preySpeciesId, const Instar& preyInstar) const;
	
	/**
     * @brief Execute predation on an edible object.
     */
	void predateEdible(const PreciseDouble& randomProbability, const PreciseDouble& probabilityToCompare, const bool retaliation, const bool exposedAttack, Landscape* const landscape, Edible &prey, const DryMass &targetDryMass, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate, const bool saveAnimalsEachDayPredationProbabilities, std::ostringstream& predationProbabilitiesContent, const bool saveActivity, std::ostringstream& activityContent, const bool competitionAmongResourceSpecies);

	/**
     * @brief Reset control variables for a new time step.
     */
	void resetControlVariables(const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay);

	/**
     * @brief Set the ID of the predator that predated this animal.
     */
	void setPredatedByID(EdibleID predatorId);

	/**
     * @brief Add new animal species for evaluation.
     */
	void addAnimalSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay);

	/**
     * @brief Add new resource species for evaluation.
     */
	void addResourceSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay);

	/**
     * @brief Check if the animal is sated.
     */
	virtual bool isSated() const;

	/**
     * @brief Check if the animal is exhausted.
     */
	virtual bool isExhausted() const;

	/**
	 * @brief Make the animal definitive in the simulation.
	 *
	 * This method converts a temporary animal into a definitive one, assigning it a fixed unique identifier.
	 * Optionally, it can save the animal's genetic information and mass information.
	 *
	 * @param landscape Pointer to the landscape where the animal exists.
	 * @param saveGenetics If true, the animal's genetic information is saved.
	 * @param saveMassInfo If true, the animal's mass information is saved.
	 */
	void doDefinitive(Landscape* const landscape, const bool saveGenetics, const bool saveMassInfo);

	/**
	 * @brief Get the age at last moult or reproduction.
	 * @return TimeStep representing the age at last moult or reproduction.
	 */
	const TimeStep getAgeOfLastMoultOrReproduction() const;

	/**
	 * @brief Set the age at last moult or reproduction.
	 * @param newAgeOfLastMoultOrReproduction The new age to set.
	 */
	void setAgeOfLastMoultOrReproduction(const TimeStep newAgeOfLastMoultOrReproduction);

	/**
	 * @brief Check if the animal has mated.
	 * @return True if the animal has mated, false otherwise.
	 */
	virtual bool isMated() const { return mated; };

	/**
	 * @brief Get the age at first reproduction.
	 * @return Const reference to the TimeStep of first reproduction.
	 */
	const TimeStep& getAgeOfFirstReproduction() const;

	/**
	 * @brief Get the number of reproductions the animal has completed.
	 * @return Const reference to reproduction counter.
	 */
	const unsigned int& getReproCounter() const;

	/**
	 * @brief Get the fecundity of the animal.
	 * @return Const reference to fecundity.
	 */
	const unsigned int& getFecundity() const;

	/**
	 * @brief Get the generation number of the female parent.
	 * @return Const reference to Generation.
	 */
	const Generation& getGenerationNumberFromFemaleParent() const;

	/**
	 * @brief Get the generation number of the male parent.
	 * @return Const reference to Generation.
	 */
	const Generation& getGenerationNumberFromMaleParent() const;

	/**
	 * @brief Get the ID of the female parent.
	 * @return Const reference to EdibleID.
	 */
	const EdibleID& getIdFromFemaleParent() const;

	/**
	 * @brief Get the ID of the male parent.
	 * @return Const reference to EdibleID.
	 */
	const EdibleID& getIdFromMaleParent() const;

	/**
	 * @brief Set the animal to a new life stage.
	 * @param landscape Pointer to the landscape.
	 * @param newLifeStage Life stage to set.
	 */
	virtual void setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage);

	/**
	 * @brief Set the animal to a new life stage, considering time steps.
	 * @param landscape Pointer to the landscape.
	 * @param newLifeStage Life stage to set.
	 * @param numberOfTimeSteps Number of time steps to consider.
	 * @param timeStepsPerDay Time steps per day for conversion.
	 */
	virtual void setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay);
	
	/**
	 * @brief Set the animal to a new life stage, considering time steps and predator.
	 * @param landscape Pointer to the landscape.
	 * @param newLifeStage Life stage to set.
	 * @param numberOfTimeSteps Number of time steps to consider.
	 * @param predatorId ID of the predator causing the life stage change.
	 * @param timeStepsPerDay Time steps per day for conversion.
	 */
	virtual void setNewLifeStage(Landscape* const landscape, const LifeStage newLifeStage, const TimeStep numberOfTimeSteps, EdibleID predatorId, const PreciseDouble& timeStepsPerDay);
	
	/**
	 * @brief Get the date of death.
	 * @return Const Day representing date of death.
	 */
	virtual const Day getDateOfDeath() const { return dateOfDeath; };
	
	/**
	 * @brief Set the date of death.
	 * @param dateOfDeathValue The day of death to set.
	 */
	virtual void setDateOfDeath(const Day dateOfDeathValue) { dateOfDeath = dateOfDeathValue; };

	/**
	 * @brief Print the animal's traits to the provided stream.
	 * @param traitsContent Stream to print traits.
	 */
	virtual void printTraits(std::ostringstream& traitsContent);

	/**
	 * @brief Prepare the animal to be born in the simulation.
	 * @param landscape Pointer to the landscape.
	 * @param timeStepsPerDay Number of time steps per day.
	 */
	virtual void isReadyToBeBorn(Landscape* const landscape, const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Tune animal state for simulation (growth, mass, etc.).
	 * @param landscape Pointer to the landscape.
	 * @param saveMassInfo Whether to save mass info.
	 * @param numberOfTimeSteps Number of time steps for tuning.
	 * @param timeStepsPerDay Time steps per day.
	 */
	virtual void tune(Landscape* const landscape, const bool saveMassInfo, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay);
	
	/**
	 * @brief Move the animal in the landscape.
	 * @param view Pointer to view object for visualization.
	 * @param landscape Pointer to the landscape.
	 * @param numberOfTimeSteps Number of time steps to simulate movement.
	 * @param timeStepsPerDay Number of time steps per day.
	 * @param saveAnimalsEachDayPredationProbabilities Whether to save daily predation probabilities.
	 * @param predationProbabilitiesContent Stream for predation probabilities.
	 * @param saveEdibilitiesFile Whether to save edibility info.
	 * @param edibilitiesContent Stream for edibility info.
	 * @param saveActivity Whether to save activity info.
	 * @param activityContent Stream for activity info.
	 * @param saveMovements Whether to save movement info.
	 * @param movementsContent Stream for movement info.
	 * @param competitionAmongResourceSpecies Whether competition affects movement.
	 */
	virtual void moveAnimal(View* view, Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, const bool saveAnimalsEachDayPredationProbabilities, std::ostringstream& predationProbabilitiesContent, bool saveEdibilitiesFile, std::ostringstream& edibilitiesContent, const bool saveActivity, std::ostringstream& activityContent, const bool saveMovements, std::ostringstream& movementsContent, const bool competitionAmongResourceSpecies);
	
	/**
	 * @brief Print voracity values to the stream.
	 * @param landscape Pointer to landscape.
	 * @param voracitiesContent Stream to print voracity.
	 * @param timeStepsPerDay Time steps per day.
	 */
	virtual void printVoracities(const Landscape* const landscape, std::ostringstream& voracitiesContent, const PreciseDouble& timeStepsPerDay);
	
	/**
	 * @brief Handle death from background mortality.
	 * @param landscape Pointer to landscape.
	 * @param numberOfTimeSteps Number of time steps for simulation.
	 * @param timeStepsPerDay Time steps per day.
	 * @param growthAndReproTest Whether to include growth/reproduction tests.
	 */
	virtual void dieFromBackground(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, const bool growthAndReproTest);
	
	/**
	 * @brief Assimilate a given food mass.
	 * @param foodMass DryMass of food to assimilate.
	 */
	virtual void assimilateFoodMass(const DryMass& foodMass);

	/**
	 * @brief Transfer assimilated food to energy tank.
	 * @param actualTimeStep Current time step.
	 */
	virtual void transferAssimilatedFoodToEnergyTank(const TimeStep actualTimeStep);

	/**
	 * @brief Apply metabolism for the animal.
	 * @param view Pointer to view for visualization.
	 * @param landscape Pointer to landscape.
	 * @param actualTimeStep Current time step.
	 */
	virtual void metabolize(View* view, const Landscape* const landscape, const TimeStep& actualTimeStep);
	
	/**
	 * @brief Breed animals and add offspring to list.
	 * @param offspring List to append offspring.
	 * @param view Pointer to view.
	 * @param landscape Pointer to landscape.
	 * @param numberOfTimeSteps Number of time steps.
	 * @param saveGenetics Whether to save genetics.
	 * @param saveMassInfo Whether to save mass info.
	 * @param timeStepsPerDay Time steps per day.
	 */
	void breed(std::list<AnimalNonStatistical*>& offspring, View* view, Landscape* const landscape, const TimeStep numberOfTimeSteps, const bool saveGenetics, const bool saveMassInfo, const PreciseDouble& timeStepsPerDay);
	
	/**
	 * @brief Actions after breeding.
	 * @param timeStepsPerDay Number of time steps per day.
	 * @return True if post-breeding process completed.
	 */
	virtual bool postBreeding(const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Set genome from a mated male.
	 * @param matedMale Pointer to the mated male animal.
	 */
	virtual void setGenomeFromMatedMale(const AnimalNonStatistical* matedMale);

	/**
	 * @brief Get total number of predation encounters.
	 * @return Const reference to total encounters.
	 */
	const size_t& getTotalPredationEncounters() const;

	/**
	 * @brief Increase age by a given number of time steps.
	 * @param landscape Pointer to landscape.
	 * @param numberOfTimeSteps Number of time steps.
	 * @param timeStepsPerDay Time steps per day.
	 */
	void increaseAge(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay);
	
	/**
	 * @brief Check animal's energy tank and update if necessary.
	 * @param landscape Pointer to landscape.
	 * @param numberOfTimeSteps Number of time steps.
	 * @param timeStepsPerDay Time steps per day.
	 */
	void checkEnergyTank(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Check if the animal is active.
	 * @return True if active.
	 */
	bool isActive();

	/**
	 * @brief Breed asexually to produce offspring.
	 *
	 * This method generates offspring from asexual reproduction.
	 *
	 * @param offspring List where the newly created offspring will be stored.
	 * @param view Pointer to the current view for visualization.
	 * @param landscape Pointer to the landscape where the breeding occurs.
	 * @param objectiveEggsNumber Target number of eggs to produce.
	 * @param numberOfTimeSteps Number of simulation time steps for this breeding event.
	 * @param saveGenetics If true, saves the genetic information of the offspring.
	 * @param saveMassInfo If true, saves mass information of the offspring.
	 * @param timeStepsPerDay Conversion factor of time steps to days.
	 */
	void breedAsexually(std::list<AnimalNonStatistical*>& offspring, View* view, Landscape* const landscape, unsigned int objectiveEggsNumber, const TimeStep numberOfTimeSteps, const bool saveGenetics, const bool saveMassInfo, const PreciseDouble& timeStepsPerDay);
	
	/**
	 * @brief Breed sexually to produce offspring.
	 *
	 * This method generates offspring from sexual reproduction using a mated partner.
	 *
	 * @param offspring List where the newly created offspring will be stored.
	 * @param view Pointer to the current view for visualization.
	 * @param landscape Pointer to the landscape where the breeding occurs.
	 * @param objectiveEggsNumber Target number of eggs to produce.
	 * @param numberOfTimeSteps Number of simulation time steps for this breeding event.
	 * @param saveGenetics If true, saves the genetic information of the offspring.
	 * @param saveMassInfo If true, saves mass information of the offspring.
	 * @param timeStepsPerDay Conversion factor of time steps to days.
	 */
	void breedSexually(std::list<AnimalNonStatistical*>& offspring, View* view, Landscape* const landscape, unsigned int objectiveEggsNumber, const TimeStep numberOfTimeSteps, const bool saveGenetics, const bool saveMassInfo, const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Convert a portion of mass to dry mass considering predator voracity.
	 *
	 * @param targetDryMass Target dry mass to convert.
	 * @param predatorRemainingVoracity Remaining voracity of the predator.
	 * @return DryMass Resulting dry mass after conversion.
	 */
	DryMass turnIntoDryMass(const DryMass &targetDryMass, const PreciseDouble& predatorRemainingVoracity) const;

	/**
	 * @brief Update the depth of the animal in its current terrain cell.
	 *
	 * @param timeStepsPerDay Conversion factor of time steps to days.
	 */
	void updateDepth(const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Search for a suitable partner to breed.
	 *
	 * @param landscape Pointer to the landscape.
	 * @param numberOfTimeSteps Number of simulation time steps for the search.
	 * @param timeStepsPerDay Conversion factor of time steps to days.
	 */
	void searchAnimalToBreed(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Search for edible animals or resources to consume.
	 *
	 * @param ediblesByEdibility Vector to store potential prey with associated edibility values.
	 * @param numberOfTimeSteps Number of simulation time steps for the search.
	 * @param animalsHasTriedToPredate List of animals that have already attempted predation.
	 * @param edibilitiesContent Stream to store debug information on edibility calculations.
	 */
	void searchAnimalsAndResourceToEat(std::vector<std::tuple<PreciseDouble, Edible*, DryMass>>& ediblesByEdibility, const TimeStep numberOfTimeSteps, std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate, std::ostringstream& edibilitiesContent);

	/**
	 * @brief Apply plasticity to a trait value based on the animal's condition.
	 *
	 * @param traitValue Base value of the trait.
	 * @param plasticityDueToCondition Factor representing plasticity due to condition.
	 * @param h Scaling factor for the adjustment.
	 * @return PreciseDouble Adjusted trait value.
	 */
	PreciseDouble applyPlasticityDueToConditionToTrait(const PreciseDouble& traitValue, const PreciseDouble& plasticityDueToCondition, const PreciseDouble& h) const;

	/**
	 * @brief Check if the animal is currently in a breeding zone.
	 *
	 * @return true if in breeding zone, false otherwise.
	 */
	bool isInBreedingZone() const;

	/**
	 * @brief Check if the animal is in a pre-breeding habitat shift.
	 *
	 * @return true if in pre-breeding habitat shift, false otherwise.
	 */
	bool isInHabitatShiftBeforeBreeding() const;

	/**
	 * @brief Check if the animal is in a post-breeding habitat shift.
	 *
	 * @return true if in post-breeding habitat shift, false otherwise.
	 */
	bool isInHabitatShiftAfterBreeding() const;

	/**
	 * @brief Check if the animal has reached its movement destination.
	 *
	 * @return true if at destination, false otherwise.
	 */
	bool isAtDestination() const;

	/**
	 * @brief Get the target neighbor the animal is traveling towards.
	 *
	 * @return Pair of map coordinates and continuous position of the target neighbor.
	 */
	const std::pair<PointMap, PointContinuous>& getTargetNeighborToTravelTo() const;

	/**
	 * @brief Get the maximum number of reproduction events allowed for this animal.
	 *
	 * @return Maximum number of reproduction events.
	 */
	unsigned int getMaxReproductionEvents() const;

	/**
	 * @brief Get the instar currently used for evaluating terrain cells.
	 *
	 * @return Reference to the current instar.
	 */
	const Instar& getInstarToEvaluateCells() const;

	/**
	 * @brief Set a new instar for evaluating terrain cells.
	 *
	 * @param newInstarToEvaluateCells The new instar to assign.
	 */
	void setInstarToEvaluateCells(const Instar& newInstarToEvaluateCells);

	/**
	 * @brief Grow the animal according to its growth model.
	 *
	 * @param landscape Pointer to the landscape.
	 * @param actualTimeStep Current time step of the simulation.
	 * @param timeStepsPerDay Conversion factor of time steps to days.
	 */
	void grow(Landscape* const landscape, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Prepare the animal to resume activity after pupation or decrease pupation timer.
	 *
	 * @param landscape Pointer to the landscape.
	 */
	void isReadyToResumeFromPupaOrDecreasePupaTimer(Landscape* const landscape);
	
	/**
	 * @brief Prepare the animal to resume activity after diapause or increase diapause time steps.
	 *
	 * @param landscape Pointer to the landscape.
	 */
	void isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(Landscape* const landscape);

	/**
	 * @brief Get the current voracity (feeding capacity) of the animal.
	 *
	 * @return Current voracity.
	 */
	const PreciseDouble& getVoracity() const;

	/**
	 * @brief Get the current speed of the animal.
	 *
	 * @return Current speed.
	 */
	const PreciseDouble& getSpeed() const;

	/**
	 * @brief Get the radius of the animal's scope area.
	 *
	 * @return Scope area radius.
	 */
	const PreciseDouble& getScopeAreaRadius() const;

	/**
	 * @brief Get the radius of the animal's search area.
	 *
	 * @return Search area radius.
	 */
	const PreciseDouble& getSearchAreaRadius() const;

	/**
	 * @brief Get the radius of the animal's interaction area.
	 *
	 * @return Interaction area radius.
	 */
	const PreciseDouble& getInteractionAreaRadius() const;

	/**
	 * @brief Calculate the animal's growth model for the current time step.
	 *
	 * @param timeStepsPerDay Conversion factor of time steps to days.
	 */
	void calculateGrowthModel(const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Calculate the growth curves of the animal.
	 *
	 * @param timeStepsPerDay Conversion factor of time steps to days.
	 */
	void calculateGrowthCurves(const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Force the animal to molt.
	 *
	 * @param landscape Pointer to the landscape.
	 * @param actualTimeStep Current simulation time step.
	 * @param timeStepsPerDay Conversion factor of time steps to days.
	 */
	void forceMolting(Landscape* const landscape, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Assign a new species to the animal.
	 *
	 * @param newSpecies Pointer to the new species.
	 */
	void setSpecies(Species* newSpecies) override;

	/**
	 * @brief Perform initial evaluation of the terrain cell for this animal.
	 */
	void doInitialCellEvaluation();

	/**
	 * @brief Convert the animal state to a string for display or logging.
	 *
	 * @return std::string Representation of the animal state.
	 */
	operator std::string() const;

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
	AnimalNonStatisticalGrowth growthBuildingBlock; /**< Growth building block of this animal */
    Decisions decisions;                               /**< Behavioral decisions module */
    HuntingMode huntingMode;                           /**< Current hunting mode */
    PreciseDouble voracity;                            /**< Animal voracity */
    PreciseDouble speed;                               /**< Movement speed */
    PreciseDouble scopeAreaRadius;                     /**< Radius of scope area */
    PreciseDouble searchAreaRadius;                    /**< Radius of search area */
    PreciseDouble interactionAreaRadius;               /**< Radius of interaction area */
    bool inBreedingZone;                               /**< Is the animal in a breeding zone? */
    bool inHabitatShiftBeforeBreeding;                /**< Is the animal shifting habitat before breeding? */
    bool inHabitatShiftAfterBreeding;                 /**< Is the animal shifting habitat after breeding? */
    bool atDestination;                                /**< Has the animal reached its target? */
    std::pair<PointMap, PointContinuous> targetNeighborToTravelTo; /**< Target neighbor to travel to */

	/** @brief Identifier of the male that mated with this animal, if applicable. */
	EdibleID idFromMatedMale;

	/** @brief Generation number of the mated male parent. */
	Generation generationNumberFromMatedMale;

	/** @brief Indicates whether this animal has mated. */
	bool mated;

	/** @brief Pointer to the genome inherited from the mated male. */
	Genome* genomeFromMatedMale;

	/** @brief Instar currently used for evaluating terrain cells. */
	Instar instarToEvaluateCells;

	/** @brief Indicates whether the animal is exhausted (cannot perform further actions). */
	bool exhausted;

	/** @brief Steps moved by the animal in the current time step. */
	PreciseDouble steps;

	/** @brief Number of attempted movement steps in the current time step. */
	unsigned int stepsAttempted;

	/** @brief Total number of predation encounters the animal has experienced. */
	size_t totalPredationEncounters;

	/** @brief Identifier of the predator that last predated this animal. */
	EdibleID predatedByID;

	/** @brief Number of time steps the animal has spent without food. */
	TimeStep timeStepsWithoutFood;

	/** @brief Indicates whether the animal is sated. */
	bool sated;

	/** @brief Age of the animal at its last molt or reproduction event. */
	TimeStep ageOfLastMoultOrReproduction;

	/** @brief Day on which the animal died (if applicable). */
	Day dateOfDeath;

	/** @brief Time step of the last movement performed by the animal. */
	TimeStep lastDayMoved;

	/** @brief Indicates whether this is the first movement of the animal. */
	bool firstMovement;

	/** @brief Indicates if the animal has exceeded its growth curve (beyond maximum growth). */
	bool beyondCurve;

	/** @brief Maximum voracity reached during the current time step. */
	DryMass maxVoracityTimeStep;

	/** @brief Generation number of the female parent. */
	Generation generationNumberFromFemaleParent;

	/** @brief Generation number of the male parent. */
	Generation generationNumberFromMaleParent;

	/** @brief Identifier of the female parent. */
	EdibleID idFromFemaleParent;

	/** @brief Identifier of the male parent. */
	EdibleID idFromMaleParent;

	/** @brief Counter of reproduction events performed by the animal. */
	unsigned int reproCounter;

	/** @brief Fecundity of the animal (number of offspring per reproduction event). */
	unsigned int fecundity;

	/** @brief Age of the animal at first reproduction. */
	TimeStep ageOfFirstReproduction;

	/** @brief Time steps the animal remains unborn (used for eggs or gestation). */
	TimeStep unbornTimeSteps;

	/** @brief Number of food items eaten today. */
	int eatenToday;

	/** @brief Food mass assimilated during the current time step, organized by species and instar. */
	CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, DryMass>> foodMassAssimilatedCurrentTimeStepPerSpecies;

	/** @brief Food mass eaten during the current time step, organized by species and instar. */
	CustomIndexedVector<Species::ID, CustomIndexedVector<Instar, DryMass>> foodMassEatenCurrentTimeStepPerSpecies;

	/** @brief Total food mass assimilated during the current time step. */
	DryMass foodMassAssimilatedCurrentTimeStep;

	/** @brief Total food mass eaten during the current time step. */
	DryMass foodMassEatenCurrentTimeStep;

	/** @brief Number of predation encounters in the current day. */
	size_t predationEncountersCurrentDay;

	/** @brief Net growth of the animal from the previous time step. */
	DryMass previousNetGrowth;

	/** @brief Current prey being consumed or targeted. */
	Prey currentPrey;


	/**
	 * @brief Returns the current prey the animal is targeting or consuming.
	 * @return Reference to the current Prey object.
	 */
	const Prey& getCurrentPrey() const;

	/**
	 * @brief Sets the current prey the animal is targeting or consuming.
	 * @param prey Reference to the Edible object representing the prey.
	 * @param landscape Pointer to the landscape where the prey is located.
	 * @param targetDryMass Desired dry mass to be consumed.
	 * @param numberOfTimeSteps Number of time steps for this predation event.
	 * @param timeStepsPerDay Number of time steps per day for scaling purposes.
	 * @param competitionAmongResourceSpecies Flag indicating if competition among resource species should be considered.
	 */
	void setCurrentPrey(Edible& prey, Landscape* const landscape, const DryMass &targetDryMass, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, const bool competitionAmongResourceSpecies);

	/**
	 * @brief Set the Scope Area Radius object
	 * 
	 * @param newValue New value for the scope area radius
	 */
	void setScopeAreaRadius(const PreciseDouble& newValue);

	/**
	 * @brief Assigns a new SpeciesGrowth object to the animal for growth calculations.
	 * @param newSpeciesGrowth Pointer to the new SpeciesGrowth object.
	 */
	void setSpeciesGrowth(SpeciesGrowth* newSpeciesGrowth);

	/**
	 * @brief Returns the number of predation encounters the animal has had during the current day.
	 * @return Reference to the current day predation encounters.
	 */
	const size_t& getPredationEncountersCurrentDay() const;

	/**
	 * @brief Sets parent-related attributes for the animal.
	 * @param g_numb_prt_female Generation number of the female parent.
	 * @param g_numb_prt_male Generation number of the male parent.
	 * @param ID_prt_female Identifier of the female parent.
	 * @param ID_prt_male Identifier of the male parent.
	 */
	void setOtherAttributes(const Generation& g_numb_prt_female, const Generation& g_numb_prt_male, EdibleID ID_prt_female, EdibleID ID_prt_male);

	/**
	 * @brief Calculates the proportion of time the animal spent moving relative to total time.
	 * @return Proportion of time spent moving (0-1).
	 */
	PreciseDouble calculateProportionOfTimeWasMoving() const;

	/**
	 * @brief Forces the animal to reproduce regardless of normal conditions.
	 * @param actualTimeStep Current time step of the simulation.
	 * @param timeStepsPerDay Number of time steps per day.
	 */
	void forceReproduction(const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Checks if the animal is leaving its initial cell during the first movement.
	 * @return True if leaving initial cell, false otherwise.
	 */
	bool checkInitialMovementCellLeaving() const;

	/**
	 * @brief Checks if the animal can leave its current cell during a movement step.
	 * @return True if the animal can leave the cell, false otherwise.
	 */
	bool checkStepCellLeaving() const;

	/**
	 * @brief Determines if the animal can eat a given prey.
	 * @param prey Reference to the potential prey animal.
	 * @param animalsHasTriedToPredate List of animals that have already been targeted to avoid double predation.
	 * @return True if the animal can eat the prey, false otherwise.
	 */
	bool canEatAnimal(const AnimalNonStatistical& prey, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate) const;
	
	/**
	 * @brief Determines if the animal can eat a resource based on its dry mass.
	 * @param dryMass Dry mass of the resource.
	 * @return True if the animal can consume the resource, false otherwise.
	 */
	bool canEatResource(const DryMass &dryMass) const;

	/**
	 * @brief Adds species-specific data for growth and feeding.
	 * @param numberOfInstars Number of instars in the species.
	 * @param timeStepsPerDay Number of time steps per day.
	 */
	void addSpecies(const unsigned int numberOfInstars, const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Checks if the animal has already tried to hunt a specific prey.
	 * @param prey Reference to the prey animal.
	 * @param animalsHasTriedToPredate List of animals that have already been targeted.
	 * @return True if the prey has been previously targeted, false otherwise.
	 */
	bool hasTriedToHunt(const AnimalNonStatistical& prey, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate) const;

	/**
	 * @brief Evaluates exposed attacks on the animal and updates relevant content streams.
	 * @param landscape Pointer to the landscape.
	 * @param numberOfTimeSteps Number of time steps.
	 * @param timeStepsPerDay Number of time steps per day.
	 * @param edibilitiesContent Stream to store edibility information.
	 * @param saveAnimalsEachDayPredationProbabilities Flag to save daily predation probabilities.
	 * @param predationProbabilitiesContent Stream to store predation probabilities.
	 * @param saveActivity Flag to save activity data.
	 * @param activityContent Stream to store activity information.
	 * @param competitionAmongResourceSpecies Flag indicating competition effects among resource species.
	 */
	void evaluateExposedAttacks(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, std::ostringstream& edibilitiesContent, const bool saveAnimalsEachDayPredationProbabilities, std::ostringstream& predationProbabilitiesContent, const bool saveActivity, std::ostringstream& activityContent, const bool competitionAmongResourceSpecies);

	/**
	 * @brief Assimilates the currently targeted prey and updates internal states.
	 * @param saveActivity Flag to save activity data.
	 * @param activityContent Stream to store activity information.
	 * @param actualTimeStep Current simulation time step.
	 * @param timeStepsPerDay Number of time steps per day.
	 */
	void assimilateCurrentPrey(const bool saveActivity, std::ostringstream& activityContent, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);
	
	/**
	 * @brief Computes the food mass currently being handled by the animal.
	 * @return Dry mass of the food under handling.
	 */
	DryMass computeHandlingFoodMass() const;
	
	/**
	 * @brief Applies handling time effects for consumed food.
	 * @param foodMass Dry mass of food being handled.
	 * @param saveActivity Flag to save activity data.
	 * @param activityContent Stream to store activity information.
	 * @param actualTimeStep Current simulation time step.
	 * @param timeStepsPerDay Number of time steps per day.
	 */
	void applyHandlingTime(const DryMass& foodMass, const bool saveActivity, std::ostringstream& activityContent, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);
	
	/**
	 * @brief Increases the number of steps moved by the animal in the current time step.
	 * @param stepsToAdd Number of steps to add.
	 */
	void increaseSteps(const PreciseDouble& stepsToAdd);
	
	/**
	 * @brief Computes the handling time for a prey based on its dry mass.
	 * @param preyDryMass Dry mass of the prey.
	 * @param timeStepsPerDay Number of time steps per day.
	 * @return Handling time as a Day object.
	 */
	Day computeHandlingTime(const DryMass& preyDryMass, const PreciseDouble& timeStepsPerDay) const;

	/**
	 * @brief Sets the flag indicating if the animal is in habitat shift before breeding.
	 * @param newInHabitatShiftBeforeBreedingValue Boolean flag value.
	 */
	void setInHabitatShiftBeforeBreeding(const bool newInHabitatShiftBeforeBreedingValue);
	
	/**
	 * @brief Sets the flag indicating if the animal is in habitat shift after breeding.
	 * @param newInHabitatShiftAfterBreedingValue Boolean flag value.
	 */
	void setInHabitatShiftAfterBreeding(const bool newInHabitatShiftAfterBreedingValue);

	/**
	 * @brief Calculates the logarithmic ratio of mass relative to prey dry mass.
	 * @param preyDryMass Dry mass of the prey.
	 * @return Logarithmic mass ratio.
	 */
	PreciseDouble calculateLogMassRatio(DryMass preyDryMass) const;

	/**
	 * @brief Sets whether the animal has reached its target destination.
	 * @param newAtDestinationValue Boolean flag indicating destination reached.
	 */
	void setAtDestination(const bool newAtDestinationValue);
	
	/**
	 * @brief Sets the target neighbor cell the animal is traveling to.
	 * @param newTargetNeighborToTravelTo Pair of map and continuous coordinates.
	 */
	void setTargetNeighborToTravelTo(const std::pair<PointMap, PointContinuous> newTargetNeighborToTravelTo);

	/**
	 * @brief Checks if breeding is possible with another animal.
	 * @param landscape Pointer to the landscape.
	 * @param numberOfTimeSteps Number of time steps.
	 * @param otherAnimal Reference to the other animal.
	 * @param timeStepsPerDay Number of time steps per day.
	 */
	void checkBreed(Landscape* const landscape, const TimeStep numberOfTimeSteps, AnimalNonStatistical &otherAnimal, const PreciseDouble& timeStepsPerDay);

	/**
	 * @brief Searches for a target to travel to within the given scope area.
	 * @param scopeArea Maximum distance to search.
	 * @param animalsHasTriedToPredate List of animals already targeted.
	 */
	virtual void searchTargetToTravelTo(const PreciseDouble &scopeArea, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate)=0;
	
	/**
	 * @brief Searches for a target to travel to within the given scope area and indicates if no destinations were found.
	 * @param scopeArea Maximum distance to search.
	 * @param animalsHasTriedToPredate List of animals already targeted.
	 * @param withoutDestinations Boolean flag set to true if no destinations found.
	 */
	virtual void searchTargetToTravelTo(const PreciseDouble &scopeArea, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate, bool& withoutDestinations)=0;
	
	/**
	 * @brief Moves the animal one step according to simulation rules.
	 * @param landscape Pointer to the landscape.
	 * @param saveActivity Flag to save activity data.
	 * @param activityContent Stream for activity information.
	 * @param saveMovements Flag to save movement data.
	 * @param movementsContent Stream for movement information.
	 * @param actualTimeStep Current simulation time step.
	 * @param timeStepsPerDay Number of time steps per day.
	 * @param edibilitiesContent Stream for edibility data.
	 * @param saveAnimalsEachDayPredationProbabilities Flag to save daily predation probabilities.
	 * @param predationProbabilitiesContent Stream for predation probability data.
	 * @param competitionAmongResourceSpecies Flag indicating competition effects.
	 */
	virtual void moveOneStep(Landscape* const landscape, const bool saveActivity, std::ostringstream& activityContent, const bool saveMovements, std::ostringstream& movementsContent, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay, std::ostringstream& edibilitiesContent, const bool saveAnimalsEachDayPredationProbabilities, std::ostringstream& predationProbabilitiesContent, const bool competitionAmongResourceSpecies)=0;
	
	/**
	 * @brief Creates an offspring animal from given parental gametes.
	 * @param firstParentGamete Pointer to the first parent's gamete.
	 * @param secondParentGamete Pointer to the second parent's gamete.
	 * @param parentTerrainCell Pointer to the terrain cell of the parents.
	 * @param factorEggMassFromMom Scaling factor for egg mass from mother.
	 * @param g_numb_prt_female Generation number of female parent.
	 * @param g_numb_prt_male Generation number of male parent.
	 * @param ID_prt_female Identifier of female parent.
	 * @param ID_prt_male Identifier of male parent.
	 * @param mySpecies Pointer to the species of the offspring.
	 * @param gender Gender of the offspring.
	 * @param actualTimeStep Current simulation time step.
	 * @param timeStepsPerDay Number of time steps per day.
	 * @return Pointer to the newly created offspring animal.
	 */
	virtual AnimalNonStatistical* createOffspring(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& factorEggMassFromMom, const Generation& g_numb_prt_female,
		const Generation& g_numb_prt_male, EdibleID ID_prt_female, EdibleID ID_prt_male, AnimalSpecies* const mySpecies, Gender gender, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)=0;
};

#endif /* ANIMAL_NON_STATISTICAL_H_ */
