/**
 * @file ResourceSpeciesGrowth.h
 * @brief Defines the ResourceSpeciesGrowth class, which models growth parameters for resource species.
 *
 * This class extends SpeciesGrowth by adding attributes specific to non-mobile
 * resource species, such as the biomass per cell and moisture requirements
 * for growth. It is used as a building block for ResourceSpecies.
 */

#ifndef RESOURCE_SPECIES_GROWTH_H_
#define RESOURCE_SPECIES_GROWTH_H_


#include <nlohmann/json.hpp>

#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/SpeciesGrowth.h"
#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"
#include "App/Model/IBM/Maths/Parabola.h"
#include "App/Model/IBM/Physics/Temperature.h"
#include "App/Model/IBM/Maths/MathFunctions.h"
#include "App/Model/IBM/Maths/Constants.h"


/**
 * @class ResourceSpeciesGrowth
 * @brief Growth model for resource-type species.
 *
 * ResourceSpeciesGrowth represents the growth properties of a resource species.
 * It extends the general SpeciesGrowth class with additional ecological
 * parameters relevant only to resources, such as:
 *
 * - fixed wet biomass per cell,
 * - minimum and maximum relative humidity (RH) thresholds that determine
 *   whether the resource can grow.
 *
 * The class may be constructed from a JSON configuration, enabling flexible
 * species definitions.
 */
class ResourceSpeciesGrowth : public SpeciesGrowth {
public:
    /**
     * @brief Default constructor.
     *
     * Initializes all growth parameters with default values.
     */
    ResourceSpeciesGrowth();

    /**
     * @brief Constructs the growth object using a JSON configuration block.
     *
     * The JSON object may specify:
     * - `"cell_mass"`: wet biomass per terrain cell,
     * - `"min_RH"`: minimum relative humidity for growth,
     * - `"max_RH"`: maximum relative humidity for growth,
     * - and any other fields inherited from SpeciesGrowth.
     *
     * @param config JSON configuration for the resource growth parameters.
     */
    ResourceSpeciesGrowth(const nlohmann::json& config);

    /**
     * @brief Virtual destructor.
     */
    virtual ~ResourceSpeciesGrowth();

    /**
     * @brief Returns the wet biomass contained in a single terrain cell.
     * @return Constant reference to the wet mass value.
     */
    const WetMass& getCellMass() const;

    /**
     * @brief Returns the minimum relative humidity threshold for growth.
     *
     * Growth is not possible if RH is below this value.
     *
     * @return Minimum RH (between 0 and 1, or percent depending on system definition).
     */
    PreciseDouble getMinRH() const;

    /**
     * @brief Returns the maximum relative humidity threshold for growth.
     *
     * Growth is limited or stops entirely when RH exceeds this value.
     *
     * @return Maximum RH.
     */
	PreciseDouble getMaxRH() const;

    /**
     * @brief Serializes the object for persistence.
     *
     * @tparam Archive Serialization archive type.
     * @param ar Archive instance.
     * @param version Serialization version.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    WetMass cellMass;     ///< Wet biomass available per terrain cell.
    PreciseDouble minRH;  ///< Minimum relative humidity required for growth.
    PreciseDouble maxRH;  ///< Maximum relative humidity allowed for growth.
};

#endif /* RESOURCE_SPECIES_GROWTH_H_ */
