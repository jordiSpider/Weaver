#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/PawarTraitTemperatureSection.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/PawarIndividualTraitTemperatureSection.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




string PawarTraitTemperatureSection::getHeader(const string& trait)
{
	ostringstream header;

	header << "constitutive_" << trait << "_" << EnumClass<PawarElement>::to_string(EnumClass<PawarElement>::getEnumValues().at(0));

	for(size_t i = 1; i < EnumClass<PawarElement>::size(); i++)
	{
		header << "\t" << "constitutive_" << trait << "_" << EnumClass<PawarElement>::to_string(EnumClass<PawarElement>::getEnumValues().at(i));
	}

	header << "\t" << "constitutive_" << trait << "_Tmin";
    header << "\t" << "constitutive_" << trait << "_Tmax";

	return header.str();
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

bool PawarTraitTemperatureSection::isInsideRestrictedRanges(const CustomIndexedVector<PawarElement, PreciseDouble>& elementsValue) const
{
	bool isInsideRestrictedRanges = true;

	for(const PawarElement& elem : EnumClass<PawarElement>::getEnumValues())
	{
		isInsideRestrictedRanges = isInsideRestrictedRanges && elements[elem]->isInsideRestrictedRanges(elementsValue[elem]);
	}

	return isInsideRestrictedRanges;
}

IndividualTraitTemperatureSection* PawarTraitTemperatureSection::generateIndividualTraitTemperatureSection(const PreciseDouble& geneticValue, const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule) const
{
	CustomIndexedVector<PawarElement, PreciseDouble> elementsValue(EnumClass<PawarElement>::size(), 0.0);

	for(const PawarElement& elem : EnumClass<PawarElement>::getEnumValues())
	{
		elementsValue[elem] = elements[elem]->getValue(genome, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule);
	}

	return new PawarIndividualTraitTemperatureSection(this, elementsValue, geneticValue, isInverse(), isStrictlyPositive());
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
