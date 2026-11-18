/**
 * @file Random.cpp
 * @brief Defines functions for generating random numbers within specified ranges.
 * @author jrbcast
 */

#include "App/Model/IBM/Maths/Random.h"

using namespace std;


bool Random::hasBeenInitialized = false;
unsigned int Random::masterSeed = 0;
std::atomic<unsigned int> Random::seedOffset{0};


void Random::initRandomGenerator(unsigned int seed)
{
	// Make sure this only initializes once
	if ( !hasBeenInitialized )
	{
		masterSeed = seed;
        seedOffset.store(0, std::memory_order_relaxed);
        hasBeenInitialized = true;
        
		// Force initialization in the main thread
        getEngine();
        getEngine64();
	}
}

std::mt19937& Random::getEngine() {
	// thread_local initializes the first time this thread calls the function
    thread_local std::mt19937 tls_rng([]() {
        // fetch_add garantizes that each thread gets a unique sequential number without locks
        unsigned int offset = seedOffset.fetch_add(1, std::memory_order_relaxed);
        return masterSeed + offset;
    }());
    
    return tls_rng;
}

std::mt19937_64& Random::getEngine64() {
	// thread_local initializes the first time this thread calls the function
    thread_local std::mt19937_64 tls_rng_64([]() {
		// Add an arbitrary extra offset to separate the sequence from the 32-bit one
        unsigned int offset = seedOffset.fetch_add(1, std::memory_order_relaxed);
        return masterSeed + offset + 10000; 
    }());
    
    return tls_rng_64;
}

PreciseDouble Random::randomDoubleInRange(const PreciseDouble& min, const PreciseDouble& max)
{
	std::uniform_real_distribution<double> dist(min.getValue(), max.getValue());
	return dist(getEngine64());
}

int Random::randomIntegerInRange(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(getEngine());
}

TimeStep Random::randomTimeStepInRange(TimeStep min, TimeStep max)
{
	std::uniform_int_distribution<unsigned int> dist(min.getValue(), max.getValue());
	return TimeStep(dist(getEngine()));
}

PreciseDouble Random::randomUniform(const PreciseDouble& min, const PreciseDouble& max) {
	std::uniform_real_distribution<double> dist(min.getValue(), (max - PreciseDouble::EPS).getValue());
	return dist(getEngine64());
}

PreciseDouble Random::randomUniform() {
	std::uniform_real_distribution<double> dist(0.0, 1.0 - PreciseDouble::EPS);
	return dist(getEngine64());
}

size_t Random::randomIndex(const size_t &size) {
	if(size <= 0) {
		throwLineInfoException("'size' must be greater than 0");
	}

	std::uniform_int_distribution<size_t> dist(0, size - 1);
	return dist(getEngine());
}

void Random::createIndicesVector(vector<size_t>& indicesVector, size_t size)
{
	indicesVector.clear();
	indicesVector.resize(size);

	iota(indicesVector.begin(), indicesVector.end(), 0u);

	shuffle(indicesVector.begin(), indicesVector.end(), getEngine());
}

void Random::createUniqueIndicesVector(vector<size_t>& uniqueIndices, size_t initialIndex, size_t finalIndex, size_t numberOfIndices)
{
	if(finalIndex - initialIndex < numberOfIndices)
	{
		throwLineInfoException("Error: The range of values is less than the number of indices.");
	}

	vector<size_t> indices(finalIndex - initialIndex);
    // Inicializar con valores consecutivos
    iota(indices.begin(), indices.end(), initialIndex);

	shuffle(indices.begin(), indices.end(), getEngine());

	uniqueIndices.clear();
	uniqueIndices.insert(uniqueIndices.end(), indices.begin(), indices.begin() + static_cast<std::ptrdiff_t>(numberOfIndices));
}

void Random::rouletteSelection(vector<size_t>& selection, const vector<double>& probabilities, const size_t numberOfElements)
{
	discrete_distribution<size_t> distribution(probabilities.begin(), probabilities.end());

	selection.clear();
	selection.reserve(numberOfElements);

	for(size_t i = 0; i < numberOfElements; i++)
	{
		selection.push_back(distribution(getEngine()));
	}
}
