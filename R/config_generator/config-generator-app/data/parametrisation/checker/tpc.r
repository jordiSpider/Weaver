
if (!exists("called_from_main", inherits = FALSE)) {
  stop("This script must be sourced from a main script.")
}

#' Source a script while retaining its path
#'
#' This function sources an R script in a local environment, keeping the script's directory
#' available and exporting all functions to the parent environment.
#'
#' @param path Character. Path to the R script to source.
#' @return Invisibly returns the local environment containing sourced objects.
#' @examples
#' \dontrun{
#' source_with_script_path("scripts/load_or_install_package.r")
#' }
source_with_script_path <- function(path) {
	path <- normalizePath(path)

    parent_env <- parent.frame()
    local_env <- new.env(parent = parent_env)

	local_env$script_path <- dirname(path)

	local_env$called_from_main <- TRUE

	sys.source(path, envir = local_env)

	for (name in ls(local_env)) {
		obj <- local_env[[name]]
		if (is.function(obj)) {
			assign(name, obj, envir = parent_env)
		}
	}

	invisible(local_env)
}


source_with_script_path(file.path(script_path, "math_functions.r"))
source_with_script_path(file.path(script_path, "mass_length_functions.r"))


#' Pawar 2018 Temperature Performance Curve
#'
#' Calculates the temperature-dependent value of a trait using the Pawar et al. (2018) model.
#'
#' @param temperature Numeric. Environmental temperature in °C.
#' @param traitAtTopt Numeric. Trait value at the optimal temperature.
#' @param Topt Numeric. Optimal temperature for the trait in °C.
#' @param ED Numeric. Deactivation energy parameter.
#' @param E Numeric. Activation energy parameter.
#' @param Tref Numeric. Reference temperature in °C.
#'
#' @return Numeric. Trait value adjusted for the given temperature.
#' @export
Pawar_2018<-function(temperature, traitAtTopt,  Topt,  ED,  E, Tref)
{
    T0Kelvin = 273.15
    BOLTZMANN = 8.62E-5

	tempKelvin = temperature + T0Kelvin

	TrefKelvin = Tref + T0Kelvin

	ToptKelvin = Topt + T0Kelvin

	traitAtTempRef = traitAtTopt / (exp(-E * ((1/(BOLTZMANN*ToptKelvin)) - (1/(BOLTZMANN*TrefKelvin)))) /
		(1 + (E / (ED - E)) * exp((ED /BOLTZMANN)* ((1/ToptKelvin) - (1/ToptKelvin)))))

	trait = traitAtTempRef * 
		exp(-E * ((1/(BOLTZMANN*tempKelvin)) - (1/(BOLTZMANN*TrefKelvin)))) /
		(1 + (E / (ED - E)) * exp((ED /BOLTZMANN)* ((1/ToptKelvin) - (1/tempKelvin))))

	propOftraitAtTopt=1e-02

	nextToZeroTrait = propOftraitAtTopt*traitAtTopt
	
	if(trait < nextToZeroTrait){
		trait = nextToZeroTrait
	}

	return(trait)
}

#' Apply Pawar 2018 TPC with trait-specific transformations
#'
#' Adjusts the trait value for traits that require inversion or can be negative,
#' then applies the Pawar 2018 TPC.
#'
#' @param trait_name Character. Name of the trait.
#' @param temperature Numeric. Environmental temperature in °C.
#' @param traitAtTopt Numeric. Trait value at the optimal temperature.
#' @param Topt Numeric. Optimal temperature for the trait in °C.
#' @param ED Numeric. Deactivation energy parameter.
#' @param E Numeric. Activation energy parameter.
#' @param Tref Numeric. Reference temperature in °C.
#'
#' @return Numeric. Trait value adjusted for temperature and trait-specific transformations.
#' @export
use_Pawar_2018<-function(trait_name, temperature, traitAtTopt,  Topt,  ED,  E, Tref)
{
    inverseTraits = list(
        "base.eggDevTime", "base.actE_met", "base.devTime", "base.pupaPeriodTime"
    )
	
    nonStrictlyPositiveTraits = list(
        "base.factorEggMass"
    )

    strictlyPositive <- !(trait_name %in% nonStrictlyPositiveTraits)

    inverse <- (trait_name %in% inverseTraits)

	if(!strictlyPositive){
		traitBefore <- traitAtTopt

		if(traitBefore < 0.0){
			traitAtTopt = abs(traitAtTopt)
		}
	}

	if(inverse){
		traitAtTopt = 1.0/traitAtTopt
	}

	trait = Pawar_2018(temperature, traitAtTopt, Topt, ED, E, Tref)

	if(inverse){
		trait = 1.0/trait
	}

	if(!strictlyPositive){
		if(traitBefore < 0.0){
			trait = trait - 2*abs(traitBefore)
		}
	}

	return(trait)
}

#' Temperature-Size Rule (TSR) interpolation
#'
#' Computes the length of an individual based on a temperature-size rule (TSR) vector.
#'
#' @param temperature Numeric. Environmental temperature in °C.
#' @param tempSizeRuleVector List. Vector of temperature-dry mass pairs.
#' Each element must have \code{temperature} and \code{dryMass} fields.
#' @param coefficientForMassAforMature Numeric. Coefficient for converting mass to length.
#' @param scaleForMassBforMature Numeric. Exponent for converting mass to length.
#'
#' @return Numeric. Length of the individual at the given temperature.
#' @export
TSR<-function(temperature, tempSizeRuleVector, coefficientForMassAforMature, scaleForMassBforMature)
{
	if (length(tempSizeRuleVector) < 2) {
		stop("The vector must contain at least two elements.")
	}

	postTSRMass <- NA
	
	if (temperature < tempSizeRuleVector[[1]]$temperature) {
		
		postTSRMass <- interpol2(
			temperature,
			tempSizeRuleVector[[1]]$temperature,
			tempSizeRuleVector[[2]]$temperature,
			tempSizeRuleVector[[1]]$dryMass,
			tempSizeRuleVector[[2]]$dryMass
		)
	}
	
	if (temperature > tempSizeRuleVector[[length(tempSizeRuleVector)]]$temperature) {
		postTSRMass <- interpol2(
			temperature,
			tempSizeRuleVector[[length(tempSizeRuleVector) - 1]]$temperature,
			tempSizeRuleVector[[length(tempSizeRuleVector)]]$temperature,
			tempSizeRuleVector[[length(tempSizeRuleVector) - 1]]$dryMass,
			tempSizeRuleVector[[length(tempSizeRuleVector)]]$dryMass
		)
	}
	
	for (i in 1:(length(tempSizeRuleVector) - 1)) {
		temp1 <- tempSizeRuleVector[[i]]$temperature
		temp2 <- tempSizeRuleVector[[i + 1]]$temperature
		dryMass1 <- tempSizeRuleVector[[i]]$dryMass
		dryMass2 <- tempSizeRuleVector[[i + 1]]$dryMass
		
		if (temperature >= temp1 && temperature <= temp2) {

			postTSRMass <- interpol2(
				temperature,
				temp1,
				temp2,
				dryMass1,
				dryMass2
			)
		}
	}

	postTSRLength <- calculateDryLength(postTSRMass, coefficientForMassAforMature, scaleForMassBforMature)

	return(postTSRLength)
}

#' Apply Temperature-Size Rule with genetic shift
#'
#' Adjusts the TSR using a genetic shift relative to a reference trait value at Tref.
#'
#' @param temperature Numeric. Environmental temperature in °C.
#' @param tempSizeRuleVector List. Vector of temperature-dry mass pairs.
#' Each element must have \code{temperature} and \code{dryMass} fields.
#' @param coefficientForMassAforMature Numeric. Coefficient for converting mass to length.
#' @param scaleForMassBforMature Numeric. Exponent for converting mass to length.
#' @param traitAtTempRef Numeric. Trait value at the reference temperature.
#' @param Tref Numeric. Reference temperature in °C.
#'
#' @return Numeric. Length of the individual at the given temperature, adjusted for genetic shift.
#' @export
use_TSR<-function(temperature, tempSizeRuleVector, coefficientForMassAforMature, scaleForMassBforMature, traitAtTempRef, Tref){

	traitAtTempRefWithoutShift = TSR(Tref, tempSizeRuleVector, coefficientForMassAforMature, scaleForMassBforMature)

	geneticShift = traitAtTempRef / traitAtTempRefWithoutShift

	tempSizeRuleVectorWithShift <- list()

	for (i in seq_along(tempSizeRuleVector)) {
		tempSizeRuleVectorWithShift[[i]] <- list(
			temperature = tempSizeRuleVector[[i]]$temperature,
			dryMass = tempSizeRuleVector[[i]]$dryMass * geneticShift
		)
	}

	return(TSR(temperature, tempSizeRuleVectorWithShift, coefficientForMassAforMature, scaleForMassBforMature))
}
