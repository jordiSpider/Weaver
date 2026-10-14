#' Run Resource Species Information Script
#'
#' Loads and executes the information script associated with a specific resource species.
#' This function dynamically determines the script path for the selected species,
#' sources it into a provided environment, and returns that environment containing
#' all variables and functions defined in the sourced script.
#'
#' It ensures that the environment contains an `app` list for storing additional data.
#'
#' @param species_name Character string with the name of the resource species whose script will be executed.
#' @param env Environment object where the sourced script will be evaluated.
#'        If it does not contain an `app` element, one is created automatically.
#'
#' @return The modified environment `env` after sourcing the species information script.
#'
#' @details
#' The function expects the global object `resource_species_paths_list` to exist in the workspace.
#' This list should map each species name to its corresponding directory path containing the `_info.r` script.
#'
#' For example:
#' \preformatted{
#' resource_species_paths_list <- list(
#'   "Grass" = "grass_data",
#'   "Tree"  = "tree_data"
#' )
#' }
#'
#' The function will then source:
#' \preformatted{
#' data/parametrisation/resource/grass_data/grass_data_info.r
#' }
#'
#' @examples
#' \dontrun{
#' env <- new.env()
#' env <- run_resource_species_info_script("Grass", env)
#' print(ls(env))  # Show objects loaded from the species info script
#' }
#'
#' @export
run_resource_species_info_script <- function(species_name, env) {
    # Ensure the environment has an app list
    if (!"app" %in% ls(envir=env)) {
        env$app <- list()
    }
    
    # Build the absolute path to the resource species script directory
    env$script_directory <- file.path(getwd(), "data", "parametrisation", "resource", resource_species_paths_list[[species_name]])

    # Dynamically load the corresponding _info.r script into the environment
    source(file.path(env$script_directory, paste0(basename(resource_species_paths_list[[species_name]]), "_info.r")), local = env)

    # Return the modified environment
    return(env)
}
