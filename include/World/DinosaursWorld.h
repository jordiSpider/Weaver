#ifndef DINOSAURS_WORLD_H_
#define DINOSAURS_WORLD_H_

#include <nlohmann/json.hpp>

#include "World/World.h"
#include "Maths/MathFunctions.h"



class DinosaursWorld : public World {
public:
    DinosaursWorld(nlohmann::json * jsonTree, nlohmann::json &worldConfig, fs::path outputFolder, fs::path WeaverFolder, fs::path configPath);
	virtual ~DinosaursWorld();

    double calculateNewBiomass(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const;
    double calculateNewVoracity(const double &wetMass, const double &conversionToWetMass) const;
    double calculateTotalMetabolicDryMassLoss(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const Animal* const animal) const;
    const bool isDinosaurs() const override;
    const double getPdfThreshold() const;
    double getPredictedSpeed(const AnimalInterface* const animal) const;
};

#endif /* DINOSAURS_WORLD_H_ */