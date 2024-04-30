#ifndef GROWTH_MODULE_H_
#define GROWTH_MODULE_H_

#include <nlohmann/json.hpp>
#include <vector>
#include <random>
#include <string>

#include "IBM/World/LivingBeings/Animals/GrowthModule/IndividualGrowthModel.h"
#include "Misc/EnumClass.h"
#include "IBM/World/LivingBeings/Instar.h"


class AnimalSpecies;


class GrowthModule {
public:
    GrowthModule(const AnimalSpecies* const animalSpecies);
    GrowthModule(const AnimalSpecies* const animalSpecies, const nlohmann::json& info);
    virtual ~GrowthModule();

    const double& getFemaleWetMass() const;
    const double& getEggDryMass() const;
    const IndividualGrowthModel::Type getGrowthModelType() const;
    bool hasIndeterminateGrowth() const;
    const Instar& getInstarFirstReproduction() const;
    const Instar& getInstarFirstMaturation() const;

private:
    const AnimalSpecies* const animalSpecies;

    double femaleWetMass;

    double eggDryMass;

    IndividualGrowthModel::Type growthModelType;

    bool indeterminateGrowth;
    Instar instarFirstReproduction;

    const AnimalSpecies* const getAnimalSpecies() const;

    void setFemaleWetMass(const double& newFemaleWetMass);
    void setEggMass(const nlohmann::json& info);
    void setGrowthModelType(const std::string& growthModelType);
    void setIndeterminateGrowth(const nlohmann::json& info);
};

#endif /* GROWTH_MODULE_H_ */
