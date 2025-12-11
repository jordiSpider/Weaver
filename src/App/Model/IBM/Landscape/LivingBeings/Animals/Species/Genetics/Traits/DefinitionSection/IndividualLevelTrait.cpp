#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/IndividualLevelTrait.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;


IndividualLevelTrait::IndividualLevelTrait()
	: TraitDefinitionSection()
{

}

IndividualLevelTrait::IndividualLevelTrait(const json& config, const size_t newOrder, const string& fileName, const string& traitStr, const string& elementStr)
	: TraitDefinitionSection(), order(newOrder),
      minTraitRange(config["ranges"]["min"].get<double>()), 
	  maxTraitRange(config["ranges"]["max"].get<double>()),
	  restrictValue(config["restrictValue"].get<double>()),
	  minTraitRestrictedRange(getMinTraitRange() + ((getMaxTraitRange() - getMinTraitRange()) * 0.5) * restrictValue),
	  maxTraitRestrictedRange(getMaxTraitRange() - ((getMaxTraitRange() - getMinTraitRange()) * 0.5) * restrictValue),
	  minTraitLimit(getMinTraitRestrictedRange() - (getMaxTraitRestrictedRange() - getMinTraitRestrictedRange()) * config["limits"]["min"].get<double>()), 
	  maxTraitLimit(getMaxTraitRestrictedRange() + (getMaxTraitRestrictedRange() - getMinTraitRestrictedRange()) * config["limits"]["max"].get<double>()),
	  fileName(fileName), traitStr(traitStr), elementStr(elementStr)
{
	
}

IndividualLevelTrait::~IndividualLevelTrait() 
{
	geneticFile.close();
}

void IndividualLevelTrait::initializeGeneticFile(const fs::path& geneticsFolder, const string& geneticHeader)
{
	createOutputFile(geneticFile, geneticsFolder, fileName, "txt");

	if(!geneticFile.is_open())
	{
		throwLineInfoException("Error opening the file '" + fileName + "'.");
	}

	geneticFile << geneticHeader;
	geneticFile.flush();
}

const size_t& IndividualLevelTrait::getOrder() const
{ 
	return order; 
}

const PreciseDouble& IndividualLevelTrait::getMinTraitRange() const
{ 
	return minTraitRange; 
}

const PreciseDouble& IndividualLevelTrait::getMaxTraitRange() const 
{ 
	return maxTraitRange; 
}

const PreciseDouble& IndividualLevelTrait::getMinTraitRestrictedRange() const 
{ 
	return minTraitRestrictedRange; 
}

const PreciseDouble& IndividualLevelTrait::getMaxTraitRestrictedRange() const 
{ 
	return maxTraitRestrictedRange; 
}

const PreciseDouble& IndividualLevelTrait::getMinTraitLimit() const 
{ 
	return minTraitLimit; 
}

const PreciseDouble& IndividualLevelTrait::getMaxTraitLimit() const 
{ 
	return maxTraitLimit; 
}

const PreciseDouble& IndividualLevelTrait::getMinPseudoValueLimit() const 
{ 
	return minPseudoValueLimit; 
}

const PreciseDouble& IndividualLevelTrait::getMaxPseudoValueLimit() const 
{ 
	return maxPseudoValueLimit; 
}

PreciseDouble IndividualLevelTrait::calculatePseudoValue(const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const vector<PreciseDouble>& rhoPerModule, const vector<size_t>& rhoRangePerModule) const
{
	//TODO Here each group can have 2 or 3 traits. This MUST be implemented.
	//Correlations of EACH group can be positive or negative. This is already implemented.
	//TODO Maybe, move this method to the class Genome so it will be all well organized (see variables involved).

	PreciseDouble traitPseudoValue = 0.0;

	size_t moduleNumber = getOrder() / traitsPerModule;

	//The division is made using RHO. For every trait, the left side alleles of their own chromosomes must be added.
	for(size_t j = 0; j < rhoRangePerModule[moduleNumber]; ++j) {
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
	size_t distanceFromDominant = getOrder() % traitsPerModule;

	//If RHO is positive, for every trait the right side alleles of the dominant chromosome must be added.
	if(rhoPerModule[moduleNumber] >= 0.0)
	{
		for (size_t j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
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
			for (size_t j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
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
			for (size_t j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
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

void IndividualLevelTrait::printGenetics(const ostringstream& animalInfo, const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const vector<PreciseDouble>& rhoPerModule, const vector<size_t>& rhoRangePerModule)
{
	geneticFile << animalInfo.str();


	size_t moduleNumber = getOrder() / traitsPerModule;

	//The division is made using RHO. For every trait, the left side alleles of their own chromosomes must be added.
	for(size_t j = 0; j < rhoRangePerModule[moduleNumber]; ++j) {
		geneticFile << "\t" << *(genome.getHomologousCorrelosomes().at(getOrder()).first->getAllele(j));
		geneticFile << "\t" << *(genome.getHomologousCorrelosomes().at(getOrder()).second->getAllele(j));
	}

	//The right side depends on two factors: the sign for RHO for the current module and the dominance of the chromosome.
	size_t distanceFromDominant = getOrder() % traitsPerModule;

	//If RHO is positive, for every trait the right side alleles of the dominant chromosome must be added.
	if(rhoPerModule[moduleNumber] >= 0.0)
	{
		for (size_t j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
		{
			geneticFile << "\t" << *(genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).first->getAllele(j));
			geneticFile << "\t" << *(genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).second->getAllele(j));
		}
	}
	//If RHO is negative.
	else {
		//If the trait IS dominant, the right side alleles of its chromosome must be added.
		if(distanceFromDominant == 0)
		{
			for (size_t j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
			{
				geneticFile << "\t" << *(genome.getHomologousCorrelosomes().at(getOrder()).first->getAllele(j));
				geneticFile << "\t" << *(genome.getHomologousCorrelosomes().at(getOrder()).second->getAllele(j));
			}
		}
		//If the trait is NOT dominant, 1 - the right side alleles of the dominant chromosome must be added.
		else
		{
			for (size_t j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
			{
				geneticFile << "\t" << *(genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).first->getAllele(j));
				geneticFile << "\t" << *(genome.getHomologousCorrelosomes().at(getOrder()-distanceFromDominant).second->getAllele(j));
			}
		}
	}

	geneticFile << "\n";

	geneticFile.flush();
}

PreciseDouble IndividualLevelTrait::getValue(const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const vector<PreciseDouble>& rhoPerModule, const vector<size_t>& rhoRangePerModule) const
{
	PreciseDouble traitPseudoValue = calculatePseudoValue(genome, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule);

	#ifdef DEBUG
		if(traitPseudoValue < getMinPseudoValueLimit() || getMaxPseudoValueLimit() < traitPseudoValue)
		{
			throwLineInfoException("Error: Pseudo-value generated outside the limits.");
		}
	#endif

	// Interpolate trait value
	PreciseDouble traitValue = MathFunctions::linearInterpolate(
		traitPseudoValue, 
		getMinPseudoValueLimit(), getMaxPseudoValueLimit(),
		getMinTraitLimit(), getMaxTraitLimit()
	);

	return traitValue;
}

bool IndividualLevelTrait::isInsideRestrictedRanges(const PreciseDouble& traitValue) const
{
	return getMinTraitRestrictedRange() <= traitValue && traitValue <= getMaxTraitRestrictedRange();
}

IndividualLevelTrait::Type IndividualLevelTrait::getType() const
{
	return Type::IndividualLevel;
}

const string& IndividualLevelTrait::getTraitStr() const
{
	return traitStr;
}

const string& IndividualLevelTrait::getElementStr() const
{
	return elementStr;
}

bool IndividualLevelTrait::isNull() const
{
	return false;
}

void IndividualLevelTrait::deserializeIndividualLevelTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits)
{
	individualLevelTraits.push_back(this);
}

void IndividualLevelTrait::setPseudoValueLimits(const vector<Locus> &loci, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const vector<PreciseDouble>& rhoPerModule, const vector<size_t>& rhoRangePerModule)
{
	minPseudoValueLimit = 0.0;
	maxPseudoValueLimit = 0.0;
	
	size_t moduleNumber = getOrder() / traitsPerModule;

	//The division is made using RHO. For every trait, the left side alleles of their own chromosomes must be added.
	for(size_t j = 0; j < rhoRangePerModule[moduleNumber]; ++j) {
		minPseudoValueLimit += loci[j].getMinAlleleValue();
		maxPseudoValueLimit += loci[j].getMaxAlleleValue();
	}

	//The right side depends on two factors: the sign for RHO for the current module and the dominance of the chromosome.
	size_t distanceFromDominant = getOrder() % traitsPerModule;

	//If RHO is positive, for every trait the right side alleles of the dominant chromosome must be added.
	if(rhoPerModule[moduleNumber] >= 0.0)
	{
		for (size_t j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
		{
			minPseudoValueLimit += loci[j].getMinAlleleValue();
			maxPseudoValueLimit += loci[j].getMaxAlleleValue();
		}
	}
	//If RHO is negative.
	else {
		//If the trait IS dominant, the right side alleles of its chromosome must be added.
		if(distanceFromDominant == 0)
		{
			for (size_t j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
			{
				minPseudoValueLimit += loci[j].getMinAlleleValue();
				maxPseudoValueLimit += loci[j].getMaxAlleleValue();
			}
		}
		//If the trait is NOT dominant, 1 - the right side alleles of the dominant chromosome must be added.
		else
		{
			for (size_t j = rhoRangePerModule[moduleNumber]; j < numberOfLociPerTrait; ++j)
			{
				minPseudoValueLimit += (1.0 - loci[j].getMaxAlleleValue());
				maxPseudoValueLimit += (1.0 - loci[j].getMinAlleleValue());
			}
		}
	}
}



BOOST_CLASS_EXPORT(IndividualLevelTrait)

template <class Archive>
void IndividualLevelTrait::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<TraitDefinitionSection>(*this);
	
	ar & order;
    ar & minTraitRange;
    ar & maxTraitRange;
    ar & restrictValue;
    ar & minTraitRestrictedRange;
    ar & maxTraitRestrictedRange;
    ar & minTraitLimit;
    ar & maxTraitLimit;

    ar & minPseudoValueLimit;
    ar & maxPseudoValueLimit;

	ar & fileName;

	ar & traitStr;
	ar & elementStr;
}

// Specialisation
template void IndividualLevelTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void IndividualLevelTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void IndividualLevelTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void IndividualLevelTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
