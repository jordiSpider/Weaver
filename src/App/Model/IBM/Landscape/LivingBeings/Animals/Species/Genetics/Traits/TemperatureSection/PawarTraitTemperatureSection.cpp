#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/PawarTraitTemperatureSection.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/PawarIndividualTraitTemperatureSection.h"

#include <fmt/format.h>
#include <iterator>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




void PawarTraitTemperatureSection::getHeader(const string& trait, std::string& header)
{
	for(const PawarElement i : EnumClass<PawarElement>::getEnumValues())
	{
		fmt::format_to(std::back_inserter(header), "\tconstitutive_{}_{}", 
			trait, EnumClass<PawarElement>::to_string(i)
		);
	}

	fmt::format_to(std::back_inserter(header), "\tconstitutive_{}_Tmin\tconstitutive_{}_Tmax", 
		trait, trait
	);
}



PawarTraitTemperatureSection::PawarTraitTemperatureSection()
	: TraitTemperatureSection()
{

}

PawarTraitTemperatureSection::PawarTraitTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& config, const vector<json>& individualLevelTraitsOrder, const bool inverse, const bool strictlyPositive, const string& trait, const string& fileName)
	: TraitTemperatureSection(), inverse(inverse), strictlyPositive(strictlyPositive)
{
	elements.resize(EnumClass<PawarElement>::size(), nullptr);

	for(const PawarElement& elem : EnumClass<PawarElement>::getEnumValues())
	{
		elements[elem] = TraitDefinitionSection::createInstance(individualLevelTraits, config[EnumClass<PawarElement>::to_string(elem)], individualLevelTraitsOrder, trait, EnumClass<PawarElement>::to_string(elem), fileName);
	}
}

PawarTraitTemperatureSection::~PawarTraitTemperatureSection() 
{
	for(TraitDefinitionSection* element : elements)
	{
		delete element;
	}
}

bool PawarTraitTemperatureSection::isInverse() const
{
	return inverse;
}

bool PawarTraitTemperatureSection::isStrictlyPositive() const
{
	return strictlyPositive;
}

IndividualTraitTemperatureSectionVariant PawarTraitTemperatureSection::generateIndividualTraitTemperatureSection(const PreciseDouble& geneticValue, const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule) const
{
	CustomIndexedVector<PawarElement, PreciseDouble> elementsValue(EnumClass<PawarElement>::size(), 0.0);

	for(const PawarElement& elem : EnumClass<PawarElement>::getEnumValues())
	{
		elementsValue[elem] = elements[elem]->getValue(genome, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule);
	}

	return PawarIndividualTraitTemperatureSection(elementsValue, geneticValue, isInverse(), isStrictlyPositive());
}

void PawarTraitTemperatureSection::deserializeIndividualLevelTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits)
{
	for(const PawarElement& elem : EnumClass<PawarElement>::getEnumValues())
	{
		elements[elem]->deserializeIndividualLevelTraits(individualLevelTraits);
	}
}


BOOST_CLASS_EXPORT(PawarTraitTemperatureSection)

template <class Archive>
void PawarTraitTemperatureSection::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<TraitTemperatureSection>(*this);
	
	ar & elements;

    ar & inverse;
	ar & strictlyPositive;
}

// Specialisation
template void PawarTraitTemperatureSection::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void PawarTraitTemperatureSection::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void PawarTraitTemperatureSection::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void PawarTraitTemperatureSection::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
