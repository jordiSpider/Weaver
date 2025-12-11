/**
 * @file LifeStage.h
 * @brief Defines the LifeStage enumeration used to represent the life stage or status of an individual organism.
 */

#ifndef LIFE_STAGE_H_
#define LIFE_STAGE_H_

/**
 * @enum LifeStage
 * @brief Represents the different life stages or conditions of an organism.
 */
enum class LifeStage : unsigned char
{
	UNBORN,        /**< The individual has not yet been born. */
    ACTIVE,        /**< The individual is alive and active. */
    STARVED,       /**< The individual is alive but experiencing starvation. */
    PREDATED,      /**< The individual has been predated upon. */
    REPRODUCING,   /**< The individual is currently reproducing. */
    PUPA,          /**< The individual is in the pupal stage (metamorphosis). */
    DIAPAUSE,      /**< The individual is in a state of dormancy (diapause). */
    BACKGROUND,    /**< The individual is alive but not currently active in key processes (e.g., reproduction). */
    SENESCED,      /**< The individual has reached senescence (old age, declining function). */
    SHOCKED        /**< The individual is temporarily impaired due to a shock or stress event. */
};

#endif /* LIFE_STAGE_H_ */
