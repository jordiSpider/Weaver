
#include "App/Model/IBM/Landscape/ArthropodsLandscape.h"

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




ArthropodsLandscape::ArthropodsLandscape()
    : Landscape()
{

}

ArthropodsLandscape::~ArthropodsLandscape()
{
	
}

PreciseDouble ArthropodsLandscape::calculateWetFood(const PreciseDouble& wetMass) const
{
    return Yodzis(wetMass);
}

PreciseDouble ArthropodsLandscape::getScaleMass() const
{
    return 1000.0;
}

DryMass ArthropodsLandscape::calculateMetabolicDryMassLossPerDay(const WetMass &wetMass, const PreciseDouble& proportionOfTimeTheAnimalWasMoving, const PreciseDouble& actE_metValue, const bool actE_metThermallyDependent, const PreciseDouble& met_rateValue, const bool met_rateThermallyDependent, const PreciseDouble&, const Temperature& tempFromLab, const Temperature& terrainCellTemperature, const PreciseDouble& conversionToWetMass) const
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

DryMass ArthropodsLandscape::calculateMaxVoracityTimeStep(const WetMass& wetMass, const PreciseDouble& conversionToWetMass, const bool hasCapitalBreeding, const DryMass& basalMetabolicDryMassLoss, const PreciseDouble& YodzisA, const PreciseDouble& YodzisB) const
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

PreciseDouble ArthropodsLandscape::calculatePostTSpeed(const PreciseDouble& speedAfterAllometric, const WetMass& wetMass) const
{
    // 25.5𝑀0.26(1−𝑒−22𝑀−0.6) Hirt et al. 2017
    return speedAfterAllometric*(1-exp(-22*(pow(wetMass.getValue(), -0.6))));
}

ArthropodsLandscape::Type ArthropodsLandscape::getType() const
{
    return Type::Arthropods;
}


BOOST_CLASS_EXPORT(ArthropodsLandscape)

template <class Archive>
void ArthropodsLandscape::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<Landscape>(*this);
}

// Specialisation
template void ArthropodsLandscape::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ArthropodsLandscape::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ArthropodsLandscape::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ArthropodsLandscape::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
