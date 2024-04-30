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

protected:
    const double calculateWetFood(const double& wetMass) const;

public:
    DinosaursWorld(nlohmann::json * jsonTree, nlohmann::json &worldConfig, boost::filesystem::path outputFolder, boost::filesystem::path configPath, int burnIn);
	virtual ~DinosaursWorld();

    double calculateNewBiomassPerDay(const double &biomass, const double &rateOfIncrease, const ResourceSpecies* const species) const;
    double calculateTotalMetabolicDryMassLossPerDay(const double &wetMass, const double &proportionOfTimeTheAnimalWasMoving, const AnimalNonStatistical* const animal) const;
    const bool isDinosaurs() const override;
    const double getPdfThreshold() const;
    
    double calculateMaxMassPredicted(const double& dryMass, const double& wetMass, const double& conversionToWetMass, const unsigned int numberOfTimeSteps, const bool hasCapitalBreeding, const double& minTotalMetabolicDryMassLoss, const double& newAAdult, const double& newB) const;

    double calculatePostTSpeed(const double& speedValue, const double& wetMass, const double& scaleForSpeed) const;

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