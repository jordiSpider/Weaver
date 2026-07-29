#' Global Data Initialization for Configuration Generator App
#'
#' This script initializes the global data structures for the Shiny
#' Configuration Generator application. It prepares registries and
#' path lists for both animal species and resource species by scanning
#' the appropriate directories under the `data/parametrisation` folder.
#'
#' @details
#' The global variables defined here are:
#' - `animal_species_register`: A data frame storing taxonomic information
#'   for each animal species.
#' - `animal_species_paths_list`: A named list mapping each animal species
#'   to its corresponding path in the file system.
#' - `resource_species_register`: A data frame storing world and resource species
#'   names.
#' - `resource_species_paths_list`: A named list mapping each resource species
#'   to its corresponding path in the file system.
#'
#' Directory scanning rules:
#' - Only paths whose number of directory levels matches the number of columns
#'   in the corresponding register are included.
#' - Underscores in folder names are converted to spaces, and the first letter
#'   is capitalized for display purposes.
#'
#' These variables are used by both the UI and server modules for populating
#' dropdowns, tables, and for locating species-specific data files.
#'
#' @export
#' @examples
#' \dontrun{
#' # Load global data for the app
#' source("global.R")
#' }

######################
##                  ##
##  Animal Species  ##
##                  ##
######################

library(jsonlite)

# Initialize an empty data frame to register animal species and their taxonomy
animal_species_register <- data.frame(
    Phylum = character(0),
    Class = character(0), 
    Order = character(0), 
    Family = character(0),  
    Genus = character(0), 
    AnimalSpecies = character(0),   
    stringsAsFactors = FALSE 
)

# List all directories under 'data/parametrisation/animalia'
animal_species_paths <- list.dirs(file.path("data", "parametrisation", "animalia"), recursive = FALSE, full.names = TRUE)

# Initialize list to store mapping from species name to path
animal_species_paths_list <- list()

# Populate the animal species register and paths list
for(path in animal_species_paths) {
    info_file <- file.path(path, "taxonomy_info.json")

    if (!file.exists(info_file)) {
        stop(paste0("Error: The taxonomy info file '", info_file, "' could not be found."))
    }

    info <- fromJSON(info_file)

    new_row <- as.data.frame(info, stringsAsFactors = FALSE)

    cols_ordered <- colnames(animal_species_register)
    new_row <- new_row[, cols_ordered]

    animal_species_register <- rbind(animal_species_register, new_row)

    animal_species_paths_list[[new_row$AnimalSpecies]] <- path
}

########################
##                    ##
##  Resource Species  ##
##                    ##
########################

# Initialize an empty data frame to register resource species
resource_species_register <- data.frame(
    World = character(0),
    ResourceSpecies = character(0),   
    stringsAsFactors = FALSE 
)

# List all directories under 'data/parametrisation/resource'
resource_paths <- list.dirs(file.path("data", "parametrisation", "resource"), recursive = TRUE, full.names = FALSE)

# Filter paths to include only those with the correct number of levels
resource_species_paths <- unlist(lapply(resource_paths, function(path) { 
    path_list <- unlist(strsplit(path, "/"))

    if(length(path_list) == length(colnames(resource_species_register))) {
        return(path)
    }
}))

# Initialize list to store mapping from resource species name to path
resource_species_paths_list <- list()

# Populate the resource species register and paths list
for(path in resource_species_paths) {
    column_values <- lapply(
        unlist(strsplit(path, "/")), 
        function(element) { 
            modified_str <- gsub("_", " ", element)
            # Capitalize first letter, keep the rest lowercase
            modified_str <- paste(toupper(substring(modified_str, 1, 1)), 
                                    substring(modified_str, 2), sep = "")
            return(modified_str)
        }
    )
    new_row <- setNames(as.list(column_values), colnames(resource_species_register))
    resource_species_register <- rbind(resource_species_register, new_row)
    resource_species_paths_list[[new_row$ResourceSpecies]] <- path
}
