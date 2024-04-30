/*
 * Genome.cpp
 *
 *  Created on: Aug 11, 2016
 *      Author: gabi
 */

#include "Genome.h"


#include <algorithm>

using namespace std;


Genome::Genome(Gamete* const firstParentGamete, Gamete* const secondParentGamete, const vector<int> &randomlyCreatedPositionsForChromosomes, 
		   	   const unsigned int& numberOfLociPerChromosome, const unsigned int& numberOfChiasmasPerChromosome) 
	: numberOfLociPerChromosome(numberOfLociPerChromosome), numberOfChiasmasPerChromosome(numberOfChiasmasPerChromosome)
{
	homologousChromosomes.reserve(firstParentGamete->size());
	for (unsigned int i = 0; i < firstParentGamete->size(); ++i)
	{
		homologousChromosomes.emplace_back(make_pair(firstParentGamete->getChromosome(i), secondParentGamete->getChromosome(i)));
	}

	createHomologousCorrelosomesFromChromosomes(randomlyCreatedPositionsForChromosomes);
}

Genome::Genome(const vector<Locus*> &loci, const vector<int> &randomlyCreatedPositionsForChromosomes, const int &numberOfChromosomes, 
		   	   const unsigned int& numberOfLociPerChromosome, const unsigned int& numberOfChiasmasPerChromosome) 
	: numberOfLociPerChromosome(numberOfLociPerChromosome), numberOfChiasmasPerChromosome(numberOfChiasmasPerChromosome)
{
	homologousCorrelosomes.reserve(numberOfChromosomes);
	for (unsigned int i = 0; i < numberOfChromosomes; ++i)
	{
		homologousCorrelosomes.emplace_back(make_pair(new Correlosome(numberOfLociPerChromosome), new Correlosome(numberOfLociPerChromosome)));

		for (auto it = loci.begin(); it < loci.end(); it++)
		{
			homologousCorrelosomes.at(i).first->pushAllele((*it)->getAlleleRandomly());
			homologousCorrelosomes.at(i).second->pushAllele((*it)->getAlleleRandomly());
		}
	}

	homologousChromosomes.reserve(numberOfChromosomes);
	for (unsigned int i = 0; i < numberOfChromosomes; ++i)
	{
		homologousChromosomes.emplace_back(make_pair(new Chromosome(numberOfLociPerChromosome), new Chromosome(numberOfLociPerChromosome)));
	}

	int positionFromCorrelosomes;
	int selectedTraitOnCorrelosome;
	int selectedLociOnCorrelosome;
	int linkedChromosome;
	int linkedLociOnChromosome;
	const Allele* selectedAllele;

	//Here we select the Alelle we have to link, and we create in the chromosomes a pointer for each allele contained in the correlosomes.
	//Notice that the Alleles are not being duplicated, but their pointers are. This saves some memory.
	//This makes initTraits much more easy and understandable, and permits the initialization based on the correlosomes alleles, rather than based on the pure chromosomes.
	//Also the correlosomes are temporary, as they will be cleared after initTraits.
	for(unsigned int i = 0; i < randomlyCreatedPositionsForChromosomes.size(); ++i)
	{
		positionFromCorrelosomes = randomlyCreatedPositionsForChromosomes.at(i);
		selectedTraitOnCorrelosome = positionFromCorrelosomes/numberOfLociPerChromosome;
		selectedLociOnCorrelosome = positionFromCorrelosomes%numberOfLociPerChromosome;

		linkedChromosome = i/numberOfLociPerChromosome;
		linkedLociOnChromosome = i%numberOfLociPerChromosome;

		selectedAllele = homologousCorrelosomes.at(selectedTraitOnCorrelosome).first->getAllele(selectedLociOnCorrelosome);
		homologousChromosomes.at(linkedChromosome).first->pushAllele(selectedAllele);
		selectedAllele = homologousCorrelosomes.at(selectedTraitOnCorrelosome).second->getAllele(selectedLociOnCorrelosome);
		homologousChromosomes.at(linkedChromosome).second->pushAllele(selectedAllele);
	}
}

Genome::Genome(const Genome& otherGenome, const vector<int> &randomlyCreatedPositionsForChromosomes) : numberOfLociPerChromosome(otherGenome.numberOfLociPerChromosome), numberOfChiasmasPerChromosome(otherGenome.numberOfChiasmasPerChromosome)
{
	homologousChromosomes.reserve(otherGenome.getHomologousChromosomes().size());
	for(size_t i = 0; i < otherGenome.getHomologousChromosomes().size(); i++)
	{
		homologousChromosomes.emplace_back(make_pair(otherGenome.getHomologousChromosomes().at(i).first->clone(), otherGenome.getHomologousChromosomes().at(i).second->clone()));
	}

	createHomologousCorrelosomesFromChromosomes(randomlyCreatedPositionsForChromosomes);
}

void Genome::createHomologousCorrelosomesFromChromosomes(const vector<int> &randomlyCreatedPositionsForChromosomes)
{
	homologousCorrelosomes.reserve(homologousChromosomes.size());
	for (size_t i = 0; i < homologousChromosomes.size(); ++i)
	{
		homologousCorrelosomes.emplace_back(make_pair(new Correlosome(numberOfLociPerChromosome, false), new Correlosome(numberOfLociPerChromosome, false)));
	}

	int positionFromCorrelosomes;
	int selectedTraitOnCorrelosome;
	int selectedLociOnCorrelosome;
	int linkedChromosome;
	int linkedLociOnChromosome;
	const Allele* selectedAllele;
	for(unsigned int i = 0; i < randomlyCreatedPositionsForChromosomes.size(); ++i)
	{
		positionFromCorrelosomes = randomlyCreatedPositionsForChromosomes.at(i);
		selectedTraitOnCorrelosome = positionFromCorrelosomes/numberOfLociPerChromosome;
		selectedLociOnCorrelosome = positionFromCorrelosomes%numberOfLociPerChromosome;

		linkedChromosome = i/numberOfLociPerChromosome;
		linkedLociOnChromosome = i%numberOfLociPerChromosome;

		selectedAllele = homologousChromosomes.at(linkedChromosome).first->getAllele(linkedLociOnChromosome);
		homologousCorrelosomes.at(selectedTraitOnCorrelosome).first->setAllele(selectedAllele, selectedLociOnCorrelosome);
		selectedAllele = homologousChromosomes.at(linkedChromosome).second->getAllele(linkedLociOnChromosome);
		homologousCorrelosomes.at(selectedTraitOnCorrelosome).second->setAllele(selectedAllele, selectedLociOnCorrelosome);
	}
}

Genome::~Genome() {
	for (auto it = homologousChromosomes.begin(); it != homologousChromosomes.end(); ++it)
	{
		delete (*it).first;
		delete (*it).second;
	}
	//Dinosaurs - I reverseded these two lines
	homologousChromosomes.clear();

	if( homologousCorrelosomes.size() != 0 )
	{
		for (auto it = homologousCorrelosomes.begin(); it != homologousCorrelosomes.end(); ++it)
		{
			delete (*it).first;
			delete (*it).second;
		}

		//Dinosaurs - I reverseded these two lines
		homologousCorrelosomes.clear();
	}
}

Gamete* Genome::getGametesFromMeiosis(const unsigned int &indexSelectedGamete)
{
	Gamete* selectedGamete = new Gamete(homologousChromosomes.size());

	Chromosome* chromatids[4];

	for(auto homologousChromosomesIt = homologousChromosomes.begin(); homologousChromosomesIt != homologousChromosomes.end(); homologousChromosomesIt++)
	{
		//0 and 1 are left Chromosome left and right chromatids. 2 and 3 are right Chromosome left and right chromatids.
		chromatids[0] = (*homologousChromosomesIt).first->clone();
		chromatids[1] = (*homologousChromosomesIt).first->clone();
		chromatids[2] = (*homologousChromosomesIt).second->clone();
		chromatids[3] = (*homologousChromosomesIt).second->clone();

		//Recombination step 5 of 12 on animation
		//It only occurs between left-Chromosome-right-chromatid and right-Chromosome-left-chromatid
		//recombine(chromatids->at(1), chromatids->at(2), chiasmaStart, chiasmaEnd);

		//Creating the first and second options using self and mate chromosomes.
		unsigned int crossStartingPosition, crossChiasmaPosition;

		vector <int> possibleLociPositions(numberOfLociPerChromosome-2);
		for (unsigned int i = 1; i < numberOfLociPerChromosome-1; ++i)
		{
			possibleLociPositions.at(i-1) = i;
		}

		vector <int> valuesOfChiasmasPerChromosome;
		//Add the inital position for the algorithm to begin.
		valuesOfChiasmasPerChromosome.push_back(0);

		for (unsigned int i = 0; i < numberOfChiasmasPerChromosome; ++i)
		{
			const unsigned int randomLociIndex = Random::randomIndex(possibleLociPositions.size());
			//Generate chiasmas only in central positions, excluding initial and last position.
			valuesOfChiasmasPerChromosome.push_back(possibleLociPositions.at(randomLociIndex));
			possibleLociPositions.erase(possibleLociPositions.begin() + randomLociIndex);
		}
		//Add the last position for the algorithm to end.
		valuesOfChiasmasPerChromosome.push_back(numberOfLociPerChromosome-1);
		sort(valuesOfChiasmasPerChromosome.begin(), valuesOfChiasmasPerChromosome.end());
		//Each chiasmas segments contains then numberOfChiasmasPerChromosome+2 positions (+2 because it includes begin and end)

		for (unsigned int currentChiasmaIndex = 0; currentChiasmaIndex < (numberOfChiasmasPerChromosome+2)-1; currentChiasmaIndex+=2)
		{
			crossStartingPosition = valuesOfChiasmasPerChromosome[currentChiasmaIndex];
			crossChiasmaPosition = valuesOfChiasmasPerChromosome[currentChiasmaIndex+1];

			for(unsigned int i = crossStartingPosition; i < crossChiasmaPosition; ++i)
			{
				chromatids[1]->swapAlleles(i, chromatids[2], i);
			}
		}
		valuesOfChiasmasPerChromosome.clear();

		//Selecting randomly how they align on the plate (step 8 of 12)
		//This will switch chromosomes chromatids inside the vector
		unsigned int upOrDown = Random::randomIndex(2);
		Chromosome* auxChromosome = chromatids[0];
		chromatids[0] = chromatids[upOrDown*2];
		chromatids[upOrDown*2] = auxChromosome;

		auxChromosome = chromatids[1];
		chromatids[1] = chromatids[1+(upOrDown*2)];
		chromatids[1+(upOrDown*2)] = auxChromosome;

		//Selecting randomly how they tilt on metaphase 2 (step 9 of 12)
		//0 and 2 are upper gametes, 1 and 3 are lower gametes
		upOrDown = Random::randomIndex(2);
		unsigned int index;
		switch (upOrDown)
		{
		case 0:
			index = indexSelectedGamete;
			break;
		case 1:
			index = indexSelectedGamete + (indexSelectedGamete % 2 == 0) ? 1 : -1;
			break;
		default:
			throwLineInfoException("Default case");
			break;
		}

		selectedGamete->pushChromosome(chromatids[index]);
		for(unsigned int i = 1; i < NUMBER_GAMETES_TO_GENERATE; i++) {
			delete chromatids[(index + i) % NUMBER_GAMETES_TO_GENERATE];
		}
	}

	return selectedGamete;
}

Gamete* Genome::cloneFirstGameteFromHaploid() const
{
	Gamete * gameteFromHaploid = new Gamete(homologousChromosomes.size());

	for(auto homologousChromosomesIt = homologousChromosomes.cbegin(); homologousChromosomesIt != homologousChromosomes.cend(); homologousChromosomesIt++)
	{
		gameteFromHaploid->pushChromosome((*homologousChromosomesIt).first->clone());
	}

	return gameteFromHaploid;
}

Gamete* Genome::cloneSecondGameteFromHaploid() const
{
	Gamete* gameteFromHaploid = new Gamete(homologousChromosomes.size());
	for(auto homologousChromosomesIt = homologousChromosomes.cbegin(); homologousChromosomesIt != homologousChromosomes.cend(); homologousChromosomesIt++)
	{
		gameteFromHaploid->pushChromosome((*homologousChromosomesIt).second->clone());
	}

	return gameteFromHaploid;
}

void Genome::deleteHomologousCorrelosomes()
{
	for (auto it = homologousCorrelosomes.begin(); it != homologousCorrelosomes.end(); ++it)
	{
		delete (*it).first;
		delete (*it).second;
	}
	homologousCorrelosomes.clear();
}
