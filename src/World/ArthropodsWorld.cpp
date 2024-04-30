
#include "World/ArthropodsWorld.h"

using namespace std;
using json = nlohmann::json;



ArthropodsWorld::ArthropodsWorld(json * jsonTree, json &worldConfig, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath, int burnIn)
    : World(jsonTree, worldConfig, outputFolder, WeaverFolder, configPath, burnIn, 0.001)
{

}


ArthropodsWorld::~ArthropodsWorld()
{
	
}


double ArthropodsWorld::calculateNewBiomass(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const
{
    return rateOfIncrease * biomass;
}


double ArthropodsWorld::calculateNewVoracity(const double &wetMass, const double &conversionToWetMass) const
{
    double AmNat = (0.1*pow((wetMass),0.75))/conversionToWetMass;

    return AmNat;
}


double ArthropodsWorld::calculateTotalMetabolicDryMassLoss(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const Animal* const animal) const
{
    double basalMetabolicTax = exp(-7.2945+43.966*animal->getTrait(Trait::actE_met)+animal->getTrait(Trait::met_rate)*log(wetMass)-animal->getTrait(Trait::actE_met)*(1/((animal->getTerrainCellInterface()->getMoistureInfo()->getTemperature().getTemperatureKelvin())*BOLZMANN)));

    double fraction_with_stress = (MathFunctions::linearInterpolate(animal->getTodayEncountersWithPredators(), 0, animal->getSpecies()->getMaxEncountersT(), 0, 0.407)/this->getTimeStepsPerDay())*basalMetabolicTax;

    //TODO multiplicar por timeStep
    //24 because the metab rates are given in days.
    basalMetabolicTax = basalMetabolicTax + fraction_with_stress;
    double loss_from_bmr = (1-proportionOfTimeTheAnimalWasMoving)*basalMetabolicTax*24;
    //TODO This 3 is a raw value from bibl.
    double field_met_tax = 3*basalMetabolicTax;
    double loss_from_fmr = proportionOfTimeTheAnimalWasMoving*field_met_tax*24;

    //7 is NOT referred to weeks. Conversion from Jules.
    loss_from_bmr=loss_from_bmr/7;
    loss_from_fmr=loss_from_fmr/7;

    return (loss_from_bmr + loss_from_fmr) / animal->getSpecies()->getConversionToWetMass();
}

const double ArthropodsWorld::getPdfThreshold() const
{
    return 0.08;
}

double ArthropodsWorld::getPredictedSpeed(const AnimalInterface* const animal) const
{
    return (animal->getSpeedAreaIni()*pow(animal->calculateWetMass(), animal->getSpecies()->getScaleForSpeed()))*(1-exp(-22*pow(animal->calculateWetMass(),-0.6)));  //25.5𝑀0.26(1−𝑒−22𝑀−0.6) Hirt et al. 2017
}
