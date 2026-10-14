#' Phase 3: Define Parameter Optimisation Boundaries & Values Server (Part 1)
#'
#' @description Handles dynamic field unrolling, complex matrix algebraic constraints validations,
#' and reactive cross-examination of sub-adjustment pools.
#' @param id Unique module namespace identifier string.
#' @param global_store The global reactiveValues store mapping transactional pipeline data metrics.
#' @import shiny
#' @importFrom shinyjs addClass removeClass
#' @export
mod_optimiser_phase_3_server <- function(id, global_store) {
  moduleServer(id, function(input, output, session) {
    ns <- session$ns
    
    # ----------------============================================================
    # UNIFIED CENTRAL ERROR DATA REGISTRY (High-scalability store)
    # ----------------============================================================
    # Replicates the Phase 1 architectural model. Map failures directly into this registry.
    active_errors <- reactiveValues(
      empty_adjustments    = list(),  
      boundary_conflicts   = list(),  
      duplicate_aliases    = list(),
      abundances_sum_error = list(),  
      permutations_empty   = list(),
      predator_instars_errors = list(),
      prey_instars_errors = list(),
      trophic_overlaps_errors = list(),
      preference_csv_errors   = list()
    )
    
    # ----------------============================================================
    # 1. READ-ONLY REACTIVE HOOKS ANCHORED TO GLOBAL CACHE PIPELINES
    # ----------------============================================================
    base_config_path        <- reactive({ global_store$phase_1$system_paths$base_configuration })
    selected_params_catalog <- reactive({ global_store$phase_2$selected_parameters_catalog })
    
    # ----------------============================================================
    # 2. LOCAL STATE REGISTRIES TO TRACK USER ADJUSTMENTS POPULATIONS
    # ----------------============================================================
    adjustment_ids_state       <- reactiveValues()
    next_adjustment_id_state   <- reactiveValues()
    registered_add_handlers    <- reactiveVal(character(0))
    registered_remove_handlers <- reactiveVal(character(0))

    last_known_base_config     <- reactiveVal(NULL)

    # ============================================================================
    # BASE CONFIGURATION CROSS-PHASE AUTOMATIC RESET INTERCEPTOR
    # ============================================================================
    observe({
      current_v1_config <- base_config_path()
      req(!is.null(current_v1_config), current_v1_config != "")
      
      current_local_config <- last_known_base_config()
      
      if (is.null(current_local_config)) {
        # Inicialización de primera entrada: Estampa la ruta actual en memoria local sin vaciar nada
        last_known_base_config(current_v1_config)
      } else if (current_local_config != current_v1_config) {
        
        # CAMBIO DETECTADO: El usuario alteró la Base Config en la Fase 1. Purgamos la Fase 3.
        message("[Phase 3 Supervisor] Upstream base configuration directory changed (", current_local_config, " -> ", current_v1_config, "). Evicting dynamic boundaries memory splits.")
        
        shiny::isolate({
          # 1. Purgamos el contador de ID de los acordeones dinámicos del servidor
          current_adjustment_keys <- names(shiny::reactiveValuesToList(adjustment_ids_state))
          for (k in current_adjustment_keys) {
            adjustment_ids_state[[k]] <- NULL
          }
          
          current_next_keys <- names(shiny::reactiveValuesToList(next_adjustment_id_state))
          for (k in current_next_keys) {
            next_adjustment_id_state[[k]] <- NULL
          }
          
          # 2. Limpiamos los manejadores y registros estáticos de eventos de botones
          registered_add_handlers(character(0))
          registered_remove_handlers(character(0))
          
          # 3. Vaciamos las variables de error para disipar alertas obsoletas
          active_errors$empty_adjustments         <- list()
          active_errors$boundary_conflicts        <- list()
          active_errors$duplicate_all_aliases     <- list()
          active_errors$permutations_empty        <- list()
          active_errors$abundances_sum_error      <- list()
          active_errors$predator_instars_errors <- list()
          active_errors$prey_instars_errors     <- list()
          active_errors$trophic_overlaps_errors <- list()
          active_errors$preference_csv_errors   <- list()
          
          # 4. Vaciamos la memoria del almacén transaccional global
          global_store$phase_3$defined_parameters <- list()
          
          # 5. Actualizamos el puntero local de control con la nueva ruta
          last_known_base_config(current_v1_config)
        })
      }
    })
    
    # ----------------============================================================
    # 3. HELPER FUNCTIONS FOR PARAMETERS TAXONOMY RESOLUTION
    # ----------------============================================================
    resolve_parameter_class <- function(schema_type, json_path, schema_file, enum_values = "") {
      if (identical(schema_file, "ontogeneticLinksPreference.csv"))   return("Preference")
      if (identical(schema_file, "ontogeneticLinksProfitability.csv")) return("Profitability")
      if (identical(schema_type, "number"))   return("RealValued")
      if (identical(schema_type, "integer"))  return("Integer")
      if (identical(schema_type, "boolean"))  return("Boolean")
      if (identical(schema_type, "array")) {
        if (identical(schema_file, "species_schema.json") && identical(json_path, "animal$individualsPerInstar")) {
          return("Abundances")
        }
        return("Array")
      }
      if (identical(schema_type, "string") && nzchar(enum_values)) return("Permutation")
      return("RealValued")
    }

    resolve_config_destinations_by_schema <- function(schema_file, config_path) {
      if (is.null(config_path) || config_path == "" || !dir.exists(config_path)) return(character(0))
      
      if (schema_file %in% c("ontogeneticLinksPreference.csv", "ontogeneticLinksProfitability.csv")) {
        target_path <- file.path(config_path, "species", schema_file)
        return(if (file.exists(target_path)) file.path("species", schema_file) else character(0))
      }
      
      schema_to_target <- list(
        simulation_params_schema.json    = list(mode = "file", path = "simulation_params.json"),
        landscape_params_schema.json     = list(mode = "file", path = "landscape_params.json"),
        species_schema.json              = list(mode = "dir",  path = "species"),
        resource_schema.json             = list(mode = "dir",  path = "resource"),
        resource_patch_schema.json       = list(mode = "dir",  path = "resource/patches"),
        obstacle_patch_schema.json       = list(mode = "dir",  path = "obstacle"),
        moisture_patch_schema.json       = list(mode = "dir",  path = "moisture"),
        habitat_domain_patch_schema.json = list(mode = "dir",  path = "habitat_domain")
      )
      
      rule <- schema_to_target[[schema_file]]
      if (is.null(rule)) return(character(0))
      
      target_path <- file.path(config_path, rule$path)
      if (identical(rule$mode, "file")) return(if (file.exists(target_path)) rule$path else character(0))
      if (!dir.exists(target_path)) return(character(0))
      
      found <- list.files(target_path, pattern = "\\.json$", full.names = FALSE)
      if (length(found) == 0) return(character(0))
      return(file.path(rule$path, found))
    }

    get_instar_count_from_destination <- function(dest_path, config_path) {
      if (is.null(dest_path) || dest_path == "" || is.null(config_path) || config_path == "") return(0)
      full_path <- file.path(config_path, dest_path)
      if (!file.exists(full_path)) return(0)
      
      tryCatch({
        data <- jsonlite::fromJSON(full_path, simplifyVector = TRUE)
        individuals <- data[["animal"]][["individualsPerInstar"]]
        return(if (is.null(individuals)) 0 else length(individuals))
      }, error = function(e) 0)
    }

    get_species_metadata_catalog <- function(config_path) {
      if (is.null(config_path) || config_path == "") return(list())
      species_dir <- file.path(config_path, "species")
      if (!dir.exists(species_dir)) return(list())
      
      json_files <- list.files(species_dir, pattern = "\\.json$", full.names = TRUE)
      catalog <- list()
      
      for (f in json_files) {
        tryCatch({
          data <- jsonlite::fromJSON(f, simplifyVector = TRUE)
          # Extraemos la metadata nativa estructurada según el schema del simulador
          sp_name     <- data[["animal"]][["name"]]
          individuals <- data[["animal"]][["individualsPerInstar"]]
          
          if (!is.null(sp_name)) {
            catalog[[sp_name]] <- list(
              file_name    = basename(f),
              instar_count = if (is.null(individuals)) 1L else length(individuals)
            )
          }
        }, error = function(e) NULL) # Silencia errores defensivamente si un JSON está malformado
      }
      return(catalog)
    }

    get_resource_metadata_catalog <- function(config_path) {
      if (is.null(config_path) || config_path == "") return(list())
      resource_dir <- file.path(config_path, "resource")
      if (!dir.exists(resource_dir)) return(list())
      
      json_files <- list.files(resource_dir, pattern = "\\.json$", full.names = TRUE)
      catalog <- list()
      
      for (f in json_files) {
        tryCatch({
          data <- jsonlite::fromJSON(f, simplifyVector = TRUE)
          res_name <- data[["resource"]][["name"]]
          
          if (!is.null(res_name)) {
            catalog[[res_name]] <- list(
              file_name    = basename(f),
              instar_count = 1L # Los recursos estáticos operan por defecto con 1 estadio único
            )
          }
        }, error = function(e) NULL)
      }
      return(catalog)
    }

    # ----------------============================================================
    # 4. DISPATCHED CENTRAL ERROR COLLECTION ENGINE (GLOBAL MULTILAYER AUDIT)
    # ------------------------------------------------============================
    observe({
      catalog <- selected_params_catalog()
      req(!is.null(catalog), nrow(catalog) > 0)
      config_root <- base_config_path()
      
      # Fresh internal tracking dictionaries to isolate logs across refreshes
      tmp_empty_adjs    <- list()
      tmp_range_errors  <- list()
      tmp_alias_errors  <- list()
      tmp_perm_errors   <- list()
      tmp_abundance_errors        <- list()
      tmp_predator_instars_errors <- list()
      tmp_prey_instars_errors     <- list()
      tmp_overlaps_errors         <- list()
      tmp_pref_csv_errors         <- list()
      
      # Stage A: First pass to gather ALL custom alias strings across all parameters to compute global frequencies
      raw_all_aliases_vector <- character(0)
      
      for (i in seq_len(nrow(catalog))) {
        p_row    <- catalog[i, , drop = FALSE]
        param_id <- p_row$id[]
        adjs     <- as.integer(adjustment_ids_state[[param_id]] %||% 1L)
        
        for (a_id in adjs) {
          alias_val <- input[[paste0("name_", param_id, "_", a_id)]]
          if (!is.null(alias_val) && trimws(alias_val) != "") {
            raw_all_aliases_vector <- c(raw_all_aliases_vector, trimws(alias_val))
          }
        }
      }
      
      # Stage B: Second pass to apply structural validations and check duplicate collisions
      for (i in seq_len(nrow(catalog))) {
        p_row       <- catalog[i, , drop = FALSE]
        param_id    <- p_row$id[]
        json_path   <- p_row$json_path[]
        schema_file <- p_row$schema_file[]
        param_type  <- p_row$type[]
        enum_values <- p_row$enum_values[]
        
        param_class <- resolve_parameter_class(param_type, json_path, schema_file, enum_values)
        adjustment_ids <- as.integer(adjustment_ids_state[[param_id]] %||% 1L)
        all_destinations <- resolve_config_destinations_by_schema(schema_file, config_root)
        
        # ----------------============================================================
        # RULE 7: CROSS-ADJUSTMENT TROPHIC CELL OVERLAP AUDIT (COMMON VALIDATION)
        # --------------------------------============================================
        if (param_class %in% c("Preference", "Profitability")) {
          cell_registry_map <- list()
          
          for (j_idx in seq_along(adjustment_ids)) {
            a_id_check <- adjustment_ids[[j_idx]]
            
            p_sp  <- input[[paste0("pref_predator_sp_", param_id, "_", a_id_check)]]
            p_ins <- input[[paste0("pref_predator_instars_", param_id, "_", a_id_check)]]
            r_sp  <- input[[paste0("pref_prey_sp_", param_id, "_", a_id_check)]]
            r_ins <- input[[paste0("pref_prey_instars_", param_id, "_", a_id_check)]]
            
            if (!is.null(p_sp) && length(p_ins) > 0 && !is.null(r_sp) && length(r_ins) > 0) {
              for (pred_instar in p_ins) {
                for (prey_instar in r_ins) {
                  column_token <- paste(p_sp, pred_instar, sep = "$")
                  row_token    <- paste(r_sp, prey_instar, sep = "$")
                  cell_coordinate_key <- paste(row_token, column_token, sep = "||")
                  
                  cell_registry_map[[cell_coordinate_key]] <- c(cell_registry_map[[cell_coordinate_key]], a_id_check)
                }
              }
            }
          }
          
          for (cell_key in names(cell_registry_map)) {
            claiming_adjustments <- cell_registry_map[[cell_key]]
            if (length(claiming_adjustments) > 1) {
              split_coords <- unlist(strsplit(cell_key, "\\|\\|"))
              row_display  <- gsub("\\$", " (Instar ", split_coords[1])
              col_display  <- gsub("\\$", " (Instar ", split_coords[2])
              
              for (offending_adj_id in claiming_adjustments) {
                target_key <- paste0(param_id, "_adj_", offending_adj_id)
                msg_blueprint <- paste0("Error [Trophic Overlap]: This matrix layer overlaps with another adjustment on Row [", 
                                        row_display, ")] and Column [", col_display, ")]. Placed cells must be mutually exclusive.")
                tmp_overlaps_errors[[target_key]] <- unique(c(tmp_overlaps_errors[[target_key]], msg_blueprint))
              }
            }
          }
        }

        # ----------------============================================================
        # RULE 8: CONSTRAINTS FEASIBILITY MATRIX BALANCE AUDIT (PREFERENCE CSV)
        # ----------------============================================================
        if (param_class == "Preference" && length(tmp_overlaps_errors) == 0) {
          tryCatch({
            csv_file_path <- file.path(config_root, "species", "ontogeneticLinksPreference.csv")
            
            if (file.exists(csv_file_path)) {
              preference_matrix_df <- read.csv(csv_file_path, header = TRUE, row.names = 1, sep = ",", check.names = FALSE)
              matrix_columns <- colnames(preference_matrix_df)
              matrix_rows    <- rownames(preference_matrix_df)
              
              # Mapa estructural para linkear qué bloque exacto (adj_id) gobierna cada celda
              cell_to_adjustment_binding <- list()
              
              for (a_id in adjustment_ids) {
                p_sp  <- input[[paste0("pref_predator_sp_", param_id, "_", a_id)]]
                p_ins <- input[[paste0("pref_predator_instars_", param_id, "_", a_id)]]
                r_sp  <- input[[paste0("pref_prey_sp_", param_id, "_", a_id)]]
                r_ins <- input[[paste0("pref_prey_instars_", param_id, "_", a_id)]]
                
                if (!is.null(p_sp) && length(p_ins) > 0 && !is.null(r_sp) && length(r_ins) > 0) {
                  for (pr_ins in p_ins) {
                    for (py_ins in r_ins) {
                      col_c <- paste(p_sp, pr_ins, sep = "$")
                      row_r <- paste(r_sp, py_ins, sep = "$")
                      cell_coord <- paste(row_r, col_c, sep = "||")
                      
                      # Enlazamos la coordenada con el ID de su tarjeta de origen
                      cell_to_adjustment_binding[[cell_coord]] <- a_id
                    }
                  }
                }
              }
              
              # Evaluamos de forma matemática la viabilidad algebraica columna por columna (Predator)
              for (j in seq_along(matrix_columns)) {
                current_col <- matrix_columns[j]
                fixed_immutable_sum <- 0
                accumulated_min_required <- 0
                accumulated_max_possible <- 0
                has_optimization_on_column <- FALSE
                
                # Lista local para saber qué ajustes tienen celdas afectadas en esta columna concreta
                offending_adjs_in_column <- integer(0)
                
                for (i in seq_along(matrix_rows)) {
                  current_row <- matrix_rows[i]
                  coord_key <- paste(current_row, current_col, sep = "||")
                  
                  if (coord_key %in% names(cell_to_adjustment_binding)) {
                    # Recuperamos el ID del bloque que gobierna esta celda específica
                    bound_adj_id <- cell_to_adjustment_binding[[coord_key]]
                    offending_adjs_in_column <- unique(c(offending_adjs_in_column, bound_adj_id))
                    
                    # Extraemos los inputs numéricos ingresados por el usuario
                    min_bound_val <- input[[paste0("min_", param_id, "_", bound_adj_id)]] %||% 0.0
                    max_bound_val <- input[[paste0("max_", param_id, "_", bound_adj_id)]] %||% 1.0
                    
                    # CORRECCIÓN PONDERADA: Sumamos el límite del bloque tantas veces como celdas intercepte en esta columna
                    accumulated_min_required <- accumulated_min_required + min_bound_val
                    accumulated_max_possible <- accumulated_max_possible + max_bound_val
                    has_optimization_on_column <- TRUE
                  } else {
                    # Celda de la parte fija (Immutable cell): Extraemos su valor estático directo del CSV
                    fixed_immutable_sum <- fixed_immutable_sum + as.numeric(preference_matrix_df[current_row, current_col])
                  }
                }
                
                if (has_optimization_on_column) {
                  available_space_remainder <- 1.0 - fixed_immutable_sum
                  conflict_msg <- NULL
                  
                  # Sub-Validación A: Si la suma de mínimos ponderados satura el espacio libre
                  if (accumulated_min_required > (available_space_remainder + 1e-5)) {
                    conflict_msg <- paste0("Error [Algebraic Feasibility]: Predator column '", current_col, 
                                           "' has immutable cells summing ", round(fixed_immutable_sum, 4), 
                                           ", leaving a free space of ", round(available_space_remainder, 4), 
                                           ". However, your dynamic group blocks demand a weighted minimum sum of ", round(accumulated_min_required, 4), ".")
                  }
                  
                  # Sub-Validación B: Si el techo de los máximos ponderados no alcanza a completar la probabilidad obligada
                  if (is.null(conflict_msg) && accumulated_max_possible < (available_space_remainder - 1e-5)) {
                    conflict_msg <- paste0("Error [Algebraic Feasibility]: Predator column '", current_col, 
                                           "' requires optimized cells to sum exactly ", round(available_space_remainder, 4), 
                                           " to complete the 1.0 probability limit, but the maximum boundary allowed by your dynamic groups is only ", round(accumulated_max_possible, 4), ".")
                  }
                  
                  # Si se detecta un desbalance, mapeamos el error local a TODOS los bloques implicados en esta columna
                  if (!is.null(conflict_msg)) {
                    for (adj_id_error in offending_adjs_in_column) {
                      target_key <- paste0(param_id, "_adj_", adj_id_error)
                      tmp_pref_csv_errors[[target_key]] <- unique(c(tmp_pref_csv_errors[[target_key]], conflict_msg))
                    }
                  }
                }
              }
            }
          }, error = function(e) {
            tmp_pref_csv_errors[[param_id]] <- paste0("Mathematical matrix parse failed due to a structural CSV layout error: ", e$message)
          })
        }

        for (idx in seq_along(adjustment_ids)) {
          adj_id            <- adjustment_ids[[idx]]
          comp_key          <- paste0(param_id, "_adj_", adj_id)
          
          selected_destinations <- input[[paste0("destinations_", param_id, "_", adj_id)]] %||% character(0)
          min_val   <- input[[paste0("min_", param_id, "_", adj_id)]]
          max_val   <- input[[paste0("max_", param_id, "_", adj_id)]]
          alias_val <- input[[paste0("name_", param_id, "_", adj_id)]]
          
          # RULE 1: Enforce at least one asset destination layout is mapped (WITH EXHAUSTION)
          if (!param_class %in% c("Preference", "Profitability") && length(selected_destinations) == 0) {
            other_adjustments <- setdiff(adjustment_ids, adj_id)
            selected_by_others <- unique(unlist(lapply(other_adjustments, function(o_id) {
              input[[paste0("destinations_", param_id, "_", o_id)]] %||% character(0)
            })))
            
            remaining_in_param <- length(setdiff(all_destinations, selected_by_others))
            
            if (remaining_in_param == 0) {
              tmp_empty_adjs[[comp_key]] <- paste0("Error [", json_path, " - Adjustment ", idx, "]: This adjustment has no destinations assigned and there are no remaining files available in the parameter pool. Please remove this block or clear an asset from another layer.")
            } else {
              tmp_empty_adjs[[comp_key]] <- paste0("Error [", json_path, " - Adjustment ", idx, "]: Destination selector cannot be left blank. Please match at least one file entry.")
            }
          }
          
          # RULE 2: Range validation constraints (min <= max)
          if (param_class %in% c("Integer", "RealValued", "Abundances", "Preference", "Profitability")) {
            if (!is.null(min_val) && !is.null(max_val) && !is.na(min_val) && !is.na(max_val)) {
              if (min_val > max_val) {
                tmp_range_errors[[comp_key]] <- paste0("Error [", json_path, " - Adjustment ", idx, "]: Continuous minimum bound cannot exceed the maximum bound.")
              }
            }
          }
          
          # RULE 3: Global Alias unique constraint audit
          if (is.null(alias_val) || trimws(alias_val) == "") {
            tmp_alias_errors[[comp_key]] <- paste0("Error [", json_path, " - Adjustment ", idx, "]: Custom configuration alias identifier cannot be left blank.")
          } else {
            clean_alias <- trimws(alias_val)
            occurrences_count <- sum(raw_all_aliases_vector == clean_alias)
            if (occurrences_count > 1) {
              tmp_alias_errors[[comp_key]] <- paste0("Error [", json_path, " - Adjustment ", idx, "]: The custom alias identifier '", clean_alias, "' is duplicated. Every adjustment must have a globally unique name.")
            }
          }

          # RULE 4: Categorical Permutations items selection audit
          if (param_class == "Permutation") {
            elements_current <- input[[paste0("elements_", param_id, "_", adj_id)]]
            status_key       <- paste0("permutation_status_", param_id, "_", adj_id)
            if (!is.null(input[[status_key]]) && !isTRUE(input[[status_key]])) {
              tmp_perm_errors[[comp_key]] <- paste0("Error [", json_path, " - Adjustment ", idx, "]: Selected elements list cannot be empty. You must mark at least one option.")
            } else if (is.null(elements_current) || length(elements_current) == 0) {
              tmp_perm_errors[[comp_key]] <- paste0("Error [", json_path, " - Adjustment ", idx, "]: Selected elements list cannot be empty. You must mark at least one option.")
            }
          }

          # RULE 5: Abundances vector mathematical constraint audit
          if (param_class == "Abundances" && length(selected_destinations) > 0) {
            target_dest <- selected_destinations[[1]]
            instar_count <- get_instar_count_from_destination(target_dest, config_root)
            
            if (instar_count > 0) {
              densities_vals <- numeric(instar_count)
              is_any_density_empty <- FALSE
              
              for (v_idx in seq_len(instar_count)) {
                val_input <- input[[paste0("density_elem_", param_id, "_", adj_id, "_", v_idx)]]
                if (is.null(val_input) || is.na(val_input)) {
                  is_any_density_empty <- TRUE
                } else {
                  densities_vals[v_idx] <- val_input
                }
              }
              
              error_key_abundance <- paste0(param_id, "_adj_", adj_id)
              if (is_any_density_empty) {
                tmp_abundance_errors[[error_key_abundance]] <- paste0("Error [", json_path, " - Adjustment ", idx, "]: All target instar density fields must be filled with a valid numeric value.")
              } else {
                total_sum <- sum(densities_vals)
                if (abs(total_sum - 1.0) > 1e-5) {
                  tmp_abundance_errors[[error_key_abundance]] <- paste0("Error [", json_path, " - Adjustment ", idx, "]: The sum of the density vector coefficients is ", round(total_sum, 4), ". It must equal exactly 1.0.")
                }
              }
            }
          }

          # RULE 6: Trophic interaction vectors instars check
          if (param_class %in% c("Preference", "Profitability")) {
            pred_ins_vector <- input[[paste0("pref_predator_instars_", param_id, "_", adj_id)]]
            prey_ins_vector <- input[[paste0("pref_prey_instars_", param_id, "_", adj_id)]]
            
            if (is.null(pred_ins_vector) || length(pred_ins_vector) == 0) {
              tmp_predator_instars_errors[[paste0(comp_key, "_pred")]] <- paste0("Error [", json_path, " - Adjustment ", idx, "]: Predator lifecycle stages list cannot be left empty.")
            }
            if (is.null(prey_ins_vector) || length(prey_ins_vector) == 0) {
              tmp_prey_instars_errors[[paste0(comp_key, "_prey")]] <- paste0("Error [", json_path, " - Adjustment ", idx, "]: Targeted prey lifecycle stages list cannot be left empty.")
            }
          }
        }
      }
      
      # Commit atomic logs directly into our master layout arrays registries
      active_errors$empty_adjustments       <- tmp_empty_adjs
      active_errors$boundary_conflicts      <- tmp_range_errors
      active_errors$duplicate_all_aliases   <- tmp_alias_errors
      active_errors$permutations_empty      <- tmp_perm_errors
      active_errors$abundances_sum_error    <- tmp_abundance_errors
      active_errors$predator_instars_errors <- tmp_predator_instars_errors
      active_errors$prey_instars_errors     <- tmp_prey_instars_errors
      active_errors$trophic_overlaps_errors <- tmp_overlaps_errors
      active_errors$preference_csv_errors   <- tmp_pref_csv_errors
    })

    # ----------------============================================================
    # 5. TRANSACTIONAL PHASE VALIDITY EVALUATOR (EXPOSED TO ORCHESTRATOR)
    # ----------------============================================================
    phase_3_is_valid <- reactive({
      all_errors_list <- shiny::reactiveValuesToList(active_errors)
      # Flatten list elements to check if any active error string token exists
      consolidated_errors <- unlist(all_errors_list)
      return(length(consolidated_errors) == 0)
    })

    # ----------------============================================================
    # 6. PASSTHROUGH GENERAL ALERTS BOX RENDERING (DASHBOARD HIGHER PANEL)
    # ----------------============================================================
    output$specifics_errors_container <- renderUI({
      all_errors <- shiny::reactiveValuesToList(active_errors)
      consolidated_errors_vector <- c()
      
      # Merge separate sub-lists into a clean unified character vector array
      if (length(all_errors$empty_adjustments) > 0)  consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$empty_adjustments))
      if (length(all_errors$boundary_conflicts) > 0) consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$boundary_conflicts))
      if (length(all_errors$duplicate_all_aliases) > 0) consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$duplicate_all_aliases))
      if (length(all_errors$abundances_sum_error) > 0) consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$abundances_sum_error))
      if (length(all_errors$permutations_empty) > 0)  consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$permutations_empty))
      if (length(all_errors$predator_instars_errors) > 0)  consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$predator_instars_errors))
      if (length(all_errors$prey_instars_errors) > 0)  consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$prey_instars_errors))

      if (length(consolidated_errors_vector) == 0) return(NULL)
      
      div(
        class = "alert alert-danger shadow-sm mb-4 text-body",
        style = "border-left: 5px solid var(--bs-danger); background-color: var(--bs-card-bg); border-color: var(--bs-border-color);",
        div(class = "d-flex align-items-center mb-2 fw-bold fs-6", icon("circle-xmark", class = "me-2 text-danger"), "General Phase 3 Validation Errors Detected"),
        tags$ul(style = "margin-bottom: 0; padding-left: 20px;",
          lapply(consolidated_errors_vector, function(err_msg) {
            tags$li(err_msg)
          })
        )
      )
    })

    # ============================================================================
    # 6. ASYNC ACCORDION EDITORS MATRIX GENERATION WORKFLOW (LOCAL ERRORS ENABLED)
    # ============================================================================
    build_adjustment_editor <- function(param_id, adj_id, display_index, adjustment_ids, param_class, enum_values, min_val, max_val, all_destinations) {
      dest_input_id <- paste0("destinations_", param_id, "_", adj_id)
      selected_current <- input[[dest_input_id]] %||% character(0)
      
      other_adjs <- setdiff(adjustment_ids, adj_id)
      selected_by_others <- unique(unlist(lapply(other_adjs, function(o_id) input[[paste0("destinations_", param_id, "_", o_id)]] %||% character(0))))
      available_choices  <- unique(c(setdiff(all_destinations, selected_by_others), selected_current))
      
      is_multiple <- (param_class != "Abundances")
      
      bslib::card(
        class = "mb-3 shadow-sm border-secondary-subtle card optimiser-input-card",
        bslib::card_header(
          div(class = "d-flex justify-content-between align-items-center w-100",
              span(class = "me-auto", tags$b(paste("Adjustment Pool Layer:", display_index))),
              if (length(adjustment_ids) > 1) actionButton(ns(paste0("remove_adjustment_", param_id, "_", adj_id)), "Remove", class = "btn-outline-danger btn-sm")
          )
        ),
        bslib::card_body(
          
          # LÓGICA DE PERSISTENCIA ATÓMICA: Comprueba si el ID ya existe en la sesión actual de Shiny.
          # Si existe, rescata el texto escrito para evitar que se borre al añadir otras tarjetas.
          renderUI({
            name_input_id <- paste0("name_", param_id, "_", adj_id)
            
            current_alias_value <- if (name_input_id %in% names(input)) {
              input[[name_input_id]]
            } else {
              ""  # Valor por defecto inicial si la tarjeta es completamente nueva
            }
            
            textInput(
              inputId     = ns(name_input_id), 
              label       = "Custom Configuration Alias Identifier:", 
              value       = current_alias_value, 
              placeholder = paste("Variation snapshot segment", display_index)
            )
          }),
          
          uiOutput(ns(paste0("error_local_alias_", param_id, "_", adj_id))),

          if (!param_class %in% c("Preference", "Profitability")) {
            tagList(
              selectizeInput(ns(dest_input_id), "Target Configuration Asset Destinations:", choices = available_choices, selected = selected_current, multiple = is_multiple,
                             options = list(placeholder = "Click to assign available system files...", plugins = if(is_multiple) list("remove_button") else list())),
              
              # COUPLING SLOT: Localized placeholder to inject destinations tracking validation outcomes
              uiOutput(ns(paste0("error_local_destinations_", param_id, "_", adj_id)))
            )
          } else { NULL },
          
          tags$div(class = "numeric-range-container mt-2", `data-param-id` = param_id, `data-adj-id` = adj_id,
              uiOutput(ns(paste0("range_ui_", param_id, "_", adj_id))),
              
              # REWRITTEN RANGE DISPLAY: Linked to active_errors directly to pass computed constraints messages
              uiOutput(ns(paste0("error_local_range_", param_id, "_", adj_id)))
          ),
          uiOutput(ns(paste0("specifics_ui_", param_id, "_", adj_id)))
        )
      )
    }

    output$dynamic_param_editors <- renderUI({
      catalog <- selected_params_catalog()
      if (is.null(catalog) || nrow(catalog) == 0) {
        return(p(class = "text-muted italic small", "Please select one or more parameter branches inside the tree layout (Phase 2) to authorize properties assignment bounds."))
      }
      
      config_root <- base_config_path()
      
      param_ui_list <- lapply(seq_len(nrow(catalog)), function(i) {
        p_row       <- catalog[i, , drop = FALSE]
        param_id    <- p_row$id[]
        json_path   <- p_row$json_path[]
        schema_file <- p_row$schema_file[]
        param_type  <- p_row$type[]
        enum_values <- p_row$enum_values[]
        
        param_class <- resolve_parameter_class(param_type, json_path, schema_file, enum_values)
        min_val     <- if (is.na(p_row$min[])) 0.0 else p_row$min[]
        max_val     <- if (is.na(p_row$max[])) 1.0 else p_row$max[]
        
        all_destinations <- resolve_config_destinations_by_schema(schema_file, config_root)
        
        if (is.null(adjustment_ids_state[[param_id]])) {
          adjustment_ids_state[[param_id]] <- 1L
          next_adjustment_id_state[[param_id]] <- 2L
        }
        
        # Register standard structural dynamic additive sub-handlers listeners once
        if (!param_id %in% registered_add_handlers()) {
          observeEvent(input[[paste0("add_adjustment_", param_id)]], {
            c_ids <- adjustment_ids_state[[param_id]] %||% 1L
            n_id  <- next_adjustment_id_state[[param_id]] %||% (max(c_ids) + 1L)
            adjustment_ids_state[[param_id]] <- c(c_ids, as.integer(n_id))
            next_adjustment_id_state[[param_id]] <- as.integer(n_id) + 1L
          }, ignoreInit = TRUE)
          registered_add_handlers(c(registered_add_handlers(), param_id))
        }
        
        output[[paste0("adjustments_list_", param_id)]] <- renderUI({
          adjustment_ids <- as.integer(adjustment_ids_state[[param_id]] %||% 1L)
          
          lapply(seq_along(adjustment_ids), function(idx) {
            adj_id <- adjustment_ids[[idx]]
            comp_key <- paste0(param_id, "_adj_", adj_id)
            
            # Setup localized contextual single-instance destruction listeners loops
            handler_key <- paste0(param_id, "::", adj_id)
            if (!handler_key %in% registered_remove_handlers()) {
              local({
                p_id <- param_id; a_id <- adj_id
                observeEvent(input[[paste0("remove_adjustment_", p_id, "_", a_id)]], {
                  c_ids <- adjustment_ids_state[[p_id]] %||% 1L
                  if (length(c_ids) > 1) adjustment_ids_state[[p_id]] <- setdiff(c_ids, a_id)
                }, ignoreInit = TRUE)
              })
              registered_remove_handlers(c(registered_remove_handlers(), handler_key))
            }
            
            # ============================================================================
            # NEW PHASE 1 PIPELINE REPLICATION: LOCAL ERRORS RENDER NODES
            # ============================================================================
            # A. Local Destinations Error Passthrough
            output[[paste0("error_local_destinations_", param_id, "_", adj_id)]] <- renderUI({
              err_msg <- active_errors$empty_adjustments[[comp_key]]
              req(!is.null(err_msg))
              p(icon("circle-xmark"), err_msg, class = "text-danger small fw-bold mt-1 mb-2 text-break")
            })

            output[[paste0("error_local_alias_", param_id, "_", adj_id)]] <- renderUI({
              err_msg <- active_errors$duplicate_all_aliases[[comp_key]]
              req(!is.null(err_msg))
              p(icon("circle-xmark"), err_msg, class = "text-danger small fw-bold mt-1 mb-2 text-break")
            })

            # B. Local Numerical Range (min <= max) Error Passthrough
            output[[paste0("error_local_range_", param_id, "_", adj_id)]] <- renderUI({
              err_msg <- active_errors$boundary_conflicts[[comp_key]]
              req(!is.null(err_msg))
              p(icon("circle-xmark"), err_msg, class = "text-danger small fw-bold mt-2 mb-1 text-break")
            })

            output[[paste0("error_local_permutation_", param_id, "_", adj_id)]] <- renderUI({
              comp_key <- paste0(param_id, "_adj_", adj_id)
              err_msg  <- active_errors$permutations_empty[[comp_key]]
              
              req(!is.null(err_msg))
              p(icon("circle-xmark"), err_msg, class = "text-danger small fw-bold mt-2 mb-1 text-break")
            })

            output[[paste0("error_local_predator_", param_id, "_", adj_id)]] <- renderUI({
              err_msg <- active_errors$predator_instars_errors[[paste0(comp_key, "_pred")]]
              req(!is.null(err_msg))
              p(icon("circle-xmark"), err_msg, class = "text-danger small fw-bold mt-2 mb-0 text-break")
            })

            output[[paste0("error_local_prey_", param_id, "_", adj_id)]] <- renderUI({
              err_msg <- active_errors$prey_instars_errors[[paste0(comp_key, "_prey")]]
              req(!is.null(err_msg))
              p(icon("circle-xmark"), err_msg, class = "text-danger small fw-bold mt-2 mb-0 text-break")
            })

            output[[paste0("error_local_trophic_overlap_", param_id, "_", adj_id)]] <- renderUI({
              comp_key_local <- paste0(param_id, "_adj_", adj_id)
              err_messages   <- active_errors$trophic_overlaps_errors[[comp_key_local]]
              
              req(!is.null(err_messages), length(err_messages) > 0)
              
              # Si un bloque colisiona en más de una celda, desglosamos las alertas secuencialmente
              tags$div(class = "mt-2 pt-1 border-top border-danger-subtle text-danger small fw-bold text-break d-block",
                lapply(err_messages, function(msg) p(icon("triangle-exclamation", class = "me-1"), msg, class = "mb-1"))
              )
            })

            output[[paste0("error_local_pref_csv_", param_id, "_", adj_id)]] <- renderUI({
              comp_key_local <- paste0(param_id, "_adj_", adj_id)
              err_messages   = active_errors$preference_csv_errors[[comp_key_local]]
              
              req(!is.null(err_messages), length(err_messages) > 0)
              
              # Si un bloque infringe la viabilidad en más de una columna, listamos los desbalances secuencialmente
              tags$div(class = "mt-2 pt-1 border-top border-danger-subtle text-danger small fw-bold text-break d-block",
                lapply(err_messages, function(msg) p(icon("triangle-exclamation", class = "me-1"), msg, class = "mb-1"))
              )
            })
            
            # Localized sub-component rendering allocations (min/max numeric cells fields)
            output[[paste0("range_ui_", param_id, "_", adj_id)]] <- renderUI({
              min_curr <- input[[paste0("min_", param_id, "_", adj_id)]] %||% min_val
              max_curr <- input[[paste0("max_", param_id, "_", adj_id)]] %||% max_val
              
              # 1. Resolución analítica de la máscara tipográfica y los intervalos de incremento
              if (param_class %in% c("Integer", "Abundances")) {
                input_class <- "integer-input"
                step_attr   <- 1L
                min_attr    <- 0L
                max_attr    <- NA
              } else if (param_class %in% c("RealValued", "Preference", "Profitability")) {
                input_class <- "decimal-input"
                step_attr   <- 0.01
                min_attr    <- 0.0
                # El límite máximo rígido de 1.0 se aplica exclusivamente al dúo Preference / Profitability
                max_attr    <- if (param_class %in% c("Preference", "Profitability")) 1.0 else NA
              } else {
                input_class <- "decimal-input"
                step_attr   <- NA
                min_attr    <- NA
                max_attr    <- NA
              }
              
              # ============================================================================
              # REESTRUCTURACIÓN COMPLETA DE REJILLA PARA EL QUINTETO NUMÉRICO AUTORIZADO
              # ============================================================================
              if (param_class %in% c("Integer", "RealValued", "Abundances", "Preference", "Profitability")) {
                
                shiny::tagList(
                  bslib::layout_columns(
                    col_widths = c(6, 6),
                    div(class = input_class, numericInput(ns(paste0("min_", param_id, "_", adj_id)), "Min Bound:", value = min_curr, min = min_attr, max = max_attr, step = step_attr)),
                    div(class = input_class, numericInput(ns(paste0("max_", param_id, "_", adj_id)), "Max Bound:", value = max_curr, min = min_attr, max = max_attr, step = step_attr))
                  ),
                  
                  if (param_class == "Preference") {
                    uiOutput(ns(paste0("error_local_pref_csv_", param_id, "_", adj_id)))
                  } else {
                    NULL
                  }
                )
                
              } else if (param_class == "Boolean") {
                switch_input_id <- paste0("value_bool_", param_id, "_", adj_id)
                current_switch_value <- if (!is.null(input[[switch_input_id]])) input[[switch_input_id]] else FALSE
                
                bslib::layout_columns(
                  col_widths = c(6, 6),
                  div(class = "pt-2 pb-1 d-flex flex-column justify-content-center",
                      tags$label(class = "control-label fw-bold mb-1", "Boolean Value Toggle:"),
                      shinyWidgets::switchInput(
                        inputId  = ns(switch_input_id),
                        label    = NULL,
                        value    = current_switch_value,
                        onLabel  = "TRUE",
                        offLabel = "FALSE",
                        onStatus = "primary",
                        inline   = TRUE,
                        width    = "100%"
                      )
                  ),
                  div()
                )
              } else {
                NULL
              }
            })
            
            # Localized sub-component class variations (String, Permutations, Abundances vector)
            output[[paste0("specifics_ui_", param_id, "_", adj_id)]] <- renderUI({
              
              # --- CASE A: TROPHIC INTERACTIONS (PREFERENCE / PROFITABILITY) ---
              if (param_class %in% c("Preference", "Profitability")) {
                config_root <- base_config_path()
                
                species_catalog  <- get_species_metadata_catalog(config_root)
                resource_catalog <- get_resource_metadata_catalog(config_root)
                
                if (length(species_catalog) == 0) {
                  return(p(class = "text-muted italic small mt-2", 
                           icon("info-circle"), " Please set up a valid species folder configuration in Phase 1 to load trophic structures."))
                }
                
                animal_names   <- names(species_catalog)
                all_prey_names <- c(animal_names, names(resource_catalog))
                
                pred_sp_id  <- paste0("pref_predator_sp_", param_id, "_", adj_id)
                pred_ins_id <- paste0("pref_predator_instars_", param_id, "_", adj_id)
                prey_sp_id  <- paste0("pref_prey_sp_", param_id, "_", adj_id)
                prey_ins_id <- paste0("pref_prey_instars_", param_id, "_", adj_id)
                
                selected_pred_sp <- input[[pred_sp_id]]  %||% animal_names[[1]]
                selected_prey_sp <- input[[prey_sp_id]]  %||% all_prey_names[[1]]
                
                pred_max_instars <- if (selected_pred_sp %in% animal_names) species_catalog[[selected_pred_sp]]$instar_count else 1L
                prey_max_instars <- if (selected_prey_sp %in% animal_names) {
                  species_catalog[[selected_prey_sp]]$instar_count
                } else if (selected_prey_sp %in% names(resource_catalog)) {
                  resource_catalog[[selected_prey_sp]]$instar_count
                } else {
                  1L
                }
                
                selected_pred_ins <- if (pred_ins_id %in% names(input)) input[[pred_ins_id]] else as.character(seq_len(pred_max_instars))
                selected_prey_ins <- if (prey_ins_id %in% names(input)) input[[prey_ins_id]] else as.character(seq_len(prey_max_instars))
                
                return(
                  tags$div(
                    class = "mt-3 p-3 border rounded preference-cell-container shadow-sm",
                    tags$label(class = "control-label fw-bold mb-2 text-primary", 
                               paste0(param_class, " Interactive Cell Matrix Mapping (LinksCell):")),
                    p(class = "text-muted small mb-3", "Define the interaction bounds between the selected predator trophic levels and their targeted prey fields:"),
                    
                    bslib::layout_columns(
                      col_widths = c(6, 6),
                      
                      # --- DEPREDADOR ---
                      div(class = "p-3 border rounded bg-white shadow-sm",
                          style = "border-left: 4px solid var(--bs-primary) !important;",
                          tags$h6(class = "fw-bold text-dark mb-3", icon("skull"), " Predator Specification Matrix"),
                          selectInput(ns(pred_sp_id), "Select Active Predator Species:", choices = animal_names, selected = selected_pred_sp),
                          
                          selectizeInput(
                            inputId  = ns(pred_ins_id),
                            label    = "Target Predator Instars Stages:",
                            choices  = as.character(seq_len(pred_max_instars)),
                            selected = selected_pred_ins,
                            multiple = TRUE,
                            options  = list(placeholder = "Select lifecycle stages...", plugins = list("remove_button"))
                          ),
                          uiOutput(ns(paste0("error_local_predator_", param_id, "_", adj_id)))
                      ),
                      
                      # --- PRESA ---
                      div(class = "p-3 border rounded bg-white shadow-sm",
                          style = "border-left: 4px solid #18bc9c !important;",
                          tags$h6(class = "fw-bold text-dark mb-3", icon("shrimp"), " Prey Target Specification Matrix"),
                          selectInput(ns(prey_sp_id), "Select Prey Entity (Animal/Resource):", choices = all_prey_names, selected = selected_prey_sp),
                          
                          selectizeInput(
                            inputId  = ns(prey_ins_id),
                            label    = "Target Prey Instars Stages:",
                            choices  = as.character(seq_len(prey_max_instars)),
                            selected = selected_prey_ins,
                            multiple = TRUE,
                            options  = list(placeholder = "Select lifecycle stages...", plugins = list("remove_button"))
                          ),
                          uiOutput(ns(paste0("error_local_prey_", param_id, "_", adj_id)))
                      )
                    ),
                    
                    uiOutput(ns(paste0("error_local_trophic_overlap_", param_id, "_", adj_id)))
                  )
                )
              }

              # --- CASE B: POPULATION DENSITIES VECTOR (ABUNDANCES) ---
              if (param_class == "Abundances") {
                target_dest  <- input[[paste0("destinations_", param_id, "_", adj_id)]] %||% ""
                config_root  <- base_config_path()
                
                instar_count <- get_instar_count_from_destination(target_dest, config_root)
                
                if (instar_count == 0) {
                  return(p(class = "text-muted italic small mt-2", 
                           icon("info-circle"), " Please select a valid target species file above to initialize its vector configuration grid structure."))
                }
                
                current_vals <- numeric(instar_count)
                is_any_empty <- FALSE
                
                for (v_idx in seq_len(instar_count)) {
                  sub_input_id <- paste0("density_elem_", param_id, "_", adj_id, "_", v_idx)
                  val_input    <- input[[sub_input_id]]
                  if (is.null(val_input) || is.na(val_input)) {
                    is_any_empty <- TRUE
                  } else {
                    current_vals[v_idx] <- val_input
                  }
                }
                
                current_sum  <- sum(current_vals)
                is_sum_valid <- !is_any_empty && (abs(current_sum - 1.0) < 1e-5)
                badge_class  <- if (is_sum_valid) "bg-success" else "bg-warning text-dark"
                
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
                
                vector_inputs <- lapply(seq_len(instar_count), function(v_idx) {
                  sub_input_id <- paste0("density_elem_", param_id, "_", adj_id, "_", v_idx)
                  current_val  <- input[[sub_input_id]] %||% 0.0
                  
                  div(class = "col",
                      div(class = "decimal-input", 
                          numericInput(ns(sub_input_id), label = paste("Instar Stage", v_idx), value = current_val, min = 0, step = 0.01)
                      )
                  )
                })
                
                return(
                  tags$div(
                    class = "mt-3 p-3 border rounded abundances-vector-container shadow-sm",
                    
                    div(class = "d-flex justify-content-between align-items-center mb-2 pb-2 border-bottom border-secondary-subtle",
                        tags$label(class = "control-label fw-bold mb-0 text-primary", "Instar Densities Weight Vector Allocation (densitiesVector):"),
                        div(
                          tags$span(class = paste("badge me-2 p-2 font-monospace", badge_class), paste("Sum space:", round(current_sum, 4))),
                          actionButton(ns(normalize_btn_id), "Normalize to 1.0", class = "btn btn-outline-success btn-sm fw-bold", icon = icon("wand-magic-sparkles"))
                        )
                    ),
                    p(class = "text-muted small mb-3", paste("This targeted animal entity layout defines exactly", instar_count, "larval instars stages. The distribution must aggregate vertically and equal exactly to 1.0:")),
                    
                    div(class = "row row-cols-1 row-cols-sm-3 row-cols-md-5 g-3 mb-1", do.call(tagList, vector_inputs)),
                    
                    # Marcador local para inyectar advertencias algebraicas si la suma es inválida
                    uiOutput(ns(paste0("error_local_abundance_", param_id, "_", adj_id)))
                  )
                )
              }

              # --- CASE C: CATEGORICAL MATRICES (PERMUTATION) ---
              if (param_class == "Permutation") {
                choices_from_schema <- if (nzchar(enum_values)) unlist(strsplit(enum_values, "\\|")) else character(0)
                elements_input_id <- paste0("elements_", param_id, "_", adj_id)
                
                elements_current <- if (elements_input_id %in% names(input)) {
                  input[[elements_input_id]] 
                } else {
                  choices_from_schema 
                }
                
                return(
                  tags$div(
                    id = ns(paste0("wrapper_elements_", param_id, "_", adj_id)),
                    class = "permutation-buttons-container mt-3",
                    `data-param-id` = param_id,
                    `data-adj-id` = adj_id,
                    
                    shinyWidgets::checkboxGroupButtons(
                      inputId    = ns(elements_input_id),
                      label      = "Target Categorical Elements Selection (Click to toggle states):",
                      choices    = choices_from_schema,
                      selected   = elements_current,
                      justified  = TRUE,
                      individual = FALSE,
                      status     = "primary"
                    ),
                    uiOutput(ns(paste0("error_local_permutation_", param_id, "_", adj_id)))
                  )
                )
              }
              
              # --- CASE D: ALPHA NUMERIC VALUES (STRING) ---
              if (param_class == "String") {
                value_input_id <- paste0("value_str_", param_id, "_", adj_id)
                current_string_value <- input[[value_input_id]] %||% ""
                
                return(
                  div(class = "mt-3",
                      textInput(ns(value_input_id), label = tags$b("Literal Text Value:"), value = current_string_value, placeholder = "Enter alphanumeric configuration token...")
                  )
                )
              }
              
              NULL
            })
            
            build_adjustment_editor(
              param_id         = param_id, 
              adj_id           = adj_id, 
              display_index    = idx, 
              adjustment_ids   = adjustment_ids, 
              param_class      = param_class, 
              enum_values      = enum_values, 
              min_val          = min_val,
              max_val          = max_val,
              all_destinations = all_destinations
            )
          })
        })
        
        bslib::accordion_panel(
          title = div(class = "d-flex justify-content-between align-items-center w-100",
                    tags$span(class = "font-monospace small", tags$code(json_path))
                  ),
          value = param_id,
          
          # Renderizado dinámico de la cabecera interna con el conteo de destinos restantes
          renderUI({
            c_ids <- as.integer(adjustment_ids_state[[param_id]] %||% 1L)
            
            # Recolectamos de forma reactiva los destinos consumidos en todos los bloques de este parámetro
            selected_union <- unique(unlist(lapply(c_ids, function(a_id) {
              input[[paste0("destinations_", param_id, "_", a_id)]] %||% character(0)
            })))
            
            remaining_count <- length(setdiff(all_destinations, selected_union))
            
            div(class = "mb-3 d-flex align-items-center gap-2",
                tags$span("Parameter Operational Category:"),
                span(class = "badge bg-info text-dark font-monospace", param_class),
                # El Badge cambia a color gris neutro si ya no quedan recursos disponibles
                span(class = paste("badge font-monospace", if(remaining_count > 0) "bg-success" else "bg-secondary"),
                     paste("Available Destinations:", remaining_count))
            )
          }),
          
          uiOutput(ns(paste0("adjustments_list_", param_id))),
          hr(),
          
          # Renderizado condicional del botón basado en los destinos restantes
          renderUI({
            c_ids <- as.integer(adjustment_ids_state[[param_id]] %||% 1L)
            selected_union <- unique(unlist(lapply(c_ids, function(a_id) {
              input[[paste0("destinations_", param_id, "_", a_id)]] %||% character(0)
            })))
            
            remaining_count <- length(setdiff(all_destinations, selected_union))
            
            if (remaining_count > 0 || param_class %in% c("Preference", "Profitability")) {
              # Se permite añadir ajustes de forma infinita mientras el conteo sea superior a cero
              actionButton(ns(paste0("add_adjustment_", param_id)), "Add Sub-Adjustment Split Block", 
                           class = "btn-outline-primary btn-sm fw-bold", icon = icon("plus"))
            } else {
              # El botón se bloquea de forma elegante imitando el look adaptativo sin bordes invasivos
              tags$button(type = "button", class = "btn btn-outline-secondary btn-sm disabled", 
                          style = "cursor: not-allowed; opacity: 0.6;",
                          icon("ban"), " No configuration asset destinations remaining")
            }
          })
        )
      })
      
      do.call(bslib::accordion, c(param_ui_list, multiple = TRUE))
    })

    # ----------------============================================================
    # 7. EXPOSE PIPELINES BINDINGS HANDLE REGISTRY BACK TO MAIN SUPERVISOR
    # ------------------------------------------------============================
    commit_phase_3_data_transaction <- function() {
      message("[Phase 3 Store] Finalizing boundaries configuration datasets mapping transaction commits.")
      
      catalog <- selected_params_catalog()
      if (is.null(catalog) || nrow(catalog) == 0) return()
      
      # Build structured serializations tracking boundaries allocations to feed Phase 4 engines
      compiled_bounds_list <- list()
      
      for (i in seq_len(nrow(catalog))) {
        p_row    <- catalog[i, , drop = FALSE]
        param_id <- p_row$id[]
        adjs     <- as.integer(adjustment_ids_state[[param_id]] %||% 1L)
        
        compiled_bounds_list[[param_id]] <- lapply(adjs, function(a_id) {
          list(
            alias        = input[[paste0("name_", param_id, "_", a_id)]] %||% "",
            destinations = input[[paste0("destinations_", param_id, "_", a_id)]],
            min          = input[[paste0("min_", param_id, "_", a_id)]],
            max          = input[[paste0("max_", param_id, "_", a_id)]],
            logical_val  = input[[paste0("value_bool_", param_id, "_", a_id)]],
            literal_val  = input[[paste0("value_str_", param_id, "_", a_id)]],
            json_array   = input[[paste0("values_", param_id, "_", a_id)]],
            
            predator_sp     = input[[paste0("pref_predator_sp_", param_id, "_", a_id)]],
            predator_instars = input[[paste0("pref_predator_instars_", param_id, "_", a_id)]],
            prey_sp         = input[[paste0("pref_prey_sp_", param_id, "_", a_id)]],
            prey_instars     = input[[paste0("pref_prey_instars_", param_id, "_", a_id)]]
          )
        })
      }

      global_store$phase_3$defined_parameters <- compiled_bounds_list
    }

    return(list(
      is_valid    = phase_3_is_valid,
      commit_data = commit_phase_3_data_transaction
    ))
  })
}