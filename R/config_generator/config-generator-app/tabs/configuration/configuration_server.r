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
#'    - Processes selected animal species, running species scripts and generating JSON files.
#'    - Processes selected resource species and generates JSON files.
#'    - Generates CSV files for ontogenetic links with normalized preferences.
#'    - Optionally generates bibliographies using `bibgen2`.
#'    - Shows success or error modal dialogs.
#' 3. `generate_animal_species_json` and `generate_resource_species_json`:
#'    - Helper functions to build structured JSON files from the environment variables.
#' 4. `generate_ontogenetic_links_csv`:
#'    - Generates ontogenetic link CSV files with scaled preference values.
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
source(file.path(getwd(), "data", "parametrisation", "utilities", "codes", "functions", "run_animal_species_info_script.r"))
source(file.path(getwd(), "data", "parametrisation", "utilities", "codes", "functions", "run_resource_species_info_script.r"))
source(file.path(getwd(), "data", "parametrisation", "utilities", "codes", "functions", "bibliography_generator", "bibgen2.r"))

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
# ----------------------------------------------------------------------
generate_animal_species_json = function(environment, output_path, name, version) {
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
    write(toJSON(json_data, auto_unbox = TRUE, pretty = TRUE, null = "null", digits = NA), file = file.path(output_path, paste0(name, ".json")))
}

# ----------------------------------------------------------------------
# Generate JSON for resource species
# ----------------------------------------------------------------------
generate_resource_species_json = function(environment, output_path, name, version) {
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
    write(toJSON(json_data, auto_unbox = TRUE, pretty = TRUE, null = "null", digits = NA), file = file.path(output_path, paste0(name, ".json")))
}

# ----------------------------------------------------------------------
# Generate CSV files for ontogenetic links
# ----------------------------------------------------------------------
generate_ontogenetic_links_csv <- function(ontogenetic_links_info, ontogenetic_links_column_header, ontogenetic_links_row_header, output_path) {
    preferences_matrix <- matrix(0, nrow = length(ontogenetic_links_row_header), ncol = length(ontogenetic_links_column_header),
                    dimnames = list(ontogenetic_links_row_header, ontogenetic_links_column_header))
    
    profitability_matrix <- preferences_matrix

    for (column in ontogenetic_links_column_header) {
        fields <- strsplit(column, "\\$")[[1]]
        predator_name <- fields[[1]]
        predator_instar <- fields[[2]]
        column_info <- subset(ontogenetic_links_info, Species_name_predator == predator_name & Instar_predator == predator_instar)
    
        available_prey <- 0
        if(nrow(column_info) > 0) {
            for (i in 1:nrow(column_info)) {
                prey <- paste(column_info$Species_name_prey[i], column_info$Instar_prey[i], sep = "$")

                if (prey %in% ontogenetic_links_row_header) {
                    preferences_matrix[prey, column] <- column_info$Preferences[i]
                    profitability_matrix[prey, column] <- column_info$Profitability[i]

                    available_prey <- available_prey + 1
                }
            }
        }
        if(available_prey == 0) {
            stop(paste("Error: No prey available for predator", predator_name, "and instar", predator_instar))
        }
    }

    # Scale preference columns
    preferences_matrix_scaled <- apply(preferences_matrix, 2, function(col) {
        if (sum(col) == 0) {
            return(rep(0, length(col))) 
        } else {
            return(col / sum(col))
        }
    })
    preferences_matrix_scaled <- matrix(preferences_matrix_scaled, nrow = nrow(preferences_matrix), dimnames = dimnames(preferences_matrix))

    preferences_df <- as.data.frame(preferences_matrix_scaled)
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
        library(jsonlite)
        
        dir.create(file.path(save_directory_path, config_name), recursive = TRUE, showWarnings = FALSE)

        # Initialize ontogenetic links info
        ontogenetic_links_info <- data.frame(
            Species_name_predator = character(0),
            Species_name_prey = character(0),
            Instar_predator = integer(0),
            Instar_prey = integer(0), 
            Preferences = numeric(0), 
            Profitability = numeric(0),  
            stringsAsFactors = FALSE 
        )
        ontogenetic_links_column_header <- c()


        # --- Animal Species ---
        animal_species_folder <- file.path(save_directory_path, config_name, "species")
        dir.create(animal_species_folder, recursive = TRUE, showWarnings = FALSE)
        animal_species_env <- c()

        for(name in animal_species) {
            env <- new.env()
            force_model <- list()
            for(temp in names(species_growth_curve_plots[[name]])) {
                growth_curve_selected <- input[[paste0("growth_curve_selected_", name, "_", temp)]]
                if(growth_curve_selected == "") {
                    stop(paste0("Error: No selected growth curve in the species \"", name, "\" at temperature \"", temp, "\""))
                }
                force_model[[temp]] <- growth_curve_selected
            }
            env$app <- list(
                force_model = force_model
            )
            env <- run_animal_species_info_script(name, env)
            generate_animal_species_json(env, animal_species_folder, name, version)
            instars <- get("json.individualsPerInstar", envir = env)
            ontogenetic_links_column_header <- c(ontogenetic_links_column_header, sapply(seq_along(instars), function(instar) { paste(get("json.name", envir = env), instar, sep = "$") }))
            new_row_ontogenetic_links_info <- get("json.ontogenetic_links", envir = env)
            new_row_ontogenetic_links_info$Species_name_predator <- get("json.name", envir = env)
            ontogenetic_links_info <- rbind(ontogenetic_links_info, new_row_ontogenetic_links_info)
            animal_species_env <- c(animal_species_env, env)
        }

        if(bibliography) {
            bibgen2(animal_species_env, file.path(save_directory_path, config_name, "species"))
        }

        ontogenetic_links_row_header <- ontogenetic_links_column_header

        # --- Resource Species ---
        resource_species_folder <- file.path(save_directory_path, config_name, "resource")
        dir.create(resource_species_folder, recursive = TRUE, showWarnings = FALSE)
        resource_species_env <- c()

        for(name in resource_species) {
            env <- new.env()
            env <- run_resource_species_info_script(name, env)
            generate_resource_species_json(env, resource_species_folder, name, version)
            ontogenetic_links_row_header <- c(ontogenetic_links_row_header, paste(get("json.name", envir = env), "1", sep = "$"))
            resource_species_env <- c(resource_species_env, env)
        }

        # --- Ontogenetic Links ---
        generate_ontogenetic_links_csv(ontogenetic_links_info, ontogenetic_links_column_header, ontogenetic_links_row_header, animal_species_folder)

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
configuration_server <- function(input, output, session) {
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
        save_path <- parseDirPath(volumes, input$save_directory_chooser)

        config_generator(input, input$config_name, input$version, save_path, FALSE, selected_animal_species$values, selected_resource_species$values)
    })
    
    # Observe "Generate Configuration with Bibliography" button
    observeEvent(input$generate_config_bib, {
        save_path <- parseDirPath(volumes, input$save_directory_chooser)

        config_generator(input, input$config_name, input$version, save_path, TRUE, selected_animal_species$values, selected_resource_species$values)
    })
}
