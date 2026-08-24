#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/PawarIndividualTraitTemperatureSection.h"

#include "App/Model/IBM/Maths/MathFunctions.h"
#include "Misc/EnumClass.h"

#include <fmt/format.h>
#include <iterator>

using namespace std;





void PawarTraitDTO::formatToBuffer(std::string& buffer) const
{
    fmt::format_to(std::back_inserter(buffer), "\t{}\t{}\t{}\t{}\t{}\t{}", 
        constitutiveActivationEnergy, constitutiveEnergyDecay,
        constitutiveTemperatureOptimal, constitutiveTemperatureRef,
        constitutiveTmin, constitutiveTmax
    );
}

void PawarTraitDTO::formatToBufferNA(std::string& buffer) const
{
    buffer.append("\tNA\tNA\tNA\tNA\tNA\tNA");
}





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

void PawarIndividualTraitTemperatureSection::flatten(PawarTraitDTO& dto) const noexcept
{
    dto.constitutiveActivationEnergy = elements[PawarElement::activationEnergy].getValue();
    dto.constitutiveEnergyDecay = elements[PawarElement::energyDecay].getValue();
    dto.constitutiveTemperatureOptimal = elements[PawarElement::temperatureOptimal].getValue();
    dto.constitutiveTemperatureRef = elements[PawarElement::temperatureRef].getValue();
    dto.constitutiveTmin = temperatureRangeTPC.first.getTemperatureCelsius().getValue();
    dto.constitutiveTmax = temperatureRangeTPC.second.getTemperatureCelsius().getValue();
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
