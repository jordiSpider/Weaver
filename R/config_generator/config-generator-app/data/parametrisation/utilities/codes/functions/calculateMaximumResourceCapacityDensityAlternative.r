#' @title Calculate Maximum Resource Capacity Density (Alternative)
#' @description 
#' Computes the maximum density of resource capacity (biomass that can be supported)
#' for a population, based on an alternative approach that allows excluding
#' early instar stages (defined by \code{firstInstarToCalculate}).
#'
#' @details 
#' This function estimates the potential maximum biomass that a habitat can support
#' given the number of individuals in each instar stage and their growth model.  
#' It uses an allometric relationship to estimate body mass and ingestion rate per instar.  
#' The function differs from \code{calculateMaximumResourceCapacityDensityBiofilm()} 
#' by allowing the exclusion of the earliest instars from the calculation.
#'
#' @param individualsPerInstar Numeric vector. Number of individuals per instar stage.
#' @param firstInstarToCalculate Integer. Index of the first instar to include in the calculation.
#' @param landscapeArea Numeric. Total area of the landscape (in consistent spatial units).
#' @param ageVector Numeric vector. Ages corresponding to instar transitions.
#' @param coefficientForMassA Numeric. Coefficient A in the allometric equation for non-mature individuals.
#' @param coefficientForMassAforMature Numeric. Coefficient A for mature individuals.
#' @param scaleForMassB Numeric. Exponent B in the allometric equation for non-mature individuals.
#' @param scaleForMassBforMature Numeric. Exponent B for mature individuals.
#' @param conversionToWetMass Numeric. Conversion factor from dry mass to wet mass.
#' @param growthModel Model object. Growth model used to predict length as a function of age.
#'
#' @return 
#' A numeric value representing the maximum resource capacity density, i.e.,
#' the estimated total wet biomass ingested per day divided by the landscape area.
#'
#' @examples
#' \dontrun{
#' growth_model <- nls(L ~ Lmax * (1 - exp(-k * (age - t0))),
#'                     data = data.frame(age = c(0, 10, 20), L = c(1, 5, 8)),
#'                     start = list(Lmax = 10, k = 0.1, t0 = 0))
#' calculateMaximumResourceCapacityDensityAlternative(
#'     individualsPerInstar = c(100, 80, 50),
#'     firstInstarToCalculate = 2,
#'     landscapeArea = 1000,
#'     ageVector = c(5, 10, 20),
#'     coefficientForMassA = 0.02,
#'     coefficientForMassAforMature = 0.03,
#'     scaleForMassB = 2.8,
#'     scaleForMassBforMature = 3.0,
#'     conversionToWetMass = 5.5,
#'     growthModel = growth_model
#' )
#' }
#'
#' @seealso 
#' \code{\link{calculateMaximumResourceCapacityDensityBiofilm}} for a version that includes all instars.
#'
#' @export
calculateMaximumResourceCapacityDensityAlternative <- function(individualsPerInstar, firstInstarToCalculate, landscapeArea, ageVector, coefficientForMassA, coefficientForMassAforMature, scaleForMassB, scaleForMassBforMature, conversionToWetMass, growthModel) {
    
    # Predict length (L) for each requested age using the provided growth model.
    # Note: newdata includes 0.0 followed by ageVector — ensures a length prediction
    L = predict(growthModel, newdata = data.frame(age = c(0.0, ageVector)))

    # Compute wet mass for each predicted length.
    # Use mature coefficients for the last element of L (seq_along(L) == length(L)),
    # otherwise use the non-mature coefficients.
    # The conversionToWetMass factor transforms the estimated dry mass to wet mass.
    massesVector = ifelse(
        seq_along(L) == length(L),
        (coefficientForMassAforMature * L^scaleForMassBforMature) * conversionToWetMass,
        (coefficientForMassA * L^scaleForMassB) * conversionToWetMass
    )

    # Estimate maximum ingestion per instar using a mass-dependent allometric scaling
    maxIngestionPerInstar = 0.1*massesVector^0.75

    # Sum the ingested wet biomass per day across the instars included in the calculation.
    # The calculation selects instars from firstInstarToCalculate up to the last instar.
    # Multiplication is element-wise: number of individuals * ingestion per individual.
    totalIngestedWetBiomassInSimulationPerDay=sum(individualsPerInstar[firstInstarToCalculate:length(individualsPerInstar)]*maxIngestionPerInstar[firstInstarToCalculate:length(maxIngestionPerInstar)])

    # Normalize the total ingested biomass per day by the landscape area to obtain
    # the maximum resource capacity density (biomass per unit area per day).
    maximumResourceCapacityDensity = totalIngestedWetBiomassInSimulationPerDay/landscapeArea

    # Return the computed density
    return(maximumResourceCapacityDensity)
}
