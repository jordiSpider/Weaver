#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Genetics.h"



using namespace std;


Genetics::Genetics()
{

}

Genetics::Genetics(AnimalSpeciesGenetics* speciesGenetics, const Genome* const initialGenome, const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab)
	: speciesGenetics(speciesGenetics)
{
	if(initialGenome == nullptr) {
		genome.initFromOther(
			Genome(
				speciesGenetics->getLociPerTrait(), speciesGenetics->getRandomlyCreatedPositionsForChromosomes(), 
				speciesGenetics->getNumberOfChromosomes(), speciesGenetics->getNumberOfLociPerChromosome(), 
				speciesGenetics->getNumberOfChiasmasPerChromosome()
			), 
			speciesGenetics->getRandomlyCreatedPositionsForChromosomes()
		);
	}
	else {
		genome.initFromOther(*initialGenome, speciesGenetics->getRandomlyCreatedPositionsForChromosomes());
	}

	initTraits(temperature, actualTimeStep, coefficientForMassAforMature, scaleForMassBforMature, tempFromLab);
}

Genetics::Genetics(AnimalSpeciesGenetics* speciesGenetics, Gamete* const firstParentGamete, Gamete* const secondParentGamete, const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab)
	: speciesGenetics(speciesGenetics),
	  genome(
		firstParentGamete, secondParentGamete, speciesGenetics->getRandomlyCreatedPositionsForChromosomes(), 
	  	speciesGenetics->getNumberOfLociPerChromosome(), speciesGenetics->getNumberOfChiasmasPerChromosome()
	  )
{
	initTraits(temperature, actualTimeStep, coefficientForMassAforMature, scaleForMassBforMature, tempFromLab);
}

Genetics::~Genetics()
{
	
}


void Genetics::initTraits(const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab)
{
	allIndividualTraits.resize(EnumClass<Trait::ExecutionOrder>::size());

	for(Trait::ExecutionOrder order : EnumClass<Trait::ExecutionOrder>::getEnumValues())
	{
		for(Trait* trait : speciesGenetics->getAllTraits()[order])
		{
			allIndividualTraits[order].emplace_back(
				trait, getGenome(), speciesGenetics->getTraitsPerModule(), speciesGenetics->getNumberOfLociPerTrait(), 
				speciesGenetics->getRhoPerModuleVector(), speciesGenetics->getRhoRangePerModuleVector(), temperature,
				actualTimeStep, coefficientForMassAforMature, scaleForMassBforMature, tempFromLab
			);
		}
	}
}

#ifdef DEBUG
	void Genetics::deserializeDebugVariables()
	{
		for(Trait::ExecutionOrder order : EnumClass<Trait::ExecutionOrder>::getEnumValues())
		{
			for(IndividualTrait& trait : allIndividualTraits[order])
			{
				trait.deserializeDebugVariables();
			}
		}
	}
#endif

const Genome& Genetics::getGenome() const 
{ 
	return genome; 
}

Genome& Genetics::getMutableGenome() 
{ 
	return genome; 
}

string Genetics::to_string() const
{
	ostringstream content;

	content << allIndividualTraits[EnumClass<Trait::ExecutionOrder>::getEnumValues().at(0)][0].to_string();

	for(size_t i = 1; i < allIndividualTraits[EnumClass<Trait::ExecutionOrder>::getEnumValues().at(0)].size(); i++)
	{
		content << "\t" << allIndividualTraits[EnumClass<Trait::ExecutionOrder>::getEnumValues().at(0)][i].to_string();
	}


	for(size_t order = 1; order < EnumClass<Trait::ExecutionOrder>::size(); order++)
	{
		for(size_t i = 0; i < allIndividualTraits[EnumClass<Trait::ExecutionOrder>::getEnumValues().at(order)].size(); i++)
		{
			content << "\t" << allIndividualTraits[EnumClass<Trait::ExecutionOrder>::getEnumValues().at(order)][i].to_string();
		}
	}

	return content.str();
}

string Genetics::printTraits() const
{
	bool firstPrint = true;

	ostringstream content;

	for(size_t order = 0; order < EnumClass<Trait::ExecutionOrder>::size(); order++)
	{
		for(size_t i = 0; i < allIndividualTraits[EnumClass<Trait::ExecutionOrder>::getEnumValues().at(order)].size(); i++)
		{
			if(allIndividualTraits[EnumClass<Trait::ExecutionOrder>::getEnumValues().at(order)][i].getType() == IndividualLevelTrait::Type::IndividualLevel)
			{
				if(firstPrint)
				{
					content << allIndividualTraits[EnumClass<Trait::ExecutionOrder>::getEnumValues().at(order)][i].getConstitutiveValue();

					firstPrint = false;
				}
				else
				{
					content << "\t" << allIndividualTraits[EnumClass<Trait::ExecutionOrder>::getEnumValues().at(order)][i].getConstitutiveValue();
				}
			}
		}
	}

	return content.str();
}

void Genetics::deleteHomologousCorrelosomes() 
{ 
	genome.deleteHomologousCorrelosomes(); 
}

void Genetics::setAnimalSpeciesGenetics(AnimalSpeciesGenetics* newSpeciesGenetics)
{
	speciesGenetics = newSpeciesGenetics;

	for(Trait::ExecutionOrder order : EnumClass<Trait::ExecutionOrder>::getEnumValues())
	{
		for(size_t i = 0; i < allIndividualTraits[order].size(); i++)
		{
			allIndividualTraits[order][i].setTrait(speciesGenetics->getAllTraits()[order][i]);
		}
	}
}

const std::vector<IndividualTrait>& Genetics::getBaseIndividualTraits() const
{
	return allIndividualTraits[Trait::ExecutionOrder::Base];
}

const IndividualTrait& Genetics::getBaseIndividualTraits(const BaseTraitType type) const
{
	return allIndividualTraits[Trait::ExecutionOrder::Base][static_cast<size_t>(type)];
}

const std::vector<IndividualTrait>& Genetics::getPredationProbabilityWeightIndividualTraits() const
{
	return allIndividualTraits[Trait::ExecutionOrder::PredationProbabilityWeight];
}

const std::vector<IndividualTrait>& Genetics::getEscapeProbabilityWeightIndividualTraits() const
{
	return allIndividualTraits[Trait::ExecutionOrder::EscapeProbabilityWeight];
}

const std::vector<IndividualTrait>& Genetics::getEdibilityValueWeightIndividualTraits() const
{
	return allIndividualTraits[Trait::ExecutionOrder::EdibilityValueWeight];
}

const IndividualTrait& Genetics::getProbabilityDensityFunctionIndividualTraits(const ProbabilityDensityFunctionTraitType type) const
{
	return allIndividualTraits[Trait::ExecutionOrder::ProbabilityDensityFunction][static_cast<size_t>(type)];
}

const IndividualTrait& Genetics::getCellValueIndividualTraits(const CellValueTraitType type) const
{
	return allIndividualTraits[Trait::ExecutionOrder::CellValue][static_cast<size_t>(type)];
}

const IndividualTrait& Genetics::getPreferencesIndividualTraits(const PreferencesTraitType type) const
{
	return allIndividualTraits[Trait::ExecutionOrder::Preferences][static_cast<size_t>(type)];
}

Genome* Genetics::cloneGenome() const
{
	return genome.clone(speciesGenetics->getRandomlyCreatedPositionsForChromosomes());
}

void Genetics::tune(const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab)
{
	for(Trait::ExecutionOrder order : EnumClass<Trait::ExecutionOrder>::getEnumValues())
	{
		for(IndividualTrait& trait : allIndividualTraits[order])
		{
			trait.tune(temperature, actualTimeStep, coefficientForMassAforMature, scaleForMassBforMature, tempFromLab);
		}
	}
}


template <class Archive>
void Genetics::serialize(Archive &ar, const unsigned int) 
{
	ar & genome;

	ar & allIndividualTraits;
}

// Specialisation
template void Genetics::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Genetics::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Genetics::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Genetics::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
