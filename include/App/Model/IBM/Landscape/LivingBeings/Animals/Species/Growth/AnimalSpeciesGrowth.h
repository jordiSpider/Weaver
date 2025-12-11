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


/** @defgroup GrowthParameters Growth Model and Plasticity
 * @brief Methods and attributes related to growth rates, plasticity, and mass–length relations.
 */

/** @defgroup DevelopmentParameters Development and Instar Information
 * @brief Methods and attributes controlling instar age, maturation, and development.
 */

/** @defgroup MorphologyAndPhysiology Mass, Length, and Morphology
 * @brief Attributes and methods defining wet mass, dry mass, and length relations.
 */

/** @defgroup ReproductionParameters Reproduction
 * @brief Attributes and methods controlling fecundity, egg batches, and breeding strategies.
 */

/** @defgroup EnergyAllocation Energy Allocation and Storage
 * @brief Attributes and methods related to structural mass, energy tanks, and molting allocation.
 */

/** @defgroup HabitatParameters Habitat Shift
 * @brief Attributes and methods controlling habitat shifts per instar.
 */


/**
 * @class AnimalSpeciesGrowth
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
    /// @ingroup MorphologyAndPhysiology
    /// @brief Returns the female wet mass.
    const WetMass& getFemaleWetMass() const;

    /// @ingroup MorphologyAndPhysiology
    /// @brief Returns the egg dry mass.
    const DryMass& getEggDryMass() const;

    /// @ingroup MorphologyAndPhysiology
    /**
     * @brief Calculates dry length from dry mass.
     * @param dryMass Dry mass of the individual.
     * @param mature Whether the individual is mature.
     * @return Calculated length.
     */
    const Length calculateDryLength(const DryMass& dryMass, const bool mature) const;

    /// @ingroup MorphologyAndPhysiology
    /**
     * @brief Calculates dry mass from length.
     * @param length Length of the individual.
     * @param mature Whether the individual is mature.
     * @return Calculated dry mass.
     */
    const DryMass calculateDryMass(const Length& length, const bool mature) const;

    /// @ingroup MorphologyAndPhysiology
    /// @brief Returns the mass coefficient A for mature individuals.
    PreciseDouble getCoefficientForMassAforMature() const;

    /// @ingroup MorphologyAndPhysiology
    /// @brief Returns the mass scale B for mature individuals.
    PreciseDouble getScaleForMassBforMature() const;

    /// @ingroup MorphologyAndPhysiology
    /// @brief Returns the general mass coefficient A.
    PreciseDouble getCoefficientForMassA() const;
	
    /// @ingroup MorphologyAndPhysiology
    /// @brief Returns the general mass scale B.
	PreciseDouble getScaleForMassB() const;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Growth Model
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @ingroup GrowthParameters
    /**
     * @brief Returns the actual growth model based on temperature.
     * @param actualTemperature Current environmental temperature.
     * @param tempFromLab Laboratory reference temperature.
     * @return Pair of growth model type and its parameter.
     */
    std::pair<IndividualGrowthModel::Type, PreciseDouble> getActualGrowthModel(const Temperature& actualTemperature, const Temperature& tempFromLab) const;
    
    /// @ingroup GrowthParameters
    /// @brief Checks if the species has indeterminate growth.
    bool hasIndeterminateGrowth() const;

    /// @ingroup GrowthParameters
    /// @brief Returns maximum plasticity for the Von Bertalanffy growth model.
    PreciseDouble getMaxPlasticityKVonBertalanffy() const;

    /// @ingroup GrowthParameters
    /// @brief Returns minimum plasticity for the Von Bertalanffy growth model.
    PreciseDouble getMinPlasticityKVonBertalanffy() const;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Development and Instars
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @ingroup DevelopmentParameters
    /// @brief Returns the first reproductive instar.
    const Instar& getInstarFirstReproduction() const;

    /// @ingroup DevelopmentParameters
    /// @brief Returns the first maturation instar.
    const Instar& getInstarFirstMaturation() const;

    /// @ingroup DevelopmentParameters
    /// @brief Returns the range of mature instars.
    const std::vector<Instar>& getMatureInstarsRange() const;

    /// @ingroup DevelopmentParameters
    /// @brief Returns vector of ages per instar.
    const CustomIndexedVector<Instar, Day>& getInstarAgeVector() const;

    /// @ingroup DevelopmentParameters
    /// @brief Returns development time for a given instar.
    const Day& getInstarDevTime(const Instar& instar) const;

    /// @ingroup DevelopmentParameters
    /// @brief Returns threshold age for molting.
    const PreciseDouble& getMoltingAgeThreshold() const;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Reproduction
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @ingroup ReproductionParameters
    /**
     * @brief Calculates number of eggs per batch for a given dry mass.
     * @param individualDryMass Dry mass of the individual.
     * @return Number of eggs.
     */
    PreciseDouble calculateEggsPerBatch(const DryMass& individualDryMass) const;

    /// @ingroup ReproductionParameters
    /// @brief Returns whether eggs per batch are calculated using an equation.
    bool getEggsPerBatchFromEquation() const;

    /// @ingroup ReproductionParameters
    /// @brief Returns predefined eggs per batch value.
    const PreciseDouble& getEggsPerBatch() const;

    /// @ingroup ReproductionParameters
    /// @brief Returns the intercept of the egg batch size equation.
    const PreciseDouble& getInterceptForEggBatchFromEquation() const;

    /// @ingroup ReproductionParameters
    /// @brief Returns the slope of the egg batch size equation.
	const PreciseDouble& getSlopeForEggBatchFromEquation() const;

    /// @ingroup ReproductionParameters
    /// @brief Checks if the species uses capital breeding.
    bool hasCapitalBreeding() const;

    /// @ingroup ReproductionParameters
    /// @brief Returns time of reproductive event during capital breeding.
    const TimeStep& getTimeOfReproEventDuringCapitalBreeding() const;

    /// @ingroup ReproductionParameters
    /// @brief Returns number of reproductive events in capital breeding.
	unsigned int getNumberOfCapitalBreeds() const;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Energy Allocation
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @ingroup EnergyAllocation
    /**
     * @brief Decomposes mass into structural and energy components.
     * @param dryMass Total dry mass.
     * @param energyTankTraitValue Energy storage trait.
     * @return Pair of structural mass and energy mass.
     */
    std::pair<DryMass, DryMass> decomposeMassElements(const DryMass &dryMass, const PreciseDouble &energyTankTraitValue) const;
    
    /// @ingroup EnergyAllocation
    /// @brief Returns beta scaling factor for energy tank.
    const PreciseDouble& getBetaScaleTank() const;

    /// @ingroup EnergyAllocation
    /// @brief Returns excess investment in size.
    PreciseDouble getExcessInvestInSize() const;

    /// @ingroup EnergyAllocation
    /// @brief Returns allocated energy for molting.
    const PreciseDouble& getAssignedForMolt() const;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Habitat Shifts
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @ingroup HabitatParameters
    /// @brief Returns whether the habitat shift occurs at a given instar.
    bool habitatShiftOccurs(const Instar &instar) const;

    /// @ingroup HabitatParameters
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
    /// @ingroup MorphologyAndPhysiology
    /// @brief Female wet mass (in grams or species-specific units).
    WetMass femaleWetMass;

    /// @ingroup MorphologyAndPhysiology
    /// @brief Egg dry mass (in grams or species-specific units).
    DryMass eggDryMass;

    /// @ingroup MorphologyAndPhysiology
    /// @brief Default growth model type and parameter for the species.
    std::pair<IndividualGrowthModel::Type, PreciseDouble> defaultGrowthModel;

    /// @ingroup GrowthParameters
    /// @brief Indicates whether the growth model depends on temperature.
    bool growthModelTemperatureDependent;

    /// @ingroup GrowthParameters
    /// @brief Lower temperature thresholds and associated growth model changes.
    std::vector<std::pair<Temperature, std::pair<IndividualGrowthModel::Type, PreciseDouble>>> lowerGrowthModelThermalChanges;

    /// @ingroup GrowthParameters
    /// @brief Upper temperature thresholds and associated growth model changes.
    std::vector<std::pair<Temperature, std::pair<IndividualGrowthModel::Type, PreciseDouble>>> upperGrowthModelThermalChanges;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Development and Instars
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @ingroup DevelopmentParameters
    /// @brief Whether the species exhibits indeterminate growth.
    bool indeterminateGrowth;

    /// @ingroup DevelopmentParameters
    /// @brief Instar at which individuals first reproduce.
    Instar instarFirstReproduction;

    /// @ingroup DevelopmentParameters
    /// @brief Range of instars considered mature.
    std::vector<Instar> matureInstarsRange;

    /// @ingroup DevelopmentParameters
    /// @brief Age in days for each instar.
    CustomIndexedVector<Instar, Day> instarAgeVector;

    /// @ingroup GrowthParameters
    /// @brief Maximum plasticity parameter for Von Bertalanffy growth.
    PreciseDouble maxPlasticityKVonBertalanffy;

    /// @ingroup GrowthParameters
    /// @brief Minimum plasticity parameter for Von Bertalanffy growth.
    PreciseDouble minPlasticityKVonBertalanffy;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Mass-Length Coefficients
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @ingroup MorphologyAndPhysiology
    /// @brief Mass coefficient A for mature individuals.
    PreciseDouble coefficientForMassAforMature;

    /// @ingroup MorphologyAndPhysiology
    /// @brief Mass scale B for mature individuals.
    PreciseDouble scaleForMassBforMature;

    /// @ingroup MorphologyAndPhysiology
    /// @brief Mass coefficient A for general individuals.
    PreciseDouble coefficientForMassA;

    /// @ingroup MorphologyAndPhysiology
    /// @brief Mass scale B for general individuals.
	PreciseDouble scaleForMassB;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Energy Allocation and Molting
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @ingroup EnergyAllocation
    /// @brief Scaling factor for energy tank.
    PreciseDouble betaScaleTank;

    /// @ingroup EnergyAllocation
    /// @brief Excess investment in size beyond standard growth.
    PreciseDouble excessInvestInSize;

    /// @ingroup EnergyAllocation
    /// @brief Threshold age for molting (in days or time steps).
    PreciseDouble moltingAgeThreshold;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Reproductive Parameters
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @ingroup ReproductionParameters
    /// @brief Indicates if eggs per batch are derived from an equation.
    bool eggsPerBatchFromEquation;

    /// @ingroup ReproductionParameters
    /// @brief Number of eggs per batch.
    PreciseDouble eggsPerBatch;

    /// @ingroup ReproductionParameters
    /// @brief Intercept used when calculating eggs per batch from an equation.
    PreciseDouble interceptForEggBatchFromEquation;

    /// @ingroup ReproductionParameters
    /// @brief Slope used when calculating eggs per batch from an equation.
	PreciseDouble slopeForEggBatchFromEquation;

    /// @ingroup ReproductionParameters
    /// @brief Indicates if species uses capital breeding strategy.
    bool capitalBreeding;

    /// @ingroup ReproductionParameters
    /// @brief Time of reproductive event during capital breeding.
	TimeStep timeOfReproEventDuringCapitalBreeding;

    /// @ingroup ReproductionParameters
    /// @brief Number of reproductive events in capital breeding.
	unsigned int numberOfCapitalBreeds;

    /// @ingroup EnergyAllocation
    /// @brief Amount of energy assigned for molting.
    PreciseDouble assignedForMolt;

    ///@}

    // --------------------------------------------------------------------------------------------
    // @name Habitat Shift
    // --------------------------------------------------------------------------------------------
    ///@{
    /// @ingroup HabitatParameters
    /// @brief Flags indicating if habitat shift occurs per instar.
    std::vector<bool> habitatShiftPerInstar;

    /// @ingroup HabitatParameters
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
