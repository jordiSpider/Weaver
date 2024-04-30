/*
 * random.c
 *
 *  Created on: Aug 2, 2012
 *      Author: jrbcast
 */

#include "Maths/Random.h"

using namespace std;


bool Random::hasBeenInitialized = false;
mt19937 Random::rng;
mt19937_64 Random::rng_64;

/**
 * \brief Inits the random generator
 *
 * This function should be called only once for all threads on the program. To avoid
 * problems, this is controlled through a static variable.
 */
void Random::initRandomGenerator(unsigned int seed)
{
	// Make sure this only initializes once
	if ( !hasBeenInitialized )
	{
		rng = mt19937(seed);
		rng_64 = mt19937_64(seed);
		hasBeenInitialized = true;
	}
}

/**
 * \brief Generate a random float within two limits (both included).
 *
 * @param min Minimum range boundary
 * @param max Maximum range boundary
 * @return The randomly generated value in [ \p min..\p max]
 */
float Random::randomFloatInRange(float value1, float value2)
{
	float min = std::min( value1, value2);
	float max = std::max( value1, value2);
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

/**
 * \brief Generate a random double within two limits (both included).
 *
 * @param min Minimum range boundary
 * @param max Maximum range boundary
 * @return The randomly generated value in [ \p min..\p max]
 */
double Random::randomDoubleInRange(double value1, double value2)
{
	double min = std::min( value1, value2);
	double max = std::max( value1, value2);
	std::uniform_real_distribution<double> dist(min, max);
	return dist(rng_64);
}

/**
 * \brief Generate a random integer within two limits (both included).
 *
 * @param min Minimum range boundary
 * @param max Maximum range boundary
 * @return The randomly generated value in [ \p min..\p max]
 */
int Random::randomIntegerInRange(int value1, int value2)
{
	int min = std::min(value1, value2);
	int max = std::max(value1, value2);
	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

/**
 * Generates a random uniform value in [min,max).
 * @param min the minimum value to be returned
 * @param max the maximum value to be returned, excluding itself
 * @return a random uniform value in [min,max).
 */
double Random::randomUniform(double min, double max) {
	std::uniform_real_distribution<double> dist(min, max - std::numeric_limits<double>::epsilon());
	return dist(rng_64);
}

/**
 * Generates a random uniform value in [0.0,1.0).
 * @return a random uniform value in [0.0,1.0).
 */
double Random::randomUniform() {
	std::uniform_real_distribution<double> dist(0.0, 1.0 - std::numeric_limits<double>::epsilon());
	return dist(rng_64);
}

/**
 * Generates one random index within the range [0,size).
 * @param size
 * @return one random index within the range [0,size).
 */
unsigned int Random::randomIndex(const unsigned int &size) {
	if(size <= 0) {
		throwLineInfoException("'size' must be greater than 0");
	}

	std::uniform_int_distribution<int> dist(0, size - 1);
	return dist(rng);
}

/**
 * Creates a randomized vector of indices from 0 to the specified size.
 */
vector<int>* Random::createIndicesVector(unsigned int size)
{
	vector<int>* indicesVector = new vector<int>(size);
	for (unsigned int i = 0; i < size; ++i) {
		indicesVector->at(i) = i;
	}
	return indicesVector;
}

/**
 * Applies the Fisher-Yates shuffle to an existing list of elements.
 */
template<typename T>
void Random::shuffleList(list<T>** sourceListPtr)
{
	list<T>* sourceList = *sourceListPtr;
	vector<T> shuffledVector(sourceList->size());

	int lastPos = 0;
	int insertPos;
	for(typename list<T>::iterator it = sourceList->begin(); it != sourceList->end(); it++, lastPos++)
	{
		std::uniform_int_distribution<int> dist(0, lastPos);
	    insertPos = dist(rng);
	    if (insertPos < lastPos) {
	        shuffledVector[lastPos] = shuffledVector[insertPos];
	    }
	    shuffledVector[insertPos] = *it;
	}

	//
	//copy(sourceList->begin(), sourceList->end(), shuffledVector.begin());
	//random_shuffle(shuffledVector.begin(), shuffledVector.end());
	//
	*sourceListPtr = new list<T>(shuffledVector.begin(), shuffledVector.end());
}
