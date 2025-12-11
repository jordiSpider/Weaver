
#include "App/Model/IBM/Landscape/DinosaursLandscape.h"

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




DinosaursLandscape::DinosaursLandscape()
    : Landscape()
{

}

DinosaursLandscape::~DinosaursLandscape()
{
	
}

PreciseDouble DinosaursLandscape::calculateWetFood(const PreciseDouble& wetMass) const
{
    return Garland1983(wetMass);
}

PreciseDouble DinosaursLandscape::getScaleMass() const
{
    return 1000000000.0;
}

DryMass DinosaursLandscape::calculateMetabolicDryMassLossPerDay(const WetMass &wetMass, const PreciseDouble& proportionOfTimeTheAnimalWasMoving, const PreciseDouble&, const bool, const PreciseDouble& met_rateValue, const bool, const PreciseDouble& search_areaValue, const Temperature&, const Temperature&, const PreciseDouble& conversionToWetMass) const
{
    //here Grady et al. 2014 provide results in Watts (j/s) and M in g
    PreciseDouble basalMetabolicTax = 0.002*pow(wetMass.getValue()*1000,met_rateValue);
    
    PreciseDouble distanceMoved = proportionOfTimeTheAnimalWasMoving*search_areaValue;
    PreciseDouble field_met_tax = distanceMoved*(10.7*pow(wetMass.getValue(),0.68)); //Calder 2016 for mammals - Jouls

    // TODO multiplicar por timeStep
    // 24*3600 because the metab rates are given in J/s.
    PreciseDouble loss_from_bmr = basalMetabolicTax*24*3600;
    PreciseDouble loss_from_fmr = field_met_tax;

    // 7 is NOT referred to weeks. Conversion from Jules.
    loss_from_bmr=loss_from_bmr/7; // 7 joule = 1mg 
    loss_from_fmr=loss_from_fmr/7;

    // here we transform from mg to Kg to dinoWeaver
    return DryMass(WetMass((loss_from_bmr + loss_from_fmr)*0.000001), conversionToWetMass);
}


bool DinosaursLandscape::isDinosaurs() const
{
    return true;
}

DryMass DinosaursLandscape::calculateMaxVoracityTimeStep(const WetMass& wetMass, const PreciseDouble& conversionToWetMass, const bool, const DryMass&, const PreciseDouble&, const PreciseDouble&) const
{
    return DryMass(DryMass(WetMass(Garland1983(wetMass.getValue())), conversionToWetMass).getValue()*getTimeStepsPerDay());
}

PreciseDouble DinosaursLandscape::calculatePostTSpeed(const PreciseDouble& speedAfterAllometric, const WetMass& wetMass) const
{
    // 25.5𝑀0.26(1−𝑒−22𝑀−0.6) Hirt et al. 2017
    return speedAfterAllometric*(1-exp(-22*(pow(wetMass.getValue(), -0.6))));
}

DinosaursLandscape::Type DinosaursLandscape::getType() const
{
    return Type::Dinosaurs;
}



BOOST_CLASS_EXPORT(DinosaursLandscape)

template <class Archive>
void DinosaursLandscape::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<Landscape>(*this);
}

// Specialisation
template void DinosaursLandscape::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void DinosaursLandscape::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void DinosaursLandscape::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void DinosaursLandscape::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
