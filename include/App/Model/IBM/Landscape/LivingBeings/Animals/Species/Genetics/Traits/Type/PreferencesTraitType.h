/**
 * @file PreferencesTraitType.h
 * @brief Defines the PreferencesTraitType enum, representing types of preference traits.
 */

#ifndef PREFERENCES_TRAIT_TYPE_H_
#define PREFERENCES_TRAIT_TYPE_H_

/**
 * @enum PreferencesTraitType
 * @brief Represents different types of preference-related traits for individuals.
 *
 * This enum is used to identify traits that modify preferences or choices of individuals
 * in the simulation.
 */
enum class PreferencesTraitType : unsigned char {
    /// Represents the influence of previous experience with edible items on individual preferences.
    experienceInfluenceWithEdibles
};

#endif // PREFERENCES_TRAIT_TYPE_H_
