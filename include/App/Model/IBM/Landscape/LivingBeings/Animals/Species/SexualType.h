/**
 * @file SexualType.h
 * @brief Defines the SexualType enumeration used to classify the reproductive system of a species.
 */

#ifndef SEXUAL_TYPE_H_
#define SEXUAL_TYPE_H_

/**
 * @enum SexualType
 * @brief Represents the type of sexual reproduction of an organism.
 */
enum class SexualType : unsigned char
{
	diploid,       /**< Organism reproduces sexually with two sets of chromosomes (diploid). */
    haplodiploid,  /**< Organism reproduces with a haplodiploid system (e.g., males haploid, females diploid). */
    asexual        /**< Organism reproduces without sexual recombination (asexual reproduction). */
};

#endif /* SEXUAL_TYPE_H_ */
