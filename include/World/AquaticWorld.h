#ifndef AQUATIC_WORLD_H_
#define AQUATIC_WORLD_H_

#include <nlohmann/json.hpp>

#include "World/World.h"
#include "Maths/MathFunctions.h"



class AquaticWorld : public World {
public:
    AquaticWorld(nlohmann::json * jsonTree, nlohmann::json &worldConfig, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath);
	virtual ~AquaticWorld();

    double calculateNewBiomass(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const;
    double calculateNewVoracity(const double &wetMass, const double &conversionToWetMass) const;
    double calculateTotalMetabolicDryMassLoss(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const Animal* const animal) const;
    const double getPdfThreshold() const;
    double getPredictedSpeed(const AnimalInterface* const animal) const;
};

#endif /* AQUATIC_WORLD_H_ */