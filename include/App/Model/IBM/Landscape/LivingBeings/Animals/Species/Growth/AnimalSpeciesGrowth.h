/**
 * @file AnimalSpeciesGrowth.h
 * @brief Defines growth, maturation, reproduction, and developmental dynamics for animal species.
 *
 * This class extends SpeciesGrowth by implementing species-specific biological rules
 * governing growth models, instar development times, mass–length relationships,
 * reproductive investment, plasticity, and habitat shifts.
 */

#ifndef ANIMAL_SPECIES_GROWTH_H_
#define ANIMAL_SPECIES_GROWTH_H_

#include <nlohmann/json.hpp>
#include <vector>

#include <boost/serialization/export.hpp>
#include <boost/serialization/utility.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/IndividualGrowthModel.h"
#include "Misc/EnumClass.h"
#include "Misc/CustomIndexedVector.h"
#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/SpeciesGrowth.h"
#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"
#include "App/Model/IBM/Physics/Temperature.h"


/**
 * @class AnimalSpeciesGrowth
 * @ingroup weaver_landscape
 * @brief Represents species-specific growth, development, and reproductive parameters for animals.
 *
 * This class extends SpeciesGrowth and provides detailed calculations for:
 * - Mass and length relationships
 * - Growth models (temperature-dependent and plasticity)
 * - Instar development and reproductive stages
 * - Energy allocation and molting
 * - Egg production and capital breeding strategies
 * - Habitat shift patterns
 *
 * All parameters can be initialized from a JSON configuration and adjusted dynamically
 * according to environmental conditions (temperature, lab reference, etc.).
 */
class AnimalSpeciesGrowth : public SpeciesGrowth {
public:
    /**
     * @brief Default constructor.
     */
    AnimalSpeciesGrowth();

    /**
     * @brief Constructs the object from a JSON configuration.
     *
     * @param config JSON object containing species-specific growth parameters.
     * @param numberOfInstars Total number of instars defined for the species.
     * @param conversionToWetMass Factor to convert dry to wet mass.
     * @param tempFromLab Laboratory reference temperature.
     * @param timeStepsPerDay Number of simulation time steps contained in one day.
     */
    AnimalSpeciesGrowth(const nlohmann::json& config, const unsigned int numberOfInstars, const std::vector<double>& conversionToWetMass, const Temperature& tempFromLab, const PreciseDouble& timeStepsPerDay);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~AnimalSpeciesGrowth();

    // --------------------------------------------------------------------------------------------
    // @name Mass and Length Parameters
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @brief Returns the female wet mass.
    const WetMass& getFemaleWetMass() const;

    /// @brief Returns the egg dry mass.
    const DryMass& getEggDryMass() const;

    /**
     * @brief Calculates dry length from dry mass.
     * @param dryMass Dry mass of the individual.
     * @param mature Whether the individual is mature.
     * @return Calculated length.
     */
    const Length calculateDryLength(const DryMass& dryMass, const bool mature) const;

    /**
     * @brief Calculates dry mass from length.
     * @param length Length of the individual.
     * @param mature Whether the individual is mature.
     * @return Calculated dry mass.
     */
    const DryMass calculateDryMass(const Length& length, const bool mature) const;

    /// @brief Returns the mass coefficient A for mature individuals.
    PreciseDouble getCoefficientForMassAforMature() const;

    /// @brief Returns the mass scale B for mature individuals.
    PreciseDouble getScaleForMassBforMature() const;

    /// @brief Returns the general mass coefficient A.
    PreciseDouble getCoefficientForMassA() const;
	
    /// @brief Returns the general mass scale B.
	PreciseDouble getScaleForMassB() const;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Growth Model
    // --------------------------------------------------------------------------------------------
    ///@{
    /**
     * @brief Returns the actual growth model based on temperature.
     * @param actualTemperature Current environmental temperature.
     * @param tempFromLab Laboratory reference temperature.
     * @return Pair of growth model type and its parameter.
     */
    std::pair<IndividualGrowthModel::Type, PreciseDouble> getActualGrowthModel(const Temperature& actualTemperature, const Temperature& tempFromLab) const;
    
    /// @brief Checks if the species has indeterminate growth.
    bool hasIndeterminateGrowth() const;

    /// @brief Returns maximum plasticity for the Von Bertalanffy growth model.
    PreciseDouble getMaxPlasticityKVonBertalanffy() const;

    /// @brief Returns minimum plasticity for the Von Bertalanffy growth model.
    PreciseDouble getMinPlasticityKVonBertalanffy() const;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Development and Instars
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @brief Returns the first reproductive instar.
    const Instar& getInstarFirstReproduction() const;

    /// @brief Returns the first maturation instar.
    const Instar& getInstarFirstMaturation() const;

    /// @brief Returns the range of mature instars.
    const std::vector<Instar>& getMatureInstarsRange() const;

    /// @brief Returns vector of ages per instar.
    const CustomIndexedVector<Instar, Day>& getInstarAgeVector() const;

    /// @brief Returns development time for a given instar.
    const Day& getInstarDevTime(const Instar& instar) const;

    /// @brief Returns threshold age for molting.
    const PreciseDouble& getMoltingAgeThreshold() const;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Reproduction
    // --------------------------------------------------------------------------------------------
    ///@{
    /**
     * @brief Calculates number of eggs per batch for a given dry mass.
     * @param individualDryMass Dry mass of the individual.
     * @return Number of eggs.
     */
    PreciseDouble calculateEggsPerBatch(const DryMass& individualDryMass) const;

    /// @brief Returns whether eggs per batch are calculated using an equation.
    bool getEggsPerBatchFromEquation() const;

    /// @brief Returns predefined eggs per batch value.
    const PreciseDouble& getEggsPerBatch() const;

    /// @brief Returns the intercept of the egg batch size equation.
    const PreciseDouble& getInterceptForEggBatchFromEquation() const;

    /// @brief Returns the slope of the egg batch size equation.
	const PreciseDouble& getSlopeForEggBatchFromEquation() const;

    /// @brief Checks if the species uses capital breeding.
    bool hasCapitalBreeding() const;

    /// @brief Returns time of reproductive event during capital breeding.
    const TimeStep& getTimeOfReproEventDuringCapitalBreeding() const;

    /// @brief Returns number of reproductive events in capital breeding.
	unsigned int getNumberOfCapitalBreeds() const;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Energy Allocation
    // --------------------------------------------------------------------------------------------
    ///@{
    /**
     * @brief Decomposes mass into structural and energy components.
     * @param dryMass Total dry mass.
     * @param energyTankTraitValue Energy storage trait.
     * @return Pair of structural mass and energy mass.
     */
    std::pair<DryMass, DryMass> decomposeMassElements(const DryMass &dryMass, const PreciseDouble &energyTankTraitValue) const;
    
    /// @brief Returns beta scaling factor for energy tank.
    const PreciseDouble& getBetaScaleTank() const;

    /// @brief Returns excess investment in size.
    PreciseDouble getExcessInvestInSize() const;

    /// @brief Returns allocated energy for molting.
    const PreciseDouble& getAssignedForMolt() const;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Habitat Shifts
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @brief Returns whether the habitat shift occurs at a given instar.
    bool habitatShiftOccurs(const Instar &instar) const;

    /// @brief Returns habitat shift factor.
    const PreciseDouble& getHabitatShiftFactor() const;

    ///@}

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
    // --------------------------------------------------------------------------------------------
    // @name Morphology and Mass
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @brief Female wet mass (in grams or species-specific units).
    WetMass femaleWetMass;

    /// @brief Egg dry mass (in grams or species-specific units).
    DryMass eggDryMass;

    /// @brief Default growth model type and parameter for the species.
    std::pair<IndividualGrowthModel::Type, PreciseDouble> defaultGrowthModel;

    /// @brief Indicates whether the growth model depends on temperature.
    bool growthModelTemperatureDependent;

    /// @brief Lower temperature thresholds and associated growth model changes.
    std::vector<std::pair<Temperature, std::pair<IndividualGrowthModel::Type, PreciseDouble>>> lowerGrowthModelThermalChanges;

    /// @brief Upper temperature thresholds and associated growth model changes.
    std::vector<std::pair<Temperature, std::pair<IndividualGrowthModel::Type, PreciseDouble>>> upperGrowthModelThermalChanges;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Development and Instars
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @brief Whether the species exhibits indeterminate growth.
    bool indeterminateGrowth;

    /// @brief Instar at which individuals first reproduce.
    Instar instarFirstReproduction;

    /// @brief Range of instars considered mature.
    std::vector<Instar> matureInstarsRange;

    /// @brief Age in days for each instar.
    CustomIndexedVector<Instar, Day> instarAgeVector;

    /// @brief Maximum plasticity parameter for Von Bertalanffy growth.
    PreciseDouble maxPlasticityKVonBertalanffy;

    /// @brief Minimum plasticity parameter for Von Bertalanffy growth.
    PreciseDouble minPlasticityKVonBertalanffy;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Mass-Length Coefficients
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @brief Mass coefficient A for mature individuals.
    PreciseDouble coefficientForMassAforMature;

    /// @brief Mass scale B for mature individuals.
    PreciseDouble scaleForMassBforMature;

    /// @brief Mass coefficient A for general individuals.
    PreciseDouble coefficientForMassA;

    /// @brief Mass scale B for general individuals.
	PreciseDouble scaleForMassB;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Energy Allocation and Molting
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @brief Scaling factor for energy tank.
    PreciseDouble betaScaleTank;

    /// @brief Excess investment in size beyond standard growth.
    PreciseDouble excessInvestInSize;

    /// @brief Threshold age for molting (in days or time steps).
    PreciseDouble moltingAgeThreshold;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Reproductive Parameters
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @brief Indicates if eggs per batch are derived from an equation.
    bool eggsPerBatchFromEquation;

    /// @brief Number of eggs per batch.
    PreciseDouble eggsPerBatch;

    /// @brief Intercept used when calculating eggs per batch from an equation.
    PreciseDouble interceptForEggBatchFromEquation;

    /// @brief Slope used when calculating eggs per batch from an equation.
	PreciseDouble slopeForEggBatchFromEquation;

    /// @brief Indicates if species uses capital breeding strategy.
    bool capitalBreeding;

    /// @brief Time of reproductive event during capital breeding.
	TimeStep timeOfReproEventDuringCapitalBreeding;

    /// @brief Number of reproductive events in capital breeding.
	unsigned int numberOfCapitalBreeds;

    /// @brief Amount of energy assigned for molting.
    PreciseDouble assignedForMolt;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Habitat Shift
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @brief Flags indicating if habitat shift occurs per instar.
    std::vector<bool> habitatShiftPerInstar;

    /// @brief Scaling factor for habitat shift.
    PreciseDouble habitatShiftFactor;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Private Setters
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @brief Sets the female wet mass.
    void setFemaleWetMass(const WetMass& newFemaleWetMass);

    /// @brief Sets the egg dry mass from configuration.
    void setEggDryMass(const nlohmann::json& config);

    /// @brief Configures growth model using JSON info and lab temperature.
    void setGrowthModel(const nlohmann::json& growthModelInfo, const Temperature& tempFromLab);

    /// @brief Sets the first reproductive instar.
    void setInstarFirstReproduction(const Instar& newInstarFirstReproduction);

    /// @brief Sets instar age vector using a vector of doubles (days per instar).
    void setInstarAgeVector(const std::vector<double>& newInstarAgeVector);

    /// @brief Sets maximum plasticity for Von Bertalanffy growth.
    void setMaxPlasticityKVonBertalanffy(const PreciseDouble& maxPlasticityKVonBertalanffyValue);

    /// @brief Sets minimum plasticity for Von Bertalanffy growth.
	void setMinPlasticityKVonBertalanffy(const PreciseDouble& minPlasticityKVonBertalanffyValue);

    /// @brief Sets mass coefficient A for mature individuals.
    void setCoefficientForMassAforMature(const PreciseDouble& coefficientForMassAforMatureValue);

    /// @brief Sets mass scale B for mature individuals.
    void setScaleForMassBforMature(const PreciseDouble& scaleForMassBforMatureValue);

    /// @brief Sets mass coefficient A for general individuals.
    void setCoefficientForMassA(const PreciseDouble& coefficientForMassAValue);

    /// @brief Sets mass scale B for general individuals.
	void setScaleForMassB(const PreciseDouble& scaleForMassBValue);

    /// @brief Sets beta scaling factor for energy tank.
    void setBetaScaleTank(const PreciseDouble& betaScaleTankValue);

    /// @brief Sets excess investment in size.
    void setExcessInvestInSize(const PreciseDouble& excessInvestInSizeValue);

    /// @brief Sets eggs per batch from JSON configuration.
    void setEggsPerBatch(const nlohmann::json& config);

    /// @brief Configures capital breeding from JSON configuration.
    void setCapitalBreeding(const nlohmann::json &config, const PreciseDouble& timeStepsPerDay);

    /// @brief Sets assigned energy for molting.
    void setAssignedForMolt(const PreciseDouble& assignedForMoltValue);

    /// @brief Configures habitat shift per instar and scaling factor.
    void setHabitatShift(const std::vector<unsigned int> &habitatShift, const nlohmann::json &newHabitatShiftFactor);

    ///@}
};

#endif /* ANIMAL_SPECIES_GROWTH_H_ */
