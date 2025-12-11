#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/PawarIndividualTraitTemperatureSection.h"


using namespace std;




string PawarIndividualTraitTemperatureSection::to_string_NA()
{
    ostringstream content;

    content << "NA";

    for(size_t i = 1; i < EnumClass<PawarElement>::size(); i++)
    {
        content << "\t" << "NA";
    }

    content << "\t" << "NA";
    content << "\t" << "NA";

    return content.str();
}



PawarIndividualTraitTemperatureSection::PawarIndividualTraitTemperatureSection()
    : IndividualTraitTemperatureSection()
{
	
}

PawarIndividualTraitTemperatureSection::PawarIndividualTraitTemperatureSection(const TraitTemperatureSection* traitTemperatureSection, const CustomIndexedVector<PawarElement, PreciseDouble>& elements, const PreciseDouble& geneticValue, bool inverse, bool strictlyPositive)
    : IndividualTraitTemperatureSection(traitTemperatureSection), elements(elements), inverse(inverse), strictlyPositive(strictlyPositive)
{
    temperatureRangeTPC = MathFunctions::calculateTemperatureRangePawar(
        geneticValue, Temperature(elements[PawarElement::temperatureOptimal]),
        elements[PawarElement::energyDecay], elements[PawarElement::activationEnergy],
        Temperature(elements[PawarElement::temperatureRef]), inverse, strictlyPositive
    );
}

PawarIndividualTraitTemperatureSection::~PawarIndividualTraitTemperatureSection() 
{
	
}


PreciseDouble PawarIndividualTraitTemperatureSection::applyTemperatureDependency(const Temperature& temperature, const PreciseDouble& traitValue,
        const PreciseDouble &, const PreciseDouble &, const Temperature&
    ) const
{
	return MathFunctions::use_Pawar_2018(
		temperature, traitValue, Temperature(elements[PawarElement::temperatureOptimal]),
		elements[PawarElement::energyDecay], elements[PawarElement::activationEnergy],
		Temperature(elements[PawarElement::temperatureRef]), inverse, strictlyPositive
	);
}

const PawarTraitTemperatureSection* PawarIndividualTraitTemperatureSection::getTraitTemperatureSection() const
{
    return static_cast<const PawarTraitTemperatureSection*>(IndividualTraitTemperatureSection::getTraitTemperatureSection());
}

bool PawarIndividualTraitTemperatureSection::isInsideRestrictedRanges() const
{
    return getTraitTemperatureSection()->isInsideRestrictedRanges(elements);
}

string PawarIndividualTraitTemperatureSection::to_string() const
{
    ostringstream content;

    content << elements[static_cast<PawarElement>(0)];

    for(size_t i = 1; i < elements.size(); i++)
    {
        content << "\t" << elements[static_cast<PawarElement>(i)];
    }

    content << "\t" << temperatureRangeTPC.first.getTemperatureCelsius();
    content << "\t" << temperatureRangeTPC.second.getTemperatureCelsius();

    return content.str();
}



BOOST_CLASS_EXPORT(PawarIndividualTraitTemperatureSection)

template <class Archive>
void PawarIndividualTraitTemperatureSection::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<IndividualTraitTemperatureSection>(*this);

    ar & elements;

    ar & inverse;
    ar & strictlyPositive;

    ar & temperatureRangeTPC;
} 

// Specialisation
template void PawarIndividualTraitTemperatureSection::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void PawarIndividualTraitTemperatureSection::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void PawarIndividualTraitTemperatureSection::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void PawarIndividualTraitTemperatureSection::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
