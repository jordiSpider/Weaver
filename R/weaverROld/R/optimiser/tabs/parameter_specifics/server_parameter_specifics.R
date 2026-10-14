# --- Tab 3 Logic: Dynamic UI Field Generation Driven by JSON Schema Matrix ---
`%||%` <- function(x, y) if (is.null(x)) y else x

resolve_parameter_class <- function(schema_type, json_path, schema_file, enum_values = "") {
  # Capture the special CSV parameter class
  if (identical(schema_file, "ontogeneticLinksPreference.csv")) {
    return("Preference")
  }

  if (identical(schema_file, "ontogeneticLinksProfitability.csv")) {
    return("Profitability")
  }
  
  if (identical(schema_type, "number")) return("RealValued")
  if (identical(schema_type, "integer")) return("Integer")
  if (identical(schema_type, "boolean")) return("Boolean")

  if (identical(schema_type, "array")) {
    if (identical(schema_file, "species_schema.json") && identical(json_path, "animal$individualsPerInstar")) {
      return("Abundances")
    }
    return("Array")
  }

  if (identical(schema_type, "string") && nzchar(enum_values)) return("Permutation")
  if (identical(schema_type, "string")) return("String")

  "RealValued"
}

resolve_config_destinations_by_schema <- function(schema_file, base_config_path) {
  if (identical(schema_file, "ontogeneticLinksPreference.csv") || 
      identical(schema_file, "ontogeneticLinksProfitability.csv")) {
    if (base_config_path == "" || !dir.exists(base_config_path)) return(character(0))
    target_path <- file.path(base_config_path, "species", schema_file)
    if (file.exists(target_path)) return(file.path("species", schema_file))
    return(character(0))
  }  

  schema_to_target <- list(
    simulation_params_schema.json = list(mode = "file", path = "simulation_params.json"),
    landscape_params_schema.json = list(mode = "file", path = "landscape_params.json"),
    species_schema.json = list(mode = "dir", path = "species"),
    resource_schema.json = list(mode = "dir", path = "resource"),
    resource_patch_schema.json = list(mode = "dir", path = "resource/patches"),
    obstacle_patch_schema.json = list(mode = "dir", path = "obstacle"),
    moisture_patch_schema.json = list(mode = "dir", path = "moisture"),
    habitat_domain_patch_schema.json = list(mode = "dir", path = "habitat_domain")
  )

  rule <- schema_to_target[[schema_file]]
  if (is.null(rule) || base_config_path == "" || !dir.exists(base_config_path)) return(character(0))

  target_path <- file.path(base_config_path, rule$path)

  if (identical(rule$mode, "file")) {
    if (file.exists(target_path)) return(rule$path)
    return(character(0))
  }

  if (!dir.exists(target_path)) return(character(0))

  found <- list.files(target_path, pattern = "\\.json$", full.names = FALSE)
  if (length(found) == 0) return(character(0))

  file.path(rule$path, found)
}

# Get the catalog of available animal species in the species subfolder
get_species_metadata_catalog <- function(base_config_path) {
  species_dir <- file.path(base_config_path, "species")
  if (base_config_path == "" || !dir.exists(species_dir)) return(list())
  
  json_files <- list.files(species_dir, pattern = "\\.json$", full.names = TRUE)
  catalog <- list()
  
  for (f in json_files) {
    tryCatch({
      data <- jsonlite::fromJSON(f, simplifyVector = TRUE)
      sp_name <- data[["animal"]][["name"]]
      individuals <- data[["animal"]][["individualsPerInstar"]]
      
      if (!is.null(sp_name)) {
        # Guardamos el formato limpio para variables R y el total de instars
        catalog[[sp_name]] <- list(
          file_name = basename(f),
          instar_count = if (is.null(individuals)) 1 else length(individuals)
        )
      }
    }, error = function(e) NULL)
  }
  return(catalog)
}

# Get the catalog of available resources in the resource subfolder
get_resource_metadata_catalog <- function(base_config_path) {
  resource_dir <- file.path(base_config_path, "resource")
  if (base_config_path == "" || !dir.exists(resource_dir)) return(list())
  
  json_files <- list.files(resource_dir, pattern = "\\.json$", full.names = TRUE)
  catalog <- list()
  
  for (f in json_files) {
    tryCatch({
      data <- jsonlite::fromJSON(f, simplifyVector = TRUE)
      res_name <- data[["resource"]][["name"]]
      
      if (!is.null(res_name)) {
        catalog[[res_name]] <- list(
          file_name = basename(f),
          instar_count = 1 # Resources always have exactly 1 instar
        )
      }
    }, error = function(e) NULL)
  }
  return(catalog)
}

get_instar_count_from_destination <- function(dest_path, base_config_path) {
  if (is.null(dest_path) || length(dest_path) == 0 || dest_path == "") return(0)
  
  # Build the full absolute path to the species JSON file
  full_path <- file.path(base_config_path, dest_path)
  
  if (!file.exists(full_path)) return(0)
  
  # Leer el JSON de forma segura
  tryCatch({
    data <- jsonlite::fromJSON(full_path, simplifyVector = TRUE)
    # Extract the vector from the animal$individualsPerInstar path
    individuals <- data[["animal"]][["individualsPerInstar"]]
    if (is.null(individuals)) return(0)
    return(length(individuals))
  }, error = function(e) {
    return(0) # Return 0 if the JSON is malformed or does not contain the path
  })
}

adjustment_ids_state <- reactiveValues()
next_adjustment_id_state <- reactiveValues()
registered_add_handlers <- reactiveVal(character(0))
registered_remove_handlers <- reactiveVal(character(0))


# =========================================================================
# CENTRALIZED ERROR COLLECTION ENGINE FOR PANEL 3
# =========================================================================
specifics_errors_list <- reactive({
  errors <- list()
  
  selected_ids <- active_selected_ids()
  if (is.null(selected_ids) || length(selected_ids) == 0) {
    return(errors)
  }
  
  catalog <- CURRENT_SCHEMA_CATALOG()
  
  # --- BUILDING THE EXTENDED VIRTUAL CATALOG FOR CSV PARAMETERS ---
  csv_rows <- data.frame(id = character(0), json_path = character(0), type = character(0), 
                         min = numeric(0), max = numeric(0), enum_values = character(0), 
                         schema_file = character(0), is_pattern = logical(0), stringsAsFactors = FALSE)
  
  if ("csv_param_ontogenetic_links_preference" %in% selected_ids) {
    csv_rows <- rbind(csv_rows, data.frame(id = "csv_param_ontogenetic_links_preference", json_path = "species/ontogeneticLinksPreference.csv", 
                                           type = "csv", min = 0, max = 1, enum_values = "", schema_file = "ontogeneticLinksPreference.csv", is_pattern = FALSE))
  }
  if ("csv_param_ontogenetic_links_profitability" %in% selected_ids) {
    csv_rows <- rbind(csv_rows, data.frame(id = "csv_param_ontogenetic_links_profitability", json_path = "species/ontogeneticLinksProfitability.csv", 
                                           type = "csv", min = 0, max = 1, enum_values = "", schema_file = "ontogeneticLinksProfitability.csv", is_pattern = FALSE))
  }
  
  # Combine the Schema catalog with the virtual CSV rows
  all_valid_params <- if (nrow(catalog) > 0) rbind(catalog, csv_rows) else csv_rows
  selected_ids <- unique(selected_ids[selected_ids %in% all_valid_params$id])
  if (length(selected_ids) == 0) return(errors)
  
  selected_params <- all_valid_params[match(selected_ids, all_valid_params$id), , drop = FALSE]
  selected_params <- selected_params[!is.na(selected_params$id), , drop = FALSE]
  
  for (i in seq_len(nrow(selected_params))) {
    p_row <- selected_params[i, , drop = FALSE]
    param_id <- p_row$id[[1]]
    json_path <- p_row$json_path[[1]]
    schema_file <- p_row$schema_file[[1]]
    param_type <- p_row$type[[1]]
    enum_values <- p_row$enum_values[[1]]
    
    param_class <- resolve_parameter_class(param_type, json_path, schema_file, enum_values)
    
    adjustment_ids <- adjustment_ids_state[[param_id]] %||% 1L
    adjustment_ids <- as.integer(adjustment_ids)
    
    all_destinations <- resolve_config_destinations_by_schema(schema_file, paths$base_config_path)
    
    for (idx in seq_along(adjustment_ids)) {
      adj_id <- adjustment_ids[[idx]]
      dest_input_id <- paste0("destinations_", param_id, "_", adj_id)
      min_input_id <- paste0("min_", param_id, "_", adj_id)
      max_input_id <- paste0("max_", param_id, "_", adj_id)
      elements_input_id <- paste0("elements_", param_id, "_", adj_id)
      
      selected_current <- input[[dest_input_id]] %||% character(0)
      min_current <- input[[min_input_id]]
      max_current <- input[[max_input_id]]
      elements_current <- input[[elements_input_id]]
      
      other_adjustments <- setdiff(adjustment_ids, adj_id)
      selected_by_others <- unique(unlist(lapply(other_adjustments, function(o_id) {
        input[[paste0("destinations_", param_id, "_", o_id)]] %||% character(0)
      })))
      remaining_in_param <- length(setdiff(all_destinations, selected_by_others))
      
      error_key_dest <- paste0(json_path, " [Adjustment ", idx, " - Destinations]")
      error_key_range <- paste0(json_path, " [Adjustment ", idx, " - Range]")
      error_key_perm <- paste0(json_path, " [Adjustment ", idx, " - Elements]")
      
      # 1. Destinations audit (runs ONLY if the parameter requires destinations)
      if (!param_class %in% c("Preference", "Profitability") && length(selected_current) == 0) {
        if (remaining_in_param == 0) {
          errors[[error_key_dest]] <- "This adjustment is empty and there are no remaining destinations available."
        } else {
          # Custom message depending on the parameter type
          if (param_class == "Abundances") {
            errors[[error_key_dest]] <- "The species destination cannot be empty. Please assign exactly one target file."
          } else {
            errors[[error_key_dest]] <- "The destinations field cannot be empty. Please assign at least one target file."
          }
        }
      }
      
      # 2. Numeric range audit (min <= max)
      if (param_class %in% c("RealValued", "Integer", "Abundances", "Preference", "Profitability")) {
        if (is.numeric(min_current) && is.numeric(max_current) && !is.na(min_current) && !is.na(max_current)) {
          if (min_current > max_current) {
            errors[[error_key_range]] <- "Error: The minimum boundary value cannot be greater than the maximum boundary value."
          }
        }
      }
      
      # 3. PERMUTATIONS audit (fed by the JavaScript focusout event)
      if (param_class == "Permutation") {
        status_key <- paste0("permutation_status_", param_id, "_", adj_id)
        
        if (!is.null(input[[status_key]])) {
          if (!input[[status_key]]) {
            errors[[error_key_perm]] <- "Error: The selected elements list cannot be empty. You must mark at least one option."
          }
        } else if (!is.null(elements_current) && length(elements_current) == 0) {
          # Preventive fallback validation at startup
          errors[[error_key_perm]] <- "Error: The selected elements list cannot be empty. You must mark at least one option."
        }
      }

      # 4. ABUNDANCES mathematical audit (Sum == 1.0)
      if (param_class == "Abundances" && length(selected_current) > 0) {
        target_dest <- selected_current[[1]]
        instar_count <- get_instar_count_from_destination(target_dest, paths$base_config_path)
        
        if (instar_count > 0) {
          densities_vals <- numeric(instar_count)
          is_any_empty <- FALSE
          
          for (v_idx in seq_len(instar_count)) {
            sub_id <- paste0("density_elem_", param_id, "_", adj_id, "_", v_idx)
            val_input <- input[[sub_id]]
            if (is.null(val_input) || is.na(val_input)) {
              is_any_empty <- TRUE
            } else {
              densities_vals[v_idx] <- val_input
            }
          }
          
          error_key_abundance <- paste0(json_path, " [Adjustment ", idx, " - Densities Sum]")
          
          if (is_any_empty) {
            errors[[error_key_abundance]] <- "Error: All instar density fields must be filled with a valid numeric value."
          } else {
            total_sum <- sum(densities_vals)
            # Small tolerance for standard floating point arithmetic (1e-5)
            if (abs(total_sum - 1.0) > 1e-5) {
              errors[[error_key_abundance]] <- paste0("Error: The sum of the density vector coefficients is ", round(total_sum, 4), ". It must equal exactly 1.0. Click the 'Normalize' button to fix this automatically.")
            }
          }
        }
      }

      # 5. Preference parameters audit (Preference - errors independent of instars)
      if (param_class %in% c("Preference", "Profitability")) {
        pred_ins_id <- paste0("pref_predator_instars_", param_id, "_", adj_id)
        prey_ins_id <- paste0("pref_prey_instars_", param_id, "_", adj_id)
        
        error_key_pred_base <- paste0(json_path, " [Adjustment ", idx, " - Predator Instars]")
        error_key_prey_base  <- paste0(json_path, " [Adjustment ", idx, " - Prey Instars]")
        
        # Validate empty Predator
        pred_status_key <- paste0(pred_ins_id, "_status")
        if (!is.null(input[[pred_status_key]])) {
          if (!input[[pred_status_key]]$valid) {
            errors[[error_key_pred_base]] <- "The predator instars selection list cannot be empty. Please mark at least one lifecycle stage."
          }
        } else if (length(input[[pred_ins_id]]) == 0) {
          errors[[error_key_pred_base]] <- "The predator instars selection list cannot be empty. Please mark at least one lifecycle stage."
        }
        
        # Validate empty Prey
        prey_status_key <- paste0(prey_ins_id, "_status")
        if (!is.null(input[[prey_status_key]])) {
          if (!input[[prey_status_key]]$valid) {
            errors[[error_key_prey_base]] <- "The prey instars selection list cannot be empty. Please mark at least one lifecycle stage."
          }
        } else if (length(input[[prey_ins_id]]) == 0) {
          errors[[error_key_prey_base]] <- "The prey instars selection list cannot be empty. Please mark at least one lifecycle stage."
        }
      }
    }
  }

  # =========================================================================
  # CORRECTED GLOBAL AUDIT: FEASIBILITY CHECK WITH IMMUTABLE CELLS
  # =========================================================================
  if (any(selected_params$schema_file %in% c("ontogeneticLinksPreference.csv")) && length(errors) == 0) {
    tryCatch({
      csv_path <- file.path(paths$base_config_path, "species", "ontogeneticLinksPreference.csv")
      
      if (file.exists(csv_path)) {
        preference_data <- read.csv(csv_path, header = TRUE, row.names = 1, sep = ",", check.names = FALSE)
        colums <- colnames(preference_data)
        rows <- rownames(preference_data)
        
        # 1. Rebuild the virtual groups defined by the user in the UI
        pref_groups_ui <- list()
        
        for (i in seq_len(nrow(selected_params))) {
          p_row <- selected_params[i, , drop = FALSE]
          if (!p_row$schema_file[1] %in% c("ontogeneticLinksPreference.csv")) next
          
          param_id <- p_row$id[1]
          adjustment_ids <- adjustment_ids_state[[param_id]] %||% 1L
          
          for (idx in seq_along(adjustment_ids)) {
            adj_id <- adjustment_ids[[idx]]
            
            min_val <- input[[paste0("min_", param_id, "_", adj_id)]] %||% p_row$min[1]
            max_val <- input[[paste0("max_", param_id, "_", adj_id)]] %||% p_row$max[1]
            
            pred_sp <- input[[paste0("pref_predator_sp_", param_id, "_", adj_id)]]
            pred_ins <- input[[paste0("pref_predator_instars_", param_id, "_", adj_id)]]
            prey_sp <- input[[paste0("pref_prey_sp_", param_id, "_", adj_id)]]
            prey_ins <- input[[paste0("pref_prey_instars_", param_id, "_", adj_id)]]
            
            group_name <- paste0(param_id, "_adj_", adj_id)
            
            # Replicar exactamente la estructura de celdas
            cells_list <- list()
            columns_values <- paste(pred_sp, pred_ins, sep = "$")
            rows_values <- paste(prey_sp, prey_ins, sep = "$")
            
            for (r in rows_values) {
              for (c in columns_values) {
                cells_list[[length(cells_list) + 1]] <- c(r, c) # Vector c(Row, Col)
              }
            }
            
            pref_groups_ui[[group_name]] <- list(
              cells = cells_list,
              min = min_val,
              max = max_val
            )
          }
        }
        
        num_groups <- length(pref_groups_ui)
        num_cols <- ncol(preference_data)

        if (num_groups > 0) {
          # 1.b Strict check: a cell cannot belong to more than one group
          cell_to_groups <- list()
          for (g_name in names(pref_groups_ui)) {
            for (cell in pref_groups_ui[[g_name]]$cells) {
              key <- paste(cell[1], cell[2], sep = "||")
              current_groups <- cell_to_groups[[key]] %||% character(0)
              cell_to_groups[[key]] <- unique(c(current_groups, g_name))
            }
          }

          overlap_keys <- names(Filter(function(groups) length(groups) > 1, cell_to_groups))
          if (length(overlap_keys) > 0) {
            overlap_preview <- paste(utils::head(overlap_keys, 5), collapse = ", ")
            errors[["Preference Groups Overlap"]] <- paste0(
              "Invalid preference setup: grouped cells cannot overlap. A cell must belong to only one optimization group. ",
              "Overlapping cells detected (row||column): ", overlap_preview,
              if (length(overlap_keys) > 5) " ..." else ""
            )
          }
        }

        if (num_groups > 0 && length(errors) == 0) {
          conflict_details <- ""

          for (j in seq_along(colums)) {
            fixed_sum <- 0
            min_required <- 0
            max_possible <- 0

            for (i in seq_along(rows)) {
              cell_coords <- c(rows[i], colums[j])
              cell_group_match <- FALSE

              for (g in seq_along(pref_groups_ui)) {
                g_name <- names(pref_groups_ui)[[g]]
                match_cell <- any(sapply(pref_groups_ui[[g_name]]$cells, function(cell) {
                  return(cell[1] == cell_coords[1] && cell[2] == cell_coords[2])
                }))

                if (match_cell) {
                  # Optimized per-cell sum (if the group has N cells in the column,
                  # its min/max is counted N times)
                  min_required <- min_required + pref_groups_ui[[g_name]]$min
                  max_possible <- max_possible + pref_groups_ui[[g_name]]$max
                  cell_group_match <- TRUE
                  break
                }
              }

              if (!cell_group_match) {
                fixed_sum <- fixed_sum + preference_data[rows[i], colums[j]]
              }
            }

            if (fixed_sum > 1.0 + 1e-9) {
              conflict_details <- paste0(
                "Predator column '", colums[j], "' has immutable values summing ", round(fixed_sum, 6),
                ", which already exceeds the 1.0 limit."
              )
              break
            }

            avail_space <- 1.0 - fixed_sum

            if (min_required > avail_space + 1e-5) {
              conflict_details <- paste0(
                "Predator column '", colums[j], "' has immutable values summing ", round(fixed_sum, 4),
                ", leaving a free space of ", round(avail_space, 4),
                ". However, your optimization boundaries require a minimum sum of ", round(min_required, 4), "."
              )
              break
            }

            if (max_possible < avail_space - 1e-5) {
              conflict_details <- paste0(
                "Predator column '", colums[j], "' requires its optimized cells to sum exactly ", round(avail_space, 4),
                " (to complete the 1.0 limit alongside immutable cells), but the maximum boundary allowed by your adjustments is only ",
                round(max_possible, 4), "."
              )
              break
            }
          }

          if (nzchar(conflict_details)) {
            errors[["Preference System Constraints"]] <- paste0(
              "Unsolvable algebraic configuration detected. The immutable cells in the CSV combined with your dynamic boundaries leave no shared viable solution. Detail: ",
              conflict_details
            )
          }
        }
      }
    }, error = function(e) {
      errors[["Preference System Constraints"]] <- paste0("Mathematical analysis failed due to an internal matrix error: ", e$message)
    })
  }
  
  if (any(selected_params$schema_file %in% c("ontogeneticLinksProfitability.csv")) && length(errors) == 0) {
    tryCatch({
      profitability_groups_ui <- list()

      for (i in seq_len(nrow(selected_params))) {
        p_row <- selected_params[i, , drop = FALSE]
        if (!p_row$schema_file[1] %in% c("ontogeneticLinksProfitability.csv")) next

        param_id <- p_row$id[1]
        adjustment_ids <- adjustment_ids_state[[param_id]] %||% 1L

        for (idx in seq_along(adjustment_ids)) {
          adj_id <- adjustment_ids[[idx]]

          pred_sp <- input[[paste0("pref_predator_sp_", param_id, "_", adj_id)]]
          pred_ins <- input[[paste0("pref_predator_instars_", param_id, "_", adj_id)]]
          prey_sp <- input[[paste0("pref_prey_sp_", param_id, "_", adj_id)]]
          prey_ins <- input[[paste0("pref_prey_instars_", param_id, "_", adj_id)]]

          group_name <- paste0(param_id, "_adj_", adj_id)
          cells_list <- list()
          columns_values <- paste(pred_sp, pred_ins, sep = "$")
          rows_values <- paste(prey_sp, prey_ins, sep = "$")

          for (r in rows_values) {
            for (c in columns_values) {
              cells_list[[length(cells_list) + 1]] <- c(r, c)
            }
          }

          profitability_groups_ui[[group_name]] <- list(cells = cells_list)
        }
      }

      if (length(profitability_groups_ui) > 0) {
        cell_to_groups <- list()

        for (g_name in names(profitability_groups_ui)) {
          for (cell in profitability_groups_ui[[g_name]]$cells) {
            key <- paste(cell[1], cell[2], sep = "||")
            current_groups <- cell_to_groups[[key]] %||% character(0)
            cell_to_groups[[key]] <- unique(c(current_groups, g_name))
          }
        }

        overlap_keys <- names(Filter(function(groups) length(groups) > 1, cell_to_groups))
        if (length(overlap_keys) > 0) {
          overlap_preview <- paste(utils::head(overlap_keys, 5), collapse = ", ")
          errors[["Profitability Groups Overlap"]] <- paste0(
            "Invalid profitability setup: grouped cells cannot overlap. A cell must belong to only one optimization group. ",
            "Overlapping cells detected (row||column): ", overlap_preview,
            if (length(overlap_keys) > 5) " ..." else ""
          )
        }
      }
    }, error = function(e) {
      errors[["Profitability System Constraints"]] <- paste0("Profitability analysis failed due to an internal error: ", e$message)
    })
  }

  return(errors)
})

# Flexible boolean expression to quickly check the Panel 3 state
specifics_has_errors <- reactive({
  return(length(specifics_errors_list()) > 0)
})

# =========================================================================
# VISUAL RENDERING OF THE RED ALERT BOX (BOOTSTRAP 5) - PANEL 3
# =========================================================================
output$specifics_errors_container <- renderUI({
  err_list <- specifics_errors_list()
  
  # If the list is empty, inject nothing at all into the HTML
  if (length(err_list) == 0) return(NULL)
  
  # Build the HTML bullet list mapping the parameter and its specific message
  error_items_ui <- lapply(names(err_list), function(param_label) {
    tags$li(
      style = "margin-bottom: 4px;",
      tags$b(paste0(param_label, ": ")), # Shows "Path$Parameter [Adjustment X]: " in bold
      tags$span(err_list[[param_label]])  # Mensaje de error detallado
    )
  })
  
  # Return the structured container using native Bootstrap 5 styling
  div(
    class = "alert alert-danger shadow-sm mb-4",
    role = "alert",
    style = "border-left: 5px solid #df4759; border-radius: 6px;",
    
    div(class = "d-flex align-items-center mb-2",
        icon("triangle-exclamation", class = "fa-lg me-2", style = "color: #842029;"),
        tags$h5(class = "alert-heading mb-0 fw-bold", "Adjustment Configurations Errors")
    ),
    
    p(class = "small text-muted mb-3 italic", 
      icon("ban"), " Review the dynamic configurations below. Every adjustment must target at least one configuration file before execution."),
    
    tags$ul(style = "padding-left: 20px; margin-bottom: 0;", error_items_ui)
  )
})


observeEvent(selection_reset_nonce(), {
  current_adjustment_keys <- names(reactiveValuesToList(adjustment_ids_state))
  for (k in current_adjustment_keys) {
    adjustment_ids_state[[k]] <- NULL
  }

  current_next_keys <- names(reactiveValuesToList(next_adjustment_id_state))
  for (k in current_next_keys) {
    next_adjustment_id_state[[k]] <- NULL
  }

  registered_add_handlers(character(0))
  registered_remove_handlers(character(0))
}, ignoreInit = TRUE)

build_adjustment_editor <- function(param_id, adjustment_id, display_index, adjustment_ids, param_class, enum_values, min_val, max_val, all_destinations) {
  destination_input_id <- paste0("destinations_", param_id, "_", adjustment_id)
  selected_current <- input[[destination_input_id]] %||% character(0)

  other_adjustments <- setdiff(adjustment_ids, adjustment_id)
  selected_by_others <- unique(unlist(lapply(other_adjustments, function(other_id) {
    input[[paste0("destinations_", param_id, "_", other_id)]] %||% character(0)
  })))

  available_choices <- unique(c(setdiff(all_destinations, selected_by_others), selected_current))
  is_invalid_status <- (length(selected_current) == 0)

  # Specific rule for Abundances: force a single selection
  is_multiple <- (param_class != "Abundances")
  placeholder_text <- if (param_class == "Abundances") {
    "Search and select exactly one target species file"
  } else {
    "Search destinations and click to add"
  }
  
  selectize_plugins <- if (is_multiple) list("remove_button") else list()

  # Identifier for the adjustment name field
  name_input_id <- paste0("name_", param_id, "_", adjustment_id)
  
  # PERSISTENCE SOLUTION WITH UNDERSCORE REPLACEMENT:
  # If it already exists in the session, sanitize it by replacing any whitespace with "_"
  current_name_value <- if (name_input_id %in% names(input)) {
    gsub("[ \t\r\n]+", "_", input[[name_input_id]])
  } else {
    ""
  }

  specific_fields <- list()

  # A. If the parameter handles numeric ranges, inject its isolated dynamic container
  if (param_class %in% c("RealValued", "Integer", "Abundances", "Preference", "Profitability")) {
    status_key <- paste0("range_status_", param_id, "_", adjustment_id)
    
    # 1. Retrieve the real-time input values if they already exist in the session
    min_input_id <- paste0("min_", param_id, "_", adjustment_id)
    max_input_id <- paste0("max_", param_id, "_", adjustment_id)
    
    min_current <- input[[min_input_id]] %||% min_val
    max_current <- input[[max_input_id]] %||% max_val
    
    # 2. Backend error evaluation (guarantees absolute consistency with the box above)
    has_range_error_init <- FALSE
    if (!is.null(input[[status_key]])) {
      has_range_error_init <- !input[[status_key]]$valid
    } else if (!is.na(min_current) && !is.na(max_current) && min_current > max_current) {
      has_range_error_init <- TRUE
    }
    
    specific_fields <- c(specific_fields, list(
      tags$div(
        # Dynamically inject the 'is-invalid' class into the container if the ranges are wrong
        class = paste("numeric-range-container", if (has_range_error_init) "is-invalid" else ""),
        `data-param-id` = param_id,
        `data-adj-id` = adjustment_id,
        
        # Isolated dynamic wrapper for the numeric boxes
        uiOutput(paste0("range_ui_", param_id, "_", adjustment_id)),
        
        # Servido directamente por R: Asegura que el texto aparezca abajo sin depender de JS
        tags$div(
          class = "range-error-msg",
          style = if (has_range_error_init) "display: block;" else "display: none;",
          if (has_range_error_init) p(icon("circle-xmark"), "Error: The minimum boundary value cannot be greater than the maximum boundary value.", class = "text-danger small fw-bold text-break mt-2 mb-1")
        )
      )
    ))
  }

  # B. Inject the dynamic container for the specific parameter class
  specific_fields <- c(specific_fields, list(
    uiOutput(paste0("specifics_ui_", param_id, "_", adjustment_id))
  ))

  remove_btn_id <- paste0("remove_adjustment_", param_id, "_", adjustment_id)
  can_remove <- length(adjustment_ids) > 1

  card(
    class = "mb-3",
    card_header(
      div(
        class = "d-flex justify-content-between align-items-center",
        span(paste("Adjustment", display_index)),
        HTML("&nbsp;&nbsp;"),
        if (can_remove) actionButton(remove_btn_id, "Remove", class = "btn-outline-danger btn-sm")
      )
    ),
    card_body(
      # 1. Text input for the adjustment name (Universal)
      div(
        class = "mb-3",
        textInput(
          inputId = name_input_id,
          label = "Adjustment Custom Name:",
          value = current_name_value,
          placeholder = paste0("e.g., Run setup variation ", display_index)
        )
      ),
      
      # 2. The Destinations field is rendered ONLY if it is NOT of type Preference/Profitability
      if (!param_class %in% c("Preference", "Profitability")) {
        tagList(
          tags$div(
            id = paste0("wrapper_destinations_", param_id, "_", adjustment_id),
            class = paste("destinations-selectize-container", if (is_invalid_status) "is-invalid" else ""),
            selectizeInput(
              destination_input_id,
              "Destinations:",
              choices = available_choices,
              selected = selected_current,
              multiple = is_multiple,
              options = list(
                placeholder = placeholder_text,
                plugins = selectize_plugins
              )
            )
          ),
          uiOutput(paste0("error_destinations_", param_id, "_", adjustment_id))
        )
      } else {
        NULL
      },
      
      # 3. Specific fields and numeric intervals (min/max)
      do.call(tagList, specific_fields)
    )
  )
}

output$dynamic_param_editors <- renderUI({
  selected_ids <- active_selected_ids()

  if (is.null(selected_ids) || length(selected_ids) == 0) {
    return(p(class = "text-muted", "Please select one or more target parameters in the Tree View (Tab 2) to configure them here."))
  }

  catalog <- CURRENT_SCHEMA_CATALOG()
  
  # Sync the accordion's visual catalog including virtual CSV parameters
  csv_rows <- data.frame(id = character(0), json_path = character(0), type = character(0), 
                         min = numeric(0), max = numeric(0), enum_values = character(0), 
                         schema_file = character(0), is_pattern = logical(0), stringsAsFactors = FALSE)
  if ("csv_param_ontogenetic_links_preference" %in% selected_ids) {
    csv_rows <- rbind(csv_rows, data.frame(id = "csv_param_ontogenetic_links_preference", json_path = "species/ontogeneticLinksPreference.csv", 
                                           type = "csv", min = 0, max = 1, enum_values = "", schema_file = "ontogeneticLinksPreference.csv", is_pattern = FALSE))
  }
  if ("csv_param_ontogenetic_links_profitability" %in% selected_ids) {
    csv_rows <- rbind(csv_rows, data.frame(id = "csv_param_ontogenetic_links_profitability", json_path = "species/ontogeneticLinksProfitability.csv", 
                                           type = "csv", min = 0, max = 1, enum_values = "", schema_file = "ontogeneticLinksProfitability.csv", is_pattern = FALSE))
  }
  
  all_valid_params <- if (nrow(catalog) > 0) rbind(catalog, csv_rows) else csv_rows
  selected_ids <- unique(selected_ids[selected_ids %in% all_valid_params$id])
  if (length(selected_ids) == 0) return(NULL)

  selected_params <- all_valid_params[match(selected_ids, all_valid_params$id), , drop = FALSE]
  selected_params <- selected_params[!is.na(selected_params$id), , drop = FALSE]

  if (!"enum_values" %in% names(selected_params)) {
    selected_params$enum_values <- ""
  }

  for (sid in selected_params$id) {
    if (is.null(adjustment_ids_state[[sid]])) {
      adjustment_ids_state[[sid]] <- 1L
      next_adjustment_id_state[[sid]] <- 2L
    }
  }

  param_ui_list <- lapply(seq_len(nrow(selected_params)), function(i) {
    p_row <- selected_params[i, , drop = FALSE]
    param_id <- p_row$id[[1]]
    json_path <- p_row$json_path[[1]]
    schema_file <- p_row$schema_file[[1]]
    param_type <- p_row$type[[1]]
    enum_values <- p_row$enum_values[[1]]

    param_class <- resolve_parameter_class(param_type, json_path, schema_file, enum_values)

    min_val <- if (is.na(p_row$min[[1]])) 0 else p_row$min[[1]]
    max_val <- if (is.na(p_row$max[[1]])) 1 else p_row$max[[1]]

    all_destinations <- resolve_config_destinations_by_schema(schema_file, paths$base_config_path)

    add_btn_id <- paste0("add_adjustment_", param_id)
    adjustments_ui_id <- paste0("adjustments_list_", param_id)

    if (is.null(adjustment_ids_state[[param_id]])) {
      adjustment_ids_state[[param_id]] <- 1L
      next_adjustment_id_state[[param_id]] <- 2L
    }

    registered_add <- registered_add_handlers()
    if (!param_id %in% registered_add) {
      observeEvent(input[[add_btn_id]], {
        current_ids <- adjustment_ids_state[[param_id]] %||% 1L
        selected_union <- unique(unlist(lapply(current_ids, function(adj_id) {
          input[[paste0("destinations_", param_id, "_", adj_id)]] %||% character(0)
        })))

        remaining_destinations <- setdiff(all_destinations, selected_union)
        if (length(remaining_destinations) == 0) return()

        new_id <- next_adjustment_id_state[[param_id]] %||% (max(current_ids) + 1L)
        adjustment_ids_state[[param_id]] <- c(current_ids, as.integer(new_id))
        next_adjustment_id_state[[param_id]] <- as.integer(new_id) + 1L
      }, ignoreInit = TRUE)

      registered_add_handlers(c(registered_add, param_id))
    }

    output[[adjustments_ui_id]] <- renderUI({
      adjustment_ids <- adjustment_ids_state[[param_id]] %||% 1L
      adjustment_ids <- as.integer(adjustment_ids)

      selected_union <- unique(unlist(lapply(adjustment_ids, function(adj_id) {
        input[[paste0("destinations_", param_id, "_", adj_id)]] %||% character(0)
      })))
      remaining_destinations <- setdiff(all_destinations, selected_union)

      for (adj_id in adjustment_ids) {
        remove_btn_id <- paste0("remove_adjustment_", param_id, "_", adj_id)
        handler_key <- paste0(param_id, "::", adj_id)
        registered_remove <- registered_remove_handlers()

        if (!handler_key %in% registered_remove) {
          local({
            param_id_local <- param_id
            adj_id_local <- adj_id
            observeEvent(input[[remove_btn_id]], {
              current_ids <- adjustment_ids_state[[param_id_local]] %||% 1L
              if (length(current_ids) > 1) {
                adjustment_ids_state[[param_id_local]] <- setdiff(current_ids, adj_id_local)
              }
            }, ignoreInit = TRUE)
          })

          registered_remove_handlers(c(registered_remove, handler_key))
        }
      }

      editors <- lapply(seq_along(adjustment_ids), function(idx) {
        adj_id <- adjustment_ids[[idx]]
        
        dest_input_id <- paste0("destinations_", param_id, "_", adj_id)
        min_input_id <- paste0("min_", param_id, "_", adj_id)
        max_input_id <- paste0("max_", param_id, "_", adj_id)
        
        # Class-specific component identifiers
        elements_input_id <- paste0("elements_", param_id, "_", adj_id)
        value_input_id <- paste0("value_", param_id, "_", adj_id)
        values_input_id <- paste0("values_", param_id, "_", adj_id)
        densities_input_id <- paste0("densitiesVector_", param_id, "_", adj_id)
        
        error_ui_id <- paste0("error_destinations_", param_id, "_", adj_id)
        range_ui_id <- paste0("range_ui_", param_id, "_", adj_id)
        specifics_ui_id <- paste0("specifics_ui_", param_id, "_", adj_id)
        wrapper_id <- paste0("wrapper_destinations_", param_id, "_", adj_id)
        
        # A. RENDER PARA ERROR DE DESTINATIONS (Selectize)
        output[[error_ui_id]] <- renderUI({
          selected_current <- input[[dest_input_id]] %||% character(0)
          other_adjustments <- setdiff(adjustment_ids, adj_id)
          selected_by_others <- unique(unlist(lapply(other_adjustments, function(o_id) {
            input[[paste0("destinations_", param_id, "_", o_id)]] %||% character(0)
          })))
          remaining_in_param <- length(setdiff(all_destinations, selected_by_others))
          
          if (length(selected_current) == 0) {
            shinyjs::addClass(id = wrapper_id, class = "is-invalid")
            if (remaining_in_param == 0) {
              msg <- "Error: This adjustment is empty and there are no remaining destinations available in the parameter."
            } else {
              msg <- "Error: The destinations field cannot be empty. Please assign at least one target file."
            }
            return(p(icon("circle-xmark"), msg, class = "text-danger small fw-bold text-break mt-1 mb-2"))
          } else {
            shinyjs::removeClass(id = wrapper_id, class = "is-invalid")
            return(NULL)
          }
        })

        # B.1. ISOLATED RENDER FOR NUMERIC INPUTS (RealValued / Integer / Abundances)
        output[[range_ui_id]] <- renderUI({
          min_current <- input[[min_input_id]] %||% min_val
          max_current <- input[[max_input_id]] %||% max_val
          
          status_key <- paste0("range_status_", param_id, "_", adj_id)
          has_range_error_curr <- FALSE
          if (!is.null(input[[status_key]])) {
            has_range_error_curr <- !input[[status_key]]$valid
          } else if (!is.na(min_current) && !is.na(max_current) && min_current > max_current) {
            has_range_error_curr <- TRUE
          }
          
          # Preference uses decimal input boxes identical to RealValued
          input_class <- if (param_class %in% c("Integer", "Abundances")) "integer-input" else "decimal-input"
          bounded_unit_interval <- param_class %in% c("Preference", "Profitability")
          min_attr <- if (param_class == "Abundances" || bounded_unit_interval) 0 else NA
          max_attr <- if (bounded_unit_interval) 1 else NA
          
          layout_columns(
            col_widths = c(6, 6),
            tags$div(
              id = paste0("wrapper_min_", param_id, "_", adj_id),
              class = if (has_range_error_curr) "is-invalid-input" else "",
              div(class = input_class, numericInput(min_input_id, "min:", value = min_current, min = min_attr, max = max_attr))
            ),
            tags$div(
              id = paste0("wrapper_max_", param_id, "_", adj_id),
              class = if (has_range_error_curr) "is-invalid-input" else "",
              div(class = input_class, numericInput(max_input_id, "max:", value = max_current, min = min_attr, max = max_attr))
            )
          )
        })

        # B.2. ISOLATED RENDER FOR PARTICULAR COMPONENTS (Permutation, Array, String, Abundances)
        # Based faithfully on the isolation and persistence of the numeric render
        output[[specifics_ui_id]] <- renderUI({
          choices_from_schema <- if (nzchar(enum_values)) unlist(strsplit(enum_values, "\\|")) else character(0)
          
          if (param_class %in% c("Preference", "Profitability")) {
            # 1. Load fresh metadata from the system folders
            species_catalog  <- get_species_metadata_catalog(paths$base_config_path)
            resource_catalog <- get_resource_metadata_catalog(paths$base_config_path)
            
            if (length(species_catalog) == 0) {
              return(p(class = "text-muted italic small mt-2", 
                       icon("info-circle"), " Please set up a valid species folder configuration to load preference structures."))
            }
            
            animal_names <- names(species_catalog)
            all_prey_names <- c(animal_names, names(resource_catalog))
            
            pred_sp_id <- paste0("pref_predator_sp_", param_id, "_", adj_id)
            pred_ins_id <- paste0("pref_predator_instars_", param_id, "_", adj_id)
            prey_sp_id <- paste0("pref_prey_sp_", param_id, "_", adj_id)
            prey_ins_id <- paste0("pref_prey_instars_", param_id, "_", adj_id)
            
            selected_pred_sp <- input[[pred_sp_id]] %||% animal_names[[1]]
            selected_prey_sp <- input[[prey_sp_id]] %||% all_prey_names[[1]]
            
            pred_max_instars <- if (selected_pred_sp %in% animal_names) species_catalog[[selected_pred_sp]]$instar_count else 1
            prey_max_instars <- if (selected_prey_sp %in% animal_names) {
              species_catalog[[selected_prey_sp]]$instar_count
            } else if (selected_prey_sp %in% names(resource_catalog)) {
              resource_catalog[[selected_prey_sp]]$instar_count
            } else {
              1
            }
            
            selected_pred_ins <- if (pred_ins_id %in% names(input)) input[[pred_ins_id]] else seq_len(pred_max_instars)
            selected_prey_ins <- if (prey_ins_id %in% names(input)) input[[prey_ins_id]] else seq_len(prey_max_instars)
            
            # --- INDEPENDENT LOCAL ERROR EVALUATION (synced with the JS blur event) ---
            pred_status_key <- paste0(pred_ins_id, "_status")
            has_pred_error <- FALSE
            if (!is.null(input[[pred_status_key]])) {
              has_pred_error <- !input[[pred_status_key]]$valid
            } else if (length(selected_pred_ins) == 0) {
              has_pred_error <- TRUE
            }
            
            prey_status_key <- paste0(prey_ins_id, "_status")
            has_prey_error <- FALSE
            if (!is.null(input[[prey_status_key]])) {
              has_prey_error <- !input[[prey_status_key]]$valid
            } else if (length(selected_prey_ins) == 0) {
              has_prey_error <- TRUE
            }
            
            return(
              tags$div(
                class = "mt-3 p-3 border rounded bg-light preference-cell-container",
                tags$label(class = "control-label fw-bold mb-2", paste0(param_class, " Interactive Cell Matrix Mapping (PreferenceCell):")),
                p(class = "text-muted small mb-3", "Define the interaction bounds between the selected predator trophic levels and their prey targets:"),
                
                layout_columns(
                  col_widths = c(6, 6),
                  
                  # --- SECTION A: PREDATOR ---
                  div(
                    class = "p-3 border rounded bg-white shadow-sm",
                    style = "border-left: 4px solid #2c3e50 !important;",
                    tags$h6(class = "fw-bold text-dark mb-3", icon("skull"), " Predator Specification"),
                    
                    selectInput(pred_sp_id, "Select Predator Species:", choices = animal_names, selected = selected_pred_sp),
                    
                    # Dynamic container that injects the red border into the Picker via CSS
                    div(
                      class = if (has_pred_error) "picker-invalid-container" else "",
                      pickerInput(
                        inputId = pred_ins_id,
                        label = "Target Predator Instars:",
                        choices = seq_len(pred_max_instars),
                        selected = selected_pred_ins,
                        multiple = TRUE,
                        options = pickerOptions(
                          actionsBox = TRUE,
                          noneSelectedText = "No instars selected (Cell inactive)"
                        )
                      )
                    ),
                    tags$div(
                      class = "pref-error-msg",
                      style = if (has_pred_error) "display: block;" else "display: none;",
                      if (has_pred_error) p(icon("circle-xmark"), "Error: You must select at least one predator instar stage.", class = "text-danger small fw-bold text-break mt-2 mb-0")
                    )
                  ),
                  
                  # --- SECTION B: PREY ---
                  div(
                    class = "p-3 border rounded bg-white shadow-sm",
                    style = "border-left: 4px solid #18bc9c !important;",
                    tags$h6(class = "fw-bold text-dark mb-3", icon("shrimp"), " Prey Target Specification"),
                    
                    selectInput(prey_sp_id, "Select Prey (Animal or Resource):", choices = all_prey_names, selected = selected_prey_sp),
                    
                    div(
                      class = if (has_prey_error) "picker-invalid-container" else "",
                      pickerInput(
                        inputId = prey_ins_id,
                        label = "Target Prey Instars:",
                        choices = seq_len(prey_max_instars),
                        selected = selected_prey_ins,
                        multiple = TRUE,
                        options = pickerOptions(
                          actionsBox = TRUE,
                          noneSelectedText = "No instars selected (Cell inactive)"
                        )
                      )
                    ),
                    tags$div(
                      class = "pref-error-msg",
                      style = if (has_prey_error) "display: block;" else "display: none;",
                      if (has_prey_error) p(icon("circle-xmark"), "Error: You must select at least one prey instar stage.", class = "text-danger small fw-bold text-break mt-2 mb-0")
                    )
                  )
                )
              )
            )
          }

          if (param_class == "Permutation") {
            
            # DEFINITIVE PERSISTENCE SOLUTION:
            # We check whether the input ID already exists in the global list of Shiny session inputs.
            # - If it does NOT exist in names(input), it means this is the FIRST render -> mark all (choices_from_schema).
            # - If it DOES exist in names(input), faithfully respect whatever it returns (if empty, it will return NULL or character(0) and stay empty).
            elements_current <- if (elements_input_id %in% names(input)) {
              input[[elements_input_id]] # Will legitimately return character(0) or NULL once emptied
            } else {
              choices_from_schema # Mandatory initial load with everything marked
            }
            
            status_key <- paste0("permutation_status_", param_id, "_", adj_id)
            has_perm_error <- FALSE
            if (!is.null(input[[status_key]])) {
              has_perm_error <- !input[[status_key]]
            }
            
            return(
              tags$div(
                id = paste0("wrapper_elements_", param_id, "_", adj_id),
                class = paste("permutation-buttons-container", if (has_perm_error) "is-invalid" else ""),
                `data-param-id` = param_id,
                `data-adj-id` = adj_id,
                checkboxGroupButtons(
                  inputId = elements_input_id,
                  label = "Target Elements Selection (Click to toggle activate/deactivate):",
                  choices = choices_from_schema,
                  selected = elements_current,
                  justified = TRUE,
                  individual = FALSE,
                  checkIcon = list(
                    yes = icon("check", class = "me-1"),
                    no = icon("circle", class = "me-1", style = "opacity: 0.15;")
                  ),
                  status = "primary"
                ),
                # Static container for JS error injections on blur
                tags$div(
                  class = "permutation-error-msg",
                  style = if (has_perm_error) "display: block;" else "display: none;",
                  if (has_perm_error) p(icon("circle-xmark"), "Error: You must select at least one element from the list.", class = "text-danger small fw-bold text-break mt-2 mb-1")
                )
              )
            )
          }
          
          if (param_class == "String") {
            value_current <- input[[value_input_id]] %||% ""
            return(textInput(value_input_id, "value:", value = value_current))
          }
          
          if (param_class == "Array") {
            values_current <- input[[values_input_id]] %||% "[]"
            return(textAreaInput(values_input_id, "values (JSON array):", value = values_current, rows = 3))
          }
          
          if (param_class == "Abundances") {
            target_dest <- input[[dest_input_id]] %||% ""
            instar_count <- get_instar_count_from_destination(target_dest, paths$base_config_path)
            
            if (instar_count == 0) {
              return(p(class = "text-muted italic small mt-2", 
                       icon("info-circle"), " Please select a valid target species file above to load its instar configuration structure."))
            }
            
            # 1. Retrieve current values and compute the real-time sum
            current_vals <- numeric(instar_count)
            is_any_empty <- FALSE
            for (v_idx in seq_len(instar_count)) {
              val_input <- input[[paste0("density_elem_", param_id, "_", adj_id, "_", v_idx)]]
              if (is.null(val_input) || is.na(val_input)) {
                is_any_empty <- TRUE
              } else {
                current_vals[v_idx] = val_input
              }
            }
            current_sum <- sum(current_vals)
            
            # Evaluate whether the sum is valid
            is_sum_valid <- !is_any_empty && (abs(current_sum - 1.0) < 1e-5)
            badge_class <- if (is_sum_valid) "bg-success" else "bg-warning text-dark"
            
            # Determine the local visual error state
            has_abundance_error <- is_any_empty || !is_sum_valid
            
            error_message_text <- if (is_any_empty) {
              "Error: All instar density fields must be filled with a valid numeric value."
            } else {
              paste0("Error: The sum of the density vector coefficients is ", round(current_sum, 4), ". It must equal exactly 1.0. Click the 'Normalize to 1.0' button to fix this automatically.")
            }
            
            # 2. Dynamically register the normalization button observer if it does not exist
            normalize_btn_id <- paste0("normalize_btn_", param_id, "_", adj_id)
            
            local({
              p_id <- param_id; a_id <- adj_id; i_count <- instar_count; n_btn_id <- normalize_btn_id
              observeEvent(input[[n_btn_id]], {
                vals <- numeric(i_count)
                for (k in seq_len(i_count)) {
                  vals[k] <- input[[paste0("density_elem_", p_id, "_", a_id, "_", k)]] %||% 0.0
                }
                total <- sum(vals)
                norm_vals <- if (total == 0) rep(1 / i_count, i_count) else vals / total
                norm_vals <- round(norm_vals, 4)
                diff_residual <- 1.0 - sum(norm_vals)
                norm_vals[i_count] <- norm_vals[i_count] + diff_residual
                
                for (k in seq_len(i_count)) {
                  updateNumericInput(session, paste0("density_elem_", p_id, "_", a_id, "_", k), value = norm_vals[k])
                }
              }, ignoreInit = TRUE, once = FALSE)
            })
            
            # 3. Build the input grid
            vector_inputs <- lapply(seq_len(instar_count), function(v_idx) {
              sub_input_id <- paste0("density_elem_", param_id, "_", adj_id, "_", v_idx)
              current_val <- input[[sub_input_id]] %||% 0.0
              
              div(
                class = "col",
                div(class = "decimal-input", 
                    numericInput(sub_input_id, label = paste0("Instar ", v_idx), value = current_val, min = 0)
                )
              )
            })
            
            # 4. Render the container panel with dynamic is-invalid classes
            return(
              tags$div(
                # The container will now receive the 'is-invalid' class, causing its entire outline to be drawn in red
                class = paste("mt-3 p-3 border rounded bg-light abundances-vector-container", if (has_abundance_error) "is-invalid" else ""),
                
                div(class = "d-flex justify-content-between align-items-center mb-2",
                    tags$label(class = "control-label fw-bold mb-0", "Densities Vector Configuration (densitiesVector):"),
                    div(
                      tags$span(class = paste("badge me-2 p-2", badge_class), 
                                paste("Current Sum:", round(current_sum, 4))),
                      actionButton(normalize_btn_id, "Normalize to 1.0", class = "btn-outline-secondary btn-sm", icon = icon("wand-magic-sparkles"))
                    )
                ),
                
                p(class = "text-muted small mb-3", 
                  paste("This species defines exactly", instar_count, "instars. The coefficients must sum up to exactly 1.0.")),
                
                div(
                  class = "row row-cols-1 row-cols-sm-3 row-cols-md-5 g-3",
                  do.call(tagList, vector_inputs)
                ),
                
                tags$div(
                  class = "abundance-error-msg",
                  style = if (has_abundance_error) "display: block;" else "display: none;",
                  if (has_abundance_error) p(icon("circle-xmark"), error_message_text, class = "text-danger small fw-bold text-break mt-3 mb-1")
                )
              )
            )
          }
          
          return(NULL)
        })

        build_adjustment_editor(
          param_id = param_id,
          adjustment_id = adj_id,
          display_index = idx,
          adjustment_ids = adjustment_ids,
          param_class = param_class,
          enum_values = enum_values,
          min_val = min_val,
          max_val = max_val,
          all_destinations = all_destinations
        )
      })

      controls <- layout_columns(
        col_widths = c(6, 6),
        if (length(remaining_destinations) > 0) {
          actionButton(add_btn_id, "Add adjustment", class = "btn-outline-primary w-100")
        } else {
          tags$button(type = "button", class = "btn btn-outline-secondary w-100", disabled = "disabled", "No destinations left")
        },
        tags$div(class = "text-muted small pt-2", paste("Remaining destinations:", length(remaining_destinations)))
      )

      if (length(all_destinations) == 0) {
        return(tagList(
          p(class = "text-warning", "No destinations available for this parameter in the selected base configuration."),
          controls,
          do.call(tagList, editors)
        ))
      }

      tagList(
        controls,
        do.call(tagList, editors)
      )
    })

    accordion_panel(
      title = json_path,
      value = param_id,
      tags$div(
        class = "mb-2",
        tags$span("Parameter class: "),
        tags$span(class = "badge bg-info text-dark", param_class)
      ),
      uiOutput(adjustments_ui_id)
    )
  })

  do.call(accordion, c(param_ui_list, multiple = TRUE))
})