
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


#' Check Species Simulation
#'
#' Performs a simulation check for a species using its data file. 
#' Loads species data, runs a test simulation for growth, reproduction, and viability, 
#' and optionally plots species traits and individual simulation results.
#'
#' @param fileName Character. Path to the species data file.
#' @param simulationType Character. Type of simulation to run (e.g., "Unlimited").
#' @param temperature Numeric. Temperature value used in the simulation.
#' @param profitability Numeric. Profitability parameter for the simulation (affects energy acquisition).
#' @param timeStep Numeric. Conversion factor from simulation steps to real-world time units (e.g., days).
#' @param todayEncountersWithPredators Numeric. Number of predator encounters for the individual today.
#' @param maxEncountersPerDayInPop Numeric. Maximum number of predator encounters allowed per day in the population.
#' @param silent Logical. If TRUE, suppresses all plots and printed output. Default is FALSE.
#' @param xAxisRange Numeric or NULL. Maximum value for the x-axis in plots. If NULL, the range is determined automatically.
#'
#' @return A list containing the simulation results for the "Unlimited" scenario. 
#' The list includes time series for body size, dry mass, growth, voracity, eggs, and other individual traits.
#' 
#' @examples
#' \dontrun{
#' result <- check_species(
#'   fileName = "species_data.json",
#'   simulationType = "Unlimited",
#'   temperature = 25,
#'   profitability = 1.2,
#'   timeStep = 1,
#'   todayEncountersWithPredators = 0,
#'   maxEncountersPerDayInPop = 5,
#'   silent = FALSE
#' )
#' }
#'
#' @export
check_species<-function(fileName, simulationType, temperature, profitability, timeStep, todayEncountersWithPredators, maxEncountersPerDayInPop, silent=FALSE, xAxisRange = NULL){
	
	multiplierForFieldMetabolicRate = 3.0

	timeAddedToMeetLastRepro = 0

	N=10

	corrLk = -0.5
	corrLdevT = 0.5
	corrkdevT = -0.5
	
	data_species <- read_data_species(fileName)

	if(!silent){
		plot_data_species(data_species)
	}
	
	result<-tryParams(data_species=data_species, timeAddedToMeetLastRepro=timeAddedToMeetLastRepro, N=N, 
		corrLk=corrLk, corrLdevT=corrLdevT, corrkdevT=corrkdevT, simulationType=simulationType, 
		temperature=temperature, propMov=0.0, multiplierForFieldMetabolicRate=multiplierForFieldMetabolicRate, 
		timeStep=timeStep, includeMetabolicRate=TRUE, profitability=profitability, silent=silent,
		todayEncountersWithPredators=todayEncountersWithPredators, maxEncountersPerDayInPop=maxEncountersPerDayInPop)


	if(!silent){
		viability_plot(data_species, result$simulations, result$individual, timeStep, xAxisRange)
	}

	return(result$simulations[["Unlimited"]])
}
