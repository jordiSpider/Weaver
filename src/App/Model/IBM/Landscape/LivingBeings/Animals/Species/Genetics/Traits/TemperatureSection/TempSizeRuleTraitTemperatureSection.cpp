#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/TemperatureSection/TempSizeRuleTraitTemperatureSection.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/TempSizeRuleIndividualTraitTemperatureSection.h"


using namespace std;
using json = nlohmann::json;




TempSizeRuleTraitTemperatureSection::TempSizeRuleTraitTemperatureSection()
	: TraitTemperatureSection()
{

}

TempSizeRuleTraitTemperatureSection::TempSizeRuleTraitTemperatureSection(const json& config)
	: TraitTemperatureSection()
{
	for(const json& tempSizeRulePoint : config["tempSizeRuleVector"])
	{
		Temperature pointTemperature(tempSizeRulePoint["temperature"].get<double>());
		tempSizeRuleVector[pointTemperature] = DryMass(tempSizeRulePoint["dryMass"].get<double>());
	}
}

TempSizeRuleTraitTemperatureSection::~TempSizeRuleTraitTemperatureSection() 
{

}

const std::map<Temperature, DryMass>& TempSizeRuleTraitTemperatureSection::getTempSizeRuleVector() const
{
	return tempSizeRuleVector;
}

IndividualTraitTemperatureSection* TempSizeRuleTraitTemperatureSection::generateIndividualTraitTemperatureSection(const PreciseDouble&, const Genome&, const size_t, const size_t, const std::vector<PreciseDouble>&, const std::vector<size_t>&) const
{
	return new TempSizeRuleIndividualTraitTemperatureSection(this);
}

void TempSizeRuleTraitTemperatureSection::deserializeIndividualLevelTraits(std::vector<IndividualLevelTrait*>&)
{

}



BOOST_CLASS_EXPORT(TempSizeRuleTraitTemperatureSection)

template <class Archive>
void TempSizeRuleTraitTemperatureSection::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<TraitTemperatureSection>(*this);
	
	ar & tempSizeRuleVector;
}

// Specialisation
template void TempSizeRuleTraitTemperatureSection::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void TempSizeRuleTraitTemperatureSection::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void TempSizeRuleTraitTemperatureSection::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void TempSizeRuleTraitTemperatureSection::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
