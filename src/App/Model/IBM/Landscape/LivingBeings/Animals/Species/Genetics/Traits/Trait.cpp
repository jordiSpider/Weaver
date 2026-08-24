#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Trait.h"

#include <fmt/format.h>
#include <iterator>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




std::vector<Trait*> Trait::defaultGenerateTraits(std::vector<IndividualLevelTrait*>&, const nlohmann::json&, const std::vector<nlohmann::json>&)
{
	throwLineInfoException("Error: Method not defined for any of the subclasses.");
}

std::vector<std::string> Trait::defaultGetTraitStrVector()
{
	throwLineInfoException("Error: Method not defined for any of the subclasses.");
}










CustomIndexedVector<Trait::ExecutionOrder, std::vector<Trait*>> Trait::generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& definitionConfig, const vector<json>& individualLevelTraitsOrder)
{
	CustomIndexedVector<ExecutionOrder, std::vector<Trait*>> allTraits(EnumClass<ExecutionOrder>::size());

	for(ExecutionOrder order : EnumClass<ExecutionOrder>::getEnumValues())
	{
		allTraits[order] = registryGenerateTraits[order](individualLevelTraits, definitionConfig, individualLevelTraitsOrder);
	}

	return allTraits;
}

void Trait::getHeader(std::string& header)
{
	auto getTraitHeader = [](const string& trait, std::string& header) {
		fmt::format_to(std::back_inserter(header), "constitutive_{}_value\tphenotypic_{}_value", 
			trait, trait
		);

		TraitTemperatureSection::getHeader(trait, header);
    };


	vector<string> allTraitStrVector;

	for(auto &function : Trait::registryGetTraitStrVector)
	{
		vector<string> traitStrVector = function();

		allTraitStrVector.insert(allTraitStrVector.end(), traitStrVector.begin(), traitStrVector.end());
	}


	for(size_t i = 0; i < allTraitStrVector.size(); i++)
	{
		header.append("\t");
		getTraitHeader(allTraitStrVector[i], header);
	}
}


Trait::Trait()
{
	
}

Trait::Trait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& config, const vector<json>& individualLevelTraitsOrder, const string& traitStr, const string& fileName, TraitTemperatureSection* temperatureSection)
	: value(TraitDefinitionSection::createInstance(
		individualLevelTraits, config, individualLevelTraitsOrder, traitStr, "value", fileName
	  )), 
	  thermallyDependent(temperatureSection != nullptr), temperatureSection(temperatureSection)
{
	if(value->isNull())
	{
		if(temperatureSection)
		{
			thermallyDependent = false;
			delete temperatureSection;
			temperatureSection = nullptr;
		}
	}
}


Trait::~Trait() 
{
	delete value;

	if(isThermallyDependent())
	{
		delete temperatureSection;
	}
}

TraitDefinitionSection* Trait::getValue()
{
	return value;
}

TraitTemperatureSection* Trait::getTemperatureSection()
{
	return temperatureSection;
}

bool Trait::isThermallyDependent() const
{
	return thermallyDependent;
}

bool Trait::isNull() const
{
	return value->isNull();
}

void Trait::deserializeIndividualLevelTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits)
{
	value->deserializeIndividualLevelTraits(individualLevelTraits);
	
	temperatureSection->deserializeIndividualLevelTraits(individualLevelTraits);
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(Trait)

template <class Archive>
void Trait::serialize(Archive &ar, const unsigned int) {
	ar & value;

    ar & thermallyDependent;

	ar & temperatureSection;
} 


// Specialisation
template void Trait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Trait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Trait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Trait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
