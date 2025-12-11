#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/ProbabilityDensityFunctionTrait.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




namespace {
    const bool registered = []() {
        Trait::registryGenerateTraits[Trait::ExecutionOrder::ProbabilityDensityFunction] = &ProbabilityDensityFunctionTrait::generateTraits;

		Trait::registryGetTraitStrVector[Trait::ExecutionOrder::ProbabilityDensityFunction] = &ProbabilityDensityFunctionTrait::getTraitStrVector;

		return true;
    }();
}


vector<Trait*> ProbabilityDensityFunctionTrait::generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& definitionConfig, const vector<json>& individualLevelTraitsOrder)
{
	vector<Trait*> probabilityDensityFunctionTraits;

	for(const ProbabilityDensityFunctionTraitType type : EnumClass<ProbabilityDensityFunctionTraitType>::getEnumValues())
	{
		probabilityDensityFunctionTraits.push_back(new ProbabilityDensityFunctionTrait(
			individualLevelTraits, type, definitionConfig[ProbabilityDensityFunctionTrait::getJsonFieldName()].at(EnumClass<ProbabilityDensityFunctionTraitType>::to_string(type)), 
			individualLevelTraitsOrder
		));
	}

	return probabilityDensityFunctionTraits;
}


vector<string> ProbabilityDensityFunctionTrait::getTraitStrVector()
{
	vector<string> traitStrVector;

	for(ProbabilityDensityFunctionTraitType type : EnumClass<ProbabilityDensityFunctionTraitType>::getEnumValues())
	{
		traitStrVector.push_back(EnumClass<ProbabilityDensityFunctionTraitType>::to_string(type));
	}

	return traitStrVector;
}




TraitTemperatureSection* ProbabilityDensityFunctionTrait::createTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& temperatureConfig, const vector<json>& individualLevelTraitsOrder, const string& traitStr, const string& fileName)
{
	if(temperatureConfig["temperature"]["dependent"].get<bool>())
	{
		return new PawarTraitTemperatureSection(individualLevelTraits, temperatureConfig["temperature"], individualLevelTraitsOrder, false, true, traitStr, fileName);
	}
	else
	{
		return nullptr;
	}
}




ProbabilityDensityFunctionTrait::ProbabilityDensityFunctionTrait()
{
	
}

ProbabilityDensityFunctionTrait::ProbabilityDensityFunctionTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const ProbabilityDensityFunctionTraitType type, const json& config, const vector<json>& individualLevelTraitsOrder)
	: Trait(
		individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<ProbabilityDensityFunctionTraitType>::to_string(type), 
		getFileName(),
		createTemperatureSection(
			individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<ProbabilityDensityFunctionTraitType>::to_string(type), 
			getFileName()
		)
	  )
{
	
}

ProbabilityDensityFunctionTrait::~ProbabilityDensityFunctionTrait() 
{
	
}


bool ProbabilityDensityFunctionTrait::canBeNegative() const
{
	return false;
}


string ProbabilityDensityFunctionTrait::getFileName() const
{
	return string("probabilityDensityFunction");
}

string ProbabilityDensityFunctionTrait::getParentTraitStr() const
{
	return string("probabilityDensityFunction");
}

string ProbabilityDensityFunctionTrait::getJsonFieldName()
{
	return string("probabilityDensityFunction");
}


BOOST_CLASS_EXPORT(ProbabilityDensityFunctionTrait)

template <class Archive>
void ProbabilityDensityFunctionTrait::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<Trait>(*this);
} 

// Specialisation
template void ProbabilityDensityFunctionTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ProbabilityDensityFunctionTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ProbabilityDensityFunctionTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ProbabilityDensityFunctionTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
