/**
 * @file Random.h
 * @brief Defines the Random class that provides various random number generation utilities.
 *
 * This class encapsulates deterministic random number generators (Mersenne Twister) for producing
 * uniform random values, integers, indices, and random selections. It also provides helper methods
 * for shuffling and sampling elements from containers.
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <float.h>
#include <list>
#include <vector>
#include <random>
#include <algorithm>
#include <memory>
#include <nlohmann/json.hpp>

#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Maths/Constants.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"
#include "Misc/Types.h"


/**
 * @brief Utility class providing random number generation functionalities.
 *
 * This class maintains a shared Mersenne Twister random number generator for reproducible and
 * thread-safe random generation. It supports generation of doubles, integers, time steps, indices,
 * and various operations like shuffling or sampling unique elements.
 *
 * @note All methods are static, meaning the class is used as a pure utility.
 */
class Random
{
private:
	static std::mt19937 rng;     /**< 32-bit Mersenne Twister random number generator. */
    static std::mt19937_64 rng_64; /**< 64-bit Mersenne Twister random number generator. */
    static bool hasBeenInitialized; /**< Flag indicating whether the random generator has been initialized. */
public:
	/**
     * @brief Initializes the random number generator with a given seed.
     *
     * @param seed Seed for the random number generator.
     * @note This method must be called before any random generation to ensure reproducibility.
     */
	static void initRandomGenerator(unsigned int seed);

	/**
     * @brief Generates a random double value within the specified inclusive range [min, max].
     *
     * @param min Minimum limit.
     * @param max Maximum limit.
     * @return A random double value between @p min and @p max.
     */
	static PreciseDouble randomDoubleInRange(const PreciseDouble& min, const PreciseDouble& max);
	
	/**
     * @brief Generates a random integer within the specified inclusive range [min, max].
     *
     * @param min Minimum integer value.
     * @param max Maximum integer value.
     * @return A random integer between @p min and @p max (both inclusive).
     */
	static int randomIntegerInRange(int min, int max);

	/**
     * @brief Generates a random TimeStep within the specified inclusive range [min, max].
     *
     * @param min Minimum limit.
     * @param max Maximum limit.
     * @return A random TimeStep between @p min and @p max (both inclusive).
     */
	static TimeStep randomTimeStepInRange(TimeStep min, TimeStep max);

	/**
     * @brief Generates a random uniform double value in the half-open interval [min, max).
     *
     * @param min Minimum possible value (inclusive).
     * @param max Maximum possible value (exclusive).
     * @return A random uniform double value in [min, max).
     */
	static PreciseDouble randomUniform(const PreciseDouble& min, const PreciseDouble& max);

	/**
     * @brief Generates a random uniform double value in [0.0, 1.0).
     *
     * @return A random uniform value in [0.0, 1.0).
     */
	static PreciseDouble randomUniform();

	/**
     * @brief Generates a random index within the range [0, size).
     *
     * @param size The upper limit of the index range.
     * @return A random index within the specified range.
     * @throws LineInfoException if @p size is zero (in DEBUG mode).
     */
	static size_t randomIndex(const size_t &size);
	
	/**
     * @brief Creates a vector of consecutive indices [0, size) and shuffles them randomly.
     *
     * @param indicesVector Reference to the vector that will be filled with shuffled indices.
     * @param size Number of indices to generate.
     */
	static void createIndicesVector(std::vector<size_t>& indicesVector, size_t size);

	/**
     * @brief Shuffles the given vector in place using the shared random generator.
     *
     * @tparam T Type of elements in the vector.
     * @param vectorToShuffle The vector to shuffle.
     */
	template<typename T>
	static void shuffleVector(std::vector<T>& vectorToShuffle);
	
	/**
     * @brief Creates a vector of unique random indices within a given range [initialIndex, finalIndex).
     *
     * @param uniqueIndices Reference to the vector that will hold the unique indices.
     * @param initialIndex Starting index of the range.
     * @param finalIndex Ending index of the range (exclusive).
     * @param numberOfIndices Number of unique indices to generate.
     * @throws LineInfoException if @p numberOfIndices exceeds the size of the range.
     */
	static void createUniqueIndicesVector(std::vector<size_t>& uniqueIndices, size_t initialIndex, size_t finalIndex, size_t numberOfIndices);
	
	/**
     * @brief Creates a vector of unique random values selected from a given range vector.
     *
     * @tparam T Type of elements in the range.
     * @param resultVector Reference to the vector that will hold the selected unique values.
     * @param range Reference to the vector containing the available range of values.
     * @param numberOfValues Number of unique values to generate.
     * @throws LineInfoException if @p numberOfValues exceeds the number of elements in @p range.
     */
	template<typename T>
	static void createUniqueVector(std::vector<T>& resultVector, std::vector<T>& range, size_t numberOfValues);
	
	/**
     * @brief Performs roulette-wheel selection based on provided probabilities.
     *
     * This method selects indices proportionally to their corresponding probability weights.
     *
     * @param selection Reference to the vector that will hold the selected indices.
     * @param probabilities Vector of selection probabilities (must sum approximately to 1.0).
     * @param numberOfElements Number of indices to select.
     * @throws LineInfoException if probabilities contain invalid values (negative or NaN).
     */
	static void rouletteSelection(std::vector<size_t>& selection, const std::vector<double>& probabilities, const size_t numberOfElements);
};


template<typename T>
void Random::shuffleVector(std::vector<T>& vectorToShuffle)
{
	shuffle(vectorToShuffle.begin(), vectorToShuffle.end(), rng);
}

template<typename T>
void Random::createUniqueVector(std::vector<T>& resultVector, std::vector<T>& range, size_t numberOfValues)
{
	std::vector<size_t> randomIndices;
	createUniqueIndicesVector(randomIndices, 0, range.size(), numberOfValues);

	resultVector.clear();
	resultVector.reserve(numberOfValues);

	transform(randomIndices.begin(), randomIndices.end(), back_inserter(resultVector),
                   [&range](const size_t index) { return range[index]; });
}


#endif /* RANDOM_H_ */
