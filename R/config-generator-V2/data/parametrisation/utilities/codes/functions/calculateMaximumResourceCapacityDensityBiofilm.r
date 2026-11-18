#' Calculate Maximum Resource Capacity Density for a Biofilm System
#'
#' Computes the theoretical maximum resource capacity density in a biofilm ecosystem,
#' based on the biomass ingestion potential of individuals at different life stages (instars),
#' their growth model, and the total available landscape area.
#'
#' The function estimates the total ingested biomass per day across all instars and
#' normalizes it by the habitat area to yield the maximum resource capacity density.
#'
#' @param individualsPerInstar Numeric vector indicating the number of individuals
#'        in each developmental stage (instar).
#' @param landscapeArea Numeric value representing the total area of the simulated
#'        landscape (in square meters or relevant spatial units).
#' @param ageVector Numeric vector of ages corresponding to the instar stages.
#' @param coefficientForMassA Numeric coefficient \(A\) used in the allometric mass
#'        scaling relationship for non-mature stages.
#' @param coefficientForMassAforMature Numeric coefficient \(A_{mature}\) used for
#'        mature individuals in the mass scaling relationship.
#' @param scaleForMassB Numeric exponent \(B\) in the allometric mass equation
#'        for non-mature stages.
#' @param scaleForMassBforMature Numeric exponent \(B_{mature}\) for mature individuals
#'        in the mass scaling relationship.
#' @param conversionToWetMass Numeric factor for converting dry mass to wet mass
#'        (dimensionless).
#' @param growthModel A predictive growth model (e.g., a nonlinear regression or logistic model)
#'        capable of estimating body length from age via \code{predict()}.
#'
#' @return A single numeric value representing the **maximum resource capacity density**
#'         (e.g., biomass per unit area per day) for the biofilm ecosystem.
#'
#' @details
#' The function follows these computational steps:
#' \enumerate{
#'   \item Predict body length (\eqn{L}) at each age using the provided \code{growthModel}.
#'   \item Compute individual wet masses for each instar using allometric relationships:
#'     \deqn{mass = A \cdot L^B \cdot conversionToWetMass}
#'     where \(A\) and \(B\) depend on whether the individual is mature or not.
#'   \item Estimate maximum ingestion rate per instar:
#'     \deqn{I_{max} = 0.1 \cdot mass^{0.75}}
#'   \item Compute total ingested wet biomass per day across all instars:
#'     \deqn{B_{total} = \sum (individualsPerInstar \cdot I_{max})}
#'   \item Normalize by the landscape area to obtain resource capacity density:
#'     \deqn{R_{density} = B_{total} / landscapeArea}
#' }
#'
#' This value provides an estimate of the maximum possible biomass flow
#' that can be sustained by the biofilm given its biological and spatial constraints.
#'
#' @examples
#' \dontrun{
#' # Example: Simulate a simple case with a logistic growth model
#' growth_model <- nls(L ~ Lmax / (1 + exp(-k * (age - t0))),
#'                     data = data.frame(age = 1:10, L = seq(1, 10, length.out = 10)),
#'                     start = list(Lmax = 10, k = 0.2, t0 = 5))
#'
#' individuals <- c(100, 80, 60, 40, 20)
#' ages <- c(1, 2, 3, 4, 5)
#'
#' density <- calculateMaximumResourceCapacityDensityBiofilm(
#'     individualsPerInstar = individuals,
#'     landscapeArea = 1000,
#'     ageVector = ages,
#'     coefficientForMassA = 0.005,
#'     coefficientForMassAforMature = 0.008,
#'     scaleForMassB = 3,
#'     scaleForMassBforMature = 3.1,
#'     conversionToWetMass = 2.5,
#'     growthModel = growth_model
#' )
#'
#' print(density)
#' }
#'
#' @export
calculateMaximumResourceCapacityDensityBiofilm <- function(individualsPerInstar, landscapeArea, ageVector, coefficientForMassA, coefficientForMassAforMature, scaleForMassB, scaleForMassBforMature, conversionToWetMass, growthModel) {
    # Predict body length values from the growth model
    L = predict(growthModel, newdata = data.frame(age = c(0.0, ageVector)))

    # Compute mass per instar (different formulas for mature and non-mature stages)
    massesVector = ifelse(
        seq_along(L) == length(L),
        (coefficientForMassAforMature * L^scaleForMassBforMature) * conversionToWetMass,
        (coefficientForMassA * L^scaleForMassB) * conversionToWetMass
    )

    # Estimate the maximum ingestion rate per instar
    maxIngestionPerInstar = 0.1*massesVector^0.75

    # Compute total ingested biomass per day across all instars
    totalIngestedWetBiomassInSimulationPerDay=sum(individualsPerInstar*maxIngestionPerInstar)

    # Normalize by landscape area to get the resource capacity density
    maximumResourceCapacityDensity = totalIngestedWetBiomassInSimulationPerDay/landscapeArea

    return(maximumResourceCapacityDensity)
}
