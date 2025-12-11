#' Calculate the Number of Individuals per Instar for *Isoperla* Species
#'
#' This function estimates the distribution of individuals across instars
#' (developmental stages) for *Isoperla* species, based on the allometric
#' relationship between body mass and population density. It assumes that
#' population density decreases as body mass increases, following a power-law
#' relationship.
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
#'
#' @details
#' The function assumes that population density follows a power-law relationship
#' with body mass, such that:
#' \deqn{D = a \cdot M^{bDensity}}
#' where:
#' - \eqn{D} is the population density,
#' - \eqn{M} is the body mass,
#' - \eqn{bDensity} is a typically negative exponent (commonly -0.75).
#'
#' The result is normalized so that the total number of individuals across all instars
#' equals `totalNumberOfIndividuals`.
#'
#' This version is tailored for *Isoperla* species, but the general formulation applies to
#' any taxon exhibiting similar scaling between density and body size.
#'
#' @examples
#' \dontrun{
#' # Example of use (assuming a valid growthModel exists):
#' individuals <- calculateIndividualsPerInstarIsoperla(
#'     totalNumberOfIndividuals = 1000,
#'     field_density = 200,
#'     ageVector = seq(1, 10, by = 1),
#'     coefficientForMassA = 0.005,
#'     coefficientForMassAforMature = 0.01,
#'     scaleForMassB = 3,
#'     scaleForMassBforMature = 3.2,
#'     conversionToWetMass = 10,
#'     growthModel = my_growth_model
#' )
#' }
#'
#' @export
calculateIndividualsPerInstarIsoperla <- function(totalNumberOfIndividuals, field_density, ageVector, coefficientForMassA, coefficientForMassAforMature, scaleForMassB, scaleForMassBforMature, conversionToWetMass, growthModel, bDensity=-0.75) {
    
    # Predict individual lengths (L) using the provided growth model
    # for age = 0 and the values in the given ageVector.
    L = predict(growthModel, newdata = data.frame(age = c(0.0, ageVector)))

    # Compute body mass for each instar based on predicted lengths.
    # Mature individuals (last instar) use distinct coefficients and scaling exponents.
    # Conversion from dry to wet mass is applied via conversionToWetMass.
    massesVector = ifelse(
        seq_along(L) == length(L),
        (coefficientForMassAforMature * L^scaleForMassBforMature) * conversionToWetMass,
        (coefficientForMassA * L^scaleForMassB) * conversionToWetMass
    )

    # Compute mean body mass assuming one individual per instar at this stage.
    meanBodyMass <- mean(massesVector)

    # Derive the proportionality constant 'a' in the allometric density relationship.
    # This ensures the mean density corresponds to the observed field density.
    aDensity <- field_density/(meanBodyMass^bDensity)

    # Compute density for each instar using the allometric relationship.
    # As body mass increases, density decreases (for negative bDensity).
    densitiesVector <- aDensity*massesVector^bDensity
    
    # Allocate individuals per instar proportionally to computed densities.
    # The resulting vector is normalized so that the total sum equals totalNumberOfIndividuals.
    individualsPerInstar <- round(totalNumberOfIndividuals*densitiesVector/sum(densitiesVector))

    # Return the estimated individuals per instar.
    return(individualsPerInstar)
}
