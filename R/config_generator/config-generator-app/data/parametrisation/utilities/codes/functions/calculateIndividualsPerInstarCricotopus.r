#' Calculate the number of individuals per instar for *Cricotopus*
#'
#' This function estimates the distribution of individuals across instars (developmental stages)
#' for the genus *Cricotopus* based on their growth model, allometric relationships, and field density.
#' It scales the number of individuals according to body mass using an allometric density relationship.
#'
#' @param totalNumberOfIndividuals Numeric. Total number of individuals to be distributed among instars.
#' @param field_density Numeric. Observed or estimated density of individuals in the field (individuals per unit area or volume).
#' @param ageVector Numeric vector. Ages corresponding to each instar (excluding 0.0).
#' @param coefficientForMassA Numeric. Coefficient for body mass scaling of non-mature individuals.
#' @param coefficientForMassAforMature Numeric. Coefficient for body mass scaling of mature individuals.
#' @param scaleForMassB Numeric. Exponent for body mass scaling of non-mature individuals.
#' @param scaleForMassBforMature Numeric. Exponent for body mass scaling of mature individuals.
#' @param conversionToWetMass Numeric. Conversion factor from dry mass to wet mass.
#' @param growthModel Model object. A predictive growth model (e.g., from `lm()` or `nls()`)
#'   relating body length to age.
#' @param bDensity Numeric, optional. Exponent for the allometric relationship between body mass
#'   and population density. Defaults to `-0.75`.
#'
#' @return A numeric vector with the estimated number of individuals per instar.
#'
#' @details
#' The function assumes an allometric relationship between population density (`D`) and body mass (`M`)
#' following the equation:
#' \deqn{D = a \cdot M^{b}}
#' where `a` is a constant derived from the field density and `b` is `bDensity`.
#'
#' The total number of individuals is distributed among instars proportionally to the predicted
#' density of each instar derived from body mass scaling.
#'
#' @examples
#' # Example usage:
#' growthModel <- lm(length ~ age, data = data.frame(age = 1:5, length = c(2, 4, 6, 8, 10)))
#' calculateIndividualsPerInstarCricotopus(
#'   totalNumberOfIndividuals = 500,
#'   field_density = 25,
#'   ageVector = 1:5,
#'   coefficientForMassA = 0.002,
#'   coefficientForMassAforMature = 0.003,
#'   scaleForMassB = 2.5,
#'   scaleForMassBforMature = 2.7,
#'   conversionToWetMass = 5,
#'   growthModel = growthModel
#' )
#'
#' @export
calculateIndividualsPerInstarCricotopus <- function(totalNumberOfIndividuals, field_density, ageVector, coefficientForMassA, coefficientForMassAforMature, scaleForMassB, scaleForMassBforMature, conversionToWetMass, growthModel, bDensity=-0.75) {
    
    # Predict body lengths at each instar age (including 0)
    L = predict(growthModel, newdata = data.frame(age = c(0.0, ageVector)))

    # Compute body mass for each instar.
    # Mature individuals use specific coefficients and exponents.
    massesVector = ifelse(
        seq_along(L) == length(L),
        (coefficientForMassAforMature * L^scaleForMassBforMature) * conversionToWetMass,
        (coefficientForMassA * L^scaleForMassB) * conversionToWetMass
    )

    # Compute mean body mass assuming one individual per instar
    meanBodyMass <- mean(massesVector)  # n = 1 per instar assumption

    # Compute 'a' constant for the allometric density relationship
    aDensity <- field_density/(meanBodyMass^bDensity)

    # Estimate density per instar using allometric scaling
    densitiesVector <- aDensity*massesVector^bDensity

     # Display intermediate density results for diagnostic purposes
    print("densitiesVector:")
    print(densitiesVector)
    
    # Scale total individuals according to the relative density of each instar
    individualsPerInstar <- round(totalNumberOfIndividuals*densitiesVector/sum(densitiesVector))

    # Return estimated number of individuals per instar
    return(individualsPerInstar)
}
