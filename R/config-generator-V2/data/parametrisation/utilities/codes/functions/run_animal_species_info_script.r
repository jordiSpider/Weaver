#' Run Animal Species Information Script
#'
#' Loads and executes the information script associated with a specific animal species.
#' This function dynamically constructs the path to the species’ directory and sources
#' its corresponding baseline/derived scripts into the provided environment.
#'
#' It is typically used within the Shiny server logic to dynamically load growth
#' parameters, behavioral traits, or other model data for a selected species.
#'
#' @param species_name Character string with the name of the animal species whose
#'        script should be executed.
#' @param env Environment object in which the sourced script will be evaluated.
#'        This allows the function to load all the variables and functions defined
#'        inside the species’ information script directly into that environment.
#' @param apply_baseline Logical flag. When `TRUE`, source `<species>_baseline.r`
#'        before `<species>_derived.r`; when `FALSE`, only source `_derived.r`
#'        (used when the app injects edited `param.*` values before recalculation).
#'
#' @return The modified environment `env`, containing all objects created
#'         by the sourced `_info.r` script.
#'
#' @details
#' This function relies on the global list `animal_species_paths_list`,
#' which maps each animal species name to its corresponding directory path.
#'
#' The expected directory structure is:
#' \preformatted{
#' data/
#' └── parametrisation/
#'     └── animalia/
#'         └── <Phylum>/<Class>/<Order>/<Family>/<Genus>/<Species>/
#'             └── <Species>_info.r
#' }
#'
#' For example, if:
#' \preformatted{
#' animal_species_paths_list <- list("Canis lupus" = "Chordata/Mammalia/Carnivora/Canidae/Canis/Canis_lupus")
#' }
#'
#' then this function will source:
#' \preformatted{
#' data/parametrisation/animalia/Chordata/Mammalia/Carnivora/Canidae/Canis/Canis_lupus/Canis_lupus_baseline.r
#' data/parametrisation/animalia/Chordata/Mammalia/Carnivora/Canidae/Canis/Canis_lupus/Canis_lupus_derived.r
#' }
#'
#' @examples
#' \dontrun{
#' env <- new.env()
#' env <- run_animal_species_info_script("Canis lupus", env)
#' print(ls(env))  # List variables loaded from the species info script
#' }
#'
#' @export
run_animal_species_info_script <- function(species_name, env, apply_baseline = TRUE) {
    # Build the path to the directory containing the species information script
    env$script_path <- file.path(getwd(), animal_species_paths_list[[species_name]])
    env$parametrisation_path <- sub("(.*/parametrisation)/.*", "\\1", env$script_path)
    
    if(apply_baseline) {
        # Baseline seeds default values that can later be transformed by the
        # derived script.
        source(file.path(env$script_path, paste0(basename(animal_species_paths_list[[species_name]]), "_baseline.r")), local = env)
    }

    # Derived script computes final values, plots and bibliography metadata.
    source(file.path(env$script_path, paste0(basename(animal_species_paths_list[[species_name]]), "_derived.r")), local = env)

    # Return the environment with the loaded data and functions
    return(env)
}
