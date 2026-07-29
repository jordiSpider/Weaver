
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


source_with_script_path(file.path(script_path, "read_data_species.r"))
source_with_script_path(file.path(script_path, "plot_data_species.r"))

source_with_script_path(file.path(script_path, "tryParams.r"))
source_with_script_path(file.path(script_path, "viability_plot.r"))


#' Test Species Simulation
#'
#' Runs a test simulation for a species with adjustable parameters. 
#' Allows for modifying growth, reproduction, development, and metabolic traits for debugging or experimental purposes.
#' The function plots species data and runs a viability simulation, displaying growth, mass, and reproduction traces.
#'
#' @param type Integer. Type of test species to run. Currently, `1` corresponds to "Cricotopus spx".
#' @param todayEncountersWithPredators Numeric. Number of predator encounters for the individual today.
#' @param maxEncountersPerDayInPop Numeric. Maximum number of predator encounters allowed per day in the population.
#'
#' @return No return value. The function produces plots of species traits and simulation outputs.
#'
#' @details
#' The function internally:
#' \itemize{
#'   \item Reads species data from a JSON file.
#'   \item Applies optional test adjustments to growth, egg production, length at maturation, development time, and breeding strategy.
#'   \item Plots the species data for inspection.
#'   \item Runs a viability simulation using `tryParams`.
#'   \item Plots the simulation results using `viability_plot`.
#' }
#'
#' @examples
#' \dontrun{
#' test_species(
#'   type = 1,
#'   todayEncountersWithPredators = 0,
#'   maxEncountersPerDayInPop = 5
#' )
#' }
#'
#' @export
test_species<-function(type, todayEncountersWithPredators, maxEncountersPerDayInPop){

    apply_test_params<-function(data_species, Debug, kProduct, LdistProd, fractToReduceEggs, instarFirstReproduction, capitalBreeding, numberOfCapitalBreeds, timeOfReproEventDuringCapitalBreeding, devTimer, fixLengthAtBirth){
        
        if(!is.na(Debug)){
            if(Debug){
                data_species$growth$min<-data_species$growth$max<-(data_species$growth$max+data_species$growth$min)/2

                data_species$lengthAtMaturation$min<-data_species$lengthAtMaturation$max<-(data_species$lengthAtMaturation$max+data_species$lengthAtMaturation$min)/2

                data_species$factorEggMass$min<-data_species$factorEggMass$max<-0

                data_species$devTime$min<-data_species$devTime$max<-data_species$ageVectorDays[length(ageVectorDays)]

                data_species$met_rate$min<-data_species$met_rate$max<-(data_species$met_rate$max+data_species$met_rate$min)/2
                
                data_species$actE_met$min<-data_species$actE_met$max<-(data_species$actE_met$max+data_species$actE_met$min)/2
            }
        }

        if(!is.na(kProduct)){
            data_species$growth$min=data_species$growth$min-data_species$growth$min*kProduct
            data_species$growth$max=data_species$growth$max+data_species$growth$max*kProduct
        }

        if(!is.na(LdistProd)){
            data_species$lengthAtMaturation$min = data_species$lengthAtMaturation$min-data_species$lengthAtMaturation$min*LdistProd
            data_species$lengthAtMaturation$max = data_species$lengthAtMaturation$max+data_species$lengthAtMaturation$max*LdistProd
        }

        if(!is.na(fractToReduceEggs)){
            data_species$eggsPerBatch = round(data_species$eggsPerBatch*fractToReduceEggs)

            data_species$eggDryMass=data_species$eggDryMass*fractToReduceEggs
        }

        if(!is.na(instarFirstReproduction)){
            data_species$instarFirstReproduction = instarFirstReproduction

            if(data_species$instarFirstReproduction == length(data_species$ageVectorDays)+1){ 
                data_species$indeterminateGrowth = TRUE
            }else{
                data_species$indeterminateGrowth = FALSE
            }
        }

        if(!is.na(capitalBreeding)){
            data_species$capitalBreeding = capitalBreeding

            if(capitalBreeding & !is.na(numberOfCapitalBreeds) & !is.na(timeOfReproEventDuringCapitalBreeding)){
                data_species$numberOfCapitalBreeds = numberOfCapitalBreeds

                data_species$timeOfReproEventDuringCapitalBreeding = timeOfReproEventDuringCapitalBreeding
            }
        }

        if(!is.na(devTimer)){
            data_species$devTime$min = data_species$devTime$min-data_species$devTime$min*devTimer
            data_species$devTime$max = data_species$devTime$max+data_species$devTime$max*devTimer
        }

        if(!is.na(fixLengthAtBirth)){
            if(fixLengthAtBirth){
                data_species$factorEggMass$min<-data_species$factorEggMass$max<-0
            }
        }

        return(data_species)
    }


    if(type==1){

		fileName=file.path(script_path, "Cricotopus spx.json")   
		simulationType = "Arthropods" 
		temperature = "useTempFromLab" 
		multiplierForFieldMetabolicRate = 3
		propMov = 0
		timeStep=1
		timeAddedToMeetLastRepro = 3
		includeMetabolicRate = TRUE

		N=10

		corrLk = -0.5
		corrLdevT = 0.5
		corrkdevT = -0.5

        Debug=NA
        kProduct=NA
        LdistProd=NA
        fractToReduceEggs=NA
        instarFirstReproduction=NA
        capitalBreeding=NA
        numberOfCapitalBreeds=NA
        timeOfReproEventDuringCapitalBreeding=NA
        devTimer=NA
        fixLengthAtBirth=NA
	}


    data_species <- read_data_species(fileName)

    data_species <- apply_test_params(data_species, Debug, kProduct, LdistProd, fractToReduceEggs, instarFirstReproduction, capitalBreeding, numberOfCapitalBreeds, timeOfReproEventDuringCapitalBreeding, devTimer, fixLengthAtBirth)

    plot_data_species(data_species)

    results<-tryParams(data_species, timeAddedToMeetLastRepro, N, corrLk, corrLdevT, corrkdevT, simulationType, temperature, propMov, multiplierForFieldMetabolicRate, timeStep, includeMetabolicRate, silent=FALSE, todayEncountersWithPredators=todayEncountersWithPredators, maxEncountersPerDayInPop=maxEncountersPerDayInPop)

	viability_plot(results$currentAge, results$currentDryMass, results$nextMassPredicted, results$netGrowth)
}
