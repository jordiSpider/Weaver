#ifndef ARTHROPODS_WORLD_H_
#define ARTHROPODS_WORLD_H_

#include <nlohmann/json.hpp>

#include "World/World.h"
#include "Maths/MathFunctions.h"



class ArthropodsWorld : public World {
public:
    ArthropodsWorld(nlohmann::json * jsonTree, nlohmann::json &worldConfig, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath, int burnIn);
	virtual ~ArthropodsWorld();

    double calculateNewBiomass(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const;
    double calculateNewVoracity(const double &wetMass, const double &conversionToWetMass) const;
    double calculateTotalMetabolicDryMassLoss(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const Animal* const animal) const;
    const double getPdfThreshold() const;
    double getPredictedSpeed(const AnimalInterface* const animal) const;
};

#endif /* ARTHROPODS_WORLD_H_ */