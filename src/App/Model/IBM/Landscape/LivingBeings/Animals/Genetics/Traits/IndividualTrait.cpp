#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/IndividualTrait.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/PawarIndividualTraitTemperatureSection.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/TempSizeRuleIndividualTraitTemperatureSection.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TempSizeRuleTraitTemperatureSection.h"


using namespace std;





IndividualTrait::IndividualTrait()
{
	
}

IndividualTrait::IndividualTrait(Trait* trait, const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule, const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab)
    : trait(trait), constitutiveValue(trait->getValue()->getValue(genome, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule))
{
    #ifdef DEBUG
    phenotypicValueLastTimeStep = TimeStep(UINT_MAX);
    #endif

    if(trait->isThermallyDependent())
    {
        temperatureSection = trait->getTemperatureSection()->generateIndividualTraitTemperatureSection(constitutiveValue, genome, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule);
    
        phenotypicValue = std::visit([&](auto&& section) -> PreciseDouble {
            return section.applyTemperatureDependency(
                temperature, constitutiveValue, 
                coefficientForMassAforMature, 
                scaleForMassBforMature, 
                tempFromLab
            );
        }, temperatureSection);
    }
    else
    {
        phenotypicValue = constitutiveValue;
    }
}

IndividualTrait::IndividualTrait(Trait* trait, const Genome& genome, const size_t traitsPerModule, const size_t numberOfLociPerTrait, const std::vector<PreciseDouble>& rhoPerModule, const std::vector<size_t>& rhoRangePerModule)
    : trait(trait), constitutiveValue(trait->getValue()->getValue(genome, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule))
{
    if(trait->isThermallyDependent())
    {
        temperatureSection = trait->getTemperatureSection()->generateIndividualTraitTemperatureSection(constitutiveValue, genome, traitsPerModule, numberOfLociPerTrait, rhoPerModule, rhoRangePerModule);
    }
}

IndividualTrait::~IndividualTrait() 
{
	
}

#ifdef DEBUG
    void IndividualTrait::deserializeDebugVariables()
    {
        phenotypicValueLastTimeStep = TimeStep(UINT_MAX);
    }

    void IndividualTrait::testSetPhenotypicValue(const TimeStep actualTimeStep)
	{
		if(phenotypicValueLastTimeStep == actualTimeStep)
        {
            // throwLineInfoException("Variable modified twice in the same time step.");
        }

        phenotypicValueLastTimeStep = actualTimeStep;
	}
#endif

void IndividualTrait::setPhenotypicValue(const PreciseDouble& newValue, const TimeStep actualTimeStep)
{
	#ifdef DEBUG
		testSetPhenotypicValue(actualTimeStep);
	#endif

	if(!trait->canBeNegative())
	{
		if(newValue < 0.0)
		{
			throwLineInfoException("The trait will have a negative value.");
		}
	}

	phenotypicValue = newValue;
}

string IndividualTrait::to_string() const
{
    ostringstream content;

    content << constitutiveValue << "\t" << phenotypicValue;

    if(trait->isThermallyDependent())
    {
        content << "\t" << std::visit([&](auto&& section) -> string {
            return section.to_string();
        }, temperatureSection);
    }
    else
    {
        content << "\t" << PawarIndividualTraitTemperatureSection::to_string_NA();
    }

    

    return content.str();
}

IndividualLevelTrait::Type IndividualTrait::getType() const
{
	return trait->getValue()->getType();
}

const PreciseDouble& IndividualTrait::getConstitutiveValue() const
{
    return constitutiveValue;
}

const PreciseDouble& IndividualTrait::getPhenotypicValue() const
{
    return phenotypicValue;
}

void IndividualTrait::tune(const Temperature& temperature, const TimeStep actualTimeStep, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Temperature& tempFromLab)
{
    if(trait->isThermallyDependent())
    {
        PreciseDouble newValue = std::visit([&](auto&& section) -> PreciseDouble {
            return section.applyTemperatureDependency(
                temperature, constitutiveValue, 
                coefficientForMassAforMature, 
                scaleForMassBforMature,
                tempFromLab
            );
        }, temperatureSection);

        setPhenotypicValue(newValue, actualTimeStep);
    }
    else
    {
        setPhenotypicValue(constitutiveValue, actualTimeStep);
    }
}

void IndividualTrait::setTrait(Trait* newTrait)
{
    trait = newTrait;

    if(trait->isThermallyDependent())
    {
        std::visit([&](auto&& section) {
            using T = std::decay_t<decltype(section)>;
            if constexpr (std::is_same_v<T, TempSizeRuleIndividualTraitTemperatureSection>) {
                section.setTraitTemperatureSection(static_cast<const TempSizeRuleTraitTemperatureSection*>(trait->getTemperatureSection()));
            }
        }, temperatureSection);
    }
}


template <class Archive>
void IndividualTrait::serialize(Archive &ar, const unsigned int) {
	ar & constitutiveValue;
    ar & phenotypicValue;

    ar & temperatureSection;
} 

// Specialisation
template void IndividualTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void IndividualTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void IndividualTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void IndividualTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
