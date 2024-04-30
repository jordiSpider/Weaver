/**
 * @file Random.cpp
 * @brief Defines functions for generating random numbers within specified ranges.
 * @author jrbcast
 */

#include "IBM/Maths/Random.h"

using namespace std;
using json = nlohmann::json;


bool Random::hasBeenInitialized = false;
mt19937 Random::rng;
mt19937_64 Random::rng_64;


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

float Random::randomFloatInRange(float value1, float value2)
{
	float min = std::min( value1, value2);
	float max = std::max( value1, value2);
	std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

double Random::randomDoubleInRange(double value1, double value2)
{
	double min = std::min( value1, value2);
	double max = std::max( value1, value2);
	std::uniform_real_distribution<double> dist(min, max);
	return dist(rng_64);
}

int Random::randomIntegerInRange(int value1, int value2)
{
	int min = std::min(value1, value2);
	int max = std::max(value1, value2);
	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

double Random::randomUniform(double min, double max) {
	std::uniform_real_distribution<double> dist(min, max - std::numeric_limits<double>::epsilon());
	return dist(rng_64);
}

double Random::randomUniform() {
	std::uniform_real_distribution<double> dist(0.0, 1.0 - std::numeric_limits<double>::epsilon());
	return dist(rng_64);
}

unsigned int Random::randomIndex(const unsigned int &size) {
	if(size <= 0) {
		throwLineInfoException("'size' must be greater than 0");
	}

	std::uniform_int_distribution<int> dist(0, size - 1);
	return dist(rng);
}

unique_ptr<vector<unsigned int>> Random::createIndicesVector(unsigned int size)
{
	auto indicesVector = make_unique<vector<unsigned int>>(size);

	for(unsigned int i = 0; i < size; ++i) {
		indicesVector->at(i) = i;
	}

	shuffle(indicesVector->begin(), indicesVector->end(), rng);

	return indicesVector;
}

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

unique_ptr<vector<unsigned int>> Random::createUniqueIndicesVector(unsigned int initialIndex, unsigned int finalIndex, unsigned int numberOfIndices)
{
	if(finalIndex - initialIndex < numberOfIndices)
	{
		throwLineInfoException("Error: The range of values is less than the number of indices.");
	}

	vector<unsigned int> indices(finalIndex - initialIndex);
    // Inicializar con valores consecutivos
    iota(indices.begin(), indices.end(), initialIndex);

	shuffle(indices.begin(), indices.end(), rng);

	unique_ptr<vector<unsigned int>> uniqueIndices = make_unique<vector<unsigned int>>(indices.begin(), indices.begin() + numberOfIndices);

	return uniqueIndices;
}

template<typename T>
unique_ptr<vector<T>> Random::createUniqueVector(vector<T>& range, unsigned int numberOfValues)
{
	unique_ptr<vector<unsigned int>> randomIndices = createUniqueIndicesVector(0, range.size(), numberOfValues);

	unique_ptr<vector<T>> resultVector = make_unique<vector<T>>();
	resultVector->reserve(numberOfValues);

	transform(randomIndices->begin(), randomIndices->end(), back_inserter(*resultVector),
                   [&range](const unsigned int index) { return range[index]; });

	return resultVector;
}

template<>
unique_ptr<vector<json>> Random::createUniqueVector(vector<json>& range, unsigned int numberOfValues) {
    unique_ptr<vector<unsigned int>> randomIndices = createUniqueIndicesVector(0, range.size(), numberOfValues);

	unique_ptr<vector<json>> resultVector = make_unique<vector<json>>();
	resultVector->reserve(numberOfValues);

	transform(randomIndices->begin(), randomIndices->end(), back_inserter(*resultVector),
                   [&range](const unsigned int index) { return range[index]; });

	return resultVector;
}

unique_ptr<vector<unsigned int>> Random::rouletteSelection(const vector<double> probabilities, const unsigned int numberOfElements)
{
	discrete_distribution<unsigned int> distribution(probabilities.begin(), probabilities.end());

	unique_ptr<vector<unsigned int>> selection = make_unique<vector<unsigned int>>();
	selection->reserve(numberOfElements);

	for(unsigned int i = 0; i < numberOfElements; i++)
	{
		selection->push_back(distribution(rng));
	}

	return selection;
}
