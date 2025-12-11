/**
* @file AquaticLandscape.h
* @brief Defines the AquaticLandscape class for aquatic ecosystem simulations.
*
* This file contains the declaration of the AquaticLandscape class, a specialized
* implementation of Landscape for modelling ecological processes in aquatic
* environments. It handles food availability, metabolic calculations, movement
* impacts, and temperature-dependent behaviours for aquatic species.
*/

#ifndef AQUATIC_LANDSCAPE_H_
#define AQUATIC_LANDSCAPE_H_

#include <nlohmann/json.hpp>


#include <boost/serialization/export.hpp>


#include <fstream>
#include <ostream>

#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/IBM/Maths/MathFunctions.h"



/**
* @class AquaticLandscape
* @brief Specialized landscape representing aquatic ecosystems.
*
* This class extends the base Landscape class and provides all calculations
* required for animals living in aquatic environments, including food intake,
* metabolic losses, speed adjustments, and temperature‑dependent processes.
*/
class AquaticLandscape : public Landscape {
protected:
    /**
    * @brief Calculates the amount of wet food available based on wet mass.
    *
    * @param wetMass Wet mass of the animal.
    * @return Amount of wet food available.
    */
    PreciseDouble calculateWetFood(const PreciseDouble& wetMass) const;

public:
    /**
    * @brief Constructs an AquaticLandscape object.
    */
    AquaticLandscape();

    /**
    * @brief Destructor.
    */
    virtual ~AquaticLandscape();

    /**
    * @brief Deleted copy constructor (AquaticLandscape is non-copyable).
    */
    AquaticLandscape(const AquaticLandscape&) = delete;

    /**
    * @brief Deleted copy assignment operator (AquaticLandscape is non-copyable).
    */
    AquaticLandscape& operator=(const AquaticLandscape&) = delete;

    /**
    * @brief Calculates daily dry mass metabolic loss.
    *
    * This calculation depends on movement proportion, temperature, thermal
    * dependencies, metabolic parameters, and conversion to wet mass.
    *
    * @param wetMass Wet mass of the animal.
    * @param proportionOfTimeTheAnimalWasMoving Fraction of the day the animal moved.
    * @param actE_metValue Activity metabolic parameter.
    * @param actE_metThermallyDependent Whether activity metabolism is temperature-dependent.
    * @param met_rateValue Basal metabolic rate parameter.
    * @param met_rateThermallyDependent Whether basal metabolism is temperature-dependent.
    * @param search_areaValue Search area parameter.
    * @param tempFromLab Laboratory temperature.
    * @param terrainCellTemperature Actual terrain cell temperature.
    * @param conversionToWetMass Conversion ratio from dry to wet mass.
    * @return DryMass Loss of dry mass per day.
    */
    DryMass calculateMetabolicDryMassLossPerDay(const WetMass &wetMass, const PreciseDouble&proportionOfTimeTheAnimalWasMoving, const PreciseDouble& actE_metValue, const bool actE_metThermallyDependent, const PreciseDouble& met_rateValue, const bool met_rateThermallyDependent, const PreciseDouble& search_areaValue, const Temperature& tempFromLab, const Temperature& terrainCellTemperature, const PreciseDouble& conversionToWetMass) const;

    /**
    * @brief Computes the maximum voracity (maximum dry mass the animal can
    * consume) in one timestep.
    *
    * @param wetMass Wet mass of the animal.
    * @param conversionToWetMass Conversion ratio from dry to wet mass.
    * @param hasCapitalBreeding Whether the species uses capital breeding.
    * @param basalMetabolicDryMassLoss Dry mass lost by basal metabolic processes.
    * @param YodzisA Yodzis & Innes constant A.
    * @param YodzisB Yodzis & Innes constant B.
    * @return DryMass Maximum voracity for this timestep.
    */
    DryMass calculateMaxVoracityTimeStep(const WetMass& wetMass, const PreciseDouble& conversionToWetMass, const bool hasCapitalBreeding, const DryMass& basalMetabolicDryMassLoss, const PreciseDouble& YodzisA, const PreciseDouble& YodzisB) const;

    /**
    * @brief Calculates the animal's post‑timestep speed.
    *
    * @param speedAfterAllometric Speed after allometric scaling.
    * @param wetMass Wet mass of the individual.
    * @return Adjusted speed after timestep.
    */
    PreciseDouble calculatePostTSpeed(const PreciseDouble& speedAfterAllometric, const WetMass& wetMass) const;

    /**
    * @brief Returns the type of this landscape.
    * @return Type Landscape type.
    */
    Type getType() const;

    /**
    * @brief Returns the scale mass used for various ecological calculations.
    * @return PreciseDouble Scaling mass value.
    */
    PreciseDouble getScaleMass() const;

    /**
    * @brief Serializes the object for persistence.
    *
    * @tparam Archive Serialization archive type.
    * @param ar Archive instance.
    * @param version Serialization version.
    */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* AQUATIC_LANDSCAPE_H_ */