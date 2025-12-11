/**
 * @file DecisionsTraitType.h
 * @brief Defines enums representing types of decision-related trait weights.
 */

#ifndef DECISIONS_TRAIT_TYPE_H_
#define DECISIONS_TRAIT_TYPE_H_

/**
 * @enum EscapeProbabilityWeightType
 * @brief Represents the weight types used to calculate the escape probability of an individual.
 *
 * These weights determine how different factors influence the probability
 * of an individual successfully escaping from a predator.
 */
enum class EscapeProbabilityWeightType : unsigned char {
    /// Weight associated with the individual's velocity.
    Pvelocity,

    /// Weight associated with the distance from an attacking predator.
    PattackDistance
};

/**
 * @enum PredationProbabilityWeightType
 * @brief Represents the weight types used to calculate the probability of predation.
 *
 * These weights determine how different factors influence the likelihood
 * of a predator successfully capturing prey.
 */
enum class PredationProbabilityWeightType : unsigned char {
    /// Weight associated with predator's reach or attack range.
    Preach,

    /// Weight associated with the prey's probability density function.
    Ppdf,

    /// Weight associated with the prey's voracity.
    PvorPred
};

/**
 * @enum EdibilityValueWeightType
 * @brief Represents the weight types used to calculate edibility of a prey item.
 *
 * These weights influence how attractive or edible a prey item is for a predator.
 */
enum class EdibilityValueWeightType : unsigned char {
    /// Weight associated with the intrinsic edibility probability.
    Pp
};

#endif // DECISIONS_TRAIT_TYPE_H_
