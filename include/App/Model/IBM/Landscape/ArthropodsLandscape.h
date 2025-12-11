/**
* @file ArthropodsLandscape.h
* @brief Defines the ArthropodsLandscape class for arthropod-specific ecosystem simulations.
*
* This file declares the ArthropodsLandscape class, a specialization of the
* Landscape base class, implementing metabolic, behavioural, and ecological
* processes specific to arthropod species. It provides functionality for
* calculating voracity, metabolic losses, wet food availability, and movement
* adjustments for arthropods.
*/

#ifndef ARTHROPODS_LANDSCAPE_H_
#define ARTHROPODS_LANDSCAPE_H_

#include <nlohmann/json.hpp>


#include <boost/serialization/export.hpp>


#include <fstream>
#include <ostream>

#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/IBM/Maths/MathFunctions.h"


/**
* @class ArthropodsLandscape
* @brief Specialized landscape model for arthropod ecosystems.
*
* This class extends the generic Landscape class with arthropod-specific
* ecological rules. It handles metabolic processes, voracity constraints,
* speed adjustments, and temperature-dependent behaviours relevant to
* arthropods.
*/
class ArthropodsLandscape : public Landscape {
protected:
    /**
    * @brief Computes the amount of wet food available to an arthropod
    * based on its wet mass.
    *
    * @param wetMass Wet mass of the individual.
    * @return PreciseDouble Amount of available wet food.
    */
    PreciseDouble calculateWetFood(const PreciseDouble& wetMass) const;

public:
    /**
    * @brief Constructs an ArthropodsLandscape object.
    */
    ArthropodsLandscape();

    /**
    * @brief Destructor.
    */
    virtual ~ArthropodsLandscape();

    /**
    * @brief Deleted copy constructor; ArthropodsLandscape is non-copyable.
    */
    ArthropodsLandscape(const ArthropodsLandscape&) = delete;
    
    /**
    * @brief Deleted copy assignment operator; ArthropodsLandscape is non-copyable.
    */
    ArthropodsLandscape& operator=(const ArthropodsLandscape&) = delete;

    /**
    * @brief Calculates the dry mass loss due to metabolic processes during a day.
    *
    * This value depends on activity time, metabolic parameters, and the thermal
    * environment.
    *
    * @param wetMass Wet mass of the individual.
    * @param proportionOfTimeTheAnimalWasMoving Fraction of the day spent moving.
    * @param actE_metValue Activity metabolic parameter.
    * @param actE_metThermallyDependent True if activity metabolism depends on temperature.
    * @param met_rateValue Basal metabolic rate parameter.
    * @param met_rateThermallyDependent True if basal metabolism depends on temperature.
    * @param search_areaValue Search area parameter.
    * @param tempFromLab Laboratory temperature data.
    * @param terrainCellTemperature Actual temperature of the landscape cell.
    * @param conversionToWetMass Conversion ratio between dry and wet mass.
    * @return DryMass Dry mass lost per day.
    */
    DryMass calculateMetabolicDryMassLossPerDay(const WetMass &wetMass, const PreciseDouble&proportionOfTimeTheAnimalWasMoving, const PreciseDouble& actE_metValue, const bool actE_metThermallyDependent, const PreciseDouble& met_rateValue, const bool met_rateThermallyDependent, const PreciseDouble& search_areaValue, const Temperature& tempFromLab, const Temperature& terrainCellTemperature, const PreciseDouble& conversionToWetMass) const;

    /**
    * @brief Computes the maximum voracity (maximum dry mass consumable) in a timestep.
    *
    * @param wetMass Wet mass of the individual.
    * @param conversionToWetMass Dry-to-wet mass conversion.
    * @param hasCapitalBreeding Whether the species uses capital breeding.
    * @param basalMetabolicDryMassLoss Basal metabolic dry mass loss.
    * @param YodzisA Constant A from Yodzis & Innes.
    * @param YodzisB Constant B from Yodzis & Innes.
    * @return DryMass Maximum consumable dry mass.
    */
    DryMass calculateMaxVoracityTimeStep(const WetMass& wetMass, const PreciseDouble& conversionToWetMass, const bool hasCapitalBreeding, const DryMass& basalMetabolicDryMassLoss, const PreciseDouble& YodzisA, const PreciseDouble& YodzisB) const;

    /**
    * @brief Computes the animal's post-timestep speed.
    *
    * @param speedAfterAllometric Base speed after allometric scaling.
    * @param wetMass Wet mass of the individual.
    * @return PreciseDouble Adjusted speed.
    */
    PreciseDouble calculatePostTSpeed(const PreciseDouble& speedAfterAllometric, const WetMass& wetMass) const;

    /**
    * @brief Returns the type of this landscape.
    * @return Type Enumeration value identifying the landscape type.
    */
    Type getType() const;

    /**
    * @brief Returns the mass scaling factor used for ecological calculations.
    * @return PreciseDouble Scaling mass for arthropods.
    */
    PreciseDouble getScaleMass() const;

    /**
    * @brief Serializes the ArthropodsLandscape object.
    *
    * @tparam Archive Archive type (e.g., boost::archive::binary_oarchive).
    * @param ar Archive instance.
    * @param version Serialization version.
    */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* ARTHROPODS_LANDSCAPE_H_ */