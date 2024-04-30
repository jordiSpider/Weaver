
#include "IBM/World/DinosaursWorld.h"

using namespace std;
using json = nlohmann::json;
namespace fs = boost::filesystem;



DinosaursWorld::DinosaursWorld(json * jsonTree, json &worldConfig, fs::path outputFolder, fs::path configPath, int burnIn)
    : World(jsonTree, worldConfig, outputFolder, configPath, burnIn, 1000.0)
{

}


DinosaursWorld::~DinosaursWorld()
{
	
}


double DinosaursWorld::calculateNewBiomassPerDay(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const
{
    double cellNumber = biomass / species->getCellMass();
    double newCellNumber = rateOfIncrease * cellNumber;
    return newCellNumber * species->getCellMass();
}

const double DinosaursWorld::calculateWetFood(const double& wetMass) const
{
    return Garland1983(wetMass);
}


double DinosaursWorld::calculateTotalMetabolicDryMassLossPerDay(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const AnimalNonStatistical* const animal) const
{
    //here Grady et al. 2014 provide results in Watts (j/s) and M in g
    double basalMetabolicTax = 0.002*pow(wetMass*1000,animal->getFinalTraitValue(Trait::Type::met_rate));
    
    double distanceMoved = proportionOfTimeTheAnimalWasMoving*animal->getFinalTraitValue(Trait::Type::search_area);
    double field_met_tax = distanceMoved*(10.7*pow(animal->calculateWetMass(),0.68)); //Calder 2016 for mammals - Jouls

    // TODO multiplicar por timeStep
    // 24*3600 because the metab rates are given in J/s.
    double loss_from_bmr = basalMetabolicTax*24*3600;
    double loss_from_fmr = field_met_tax;

    // 7 is NOT referred to weeks. Conversion from Jules.
    loss_from_bmr=loss_from_bmr/7; // 7 joule = 1mg 
    loss_from_fmr=loss_from_fmr/7;

    // here we transform from mg to Kg to dinoWeaver
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

template <class Archive>
void DinosaursWorld::serialize(Archive &ar, const unsigned int version) {

}

double DinosaursWorld::calculateMaxMassPredicted(const double& dryMass, const double& wetMass, const double& conversionToWetMass, const unsigned int numberOfTimeSteps, const bool hasCapitalBreeding, const double& minTotalMetabolicDryMassLoss, const double& newAAdult, const double& newB) const
{
    return dryMass + (Garland1983(wetMass)/conversionToWetMass)*(numberOfTimeSteps*getTimeStepsPerDay());
}

double DinosaursWorld::calculatePostTSpeed(const double& speedValue, const double& wetMass, const double& scaleForSpeed) const
{
    // 25.5𝑀0.26(1−𝑒−22𝑀−0.6) Hirt et al. 2017
    return (speedValue*pow(wetMass, scaleForSpeed))*(1-exp(-22*(pow(wetMass, -0.6))));
}
