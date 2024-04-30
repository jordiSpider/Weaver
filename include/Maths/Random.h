/*
 * random.h
 *
 *  Created on: Aug 2, 2012
 *      Author: jrbcast
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
#include "LineInfoException.h"
#include "Maths/Constants.h"

class Random
{
private:
	static std::mt19937 rng;
	static std::mt19937_64 rng_64;
	static bool hasBeenInitialized;
public:
	static void initRandomGenerator(unsigned int seed);
	static float randomFloatInRange(float min, float max);
	static double randomDoubleInRange(double min, double max);
	static int randomIntegerInRange(int min, int max);
	static double randomUniform(double min, double max);
	static double randomUniform();
	static unsigned int randomIndex(const unsigned int &size);
	static std::vector<int>* createIndicesVector(unsigned int size);
	template<typename T>
	static void shuffleList(std::list<T>** sourceListPtr);
};

#endif /* RANDOM_H_ */
