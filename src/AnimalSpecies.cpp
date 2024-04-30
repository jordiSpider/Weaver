/*
 * AnimalSpecies.cpp
 *
 *  Created on: Jul 6, 2013
 *      Author: jrbcast
 */

#include "AnimalSpecies.h"
#include "LineInfoException.h"
#include "Maths/Random.h"
#include "Maths/Math_Functions.h"



AnimalSpecies::AnimalSpecies(const string& scientificName, const unsigned int& numberOfLociPerTrait, const unsigned int& numberOfAllelesPerLocus, const std::vector<double>& restrictPerTrait, const std::vector<double>& rhoPerModule, const unsigned int& traitsPerModule, const unsigned int& numberOfChromosomes, const unsigned int& numberOfChiasmasPerChromosome, const unordered_map<string,nlohmann::json>& fixedTraits, const unordered_map<string,double>& minTraitRanges, 
							  const unordered_map<string,double>& maxTraitRanges, const unordered_map<string,double>& minTraitLimits, 
							  const unordered_map<string,double>& maxTraitLimits, const vector<string>& order) :
	Species(scientificName)
{
	setTraits(
		fixedTraits, minTraitRanges, maxTraitRanges,
		minTraitLimits, maxTraitLimits, order
	);

	extinguished = false;
	this->numberOfLociPerTrait = numberOfLociPerTrait;
	this->numberOfAllelesPerLocus = numberOfAllelesPerLocus;
	this->traitsPerModule = traitsPerModule;

	this->numberOfChromosomes = numberOfChromosomes;

	if (numberOfChiasmasPerChromosome % 2 == 0 && numberOfChiasmasPerChromosome >= 0)
	{
		this->numberOfChiasmasPerChromosome = numberOfChiasmasPerChromosome;
	}
	else
	{
		std::cerr << "For the species '" << scientificName << "':" << std::endl;
		std::cerr << "numberOfChiasmasPerChromosome must be multiple of 2. You entered " << numberOfChiasmasPerChromosome << " = " << numberOfChiasmasPerChromosome << ". Exiting now" << std::endl;
		exit(-1);
	}

	//TODO Hay alguna restricción más para el parametrizador? Cual es el máximo de numberOfChromosomes??
	if ((getNumberOfVariableTraits()*numberOfLociPerTrait) % numberOfChromosomes == 0)
	{
		this->numberOfLociPerChromosome = (getNumberOfVariableTraits()*numberOfLociPerTrait) / numberOfChromosomes;
	}
	else
	{
		std::cerr << "For the species '" << scientificName << "':" << std::endl;
		std::cerr << "numberOfTraits*numberOfLoci must be multiple of numberOfChromosomes. You entered " << getNumberOfVariableTraits() << "*" << numberOfLociPerTrait << " = " << getNumberOfVariableTraits()*numberOfLociPerTrait << " --> " << getNumberOfVariableTraits()*numberOfLociPerTrait << " % " << numberOfChromosomes << " = " << ((getNumberOfVariableTraits()*numberOfLociPerTrait) % numberOfChromosomes) << ". Exiting now" << std::endl;
		exit(-1);
	}

	this->restrictPerTrait = restrictPerTrait;
	this->rhoPerModule = rhoPerModule;

	this->rhoRangePerModule.reserve(rhoPerModule.size());
	for(size_t i = 0; i < rhoPerModule.size(); ++i)
	{
		this->rhoRangePerModule[i] = round((1.0 - fabs(rhoPerModule[i])) * numberOfLociPerTrait);
	}

	chromosomesGenerator = new ChromosomesGenerator(this);
	normal_distribution<double> normalDistributionX (0.0,1.0);
	normal_distribution<double> normalDistributionY (0.0,1.0);
	pseudoGrowthMean = 0.0;
	pseudoGrowthSd = 0.0;

	//Constants for interactions
	minSizeHunted = 10000000.000;
	maxSizeHunted = 0;
	meanSizeHunted = 0.0;
	sdSizeHunted = 0.0;
	minVorHunted = 10000000.000;
	maxVorHunted = 0;
	meanVorHunted = 0.0;
	sdVorHunted = 0.0;
	minSpeedHunted = 10000000.00;
	maxSpeedHunted = 0.0;
	meanSpeedHunted = 0.0;
	sdSpeedHunted = 0.0;

	minSizeHunter = 10000000.000;
	maxSizeHunter = 0.0;
	meanSizeHunter = 0.0;
	sdSizeHunter = 0.0;
	minVorHunter = 10000000.000;
	maxVorHunter = 0.0;
	meanVorHunter = 0.0;
	sdVorHunter = 0.0;
	minSpeedHunter = 10000000.000;
	maxSpeedHunter = 0.0;
	meanSpeedHunter = 0.0;
	sdSpeedHunter = 0.0;
	minSearchAreaHunter = 10000000.000;
	maxSearchAreaHunter = 0.0;
	meanSearchAreaHunter = 0.0;
	sdSearchAreaHunter = 0.0;

	minProbabilityDensityFunction = 10000000.000;
	maxProbabilityDensityFunction = 0.0;
	meanProbabilityDensityFunction = 0.0;
	sdProbabilityDensityFunction = 0.0;
	minVorXVor = 0.0;
	maxVorXVor = 0.0;
	meanVorXVor = 0.0;
	sdVorXVor = 0.0;
	minSpeedRatio = 0.0;
	maxSpeedRatio = 0.0;
	meanSpeedRatio = 0.0;
	sdSpeedRatio = 0.0;
	
	meanSizeRatio = 0.0;
	sdSizeRatio = 0.0;

	maxPredationProbability = 0.0;
	maxPredationIndividualWetMass = 0.0;
}

AnimalSpecies::~AnimalSpecies()
{
	delete chromosomesGenerator;
}

void AnimalSpecies::addEdibleFungusSpecies(Species* species)
{
	if (std::find(edibleFungusSpecies.begin(), edibleFungusSpecies.end(), species) != edibleFungusSpecies.end())
	{
		std::cerr << "Trying to add an edible fungus species twice, please contact developers." << std::endl;
		exit(-1);
	}
	else
	{
		edibleFungusSpecies.push_back(species);
	}
}

void AnimalSpecies::addEdibleAnimalSpecies(Species* species)
{
	if (std::find(edibleAnimalSpecies.begin(), edibleAnimalSpecies.end(), species) != edibleAnimalSpecies.end())
	{
		std::cerr << "Trying to add an edible animal species twice, please contact developers." << std::endl;
		exit(-1);
	}
	else
	{
		edibleAnimalSpecies.push_back(species);
	}
}

void AnimalSpecies::addEdiblePlantSpecies(Species* species)
{
	if (std::find(ediblePlantSpecies.begin(), ediblePlantSpecies.end(), species) != ediblePlantSpecies.end())
	{
		std::cerr << "Trying to add an edible plant species twice, please contact developers." << std::endl;
		exit(-1);
	}
	else
	{
		ediblePlantSpecies.push_back(species);
	}
}

void AnimalSpecies::addEdiblePreference(Species* speciesToBeAdded, float ediblePreferenceToBeAdded)
{
	if(ediblePreferenceToBeAdded > 0.0 && ediblePreferenceToBeAdded < 1.0)
	{
		ediblePreferences[speciesToBeAdded] = ediblePreferenceToBeAdded;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "ediblePreference must be in the range (0.0 - 1.0). You entered " << ediblePreferenceToBeAdded << ". Exiting now" << std::endl;
		exit(-1);
	}
}

vector<Trait> AnimalSpecies::getVariableTraits() const
{
	vector<Trait> variableTraits;

	for(const auto& [key, value] : minTraitRanges)
	{
		variableTraits.push_back(key);
	}

	return variableTraits;
}

void AnimalSpecies::addEdibleProfitability(Species* speciesToBeAdded, float edibleProfitabilityToBeAdded)
{
	if(edibleProfitabilityToBeAdded + getMinTraitLimit(Trait::assim) > 0.0 && edibleProfitabilityToBeAdded + getMinTraitLimit(Trait::assim) < 1.0)
	{
		edibleProfitabilities[speciesToBeAdded] = edibleProfitabilityToBeAdded;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "edibleProfitability + assimMinTraitLimit must be in the range (0.0 - 1.0). You entered " << edibleProfitabilityToBeAdded + getMinTraitLimit(Trait::assim) << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setInitialPredationEventsOnOtherSpecies(unsigned int numberOfSpecies)
{
	for (unsigned int i = 0; i < numberOfSpecies; ++i) {
		predationEventsOnOtherSpecies.push_back(0);
	}
}

vector<int>* AnimalSpecies::getPredationEventsOnOtherSpecies()
{
	return (&predationEventsOnOtherSpecies);
}

vector<Species*> * AnimalSpecies::getInvolvedFungusSpecies()
{
	vector<Species*> * involvedFungusSpecies = new vector<Species*>(edibleFungusSpecies);
	vector<Species*> * alreadyCheckedAnimalSpecies = new vector<Species *>();
	alreadyCheckedAnimalSpecies->push_back(this);

	fillInvolvedFungusSpecies(involvedFungusSpecies, alreadyCheckedAnimalSpecies, &edibleAnimalSpecies);

	delete alreadyCheckedAnimalSpecies;
	return involvedFungusSpecies;
}

void AnimalSpecies::fillInvolvedFungusSpecies(vector<Species*> * involvedFungusSpecies, vector<Species*> * alreadyCheckedAnimalSpecies, vector<Species*> * involvedAnimalSpecies)
{
	for(vector<Species*>::iterator animalSpeciesIt = involvedAnimalSpecies->begin(); animalSpeciesIt != involvedAnimalSpecies->end(); animalSpeciesIt++)
	{
		Species* currentAnimalSpecies = *animalSpeciesIt;
		//Avoid the infinite loop when animals are predating each other
		if(std::find(alreadyCheckedAnimalSpecies->begin(), alreadyCheckedAnimalSpecies->end(), currentAnimalSpecies) == alreadyCheckedAnimalSpecies->end())
		{
			vector<Species*> * currentEdibleFungusSpecies = currentAnimalSpecies->getEdibleFungusSpecies();
			for(vector<Species*>::iterator fungusSpeciesIt = currentEdibleFungusSpecies->begin(); fungusSpeciesIt != currentEdibleFungusSpecies->end(); fungusSpeciesIt++)
			{
				Species* currentFungusSpecies = *fungusSpeciesIt;
				if(std::find(involvedFungusSpecies->begin(), involvedFungusSpecies->end(), currentFungusSpecies) == involvedFungusSpecies->end())
				{
					involvedFungusSpecies->push_back(currentFungusSpecies);
				}
			}
			alreadyCheckedAnimalSpecies->push_back(currentAnimalSpecies);
			fillInvolvedFungusSpecies(involvedFungusSpecies, alreadyCheckedAnimalSpecies, currentAnimalSpecies->getEdibleAnimalSpecies());
		}
	}
}

void AnimalSpecies::setAssignedForMolt(const double& assignedForMolt)
{
	if (assignedForMolt > 0)
	{
		this->assignedForMolt = assignedForMolt;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "assignedForMolt must be a positive value. You entered " << assignedForMolt << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMinRelativeHumidityThreshold(const double& minRelativeHumidityThreshold)
{
	if (minRelativeHumidityThreshold >= 0)
	{
		this->minRelativeHumidityThreshold = minRelativeHumidityThreshold;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "minRelativeHumidityThreshold must be 0 or a positive value. You entered " << minRelativeHumidityThreshold << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::calculateMinSizeHunted(double minSizeHunted)
{
	this->minSizeHunted = min(this->minSizeHunted, minSizeHunted);

}

void AnimalSpecies::calculateMaxSizeHunted(double maxSizeHunted)
{
	this->maxSizeHunted = max(this->maxSizeHunted, maxSizeHunted);

}

void AnimalSpecies::calculateMinVorHunted(double minVorHunted)
{
	this->minVorHunted = min(this->minVorHunted, minVorHunted);

}

void AnimalSpecies::calculateMaxVorHunted(double maxVorHunted)
{
	this->maxVorHunted = max(this->maxVorHunted, maxVorHunted);

}



void AnimalSpecies::calculateMinSpeedHunted(double minSpeedHunted)
{
	this->minSpeedHunted = min(this->minSpeedHunted, minSpeedHunted);

}

void AnimalSpecies::calculateMaxSpeedHunted(double maxSpeedHunted)
{
	this->maxSpeedHunted = max(this->maxSpeedHunted, maxSpeedHunted);

}


void AnimalSpecies::calculateMinSizeHunter(double minSizeHunter)
{
	this->minSizeHunter = min(this->minSizeHunter, minSizeHunter);

}

void AnimalSpecies::calculateMaxSizeHunter(double maxSizeHunter)
{
	this->maxSizeHunter = max(this->maxSizeHunter, maxSizeHunter);

}

void AnimalSpecies::calculateMinVorHunter(double minVorHunter)
{
	this->minVorHunter = min(this->minVorHunter, minVorHunter);

}

void AnimalSpecies::calculateMaxVorHunter(double maxVorHunter)
{
	this->maxVorHunter = max(this->maxVorHunter, maxVorHunter);

}

void AnimalSpecies::calculateMinSpeedHunter(double minSpeedHunter)
{
	this->minSpeedHunter = min(this->minSpeedHunter, minSpeedHunter);

}

void AnimalSpecies::calculateMaxSpeedHunter(double maxSpeedHunter)
{
	this->maxSpeedHunter = max(this->maxSpeedHunter, maxSpeedHunter);

}


void AnimalSpecies::calculateMinSearchAreaHunter(double minSearchAreaHunter)
{
	this->minSearchAreaHunter = min(this->minSearchAreaHunter, minSearchAreaHunter);

}

void AnimalSpecies::calculateMaxSearchAreaHunter(double maxSearchAreaHunter)
{
	this->maxSearchAreaHunter = max(this->maxSearchAreaHunter, maxSearchAreaHunter);

}

void AnimalSpecies::calculateMinProbabilityDensityFunction(double minProbabilityDensityFunction)
{
	this->minProbabilityDensityFunction = min(this->minProbabilityDensityFunction, minProbabilityDensityFunction);

}

void AnimalSpecies::calculateMaxProbabilityDensityFunction(double maxProbabilityDensityFunction)
{
	this->maxProbabilityDensityFunction = max(this->maxProbabilityDensityFunction, maxProbabilityDensityFunction);

}

void AnimalSpecies::calculateMinVorXVor(double minVorXVor)
{
	this->minVorXVor = min(this->minVorXVor, minVorXVor);

}

void AnimalSpecies::calculateMaxVorXVor(double maxVorXVor)
{
	this->maxVorXVor = max(this->maxVorXVor, maxVorXVor);

}


void AnimalSpecies::calculateMinSpeedRatio(double minSpeedRatio)
{
	this->minSpeedRatio = min(this->minSpeedRatio, minSpeedRatio);

}

void AnimalSpecies::calculateMaxSpeedRatio(double maxSpeedRatio)
{
	this->maxSpeedRatio = max(this->maxSpeedRatio, maxSpeedRatio);

}

void AnimalSpecies::sumMeanSizeHunted(double meanSizeHunted)
{
	this->meanSizeHunted += meanSizeHunted;
}

void AnimalSpecies::sumMeanVorHunted(double meanVorHunted)
{
	this->meanVorHunted += meanVorHunted;
}

void AnimalSpecies::sumSdSizeHunted(double sdSizeHunted)
{
	this->sdSizeHunted += pow(sdSizeHunted - meanSizeHunted, 2);
}

void AnimalSpecies::sumSdVorHunted(double sdVorHunted)
{
	this->sdVorHunted += pow(sdVorHunted - meanVorHunted, 2);
}

void AnimalSpecies::sumMeanSpeedHunted(double meanSpeedHunted)
{
	this->meanSpeedHunted += meanSpeedHunted;
}

void AnimalSpecies::sumSdSpeedHunted(double sdSpeedHunted)
{
	this->sdSpeedHunted += pow(sdSpeedHunted - meanSpeedHunted, 2);
}

void AnimalSpecies::sumMeanSizeHunter(double meanSizeHunter)
{
	this->meanSizeHunter += meanSizeHunter;
}

void AnimalSpecies::sumSdSizeHunter(double sdSizeHunter)
{
	this->sdSizeHunter += pow(sdSizeHunter - meanSizeHunter, 2);
}

void AnimalSpecies::sumMeanVorHunter(double meanVorHunter)
{
	this->meanVorHunter += meanVorHunter;
}

void AnimalSpecies::sumSdVorHunter(double sdVorHunter)
{
	this->sdVorHunter += pow(sdVorHunter - meanVorHunter, 2);
}

void AnimalSpecies::sumMeanSpeedHunter(double meanSpeedHunter)
{
	this->meanSpeedHunter += meanSpeedHunter;
}

void AnimalSpecies::sumSdSpeedHunter(double sdSpeedHunter)
{
	this->sdSpeedHunter += pow(sdSpeedHunter - meanSpeedHunter, 2);
}

void AnimalSpecies::sumMeanSearchAreaHunter(double meanSearchAreaHunter)
{
	this->meanSearchAreaHunter += meanSearchAreaHunter;
}

void AnimalSpecies::sumSdSearchAreaHunter(double sdSearchAreaHunter)
{
	this->sdSearchAreaHunter += pow(sdSearchAreaHunter - meanSearchAreaHunter, 2);
}

void AnimalSpecies::sumMeanProbabilityDensityFunction(double meanProbabilityDensityFunction)
{
	this->meanProbabilityDensityFunction += meanProbabilityDensityFunction;
}

void AnimalSpecies::sumSdProbabilityDensityFunction(double sdProbabilityDensityFunction)
{
	this->sdProbabilityDensityFunction += pow(sdProbabilityDensityFunction - meanProbabilityDensityFunction, 2);
}

void AnimalSpecies::sumMeanVorXVor(double meanVorXVor)
{
	this->meanVorXVor += meanVorXVor;
}

void AnimalSpecies::sumSdVorXVor(double sdVorXVor)
{
	this->sdVorXVor += pow(sdVorXVor - meanVorXVor, 2);
}

void AnimalSpecies::sumMeanSpeedRatio(double meanSpeedRatio)
{

	this->meanSpeedRatio += meanSpeedRatio;

}

void AnimalSpecies::sumSdSpeedRatio(double sdSpeedRatio)
{
	this->sdSpeedRatio += pow(sdSpeedRatio - meanSpeedRatio, 2);
}

void AnimalSpecies::sumMeanSizeRatio(double meanSizeRatio)
{
	this->meanSizeRatio += meanSizeRatio;
}

void AnimalSpecies::sumSdSizeRatio(double sdSizeRatio)
{
	this->sdSizeRatio += pow(sdSizeRatio - meanSizeRatio, 2);
}

void AnimalSpecies::calculateMeanSizeHunted(int population)
{
	this->meanSizeHunted = meanSizeHunted / population;
}

void AnimalSpecies::calculateSdSizeHunted(int population)
{
	this->sdSizeHunted = sqrt(sdSizeHunted / population);
}

void AnimalSpecies::calculateMeanVorHunted(int population)
{
	this->meanVorHunted = meanVorHunted / population;
}

void AnimalSpecies::calculateSdVorHunted(int population)
{
	this->sdVorHunted = sqrt(sdVorHunted / population);
}

void AnimalSpecies::calculateMeanSpeedHunted(int population)
{
	this->meanSpeedHunted = meanSpeedHunted / population;
}

void AnimalSpecies::calculateSdSpeedHunted(int population)
{
	this->sdSpeedHunted = sqrt(sdSpeedHunted / population);
}

void AnimalSpecies::calculateMeanSizeHunter(int population)
{
	this->meanSizeHunter = meanSizeHunter / population;
}

void AnimalSpecies::calculateSdSizeHunter(int population)
{
	this->sdSizeHunter = sqrt(sdSizeHunter / population);
}

void AnimalSpecies::calculateMeanVorHunter(int population)
{
	this->meanVorHunter = meanVorHunter / population;
}

void AnimalSpecies::calculateSdVorHunter(int population)
{
	this->sdVorHunter = sqrt(sdVorHunter / population);
}

void AnimalSpecies::calculateMeanSpeedHunter(int population)
{
	this->meanSpeedHunter = meanSpeedHunter / population;
}

void AnimalSpecies::calculateSdSpeedHunter(int population)
{
	this->sdSpeedHunter = sqrt(sdSpeedHunter / population);
}

void AnimalSpecies::calculateMeanSearchAreaHunter(int population)
{
	this->meanSearchAreaHunter = meanSearchAreaHunter / population;
}

void AnimalSpecies::calculateSdSearchAreaHunter(int population)
{
	this->sdSearchAreaHunter = sqrt(sdSearchAreaHunter / population);
}

void AnimalSpecies::calculateMeanProbabilityDensityFunction(int population)
{
	this->meanProbabilityDensityFunction = meanProbabilityDensityFunction / population;
}

void AnimalSpecies::calculateSdProbabilityDensityFunction(int population)
{
	this->sdProbabilityDensityFunction = sqrt(sdProbabilityDensityFunction / population);
}

void AnimalSpecies::calculateMeanVorXVor(int population)
{
	this->meanVorXVor = meanVorXVor / population;
}

void AnimalSpecies::calculateSdVorXVor(int population)
{
	this->sdVorXVor = sqrt(sdVorXVor / population);
}

void AnimalSpecies::calculateMeanSpeedRatio(int population)
{
	this->meanSpeedRatio = meanSpeedRatio / population;

	cout << "mean" << meanSpeedRatio << endl;
	cout << "pop" << population << endl;
}

void AnimalSpecies::calculateSdSpeedRatio(int population)
{
	this->sdSpeedRatio = sqrt(sdSpeedRatio / population);
}

void AnimalSpecies::calculateMeanSizeRatio(int population)
{
	this->meanSizeRatio = meanSizeRatio / population;
}

void AnimalSpecies::calculateSdSizeRatio(int population)
{
	this->sdSizeRatio = sqrt(sdSizeRatio / population);
}

void AnimalSpecies::setVonBertLinf(double vonBertLinf)
{
	this->vonBertLinf = vonBertLinf;
}

void AnimalSpecies::setVonBertKini(double vonBertKini)
{
	this->vonBertKini = vonBertKini;
}

void AnimalSpecies::setLinfKcorr(const double& LinfKcorr)
{
	this->LinfKcorr = LinfKcorr;
	cholMat = Math_Functions::cholesky(LinfKcorr);
}

void AnimalSpecies::setEggMassFromEquation(const bool& eggMassFromEquation)
{
	this->eggMassFromEquation = eggMassFromEquation;
	if(eggMassFromEquation)
	{
		eggDryMass = (forEggMassCoefficient * pow(femaleWetMass, forEggMassScale)) / getConversionToWetMass();
	}
}

void AnimalSpecies::setMaxPlasticityDueToConditionVor(const double& maxPlasticityDueToConditionVor)
{
	if (maxPlasticityDueToConditionVor > 0)
	{
		this->maxPlasticityDueToConditionVor = maxPlasticityDueToConditionVor;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "maxPlasticityDueToConditionVor must be a positive value. You entered " << maxPlasticityDueToConditionVor << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMinPlasticityDueToConditionVor(const double& minPlasticityDueToConditionVor)
{
	if (minPlasticityDueToConditionVor > 0)
	{
		this->minPlasticityDueToConditionVor = minPlasticityDueToConditionVor;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "minPlasticityDueToConditionVor must be a positive value. You entered " << minPlasticityDueToConditionVor << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMaxPlasticityDueToConditionSearch(const double& maxPlasticityDueToConditionSearch)
{
	if (maxPlasticityDueToConditionSearch > 0)
	{
		this->maxPlasticityDueToConditionSearch = maxPlasticityDueToConditionSearch;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "maxPlasticityDueToConditionSearch must be a positive value. You entered " << maxPlasticityDueToConditionSearch << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMinPlasticityDueToConditionSearch(const double& minPlasticityDueToConditionSearch)
{
	if (minPlasticityDueToConditionSearch > 0)
	{
		this->minPlasticityDueToConditionSearch = minPlasticityDueToConditionSearch;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "minPlasticityDueToConditionSearch must be a positive value. You entered " << minPlasticityDueToConditionSearch << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMaxPlasticityDueToConditionSpeed(const double& maxPlasticityDueToConditionSpeed)
{
	if (maxPlasticityDueToConditionSpeed > 0)
	{
		this->maxPlasticityDueToConditionSpeed = maxPlasticityDueToConditionSpeed;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "maxPlasticityDueToConditionSpeed must be a positive value. You entered " << maxPlasticityDueToConditionSpeed << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMinPlasticityDueToConditionSpeed(const double& minPlasticityDueToConditionSpeed)
{
	if (minPlasticityDueToConditionSpeed > 0)
	{
		this->minPlasticityDueToConditionSpeed = minPlasticityDueToConditionSpeed;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "minPlasticityDueToConditionSpeed must be a positive value. You entered " << minPlasticityDueToConditionSpeed << ". Exiting now" << std::endl;
		exit(-1);
	}
}

int AnimalSpecies::getRandomGender()
{
	int newAnimalGender = AnimalSpecies::GENDERS::MALE;
	if(Random::randomUniform() < sexRatio || sexualType == AnimalSpecies::SEXUAL_TYPES::ASEXUAL)
	{
		newAnimalGender = AnimalSpecies::GENDERS::FEMALE;
	}
	return newAnimalGender;
}

void AnimalSpecies::setExperienceInfluencePerDay(const float& experienceInfluencePerDay)
{
	if (experienceInfluencePerDay >= 0.0 && experienceInfluencePerDay < 1.0)
	{
		this->experienceInfluencePerDay = experienceInfluencePerDay;
	}
	else
	{
		std::cerr << "For the species '" << getScientificName() << "':" << std::endl;
		std::cerr << "experienceInfluencePerDay must be in the range [0.0 - 1.0). You entered " << experienceInfluencePerDay << ". Exiting now" << std::endl;
		exit(-1);
	}
}

void AnimalSpecies::setMassInfo(const float& conversionToWetMass, const bool& eggClutchFromEquation, const double& forClutchMassCoefficient,
					 			const double& forClutchMassScale, const double& forEggMassCoefficient, const double& forEggMassScale,
					 			const double& eggDryMass, const double& femaleWetMass, const bool& eggMassFromEquation)
{
	setConversionToWetMass(conversionToWetMass);
	setEggClutchFromEquation(eggClutchFromEquation);
	setForClutchMassCoefficient(forClutchMassCoefficient);
	setForClutchMassScale(forClutchMassScale);
	setForEggMassCoefficient(forEggMassCoefficient);
	setForEggMassScale(forEggMassScale);
	setEggDryMass(eggDryMass);
	setFemaleWetMass(femaleWetMass);
	//This setEggMassFromEquation must be used AFTER setting these previous 5 values!!
	setEggMassFromEquation(eggMassFromEquation);
}

void AnimalSpecies::setDefaultHuntingMode(unsigned int huntingMode)
{
	defaultHuntingMode = huntingMode;
}

void AnimalSpecies::initWetBiomassDensitiesPerAge(float temperature, int timeStepsPerDay)
{
	//double tmax = 273 + tempFromLab;

	double preGrowth = minTraitRanges[Trait::growth] + ((maxTraitRanges[Trait::growth] - minTraitRanges[Trait::growth])/2);

	double thisAnimalTempSizeRuleConstant = tempSizeRuleConstant;
	double degreesDifference = abs(temperature - tempFromLab);
	if(temperature > tempFromLab)
	{
		thisAnimalTempSizeRuleConstant = (-thisAnimalTempSizeRuleConstant);
	}

	double Linf = vonBertLdistanceMin+((vonBertLdistanceMax - vonBertLdistanceMin)/2);

	//Here the TSR rule is improved by directly affecting asymptotic body mass, not length.
	 //Here to calculate the new dev time we need to calculate the mass of the adult after TSR has been applied
	    double ageLastInstar = devTimeVector.back();

	    double maxLfromSpeciesLastInstar = Linf*(1-exp(-preGrowth*(ageLastInstar-getVonBertTime0())));

	    double massLastInstarForDevT = getCoefficientForMassAforMature()*pow(maxLfromSpeciesLastInstar,getScaleForMassBforMature());

	    double postTSRMassLastInstarForDevT = massLastInstarForDevT + thisAnimalTempSizeRuleConstant*degreesDifference*massLastInstarForDevT;

	    //transform to g in wetMass to adjust to the equation by Gillooly et al. 2012
	    double massLastInstarForDevTinG = getConversionToWetMass() * postTSRMassLastInstarForDevT * 1000;

	    double devTime = exp(-0.11 * (temperature / (1 + temperature / 273)) + getDevInter()) * pow(massLastInstarForDevTinG, 0.25);

		double finalDevTimeProportion = devTime / ageLastInstar;

	//Fixing t0 in BV

	//Now we do anchor t0 regardless of temperature
	double thisAnimalVonBertTime0 = vonBertTime0;

	double finalMatureAge = ageLastInstar*finalDevTimeProportion;

	for(double currentAge = 0; currentAge <= finalMatureAge; currentAge += 1.0/timeStepsPerDay)
	{
		double lengthAtCurrentAge = Linf*(1-exp(-preGrowth*(currentAge-thisAnimalVonBertTime0)));
		double massAtCurrentAge;
		if(currentAge<finalMatureAge){
		massAtCurrentAge = getCoefficientForMassA()*pow(lengthAtCurrentAge,getScaleForMassB());
		}else{
		massAtCurrentAge = getCoefficientForMassAforMature()*pow(lengthAtCurrentAge,getScaleForMassBforMature());
		}

		//TSR is applied directly here so the k value is effectively changed at the individual level
		massAtCurrentAge = massAtCurrentAge + massAtCurrentAge*thisAnimalTempSizeRuleConstant*degreesDifference;


		double wetMassAtCurrentAge = massAtCurrentAge*getConversionToWetMass();
		double density = forDensitiesA*pow(wetMassAtCurrentAge,forDensitiesB);
		density = wetMassAtCurrentAge * density;
		initialPopulation.push_back(density);
	}
}

void AnimalSpecies::scaleInitialPopulation(double totalPopulationDensity, double initialEcosystemSize)
{
	for (unsigned int i = 0; i < initialPopulation.size(); i++)
	{
		initialPopulation[i] = (initialPopulation[i]/totalPopulationDensity)*initialEcosystemSize;
	}
}

double AnimalSpecies::getTotalInitialPopulation()
{
	vector<double>::iterator it;
	double totalInitialPopulation = 0;
	for (it = initialPopulation.begin(); it != initialPopulation.end(); it++) {
		totalInitialPopulation += (*it);
	}
	return totalInitialPopulation;
}

unsigned long AnimalSpecies::getTotalStatisticsInitialPopulation()
{
	vector<unsigned long>::iterator it;
	unsigned long totalStatisticsInitialPopulation = 0;
	for (it = statisticsInitialPopulation.begin(); it != statisticsInitialPopulation.end(); it++) {
		totalStatisticsInitialPopulation += (*it);
	}
	return totalStatisticsInitialPopulation;
}



void AnimalSpecies::calculatePseudoGrowthMean()
{
	pseudoGrowthMean = pseudoGrowthMean / getTotalStatisticsInitialPopulation();
	cout << getScientificName() << "_pseudoGrowthMean=" << pseudoGrowthMean << endl;
}

void AnimalSpecies::calculatePseudoGrowthSd()
{
	pseudoGrowthSd = sqrt(pseudoGrowthSd / getTotalStatisticsInitialPopulation());
	cout << getScientificName() << "_pseudoGrowthSd=" << pseudoGrowthSd << endl;
}



void AnimalSpecies::interactionRanges(Edible* hunterAnimal, Edible* huntedAnimal, double muForPDF, double sigmaForPDF)
{
	calculateMinSizeHunted(huntedAnimal->getCurrentBodySize());
	calculateMaxSizeHunted(huntedAnimal->getCurrentBodySize());
	
    calculateMinVorHunted(huntedAnimal->getVoracity());
	calculateMaxVorHunted(huntedAnimal->getVoracity());
	
	calculateMinSpeedHunted(huntedAnimal->getSpeed());
	calculateMaxSpeedHunted(huntedAnimal->getSpeed());

	calculateMinSizeHunter(hunterAnimal->getCurrentBodySize());
	calculateMaxSizeHunter(hunterAnimal->getCurrentBodySize());
	
    calculateMinVorHunter(hunterAnimal->getVoracity());
	calculateMaxVorHunter(hunterAnimal->getVoracity());
	
	calculateMinSpeedHunter(hunterAnimal->getSpeed());
	calculateMaxSpeedHunter(hunterAnimal->getSpeed());

	double probabilityDensityFunction = exp(-0.5 * pow((log(hunterAnimal->calculateDryMass()/huntedAnimal->calculateDryMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
	calculateMinProbabilityDensityFunction(probabilityDensityFunction);
	calculateMaxProbabilityDensityFunction(probabilityDensityFunction);

	calculateMinVorXVor(hunterAnimal->getVoracity()*huntedAnimal->getVoracity());
	calculateMaxVorXVor(hunterAnimal->getVoracity()*huntedAnimal->getVoracity());



	if(huntedAnimal->getSpeed()<0.001){

		calculateMinSpeedRatio(hunterAnimal->getSpeed()/huntedAnimal->getSpeed());
		calculateMaxSpeedRatio(hunterAnimal->getSpeed()/huntedAnimal->getSpeed());

	}else{

		calculateMinSpeedRatio(hunterAnimal->getSpeed()/huntedAnimal->getSpeed());
		calculateMaxSpeedRatio(hunterAnimal->getSpeed()/huntedAnimal->getSpeed());

	}

	sumMeanSizeRatio(hunterAnimal->getCurrentBodySize() / huntedAnimal->getCurrentBodySize());
}
 

void AnimalSpecies::sumStatisticMeans(Edible* hunterAnimal, Edible* huntedAnimal, double muForPDF, double sigmaForPDF)
{
	sumMeanSizeHunted(huntedAnimal->getCurrentBodySize());
	sumMeanVorHunted(huntedAnimal->getVoracity());
	sumMeanSpeedHunted(huntedAnimal->getSpeed());
	sumMeanSizeHunter(hunterAnimal->getCurrentBodySize());
	sumMeanVorHunter(hunterAnimal->getVoracity());
	sumMeanSpeedHunter(hunterAnimal->getSpeed());
	double probabilityDensityFunction = exp(-0.5 * pow((log(hunterAnimal->calculateDryMass()/huntedAnimal->calculateDryMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
	sumMeanProbabilityDensityFunction(probabilityDensityFunction);
	sumMeanVorXVor(hunterAnimal->getVoracity() * huntedAnimal->getVoracity());

	//this is to avoid zeroes in the denominator
	if(huntedAnimal->getSpeed()<0.001){

		sumMeanSpeedRatio(hunterAnimal->getSpeed() / hunterAnimal->getSpeed());

	}else{

		sumMeanSpeedRatio(hunterAnimal->getSpeed() / huntedAnimal->getSpeed());

	}

	//double test=hunterAnimal->getSpeed() / huntedAnimal->getSpeed();
	//if(std::isnan(test)){
	//if(huntedAnimal->getSpeed()<0.0000000001){
	/*if(huntedAnimal->getId()==24600){
		cout << "hunter vs hunted speed" << endl;
		cout << hunterAnimal->getSpeed() << endl;
		cout << "id: " << huntedAnimal->getId() << endl;
		cout << huntedAnimal->getSpeed() << endl;
		cout << "dry mass :"<< huntedAnimal->calculateDryMass() << endl;
		cout << huntedAnimal->speed_ini << endl;
		cout << huntedAnimal->getSpeed() << endl;
		cout << "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ" << endl;
	}*/

	sumMeanSizeRatio(hunterAnimal->getCurrentBodySize() / huntedAnimal->getCurrentBodySize());
}

void AnimalSpecies::computeStatisticMeans(unsigned int numberOfAttacks)
{
	calculateMeanSizeHunted(numberOfAttacks);
	calculateMeanVorHunted(numberOfAttacks);
	calculateMeanSpeedHunted(numberOfAttacks);
	calculateMeanSizeHunter(numberOfAttacks);
	calculateMeanVorHunter(numberOfAttacks);
	calculateMeanSpeedHunter(numberOfAttacks);
	calculateMeanSearchAreaHunter(numberOfAttacks);
	calculateMeanProbabilityDensityFunction(numberOfAttacks);
	calculateMeanVorXVor(numberOfAttacks);
	calculateMeanSpeedRatio(numberOfAttacks);
	calculateMeanSizeRatio(numberOfAttacks);
}

void AnimalSpecies::sumStatisticSds(Edible* hunterAnimal, Edible* huntedAnimal, double muForPDF, double sigmaForPDF)
{
	sumSdSizeHunted(huntedAnimal->getCurrentBodySize());
	sumSdVorHunted(huntedAnimal->getVoracity());
	sumSdSpeedHunted(huntedAnimal->getSpeed());
	sumSdSizeHunter(hunterAnimal->getCurrentBodySize());
	sumSdVorHunter(hunterAnimal->getVoracity());
	sumSdSpeedHunter(hunterAnimal->getSpeed());
	double probabilityDensityFunction = exp(-0.5 * pow((log(hunterAnimal->calculateDryMass()/huntedAnimal->calculateDryMass()) - muForPDF) / sigmaForPDF, 2)) / (sigmaForPDF * sqrt(2*PI));
	sumSdProbabilityDensityFunction(probabilityDensityFunction);
	sumSdVorXVor(hunterAnimal->getVoracity() * huntedAnimal->getVoracity());
	sumSdSpeedRatio(hunterAnimal->getSpeed() / huntedAnimal->getSpeed());
	sumSdSizeRatio(hunterAnimal->getCurrentBodySize() / huntedAnimal->getCurrentBodySize());
}

void AnimalSpecies::computeStatisticSds(unsigned int numberOfAttacks)
{
	calculateSdSizeHunted(numberOfAttacks);
	calculateSdVorHunted(numberOfAttacks);
	calculateSdSpeedHunted(numberOfAttacks);
	calculateSdSizeHunter(numberOfAttacks);
	calculateSdVorHunter(numberOfAttacks);
	calculateSdSpeedHunter(numberOfAttacks);
	calculateSdSearchAreaHunter(numberOfAttacks);
	calculateSdProbabilityDensityFunction(numberOfAttacks);
	calculateSdVorXVor(numberOfAttacks);
	calculateSdSpeedRatio(numberOfAttacks);
	calculateSdSizeRatio(numberOfAttacks);
}

double AnimalSpecies::getMaxPredationProbability()
{
	return maxPredationProbability;
}

void AnimalSpecies::setMaxPredationProbability(double maxPredationProbability)
{
	this->maxPredationProbability = maxPredationProbability;
}

double AnimalSpecies::getMaxPredationIndividualWetMass()
{
	return maxPredationIndividualWetMass;
}

void AnimalSpecies::setMaxPredationIndividualWetMass(double maxPredationIndividualWetMass)
{
	this->maxPredationIndividualWetMass = maxPredationIndividualWetMass;
}

const double& AnimalSpecies::getTrait(const Trait& trait) const
{
	try
	{
		return fixedTraits.at(trait);
	}
	catch(const std::out_of_range& e)
	{
		try
		{
			return minTraitRanges.at(trait);
		}
		catch(const std::out_of_range& e)
		{
			throwLineInfoException("Trait " + to_string((unsigned int)trait) + " not found");
		}
	}
}

void AnimalSpecies::setVariableTraitsOrder(const vector<string>& order)
{
	for(unsigned int pos = 0; pos < order.size(); pos++)
	{
		getTrait(TraitConverter::stringToEnumValue(order[pos]));
		
		variableTraitsOrder[TraitConverter::stringToEnumValue(order[pos])] = pos;
	}
}

void AnimalSpecies::setTraits(const unordered_map<string,nlohmann::json>& fixedTraits, const unordered_map<string,double>& minTraitRanges, 
							  const unordered_map<string,double>& maxTraitRanges, const unordered_map<string,double>& minTraitLimits, 
							  const unordered_map<string,double>& maxTraitLimits, const vector<string>& order)
{
	setFixedTraits(fixedTraits);
	setTraitRanges(minTraitRanges, maxTraitRanges);
	setTraitLimits(minTraitLimits, maxTraitLimits);
	setVariableTraitsOrder(order);
}

void AnimalSpecies::setFixedTraits(const unordered_map<string,nlohmann::json>& fixedTraits)
{
	for(const auto& [key, value] : fixedTraits) {
		if(value.is_number_float()) {
			this->fixedTraits[TraitConverter::stringToEnumValue(key)] = value;
		}
	}
}

void AnimalSpecies::setTraitRanges(const unordered_map<string,double>& minTraitRanges, const unordered_map<string,double>& maxTraitRanges)
{
	setMinTraitRanges(minTraitRanges);
	setMaxTraitRanges(maxTraitRanges);
}

void AnimalSpecies::setMinTraitRanges(const unordered_map<string,double>& minTraitRanges)
{
	for(const auto& [key, value] : minTraitRanges)
	{
		this->minTraitRanges[TraitConverter::stringToEnumValue(key)] = value;
		this->minObservedPseudoValues[TraitConverter::stringToEnumValue(key)] = DBL_MAX;
	}
}

void AnimalSpecies::setMaxTraitRanges(const unordered_map<string,double>& maxTraitRanges)
{
	for(const auto& [key, value] : maxTraitRanges)
	{
		this->maxTraitRanges[TraitConverter::stringToEnumValue(key)] = value;
		this->maxObservedPseudoValues[TraitConverter::stringToEnumValue(key)] = 0.0;
	}
}

void AnimalSpecies::setTraitLimits(const unordered_map<string,double>& minTraitLimits, const unordered_map<string,double>& maxTraitLimits)
{
	setMinTraitLimits(minTraitLimits);
	setMaxTraitLimits(maxTraitLimits);
}

void AnimalSpecies::setMinTraitLimits(const unordered_map<string,double>& minTraitLimits)
{
	for(const auto& [key, value] : minTraitLimits)
	{
		this->minTraitLimits[TraitConverter::stringToEnumValue(key)] = value;
	}
}

void AnimalSpecies::setMaxTraitLimits(const unordered_map<string,double>& maxTraitLimits)
{
	for(const auto& [key, value] : maxTraitLimits)
	{
		this->maxTraitLimits[TraitConverter::stringToEnumValue(key)] = value;
	}
}

void AnimalSpecies::resetLimits()
{
	for(const auto& [key, value] : minTraitRanges)
	{
		minObservedPseudoValues[key] = DBL_MAX;
		maxObservedPseudoValues[key] = DBL_MIN;
	}
}

void AnimalSpecies::resetPseudoGrowthMean()
{
	pseudoGrowthMean = 0.0;
}

void AnimalSpecies::resetPseudoGrowthSd()
{
	pseudoGrowthSd = 0.0;
}

void AnimalSpecies::computeRestrictedTraits()
{
	for(const auto& [key, value] : minTraitRanges)
	{
		minRestrictedRanges[key] = minTraitRanges[key] + ((maxTraitRanges[key] - minTraitRanges[key]) * 0.5) * restrictPerTrait[getTraitOrder(key)];
		maxRestrictedRanges[key] = maxTraitRanges[key] - ((maxTraitRanges[key] - minTraitRanges[key]) * 0.5) * restrictPerTrait[getTraitOrder(key)];
	}
}
