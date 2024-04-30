/*
 * Genome.cpp
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#include "Genome.h"

Genome::Genome(vector<pair<Chromosome*, Chromosome*> >* inheritedChromosomes, Species* mySpecies)
{
	this->mySpecies = mySpecies;
	homologousChromosomes = inheritedChromosomes;
	homologousCorrelosomes = NULL;
}

Genome::Genome(const Genome* otherGenome)
{
	this->mySpecies = otherGenome->getSpecies();
	homologousChromosomes = new vector<pair<Chromosome*, Chromosome*> >(otherGenome->getHomologousChromosomes()->size());
	for(size_t i = 0; i < otherGenome->getHomologousChromosomes()->size(); i++)
	{
		homologousChromosomes->at(i) = make_pair(otherGenome->getHomologousChromosomes()->at(i).first->clone(), otherGenome->getHomologousChromosomes()->at(i).second->clone());
	}
	homologousCorrelosomes = NULL;
}

Genome::~Genome() {
	for (auto it = homologousChromosomes->begin(); it != homologousChromosomes->end(); ++it)
	{
		delete (*it).first;
		delete (*it).second;
	}
	//Dinosaurs - I reverseded these two lines
	homologousChromosomes->clear();
	delete homologousChromosomes;

	if( homologousCorrelosomes != NULL )
	{
		for (auto it = homologousCorrelosomes->begin(); it != homologousCorrelosomes->end(); ++it)
		{
			delete (*it).first;
			delete (*it).second;
		}

		//Dinosaurs - I reverseded these two lines
		homologousCorrelosomes->clear();
		delete homologousCorrelosomes;
	}
}

// This is a convenience method to better manage memory. When you only
// need one of the gametes generated from meiosis, you keep it and discard the 
// rest by removing them from memory
Gamete * Genome::getRandomGameteFromMeiosis()
{
	unsigned int index = Random::randomIndex(4);
	vector<Gamete*> * vectorOfGametes = getGametesFromMeiosis();
	
	// Remove the selected gamete pointer from vector to avoid unwanted memory deletion
	Gamete * result = vectorOfGametes->at(index);
	vectorOfGametes->erase(vectorOfGametes->begin() + index);

	// Destroy the rest of unused gametes from memory
	for (auto it = vectorOfGametes->begin(); it != vectorOfGametes->end(); ++it)   	
	{
    	delete *it;
   	} 
   	vectorOfGametes->clear();
	delete vectorOfGametes;

	return result;
}

vector<Gamete*>* Genome::getGametesFromMeiosis()
{
	vector<Gamete*>* vectorOfGametes = new vector<Gamete*>(4);
	
	for(int i = 0; i < 4; i++)
	{
		vectorOfGametes->at(i) = new Gamete();
	}

	int upOrDown;
	//vector<Chromosome* >* chromatids;
	Chromosome* auxChromosome;
	for(vector<pair<Chromosome*, Chromosome*> >::iterator homologousChromosomesIt = homologousChromosomes->begin(); homologousChromosomesIt != homologousChromosomes->end(); homologousChromosomesIt++)
	{
		vector<Chromosome*>* chromatids;						  
		chromatids = new vector<Chromosome*>(4);

		//0 and 1 are left Chromosome left and right chromatids. 2 and 3 are right Chromosome left and right chromatids.
		chromatids->at(0) = (*homologousChromosomesIt).first->clone();
		chromatids->at(1) = (*homologousChromosomesIt).first->clone();
		chromatids->at(2) = (*homologousChromosomesIt).second->clone();
		chromatids->at(3) = (*homologousChromosomesIt).second->clone();

		//Recombination step 5 of 12 on animation
		//It only occurs between left-Chromosome-right-chromatid and right-Chromosome-left-chromatid
		//recombine(chromatids->at(1), chromatids->at(2), chiasmaStart, chiasmaEnd);

		//Creating the first and second options using self and mate chromosomes.
		unsigned int crossStartingPosition, crossChiasmaPosition;

		vector <int> possibleLociPositions(mySpecies->getNumberOfLociPerChromosome()-2);
		for (unsigned int i = 1; i < mySpecies->getNumberOfLociPerChromosome()-1; ++i)
		{
			possibleLociPositions.at(i-1) = i;
		}

		vector <int> valuesOfChiasmasPerChromosome;
		//Add the inital position for the algorithm to begin.
		valuesOfChiasmasPerChromosome.push_back(0);

		for (unsigned int i = 0; i < mySpecies->getNumberOfChiasmasPerChromosome(); ++i)
		{
			int randomLociIndex = Random::randomIndex(possibleLociPositions.size());
			//Generate chiasmas only in central positions, excluding initial and last position.
			valuesOfChiasmasPerChromosome.push_back(possibleLociPositions.at(randomLociIndex));
			possibleLociPositions.erase(possibleLociPositions.begin() + randomLociIndex);
		}
		//Add the last position for the algorithm to end.
		valuesOfChiasmasPerChromosome.push_back(mySpecies->getNumberOfLociPerChromosome()-1);
		sort(valuesOfChiasmasPerChromosome.begin(), valuesOfChiasmasPerChromosome.end());
		//Each chiasmas segments contains then numberOfChiasmasPerChromosome+2 positions (+2 because it includes begin and end)

		for (unsigned int currentChiasmaIndex = 0; currentChiasmaIndex < (mySpecies->getNumberOfChiasmasPerChromosome()+2)-1; currentChiasmaIndex+=2)
		{
			crossStartingPosition = valuesOfChiasmasPerChromosome[currentChiasmaIndex];
			crossChiasmaPosition = valuesOfChiasmasPerChromosome[currentChiasmaIndex+1];

			for(unsigned int i = crossStartingPosition; i < crossChiasmaPosition; ++i)
			{
				chromatids->at(1)->swapAlleles(i, chromatids->at(2), i);
			}
		}
		valuesOfChiasmasPerChromosome.clear();

		//Selecting randomly how they align on the plate (step 8 of 12)
		//This will switch chromosomes chromatids inside the vector
		upOrDown = Random::randomIndex(2);
		auxChromosome = chromatids->at(0);
		chromatids->at(0) = chromatids->at(upOrDown*2);
		chromatids->at(upOrDown*2) = auxChromosome;

		auxChromosome = chromatids->at(1);
		chromatids->at(1) = chromatids->at(1+(upOrDown*2));
		chromatids->at(1+(upOrDown*2)) = auxChromosome;

		//Selecting randomly how they tilt on metaphase 2 (step 9 of 12)
		//0 and 2 are upper gametes, 1 and 3 are lower gametes
		//left chromosome (after having aligned them on the plate)
		upOrDown = Random::randomIndex(2);
		vectorOfGametes->at(upOrDown)->pushChromosome(chromatids->at(0));
		vectorOfGametes->at((upOrDown+1)%2)->pushChromosome(chromatids->at(1));

		//right chromosome (after having aligned them on the plate)
		upOrDown = Random::randomIndex(2);
		vectorOfGametes->at(upOrDown+2)->pushChromosome(chromatids->at(2));
		vectorOfGametes->at(((upOrDown+1)%2)+2)->pushChromosome(chromatids->at(3));
	
  		chromatids->clear();
	    delete chromatids;
	}

	return vectorOfGametes;
}

Gamete* Genome::cloneFirstGameteFromHaploid() const
{
	Gamete * gameteFromHaploid = new Gamete();

	for(vector<pair<Chromosome*, Chromosome*> >::iterator homologousChromosomesIt = homologousChromosomes->begin(); homologousChromosomesIt != homologousChromosomes->end(); homologousChromosomesIt++)
	{
		gameteFromHaploid->pushChromosome((*homologousChromosomesIt).first->clone());
	}

	return gameteFromHaploid;
}

Gamete* Genome::cloneSecondGameteFromHaploid() const
{
	Gamete* gameteFromHaploid = new Gamete();
	for(vector<pair<Chromosome*, Chromosome*> >::iterator homologousChromosomesIt = homologousChromosomes->begin(); homologousChromosomesIt != homologousChromosomes->end(); homologousChromosomesIt++)
	{
		gameteFromHaploid->pushChromosome((*homologousChromosomesIt).second->clone());
	}
	return gameteFromHaploid;
}

const vector<pair<Chromosome*, Chromosome*> >* Genome::getHomologousChromosomes() const
{
	return homologousChromosomes;
}

const vector<pair<Correlosome*, Correlosome*> >* Genome::getHomologousCorrelosomes() const {
	return homologousCorrelosomes;
}

Genome* Genome::clone() const
{
	return new Genome(this);
}

void Genome::createHomologousCorrelosomes()
{
	homologousCorrelosomes = mySpecies->getChromosomesGenerator()->getCorrelosomesFromChromosomes(homologousChromosomes);
}

void Genome::deleteHomologousCorrelosomes()
{
	for (auto it = homologousCorrelosomes->begin(); it != homologousCorrelosomes->end(); ++it)
	{
		delete (*it).first;
		delete (*it).second;
	}
	homologousCorrelosomes->clear();
	delete homologousCorrelosomes;
	homologousCorrelosomes= NULL;
}
