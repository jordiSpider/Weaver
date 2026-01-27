#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/AnimalSpeciesGenetics.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;


AnimalSpeciesGenetics::AnimalSpeciesGenetics()
{

}

AnimalSpeciesGenetics::AnimalSpeciesGenetics(const json &config)
	: numberOfLociPerTrait(config["numberOfLociPerTrait"].get<size_t>()),
	  numberOfAllelesPerLocus(config["numberOfAllelesPerLocus"].get<size_t>()), 
	  traitsPerModule(config["modules"]["traitsPerModule"].get<size_t>()),
	  numberOfChiasmasPerChromosome(config["numberOfChiasmasPerChromosome"].get<size_t>())
{
	setRhoPerModule(config["modules"]["correlationCoefficientRhoPerModule"]);

	if(getNumberOfChiasmasPerChromosome() % 2 != 0)
	{
		ostringstream message;
		message << "numberOfChiasmasPerChromosome must be multiple of 2. You entered " << numberOfChiasmasPerChromosome << " = " << numberOfChiasmasPerChromosome << ". Exiting now";

		throwLineInfoException(message.str());
	}

	rhoRangePerModule.reserve(rhoPerModule.size());
	for(size_t i = 0; i < rhoPerModule.size(); ++i)
	{
		rhoRangePerModule.push_back(static_cast<size_t>(round((1.0 - fabs(rhoPerModule[i])) * static_cast<double>(numberOfLociPerTrait)).getValue()));
	}


	setTraits(config["traits"], config["modifyAlleles"]);


	if(getNumberOfIndividualLevelTraits()/getTraitsPerModule() + (getNumberOfIndividualLevelTraits()%getTraitsPerModule() == 0) ? 0 : 1)
	{
		throwLineInfoException("Error: Item 'correlationCoefficientRhoPerModule' must have a size equal to the value of the arithmetic expression 'numberOfIndividualLevelTraits/traitsPerModule + (numberOfIndividualLevelTraits%traitsPerModule==0?0:1)'.");
	}
}

AnimalSpeciesGenetics::~AnimalSpeciesGenetics()
{
	for(Trait::ExecutionOrder order : EnumClass<Trait::ExecutionOrder>::getEnumValues())
	{
		for(Trait* trait : allTraits[order])
		{
			delete trait;
		}
	}
}

void AnimalSpeciesGenetics::setRhoPerModule(const vector<double>& correlationCoefficientRhoPerModule)
{
	for(const double correlationCoefficient : correlationCoefficientRhoPerModule)
	{
		rhoPerModule.emplace_back(correlationCoefficient);
	}
}


size_t AnimalSpeciesGenetics::getNumberOfLociPerTrait() const
{
	return numberOfLociPerTrait;
}


size_t AnimalSpeciesGenetics::getTraitsPerModule() const
{
	return traitsPerModule;
}


size_t AnimalSpeciesGenetics::getNumberOfChiasmasPerChromosome() const
{
	return numberOfChiasmasPerChromosome;
}


size_t AnimalSpeciesGenetics::getNumberOfLociPerChromosome() const
{
	return numberOfLociPerChromosome;
}


size_t AnimalSpeciesGenetics::getNumberOfChromosomes() const
{
	return numberOfChromosomes;
}


const std::vector<size_t>& AnimalSpeciesGenetics::getRhoRangePerModuleVector() const
{
	return rhoRangePerModule;
}


const std::vector<PreciseDouble>& AnimalSpeciesGenetics::getRhoPerModuleVector() const
{
	return rhoPerModule;
}


const std::vector<std::vector<Locus>>& AnimalSpeciesGenetics::getLociPerTrait() const 
{ 
	return lociPerTrait; 
}


const std::vector<size_t>& AnimalSpeciesGenetics::getRandomlyCreatedPositionsForChromosomes() const 
{ 
	return randomlyCreatedPositionsForChromosomes; 
}


const std::vector<IndividualLevelTrait*>& AnimalSpeciesGenetics::getIndividualLevelTraits() const
{
	return individualLevelTraits;
}


string AnimalSpeciesGenetics::generateGeneticHeader(const size_t numberOfLociPerTrait)
{
	ostringstream geneticHeader;

	geneticHeader << "id\tspecies\tg_numb_prt1\tg_numb_prt2\tID_prt1\tID_prt2";

	for(unsigned int numLocus = 1; numLocus <= numberOfLociPerTrait; numLocus++)
	{
		geneticHeader << "\t" << "first_locus_" << numLocus << "_value" << "\t" << "first_locus_" << numLocus << "_order";
		geneticHeader << "\t" << "second_locus_" << numLocus << "_value" << "\t" << "second_locus_" << numLocus << "_order";
	}

	geneticHeader << "\n";

	return geneticHeader.str();
}

void AnimalSpeciesGenetics::initializeGeneticFiles(const fs::path& geneticsFolder)
{
	fs::create_directories(geneticsFolder);

	string geneticHeader = AnimalSpeciesGenetics::generateGeneticHeader(getNumberOfLociPerTrait());

	for(IndividualLevelTrait* trait : individualLevelTraits)
	{
		trait->initializeGeneticFile(geneticsFolder, geneticHeader);
	}
}

void AnimalSpeciesGenetics::updatePseudoValueRanges(const Genome& genome)
{
	for(IndividualLevelTrait* trait : individualLevelTraits)
	{
		trait->updatePseudoValueRanges(
			genome, getTraitsPerModule(), getNumberOfLociPerTrait(),
			getRhoPerModuleVector(), getRhoRangePerModuleVector()
		);
	}
}

void AnimalSpeciesGenetics::setTraits(const json& traitsConfig, const json& modifyAlleles)
{
	allTraits = Trait::generateTraits(individualLevelTraits, traitsConfig["definition"], traitsConfig["individualLevelTraitsOrder"]);


	
	generateLociPerTrait(modifyAlleles);



	numberOfChromosomes = getNumberOfIndividualLevelTraits();


	randomlyCreatedPositionsForChromosomes.reserve(getNumberOfIndividualLevelTraits()*getNumberOfLociPerTrait());
	for(size_t i = 0; i < getNumberOfIndividualLevelTraits()*getNumberOfLociPerTrait(); ++i)
	{
		randomlyCreatedPositionsForChromosomes.emplace_back(i);
	}
	Random::shuffleVector(randomlyCreatedPositionsForChromosomes);


	//TODO Hay alguna restricción más para el parametrizador? Cual es el máximo de numberOfChromosomes??
	if(getNumberOfIndividualLevelTraits() == 0)
	{
		this->numberOfLociPerChromosome = 0;
	}
	else
	{
		if ((getNumberOfIndividualLevelTraits()*numberOfLociPerTrait) % numberOfChromosomes == 0)
		{
			this->numberOfLociPerChromosome = numberOfLociPerTrait;
		}
		else
		{
			ostringstream message;
			message << "numberOfTraits*numberOfLoci must be multiple of numberOfChromosomes. You entered " << getNumberOfIndividualLevelTraits() << "*" << numberOfLociPerTrait << " = " << getNumberOfIndividualLevelTraits()*numberOfLociPerTrait << " --> " << getNumberOfIndividualLevelTraits()*numberOfLociPerTrait << " % " << numberOfChromosomes << " = " << ((getNumberOfIndividualLevelTraits()*numberOfLociPerTrait) % numberOfChromosomes) << ". Exiting now";

			throwLineInfoException(message.str());
		}
	}
}

bool AnimalSpeciesGenetics::isGrowthTraitsThermallyDependent() const
{
	return getBaseTraits(BaseTraitType::growth)->isThermallyDependent() ||
        getBaseTraits(BaseTraitType::lengthAtMaturation)->isThermallyDependent() ||
        getBaseTraits(BaseTraitType::devTime)->isThermallyDependent();
}

void AnimalSpeciesGenetics::generateLociPerTrait(const json& modifyAlleles)
{
	if(lociPerTrait.size() != getIndividualLevelTraits().size())
	{
		lociPerTrait.resize(getIndividualLevelTraits().size());

		for(size_t i = 0; i < lociPerTrait.size(); i++)
		{
			lociPerTrait[i].resize(getNumberOfLociPerTrait());
		}
	}
	

	for(size_t i = 0; i < lociPerTrait.size(); i++)
	{
		for (size_t j = 0; j < getNumberOfLociPerTrait(); ++j)
		{
			Locus newLocus(getNumberOfAllelesPerLocus());
			lociPerTrait[i][j] = newLocus;
		}
	}


	for(const json& modification : modifyAlleles.get<vector<json>>())
	{
		vector<size_t> affectedLocusPositions;

		for(const PreciseDouble& percentage : modification["affectedLocus"].get<vector<double>>())
		{
			affectedLocusPositions.push_back(static_cast<size_t>(round(percentage * static_cast<double>(getNumberOfLociPerTrait()-1)).getValue()));
		}
		

		size_t affectedAllelesPosition = static_cast<size_t>(round(modification["affectedAlleles"].get<double>() * static_cast<double>(getNumberOfAllelesPerLocus()-1)));


		PreciseDouble timesAlleles = modification["timesAlleles"].get<double>();

		for(const json& affectedTrait : modification["affectedTraits"].get<vector<json>>())
		{
			IndividualLevelTrait* trait = nullptr;

			for(IndividualLevelTrait* elem : individualLevelTraits)
			{
				if(elem->getTraitStr() == affectedTrait["trait"].get<string>() && elem->getElementStr() == affectedTrait["element"].get<string>())
				{
					trait = elem;
				}
			}

			if(trait == nullptr)
			{
				throwLineInfoException("Error: The affected trait must be of type IndividualLevel.");
			}

			for(const size_t& locusPosition : affectedLocusPositions)
			{
				lociPerTrait[trait->getOrder()][locusPosition].modifyAllele(affectedAllelesPosition, timesAlleles);
			}
		}
	}
}

void AnimalSpeciesGenetics::deserializeIndividualLevelTraits(CustomIndexedVector<Trait::ExecutionOrder, std::vector<Trait*>>& allTraits, std::vector<IndividualLevelTrait*>& individualLevelTraits)
{
	for(Trait::ExecutionOrder order : EnumClass<Trait::ExecutionOrder>::getEnumValues())
	{
		for(Trait* trait : allTraits[order])
		{
			trait->deserializeIndividualLevelTraits(individualLevelTraits);
		}
	}
}

void AnimalSpeciesGenetics::printGenetics(const ostringstream& animalInfo, const Genome& genome) const
{
	for(IndividualLevelTrait* trait : individualLevelTraits)
	{
		trait->printGenetics(animalInfo, genome, getTraitsPerModule(), getNumberOfLociPerTrait(), getRhoPerModuleVector(), getRhoRangePerModuleVector());
	}
}

CustomIndexedVector<Trait::ExecutionOrder, std::vector<Trait*>>& AnimalSpeciesGenetics::getAllTraits()
{
	return allTraits;
}

const std::vector<Trait*>& AnimalSpeciesGenetics::getBaseTraits() const
{
	return allTraits[Trait::ExecutionOrder::Base];
}

const Trait* AnimalSpeciesGenetics::getBaseTraits(const BaseTraitType type) const
{
	return allTraits[Trait::ExecutionOrder::Base][static_cast<size_t>(type)];
}

const std::vector<Trait*>& AnimalSpeciesGenetics::getEdibilityValueWeightTraits() const
{
	return allTraits[Trait::ExecutionOrder::EdibilityValueWeight];
}

const std::vector<Trait*>& AnimalSpeciesGenetics::getEscapeProbabilityWeightTraits() const
{
	return allTraits[Trait::ExecutionOrder::EscapeProbabilityWeight];
}

const std::vector<Trait*>& AnimalSpeciesGenetics::getPredationProbabilityWeightTraits() const
{
	return allTraits[Trait::ExecutionOrder::PredationProbabilityWeight];
}

size_t AnimalSpeciesGenetics::getNumberOfAllelesPerLocus() const
{
	return numberOfAllelesPerLocus;
}


size_t AnimalSpeciesGenetics::getNumberOfIndividualLevelTraits() const 
{ 
	return individualLevelTraits.size(); 
}


template<class Archive>
void AnimalSpeciesGenetics::serialize(Archive & ar, const unsigned int)
{
	ar & numberOfLociPerTrait;
	ar & numberOfAllelesPerLocus;
	ar & traitsPerModule;

	ar & numberOfChromosomes;
	ar & numberOfChiasmasPerChromosome;
	ar & numberOfLociPerChromosome;

	ar & rhoPerModule;
	ar & rhoRangePerModule;

	ar & allTraits;

	if (Archive::is_loading::value)
	{
        AnimalSpeciesGenetics::deserializeIndividualLevelTraits(allTraits, individualLevelTraits);
    }

	ar & lociPerTrait;
	ar & randomlyCreatedPositionsForChromosomes;
}

// // Specialisation
template void AnimalSpeciesGenetics::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalSpeciesGenetics::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalSpeciesGenetics::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalSpeciesGenetics::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
