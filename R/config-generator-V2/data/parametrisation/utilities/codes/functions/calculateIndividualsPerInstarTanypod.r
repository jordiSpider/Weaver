#' Calculate the Number of Individuals per Instar for Tanypod Chironomids
#'
#' This function estimates the distribution of individuals across instars
#' (developmental stages) based on an allometric relationship between individual
#' body mass and density. It assumes a negative power-law relationship
#' between density and body mass.
#'
#' @param totalNumberOfIndividuals Numeric. Total number of individuals in the population.
#' @param field_density Numeric. Field-observed density (e.g., individuals per square meter).
#' @param ageVector Numeric vector. Ages corresponding to the instar transitions.
#' @param coefficientForMassA Numeric. Coefficient \eqn{A} for non-mature individuals in the allometric mass equation.
#' @param coefficientForMassAforMature Numeric. Coefficient \eqn{A} for mature individuals.
#' @param scaleForMassB Numeric. Exponent \eqn{B} for non-mature individuals in the allometric mass equation.
#' @param scaleForMassBforMature Numeric. Exponent \eqn{B} for mature individuals.
#' @param conversionToWetMass Numeric. Conversion factor from dry mass to wet mass.
#' @param growthModel Model object. Growth model used to predict individual length as a function of age.
#' @param bDensity Numeric (default = -0.75). Exponent defining the inverse relationship
#' between body mass and population density.
#'
#' @return Numeric vector with the estimated number of individuals per instar.
#' @details
#' The function assumes that population density follows a power-law scaling with body mass:
#' \deqn{D = a \cdot M^{bDensity}}
#' where:
#' - \eqn{D} is the density,
#' - \eqn{M} is the body mass,
#' - \eqn{bDensity} is typically negative (e.g., -0.75), meaning that larger individuals
#' occur at lower densities.
#'
#' The resulting number of individuals per instar is normalized so that their total sum equals
#' the provided `totalNumberOfIndividuals`.
#'
#' @export
calculateIndividualsPerInstarTanypod <- function(totalNumberOfIndividuals, field_density, ageVector, coefficientForMassA, coefficientForMassAforMature, scaleForMassB, scaleForMassBforMature, conversionToWetMass, growthModel, bDensity=-0.75) {
    
    # Predict individual lengths (L) using the growth model for age 0 and the given ageVector.
    L = predict(growthModel, newdata = data.frame(age = c(0.0, ageVector)))

    # Compute body mass for each instar based on predicted lengths.
    # Mature individuals (last instar) use separate coefficients and exponents.
    # Convert from dry to wet mass using conversionToWetMass.
    massesVector = ifelse(
        seq_along(L) == length(L),
        (coefficientForMassAforMature * L^scaleForMassBforMature) * conversionToWetMass,
        (coefficientForMassA * L^scaleForMassB) * conversionToWetMass
    )

    # Compute mean body mass across instars (assuming n=1 for each instar at this step).
    meanBodyMass <- mean(massesVector)

    # Calculate the proportionality constant 'a' for the density-mass relationship.
    # This ensures that the mean density corresponds to the observed field density.
    aDensity <- field_density/(meanBodyMass^bDensity)

    # Compute the expected density for each instar based on its body mass.
    # Larger individuals will have smaller densities when bDensity < 0.
    densitiesVector <- aDensity*massesVector^bDensity
    
    # Allocate individuals per instar proportionally to density values.
    # The total number of individuals is conserved (sum equals totalNumberOfIndividuals).
    individualsPerInstar <- round(totalNumberOfIndividuals*densitiesVector/sum(densitiesVector))

    # Return the final vector of individuals per instar.
    return(individualsPerInstar)
}
