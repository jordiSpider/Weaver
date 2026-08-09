#' Animal Species Server Logic
#'
#' This file contains the server-side logic for the "Animal Species" tab in the Weaver Configuration Generator Shiny app.
#' It handles taxonomic filtering, dynamic updating of selectize inputs, growth curve management, and adding/removing species tabs.

library(dplyr)      # For filtering data frames
library(shinyBS)    # For collapsible panels
library(plotly)     # For plotting growth curves interactively
library(listviewer) # Para el visor interactivo de JSON

library(shiny)
library(purrr)

# Source script to obtain animal species information and growth curves
source(file.path(getwd(), "data", "parametrisation", "utilities", "codes", "functions", "run_animal_species_info_script.r"))


#' Filter Animal Species Register
#'
#' Filters the global `animal_species_register` dataframe based on the selected taxonomic inputs.
#'
#' @param input The Shiny input list.
#' @return A filtered data frame containing only rows that match selected taxonomic fields.
get_filtered_animal_species_register <- function(input) {
    df <- animal_species_register

    if (!is.null(input$Phylum) && length(input$Phylum) > 0) {
        df <- df %>% filter(Phylum %in% input$Phylum)
    }
    if (!is.null(input$Class) && length(input$Class) > 0) {
        df <- df %>% filter(Class %in% input$Class)
    }
    if (!is.null(input$Order) && length(input$Order) > 0) {
        df <- df %>% filter(Order %in% input$Order)
    }
    if (!is.null(input$Family) && length(input$Family) > 0) {
        df <- df %>% filter(Family %in% input$Family)
    }
    if (!is.null(input$Genus) && length(input$Genus) > 0) {
        df <- df %>% filter(Genus %in% input$Genus)
    }

    return(df)
}


#' Update All Selectize Inputs Dynamically
#'
#' Updates downstream taxonomic selectize inputs whenever a parent field changes.
#'
#' @param session The Shiny session object.
#' @param input The Shiny input list.
#' @param field_event The name of the field that triggered the update.
update_all_selectize_input_animal_species <- function(session, input, field_event) {
    fields <- colnames(animal_species_register)
    field_event_index <- which(fields == field_event)
    current_input <- reactiveValuesToList(input) 
    filtered_animal_species_register <- get_filtered_animal_species_register(current_input)

    for(index in seq(from = field_event_index+1, to = length(fields))) {
        if(index == length(fields)) {
            # Last field uses selectInput instead of selectizeInput
            filtered_animal_species_register <- get_filtered_animal_species_register(current_input)
            choices <- unique(filtered_animal_species_register[[fields[[index]]]])
            updateSelectInput(session, fields[[index]], choices = choices)
        }
        else {
            selected <- input[[fields[[index]]]]
            choices <- unique(filtered_animal_species_register[[fields[[index]]]])

            if(!all(selected %in% choices)) {
                selected <- NULL
                current_input[[fields[[index]]]] <- NULL
                filtered_animal_species_register <- get_filtered_animal_species_register(current_input)
                choices <- unique(filtered_animal_species_register[[fields[[index]]]])
            }

            updateSelectizeInput(
                session, fields[[index]], 
                choices = choices,
                selected = selected,
                server = TRUE
            )
        }
    }
}


#' Set Nested Value
#' Función recursiva para colocar un valor en la profundidad correcta de una lista
set_nested_value <- function(lst, keys, value) {
    if (length(keys) == 1) {
        lst[[keys[1]]] <- value
        return(lst)
    }
    if (is.null(lst[[keys[1]]])) {
        lst[[keys[1]]] <- list()
    }
    lst[[keys[1]]] <- set_nested_value(lst[[keys[1]]], keys[-1], value)
    return(lst)
}


#' Unflatten List
#' Convierte una lista plana con nombres "a.b.c" en una lista anidada list(a=list(b=list(c)))
unflatten_list <- function(flat_list) {
    res <- list()
    for (n in names(flat_list)) {
        keys <- strsplit(n, "\\.")[[1]]
        res <- set_nested_value(res, keys, flat_list[[n]])
    }
    return(res)
}


# The species scripts expose the contents of the schema's `animal` object, not
# the outer JSON document. The schema is therefore the source of truth for the
# editor's available paths.
load_animal_schema_properties <- function(version) {
    schema_dir_path <- Sys.getenv("APP_SCHEMA_DIR_PATH")
    if (!nzchar(schema_dir_path)) {
        schema_dir_path <- normalizePath(file.path(getwd(), "..", "..", "schema"), mustWork = FALSE)
    }
    schema_file <- file.path(schema_dir_path, version, "species_schema.json")

    if (!file.exists(schema_file)) {
        stop(paste("Species schema not found for version", version))
    }

    schema <- jsonlite::fromJSON(schema_file, simplifyVector = FALSE)
    
    return(schema$properties$animal$properties)
}


#' Transforma patternProperties expandiendo los enums en claves independientes
#' y eliminando el campo enum original.
expand_pattern_properties <- function(pattern_properties) {
    expanded_list <- list()
    
    # Recorrer cada elemento principal dentro de patternProperties
    for (trait_name in names(pattern_properties)) {
        trait_obj <- pattern_properties[[trait_name]]
        
        # Obtener los valores del campo enum
        enum_values <- trait_obj$enum
        
        if (!is.null(enum_values)) {
            # Eliminar el campo enum del objeto
            trait_obj$enum <- NULL
            
            # Crear un elemento en la nueva lista por cada cadena presente en el enum
            for (val in enum_values) {
                expanded_list[[val]] <- trait_obj
            }
        }
    }
    
    return(expanded_list)
}


schema_child_properties <- function(schema_node) {
    # Merge explicit properties with expanded pattern-based properties so the
    # editor can traverse both static and dynamic keys uniformly.
    if (is.null(schema_node$properties)) {
        properties <- list()
    } 
    else {
        properties <- schema_node$properties
    }

    if (!is.null(schema_node$patternProperties)) {
        properties <- c(properties, expand_pattern_properties(schema_node$patternProperties))
    }
    
    properties
}


get_schema_property <- function(schema_properties, node_name) {
    if (!is.null(schema_properties[[node_name]])) {
        return(schema_properties[[node_name]])
    }

    pattern_properties <- attr(schema_properties, "patternProperties")
    if (is.null(pattern_properties)) return(NULL)

    for (pattern_name in names(pattern_properties)) {
        candidate <- pattern_properties[[pattern_name]]
        matches_pattern <- tryCatch(grepl(pattern_name, node_name), error = function(e) FALSE)
        matches_enum <- !is.null(candidate$enum) && node_name %in% unlist(candidate$enum)
        if (matches_pattern || matches_enum) return(candidate)
    }
    NULL
}


# Resolves a dotted parameter path against the schema. Returns NULL when the
# path does not exist in the selected schema version.
get_schema_node <- function(schema_properties, path) {
    node <- list(properties = schema_properties)
    for (part in strsplit(path, "\\.")[[1]]) {
        node <- get_schema_property(schema_child_properties(node), part)
        if (is.null(node)) return(NULL)
    }
    node
}


# Drops script parameters that are not represented by the active schema.
filter_params_to_schema <- function(flat_params, schema_properties) {
    allowed <- vapply(names(flat_params), function(path) {
        !is.null(get_schema_node(schema_properties, path))
    }, logical(1))
    flat_params[allowed]
}


coerce_schema_value <- function(value, schema_node) {
    if (identical(schema_node$type, "integer")) return(as.integer(round(as.numeric(value))))
    if (identical(schema_node$type, "number")) return(as.numeric(value))
    if (identical(schema_node$type, "boolean")) return(isTRUE(value))
    as.character(value)
}


# Parses comma-separated scalar arrays from text inputs and coerces each item
# to the schema type.
parse_primitive_array <- function(value, item_schema) {
    pieces <- trimws(strsplit(ifnull(value, ""), ",", fixed = TRUE)[[1]])
    pieces <- pieces[nzchar(pieces)]
    unlist(lapply(pieces, coerce_schema_value, schema_node = item_schema), use.names = FALSE)
}


schema_default_value <- function(schema_node) {
    if (identical(schema_node$type, "object")) {
        return(lapply(schema_node$properties, schema_default_value))
    }
    if (identical(schema_node$type, "array")) return(list())
    if (identical(schema_node$type, "boolean")) return(FALSE)
    if (!is.null(schema_node$enum)) return(schema_node$enum[[1]])
    if (identical(schema_node$type, "integer")) return(as.integer(ifnull(schema_node$minimum, 0)))
    if (identical(schema_node$type, "number")) return(ifnull(schema_node$minimum, 0))
    ""
}


is_schema_leaf_node <- function(schema_node) {
    !identical(schema_node$type, "object") &&
        !(identical(schema_node$type, "array") && identical(schema_node$items$type, "object"))
}


# Wrapped entries preserve both the value and whether it is derived (read-only).
is_parameter_entry <- function(value) {
    is.list(value) &&
        identical(sort(names(value)), c("derived", "value"))
}


parameter_entry <- function(value, derived = FALSE) {
    list(value = value, derived = isTRUE(derived))
}


parameter_value <- function(value) {
    if (is_parameter_entry(value)) value$value else value
}


parameter_is_derived <- function(value) {
    if (is_parameter_entry(value)) isTRUE(value$derived) else FALSE
}


disable_input_script <- function(input_ids, disabled) {
    if (!isTRUE(disabled) || length(input_ids) == 0) return(NULL)

    commands <- paste(sprintf("$('#%s').prop('disabled', true);", input_ids), collapse = "")
    tags$script(HTML(sprintf("setTimeout(function(){%s}, 0);", commands)))
}


field_title_with_badge <- function(node_name, is_derived) {
    tags$div(
        style = "display: inline-flex; align-items: center; gap: 8px;",
        tags$strong(node_name, style = "color: #e67e22;"),
        if (isTRUE(is_derived)) {
            tags$span(
                "Derived",
                style = "font-size: 10px; font-weight: 700; text-transform: uppercase; letter-spacing: 0.03em; color: #5f3f00; background-color: #fff3cd; border: 1px solid #ffe69c; border-radius: 999px; padding: 2px 8px;"
            )
        }
    )
}


detect_derived_param_paths <- function(species_name) {
    species_path <- animal_species_paths_list[[species_name]]
    if (is.null(species_path)) return(character(0))

    script_dir <- file.path(getwd(), species_path)
    derived_script <- file.path(script_dir, paste0(basename(species_path), "_derived.r"))
    if (!file.exists(derived_script)) return(character(0))

    lines <- readLines(derived_script, warn = FALSE)
    lines <- sub("#.*$", "", lines)

    pattern <- "param\\.([A-Za-z0-9_.]+)\\s*(?:<-|=)"
    matches <- unlist(regmatches(lines, gregexpr(pattern, lines, perl = TRUE)), use.names = FALSE)
    if (length(matches) == 0) return(character(0))

    unique(sub(pattern, "\\1", matches, perl = TRUE))
}


derived_param_paths_cache <- new.env(parent = emptyenv())


get_cached_derived_param_paths <- function(species_name) {
    if (exists(species_name, envir = derived_param_paths_cache, inherits = FALSE)) {
        return(derived_param_paths_cache[[species_name]])
    }

    derived_paths <- detect_derived_param_paths(species_name)
    derived_param_paths_cache[[species_name]] <- derived_paths
    derived_paths
}


# Maps `_derived.r` assignment paths to editor schema paths. Mapping is explicit
# to avoid marking full subtrees as derived via broad prefix matching.
map_derived_paths_to_editor_paths <- function(derived_param_paths, flat_param_names) {
    if (length(derived_param_paths) == 0) return(character(0))

    mapped <- character(0)

    for (path in derived_param_paths) {
        if (path %in% flat_param_names) {
            mapped <- c(mapped, path)
            next
        }

        if (path == "traits.individualLevelTraitsOrder") {
            mapped <- c(mapped, "genetics.traits.individualLevelTraitsOrder")
            next
        }

        trait_match <- regexec("^traits\\.(.+)\\.(ranges|limits)\\.(min|max)$", path)
        trait_groups <- regmatches(path, trait_match)[[1]]
        if (length(trait_groups) > 0) {
            trait <- trait_groups[[2]]
            section <- trait_groups[[3]]
            bound <- trait_groups[[4]]
            mapped <- c(
                mapped,
                paste0("genetics.traits.definition.", trait, ".individualLevelParams.", section, ".", bound)
            )
            next
        }

        restrict_match <- regexec("^traits\\.(.+)\\.restrictValue$", path)
        restrict_groups <- regmatches(path, restrict_match)[[1]]
        if (length(restrict_groups) > 0) {
            trait <- restrict_groups[[2]]
            mapped <- c(
                mapped,
                paste0("genetics.traits.definition.", trait, ".individualLevelParams.restrictValue")
            )
            next
        }

        if (grepl("^traits\\..+\\.temperature\\.dependent$", path)) {
            trait <- sub("^traits\\.(.+)\\.temperature\\.dependent$", "\\1", path)
            mapped <- c(
                mapped,
                paste0("genetics.traits.definition.", trait, ".temperature.dependent")
            )
            next
        }

        if (grepl("^traits\\..+\\.temperature\\.tempSizeRuleVector$", path)) {
            trait <- sub("^traits\\.(.+)\\.temperature\\.tempSizeRuleVector$", "\\1", path)
            mapped <- c(
                mapped,
                paste0("genetics.traits.definition.", trait, ".temperature.tempSizeRuleVector")
            )
            next
        }

        temp_value_match <- regexec("^traits\\.(.+)\\.temperature\\.(activationEnergy|energyDecay|temperatureOptimal|temperatureRef)$", path)
        temp_value_groups <- regmatches(path, temp_value_match)[[1]]
        if (length(temp_value_groups) > 0) {
            trait <- temp_value_groups[[2]]
            element <- temp_value_groups[[3]]
            mapped <- c(
                mapped,
                paste0("genetics.traits.definition.", trait, ".temperature.", element, ".speciesLevelParams.value")
            )
            next
        }
    }

    unique(mapped[mapped %in% flat_param_names])
}


extract_param_plot_entries <- function(env) {
    plot_names <- ls(env, pattern = "^plot\\.")
    if (length(plot_names) == 0) return(list())

    plot_entries <- mget(plot_names, envir = env)
    names(plot_entries) <- sub("^plot\\.", "", names(plot_entries))
    plot_entries
}


extract_bibliography_entries <- function(env) {
    id_names <- ls(env, pattern = "^ID_")
    entries <- list()

    if (length(id_names) > 0) {
        for (id_name in id_names) {
            entries[[id_name]] <- get(id_name, envir = env)
        }
    }

    script_path <- env$script_path
    ris_dir <- if ("ris_dir" %in% ls(env)) env$ris_dir else NULL

    list(
        entries = entries,
        script_path = script_path,
        ris_dir = ris_dir
    )
}


normalize_param_path_for_plot <- function(path) {
    gsub("\\.[0-9]+", "", path)
}


get_plot_entry_for_path <- function(plot_entries, path) {
    if (length(plot_entries) == 0) return(NULL)

    if (!is.null(plot_entries[[path]])) {
        return(plot_entries[[path]])
    }

    normalized_path <- normalize_param_path_for_plot(path)
    plot_entries[[normalized_path]]
}


is_plot_choice_map <- function(value) {
    is.list(value) &&
        !inherits(value, c("plotly", "htmlwidget")) &&
        !is.null(names(value)) &&
        length(value) > 0 &&
        all(nzchar(names(value)))
}


plot_output_id <- function(species_name, path) {
    paste0("plot_", species_input_key(species_name), "_", gsub("\\.", "_", path))
}


wrap_flat_params <- function(flat_params, derived_map) {
    wrapped <- lapply(names(flat_params), function(path) {
        parameter_entry(flat_params[[path]], isTRUE(derived_map[[path]]))
    })
    names(wrapped) <- names(flat_params)
    wrapped
}


unwrap_flat_params <- function(flat_params) {
    values <- lapply(flat_params, parameter_value)
    names(values) <- names(flat_params)
    values
}


extract_script_flat_params <- function(env) {
    # Split generic params and trait-specific params because traits are
    # re-shaped to match the schema-backed editor model.
    param_names <- ls(env, pattern = "^param\\.")
    flat_params <- mget(param_names, envir = env)
    names(flat_params) <- sub("^param\\.", "", names(flat_params))

    trait_mask <- grepl("^traits\\.", names(flat_params))
    traits_parameters <- flat_params[trait_mask]
    flat_params[trait_mask] <- NULL

    list(flat_params = flat_params, traits_parameters = traits_parameters)
}


append_traits_editor_params <- function(flat_params, traits_parameters) {
    if (length(traits_parameters) == 0) {
        return(flat_params)
    }

    number_default <- schema_default_value(list(type = "number"))
    trait_names <- unique(sub("^traits\\.(.*?)\\.ranges\\.min$", "\\1", grep("\\.ranges\\.min$", names(traits_parameters), value = TRUE)))

    for (trait in trait_names) {
        # Convert legacy trait scalar namespace into the schema structure used
        # by the editor: definitionType + individual/species-level branches.
        ranges_min <- traits_parameters[[paste(c("traits", trait, "ranges.min"), collapse = ".")]]
        ranges_max <- traits_parameters[[paste(c("traits", trait, "ranges.max"), collapse = ".")]]

        if (ranges_min == ranges_max) {
            flat_params[[paste(c("genetics.traits.definition", trait, "definitionType"), collapse = ".")]] <- "SpeciesLevel"
            flat_params[[paste(c("genetics.traits.definition", trait, "individualLevelParams.limits.min"), collapse = ".")]] <- number_default
            flat_params[[paste(c("genetics.traits.definition", trait, "individualLevelParams.limits.max"), collapse = ".")]] <- number_default
            flat_params[[paste(c("genetics.traits.definition", trait, "individualLevelParams.ranges.min"), collapse = ".")]] <- number_default
            flat_params[[paste(c("genetics.traits.definition", trait, "individualLevelParams.ranges.max"), collapse = ".")]] <- number_default
            flat_params[[paste(c("genetics.traits.definition", trait, "individualLevelParams.restrictValue"), collapse = ".")]] <- number_default
            flat_params[[paste(c("genetics.traits.definition", trait, "speciesLevelParams.value"), collapse = ".")]] <- ranges_min
        } else {
            limits_min <- traits_parameters[[paste(c("traits", trait, "limits.min"), collapse = ".")]]
            limits_max <- traits_parameters[[paste(c("traits", trait, "limits.max"), collapse = ".")]]
            restrict_value <- traits_parameters[[paste(c("traits", trait, "restrictValue"), collapse = ".")]]

            flat_params[[paste(c("genetics.traits.definition", trait, "definitionType"), collapse = ".")]] <- "IndividualLevel"
            flat_params[[paste(c("genetics.traits.definition", trait, "individualLevelParams.limits.min"), collapse = ".")]] <- limits_min
            flat_params[[paste(c("genetics.traits.definition", trait, "individualLevelParams.limits.max"), collapse = ".")]] <- limits_max
            flat_params[[paste(c("genetics.traits.definition", trait, "individualLevelParams.ranges.min"), collapse = ".")]] <- ranges_min
            flat_params[[paste(c("genetics.traits.definition", trait, "individualLevelParams.ranges.max"), collapse = ".")]] <- ranges_max
            flat_params[[paste(c("genetics.traits.definition", trait, "individualLevelParams.restrictValue"), collapse = ".")]] <- restrict_value
            flat_params[[paste(c("genetics.traits.definition", trait, "speciesLevelParams.value"), collapse = ".")]] <- number_default
        }

        if (trait != "base.energy_tank") {
            flat_params[[paste(c("genetics.traits.definition", trait, "temperature.dependent"), collapse = ".")]] <- traits_parameters[[paste(c("traits", trait, "temperature.dependent"), collapse = ".")]]

            if (trait == "base.lengthAtMaturation") {
                flat_params[[paste(c("genetics.traits.definition", trait, "temperature.tempSizeRuleVector"), collapse = ".")]] <- traits_parameters[[paste(c("traits", trait, "temperature.tempSizeRuleVector"), collapse = ".")]]
            } else {
                for (element in c("activationEnergy", "energyDecay", "temperatureOptimal", "temperatureRef")) {
                    value <- traits_parameters[[paste(c("traits", trait, "temperature", element), collapse = ".")]]

                    flat_params[[paste(c("genetics.traits.definition", trait, "temperature", element, "definitionType"), collapse = ".")]] <- "SpeciesLevel"
                    flat_params[[paste(c("genetics.traits.definition", trait, "temperature", element, "individualLevelParams.limits.min"), collapse = ".")]] <- number_default
                    flat_params[[paste(c("genetics.traits.definition", trait, "temperature", element, "individualLevelParams.limits.max"), collapse = ".")]] <- number_default
                    flat_params[[paste(c("genetics.traits.definition", trait, "temperature", element, "individualLevelParams.ranges.min"), collapse = ".")]] <- number_default
                    flat_params[[paste(c("genetics.traits.definition", trait, "temperature", element, "individualLevelParams.ranges.max"), collapse = ".")]] <- number_default
                    flat_params[[paste(c("genetics.traits.definition", trait, "temperature", element, "individualLevelParams.restrictValue"), collapse = ".")]] <- number_default
                    flat_params[[paste(c("genetics.traits.definition", trait, "temperature", element, "speciesLevelParams.value"), collapse = ".")]] <- value
                }
            }
        }
    }

    flat_params[["genetics.traits.individualLevelTraitsOrder"]] <- traits_parameters[["traits.individualLevelTraitsOrder"]]
    flat_params
}


build_species_state_from_env <- function(env, derived_param_paths) {
    # Build a normalized state object consumed by the tab runtime and exporter.
    params_from_env <- extract_script_flat_params(env)
    flat_params <- append_traits_editor_params(params_from_env$flat_params, params_from_env$traits_parameters)

    flat_params <- filter_params_to_schema(flat_params, animal_schema_properties)

    derived_map <- setNames(rep(FALSE, length(flat_params)), names(flat_params))
    mapped_derived_paths <- map_derived_paths_to_editor_paths(derived_param_paths, names(flat_params))
    derived_map[names(flat_params) %in% mapped_derived_paths] <- TRUE

    flat_params <- wrap_flat_params(flat_params, derived_map)

    list(
        param_plots = extract_param_plot_entries(env),
        bibliography_data = extract_bibliography_entries(env),
        ontogenetic_links = env$ontogenetic_links,
        params = unflatten_list(flat_params),
        flat_params = flat_params
    )
}


species_input_key <- function(species_name) {
    gsub("[^A-Za-z0-9]", "_", species_name)
}


editor_input_id <- function(species_name, path) {
    paste0("param_", species_input_key(species_name), "_", gsub("\\.", "_", path))
}


array_add_input_id <- function(species_name, path) {
    paste0("add_array_", species_input_key(species_name), "_", gsub("\\.", "_", path))
}


array_remove_input_id <- function(species_name, path, index) {
    paste0("remove_array_", species_input_key(species_name), "_", gsub("\\.", "_", path), "_", index)
}


path_part_index <- function(part) {
    if (grepl("^[0-9]+$", part)) as.integer(part) else part
}


get_nested_path_value <- function(value, path) {
    # Supports numeric indices in dotted paths for arrays of objects.
    for (part in strsplit(path, "\\.")[[1]]) {
        if (is.null(value)) return(NULL)
        if (is_parameter_entry(value)) value <- parameter_value(value)
        value <- value[[path_part_index(part)]]
    }
    if (is_parameter_entry(value)) value <- parameter_value(value)
    value
}


set_nested_path_value <- function(value, path, replacement) {
    parts <- strsplit(path, "\\.")[[1]]

    # Preserve `derived` metadata when replacing leaf/container values.
    set_value <- function(current_value, remaining_parts) {
        part <- path_part_index(remaining_parts[[1]])
        if (length(remaining_parts) == 1) {
            existing_value <- current_value[[part]]
            if (is_parameter_entry(existing_value)) {
                current_value[[part]] <- parameter_entry(
                    replacement,
                    parameter_is_derived(existing_value)
                )
            } else {
                current_value[[part]] <- replacement
            }
            return(current_value)
        }

        if (is.null(current_value[[part]])) current_value[[part]] <- list()

        if (is_parameter_entry(current_value[[part]])) {
            entry <- current_value[[part]]
            entry_value <- set_value(parameter_value(entry), remaining_parts[-1])
            current_value[[part]] <- parameter_entry(entry_value, parameter_is_derived(entry))
        } else {
            current_value[[part]] <- set_value(current_value[[part]], remaining_parts[-1])
        }

        current_value
    }

    set_value(value, parts)
}


flatten_params_for_editor <- function(value, schema_node, path = "") {
    node_value <- parameter_value(value)

    if (identical(schema_node$type, "object")) {
        result <- list()
        child_properties <- schema_child_properties(schema_node)

        for (name in names(node_value)) {
            child_schema <- get_schema_property(child_properties, name)
            if (!is.null(child_schema)) {
                child_path <- if (path == "") name else paste0(path, ".", name)
                result <- c(result, flatten_params_for_editor(node_value[[name]], child_schema, child_path))
            }
        }

        return(result)
    }

    leaf_value <- if (is_parameter_entry(value)) value else parameter_entry(node_value, FALSE)
    setNames(list(leaf_value), path)
}


filter_params_for_search <- function(value, schema_node, path, search_term) {
    current_value <- parameter_value(value)

    if (grepl(search_term, path, fixed = TRUE)) return(value)

    if (identical(schema_node$type, "object")) {
        result <- list()
        child_properties <- schema_child_properties(schema_node)

        for (name in names(current_value)) {
            child_schema <- get_schema_property(child_properties, name)
            if (!is.null(child_schema)) {
                child_path <- if (path == "") name else paste0(path, ".", name)
                filtered_value <- filter_params_for_search(current_value[[name]], child_schema, child_path, search_term)
                if (!is.null(filtered_value)) result[[name]] <- filtered_value
            }
        }

        return(if (length(result) == 0) NULL else result)
    }

    if (identical(schema_node$type, "array") && identical(schema_node$items$type, "object")) {
        # Keep only matching array elements instead of returning whole arrays.
        result <- list()
        for (index in seq_along(current_value)) {
            element_path <- paste0(path, ".", index)
            filtered_element <- filter_params_for_search(current_value[[index]], schema_node$items, element_path, search_term)
            if (!is.null(filtered_element)) {
                result[[length(result) + 1]] <- filtered_element
            }
        }

        return(if (length(result) == 0) NULL else result)
    }

    NULL
}


add_complex_array_element <- function(species_name, array_path, schema_node) {
    # Mutates nested params directly, then rebuilds the flattened editor index.
    current_data <- species_data[[species_name]]
    array_value <- get_nested_path_value(current_data$params, array_path)
    if (is.null(array_value)) array_value <- list()

    array_value <- c(array_value, list(schema_default_value(schema_node$items)))
    current_data$params <- set_nested_path_value(current_data$params, array_path, array_value)
    current_data$flat_params <- flatten_params_for_editor(
        current_data$params,
        list(type = "object", properties = animal_schema_properties)
    )
    species_data[[species_name]] <- current_data
}


remove_complex_array_element <- function(species_name, array_path, index) {
    current_data <- species_data[[species_name]]
    array_value <- get_nested_path_value(current_data$params, array_path)
    if (is.null(array_value) || index > length(array_value)) return(invisible(NULL))

    array_value <- array_value[-index]
    current_data$params <- set_nested_path_value(current_data$params, array_path, array_value)
    current_data$flat_params <- flatten_params_for_editor(
        current_data$params,
        list(type = "object", properties = animal_schema_properties)
    )
    species_data[[species_name]] <- current_data
}


# Read the current controls only when the user presses Save. Inputs that are
# not rendered (for example because of an active search) retain their value.
read_editor_value <- function(input, value, schema_node, species_name, path, force_readonly = FALSE) {
    node_is_derived <- is_schema_leaf_node(schema_node) && parameter_is_derived(value)
    node_readonly <- isTRUE(force_readonly) || node_is_derived
    current_value <- parameter_value(value)

    if (identical(schema_node$type, "object")) {
        child_properties <- schema_child_properties(schema_node)
        result <- current_value
        for (name in names(current_value)) {
            child_schema <- get_schema_property(child_properties, name)
            if (!is.null(child_schema)) {
                child_path <- if (path == "") name else paste0(path, ".", name)
                result[[name]] <- read_editor_value(
                    input, current_value[[name]], child_schema, species_name,
                    child_path, isTRUE(force_readonly)
                )
            }
        }
        return(result)
    }

    if (node_readonly) {
        if (is_parameter_entry(value)) return(value)
        return(parameter_entry(current_value, TRUE))
    }

    input_value <- input[[editor_input_id(species_name, path)]]
    if (identical(schema_node$type, "array")) {
        if (identical(schema_node$items$type, "object")) {
            updated <- lapply(seq_along(current_value), function(index) {
                read_editor_value(
                    input, current_value[[index]], schema_node$items, species_name,
                    paste0(path, ".", index), isTRUE(force_readonly)
                )
            })
            return(parameter_entry(updated, FALSE))
        }

        updated <- if (is.null(input_value)) {
            current_value
        } else {
            parse_primitive_array(input_value, schema_node$items)
        }

        return(parameter_entry(updated, FALSE))
    }

    updated <- if (is.null(input_value)) {
        current_value
    } else {
        coerce_schema_value(input_value, schema_node)
    }

    parameter_entry(updated, FALSE)
}


animal_schema_properties <- NULL


#' Load All Animal Species Data
#'
#' Runs the species info script to obtain growth curves and parameters.
load_animal_species_data <- function(species) {
    env <- new.env()
    env$app <- list()
    env <- run_animal_species_info_script(species, env, apply_baseline = TRUE)
    derived_param_paths <- get_cached_derived_param_paths(species)
    build_species_state_from_env(env, derived_param_paths)
}



#' Update Animal Species Data from UI Parameters
#'
#' Maps the modified `flat_params` back to the original script namespace (`param.*` y `traits.*`),
#' injects them into an environment, and re-runs the species script.
#'
#' @param species Character. Name of the species.
#' @param current_flat_params List. The flat_params (modified) from the UI state.
#' @return A list identical in structure to `load_animal_species_data()` output.
update_animal_species_data <- function(species, current_flat_params) {
    env <- new.env()
    env$app <- list()
    current_flat_params <- unwrap_flat_params(current_flat_params)

    # =========================================================================
    # 1. INGENIERÍA INVERSA: Mapear flat_params a variables del script
    # =========================================================================
    
    # 1.1. Parámetros Estándar (Todo lo que no es genética/traits)
    non_trait_keys <- names(current_flat_params)[!grepl("^genetics\\.traits", names(current_flat_params))]
    for (key in non_trait_keys) {
        assign(paste0("param.", key), current_flat_params[[key]], envir = env)
    }

    # 1.2. Orden de los Traits
    traits_order <- current_flat_params[["genetics.traits.individualLevelTraitsOrder"]]
    if (!is.null(traits_order)) {
        assign("param.traits.individualLevelTraitsOrder", traits_order, envir = env)
    }

    # 1.3. Reconstruir Variables de Traits (Desempaquetando el Schema JSON)
    trait_keys <- grep("^genetics\\.traits\\.definition\\.", names(current_flat_params), value = TRUE)
    traits_name <- unique(sub("^genetics\\.traits\\.definition\\.([^.]+)\\..*", "\\1", trait_keys))

    for (trait in traits_name) {
        prefix <- paste0("genetics.traits.definition.", trait, ".")
        def_type <- current_flat_params[[paste0(prefix, "definitionType")]]
        
        # Desempaquetar Rangos/Límites según el tipo de definición
        if (!is.null(def_type)) {
            if (def_type == "SpeciesLevel") {
                val <- current_flat_params[[paste0(prefix, "speciesLevelParams.value")]]
                # Asignamos el mismo valor a todo para simular "SpeciesLevel" rígido
                assign(paste0("param.traits.", trait, ".ranges.min"), val, envir = env)
                assign(paste0("param.traits.", trait, ".ranges.max"), val, envir = env)
                assign(paste0("param.traits.", trait, ".limits.min"), val, envir = env)
                assign(paste0("param.traits.", trait, ".limits.max"), val, envir = env)
                assign(paste0("param.traits.", trait, ".restrictValue"), val, envir = env)
            } else if (def_type == "IndividualLevel") {
                assign(paste0("param.traits.", trait, ".limits.min"), current_flat_params[[paste0(prefix, "individualLevelParams.limits.min")]], envir = env)
                assign(paste0("param.traits.", trait, ".limits.max"), current_flat_params[[paste0(prefix, "individualLevelParams.limits.max")]], envir = env)
                assign(paste0("param.traits.", trait, ".ranges.min"), current_flat_params[[paste0(prefix, "individualLevelParams.ranges.min")]], envir = env)
                assign(paste0("param.traits.", trait, ".ranges.max"), current_flat_params[[paste0(prefix, "individualLevelParams.ranges.max")]], envir = env)
                assign(paste0("param.traits.", trait, ".restrictValue"), current_flat_params[[paste0(prefix, "individualLevelParams.restrictValue")]], envir = env)
            }
        }
        
        # Desempaquetar Dependencias de Temperatura
        if (trait != "base.energy_tank") {
            temp_dep <- current_flat_params[[paste0(prefix, "temperature.dependent")]]
            if (!is.null(temp_dep)) {
                assign(paste0("param.traits.", trait, ".temperature.dependent"), temp_dep, envir = env)
            }
            
            if (trait == "base.lengthAtMaturation") {
                rule_vec <- current_flat_params[[paste0(prefix, "temperature.tempSizeRuleVector")]]
                if (!is.null(rule_vec)) {
                    assign(paste0("param.traits.", trait, ".temperature.tempSizeRuleVector"), rule_vec, envir = env)
                }
            } else {
                for (element in c("activationEnergy", "energyDecay", "temperatureOptimal", "temperatureRef")) {
                    el_val <- current_flat_params[[paste0(prefix, "temperature.", element, ".speciesLevelParams.value")]]
                    if (!is.null(el_val)) {
                        assign(paste0("param.traits.", trait, ".temperature.", element), el_val, envir = env)
                    }
                }
            }
        }
    }

    # =========================================================================
    # 2. EJECUCIÓN: Llamar al script con las variables inyectadas
    # =========================================================================
    
    # Derived script reuses preloaded values as overrides (no baseline on save).
    env <- run_animal_species_info_script(species, env, apply_baseline = FALSE)
    derived_param_paths <- get_cached_derived_param_paths(species)
    build_species_state_from_env(env, derived_param_paths)
}



# Función auxiliar para manejar valores nulos de forma segura
ifnull <- function(x, y) {
  if (is.null(x) || length(x) == 0) y else x
}


#' Generate Tree UI based on Schema
#' Función recursiva para crear inputs de Shiny basados estrictamente en el schema JSON
generate_tree_ui <- function(param_list, schema_properties, species_name, path = "", force_readonly = FALSE, param_plots = list(), show_all_defaults = TRUE) {
    ui_elements <- lapply(names(schema_properties), function(node_name) {
        
        schema_node <- get_schema_property(schema_properties, node_name)

        if (node_name %in% names(param_list)) {
            val_entry <- param_list[[node_name]]
        }
        else {
            if (!isTRUE(show_all_defaults)) return(NULL)
            val_entry <- parameter_entry(schema_default_value(schema_node), FALSE)
        }

        node_is_derived <- is_schema_leaf_node(schema_node) && parameter_is_derived(val_entry)
        node_readonly <- isTRUE(force_readonly) || node_is_derived
        val <- parameter_value(val_entry)
        
        
        # Construir la ruta actual para el input ID (ej. animal.genetics.modules)
        current_path <- if (path == "") node_name else paste0(path, ".", node_name)
        input_id <- editor_input_id(species_name, current_path)
        plot_entry <- get_plot_entry_for_path(param_plots, current_path)
        has_plot <- !is.null(plot_entry)
        plot_ui <- if (has_plot) {
            tags$div(
                style = "margin-top: 8px; margin-bottom: 6px;",
                plotlyOutput(plot_output_id(species_name, current_path), height = "260px")
            )
        } else {
            NULL
        }
        
        # 1. Si es un objeto (rama anidada) -> tags$details estructurado
        if (identical(schema_node$type, "object")) {
            sub_props <- schema_child_properties(schema_node)
            
            tags$details(
                open = TRUE,
                tags$summary(tags$strong(node_name), style = "cursor: pointer; margin-top: 10px; color: #2c3e50;"),
                tags$div(
                    style = "margin-left: 20px; border-left: 2px solid #ecf0f1; padding-left: 15px;",
                    generate_tree_ui(val, sub_props, species_name, current_path, isTRUE(force_readonly), param_plots, show_all_defaults)
                )
            )
        } 
        # 2. Si es un booleano -> checkboxInput
        else if (identical(schema_node$type, "boolean")) {
            tags$div(
                # Añadimos un margin-bottom de 15px para igualar el margen nativo de los otros .form-group
                style = "margin-top: 10px; margin-bottom: 15px;", 
                
                # Estilo CSS exclusivo calibrado para el checkbox escalado
                tags$style(HTML(sprintf("
                    #%s {
                        transform: scale(1.4); 
                        transform-origin: left center; 
                        margin-top: 6px;     /* Espaciado perfecto respecto al título superior */
                        margin-bottom: 0px;   /* Reseteamos el margen interno del input */
                        cursor: pointer; 
                    }
                    /* Ajuste para el contenedor interno que Shiny crea automáticamente */
                    .shiny-input-container:has(#%s) {
                        margin-bottom: 0px !important;
                    }
                ", input_id, input_id))),                

                field_title_with_badge(node_name, node_is_derived),
                p(schema_node$type, style = "font-size: 11px; color: #7f8c8d;"),
                
                # Checkbox nativo
                checkboxInput(
                    inputId = input_id,
                    label = NULL, 
                    value = val,
                    width = "100%"
                ),
                plot_ui,
                disable_input_script(input_id, node_readonly)
            )
        }
        # 3. Si tiene un enum definido -> selectInput
        else if (!is.null(schema_node$enum)) {
            enum_choices <- schema_node$enum
            if (is_plot_choice_map(plot_entry)) {
                enum_choices <- enum_choices[enum_choices %in% names(plot_entry)]
            }

            if (length(enum_choices) == 0) {
                return(NULL)
            }

            selected_value <- if (as.character(val) %in% enum_choices) as.character(val) else enum_choices[[1]]

            tags$div(
                style = "margin-top: 10px;",

                field_title_with_badge(node_name, node_is_derived),
                p("enum", style = "font-size: 11px; color: #7f8c8d;"),

                selectInput(
                    inputId = input_id,
                    label = NULL,
                    choices = enum_choices,
                    selected = selected_value,
                    width = "100%"
                ),
                plot_ui,
                disable_input_script(input_id, node_readonly)
            )
        }
        # 4. Si es un número con mínimo y máximo -> sliderInput
        else if ((identical(schema_node$type, "number") || identical(schema_node$type, "integer")) &&
            !is.null(schema_node$minimum) && !is.null(schema_node$maximum)) {
        
            slider_id <- input_id
            numeric_id <- paste0(input_id, "_num")
            is_int <- identical(schema_node$type, "integer")
            step_val <- if (is_int) 1 else 0.01
            
            tags$div(
                style = "margin-top: 10px;",

                field_title_with_badge(node_name, node_is_derived),
                p(schema_node$type, style = "font-size: 11px; color: #7f8c8d;"),
                
                fluidRow(
                    column(9, 
                        sliderInput(
                            inputId = slider_id,
                            label = NULL,
                            min = schema_node$minimum,
                            max = schema_node$maximum,
                            value = val,
                            step = step_val,
                            width = "100%"
                        )
                    ),
                    column(3,
                        numericInput(
                            inputId = numeric_id,
                            label = NULL,
                            min = schema_node$minimum,
                            max = schema_node$maximum,
                            value = val,
                            step = step_val,
                            width = "100%"
                        )
                    )
                ),
                
                # Script JS con protección contra bucles de sobreescritura
                tags$script(HTML(sprintf("
                    setTimeout(function() {
                        var $slider = $('#%s');
                        var $num = $('#%s');
                        
                        // 1. Al mover el slider, SOLO actualiza el input numérico si el usuario NO está escribiendo en él
                        $slider.on('change.sync', function() {
                            if (!$num.is(':focus')) {
                                $num.val($(this).val());
                            }
                        });
                        
                        // 2. Al escribir en el input numérico, actualiza el slider libremente
                        $num.on('input.sync', function() {
                            var val = parseFloat($(this).val());
                            if (!isNaN(val)) {
                                var sliderInstance = $slider.data('ionRangeSlider');
                                if (sliderInstance) {
                                    sliderInstance.update({from: val});
                                }
                                $slider.trigger('change'); 
                            }
                        });
                    }, 150);
                ", slider_id, numeric_id)))
                ,
                tags$script(HTML(sprintf("setTimeout(function(){ var slider = $('#%s').data('ionRangeSlider'); if (slider) { slider.update({ disable: %s }); } $('#%s').prop('disabled', %s); }, 160);", slider_id, tolower(node_readonly), numeric_id, tolower(node_readonly))))
                ,
                plot_ui
            )
        }
        # 5. Si es numérico estándar -> numericInput
        else if (identical(schema_node$type, "number") || identical(schema_node$type, "integer")) {
            tags$div(
                style = "margin-top: 10px;",

                field_title_with_badge(node_name, node_is_derived),
                p(schema_node$type, style = "font-size: 11px; color: #7f8c8d;"),

                numericInput(
                    inputId = input_id,
                    label = NULL,
                    value = val,
                    min = if (!is.null(schema_node$minimum)) schema_node$minimum else NA,
                    step = if (identical(schema_node$type, "integer")) 1 else 0.01,
                    width = "100%"
                ),
                plot_ui,
                disable_input_script(input_id, node_readonly)
            )
        }
        # 6. Si es un array complejo -> uiOutput reservado
        else if (identical(schema_node$type, "array")) {
            tags$div(
                # ELIMINADO: padding, border y border-radius
                style = "margin-top: 10px;", 
                
                field_title_with_badge(node_name, node_is_derived),
                
                if (identical(schema_node$items$type, "object")) {
                    tags$span(paste0(" (", length(val), " element(s))"), style = "color: #7f8c8d;")
                },
                
                p(schema_node$type, style = "font-size: 11px; color: #7f8c8d;"),
                
                if (identical(schema_node$items$type, "object")) {
                    tagList(
                        lapply(seq_along(val), function(index) {
                            tags$div(
                                # Nota: Este div interno mantiene su propio recuadro sutil para separar cada elemento del array. 
                                # Si también quieres quitarlo, borra "border: 1px solid #f1f3f5;" de la línea de abajo.
                                style = "margin: 8px 0 12px 15px; padding: 10px 12px; border-left: 2px solid #ecf0f1; border: 1px solid #f1f3f5; border-radius: 4px;",
                                tags$div(
                                    style = "display: flex; justify-content: space-between; align-items: center; gap: 0.75rem; margin-bottom: 8px;",
                                    tags$strong(paste("Element", index)),
                                    actionButton(
                                        inputId = array_remove_input_id(species_name, current_path, index),
                                        label = NULL,
                                        icon = icon("trash"),
                                        class = "btn btn-outline-danger btn-sm",
                                        title = paste("Remove element", index)
                                    )
                                ),
                                generate_tree_ui(
                                    val[[index]],
                                    schema_child_properties(schema_node$items),
                                    species_name,
                                    paste0(current_path, ".", index),
                                    isTRUE(force_readonly),
                                    param_plots,
                                    show_all_defaults
                                )
                            )
                        }),
                        if (!node_readonly) {
                            actionButton(
                                inputId = array_add_input_id(species_name, current_path),
                                label = "Add element",
                                class = "btn btn-outline-secondary btn-sm"
                            )
                        }
                    )
                } else {
                    textInput(
                        inputId = input_id,
                        label = NULL,
                        value = paste(unlist(val), collapse = ", "),
                        placeholder = "Values separated by commas",
                        width = "100%"
                    )
                }
                ,
                plot_ui,
                if (identical(schema_node$items$type, "object") && node_readonly) {
                    tags$script(HTML(sprintf("setTimeout(function(){ $('[id^=remove_array_%s_%s_]').prop('disabled', true); }, 0);", species_input_key(species_name), gsub("\\.", "_", current_path))))
                },
                if (!identical(schema_node$items$type, "object")) disable_input_script(input_id, node_readonly)
            )
        }
        # 7. Por defecto -> textInput
        else {
            tags$div(
                style = "margin-top: 10px;",

                field_title_with_badge(node_name, node_is_derived),
                p(schema_node$type, style = "font-size: 11px; color: #7f8c8d;"),

                textInput(
                    inputId = input_id,
                    label = NULL,
                    value = as.character(val),
                    width = "100%"
                ),
                plot_ui,
                disable_input_script(input_id, node_readonly)
            )
        }
    })
    
    # Eliminar valores nulos correspondientes a propiedades omitidas
    purrr::compact(ui_elements)
}


# ReactiveValues to store selected animal species
selected_animal_species <- reactiveValues(values = character(0))

# Per-species runtime state: params, flattened params, plot bindings,
# bibliography metadata, and derived ontogenetic links.
species_data <- reactiveValues()
unsaved_changes <- reactiveValues()
complex_array_observer_registry <- new.env(parent = emptyenv())
species_remove_observer_registry <- new.env(parent = emptyenv())


#' Animal Species Server Module
#'
#' Manages the server logic for the Animal Species tab, including:
#' - Filtering species by taxonomy
#' - Adding selected species as tabs
#' - Generating dynamic UI for growth curves and parameters
#' - Removing species tabs
#'
#' @param input Shiny input object.
#' @param output Shiny output object.
#' @param session Shiny session object.
#' @export
animal_species_server <- function(input, output, session) {

    render_parameter_plot_outputs <- function(species_name) {
        # Plot bindings are keyed by parameter path (`plot.<path>`). Some paths
        # map to a single plot, others to named choices (e.g., enum values).
        current_data <- species_data[[species_name]]
        if (is.null(current_data) || length(current_data$param_plots) == 0) return(invisible(NULL))

        candidate_paths <- unique(c(
            names(current_data$flat_params),
            names(current_data$param_plots)
        ))
        candidate_paths <- candidate_paths[sapply(candidate_paths, function(path) {
            !is.null(get_plot_entry_for_path(current_data$param_plots, path))
        })]

        lapply(candidate_paths, function(plot_path) {
            output_id <- plot_output_id(species_name, plot_path)

            local({
                current_species <- species_name
                current_plot_path <- plot_path

                output[[output_id]] <- renderPlotly({
                    data_for_species <- species_data[[current_species]]
                    req(!is.null(data_for_species))

                    plot_binding <- get_plot_entry_for_path(data_for_species$param_plots, current_plot_path)
                    req(!is.null(plot_binding))

                    if (is_plot_choice_map(plot_binding)) {
                        selected_value <- input[[editor_input_id(current_species, current_plot_path)]]

                        if (is.null(selected_value) || !(selected_value %in% names(plot_binding))) {
                            fallback_value <- get_nested_path_value(data_for_species$params, current_plot_path)
                            fallback_value <- as.character(parameter_value(fallback_value))

                            if (!is.null(fallback_value) && fallback_value %in% names(plot_binding)) {
                                selected_value <- fallback_value
                            } else {
                                selected_value <- names(plot_binding)[[1]]
                            }
                        }

                        return(plot_binding[[selected_value]])
                    }

                    plot_binding
                })
            })
        })

        invisible(NULL)
    }

    register_complex_array_handlers <- function(species_name) {
        # Dynamic IDs are registered once and kept across re-renders to prevent
        # duplicated observers and repeated event firing.
        register_once <- function(input_id, handler) {
            if (!exists(input_id, envir = complex_array_observer_registry, inherits = FALSE)) {
                complex_array_observer_registry[[input_id]] <- TRUE
                handler()
            }
        }

        recurse <- function(value, schema_node, path = "") {
            current_value <- parameter_value(value)

            if (identical(schema_node$type, "object")) {
                child_properties <- schema_child_properties(schema_node)

                for (name in names(current_value)) {
                    child_schema <- get_schema_property(child_properties, name)
                    if (!is.null(child_schema)) {
                        child_path <- if (path == "") name else paste0(path, ".", name)
                        recurse(current_value[[name]], child_schema, child_path)
                    }
                }

                return(invisible(NULL))
            }

            if (!identical(schema_node$type, "array") || !identical(schema_node$items$type, "object")) {
                return(invisible(NULL))
            }

            add_id <- array_add_input_id(species_name, path)
            register_once(add_id, local({
                current_species <- species_name
                current_path <- path
                current_schema <- schema_node
                current_add_id <- add_id

                function() {
                    observeEvent(input[[current_add_id]], {
                        add_complex_array_element(current_species, current_path, current_schema)
                        unsaved_changes[[current_species]] <- TRUE
                        register_complex_array_handlers(current_species)
                    }, ignoreInit = TRUE)
                }
            }))

            for (index in seq_along(current_value)) {
                remove_id <- array_remove_input_id(species_name, path, index)
                register_once(remove_id, local({
                    current_species <- species_name
                    current_path <- path
                    current_index <- index
                    current_remove_id <- remove_id

                    function() {
                        observeEvent(input[[current_remove_id]], {
                            remove_complex_array_element(current_species, current_path, current_index)
                            unsaved_changes[[current_species]] <- TRUE
                            register_complex_array_handlers(current_species)
                        }, ignoreInit = TRUE)
                    }
                }))

                recurse(current_value[[index]], schema_node$items, paste0(path, ".", index))
            }

            invisible(NULL)
        }

        current_data <- species_data[[species_name]]
        if (!is.null(current_data)) {
            recurse(current_data$params, list(type = "object", properties = animal_schema_properties))
        }
    }

    observeEvent(active_schema_version(), {
        version <- active_schema_version()
        req(version)
        animal_schema_properties <<- load_animal_schema_properties(version)

        # Parameters are schema-version specific. Remove all selected entities
        # rather than leaving tabs backed by a different schema revision.
        lapply(selected_animal_species$values, function(species) {
            removeTab("tabset_selected_animal_species", target = species)
            species_data[[species]] <- NULL
            unsaved_changes[[species]] <- NULL
        })
        selected_animal_species$values <- character(0)

        if (exists("selected_resource_species", inherits = TRUE)) {
            lapply(selected_resource_species$values, function(species) {
                removeTab("tabset_selected_resource_species", target = species)
            })
            selected_resource_species$values <- character(0)
        }
        showNotification(paste("Schema", version, "loaded. Species tabs were reset."), type = "message")
    }, ignoreInit = FALSE)
    
    # Observe taxonomic field changes to update downstream selectize inputs.
    lapply(colnames(animal_species_register)[-length(colnames(animal_species_register))], function(field) {
        observeEvent(input[[field]], {
            update_all_selectize_input_animal_species(session, input, field)
        }, ignoreInit = TRUE)
    })

    # Add selected animal species
    observeEvent(input$add_animal_species_button, {
        if (!is.null(input$AnimalSpecies) && !input$AnimalSpecies %in% selected_animal_species$values) {
            req(!is.null(animal_schema_properties))
            
            # 1. Capturamos la especie actual en una variable fija para esta pestaña
            current_species <- input$AnimalSpecies
            
            selected_animal_species$values <- c(selected_animal_species$values, current_species)

            species_data[[current_species]] <- load_animal_species_data(current_species)
            unsaved_changes[[current_species]] <- FALSE
            register_complex_array_handlers(current_species)
            render_parameter_plot_outputs(current_species)

            # Insert main species tab with collapsible panels for Parameters and Growth Curve
            insertTab(inputId = "tabset_selected_animal_species",
                tabPanel(
                    title = uiOutput(paste0("tab_title_", current_species), inline = TRUE), 
                    value = current_species,
                    br(),
                    tabsetPanel(
                        id = paste0("subtabs_", current_species),
                        type = "pills", 

                        # --- Pestaña: Editor de Parámetros ---
                        tabPanel(
                            title = "Parameter Editor",
                            icon = icon("sliders"),
                            br(),
                            fluidRow(
                                column(5, tags$div(style = "display: flex; gap: 10px;",
                                    actionButton(
                                        inputId = paste0("save_params_", current_species),
                                        label = "Save changes",
                                        icon = icon("save"),
                                        class = "btn-primary"
                                    ),
                                    actionButton(
                                        inputId = paste0("discard_params_", current_species),
                                        label = "Discard changes",
                                        icon = icon("rotate-left"),
                                        class = "btn-secondary"
                                    )
                                )),
                                column(7, uiOutput(paste0("params_status_", current_species)))
                            ),
                            br(),
                            # Buscador de parámetros
                            textInput(
                                inputId = paste0("search_params_", current_species),
                                label = NULL,
                                placeholder = "🔍 Buscar parámetro o categoría...",
                                width = "100%"
                            ),
                            hr(),
                            # UI Dinámica para el árbol de inputs
                            uiOutput(paste0("ui_parameter_editor_", current_species)),
                            uiOutput(paste0("parameter_change_listener_", current_species))
                        )
                    )
                )
            )

            # Renderizar Pestaña 2: Parameter Editor (Usando current_species)
            output[[paste0("ui_parameter_editor_", current_species)]] <- renderUI({
                force(species_data[[current_species]]$render_trigger)
                search_term <- trimws(ifnull(input[[paste0("search_params_", current_species)]], ""))
                search_active <- nzchar(search_term)
                params <- species_data[[current_species]]$params
                
                if (search_active) {
                    params <- filter_params_for_search(
                        params,
                        list(type = "object", properties = animal_schema_properties),
                        "",
                        search_term
                    )
                }
                
                if (is.null(params) || length(params) == 0) {
                    return(tags$p(tags$em("No parameters were found matching that search.")))
                }
                
                ui_tree <- generate_tree_ui(
                    params,
                    animal_schema_properties,
                    current_species,
                    param_plots = ifnull(species_data[[current_species]]$param_plots, list()),
                    show_all_defaults = !search_active
                )
                
                do.call(tagList, ui_tree)
            })

            output[[paste0("params_status_", current_species)]] <- renderUI({
                if (isTRUE(unsaved_changes[[current_species]])) {
                    tags$span(icon("circle-exclamation"), " Unsaved changes", class = "text-warning", style = "line-height: 34px;")
                } else {
                    tags$span(icon("circle-check"), " Saved", class = "text-success", style = "line-height: 34px;")
                }
            })

            output[[paste0("parameter_change_listener_", current_species)]] <- renderUI({
                tags$script(sprintf(
                    "$(document).off('change.weaver-%1$s', '[id^=param_%1$s_]').on('change.weaver-%1$s', '[id^=param_%1$s_]', function(){ Shiny.setInputValue('editor_changed_%1$s', Date.now(), {priority:'event'}); });",
                    species_input_key(current_species)
                ))
            })

            observeEvent(input[[paste0("editor_changed_", species_input_key(current_species))]], {
                unsaved_changes[[current_species]] <- TRUE
            }, ignoreInit = TRUE)

            # Botón Save Changes (Popup)
            observeEvent(input[[paste0("save_params_", current_species)]], {
                if (isTRUE(unsaved_changes[[current_species]])) {
                    showModal(modalDialog(
                        title = "Save Changes",
                        "Are you sure you want to save all applied changes?",
                        footer = tagList(
                            modalButton("Cancel"),
                            actionButton(
                                inputId = paste0("confirm_save_", current_species), 
                                label = "Yes, save changes", 
                                class = "btn-primary"
                            )
                        )
                    ))
                } else {
                    showNotification("No changes to save.", type = "message")
                }
            }, ignoreInit = TRUE)

            observeEvent(input[[paste0("confirm_save_", current_species)]], {
                current_data <- species_data[[current_species]]
                # Read only currently rendered controls; hidden controls keep
                # previous values in `current_data$params`.
                current_data$params <- read_editor_value(
                    input,
                    current_data$params,
                    list(type = "object", properties = animal_schema_properties),
                    current_species,
                    ""
                )
                current_data$flat_params <- flatten_params_for_editor(
                    current_data$params,
                    list(type = "object", properties = animal_schema_properties)
                )

                # Avoid expensive script recomputation when the effective state
                # is unchanged (for example, user edits and reverts values).
                if (identical(current_data$flat_params, species_data[[current_species]]$flat_params)) {
                    unsaved_changes[[current_species]] <- FALSE
                    removeModal()
                    showNotification("No effective parameter changes to save.", type = "message")
                    return(invisible(NULL))
                }

                updated_data <- update_animal_species_data(current_species, current_data$flat_params)

                # Keep previously captured bibliography metadata if the updated
                # script pass did not emit bibliography IDs.
                if (
                    is.null(updated_data$bibliography_data$entries) ||
                    length(updated_data$bibliography_data$entries) == 0
                ) {
                    updated_data$bibliography_data <- current_data$bibliography_data
                }

                species_data[[current_species]] <- updated_data
                render_parameter_plot_outputs(current_species)

                unsaved_changes[[current_species]] <- FALSE
                
                removeModal()
                showNotification("Parameter changes saved.", type = "message")
            }, ignoreInit = TRUE)

            # Botón Discard Changes (Popup)
            observeEvent(input[[paste0("discard_params_", current_species)]], {
                if (isTRUE(unsaved_changes[[current_species]])) {
                    showModal(modalDialog(
                        title = "Discard Changes",
                        "Are you sure you want to discard all unsaved changes? This action cannot be undone.",
                        footer = tagList(
                            modalButton("Cancel"),
                            actionButton(
                                inputId = paste0("confirm_discard_", current_species), 
                                label = "Yes, discard changes", 
                                class = "btn-danger"
                            )
                        )
                    ))
                } else {
                    showNotification("No changes to discard.", type = "message")
                }
            }, ignoreInit = TRUE)

            observeEvent(input[[paste0("confirm_discard_", current_species)]], {
                unsaved_changes[[current_species]] <- FALSE
                current_data <- species_data[[current_species]]
                current_data$render_trigger <- Sys.time() 
                species_data[[current_species]] <- current_data
                
                removeModal()
                showNotification("Changes discarded. Restored to previous values.", type = "warning")
            }, ignoreInit = TRUE)

            # Renderizado del título dinámico de la pestaña
            output[[paste0("tab_title_", current_species)]] <- renderUI({
                is_dirty <- isTRUE(unsaved_changes[[current_species]])
                bg_color <- if (is_dirty) "#fef9e7" else "#e8f8f5"
                border_color <- if (is_dirty) "#f39c12" else "#27ae60"
                text_color <- if (is_dirty) "#7d6608" else "#0e6251"
                
                tags$div(
                    style = sprintf(
                        "background-color: %s; border: 1px solid %s; color: %s; padding: 6px 12px; border-radius: 6px; display: inline-flex; align-items: center; gap: 8px; transition: all 0.3s ease;",
                        bg_color, border_color, text_color
                    ),
                    tags$span(tags$em(current_species)),
                    actionButton(
                        paste0("remove_", current_species), 
                        icon("xmark"),
                        style = "padding: 0px 4px; background: transparent; border: none; color: #c0392b; cursor: pointer;"
                    )
                )
            })
        }
    })

    # Observe remove buttons to delete species tabs
    observe({
        lapply(selected_animal_species$values, function(value) {
            button_id <- paste0("remove_", value)
            if (exists(button_id, envir = species_remove_observer_registry, inherits = FALSE)) {
                return(invisible(NULL))
            }
            species_remove_observer_registry[[button_id]] <- TRUE

            observeEvent(input[[button_id]], {
                showModal(modalDialog(
                    title = "Confirm Deletion",
                    paste("Are you sure you want to delete the animal species \"", value, "\"?"),
                    footer = tagList(
                        modalButton("No"),
                        actionButton(paste0("confirm_remove_", value), "Yes")
                    )
                ))
            }, ignoreInit = TRUE)

            observeEvent(input[[paste0("confirm_remove_", value)]], {
                removeTab(inputId = "tabset_selected_animal_species", target = value)
                selected_animal_species$values <- selected_animal_species$values[selected_animal_species$values != value]  # Filtra el elemento a eliminar
                species_data[[value]] <- NULL
                unsaved_changes[[value]] <- NULL
                rm(list = button_id, envir = species_remove_observer_registry)
                removeModal()
            }, ignoreInit = TRUE, once = TRUE)
        })
    })
}
