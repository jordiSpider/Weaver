/**
 * @file MathFunctions.h
 * @brief Collection of mathematical and trait-based functions for ecological and physiological modeling.
 *
 * This class provides static methods to implement temperature-dependent trait calculations
 * using the Pawar 2018 model, temperature range estimation, size-temperature relationships (TSR),
 * linear interpolation, and Cholesky decomposition.
 */

#ifndef MATH_FUNCTIONS_H_
#define MATH_FUNCTIONS_H_

#include <vector>
#include <map>
#include <utility>
#include <cmath>
#include <functional>
#include <iostream>

#include "App/Model/IBM/Physics/Temperature.h"
#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"
#include "App/View/View.h"
#include "App/Model/IBM/Maths/Constants.h"


/**
 * @class MathFunctions
 * @brief Provides static mathematical utilities for traits, temperature responses, and ecological models.
 *
 * This class is not meant to be instantiated. All functions are static and include:
 * - Trait scaling and processing for Pawar 2018 thermal performance curves.
 * - Bisection-based temperature range estimation for traits.
 * - Size-temperature rule (TSR) calculations.
 * - Linear interpolation.
 * - Cholesky decomposition for variance-covariance matrices.
 */
class MathFunctions
{
protected:
    /**
     * @brief Preprocesses a trait value for Pawar 2018 model calculations.
     * @param traitAtTopt Trait value at optimal temperature.
     * @param inverse Whether to invert the trait.
     * @param strictlyPositive If true, ensures output is strictly positive.
     * @return Processed trait value ready for Pawar 2018 calculation.
     */
    static PreciseDouble preprocessTraitForPawar(const PreciseDouble& traitAtTopt, const bool inverse, const bool strictlyPositive);
    
    /**
     * @brief Postprocesses a trait after Pawar 2018 calculation.
     * @param traitAtTemp Trait value at current temperature.
     * @param traitAtTopt Trait value at optimal temperature.
     * @param inverse Whether the trait was inverted.
     * @param strictlyPositive Whether output should be strictly positive.
     * @return Final processed trait value.
     */
    static PreciseDouble postprocessTraitForPawar(const PreciseDouble& traitAtTemp, const PreciseDouble& traitAtTopt, const bool inverse, const bool strictlyPositive);
    
    /**
     * @brief Pawar 2018 model function for temperature-dependent trait.
     * @param temperature Current temperature.
     * @param traitAtTopt Trait value at optimal temperature.
     * @param Topt Optimal temperature.
     * @param ED Deactivation energy.
     * @param E Activation energy.
     * @param Tref Reference temperature.
     * @return Trait value at the given temperature.
     */
    static PreciseDouble Pawar_2018(const Temperature& temperature, const PreciseDouble& traitAtTopt, const Temperature& Topt, const PreciseDouble& ED, const PreciseDouble& E, const Temperature& Tref);
    
    /**
     * @brief Finds the temperature at which a trait reaches a specific value using bisection.
     * @param searchRange Pair of minimum and maximum temperatures to search.
     * @param traitAtTopt Trait value at optimal temperature.
     * @param Topt Optimal temperature.
     * @param ED Deactivation energy.
     * @param E Activation energy.
     * @param Tref Reference temperature.
     * @param nextToZeroTrait Trait value near zero to find bounds.
     * @param tol Tolerance for convergence.
     * @param max_iter Maximum number of iterations.
     * @return Temperature at which trait reaches near-zero value.
     */
    static Temperature bisection(const std::pair<Temperature, Temperature>& searchRange, const PreciseDouble& traitAtTopt, const Temperature& Topt, const PreciseDouble& ED, const PreciseDouble& E, const Temperature& Tref, const PreciseDouble& nextToZeroTrait, PreciseDouble tol = 1e-6, unsigned int max_iter = 1000);
    
    /**
     * @brief Determines the temperature range where a trait is expressed based on Pawar 2018.
     * @param traitAtTopt Trait value at optimal temperature.
     * @param Topt Optimal temperature.
     * @param ED Deactivation energy.
     * @param E Activation energy.
     * @param Tref Reference temperature.
     * @param nextToZeroTrait Trait value near zero for thresholding.
     * @return Pair of minimum and maximum temperatures for trait expression.
     */
    static std::pair<Temperature,Temperature> TemperatureRangePawar_2018(const PreciseDouble& traitAtTopt, const Temperature& Topt, const PreciseDouble& ED, const PreciseDouble& E, const Temperature& Tref, const PreciseDouble& nextToZeroTrait);
    
    /**
     * @brief Calculates size-temperature relationship (TSR) for a given temperature.
     * @param temperature Current temperature.
     * @param tempSizeRuleVector Map of temperature to DryMass values.
     * @param coefficientForMassAforMature Coefficient A for mass scaling.
     * @param scaleForMassBforMature Exponent B for mass scaling.
     * @return Predicted Length for given temperature.
     */
    static Length TSR(const Temperature& temperature, const std::map<Temperature, DryMass>& tempSizeRuleVector, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature);

public:
    /**
     * @brief Calculates a trait value at a temperature using Pawar 2018 model.
     * @param temperature Current temperature.
     * @param traitAtTopt Trait value at optimal temperature.
     * @param Topt Optimal temperature.
     * @param ED Deactivation energy.
     * @param E Activation energy.
     * @param Tref Reference temperature.
     * @param inverse Whether to invert the trait.
     * @param strictlyPositive Ensure result is strictly positive.
     * @param propOftraitAtTopt Small proportion for preprocessing (default 0.01).
     * @return Trait value at the given temperature.
     */
    static PreciseDouble use_Pawar_2018(const Temperature& temperature, const PreciseDouble& traitAtTopt, const Temperature& Topt, const PreciseDouble& ED, const PreciseDouble& E, const Temperature& Tref, const bool inverse, const bool strictlyPositive, const PreciseDouble& propOftraitAtTopt = 1e-02);
    
    /**
     * @brief Calculates the temperature range for a trait based on Pawar 2018.
     * @param traitAtTopt Trait value at optimal temperature.
     * @param Topt Optimal temperature.
     * @param ED Deactivation energy.
     * @param E Activation energy.
     * @param Tref Reference temperature.
     * @param inverse Whether trait was inverted.
     * @param strictlyPositive Ensure positive values.
     * @param propOftraitAtTopt Small proportion for preprocessing (default 0.01).
     * @return Pair of minimum and maximum temperatures where trait is expressed.
     */
    static std::pair<Temperature,Temperature> calculateTemperatureRangePawar(const PreciseDouble& traitAtTopt, const Temperature& Topt, const PreciseDouble& ED, const PreciseDouble& E, const Temperature& Tref, const bool inverse, const bool strictlyPositive, const PreciseDouble& propOftraitAtTopt = 1e-02);
    
    /**
     * @brief Calculates size-temperature relationship (TSR) at a given temperature.
     * @param temperature Current temperature.
     * @param tempSizeRuleVector Map of temperature to DryMass values.
     * @param coefficientForMassAforMature Coefficient A for mass scaling.
     * @param scaleForMassBforMature Exponent B for mass scaling.
     * @param traitAtTempRef Reference length at reference temperature.
     * @param Tref Reference temperature.
     * @return Predicted Length for given temperature.
     */
    static Length use_TSR(const Temperature& temperature, const std::map<Temperature, DryMass>& tempSizeRuleVector, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Length& traitAtTempRef, const Temperature& Tref);

    /**
     * @brief Performs linear interpolation between two points.
     * @param x Independent variable value.
     * @param min_x Minimum x-value.
     * @param max_x Maximum x-value.
     * @param min_y Corresponding y-value at min_x.
     * @param max_y Corresponding y-value at max_x.
     * @return Interpolated y-value at x.
     */
    static PreciseDouble linearInterpolate(const PreciseDouble& x, const PreciseDouble& min_x, const PreciseDouble& max_x, const PreciseDouble& min_y, const PreciseDouble& max_y);

    /**
     * @brief Computes the Cholesky decomposition of a covariance matrix represented by sigma values.
     * @param sigmaValues Vector of sigma values for decomposition.
     * @return Lower-triangular matrix as a vector of vectors.
     */
    static std::vector<std::vector<PreciseDouble>> cholesky(const std::vector<PreciseDouble>& sigmaValues);
};



#endif /* MATH_FUNCTIONS_H_ */
