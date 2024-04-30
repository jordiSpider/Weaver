
#include "IBM/World/LivingBeings/Animals/Species/GrowthModule.h"

#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/World.h"

using namespace std;
using json = nlohmann::json;

GrowthModule::GrowthModule(const AnimalSpecies* const animalSpecies)
    : animalSpecies(animalSpecies)
{

}

GrowthModule::GrowthModule(const AnimalSpecies* const animalSpecies, const json& info)
    : animalSpecies(animalSpecies)
{
    setFemaleWetMass(info["femaleWetMass"]);
    setEggMass(info["eggMass"]);
    setGrowthModelType(info["growthModel"]);
    setIndeterminateGrowth(info["indeterminateGrowth"]);
}

GrowthModule::~GrowthModule()
{
    
}

const double& GrowthModule::getFemaleWetMass() const
{
    return femaleWetMass;
}

const double& GrowthModule::getEggDryMass() const
{
    return eggDryMass;
}

const IndividualGrowthModel::Type GrowthModule::getGrowthModelType() const
{
    return growthModelType;
}

bool GrowthModule::hasIndeterminateGrowth() const 
{ 
    return indeterminateGrowth; 
}

const Instar& GrowthModule::getInstarFirstReproduction() const 
{ 
    return instarFirstReproduction; 
}

const Instar& GrowthModule::getInstarFirstMaturation() const
{
    if(hasIndeterminateGrowth())
    {
        return getInstarFirstReproduction();
    }
    else
    {
        return getAnimalSpecies()->getLastInstar();
    }
}

void GrowthModule::setFemaleWetMass(const double& newFemaleWetMass)
{
    femaleWetMass = newFemaleWetMass;
}

void GrowthModule::setEggMass(const json& info)
{
    if(info["eggMassFromEquation"])
    {
        double coefficient = info["equationParameters"]["coefficient"];
        double scale = info["equationParameters"]["scale"];
        
        eggDryMass = (coefficient * pow(getFemaleWetMass(), scale)) / getAnimalSpecies()->getConversionToWetMass();
    }
    else
    {
        eggDryMass = info["value"];
    }
}

void GrowthModule::setGrowthModelType(const string& growthModelInfo)
{
    growthModelType = EnumClass<IndividualGrowthModel::Type>::stringToEnumValue(growthModelInfo);
}

void GrowthModule::setIndeterminateGrowth(const json& info)
{
    indeterminateGrowth = info["enabled"];

    if(indeterminateGrowth)
    {
        instarFirstReproduction = Instar(info["indeterminateGrowthParams"]["instarFirstReproduction"]);
    }
    else
    {
        instarFirstReproduction = Instar(getAnimalSpecies()->getNumberOfInstars());
    }
}

const AnimalSpecies* const GrowthModule::getAnimalSpecies() const
{
    return animalSpecies;
}
