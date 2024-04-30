
#include "World/DinosaursWorld.h"

using namespace std;
using json = nlohmann::json;



DinosaursWorld::DinosaursWorld(json * jsonTree, json &worldConfig, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath)
    : World(jsonTree, worldConfig, outputFolder, WeaverFolder, configPath, 1000.0)
{

}


DinosaursWorld::~DinosaursWorld()
{
	
}


double DinosaursWorld::calculateNewBiomass(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const
{
    double cellNumber = biomass / species->getCellMass();
    double newCellNumber = rateOfIncrease * cellNumber;
    return newCellNumber * species->getCellMass();
}


double DinosaursWorld::calculateNewVoracity(const double &wetMass, const double &conversionToWetMass) const
{
    double Garland1983 = ((152*pow((wetMass),0.738))/1000)/conversionToWetMass;

    return Garland1983;
}


double DinosaursWorld::calculateTotalMetabolicDryMassLoss(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const Animal* const animal) const
{
    //here Grady et al. 2014 provide results in Watts (j/s) and M in g
    double basalMetabolicTax = 0.002*pow(wetMass*1000,animal->getTrait(Trait::met_rate));

    double fraction_with_stress = (MathFunctions::linearInterpolate(animal->getTodayEncountersWithPredators(), 0, animal->getSpecies()->getMaxEncountersT(), 0, 0.407)/this->getTimeStepsPerDay())*basalMetabolicTax;

    //TODO multiplicar por timeStep
    //24*3600 because the metab rates are given in J/s.
    basalMetabolicTax = basalMetabolicTax + fraction_with_stress;
    
    double distanceMoved = proportionOfTimeTheAnimalWasMoving*animal->getTrait(Trait::search_area);
    double field_met_tax = distanceMoved*(10.7*pow(animal->calculateWetMass(),0.68)); //Calder 2016 for mammals - Jouls
    //field_met_tax = 0;
    //Remove this and use cost of transport in Calders
    //double loss_from_bmr = (1-proportionOfTimeTheAnimalWasMoving)*basalMetabolicTax*24*3600;
    //TODO This 3 is a raw value from bibl.
    //double field_met_tax = 3*basalMetabolicTax;
    //double loss_from_fmr = proportionOfTimeTheAnimalWasMoving*field_met_tax*24*3600;

    double loss_from_bmr = basalMetabolicTax*24*3600;
    double loss_from_fmr = field_met_tax;

    //7 is NOT referred to weeks. Conversion from Jules.
    loss_from_bmr=loss_from_bmr/7;//7 joule = 1mg 
    loss_from_fmr=loss_from_fmr/7;
    //here we transform from mg to Kg to dinoWeaver

    return ((loss_from_bmr + loss_from_fmr)*0.000001) / animal->getSpecies()->getConversionToWetMass();
}


const bool DinosaursWorld::isDinosaurs() const
{
    return true;
}


const double DinosaursWorld::getPdfThreshold() const
{
    return 0.0003;
}

double DinosaursWorld::getPredictedSpeed(const AnimalInterface* const animal) const
{
    return (animal->getSpeedAreaIni()*pow(animal->calculateWetMass(), animal->getSpecies()->getScaleForSpeed()))*(1-exp(-22*pow(animal->calculateWetMass(),-0.6)));  //25.5𝑀0.26(1−𝑒−22𝑀−0.6) Hirt et al. 2017
}
