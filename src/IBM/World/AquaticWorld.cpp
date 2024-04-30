
#include "IBM/World/AquaticWorld.h"

using namespace std;
using json = nlohmann::json;
namespace fs = boost::filesystem;



AquaticWorld::AquaticWorld(json * jsonTree, json &worldConfig, fs::path outputFolder, fs::path configPath, int burnIn)
    : World(jsonTree, worldConfig, outputFolder, configPath, burnIn, 0.001)
{

}


AquaticWorld::~AquaticWorld()
{
	
}

double AquaticWorld::calculateNewBiomassPerDay(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const
{
    return rateOfIncrease * biomass;
}

const double AquaticWorld::calculateWetFood(const double& wetMass) const
{
    return Yodzis(wetMass);
}


double AquaticWorld::calculateTotalMetabolicDryMassLossPerDay(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const AnimalNonStatistical* const animal) const
{
    const Temperature* temperature;

    if(animal->getSpecies()->getTrait(Trait::Type::met_rate).isThermallyDependent() ||
       animal->getSpecies()->getTrait(Trait::Type::actE_met).isThermallyDependent())
    {
        temperature = &animal->getTerrainCellInterface()->getMoistureInfo()->getTemperature();
    }
    else
    {
        temperature = &animal->getSpecies()->getTempFromLab();
    }

    double basalMetabolicTax = exp(-7.2945+43.966*animal->getFinalTraitValue(Trait::Type::actE_met)+animal->getFinalTraitValue(Trait::Type::met_rate)*log(wetMass)-animal->getFinalTraitValue(Trait::Type::actE_met)*(1/(temperature->getTemperatureKelvin()*BOLTZMANN)));
    
    // This correction for smaller animals produces metabolic rates as small as 4 times less
    if(wetMass < 5.2)
    {
        basalMetabolicTax = exp(-2.605722)*pow(basalMetabolicTax, 0.7961851);
    }

    // TODO multiplicar por timeStep
    // 24 because the metab rates are given in days.
    double loss_from_bmr = (1-proportionOfTimeTheAnimalWasMoving)*basalMetabolicTax*24;
    double field_met_tax = multiplierForFieldMetabolicRate*basalMetabolicTax;

    // TODO multiplicar por timeStep
    // 24 because the metab rates are given in days.
    double loss_from_fmr = proportionOfTimeTheAnimalWasMoving*field_met_tax*24;

    // 7 is NOT referred to weeks. Conversion from Jules.
    loss_from_bmr=loss_from_bmr/7;
    loss_from_fmr=loss_from_fmr/7;

    return (loss_from_bmr + loss_from_fmr) / animal->getSpecies()->getConversionToWetMass();
}

const double AquaticWorld::getPdfThreshold() const
{
    return 0.08;
}

template <class Archive>
void AquaticWorld::serialize(Archive &ar, const unsigned int version) {

}

double AquaticWorld::calculateMaxMassPredicted(const double& dryMass, const double& wetMass, const double& conversionToWetMass, const unsigned int numberOfTimeSteps, const bool hasCapitalBreeding, const double& minTotalMetabolicDryMassLoss, const double& newAAdult, const double& newB) const
{
    if(hasCapitalBreeding)
    {
        return dryMass + minTotalMetabolicDryMassLoss;
    }
    else
    {
        return dryMass + (Yodzis(wetMass, newAAdult, newB)/conversionToWetMass)*(numberOfTimeSteps*getTimeStepsPerDay());
    }
}

double AquaticWorld::calculatePostTSpeed(const double& speedValue, const double& wetMass, const double& scaleForSpeed) const
{
    return speedValue*pow(wetMass, scaleForSpeed);
}
