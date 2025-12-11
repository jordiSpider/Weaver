/**
 * @file AnimalStatisticalGrowth.h
 * @brief Defines the AnimalStatisticalGrowth class, which represents the growth of an animal 
 *        using statistical growth models. Inherits from AnimalGrowth.
 */

#ifndef ANIMAL_STATISTICAL_GROWTH_H_
#define ANIMAL_STATISTICAL_GROWTH_H_


#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/AnimalGrowth.h"


/**
 * @class AnimalStatisticalGrowth
 * @brief Represents an animal growth model based on statistical approaches.
 * 
 * This class implements the growth of animals using statistical models rather than deterministic curves.
 * It is used for species where growth is influenced by probabilistic factors and variability among individuals.
 */
class AnimalStatisticalGrowth : public AnimalGrowth
{
public:
    /**
     * @brief Constructor for AnimalStatisticalGrowth.
     * 
     * Initializes an animal's growth with species-specific parameters and individual traits.
     * 
     * @param speciesGrowth Pointer to the species growth data.
     * @param instar Current instar of the animal.
     * @param temperatureDependent Boolean indicating if growth is temperature-dependent.
     * @param gender Gender of the animal.
     * @param baseIndividualTraits Vector of individual traits affecting growth.
     * @param factorEggMassValue Factor for scaling egg mass.
     * @param femaleMaxReproductionEvents Maximum number of reproduction events for females.
     * @param actualTemperature Current environmental temperature.
     * @param tempFromLab Temperature data from laboratory experiments.
     * @param timeStepsPerDay Number of simulation time steps per day.
     */
    AnimalStatisticalGrowth(AnimalSpeciesGrowth* const speciesGrowth, const Instar &instar, const bool temperatureDependent, Gender gender, const std::vector<IndividualTrait>& baseIndividualTraits, const PreciseDouble& factorEggMassValue, const unsigned int femaleMaxReproductionEvents, const Temperature& actualTemperature, const Temperature& tempFromLab, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Destructor for AnimalStatisticalGrowth.
     */
    virtual ~AnimalStatisticalGrowth();

    /**
     * @brief Forces molting for the animal.
     * 
     * Adjusts the animal's state to simulate molting events according to the base individual traits.
     * 
     * @param baseIndividualTraits Vector of traits influencing molting.
     * @param gender Gender of the animal.
     * @param timeStepsPerDay Number of simulation time steps per day.
     */
    void forceMolting(const std::vector<IndividualTrait>& baseIndividualTraits, Gender gender, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Checks whether the animal uses capital breeding.
     * 
     * @return True if the animal reproduces using capital breeding strategy, false otherwise.
     */
    bool hasCapitalBreeding() const;
};

#endif // ANIMAL_STATISTICAL_GROWTH_H_
