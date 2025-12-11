/**
* @file DinosaursLandscape.h
* @brief Defines the DinosaursLandscape class for dinosaur-specific ecosystem simulations.
*
* This file declares the DinosaursLandscape class, a specialization of the
* Landscape base class. It models metabolic processes, voracity, movement,
* and food intake for dinosaur species, including both small and large-bodied
* dinosaurs with potentially unique thermal and ecological dynamics.
*/

#ifndef DINOSAURS_LANDSCAPE_H_
#define DINOSAURS_LANDSCAPE_H_

#include <nlohmann/json.hpp>


#include <boost/serialization/export.hpp>


#include <fstream>
#include <ostream>

#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/IBM/Maths/MathFunctions.h"


/**
* @class DinosaursLandscape
* @brief Specialized landscape model representing dinosaur ecosystems.
*
* This class extends the generic Landscape class with rules and calculations
* specific to dinosaurs. It includes metabolic computations, thermally dependent
* behaviours, voracity models, speed adjustments, and species-type identification.
*/
class DinosaursLandscape : public Landscape {
protected:
    /**
    * @brief Computes the amount of wet food available to a dinosaur
    * based on its wet mass.
    *
    * @param wetMass Wet mass of the individual.
    * @return PreciseDouble Quantity of available wet food.
    */
    PreciseDouble calculateWetFood(const PreciseDouble& wetMass) const;

public:
    /**
    * @brief Constructs a DinosaursLandscape object.
    */
    DinosaursLandscape();

    /**
    * @brief Virtual destructor.
    */
	virtual ~DinosaursLandscape();

    /**
    * @brief Deleted copy constructor. DinosaursLandscape is non-copyable.
    */
    DinosaursLandscape(const DinosaursLandscape&) = delete;
    
    /**
    * @brief Deleted copy assignment operator. DinosaursLandscape is non-copyable.
    */
    DinosaursLandscape& operator=(const DinosaursLandscape&) = delete;

    /**
    * @brief Calculates daily metabolic dry mass loss.
    *
    * This computation considers movement, thermal dependencies, metabolic
    * constants, search area, and dry–wet mass conversion.
    *
    * @param wetMass Wet mass of the dinosaur.
    * @param proportionOfTimeTheAnimalWasMoving Portion of the day spent moving.
    * @param actE_metValue Activity metabolic parameter.
    * @param actE_metThermallyDependent Whether activity metabolism depends on temperature.
    * @param met_rateValue Basal metabolic rate parameter.
    * @param met_rateThermallyDependent Whether basal metabolism depends on temperature.
    * @param search_areaValue Search area parameter.
    * @param tempFromLab Laboratory reference temperature.
    * @param terrainCellTemperature Actual landscape temperature.
    * @param conversionToWetMass Dry–wet mass conversion factor.
    * @return DryMass Daily dry mass loss.
    */
    DryMass calculateMetabolicDryMassLossPerDay(const WetMass &wetMass, const PreciseDouble&proportionOfTimeTheAnimalWasMoving, const PreciseDouble& actE_metValue, const bool actE_metThermallyDependent, const PreciseDouble& met_rateValue, const bool met_rateThermallyDependent, const PreciseDouble& search_areaValue, const Temperature& tempFromLab, const Temperature& terrainCellTemperature, const PreciseDouble& conversionToWetMass) const;
    
    /**
    * @brief Identifies this landscape as a dinosaur-specific environment.
    * @return true Always true for this class.
    */
    bool isDinosaurs() const override;
    
    /**
    * @brief Computes the maximum possible voracity (maximum consumable dry mass)
    * in a simulation timestep.
    *
    * @param wetMass Wet mass of the dinosaur.
    * @param conversionToWetMass Dry–wet mass conversion factor.
    * @param hasCapitalBreeding Whether the species exhibits capital breeding.
    * @param basalMetabolicDryMassLoss Basal metabolic dry mass loss.
    * @param YodzisA Constant A from Yodzis & Innes.
    * @param YodzisB Constant B from Yodzis & Innes.
    * @return DryMass Maximum dry mass consumable.
    */
    DryMass calculateMaxVoracityTimeStep(const WetMass& wetMass, const PreciseDouble& conversionToWetMass, const bool hasCapitalBreeding, const DryMass& basalMetabolicDryMassLoss, const PreciseDouble& YodzisA, const PreciseDouble& YodzisB) const;

    /**
    * @brief Computes the dinosaur's post-timestep movement speed.
    *
    * @param speedAfterAllometric Base allometric speed.
    * @param wetMass Wet mass of the individual.
    * @return PreciseDouble Adjusted speed.
    */
    PreciseDouble calculatePostTSpeed(const PreciseDouble& speedAfterAllometric, const WetMass& wetMass) const;

    /**
    * @brief Returns the type of this landscape.
    * @return Type Dinosaur landscape type.
    */
    Type getType() const;

    /**
    * @brief Returns the mass scaling factor used in dinosaur metabolic models.
    * @return PreciseDouble Scaling mass.
    */
    PreciseDouble getScaleMass() const;

    /**
    * @brief Serializes the DinosaursLandscape object.
    *
    * @tparam Archive Type of archive to use.
    * @param ar Archive instance.
    * @param version Serialization version.
    */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* DINOSAURS_LANDSCAPE_H_ */