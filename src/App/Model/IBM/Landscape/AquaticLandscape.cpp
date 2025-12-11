
#include "App/Model/IBM/Landscape/AquaticLandscape.h"

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




AquaticLandscape::AquaticLandscape()
    : Landscape()
{

}


AquaticLandscape::~AquaticLandscape()
{
	
}

PreciseDouble AquaticLandscape::calculateWetFood(const PreciseDouble& wetMass) const
{
    return Yodzis(wetMass);
}

PreciseDouble AquaticLandscape::getScaleMass() const
{
    return 1000.0;
}

DryMass AquaticLandscape::calculateMetabolicDryMassLossPerDay(const WetMass &wetMass, const PreciseDouble& proportionOfTimeTheAnimalWasMoving, const PreciseDouble& actE_metValue, const bool actE_metThermallyDependent, const PreciseDouble& met_rateValue, const bool met_rateThermallyDependent, const PreciseDouble&, const Temperature& tempFromLab, const Temperature& terrainCellTemperature, const PreciseDouble& conversionToWetMass) const
{
    const Temperature* temperature;

    if(met_rateThermallyDependent ||
       actE_metThermallyDependent)
    {
        temperature = &terrainCellTemperature;
    }
    else
    {
        temperature = &tempFromLab;
    }

    PreciseDouble basalMetabolicTax = exp(-7.2945+43.966*actE_metValue+met_rateValue*log(wetMass.getValue())-actE_metValue*(1/(temperature->getTemperatureKelvin()*BOLTZMANN)));
    
    // This correction for smaller animals produces metabolic rates as small as 4 times less
    if(wetMass < 5.2)
    {
        basalMetabolicTax = exp(-2.605722)*pow(basalMetabolicTax, 0.7961851);
    }

    // TODO multiplicar por timeStep
    // 24 because the metab rates are given in days.
    PreciseDouble loss_from_bmr = (1-proportionOfTimeTheAnimalWasMoving)*basalMetabolicTax*24;
    PreciseDouble field_met_tax = multiplierForFieldMetabolicRate*basalMetabolicTax;

    // TODO multiplicar por timeStep
    // 24 because the metab rates are given in days.
    PreciseDouble loss_from_fmr = proportionOfTimeTheAnimalWasMoving*field_met_tax*24;

    // 7 is NOT referred to weeks. Conversion from Jules.
    loss_from_bmr=loss_from_bmr/7;
    loss_from_fmr=loss_from_fmr/7;

    return DryMass(WetMass(loss_from_bmr + loss_from_fmr), conversionToWetMass);
}

DryMass AquaticLandscape::calculateMaxVoracityTimeStep(const WetMass& wetMass, const PreciseDouble& conversionToWetMass, const bool hasCapitalBreeding, const DryMass& basalMetabolicDryMassLoss, const PreciseDouble& YodzisA, const PreciseDouble& YodzisB) const
{
    if(hasCapitalBreeding)
    {
        return basalMetabolicDryMassLoss;
    }
    else
    {
        return DryMass(DryMass(WetMass(Yodzis(wetMass.getValue(), YodzisA, YodzisB)), conversionToWetMass).getValue()*getTimeStepsPerDay());
    }
}

PreciseDouble AquaticLandscape::calculatePostTSpeed(const PreciseDouble& speedAfterAllometric, const WetMass&) const
{
    return speedAfterAllometric;
}

AquaticLandscape::Type AquaticLandscape::getType() const
{
    return Type::Aquatic;
}




BOOST_CLASS_EXPORT(AquaticLandscape)

template <class Archive>
void AquaticLandscape::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<Landscape>(*this);
}

// Specialisation
template void AquaticLandscape::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AquaticLandscape::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AquaticLandscape::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AquaticLandscape::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
