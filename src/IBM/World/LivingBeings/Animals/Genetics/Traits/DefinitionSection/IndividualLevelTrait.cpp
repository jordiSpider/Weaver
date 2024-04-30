#include "IBM/World/LivingBeings/Animals/Genetics/Traits/DefinitionSection/IndividualLevelTrait.h"


using namespace std;
using json = nlohmann::json;


IndividualLevelTrait::IndividualLevelTrait(const json& info, const unsigned int order, const vector<Locus*> &loci, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const vector<double>& rhoPerModule, const vector<unsigned int>& rhoRangePerModule)
	: TraitDefinitionSection(), order(order),
      minTraitRange(info["ranges"]["min"]), 
	  maxTraitRange(info["ranges"]["max"]),
	  restrictValue(info["restrictValue"]),
	  minTraitRestrictedRange(minTraitRange + ((maxTraitRange - minTraitRange) * 0.5) * restrictValue),
	  maxTraitRestrictedRange(maxTraitRange - ((maxTraitRange - minTraitRange) * 0.5) * restrictValue),
	  minTraitLimit(getMinTraitRange() - (getMaxTraitRange() - getMinTraitRange()) * static_cast<double>(info["limits"]["min"])), 
	  maxTraitLimit(getMaxTraitRange() + (getMaxTraitRange() - getMinTraitRange()) * static_cast<double>(info["limits"]["max"]))
{
	setPseudoValueLimits(loci, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule);
}

IndividualLevelTrait::~IndividualLevelTrait() 
{

}

const unsigned int& IndividualLevelTrait::getOrder() const
{ 
	return order; 
}

const double& IndividualLevelTrait::getMinTraitRange() const
{ 
	return minTraitRange; 
}

const double& IndividualLevelTrait::getMaxTraitRange() const 
{ 
	return maxTraitRange; 
}

const double& IndividualLevelTrait::getMinTraitRestrictedRange() const 
{ 
	return minTraitRestrictedRange; 
}

const double& IndividualLevelTrait::getMaxTraitRestrictedRange() const 
{ 
	return maxTraitRestrictedRange; 
}

const double& IndividualLevelTrait::getMinTraitLimit() const 
{ 
	return minTraitLimit; 
}

const double& IndividualLevelTrait::getMaxTraitLimit() const 
{ 
	return maxTraitLimit; 
}

const double& IndividualLevelTrait::getMinPseudoValueLimit() const 
{ 
	return minPseudoValueLimit; 
}

const double& IndividualLevelTrait::getMaxPseudoValueLimit() const 
{ 
	return maxPseudoValueLimit; 
}

const double IndividualLevelTrait::calculatePseudoValue(const Genome& genome, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const vector<double>& rhoPerModule, const vector<unsigned int>& rhoRangePerModule) const
{
	//TODO Here each group can have 2 or 3 traits. This MUST be implemented.
	//Correlations of EACH group can be positive or negative. This is already implemented.
	//TODO Maybe, move this method to the class Genome so it will be all well organized (see variables involved).

	double traitPseudoValue = 0.0;

	unsigned int moduleNumber = getOrder() / traitsPerModule;

	//The division is made using RHO. For every trait, the left side alleles of their own chromosomes must be added.
	for(unsigned int j = 0; j < rhoRangePerModule[moduleNumber]; ++j) {
		if(genome.getHomologousCorrelosomes().at(getOrder()).first->getAllele(j)->getAlphabeticOrder() >= genome.getHomologousCorrelosomes().at(getOrder()).second->getAllele(j)->getAlphabeticOrder())
		{
			traitPseudoValue += genome.getHomologousCorrelosomes().at(getOrder()).first->getAllele(j)->getValue();
		}
		else
		{
			traitPseudoValue += genome.getHomologousCorrelosomes().at(getOrder()).second->getAllele(j)->getValue();
		}
	}

	//The right side depends on two factors: the sign for RHO for the current module and the dominance of the chromosome.
	unsigned int distanceFromDominant = getOrder() % traitsPerModule;

	//If RHO is positive, for every trait the right side alleles of the dominant chromosome must be added.
	if(rhoPerModule[moduleNumber] >= 0.0)
	{
		for (unsigned int j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
		{
			if(genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).first->getAllele(j)->getAlphabeticOrder() >= genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).second->getAllele(j)->getAlphabeticOrder())
			{
				traitPseudoValue += genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).first->getAllele(j)->getValue();
			}
			else
			{
				traitPseudoValue += genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).second->getAllele(j)->getValue();
			}

		}
	}
	//If RHO is negative.
	else {
		//If the trait IS dominant, the right side alleles of its chromosome must be added.
		if(distanceFromDominant == 0)
		{
			for (unsigned int j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
			{
				if(genome.getHomologousCorrelosomes().at(getOrder()).first->getAllele(j)->getAlphabeticOrder() >= genome.getHomologousCorrelosomes().at(getOrder()).second->getAllele(j)->getAlphabeticOrder())
				{
					traitPseudoValue += genome.getHomologousCorrelosomes().at(getOrder()).first->getAllele(j)->getValue();
				}
				else
				{
					traitPseudoValue += genome.getHomologousCorrelosomes().at(getOrder()).second->getAllele(j)->getValue();
				}
			}
		}
		//If the trait is NOT dominant, 1 - the right side alleles of the dominant chromosome must be added.
		else
		{
			for (unsigned int j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
			{
				if(genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).first->getAllele(j)->getAlphabeticOrder() >= genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).second->getAllele(j)->getAlphabeticOrder())
				{
					traitPseudoValue += (1.0 - genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).first->getAllele(j)->getValue());
				}
				else
				{
					traitPseudoValue += (1.0 - genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).second->getAllele(j)->getValue());
				}
			}
		}
	}

	return traitPseudoValue;
}

const double IndividualLevelTrait::getValue(const Genome& genome, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const vector<double>& rhoPerModule, const vector<unsigned int>& rhoRangePerModule) const
{
	double traitPseudoValue = calculatePseudoValue(genome, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule);

	#ifdef DEBUG
		if(traitPseudoValue < getMinPseudoValueLimit() || getMaxPseudoValueLimit() < traitPseudoValue)
		{
			throwLineInfoException("Error: Pseudo-value generated outside the limits.");
		}
	#endif

	// Interpolate trait value
	double traitValue = MathFunctions::linearInterpolate(
		traitPseudoValue, 
		getMinPseudoValueLimit(), getMaxPseudoValueLimit(),
		getMinTraitLimit(), getMaxTraitLimit()
	);

	return traitValue;
}

bool IndividualLevelTrait::isInsideRestrictedRanges(const double& traitValue) const
{
	return getMinTraitRestrictedRange() <= traitValue && traitValue <= getMaxTraitRestrictedRange();
}

const IndividualLevelTrait::Type IndividualLevelTrait::getType() const
{
	return Type::IndividualLevel;
}

void IndividualLevelTrait::setPseudoValueLimits(const vector<Locus*> &loci, const unsigned int traitsPerModule, const unsigned int numberOfLociPerTrait, const vector<double>& rhoPerModule, const vector<unsigned int>& rhoRangePerModule)
{
	minPseudoValueLimit = 0.0;
	maxPseudoValueLimit = 0.0;
	
	int moduleNumber = getOrder() / traitsPerModule;

	//The division is made using RHO. For every trait, the left side alleles of their own chromosomes must be added.
	for(unsigned int j = 0; j < rhoRangePerModule[moduleNumber]; ++j) {
		minPseudoValueLimit += loci[j]->getMinAlleleValue();
		maxPseudoValueLimit += loci[j]->getMaxAlleleValue();
	}

	//The right side depends on two factors: the sign for RHO for the current module and the dominance of the chromosome.
	unsigned int distanceFromDominant = getOrder() % traitsPerModule;

	//If RHO is positive, for every trait the right side alleles of the dominant chromosome must be added.
	if(rhoPerModule[moduleNumber] >= 0.0)
	{
		for (unsigned int j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
		{
			minPseudoValueLimit += loci[j]->getMinAlleleValue();
			maxPseudoValueLimit += loci[j]->getMaxAlleleValue();
		}
	}
	//If RHO is negative.
	else {
		//If the trait IS dominant, the right side alleles of its chromosome must be added.
		if(distanceFromDominant == 0)
		{
			for (unsigned int j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
			{
				minPseudoValueLimit += loci[j]->getMinAlleleValue();
				maxPseudoValueLimit += loci[j]->getMaxAlleleValue();
			}
		}
		//If the trait is NOT dominant, 1 - the right side alleles of the dominant chromosome must be added.
		else
		{
			for (unsigned int j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
			{
				minPseudoValueLimit += (1.0 - loci[j]->getMaxAlleleValue());
				maxPseudoValueLimit += (1.0 - loci[j]->getMinAlleleValue());
			}
		}
	}
}

template <class Archive>
void IndividualLevelTrait::serialize(Archive &ar, const unsigned int version) {
	ar & order;
	ar & minTraitRange;
	ar & maxTraitRange;
	ar & minTraitRestrictedRange;
	ar & maxTraitRestrictedRange;
	ar & minTraitLimit;
	ar & maxTraitLimit;
	ar & minPseudoValueLimit;
	ar & maxPseudoValueLimit;
	ar & restrictValue;
}
