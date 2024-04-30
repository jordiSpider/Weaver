/**
 * @file Random.h
 * @brief Contains the implementation of the Parabola class.
 * @author jrbcast
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
#include "Constants.h"

class Random
{
private:
	static std::mt19937 rng;
	static std::mt19937_64 rng_64;
	static bool hasBeenInitialized;
public:
	/**
	 * @brief Inits the random generator.
	 * @param seed Seed for the random number generator.
	 */
	static void initRandomGenerator(unsigned int seed);

	/**
	 * @brief Generate a random float within two limits (both included).
	 * @param value1 First boundary.
	 * @param value2 Second boundary.
	 * @return The randomly generated value in [value1..value2].
 	*/
	static float randomFloatInRange(float min, float max);

	/**
	 * @brief Generate a random double within two limits (both included).
	 * @param value1 First boundary.
	 * @param value2 Second boundary.
	 * @return The randomly generated value in [value1..value2].
	 */
	static double randomDoubleInRange(double min, double max);
	
	/**
	 * @brief Generate a random integer within two limits (both included).
	 * @param value1 First boundary.
	 * @param value2 Second boundary.
	 * @return The randomly generated value in [value1..value2].
	 */
	static int randomIntegerInRange(int min, int max);

	/**
	 * @brief Generates a random uniform value in [min,max).
	 * @param min The minimum value to be returned.
	 * @param max The maximum value to be returned, excluding itself.
	 * @return A random uniform value in [min,max).
	 */
	static double randomUniform(double min, double max);

	/**
	 * @brief Generates a random uniform value in [0.0,1.0).
	 * @return A random uniform value in [0.0,1.0).
 	 */
	static double randomUniform();

	/**
	 * @brief Generates one random index within the range [0,size).
	 * @param size The upper limit of the range.
	 * @return One random index within the range [0,size).
	 */
	static unsigned int randomIndex(const unsigned int &size);
	
	/**
	 * @brief Creates a randomized vector of indices from 0 to the specified size.
	 * @param size The size of the vector to be created.
	 * @return A pointer to the vector of randomized indices.
	 */
	static std::unique_ptr<std::vector<unsigned int>> createIndicesVector(unsigned int size);
	
	/**	
	 * @brief Applies the Fisher-Yates shuffle to an existing list of elements.
	 * @param sourceListPtr Pointer to the list to be shuffled.
	 */
	template<typename T>
	static void shuffleList(std::list<T>** sourceListPtr);
	static std::unique_ptr<std::vector<unsigned int>> createUniqueIndicesVector(unsigned int initialIndex, unsigned int finalIndex, unsigned int numberOfIndices);
	template<typename T>
	static std::unique_ptr<std::vector<T>> createUniqueVector(std::vector<T>& range, unsigned int numberOfValues);
	static std::unique_ptr<std::vector<unsigned int>> rouletteSelection(const std::vector<double> probabilities, const unsigned int numberOfElements);
};

#endif /* RANDOM_H_ */
