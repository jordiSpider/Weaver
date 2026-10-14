################################################################################
# Function: bibgen2
# Programmed by: David P. Quevedo.
# Last update: 2025-18-01
# Version: 2.0
#
# Description:
#   This function automatically generates bibliography documents (in RIS format)
#   for one or more species from a provided species environment. Each species
#   environment is expected to contain a 'script_path' (the path to the directory
#   where species-specific R scripts are stored) and optionally a 'ris_dir'
#   (the path to the directory containing RIS files). If 'ris_dir' is not provided,
#   it is assumed that the RIS files reside in a subfolder named "ris" within the
#   species directory.
#
#   The function operates as follows:
#     1. It processes each species environment (either a single environment or
#        a list of them). If only one species environment is provided, it is wrapped
#        into a list for uniform processing.
#
#     2. For each species, it:
#         a. Sources all R scripts in the species directory with filenames ending
#            in '_info.r' (these scripts are expected to contain bibliography data).
#         b. Extracts bibliography objects from the sourced scripts. These objects
#            are identified by names beginning with "ID_" and must be lists that include
#            at least the fields "Variable" and "DataTreatment".
#         c. Extracts DOI/ISSN identifiers from the bibliography objects.
#         d. Searches the corresponding RIS directory for RIS files whose names match
#            the DOI/ISSN identifiers.
#
#     3. Output:
#         a. For each species, if RIS files are found, a per-species RIS document is
#            generated and saved in the output directory. The file is named in the
#            format "literature_export_<species_name>_<timestamp>.ris".
#         b. A combined bibliography file that consolidates RIS files from all species is
#            also generated, named "literature_export_all_<timestamp>.ris".
#
# Usage:
#   This function is designed to be embedded within a larger program. It is called
#   only when bibliography generation is required by the program.
#
# Parameters:
#   animal_species_env:
#     A single species environment (as a list with at least a 'script_path') or a
#     list of species environments. Each environment must include:
#       - script_path: (string) The directory containing species-specific scripts.
#       - ris_dir: (optional string) The directory containing RIS files. If omitted,
#                  the function assumes a subfolder "ris" within script_path.
#
#   path_to_output:
#     (string) The directory where the generated RIS documents (both per species and
#     the combined document) will be saved. If the directory does not exist, it will be created.
#
# Dependencies & Assumptions:
#   - The provided paths are assumed to be correctly formatted (no normalization is performed).
#   - Species-specific scripts must follow the naming convention: *_info.r.
#   - Bibliography objects in these scripts should have names starting with "ID_" and must be
#     lists containing at least "Variable" and "DataTreatment" fields.
#
# Example:
#   # Define a single species environment:
#   species_env <- list(
#       script_path = "/path/to/species_directory",
#       ris_dir = "/path/to/species_directory/ris"  # Optional; defaults to script_path/ris if omitted
#   )
#
#   # Or define a list of species environments:
#   all_species_env <- list(
#       species1 = list(script_path = "/path/to/species1", ris_dir = "/path/to/species1/ris"),
#       species2 = list(script_path = "/path/to/species2")  # Uses default ris_dir = "/path/to/species2/ris"
#   )
#
#   # Specify the output path:
#   output_path <- "/path/to/output_directory"
#
#   # Generate the bibliography:
#   bibgen2(all_species_env, output_path)
#
################################################################################

bibgen2<- function(animal_species_env, path_to_output) {
  
  # Create the output directory if it does not exist.
  if (!dir.exists(path_to_output)) {
    dir.create(path_to_output, recursive = TRUE)
  }
  
  combined_ris_files <- c()  # Vector to accumulate RIS file paths from all species
  
  # Process each species environment.
  for (i in seq_along(animal_species_env)) {
    species_info <- animal_species_env[[i]]
    species_name <- basename(animal_species_env[[i]]$script_path)
    
    # Retrieve the RIS directory if provided; otherwise, assume a subdirectory "ris" within script_path.
    ris_dir <- if (!is.null(animal_species_env[[i]]$ris_dir)) animal_species_env[[i]]$ris_dir else file.path(animal_species_env[[i]]$script_path, "ris")
    
    cat("Processing species:", species_name, "\n")
    
    # Extract bibliography information from objects whose names begin with "ID_".
    # Each such object should be a list containing at least "Variable" and "DataTreatment".
    species_df_list <- list()
    id_objects <- ls(envir = animal_species_env[[i]], pattern = "^ID_")
    if (length(id_objects) == 0) {
      cat("  No bibliography (ID_) objects found in species:", species_name, "\n")
      next
    }
    
    for (id_obj in id_objects) {
      obj <- get(id_obj, envir = animal_species_env[[i]])
      if (is.list(obj) && "Variable" %in% names(obj) && "DataTreatment" %in% names(obj)) {
        # Construct a data frame row for this bibliography entry.
        df <- data.frame(
          Species = species_name,
          Directory = ris_dir,
          Variable = obj$Variable,
          DataTreatment = obj$DataTreatment,
          stringsAsFactors = FALSE
        )
        # Include any additional fields present in the bibliography object.
        extra_cols <- setdiff(names(obj), c("Variable", "DataTreatment"))
        for (col in extra_cols) {
          df[[col]] <- obj[[col]]
        }
        species_df_list[[id_obj]] <- df
      }
    }
    
    if (length(species_df_list) == 0) {
      cat("  No valid bibliography info found for species:", species_name, "\n")
      next
    }
    
    # Combine all bibliography entries for the current species into a single data frame.
    species_df <- do.call(plyr::rbind.fill, species_df_list)
    
    # Identify columns that potentially contain DOI or ISSN identifiers.
    doi_issn_columns <- grep("^(DOI|ISSN)", names(species_df), value = TRUE)
    all_dois_issns <- unique(unlist(species_df[doi_issn_columns]))
    all_dois_issns <- all_dois_issns[!is.na(all_dois_issns)]
    
    species_ris_files <- c()  # To accumulate RIS file paths for the current species.
    
    # For each DOI/ISSN value, search for a matching RIS file in the species' RIS directory.
    for (doi in all_dois_issns) {
      # Expected pattern: file name starting with the DOI/ISSN followed by ".ris".
      files <- list.files(ris_dir, pattern = paste0("^", doi, "\\.ris$"), full.names = TRUE)
      if (length(files) > 0) {
        species_ris_files <- c(species_ris_files, files)
      } else {
        cat("  No RIS file found for", doi, "in species:", species_name, "\n")
      }
    }
    
    # If RIS files were located for the current species, generate a per-species RIS document.
    if (length(species_ris_files) > 0) {
      species_ris_content <- unlist(lapply(species_ris_files, readLines))
      species_output_file <- file.path(
        path_to_output,
        paste0("literature_export_", species_name, "_", 
               format(Sys.time(), "%Y-%m-%d_%H-%M-%S"), ".ris")
      )
      writeLines(species_ris_content, species_output_file)
      cat("  Exported bibliography for species", species_name, "to", species_output_file, "\n")
      
      # Accumulate the RIS file paths for creating the combined bibliography.
      combined_ris_files <- c(combined_ris_files, species_ris_files)
    } else {
      cat("  No RIS files found for species:", species_name, "\n")
    }
  }
  
  # After processing all species, generate one combined bibliography file if any RIS files were found.
  if (length(combined_ris_files) > 0) {
    general_content <- unlist(lapply(combined_ris_files, readLines))
    general_output_file <- file.path(
      path_to_output,
      paste0("literature_export_all_", format(Sys.time(), "%Y-%m-%d_%H-%M-%S"), ".ris")
    )
    writeLines(general_content, general_output_file)
    cat("Exported general bibliography to", general_output_file, "\n")
  } else {
    cat("No RIS files found for any species.\n")
  }
}
