
#include "IBM/World/AquaticWorld.h"

using namespace std;
using json = nlohmann::json;



AquaticWorld::AquaticWorld(json * jsonTree, json &worldConfig, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath, int burnIn)
    : World(jsonTree, worldConfig, outputFolder, WeaverFolder, configPath, burnIn, 0.001)
{

}


AquaticWorld::~AquaticWorld()
{
	
}

double AquaticWorld::calculateNewBiomass(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const
{
    return rateOfIncrease * biomass;
}


double AquaticWorld::calculateNewVoracity(const double &wetMass, const double &conversionToWetMass) const
{
    double AmNat = (0.1*pow((wetMass),0.75))/conversionToWetMass;

    return AmNat;
}


double AquaticWorld::calculateTotalMetabolicDryMassLoss(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const Animal* const animal) const
{
    double basalMetabolicTax = exp(-7.2945+43.966*animal->getTrait(Trait::actE_met)+animal->getTrait(Trait::met_rate)*log(wetMass)-animal->getTrait(Trait::actE_met)*(1/((animal->getTerrainCellInterface()->getMoistureInfo()->getTemperature().getTemperatureKelvin())*BOLTZMANN)));
    
    double loss_from_bmr = (1-proportionOfTimeTheAnimalWasMoving)*basalMetabolicTax*24;
    //TODO This 3 is a raw value from bibl.
    double field_met_tax = 3*basalMetabolicTax;
    double loss_from_fmr = proportionOfTimeTheAnimalWasMoving*field_met_tax*24;

    //7 is NOT referred to weeks. Conversion from Jules.
    loss_from_bmr=loss_from_bmr/7;
    loss_from_fmr=loss_from_fmr/7;

    return (loss_from_bmr + loss_from_fmr) / animal->getSpecies()->getConversionToWetMass();
}

const double AquaticWorld::getPdfThreshold() const
{
    return 0.08;
}

double AquaticWorld::getPredictedSpeed(const AnimalInterface* const animal) const
{
    const double a = 2.37;
    const double b = 0.35;

    return a*pow(animal->calculateWetMass(), b); // Peters 1983 - Pg 87
}

template <class Archive>
void AquaticWorld::serialize(Archive &ar, const unsigned int version) {

}
