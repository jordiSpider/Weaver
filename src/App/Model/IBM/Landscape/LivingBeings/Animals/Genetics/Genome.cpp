#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Genome.h"

using namespace std;


Genome::Genome()
{

}

Genome::Genome(Gamete* const firstParentGamete, Gamete* const secondParentGamete, const vector<size_t> &randomlyCreatedPositionsForChromosomes, 
		   	   const size_t& numberOfLociPerChromosome, const size_t& numberOfChiasmasPerChromosome) 
	: numberOfLociPerChromosome(numberOfLociPerChromosome), numberOfChiasmasPerChromosome(numberOfChiasmasPerChromosome)
{
	homologousChromosomes.reserve(firstParentGamete->size());
	for (size_t i = 0; i < firstParentGamete->size(); ++i)
	{
		homologousChromosomes.emplace_back(make_pair(firstParentGamete->getChromosome(i), secondParentGamete->getChromosome(i)));
	}

	createHomologousCorrelosomesFromChromosomes(randomlyCreatedPositionsForChromosomes);
}

Genome::Genome(const vector<vector<Locus>> &lociPerTrait, const vector<size_t> &randomlyCreatedPositionsForChromosomes, const size_t &numberOfChromosomes, 
		   	   const size_t& numberOfLociPerChromosome, const size_t& numberOfChiasmasPerChromosome) 
	: numberOfLociPerChromosome(numberOfLociPerChromosome), numberOfChiasmasPerChromosome(numberOfChiasmasPerChromosome)
{
	homologousCorrelosomes.reserve(numberOfChromosomes);
	for (size_t i = 0; i < numberOfChromosomes; ++i)
	{
		homologousCorrelosomes.emplace_back(make_pair(new Correlosome(numberOfLociPerChromosome), new Correlosome(numberOfLociPerChromosome)));

		for (auto it = lociPerTrait[i].begin(); it < lociPerTrait[i].end(); it++)
		{
			homologousCorrelosomes.at(i).first->pushAllele(it->getAlleleRandomly());
			homologousCorrelosomes.at(i).second->pushAllele(it->getAlleleRandomly());
		}
	}

	homologousChromosomes.reserve(numberOfChromosomes);
	for (size_t i = 0; i < numberOfChromosomes; ++i)
	{
		homologousChromosomes.emplace_back(make_pair(new Chromosome(numberOfLociPerChromosome), new Chromosome(numberOfLociPerChromosome)));
	}

	//Here we select the Alelle we have to link, and we create in the chromosomes a pointer for each allele contained in the correlosomes.
	//Notice that the Alleles are not being duplicated, but their pointers are. This saves some memory.
	//This makes initTraits much more easy and understandable, and permits the initialization based on the correlosomes alleles, rather than based on the pure chromosomes.
	//Also the correlosomes are temporary, as they will be cleared after initTraits.
	for(size_t i = 0; i < randomlyCreatedPositionsForChromosomes.size(); ++i)
	{
		size_t positionFromCorrelosomes = randomlyCreatedPositionsForChromosomes.at(i);
		size_t selectedTraitOnCorrelosome = positionFromCorrelosomes/numberOfLociPerChromosome;
		size_t selectedLociOnCorrelosome = positionFromCorrelosomes%numberOfLociPerChromosome;

		size_t linkedChromosome = i/numberOfLociPerChromosome;

		const Allele* selectedAllele = homologousCorrelosomes.at(selectedTraitOnCorrelosome).first->getAllele(selectedLociOnCorrelosome);
		homologousChromosomes.at(linkedChromosome).first->pushAllele(selectedAllele);
		selectedAllele = homologousCorrelosomes.at(selectedTraitOnCorrelosome).second->getAllele(selectedLociOnCorrelosome);
		homologousChromosomes.at(linkedChromosome).second->pushAllele(selectedAllele);
	}
}

Genome::Genome(const Genome& otherGenome, const vector<size_t> &randomlyCreatedPositionsForChromosomes) : numberOfLociPerChromosome(otherGenome.numberOfLociPerChromosome), numberOfChiasmasPerChromosome(otherGenome.numberOfChiasmasPerChromosome)
{
	homologousChromosomes.reserve(otherGenome.getHomologousChromosomes().size());
	for(size_t i = 0; i < otherGenome.getHomologousChromosomes().size(); i++)
	{
		homologousChromosomes.emplace_back(make_pair(otherGenome.getHomologousChromosomes().at(i).first->clone(), otherGenome.getHomologousChromosomes().at(i).second->clone()));
	}

	createHomologousCorrelosomesFromChromosomes(randomlyCreatedPositionsForChromosomes);
}

void Genome::createHomologousCorrelosomesFromChromosomes(const vector<size_t> &randomlyCreatedPositionsForChromosomes)
{
	homologousCorrelosomes.reserve(homologousChromosomes.size());
	for (size_t i = 0; i < homologousChromosomes.size(); ++i)
	{
		homologousCorrelosomes.emplace_back(make_pair(new Correlosome(numberOfLociPerChromosome, false), new Correlosome(numberOfLociPerChromosome, false)));
	}

	for(size_t i = 0; i < randomlyCreatedPositionsForChromosomes.size(); ++i)
	{
		size_t positionFromCorrelosomes = randomlyCreatedPositionsForChromosomes.at(i);
		size_t selectedTraitOnCorrelosome = positionFromCorrelosomes/numberOfLociPerChromosome;
		size_t selectedLociOnCorrelosome = positionFromCorrelosomes%numberOfLociPerChromosome;

		size_t linkedChromosome = i/numberOfLociPerChromosome;
		size_t linkedLociOnChromosome = i%numberOfLociPerChromosome;

		const Allele* selectedAllele = homologousChromosomes.at(linkedChromosome).first->getAllele(linkedLociOnChromosome);
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

void Genome::initFromOther(const Genome& otherGenome, const std::vector<size_t> &randomlyCreatedPositionsForChromosomes)
{
	this->numberOfLociPerChromosome = otherGenome.numberOfLociPerChromosome;
	this->numberOfChiasmasPerChromosome = otherGenome.numberOfChiasmasPerChromosome;

	homologousChromosomes.reserve(otherGenome.getHomologousChromosomes().size());
	for(size_t i = 0; i < otherGenome.getHomologousChromosomes().size(); i++)
	{
		homologousChromosomes.emplace_back(make_pair(otherGenome.getHomologousChromosomes().at(i).first->clone(), otherGenome.getHomologousChromosomes().at(i).second->clone()));
	}

	createHomologousCorrelosomesFromChromosomes(randomlyCreatedPositionsForChromosomes);
}

Gamete* Genome::getGametesFromMeiosis(const size_t &indexSelectedGamete)
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

		vector <size_t> possibleLociPositions(numberOfLociPerChromosome-2);
		for (size_t i = 1; i < numberOfLociPerChromosome-1; ++i)
		{
			possibleLociPositions.at(i-1) = i;
		}

		vector <size_t> valuesOfChiasmasPerChromosome;
		//Add the inital position for the algorithm to begin.
		valuesOfChiasmasPerChromosome.push_back(0);

		for (size_t i = 0; i < numberOfChiasmasPerChromosome; ++i)
		{
			const size_t randomLociIndex = Random::randomIndex(possibleLociPositions.size());
			//Generate chiasmas only in central positions, excluding initial and last position.
			valuesOfChiasmasPerChromosome.push_back(possibleLociPositions.at(randomLociIndex));
			possibleLociPositions.erase(possibleLociPositions.begin() + static_cast<std::ptrdiff_t>(randomLociIndex));
		}
		//Add the last position for the algorithm to end.
		valuesOfChiasmasPerChromosome.push_back(numberOfLociPerChromosome-1);
		sort(valuesOfChiasmasPerChromosome.begin(), valuesOfChiasmasPerChromosome.end());
		//Each chiasmas segments contains then numberOfChiasmasPerChromosome+2 positions (+2 because it includes begin and end)

		for (unsigned int currentChiasmaIndex = 0; currentChiasmaIndex < (numberOfChiasmasPerChromosome+2)-1; currentChiasmaIndex+=2)
		{
			size_t crossStartingPosition = valuesOfChiasmasPerChromosome[currentChiasmaIndex];
			size_t crossChiasmaPosition = valuesOfChiasmasPerChromosome[currentChiasmaIndex+1];

			for(size_t i = crossStartingPosition; i < crossChiasmaPosition; ++i)
			{
				chromatids[1]->swapAlleles(i, chromatids[2], i);
			}
		}
		valuesOfChiasmasPerChromosome.clear();

		//Selecting randomly how they align on the plate (step 8 of 12)
		//This will switch chromosomes chromatids inside the vector
		size_t upOrDown = Random::randomIndex(2);
		Chromosome* auxChromosome = chromatids[0];
		chromatids[0] = chromatids[upOrDown*2];
		chromatids[upOrDown*2] = auxChromosome;

		auxChromosome = chromatids[1];
		chromatids[1] = chromatids[1+(upOrDown*2)];
		chromatids[1+(upOrDown*2)] = auxChromosome;

		//Selecting randomly how they tilt on metaphase 2 (step 9 of 12)
		//0 and 2 are upper gametes, 1 and 3 are lower gametes
		upOrDown = Random::randomIndex(2);
		size_t index;
		switch (upOrDown)
		{
		case 0:
			index = indexSelectedGamete;
			break;
		case 1:
			index = indexSelectedGamete + ((indexSelectedGamete % 2 == 0) ? 1 : -1);
			break;
		default:
			throwLineInfoException("Default case");
			break;
		}

		selectedGamete->pushChromosome(chromatids[index]);
		for(size_t i = 1; i < NUMBER_GAMETES_TO_GENERATE; i++) {
			delete chromatids[(index + i) % NUMBER_GAMETES_TO_GENERATE];
		}
	}

	return selectedGamete;
}

Gamete* Genome::getRandomGameteFromMeiosis() 
{ 
	return getGametesFromMeiosis(Random::randomIndex(NUMBER_GAMETES_TO_GENERATE)); 
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

const std::vector<std::pair<Chromosome*, Chromosome*> >& Genome::getHomologousChromosomes() const 
{ 
	return homologousChromosomes; 
}

const std::vector<std::pair<Correlosome*, Correlosome*> >& Genome::getHomologousCorrelosomes() const 
{ 
	return homologousCorrelosomes; 
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

Genome* Genome::clone(const std::vector<size_t> &randomlyCreatedPositionsForChromosomes) const 
{ 
	return new Genome(*this, randomlyCreatedPositionsForChromosomes); 
}



BOOST_CLASS_EXPORT(Genome)

template<class Archive>
void Genome::serialize(Archive & ar, const unsigned int) {
	ar & homologousChromosomes;
	ar & numberOfLociPerChromosome;
	ar & numberOfChiasmasPerChromosome;
}

// Specialisation
template void Genome::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Genome::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Genome::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Genome::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
