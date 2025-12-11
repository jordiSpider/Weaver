/**
 * @file HuntingMode.h
 * @brief Defines the HuntingMode enumeration representing the foraging or hunting strategy of an animal.
 */

#ifndef HUNTING_MODE_H_
#define HUNTING_MODE_H_

/**
 * @enum HuntingMode
 * @brief Represents the hunting or feeding strategy of an animal.
 */
enum class HuntingMode : unsigned char
{
	does_not_hunt, /**< The animal does not hunt; it may rely on other food sources. */
    sit_and_wait,  /**< The animal employs an ambush strategy, waiting for prey to come near. */
    grazer         /**< The animal actively grazes or feeds on resources available in its environment. */
};

#endif /* HUNTING_MODE_H_ */
