/**
 * @file SpeciesType.h
 * @brief Defines the type of species in the simulation.
 */

#ifndef SPECIES_TYPE_H_
#define SPECIES_TYPE_H_

/**
 * @enum SpeciesType
 * @brief Enumerates the different types of species in the ecosystem.
 *
 * This enum is used to distinguish between resource species (e.g., plants, prey)
 * and animal species (e.g., predators or herbivores) in the simulation.
 */
enum class SpeciesType : unsigned int
{
	/// Represents a resource species such as plants or other consumable entities
    Resource,

    /// Represents an animal species such as predators or herbivores
    Animal
};

#endif /* SPECIES_TYPE_H_ */
