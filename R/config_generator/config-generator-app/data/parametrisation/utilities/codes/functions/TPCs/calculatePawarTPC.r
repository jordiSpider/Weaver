#' Pawar et al. (2018) Thermal Performance Curve (TPC) Functions
#'
#' Implements the temperature dependence of biological traits following the Sharpe–Schoolfield
#' model as extended by Pawar et al. (2018). Provides functions to estimate trait values at
#' arbitrary temperatures, calculate trait values at optimal temperature, and fit TPCs using
#' genetic algorithms.
#'
#' @details
#' This file contains several interrelated functions:
#' 
#' - `Pawar_2018`: Computes the trait value at a given temperature using the Pawar et al. 2018 model.
#' - `use_Pawar_2018`: Handles special trait cases (inverse traits, traits not strictly positive)
#'   and calls `Pawar_2018`.
#' - `calculateTraitAtTopt`: Calculates the trait value at the optimal temperature (`Topt`) given a reference trait.
#' - `GAs`: Fits the TPC parameters using a genetic algorithm from the `GA` package.
#' - `getGAresult`: Sets up parameter bounds and calls `GAs` to estimate optimal TPC parameters.
#' - `calculatePawarTPC`: High-level function to fit TPCs for a given trait across multiple temperatures.
#'
#' @section Notes on Trait Handling:
#' Certain traits are "inverse traits" (e.g., development times) that must be inverted before model
#' fitting and transformed back after. Some traits are not strictly positive (e.g., correction factors)
#' and require absolute value handling.
#'
#' @param trait_name Character. Name of the trait being modeled. Determines special handling.
#' @param temperature Numeric. Temperature(s) at which to evaluate the trait (°C).
#' @param traitAtTopt Numeric. Trait value at the optimal temperature.
#' @param Topt Numeric. Optimal temperature for the trait.
#' @param ED Numeric. High-temperature deactivation energy (eV).
#' @param E Numeric. Activation energy (eV).
#' @param Tref Numeric. Reference temperature for normalization (°C).
#' @param traits Data frame with columns `temperature` and `traitValue`, containing observed trait values.
#'
#' @return
#' - `Pawar_2018` / `use_Pawar_2018`: Numeric trait value at specified temperature.
#' - `calculateTraitAtTopt`: Numeric trait value at optimal temperature.
#' - `calculatePawarTPC`: Data frame with estimated TPC parameters (`traitAtTopt`, `E`, `ED`, `Topt`) for the trait.
#'
#' @examples
#' # Example: Fit Pawar TPC for a hypothetical trait
#' traits_data <- data.frame(
#'   temperature = c(10, 15, 20, 25, 30),
#'   traitValue = c(0.1, 0.3, 0.5, 0.4, 0.2)
#' )
#' calculatePawarTPC(
#'   trait_name = "base.devTime",
#'   traits = traits_data,
#'   Tref = 20
#' )
#'
#' @name calculatePawarTPC
#' @export

Pawar_2018<-function(temperature, traitAtTopt,  Topt,  ED,  E, Tref)
{
    # Constants
    T0Kelvin = 273.15
    BOLTZMANN = 8.62E-5

    # Convert temperatures to Kelvin
	tempKelvin = temperature + T0Kelvin
	TrefKelvin = Tref + T0Kelvin
	ToptKelvin = Topt + T0Kelvin

	# Calculate trait at reference temperature
	traitAtTempRef = traitAtTopt / (exp(-E * ((1/(BOLTZMANN*ToptKelvin)) - (1/(BOLTZMANN*TrefKelvin)))) /
	    (1 + (E / (ED - E)) * exp((ED /BOLTZMANN)* ((1/ToptKelvin) - (1/ToptKelvin)))))

    # Calculate trait at the target temperature
	trait = traitAtTempRef * 
	    exp(-E * ((1/(BOLTZMANN*tempKelvin)) - (1/(BOLTZMANN*TrefKelvin)))) /
	    (1 + (E / (ED - E)) * exp((ED /BOLTZMANN)* ((1/ToptKelvin) - (1/tempKelvin))))
	
    # Prevent trait from going too close to zero
	propOftraitAtTopt=1e-02
	nextToZeroTrait = propOftraitAtTopt*traitAtTopt
	if(trait < nextToZeroTrait){
		trait = nextToZeroTrait
	}

	return(trait)
}

use_Pawar_2018<-function(trait_name, temperature, traitAtTopt,  Topt,  ED,  E, Tref)
{
    # Traits that require inversion for model fitting
    inverseTraits = list(
        "base.eggDevTime", "base.actE_met", "base.devTime", "base.pupaPeriodTime"
    )
    # Traits that can be negative or zero
    nonStrictlyPositiveTraits = list(
        "base.factorEggMass"
    )

    strictlyPositive <- !(trait_name %in% nonStrictlyPositiveTraits)
    inverse <- (trait_name %in% inverseTraits)

    # Adjust non-strictly positive traits
	if(!strictlyPositive){
		traitBefore <- traitAtTopt
		if(traitBefore < 0.0){
			traitAtTopt = abs(traitAtTopt)
		}
	}

    # Invert traits if required
	if(inverse){
		traitAtTopt = 1.0/traitAtTopt
	}

    # Compute trait at temperature
	trait = Pawar_2018(temperature, traitAtTopt, Topt, ED, E, Tref)

    # Revert inversion
	if(inverse){
		trait = 1.0/trait
	}

    # Adjust non-strictly positive traits
	if(!strictlyPositive){
		if(traitBefore < 0.0){
			trait = trait - 2*abs(traitBefore)
		}
	}

	return(trait)
}


calculateTraitAtTopt <- function(trait_name, traitAtTempRef,  Topt,  ED,  E, Tref){

	inverseTraits = list(
        "base.eggDevTime", "base.actE_met", "base.devTime", "base.pupaPeriodTime"
    )
    nonStrictlyPositiveTraits = list(
        "base.factorEggMass"
    )

    strictlyPositive <- !(trait_name %in% nonStrictlyPositiveTraits)
    inverse <- (trait_name %in% inverseTraits)

	if(!strictlyPositive){
		traitBefore <- traitAtTempRef
		if(traitBefore < 0.0){
			traitAtTempRef = abs(traitAtTempRef)
		}
	}

	if(inverse){
		traitAtTempRef = 1.0/traitAtTempRef
	}

	T0Kelvin = 273.15
    BOLTZMANN = 8.62E-5

	TrefKelvin = Tref + T0Kelvin
	ToptKelvin = Topt + T0Kelvin

    # Compute trait at Topt
	traitAtTopt = traitAtTempRef * (exp(-E * ((1/(BOLTZMANN*ToptKelvin)) - (1/(BOLTZMANN*TrefKelvin)))) /
		(1 + (E / (ED - E)) * exp((ED /BOLTZMANN)* ((1/ToptKelvin) - (1/ToptKelvin)))))

	if(inverse){
		traitAtTopt = 1.0/traitAtTopt
	}

	if(!strictlyPositive){
		if(traitBefore < 0.0){
			traitAtTopt = traitAtTopt - 2*abs(traitBefore)
		}
	}

	return(traitAtTopt)
}

GAs<-function(trait_name, TPC,lower,upper,Tref) 
{
	library(GA)

    nIniPop=1000
    fractBest = 0.1
    nOffs = 1000
    nGenerations = 200
    mutRate=0.1
	params = c("traitAtTopt", "E", "ED","Topt")

    # Fitness function (negative sum of squared errors)
	fitnessL2 <- function(theta, x, y,Tref) {
		theta <- setNames(theta, params)
		values <- sapply(x, function(temp) {
			use_Pawar_2018(
				trait_name = trait_name,
				temperature = temp, 
				traitAtTopt = theta[["traitAtTopt"]],  
				Topt = theta[["Topt"]],  
				ED = theta[["ED"]],  
				E = theta[["E"]], 
				Tref = Tref
			)
		})
		return(-sum((y - values)^2))
	}

    # Run genetic algorithm to optimize TPC parameters
	GA2 <- ga(type = "real-valued",seed=123,pmutation=mutRate, 
		Tref=Tref,
		fitness = fitnessL2,
		x = TPC$temperature, y = TPC$traitValue, lower = lower, 
		upper = upper,
		popSize = 500, crossover = gareal_blxCrossover, maxiter = 5000,
		run = 200,monitor=FALSE,keepBest=TRUE,
		names = params,
		elitism = base::max(1, round(nOffs*fractBest)),parallel=FALSE
	)

	result<-summary(GA2)$solution[1,1:4]
	print(summary(GA2))
	
    return(result)
}

getGAresult<-function(trait_name, traits,Tref,ToptIni,EIni,EDIni,traitAtToptIni)
{
	lower =	c(min(traitAtToptIni),min(EIni),min(EDIni),min(ToptIni))
	upper = c(max(traitAtToptIni),max(EIni),max(EDIni),max(ToptIni))

	GAresult<-GAs(trait_name=trait_name, TPC=traits,lower,upper,Tref)

    return(GAresult)
}

#' Fit Pawar Thermal Performance Curve (TPC) Using Genetic Algorithm
#'
#' @param trait_name Character. Trait to fit (e.g., "base.devTime").
#' @param traits Data frame with columns `temperature` and `traitValue`.
#' @param Tref Numeric. Reference temperature (°C).
#'
#' @return Data frame with columns: `traitAtTopt`, `E`, `ED`, `Topt`.
#'
#' @export
calculatePawarTPC <- function(trait_name, traits, Tref) {
    ToptIni = seq(min(traits$temperature),max(traits$temperature),length.out=10) 
    EIni = seq(0.1,2.5,length.out=10)
    EDIni = seq(0.5,20,length.out=10)
    traitAtToptIni = seq(0,2.0*max(traits$traitValue,na.rm=T),length.out=10)

	GAresult<-getGAresult(trait_name, traits,Tref,ToptIni,EIni,EDIni,traitAtToptIni)
    GAresult <- as.data.frame(t(GAresult))

    return(GAresult)
}
