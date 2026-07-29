#' Configuration Setup Server Logic for the Configuration Generator App
#'
#' This script defines the **server-side logic** for managing the
#' "Configuration Setup" tab in the Shiny Configuration Generator application.
#' It handles directory selection, configuration file generation, animal and
#' resource species processing, ontogenetic links, and optional bibliography.
#'
#' @details
#' The server module includes:
#' 
#' 1. Directory selection using `shinyDirChoose` and display of the chosen path.
#' 2. The `config_generator` function:
#'    - Creates the configuration folder structure.
#'    - Exports selected animal species directly from the in-memory editor state (`species_data`) without re-running species scripts.
#'    - Processes selected resource species and generates JSON files.
#'    - Generates CSV files for ontogenetic links.
#'    - Optionally generates bibliographies using `bibgen2`.
#'    - Validates that all edited animal species are saved before exporting.
#'    - Shows success or error modal dialogs.
#' 3. Helpers to transform editor/runtime state into export artifacts (JSON + CSV).
#' 4. `generate_ontogenetic_links_csv`:
#'    - Generates ontogenetic link CSV files.
#' 5. `assign_nested`:
#'    - Utility function to assign values to nested lists (used for JSON construction).
#'
#' @import shiny
#' @import shinyFiles
#' @importFrom fs path_home
#' @importFrom jsonlite toJSON
#' @export
#'
#' @examples
#' \dontrun{
#' # Include this server module in the main server function:
#' configuration_server(input, output, session)
#' }

library(shinyFiles)
library(fs)

# Source helper scripts for species info and bibliography generation
source(file.path(getwd(), "data", "parametrisation", "utilities", "codes", "functions", "run_resource_species_info_script.r"))
source(file.path(getwd(), "data", "parametrisation", "utilities", "codes", "functions", "bibliography_generator", "bibgen2.r"))


# Detects wrapped editor entries `list(value=..., derived=...)`.
is_parameter_entry_config <- function(value) {
    is.list(value) &&
        !is.null(names(value)) &&
        identical(sort(names(value)), c("derived", "value"))
}


# Recursively unwraps editor metadata to obtain plain values for JSON export.
unwrap_parameter_entries_recursive <- function(value) {
    if (is_parameter_entry_config(value)) {
        return(unwrap_parameter_entries_recursive(value$value))
    }

    if (is.list(value) && !is.data.frame(value)) {
        return(lapply(value, unwrap_parameter_entries_recursive))
    }

    value
}


# Builds the canonical JSON payload for one animal species from current tab state.
build_animal_species_json_data <- function(species_name, species_state, version) {
    animal_values <- unwrap_parameter_entries_recursive(species_state$params)

    list(
        animal = animal_values,
        version = version
    )
}


# Writes species JSON using `animal.name` as filename when available.
write_animal_species_json_from_state <- function(species_name, species_state, output_path, version) {
    json_data <- build_animal_species_json_data(species_name, species_state, version)

    file_name <- species_name
    if (!is.null(json_data$animal$name) && nzchar(as.character(json_data$animal$name))) {
        file_name <- as.character(json_data$animal$name)
    }

    write(
        toJSON(json_data, auto_unbox = TRUE, pretty = TRUE, null = "null", digits = NA),
        file = file.path(output_path, paste0(file_name, ".json"))
    )

    invisible(json_data)
}


# Creates a lightweight environment with only the fields needed by link CSV generation.
build_animal_species_env_for_links <- function(species_name, species_state) {
    json_data <- build_animal_species_json_data(species_name, species_state, version = NULL)
    env <- new.env()

    json_name <- species_name
    if (!is.null(json_data$animal$name) && nzchar(as.character(json_data$animal$name))) {
        json_name <- as.character(json_data$animal$name)
    }

    assign("json.name", json_name, envir = env)
    assign("json.individualsPerInstar", if (is.null(json_data$animal$individualsPerInstar)) list() else json_data$animal$individualsPerInstar, envir = env)

    if (!is.null(species_state$ontogenetic_links)) {
        assign("json.ontogenetic_links", species_state$ontogenetic_links, envir = env)
    }

    env
}


# Extracts bibliography metadata persisted in the species editor state.
build_animal_species_bibliography_record <- function(species_name, species_state) {
    bibliography_data <- species_state$bibliography_data

    entries <- list()
    script_path <- NULL
    ris_dir <- NULL

    if (!is.null(bibliography_data)) {
        entries <- if (is.null(bibliography_data$entries)) list() else bibliography_data$entries
        script_path <- bibliography_data$script_path
        ris_dir <- bibliography_data$ris_dir
    }

    list(
        species_name = species_name,
        bibliography_entries = entries,
        script_path = script_path,
        ris_dir = ris_dir
    )
}


# Guards export: selected species must exist in-memory and have no unsaved changes.
validate_animal_species_saved <- function(animal_species) {
    if (length(animal_species) == 0) {
        return(list(ok = TRUE, message = NULL))
    }

    if (!exists("species_data", inherits = TRUE) || !exists("unsaved_changes", inherits = TRUE)) {
        return(list(ok = FALSE, message = "Animal Species tab state is unavailable. Please open Animal Species and save all species changes before generating the configuration."))
    }

    missing_species <- animal_species[sapply(animal_species, function(name) is.null(species_data[[name]]))]
    if (length(missing_species) > 0) {
        return(list(
            ok = FALSE,
            message = paste0(
                "The following species do not have loaded parameter data: ",
                paste(missing_species, collapse = ", "),
                ". Please open each species in Animal Species and save the changes before generating the configuration."
            )
        ))
    }

    unsaved_species <- animal_species[sapply(animal_species, function(name) isTRUE(unsaved_changes[[name]]))]
    if (length(unsaved_species) > 0) {
        return(list(
            ok = FALSE,
            message = paste0(
                "There are unsaved changes in: ",
                paste(unsaved_species, collapse = ", "),
                ". Please save all species changes before generating the configuration."
            )
        ))
    }

    list(ok = TRUE, message = NULL)
}

# ----------------------------------------------------------------------
# Helper function to assign values to a nested list by parts of a path
# ----------------------------------------------------------------------
assign_nested <- function(lst, parts, value) {
    if (length(parts) == 1) {
        lst[[parts]] <- value
    } else {
        if (is.null(lst[[parts[1]]])) lst[[parts[1]]] <- list()
        lst[[parts[1]]] <- assign_nested(lst[[parts[1]]], parts[-1], value)
    }
    lst
}

# ----------------------------------------------------------------------
# Generate JSON for animal species based on environment variables
# Legacy helper kept for compatibility with env-based exporters.
# ----------------------------------------------------------------------
generate_animal_species_json = function(environment, output_path, version) {
    add_parameters <- function(environment, json_data, parameters_to_add) {
        for(parameter in parameters_to_add) {
            json_path <- paste(c("animal", unlist(strsplit(parameter, "\\."))[-1]), collapse = "$")

            eval(parse(text = paste0("json_data$", json_path, " <- get(parameter, envir = environment)")))
        }

        return(json_data)
    }

    all_parameters <- grep("^json\\.", ls(envir = environment), value = TRUE)
    
    traits_parameters <- all_parameters[grep("^json\\.traits\\.", all_parameters)]
    other_parameters <- setdiff(all_parameters, c(traits_parameters, "json.ontogenetic_links"))

    json_data <- list(
        animal = list(),
        version = version
    )
    json_data <- add_parameters(environment, json_data, other_parameters)

    traits_parameters_filtered <- grep("ranges\\.min$", traits_parameters, value = TRUE)
    traits_name <- unique(unlist(sapply(traits_parameters_filtered, function(parameter) { 
        path <- strsplit(parameter, "\\.")[[1]]

        return(paste(path[3:(length(path)-2)], collapse = "."))
    })))

    for(trait in traits_name) {
        path <- strsplit(trait, "\\.")[[1]]

        ranges_min <- get(paste(c("json.traits", trait, "ranges.min"), collapse = "."), envir = environment)
        ranges_max <- get(paste(c("json.traits", trait, "ranges.max"), collapse = "."), envir = environment)
        
        # Define trait as species-level or individual-level
        if(ranges_min == ranges_max) {
            json_data$animal$genetics$traits$definition <- assign_nested(
                json_data$animal$genetics$traits$definition,
                strsplit(trait, "\\.")[[1]],
                list(
                    definitionType = "SpeciesLevel",
                    individualLevelParams = list(
                        limits = list(
                            max = NULL,
                            min = NULL
                        ),
                        ranges = list(
                            max = NULL,
                            min = NULL
                        ),
                        restrictValue = NULL
                    ),
                    speciesLevelParams = list(
                        value = ranges_min
                    )
                )
            )
        }
        else {
            json_data$animal$genetics$traits$definition <- assign_nested(
                json_data$animal$genetics$traits$definition,
                strsplit(trait, "\\.")[[1]],
                list(
                    definitionType = "IndividualLevel",
                    individualLevelParams = list(
                        limits = list(
                            max = get(paste(c("json.traits", trait, "limits.max"), collapse = "."), envir = environment),
                            min = get(paste(c("json.traits", trait, "limits.min"), collapse = "."), envir = environment)
                        ),
                        ranges = list(
                            max = ranges_max,
                            min = ranges_min
                        ),
                        restrictValue = get(paste(c("json.traits", trait, "restrictValue"), collapse = "."), envir = environment)
                    ),
                    speciesLevelParams = list(value = NULL)
                )
            )
        }

        # Handle temperature-dependent traits
        if(trait != "base.energy_tank") {
            json_data$animal$genetics$traits$definition <- assign_nested(
                json_data$animal$genetics$traits$definition, 
                strsplit(paste(trait, "temperature.dependent", sep = "."), "\\.")[[1]], 
                get(paste(c("json.traits", trait, "temperature.dependent"), collapse = "."), envir = environment)
            )

            if(trait == "base.lengthAtMaturation") {
                json_data$animal$genetics$traits$definition <- assign_nested(
                    json_data$animal$genetics$traits$definition, 
                    strsplit(paste(trait, "temperature.tempSizeRuleVector", sep = "."), "\\.")[[1]], 
                    get(paste(c("json.traits", trait, "temperature.tempSizeRuleVector"), collapse = "."), envir = environment)
                )
            } else {
                for(element in c("activationEnergy", "energyDecay", "temperatureOptimal", "temperatureRef")) {
                    value <- get(paste(c("json.traits", trait, "temperature", element), collapse = "."), envir = environment)
                    
                    json_data$animal$genetics$traits$definition <- assign_nested(
                        json_data$animal$genetics$traits$definition, 
                        strsplit(paste(trait, "temperature", element, sep = "."), "\\.")[[1]], 
                        list(
                            definitionType = "SpeciesLevel",
                            individualLevelParams = list(
                                limits = list(
                                    max = NULL,
                                    min = NULL
                                ),
                                ranges = list(
                                    max = NULL,
                                    min = NULL
                                ),
                                restrictValue = NULL
                            ),
                            speciesLevelParams = list(
                                value = value
                            )
                        )
                    )
                }
            }
        }
    }

    json_data$animal$genetics$traits$individualLevelTraitsOrder <- get("json.traits.individualLevelTraitsOrder", envir = environment)

    # Save JSON to file
    name <- get("json.name", envir = environment)
    write(toJSON(json_data, auto_unbox = TRUE, pretty = TRUE, null = "null", digits = NA), file = file.path(output_path, paste0(name, ".json")))
}

# ----------------------------------------------------------------------
# Generate JSON for resource species
# ----------------------------------------------------------------------
generate_resource_species_json = function(environment, output_path, version) {
    add_parameters <- function(environment, json_data, parameters_to_add) {
        for(parameter in parameters_to_add) {
            json_path <- paste(c("resource", unlist(strsplit(parameter, "\\."))[-1]), collapse = "$")
            eval(parse(text = paste0("json_data$", json_path, " <- get(parameter, envir = environment)")))
        }
        return(json_data)
    }

    all_parameters <- grep("^json\\.", ls(envir = environment), value = TRUE)
    json_data <- list(
        resource = list(),
        version = version
    )
    json_data <- add_parameters(environment, json_data, all_parameters)

    # Save JSON to file
    name <- get("json.name", envir = environment)
    write(toJSON(json_data, auto_unbox = TRUE, pretty = TRUE, null = "null", digits = NA), file = file.path(output_path, paste0(name, ".json")))
}

# ----------------------------------------------------------------------
# Generate CSV files for ontogenetic links
# ----------------------------------------------------------------------
generate_ontogenetic_links_csv <- function(animal_species_env, resource_species_env, output_path) {

    # --- Columns ---

    ontogenetic_links_column_header <- unlist(lapply(animal_species_env, function(env) {
        instars <- seq_along(get("json.individualsPerInstar", envir = env))
        sapply(instars, function(instar) {
            paste(get("json.name", envir = env), instar, sep = "$")
        })
    }), use.names = FALSE)

    # --- Rows ---

    ontogenetic_links_row_header <- c(
        ontogenetic_links_column_header,
        unlist(lapply(resource_species_env, function(env) {
            paste(get("json.name", envir = env), "1", sep = "$")
        }), use.names = FALSE)
    )

    # --- Links ---

    preferences_matrix <- matrix(0.0, nrow = length(ontogenetic_links_row_header), ncol = length(ontogenetic_links_column_header),
                    dimnames = list(ontogenetic_links_row_header, ontogenetic_links_column_header))
    
    profitability_matrix <- preferences_matrix


    for (species_env in animal_species_env) {
        
        # Obtener el nombre de la especie actual
        species_name <- get("json.name", envir = species_env)
        
        # Comprobar si tiene definidos enlaces ontogenéticos
        if (exists("json.ontogenetic_links", envir = species_env)) {
            ontogenetic_links <- get("json.ontogenetic_links", envir = species_env)
            
            # 3. Iterar por los instares de esta especie (depredador)
            for (predator_instar in names(ontogenetic_links)) {
                
                # Construir el nombre de la columna (Ej. "Amblyseius_swirskii$1")
                col_name <- paste0(species_name, "$", predator_instar)
                
                # Asegurarnos de que este instar está activo en la simulación actual
                if (col_name %in% ontogenetic_links_column_header) {
                    prey_list <- ontogenetic_links[[predator_instar]]
                    
                    # 4. Iterar sobre las especies presa con las que tiene enlace
                    for (prey_name in names(prey_list)) {
                        links <- prey_list[[prey_name]]
                        
                        # 5. Iterar sobre las reglas/instares de la presa
                        for (link in links) {
                            pref_val <- link$preference
                            prof_val <- link$profitability
                            t_instars <- link$target_instars
                            
                            # Determinar los nombres de las filas objetivo (presas)
                            if (is.character(t_instars) && length(t_instars) == 1 && t_instars == "all") {
                                # Si aplica a todos, buscar todas las filas que empiecen por el nombre de la presa
                                matching_rows <- grep(paste0("^", prey_name, "\\$"), ontogenetic_links_row_header, value = TRUE)
                            } else {
                                # Si son instares específicos, construir los nombres exactos y verificar que existen
                                expected_rows <- paste0(prey_name, "$", t_instars)
                                matching_rows <- intersect(expected_rows, ontogenetic_links_row_header)
                            }
                            
                            # 6. Fijar la preferencia y profitability en las matrices
                            for (row_match in matching_rows) {
                                preferences_matrix[row_match, col_name] <- pref_val
                                profitability_matrix[row_match, col_name] <- prof_val
                            }
                        }
                    }
                    
                    # 7. Normalize each predator column so preferences become a probability vector.
                    col_sum <- sum(preferences_matrix[, col_name])
                    if (col_sum > 0) {
                        preferences_matrix[, col_name] <- preferences_matrix[, col_name] / col_sum
                    }
                }
            }
        }
    }


    preferences_df <- as.data.frame(preferences_matrix)
    profitability_df <- as.data.frame(profitability_matrix)

    preferences_df <- cbind(RowHeader = rownames(preferences_df), preferences_df)
    colnames(preferences_df)[1] <- "prey\\predator"

    profitability_df <- cbind(RowHeader = rownames(profitability_df), profitability_df)
    colnames(profitability_df)[1] <- "prey\\predator"

    write.csv(preferences_df, file = file.path(output_path, "ontogeneticLinksPreference.csv"), row.names = FALSE)
    write.csv(profitability_df, file = file.path(output_path, "ontogeneticLinksProfitability.csv"), row.names = FALSE)
}

# ----------------------------------------------------------------------
# Main configuration generator function
# ----------------------------------------------------------------------
config_generator <- function(input, config_name, version, save_directory_path, bibliography, animal_species, resource_species) {
    tryCatch({
        # Re-validate right before exporting to avoid race conditions across tabs.
        validation <- validate_animal_species_saved(animal_species)
        if (!isTRUE(validation$ok)) {
            stop(validation$message)
        }

        dir.create(file.path(save_directory_path, config_name), recursive = TRUE, showWarnings = FALSE)


        # --- Animal Species ---
        # Export from `species_data` (UI state), preserving user-edited values.
        animal_species_folder <- file.path(save_directory_path, config_name, "species")
        dir.create(animal_species_folder, recursive = TRUE, showWarnings = FALSE)
        animal_species_env <- vector("list", length(animal_species))
        animal_species_bibliography <- vector("list", length(animal_species))

        for(i in seq_along(animal_species)) {
            name <- animal_species[[i]]
            current_species_state <- species_data[[name]]
            write_animal_species_json_from_state(name, current_species_state, animal_species_folder, version)
            animal_species_env[[i]] <- build_animal_species_env_for_links(name, current_species_state)
            animal_species_bibliography[[i]] <- build_animal_species_bibliography_record(name, current_species_state)
        }

        if (isTRUE(bibliography)) {
            bibgen2(animal_species_bibliography, file.path(save_directory_path, config_name, "species"))
        }

        # --- Resource Species ---
        # Resource species are still loaded from their scripts at export time.
        resource_species_folder <- file.path(save_directory_path, config_name, "resource")
        dir.create(resource_species_folder, recursive = TRUE, showWarnings = FALSE)
        resource_species_env <- vector("list", length(resource_species))

        for(i in seq_along(resource_species)) {
            name <- resource_species[[i]]
            env <- new.env()
            env <- run_resource_species_info_script(name, env)
            generate_resource_species_json(env, resource_species_folder, version)
            resource_species_env[[i]] <- env
        }

        # --- Ontogenetic Links ---
        # Build interaction matrices from exported animal/resource runtime metadata.
        generate_ontogenetic_links_csv(animal_species_env, resource_species_env, animal_species_folder)

        # Show success modal
        if(bibliography) {
            showModal(modalDialog(
                title = "Configuration with Bibliography Generated",
                paste0("Configuration \"", config_name, "\" with bibliography generated successfully at ", save_directory_path),
                easyClose = TRUE,
                footer = NULL
            ))
        } else {
            showModal(modalDialog(
                title = "Configuration Generated",
                paste0("Configuration \"", config_name, "\" generated successfully at ", save_directory_path),
                easyClose = TRUE,
                footer = NULL
            ))
        }

    }, error = function(e) {
        showModal(modalDialog(
            title = "Error",
            e$message,
            easyClose = TRUE,
            footer = NULL
        ))
        unlink(file.path(save_directory_path, config_name), recursive = TRUE)
    })
}

# ----------------------------------------------------------------------
# Server module for Configuration Setup tab
# ----------------------------------------------------------------------
active_schema_version <- shiny::reactiveVal(NULL)

configuration_server <- function(input, output, session) {

    schema_dir_path <- Sys.getenv("APP_SCHEMA_DIR_PATH")

    if (dir.exists(schema_dir_path)) {
        # Obtener solo los nombres de los subdirectorios, sin la ruta completa
        available_versions <- list.dirs(schema_dir_path, full.names = FALSE, recursive = FALSE)
        
        # Filtrar estrictamente para obtener solo los directorios con el formato YYYY.MM.DD
        available_versions <- available_versions[grepl("^\\d{4}\\.\\d{2}\\.\\d{2}$", available_versions)]
        
        # ¡NUEVO!: Ordenar de más reciente a más antigua (descendente)
        available_versions <- sort(available_versions, decreasing = TRUE)

        # Actualizar el desplegable en la interfaz
        updateSelectInput(session, "version", choices = available_versions)
    }

    # Other tabs consume this reactive value instead of hard-coding a schema
    # version. Changing it invalidates their schema-backed state.
    observeEvent(input$version, {
        active_schema_version(input$version)
    }, ignoreInit = FALSE)

    output$schema_version_title <- renderUI({
        version <- active_schema_version()
        if (is.null(version) || identical(version, "")) {
            return(NULL)
        }

        tags$span(
            style = "font-size: 0.85em; color: #666;",
            paste0("Schema: ", version)
        )
    })

    volumes <- c(Home = fs::path_home(), getVolumes()())

    # Enable directory chooser
    shinyDirChoose(input, "save_directory_chooser", roots = volumes, session = session, restrictions = system.file(package = "base"), allowDirCreate = FALSE)

    # Display selected directory path
    output$save_directory_selected <- renderPrint({
        if (is.integer(input$save_directory_chooser)) {
            cat("No directory has been selected.")
        } else {
            parseDirPath(volumes, input$save_directory_chooser)
        }
    })

    # Observe "Generate Configuration" button
    observeEvent(input$generate_config, {
        validation <- validate_animal_species_saved(selected_animal_species$values)
        if (!isTRUE(validation$ok)) {
            showModal(modalDialog(
                title = "Unsaved species changes",
                validation$message,
                easyClose = TRUE,
                footer = NULL
            ))
            return(invisible(NULL))
        }

        save_path <- parseDirPath(volumes, input$save_directory_chooser)

        config_generator(input, input$config_name, input$version, save_path, FALSE, selected_animal_species$values, selected_resource_species$values)
    })
    
    # Observe "Generate Configuration with Bibliography" button
    observeEvent(input$generate_config_bib, {
        validation <- validate_animal_species_saved(selected_animal_species$values)
        if (!isTRUE(validation$ok)) {
            showModal(modalDialog(
                title = "Unsaved species changes",
                validation$message,
                easyClose = TRUE,
                footer = NULL
            ))
            return(invisible(NULL))
        }

        save_path <- parseDirPath(volumes, input$save_directory_chooser)

        config_generator(input, input$config_name, input$version, save_path, TRUE, selected_animal_species$values, selected_resource_species$values)
    })
}
