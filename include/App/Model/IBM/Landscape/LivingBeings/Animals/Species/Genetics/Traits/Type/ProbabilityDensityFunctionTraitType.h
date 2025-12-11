/**
 * @file ProbabilityDensityFunctionTraitType.h
 * @brief Defines the ProbabilityDensityFunctionTraitType enum, representing parameters for a probability density function trait.
 */

#ifndef PROBABILITY_DENSITY_FUNCTION_TRAIT_TYPE_H_
#define PROBABILITY_DENSITY_FUNCTION_TRAIT_TYPE_H_

/**
 * @enum ProbabilityDensityFunctionTraitType
 * @brief Represents the type of parameters for a probability density function trait.
 *
 * This enum is used to specify which parameter of a PDF (Probability Density Function)
 * a trait corresponds to.
 */
enum class ProbabilityDensityFunctionTraitType : unsigned char {
    /// Represents the mean (mu) parameter of the PDF.
    muForPDF,

    /// Represents the standard deviation (sigma) parameter of the PDF.
    sigmaForPDF
};

#endif // PROBABILITY_DENSITY_FUNCTION_TRAIT_TYPE_H_
