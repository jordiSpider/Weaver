/**
 * @file BaseTraitType.h
 * @brief Defines the BaseTraitType enum, representing fundamental traits of individuals.
 */

#ifndef BASE_TRAIT_TYPE_H_
#define BASE_TRAIT_TYPE_H_

/**
 * @enum BaseTraitType
 * @brief Represents the basic physiological and life-history traits of an individual.
 *
 * These traits are fundamental characteristics that affect growth, reproduction,
 * metabolism, and other biological processes in the simulation.
 */
enum class BaseTraitType : unsigned char {
    /// Energy storage capacity of the individual.
    energy_tank,

    /// Growth rate of the individual.
    growth,

    /// Duration of egg development.
    eggDevTime,

    /// Factor modifying egg mass.
    factorEggMass,

    /// Assimilation efficiency.
    assim,

    /// Proportion of voracity (feeding rate).
    voracityProportion,

    /// Metabolic rate of the individual.
    met_rate,

    /// Resistance to shock or environmental stress.
    shock_resistance,

    /// Activity-based energy expenditure.
    actE_met,

    /// Memory depth or duration of retained information.
    memoryDepth,

    /// Coefficient relating mass to scope radius.
    coeffMassForScopeRadius,

    /// Scale factor relating mass to scope radius.
    scaleMassForScopeRadius,

    /// Coefficient relating mass to interaction radius.
    coeffMassForInteractionRadius,

    /// Scale factor relating mass to interaction radius.
    scaleMassForInteractionRadius,

    /// Coefficient relating mass to search radius.
    coeffMassForSearchRadius,

    /// Scale factor relating mass to search radius.
    scaleMassForSearchRadius,

    /// Coefficient relating mass to speed.
    coeffMassForSpeed,

    /// Scale factor relating mass to speed.
    scaleMassForSpeed,

    /// Development time (general).
    devTime,

    /// Length at maturation.
    lengthAtMaturation,

    /// Duration of the pupa period.
    pupaPeriodTime,

    /// Longevity since maturation.
    longevitySinceMaturation,

    /// Dummy trait (placeholder).
    dummy,

    /// Egg fertility.
    eggFertility
};

#endif // BASE_TRAIT_TYPE_H_
