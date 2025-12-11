/**
 * @file AnimalGrowth.h
 * @brief Defines the AnimalGrowth class for managing the growth, mass, and age of animals.
 *
 * This class represents the growth dynamics of an animal, including body size, energy tank,
 * instar ages, and maturation status. It extends EdibleGrowth and provides
 * temperature-dependent growth calculations using individual traits.
 */

#ifndef ANIMAL_GROWTH_H_
#define ANIMAL_GROWTH_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/LivingBeings/Growth/EdibleGrowth.h"
#include "App/Model/IBM/Physics/Temperature.h"
#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/IndividualGrowthModel.h"
#include "Misc/CustomIndexedVector.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Gender.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTrait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Growth/AnimalSpeciesGrowth.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Type/BaseTraitType.h"


/**
 * @class AnimalGrowth
 * @brief Manages the growth and mass dynamics of an individual animal.
 *
 * AnimalGrowth tracks the current body size, energy reserves, instar ages, and maturity
 * of an individual animal. It supports temperature-dependent growth and energy allocation.
 */
class AnimalGrowth : public EdibleGrowth
{
public:
    /**
     * @brief Default constructor.
     */
    AnimalGrowth();

    /**
     * @brief Constructor with initialization.
     * @param speciesGrowth Pointer to the species growth template.
     * @param instar Initial instar of the animal.
     * @param temperatureDependent Whether growth depends on temperature.
     * @param gender Gender of the animal.
     * @param baseIndividualTraits Base traits for growth calculation.
     * @param factorEggMassValue Factor to adjust egg mass at birth.
     * @param femaleMaxReproductionEvents Maximum reproduction events for females.
     * @param actualTemperature Current environmental temperature.
     * @param tempFromLab Laboratory reference temperature.
     * @param timeStepsPerDay Number of time steps per day.
     */
    AnimalGrowth(AnimalSpeciesGrowth* const speciesGrowth, const Instar &instar, const bool temperatureDependent, Gender gender, const std::vector<IndividualTrait>& baseIndividualTraits, const PreciseDouble& factorEggMassValue, const unsigned int femaleMaxReproductionEvents, const Temperature& actualTemperature, const Temperature& tempFromLab, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Destructor.
     */
    virtual ~AnimalGrowth();

    /**
     * @brief Get the current body size (dry mass) of the animal.
     * @return Current body size.
     */
    const DryMass& getCurrentBodySize() const;

    /**
     * @brief Get the age of a specific instar.
     * @param instarValue Instar to query.
     * @return Age in time steps.
     */
    const TimeStep& getInstarAge(const Instar& instarValue) const;

    /**
     * @brief Get the full vector of individual instar ages.
     * @return Vector of instar ages.
     */
    const CustomIndexedVector<Instar, TimeStep>& getIndividualInstarAgeVector() const;

    /**
     * @brief Get the growth model used for this animal.
     * @return Pointer to the IndividualGrowthModel.
     */
    const IndividualGrowthModel* getIndividualGrowthModel() const;

    /**
     * @brief Get the dry mass of the egg at birth.
     * @return Egg dry mass.
     */
    const DryMass& getEggDryMassAtBirth() const;

    /**
     * @brief Get the length of the animal at birth.
     * @return Length at birth.
     */
    const Length& getLengthAtBirth() const;

    /**
     * @brief Calculate the growth model based on traits and temperature.
     * @param baseIndividualTraits Base traits for growth calculation.
     * @param actualTemperature Current temperature.
     * @param tempFromLab Reference lab temperature.
     * @param timeStepsPerDay Number of time steps per day.
     */
    void calculateGrowthModel(const std::vector<IndividualTrait>& baseIndividualTraits, const Temperature& actualTemperature, const Temperature& tempFromLab, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Set the animal's dry mass and energy tank values.
     * @param newDryMass New body size.
     * @param investment Investment in energy allocation.
     * @param energyTankFinalTraitValue Value of the energy tank trait.
     * @param isMoulting True if the animal is moulting.
     * @param gender Gender of the animal.
     * @param checkPreviousValues Whether to verify previous mass values.
     * @param actualTimeStep Current simulation time step.
     */
    void setDryMass(const DryMass &newDryMass, const DryMass& investment, const PreciseDouble& energyTankFinalTraitValue, const bool isMoulting, Gender gender, const bool checkPreviousValues, const TimeStep actualTimeStep);

    /**
     * @brief Modify the energy tank by a given value.
     * @param newValue New energy tank value.
     * @param actualTimeStep Current simulation time step.
     */
    void modifyEnergyTank(const DryMass &newValue, const TimeStep actualTimeStep);

    /**
     * @brief Get the current energy tank.
     * @return Current energy tank.
     */
    const DryMass& getCurrentEnergyTank() const;

    /**
     * @brief Check if the species uses capital breeding.
     * @return True if capital breeder, false otherwise.
     */
    virtual bool hasCapitalBreeding() const=0;

    /**
     * @brief Set the current age of the animal.
     * @param newCurrentAge New age in time steps.
     */
    void setCurrentAge(const TimeStep &newCurrentAge);

    /**
     * @brief Get the current age of the animal.
     * @return Current age in time steps.
     */
    const TimeStep& getCurrentAge() const;

    /**
     * @brief Set the animal's mature state.
     * @param isMature True if the animal is mature.
     * @param gender Gender of the animal.
     */
    virtual void setMatureValue(const bool isMature, const Gender& gender);

    /**
     * @brief Check if the animal is mature.
     * @return True if mature, false otherwise.
     */
    bool isMature() const;

    /**
     * @brief Set the current total wet mass of the animal.
     * @param newCurrentTotalWetMass New wet mass.
     */
    void setCurrentTotalWetMass(const WetMass& newCurrentTotalWetMass) override;

    #ifdef DEBUG
        /**
         * @brief Restores internal debug variables for testing and verification purposes.
         *
         * This method is only included in debug builds (#ifdef DEBUG) and is used to
         * deserialize or re-initialize variables that are maintained exclusively
         * for debugging. It allows developers to verify the state of the object
         * across different time steps or after deserialization in a debug environment.
         *
         * Typical uses include checking the current body size, energy tank, instar ages,
         * and other intermediate calculations that are not part of the standard simulation output.
         */
        void deserializeDebugVariables();
    #endif

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
    CustomIndexedVector<Instar, TimeStep> individualInstarAgeVector; /**< Age of each instar. */
    CustomIndexedVector<Instar, Length> instarLengthsVector; /**< Lengths at each instar. */
    CustomIndexedVector<Instar, DryMass> instarMassesVector; /**< Masses at each instar. */

    IndividualGrowthModel* individualGrowthModel; /**< Growth model for the individual. */

    DryMass eggDryMassAtBirth; /**< Dry mass at birth. */
    Length lengthAtBirth; /**< Length at birth. */

    PreciseDouble baseDevTimeProportion; /**< Base proportion of development time. */

    DryMass currentBodySize; /**< Current body size. */
    DryMass currentEnergyTank; /**< Current energy tank. */

    bool temperatureDependent; /**< Indicates if growth depends on temperature. */

    #ifdef DEBUG
        /**
         * @brief Indicates whether the current body size has been initialized.
         * 
         * Used only in debug mode to track if the currentBodySize value is valid
         * and has been properly set for the current time step.
         */
        bool currentBodySizeInitialised;

        /**
         * @brief Stores the last time step at which currentBodySize was updated.
         * 
         * Helps in debugging to verify the evolution of body size over time
         * and detect possible inconsistencies.
         */
        TimeStep currentBodySizeLastTimeStep;

        /**
         * @brief Indicates whether the current energy tank has been initialized.
         * 
         * Used only in debug mode to track if the currentEnergyTank value is valid
         * and has been properly set for the current time step.
         */
        bool currentEnergyTankInitialised;

        /**
         * @brief Stores the last time step at which currentEnergyTank was updated.
         * 
         * Helps in debugging to verify the evolution of the energy tank over time
         * and detect possible inconsistencies.
         */
        TimeStep currentEnergyTankLastTimeStep;
    #endif

    TimeStep currentAge; /**< Current age of the animal. */
    bool mature; /**< Maturity status. */
    
    /**
     * @brief Calculate body size and energy at birth.
     * @param factorEggMass Factor for egg mass.
     * @param gender Gender of the animal.
     */
    void calculateValuesAtBirth(const PreciseDouble& factorEggMass, Gender gender);

    /**
     * @brief Calculate growth curves using individual traits.
     * @param baseIndividualTraits Base traits for growth.
     * @param femaleMaxReproductionEvents Maximum female reproduction events.
     * @param timeStepsPerDay Number of time steps per day.
     */
    virtual void calculateGrowthCurves(const std::vector<IndividualTrait>& baseIndividualTraits, const unsigned int femaleMaxReproductionEvents, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Sets both the current body size and energy tank values for the animal.
     * 
     * @param newCurrentBodySize The new body size to set.
     * @param newEnergyTank The new energy tank value to set.
     * @param actualTimeStep The current time step of the simulation.
     * 
     * Updates internal variables and ensures consistency for the given time step.
     */
    void setMassElements(const DryMass &newCurrentBodySize, const DryMass& newEnergyTank, const TimeStep actualTimeStep);

    /**
     * @brief Sets the current body size of the animal.
     * 
     * @param newCurrentBodySize The new body size to assign.
     * @param actualTimeStep The current time step of the simulation.
     * 
     * This method updates the currentBodySize and the debug tracking variables if DEBUG mode is enabled.
     */
    void setCurrentBodySize(const DryMass& newCurrentBodySize, const TimeStep actualTimeStep);

    /**
     * @brief Updates the total mass of the animal.
     * 
     * Combines body size, energy tank, and other mass components (if any) to keep total mass consistent.
     */
    void updateTotalMass();

    /**
     * @brief Checks whether the animal's growth depends on temperature.
     * 
     * @return True if growth is temperature-dependent, false otherwise.
     */
    bool isTemperatureDependent() const;

    #ifdef DEBUG
        /**
         * @brief Debug method to manually set the current body size at a specific time step.
         * 
         * @param actualTimeStep The time step at which to set the body size.
         */
        void testSetCurrentBodySize(const TimeStep actualTimeStep);

        /**
         * @brief Debug method to get the current body size.
         * 
         * Checks internal consistency and returns the current body size.
         */
        void testGetCurrentBodySize() const;

        /**
         * @brief Debug method to manually modify the energy tank at a specific time step.
         * 
         * @param actualTimeStep The time step at which to modify the energy tank.
         */
		void testModifyEnergyTank(const TimeStep actualTimeStep);

        /**
         * @brief Debug method to get the current energy tank value.
         * 
         * Checks internal consistency and returns the energy tank value.
         */
        void testGetCurrentEnergyTank() const;
	#endif

    /**
     * @brief Gets a constant pointer to the species-specific growth information.
     * 
     * @return Pointer to the AnimalSpeciesGrowth object associated with this animal.
     */
    const AnimalSpeciesGrowth* getSpeciesGrowth() const;

    /**
     * @brief Gets a mutable pointer to the species-specific growth information.
     * 
     * @return Pointer to the AnimalSpeciesGrowth object associated with this animal.
     * 
     * Allows modifying species-specific growth parameters.
     */
    AnimalSpeciesGrowth* getMutableSpeciesGrowth();
};

#endif // ANIMAL_GROWTH_H_
