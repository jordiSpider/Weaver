#ifndef WORLD_INTERFACE_H_
#define WORLD_INTERFACE_H_


#include <string>
#include <unordered_map>
#include <magic_enum.hpp>

#include "World/Map/Patches/Moisture/MoisturePatch.h"
#include "World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "World/LivingBeings/Animals/Animal.h"
#include "Misc/Macros.h"


class WorldInterface
{
public:
	WorldInterface();
	virtual ~WorldInterface();

    virtual void initializeAnimals()=0;
    virtual void evolveWorld()=0;
    virtual double getMassRatio() const=0;
    virtual void addAppliedMoisture(MoisturePatch &moisturePatch)=0;
    virtual const unsigned int getTimeStepsPerDay() const=0;
	virtual void printAnimalsAlongCells(const int day, const int simulationPoint) const=0;
    virtual double calculateNewBiomass(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const=0;
    virtual double calculateNewVoracity(const double &wetMass, const double &conversionToWetMass) const=0;
    virtual double calculateTotalMetabolicDryMassLoss(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const Animal* const animal) const=0;
    virtual const bool isDinosaurs() const;
    virtual bool getCompetitionAmongResourceSpecies() const=0;
    virtual const std::vector<const ResourceSpecies*>& getExistingResourceSpecies() const=0;
    virtual const std::vector<ResourceSpecies*>& getMutableExistingResourceSpecies() const=0;
    virtual const std::vector<AnimalSpecies*>& getMutableExistingAnimalSpecies() const=0;
    virtual const std::vector<const AnimalSpecies*>& getExistingAnimalSpecies() const=0;
    virtual const std::vector<const Species*>& getExistingSpecies() const=0;
    virtual bool getSaveAnimalConstitutiveTraits() const=0;
    virtual std::ofstream& getConstitutiveTraitsFile()=0;
    virtual bool isGrowthAndReproTest() const=0;
    virtual double getPredictedSpeed(const AnimalInterface* const animal) const=0;
    virtual void increaseMovePrintBar()=0;
};

#endif // WORLD_INTERFACE_H_
