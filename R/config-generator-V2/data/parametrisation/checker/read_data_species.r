
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


source_with_script_path(file.path(script_path, "load_or_install_package.r"))


#' Read species data from a JSON file
#'
#' This function reads an animal/species JSON file and extracts all relevant
#' biological parameters, including traits, reproduction, growth, length,
#' and other general parameters. It returns a structured list ready for analysis
#' or plotting.
#'
#' @param fileName Character. Path to the JSON file containing species data.
#' @return A list containing:
#' \describe{
#'   \item{traits}{List of traits with min/max values and temperature-dependence info.}
#'   \item{femaleMaxReproductionEvents}{Maximum reproduction events for females.}
#'   \item{eggsPerBatch}{Number of eggs per batch (if fixed).}
#'   \item{interceptForEggBatchFromEquation}{Intercept for egg batch calculation (if using equation).}
#'   \item{slopeForEggBatchFromEquation}{Slope for egg batch calculation (if using equation).}
#'   \item{ageVectorDays}{Vector of ages in days for growth simulation.}
#'   \item{eggDryMass}{Dry mass of eggs.}
#'   \item{devInter}{Developmental interval parameter.}
#'   \item{instarFirstReproduction}{Instar of first reproduction.}
#'   \item{indeterminateGrowth}{Boolean indicating if growth is indeterminate.}
#'   \item{betaScaleTank}{Growth scaling parameter for energy tank.}
#'   \item{minPlasticityKVonBertalanffy}{Minimum plasticity of growth rate.}
#'   \item{capitalBreeding}{Boolean indicating if species is capital breeder.}
#'   \item{numberOfCapitalBreeds}{Number of capital breeding events.}
#'   \item{timeOfReproEventDuringCapitalBreeding}{Timing of reproduction events during capital breeding.}
#'   \item{assignedForMolt}{Assigned resources for molt.}
#'   \item{excessInvestInSize}{Extra investment in size.}
#'   \item{growthModel}{List containing default and temperature-dependent growth models.}
#'   \item{conversionToWetMass}{Conversion factor from dry mass to wet mass.}
#'   \item{femaleWetMass}{Female wet mass.}
#'   \item{coefficientForMassA, scaleForMassB}{Coefficients for mass-length calculations.}
#'   \item{name}{Species name.}
#'   \item{tempFromLab}{Reference temperature from laboratory data.}
#'   \item{YodzisA, YodzisB}{Coefficients for voracity calculation.}
#'   \item{plasticityDueToConditionSearch, plasticityDueToConditionSpeed}{Plasticity coefficients for behavioral adjustments.}
#'   \item{activityUnderPredationRisk}{Activity reduction under predation risk.}
#' }
#' @details
#' - Extracts numeric, character, and boolean values from nested JSON structures.
#' - Handles species-level and individual-level trait definitions.
#' - Computes temperature-dependent traits using tempSizeRuleVector or Pawar 2018 elements.
#' - Supports both fixed egg number and egg number calculated from a linear equation.
#' - Determines indeterminate growth based on instarFirstReproduction and age vector.
#' @examples
#' \dontrun{
#' species_data <- read_data_species("data/Tyrannosaurus_rex_2023.json")
#' }
read_data_species<-function(fileName){
	
    load_or_install_package("jsonlite")

    #' Helper to extract numeric value from JSON by path
    find_number_in_json<-function(path,json_data)
    {
        fields <- unlist(strsplit(path, "\\."))
        value <- json_data
        for (i in seq_along(fields)) {
            field <- fields[i]
            if (!is.na(suppressWarnings(as.numeric(field)))) {
                field <- as.numeric(field)
            }
            if (!is.null(value[[field]])) {
                value <- value[[field]]
            } else {
                if (i < length(fields)) {
                    stop(paste("Error: The field", field, "does not exist in the JSON at path:", path))
                } else {
                    return(NULL) 
                }
            }
        }
        return(as.numeric(value))
    }

    #' Helper to extract character value from JSON by path
    find_char_in_json<-function(path,json_data)
    {
        fields <- unlist(strsplit(path, "\\."))
        value <- json_data
        for (i in seq_along(fields)) {
            field <- fields[i]
            if (!is.na(suppressWarnings(as.numeric(field)))) {
                field <- as.numeric(field)
            }
            if (!is.null(value[[field]])) {
                value <- value[[field]]
            } else {
                if (i < length(fields)) {
                    stop(paste("Error: The field", field, "does not exist in the JSON at path:", path))
                } else {
                    return(NULL) 
                }
            }
        }
        return(value)
    }

    #' Helper to extract boolean value from JSON by path
    find_bool_in_json<-function(path,json_data)
    {
        fields <- unlist(strsplit(path, "\\."))
        value <- json_data
        for (i in seq_along(fields)) {
            field <- fields[i]
            if (!is.na(suppressWarnings(as.numeric(field)))) {
                field <- as.numeric(field)
            }
            if (!is.null(value[[field]])) {
                value <- value[[field]]
            } else {
                if (i < length(fields)) {
                    stop(paste("Error: The field", field, "does not exist in the JSON at path:", path))
                } else {
                    return(NULL) 
                }
            }
        }
        return(as.logical(value))
    }

    #' Recursive helper to get paths to all traits
    get_paths_to_traits <- function(lst, path = character()) {
        paths <- list()
        for (name in names(lst)) {
            value <- lst[[name]]
            new_path <- c(path, name)
            if (is.list(value)) {
                if ("definitionType" %in% names(value)) {
                    paths <- c(paths, list(paste(new_path, collapse = ".")))
                }
                else {
                    paths <- c(paths, get_paths_to_traits(value, new_path))
                }
            }
        }
        return(paths)
    }
    
    data = list()
    anim<-read_json(fileName) 

    traits_paths <- get_paths_to_traits(find_char_in_json("animal.genetics.traits.definition", anim))
    
    for(path in traits_paths) {
        trait_level = find_char_in_json(paste("animal.genetics.traits.definition", path, "definitionType", sep = "."), anim)

        if(trait_level == "SpeciesLevel"){
            data[["traits"]][[path]][["min"]] = find_number_in_json(paste("animal.genetics.traits.definition", path, "speciesLevelParams.value", sep = "."), anim) 
            data[["traits"]][[path]][["max"]] = data[["traits"]][[path]][["min"]]
        }
        else{
            data[["traits"]][[path]][["min"]] = find_number_in_json(paste("animal.genetics.traits.definition", path, "individualLevelParams.ranges.min", sep = "."), anim) 
            data[["traits"]][[path]][["max"]] = find_number_in_json(paste("animal.genetics.traits.definition", path, "individualLevelParams.ranges.max", sep = "."), anim) 
        }

        if(path != "base.energy_tank"){
            data[["traits"]][[path]][["temperature"]][["dependent"]] = find_bool_in_json(paste("animal.genetics.traits.definition", path, "temperature", "dependent", sep = "."), anim)

            if(data[["traits"]][[path]][["temperature"]][["dependent"]]){
                if(path == "base.lengthAtMaturation"){
                    data[["traits"]][[path]][["temperature"]][["tempSizeRuleVector"]] <- list()

                    tempSizeRuleVector <- find_char_in_json(paste("animal.genetics.traits.definition", path, "temperature.tempSizeRuleVector", sep = "."), anim)

                    for(point in seq_along(tempSizeRuleVector)){
                        data[["traits"]][[path]][["temperature"]][["tempSizeRuleVector"]][[point]] <- list()

                        data[["traits"]][[path]][["temperature"]][["tempSizeRuleVector"]][[point]][["temperature"]] = find_number_in_json(paste("animal.genetics.traits.definition", path, "temperature.tempSizeRuleVector", point, "temperature", sep = "."), anim)
                        data[["traits"]][[path]][["temperature"]][["tempSizeRuleVector"]][[point]][["dryMass"]] = find_number_in_json(paste("animal.genetics.traits.definition", path, "temperature.tempSizeRuleVector", point, "dryMass", sep = "."), anim)
                    }
                }else{
                    data[["traits"]][[path]][["temperature"]][["elements"]] <- list()

                    Pawar_2018_elements <- c("activationEnergy", "energyDecay", "temperatureOptimal", "temperatureRef")

                    for (element in Pawar_2018_elements) {
                        element_level = find_char_in_json(paste("animal.genetics.traits.definition", path, "temperature", element, "definitionType", sep = "."), anim)

                        if(element_level == "SpeciesLevel"){
                            data[["traits"]][[path]][["temperature"]][["elements"]][[element]][["min"]] = find_number_in_json(paste("animal.genetics.traits.definition", path, "temperature", element, "speciesLevelParams.value", sep = "."), anim)
                            data[["traits"]][[path]][["temperature"]][["elements"]][[element]][["max"]] = data[["traits"]][[path]][["temperature"]][["elements"]][[element]][["min"]]
                        }
                        else{
                            data[["traits"]][[path]][["temperature"]][["elements"]][[element]][["min"]] = find_number_in_json(paste("animal.genetics.traits.definition", path, "temperature", element, "individualLevelParams.ranges.min", sep = "."), anim)
                            data[["traits"]][[path]][["temperature"]][["elements"]][[element]][["max"]] = find_number_in_json(paste("animal.genetics.traits.definition", path, "temperature", element, "individualLevelParams.ranges.max", sep = "."), anim)
                        }
                    }
                }
            }
        }
        else{
            data[["traits"]][[path]][["temperature"]][["dependent"]] = FALSE
        }
    }


    data$femaleMaxReproductionEvents = find_number_in_json("animal.femaleMaxReproductionEvents",anim)

    data$eggsPerBatchFromEquation = find_bool_in_json("animal.growthModule.eggsPerBatch.eggsPerBatchFromEquation",anim)

    if(data$eggsPerBatchFromEquation){
        data$eggsPerBatch = NULL
        data$interceptForEggBatchFromEquation = find_number_in_json("animal.growthModule.eggsPerBatch.equationParameters.interceptForEggBatchFromEquation",anim)
        data$slopeForEggBatchFromEquation = find_number_in_json("animal.growthModule.eggsPerBatch.equationParameters.slopeForEggBatchFromEquation",anim)
    }
    else{
        data$eggsPerBatch = find_number_in_json("animal.growthModule.eggsPerBatch.value",anim)
        data$interceptForEggBatchFromEquation = NULL
        data$slopeForEggBatchFromEquation = NULL
    }    


    data$ageVectorDays = find_number_in_json("animal.growthModule.ageVector",anim)

    data$eggDryMass = find_number_in_json("animal.growthModule.eggDryMass.value",anim) ##

    data$devInter<-find_number_in_json("animal.devInter",anim)
    
    data$instarFirstReproduction<-find_number_in_json("animal.growthModule.instarFirstReproduction",anim)

    if(data$instarFirstReproduction == length(data$ageVectorDays)+1){ 
        data$indeterminateGrowth = FALSE
    }else{
        data$indeterminateGrowth = TRUE
    }

    data$betaScaleTank = find_number_in_json("animal.growthModule.betaScaleTank",anim) 

    data$minPlasticityKVonBertalanffy = find_number_in_json("animal.growthModule.minPlasticityKVonBertalanffy",anim)

    data$capitalBreeding = find_bool_in_json("animal.growthModule.capitalBreeding.enabled",anim)

    data$numberOfCapitalBreeds = find_number_in_json("animal.growthModule.capitalBreeding.capitalBreedingParams.numberOfCapitalBreeds",anim)
    
    data$timeOfReproEventDuringCapitalBreeding = find_number_in_json("animal.growthModule.capitalBreeding.capitalBreedingParams.timeOfReproEventDuringCapitalBreeding",anim)
    
    data$assignedForMolt = find_number_in_json("animal.growthModule.assignedForMolt",anim)

    data$excessInvestInSize = find_number_in_json("animal.growthModule.excessInvestInSize",anim)


    data$growthModel <- list(
        "defaultAtTempFromLab" = list(),
        "growthModelThermalChanges" = list()
    )

    data$growthModel$defaultAtTempFromLab$model = find_char_in_json("animal.growthModule.growthModel.defaultAtTempFromLab.model",anim)

    data$growthModel$defaultAtTempFromLab$A = find_number_in_json("animal.growthModule.growthModel.defaultAtTempFromLab.A",anim)

    growthModelThermalChanges = find_char_in_json("animal.growthModule.growthModel.temperature.growthModelThermalChanges", anim)

    for(point in seq_along(growthModelThermalChanges))
    {
        data$growthModel$growthModelThermalChanges[[point]] <- list(
            "temperature" = find_number_in_json(paste("animal.growthModule.growthModel.temperature.growthModelThermalChanges", point, "temperature", sep = "."), anim),
            "model" = find_char_in_json(paste("animal.growthModule.growthModel.temperature.growthModelThermalChanges", point, "model", sep = "."), anim),
            "A" = find_number_in_json(paste("animal.growthModule.growthModel.temperature.growthModelThermalChanges", point, "A", sep = "."), anim)
        )
    }

    data$conversionToWetMass = find_char_in_json("animal.conversionToWetMass",anim)

    data$femaleWetMass = find_number_in_json("animal.growthModule.femaleWetMass",anim) 
    
    data$coefficientForMassA = find_number_in_json("animal.growthModule.coefficientForMassA",anim) ##because for holmetaboles ther are 2
    
    data$coefficientForMassAforMature = find_number_in_json("animal.growthModule.coefficientForMassAforMature",anim)
    
    data$scaleForMassB = find_number_in_json("animal.growthModule.scaleForMassB",anim) ##because for holmetaboles ther are 2
    
    data$scaleForMassBforMature = find_number_in_json("animal.growthModule.scaleForMassBforMature",anim)
    
    
    data$name = find_char_in_json("animal.name",anim)

    data$tempFromLab = find_number_in_json("animal.tempFromLab",anim) 

    data$YodzisA = find_number_in_json("animal.YodzisA",anim) 

    data$YodzisB = find_number_in_json("animal.YodzisB",anim) 

    data$plasticityDueToConditionSearch = find_number_in_json("animal.plasticityDueToConditionSearch",anim) 

    data$plasticityDueToConditionSpeed = find_number_in_json("animal.plasticityDueToConditionSpeed",anim)   
		
	data$activityUnderPredationRisk = find_number_in_json("animal.activityUnderPredationRisk",anim)


	return(data) 
}
