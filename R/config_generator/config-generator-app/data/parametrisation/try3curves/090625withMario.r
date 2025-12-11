
rm(list=ls(all=TRUE))

if (interactive()) {
    script_path <- sys.frame(1)$ofile
    script_path <- dirname(script_path)
} else {
    args <- commandArgs(trailingOnly = FALSE)
    script_path <- sub("--file=", "", args[grep("--file=", args)])
    script_path <- dirname(script_path)
}

parametrisation_path <- sub("(.*/parametrisation)/.*", "\\1", script_path)


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


source_with_script_path(file.path(parametrisation_path, "checker", "check_species.r"))


# path_to_weaver_result <- "C:/Users/davqu/Documents/weaver/default_Amblyseius_swirskii__2025-2-14_20-17-0_instar_1"
# library(jsonlite)
# data_path_sim_params <- read_json(file.path(path_to_weaver_result, "config", "simulation_params.json")) # for time step per day
# timeStepsPerDay <- data_path_sim_params$simulation$timeStepsPerDay

# world_params <- read_json(file.path(path_to_weaver_result, config, "world_params.json")) 
# simulationType <- data_path_sim_params$world$simulationType



# check_species(
# 	fileName=file.path(path_to_weaver_result, "config", "species", "Amblyseius_swirskii.json"),
# 	simulationType = simulationType,
# 	temperature = "useTempFromLab",
# 	profitability = 0,
# 	timeStep=timeStepsPerDay,
# 	silent=TRUE
# )
