#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/Trait.h"


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

string Trait::getHeader()
{
	auto getTraitHeader = [](const string& trait) -> string {
		ostringstream header;

		header << "constitutive_" << trait << "_value";

		header << "\t" << "phenotypic_" << trait << "_value";

		header << "\t" << TraitTemperatureSection::getHeader(trait);

        return header.str();
    };


	vector<string> allTraitStrVector;

	for(auto &function : Trait::registryGetTraitStrVector)
	{
		vector<string> traitStrVector = function();

		allTraitStrVector.insert(allTraitStrVector.end(), traitStrVector.begin(), traitStrVector.end());
	}


	ostringstream header;

	header << getTraitHeader(allTraitStrVector[0]);

	for(size_t i = 1; i < allTraitStrVector.size(); i++)
	{
		header << "\t" << getTraitHeader(allTraitStrVector[i]);
	}

	return header.str();
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
