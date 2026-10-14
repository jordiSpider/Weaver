
rm(list=ls(all=TRUE))

if (interactive()) {
    script_path <- sys.frame(1)$ofile
    script_path <- dirname(script_path)
} else {
    args <- commandArgs(trailingOnly = FALSE)
    script_path <- sub("--file=", "", args[grep("--file=", args)])
    script_path <- dirname(script_path)
}


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


source_with_script_path(file.path(script_path, "..", "check_species.r"))


check_species(
	fileName=file.path(script_path, "..", "species_to_check", "species", "Isoperla grammatica.json"),
	simulationType = "Aquatic",
	temperature = "useTempFromLab",
	profitability = 0.0,
	timeStep=1,
	todayEncountersWithPredators=0, 
	maxEncountersPerDayInPop=2,
	silent=FALSE
)
