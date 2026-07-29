################################################################################
# Function: bibgen2
# Programmed by: David P. Quevedo.
# Last update: 2025-18-01
# Version: 2.0
#
# Description:
#   This function automatically generates bibliography documents (in RIS format)
#   for one or more species from preloaded bibliography metadata.
#
#   The function no longer sources species scripts. Instead, each species item
#   is expected to include bibliography entries already extracted from the
#   species loading pipeline (e.g., objects named ID_* stored in app state).
#
#   Each species item can include:
#     - species_name: species identifier used in output filenames/logging.
#     - bibliography_entries: named list of bibliography objects.
#     - ris_dir: (optional) path to RIS files.
#     - script_path: (optional) base path used to infer ris_dir as script_path/ris.
#
#   The function operates as follows:
#     1. It processes each species item in the provided list.
#
#     2. For each species, it:
#         a. Reads bibliography objects from bibliography_entries.
#            Objects should contain at least "Variable" and "DataTreatment".
#            DOI*/ISSN* fields are used to locate RIS files.
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
#     A list of species metadata records. Each record should include:
#       - species_name: (string) The species name used in reporting/output naming.
#       - bibliography_entries: (named list) Bibliography objects (typically ID_*).
#       - ris_dir: (optional string) Directory containing RIS files.
#       - script_path: (optional string) Used to infer ris_dir as script_path/ris.
#
#   path_to_output:
#     (string) The directory where the generated RIS documents (both per species and
#     the combined document) will be saved. If the directory does not exist, it will be created.
#
# Dependencies & Assumptions:
#   - The provided paths are assumed to be correctly formatted.
#   - Bibliography entries are preloaded; this function does not source any scripts.
#   - Bibliography objects should contain at least "Variable" and "DataTreatment" fields.
#   - RIS files are expected to be named as <DOI_or_ISSN>.ris.
#
# Example:
#   # Define species bibliography metadata already extracted from app state:
#   all_species_env <- list(
#       list(
#           species_name = "amblyseius_swirskii",
#           script_path = "/path/to/species1",
#           bibliography_entries = list(
#               ID_EXAMPLE_1 = list(
#                   Variable = "tempFromLab",
#                   DataTreatment = "Measured from literature",
#                   DOI1 = "10.1234/example"
#               )
#           )
#       )
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
    species_name <- species_info$species_name

    # Retrieve the RIS directory if provided; otherwise, assume a subdirectory "ris" within script_path.
    ris_dir <- species_info$ris_dir
    if (is.null(ris_dir)) {
      ris_dir <- file.path(species_info$script_path, "ris")
    }
    
    bibliography_entries <- species_info$bibliography_entries
    
    cat("Processing species:", species_name, "\n")
    
    species_df_list <- list()
    if (length(bibliography_entries) == 0) {
      cat("  No bibliography (ID_) objects found in species:", species_name, "\n")
      next
    }

    for (id_obj in names(bibliography_entries)) {
      obj <- bibliography_entries[[id_obj]]
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
    if (length(doi_issn_columns) == 0) {
      cat("  No DOI/ISSN columns found for species:", species_name, "\n")
      next
    }

    if (is.null(ris_dir) || !dir.exists(ris_dir)) {
      cat("  RIS directory not found for species:", species_name, "\n")
      next
    }

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
      cat("Exported bibliography for species", species_name, "to", species_output_file, "\n")
      
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
