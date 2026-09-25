#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/PawarIndividualTraitTemperatureSection.h"

#include "App/Model/IBM/Maths/MathFunctions.h"
#include "Misc/EnumClass.h"

#include <fmt/compile.h>
#include <fmt/format.h>
#include <iterator>

using namespace std;





PawarIndividualTraitTemperatureSection::PawarIndividualTraitTemperatureSection() = default;

PawarIndividualTraitTemperatureSection::PawarIndividualTraitTemperatureSection(const CustomIndexedVector<PawarElement, PreciseDouble>& elements, const PreciseDouble& geneticValue, bool inverse, bool strictlyPositive)
    : elements(elements), inverse(inverse), strictlyPositive(strictlyPositive)
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

void PawarIndividualTraitTemperatureSection::formatTraitDirect(fmt::memory_buffer& buf) const noexcept
{
    fmt::format_to(fmt::appender(buf), FMT_COMPILE("\t{}\t{}\t{}\t{}\t{}\t{}"),
        elements[PawarElement::activationEnergy],
        elements[PawarElement::energyDecay],
        elements[PawarElement::temperatureOptimal],
        elements[PawarElement::temperatureRef],
        temperatureRangeTPC.first.getTemperatureCelsius(),
        temperatureRangeTPC.second.getTemperatureCelsius()
    );
}



template <class Archive>
void PawarIndividualTraitTemperatureSection::serialize(Archive &ar, const unsigned int) {
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
