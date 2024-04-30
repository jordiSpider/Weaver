#ifndef DINOSAURS_WORLD_H_
#define DINOSAURS_WORLD_H_

#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>
#include <boost/filesystem.hpp>

#include "IBM/World/World.h"
#include "IBM/Maths/MathFunctions.h"



class DinosaursWorld : public World {
private:
    friend class boost::serialization::access;

public:
    DinosaursWorld(nlohmann::json * jsonTree, nlohmann::json &worldConfig, boost::filesystem::path outputFolder, boost::filesystem::path configPath, int burnIn);
	virtual ~DinosaursWorld();

    double calculateNewBiomass(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const;
    double calculateNewVoracity(const double &wetMass, const double &conversionToWetMass) const;
    double calculateTotalMetabolicDryMassLoss(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const Animal* const animal) const;
    const bool isDinosaurs() const override;
    const double getPdfThreshold() const;
    double getPredictedSpeed(const AnimalInterface* const animal) const;

    /**
     * @brief Serialize the DinosaursWorld object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* DINOSAURS_WORLD_H_ */