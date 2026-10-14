#' Phase 2: Define Optimization Variables Server (Merged Skeleton)
#'
#' @description Orchestrates dynamic UI generation for variable creation, tracks parameter 
#' class definitions, and provides atomic removal hooks for single card elements.
#' @param id Unique module namespace identifier string.
#' @param global_store The global reactiveValues store mapping transactional pipeline data metrics.
#' @import shiny
#' @export
mod_optimiser_phase_2_server <- function(id, global_store, current_phase) {
  moduleServer(id, function(input, output, session) {
    ns <- session$ns
    message("[Phase 2 Merged Server] Initial configuration engine deployed successfully.")
    
    # Local reactive registries to track user added optimization variables
    inserted_variables <- reactiveValues(
      ids = c()  # Integer tracking vector (1, 2, 3...)
    )

    # State tracking matrix mapping sub-pair rows for each created optimization variable
    pair_rows_state            <- reactiveValues() # List of vectors: pair_rows_state[["1"]] = c(1, 2)
    next_pair_row_id_state     <- reactiveValues() # Tracks next serial integer per variable snapshot
    registered_pair_handlers   <- reactiveVal(character(0)) # Avoids double observer injection loops

    # ============================================================================
    # UNIFIED CENTRAL ERROR DATA REGISTRY (Fase 2 Fusionada - Totalmente Desacoplada)
    # ============================================================================
    active_errors <- reactiveValues(
      empty_variables          = NULL,
      duplicate_aliases        = list(),
      boundary_conflicts       = list(),
      permutations_empty       = list(),
      abundances_sum_error     = list(),
      predator_instars_errors  = list(),
      prey_instars_errors      = list(),
      trophic_overlaps_errors  = list(),
      empty_destinations       = list(),
      coordinate_overlaps      = list()
    )

    # Panel superior de visualización de alertas combinadas de la fase
    output$display_phase_2_errors <- renderUI({
      all_errors <- shiny::reactiveValuesToList(active_errors)
      consolidated_errors_vector <- c()
      
      if (!is.null(all_errors$empty_variables))         consolidated_errors_vector <- c(consolidated_errors_vector, all_errors$empty_variables)
      if (length(all_errors$duplicate_aliases) > 0)     consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$duplicate_aliases))
      if (length(all_errors$boundary_conflicts) > 0)    consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$boundary_conflicts))
      if (length(all_errors$permutations_empty) > 0)    consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$permutations_empty))
      if (length(all_errors$abundances_sum_error) > 0)  consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$abundances_sum_error))
      if (length(all_errors$predator_instars_errors) > 0) consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$predator_instars_errors))
      if (length(all_errors$prey_instars_errors) > 0)     consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$prey_instars_errors))
      if (length(all_errors$trophic_overlaps_errors) > 0) consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$trophic_overlaps_errors))
      if (length(all_errors$empty_destinations) > 0)    consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$empty_destinations))
      if (length(all_errors$coordinate_overlaps) > 0)   consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$coordinate_overlaps))
      
      if (length(consolidated_errors_vector) == 0) return(NULL)
      
      div(class = "alert alert-danger mb-4 text-body",
          style = "border-left: 5px solid var(--bs-danger); background-color: var(--bs-card-bg); border-color: var(--bs-border-color);",
          div(class = "d-flex align-items-center mb-2 fw-bold fs-6", icon("circle-xmark", class = "me-2 text-danger"), "Phase 2 General Validation Errors Detected"),
          tags$ul(style = "margin-bottom: 0; padding-left: 20px;", 
              lapply(consolidated_errors_vector, function(err_msg) tags$li(err_msg))
          )
      )
    })

    # ============================================================================
    # CENTRAL EMBEDDED DIAGNOSTIC EVALUATOR PIPELINE (SOLID BLINDADO CONTRA NULL/NA)
    # ============================================================================
    observe({
      var_ids <- inserted_variables$ids
      
      # Si no hay IDs creados o la lista está vacía, evitamos evaluar ifs y salimos limpiamente
      if (is.null(var_ids) || length(var_ids) == 0) {
        active_errors$empty_variables         <- "Error: At least one optimization variable must be defined to run the configuration."
        active_errors$duplicate_aliases       <- list(); active_errors$boundary_conflicts <- list()
        active_errors$permutations_empty       <- list(); active_errors$abundances_sum_error <- list()
        active_errors$predator_instars_errors  <- list(); active_errors$prey_instars_errors <- list()
        active_errors$trophic_overlaps_errors  <- list(); active_errors$empty_destinations <- list()
        active_errors$coordinate_overlaps      <- list()
        return()
      } else {
        active_errors$empty_variables <- NULL
      }
      
      # Inicializar buffers temporales limpios por ciclo reactivo
      tmp_aliases = list(); tmp_ranges = list(); tmp_perms = list(); tmp_abundances = list()
      tmp_preds = list(); tmp_preys = list(); tmp_trophic_overlaps = list(); tmp_empty_dests = list()
      tmp_coord_overlaps = list()
      
      raw_all_aliases_vector <- character(0)
      global_coordinate_registry <- list() 
      
      # 1. Recolección segura de Alias identificadores
      for (v_id in var_ids) {
        alias_val <- input[[paste0("var_name_", v_id)]]
        if (!is.null(alias_val) && !is.na(alias_val) && trimws(alias_val) != "") {
          raw_all_aliases_vector <- c(raw_all_aliases_vector, trimws(alias_val))
        }
      }
      
      # 2. Análisis profundo por variable de decisión
      for (v_id in var_ids) {
        id_str      <- as.character(v_id)
        alias_val   <- input[[paste0("var_name_", v_id)]]
        param_class <- input[[paste0("var_type_", v_id)]]
        active_rows <- as.integer(pair_rows_state[[id_str]] %||% integer(0))
        
        # Sanitizar alias_val de forma segura
        alias_clean <- if(!is.null(alias_val) && !is.na(alias_val)) trimws(alias_val) else ""
        
        # Validación de Alias Único
        if (alias_clean == "") {
          tmp_aliases[[id_str]] <- paste0("Error [Variable #", v_id, "]: Alias identifier cannot be left blank.")
        } else if (sum(raw_all_aliases_vector == alias_clean, na.rm = TRUE) > 1) {
          tmp_aliases[[id_str]] <- paste0("Error: The custom identifier name '", alias_clean, "' is duplicated. Each variable must have a unique namespace.")
        }
        
        # Validación de Campo de Destinos Pares Vacíos o Incompletos
        has_at_least_one_valid_pair <- FALSE
        
        if (!is.null(active_rows) && length(active_rows) > 0) {
          for (p_row_id in active_rows) {
            if (is.na(p_row_id)) next
            
            ck <- paste0(v_id, "_", p_row_id)
            target_file <- input[[paste0("pair_file_", ck)]]
            target_path <- input[[paste0("pair_path_", ck)]]
            
            if (!is.null(target_file) && !is.na(target_file) && target_file != "" && 
                !is.null(target_path) && !is.na(target_path) && target_path != "") {
              
              has_at_least_one_valid_pair <- TRUE
              coordinate_key <- paste(target_file, target_path, sep = "||")
              
              if (coordinate_key %in% names(global_coordinate_registry)) {
                offending_source <- global_coordinate_registry[[coordinate_key]]
                collision_msg <- paste0("Error [Destinations Collision]: The pair mapping File [", target_file, "] and Path [", target_path, "] collapses vertically. Simultaneously assigned to Variable #", v_id, " and Variable #", offending_source, ".")
                tmp_coord_overlaps[[id_str]] <- collision_msg
                tmp_coord_overlaps[[as.character(offending_source)]] <- collision_msg
              } else {
                global_coordinate_registry[[coordinate_key]] <- v_id
              }
            }
          }
        }
        
        # Cláusula de control para evitar evaluar strings vacíos/nulos en param_class
        class_ready <- !is.null(param_class) && !is.na(param_class) && param_class != ""
        
        if (!has_at_least_one_valid_pair && class_ready) {
          tmp_empty_dests[[id_str]] <- paste0("Error [Variable #", v_id, "]: Mapped destinations coordinate list cannot be empty. Associate at least one File and Internal Path pair.")
        }
        
        # --- REPLICACIÓN FIEL DE ERRORES SEGÚN CLASE PARAMÉTRICA ---
        if (class_ready) {
          
          # Rangos (Integer, RealValued, Preference, Profitability)
          if (param_class %in% c("Integer", "RealValued", "Preference", "Profitability")) {
            min_val <- input[[paste0("min_v_", v_id)]]
            max_val <- input[[paste0("max_v_", v_id)]]
            
            if (!is.null(min_val) && !is.null(max_val) && !is.na(min_val) && !is.na(max_val)) {
              if (min_val > max_val) {
                tmp_ranges[[id_str]] <- paste0("Error [Variable #", v_id, " - Range]: Continuous minimum bound cannot exceed the maximum tolerance limit.")
              }
            }
          }
          
          # Permutaciones Categorías
          if (identical(param_class, "Permutation")) {
            elements_current <- input[[paste0("elements_v_", v_id)]]
            if (is.null(elements_current) || length(elements_current) == 0) {
              tmp_perms[[id_str]] <- paste0("Error [Variable #", v_id, " - Categorical]: Selected choices list cannot be empty. You must toggle at least one option token.")
            }
          }
          
          # Vector de Abundancias Larvarias (Suma obligada = 1.0)
          if (identical(param_class, "Abundances")) {
            instar_count <- 5L
            densities_vals <- numeric(instar_count)
            is_any_density_empty <- FALSE
            
            for (v_idx in seq_len(instar_count)) {
              val_input <- input[[paste0("density_elem_v_", v_id, "_", v_idx)]]
              if (is.null(val_input) || is.na(val_input)) {
                is_any_density_empty <- TRUE
              } else {
                densities_vals[v_idx] <- val_input
              }
            }
            
            if (is_any_density_empty) {
              tmp_abundances[[id_str]] <- paste0("Error [Variable #", v_id, " - Abundances]: All target larval density parameters fields must be filled with valid coefficients.")
            } else {
              total_sum <- sum(densities_vals)
              if (abs(total_sum - 1.0) > 1e-5) {
                tmp_abundances[[id_str]] <- paste0("Error [Variable #", v_id, " - Abundances]: The sum of the density vector coefficients is ", round(total_sum, 4), ". It must equal exactly 1.0.")
              }
            }
          }
          
          # Matrices Tróficas (Preference & Profitability)
          if (param_class %in% c("Preference", "Profitability")) {
            pred_ins_vector <- input[[paste0("pref_predator_instars_v_", v_id)]]
            prey_ins_vector <- input[[paste0("pref_prey_instars_v_", v_id)]]
            
            if (is.null(pred_ins_vector) || length(pred_ins_vector) == 0) {
              tmp_preds[[id_str]] <- paste0("Error [Variable #", v_id, "]: Predator lifecycle stages list cannot be left empty.")
            }
            if (is.null(prey_ins_vector) || length(prey_ins_vector) == 0) {
              tmp_preys[[id_str]] <- paste0("Error [Variable #", v_id, "]: Targeted prey lifecycle stages list cannot be left empty.")
            }
          }
        }
      }
      
      # Vuelco atómico seguro a la reactividad central
      active_errors$duplicate_aliases       <- tmp_aliases
      active_errors$boundary_conflicts      <- tmp_ranges
      active_errors$permutations_empty      <- tmp_perms
      active_errors$abundances_sum_error    <- tmp_abundances
      active_errors$predator_instars_errors  <- tmp_preds
      active_errors$prey_instars_errors      <- tmp_preys
      active_errors$empty_destinations       <- tmp_empty_dests
      active_errors$coordinate_overlaps      <- tmp_coord_overlaps
    })

    # --- METADATA VARIABLES LOCALIZED PASSIVE ERROR RENDERERS ---
    # Enlaza las cadenas de texto del registro centralizado debajo de cada input de la tarjeta
    observe({
      var_ids <- inserted_variables$ids
      req(length(var_ids) > 0)
      
      lapply(var_ids, function(var_id) {
        id_str <- as.character(var_id)
        
        output[[paste0("error_local_alias_v_", var_id)]]       <- renderUI({ req(active_errors$duplicate_aliases[[id_str]]); p(icon("circle-xmark"), active_errors$duplicate_aliases[[id_str]], class = "text-danger small fw-bold mt-1 mb-0") })
        output[[paste0("error_local_range_", var_id)]]         <- renderUI({ req(active_errors$boundary_conflicts[[id_str]]); p(icon("circle-xmark"), active_errors$boundary_conflicts[[id_str]], class = "text-danger small fw-bold mt-1 mb-0") })
        output[[paste0("error_local_permutation_", var_id)]]   <- renderUI({ req(active_errors$permutations_empty[[id_str]]); p(icon("circle-xmark"), active_errors$permutations_empty[[id_str]], class = "text-danger small fw-bold mt-1 mb-0") })
        output[[paste0("error_local_abundance_", var_id)]]     <- renderUI({ req(active_errors$abundances_sum_error[[id_str]]); p(icon("circle-xmark"), active_errors$abundances_sum_error[[id_str]], class = "text-danger small fw-bold mt-1 mb-0") })
        output[[paste0("error_local_dest_empty_v_", var_id)]] <- renderUI({ req(active_errors$empty_destinations[[id_str]]); p(icon("circle-xmark"), active_errors$empty_destinations[[id_str]], class = "text-danger small fw-bold mt-1 mb-0") })
        output[[paste0("error_local_overlap_v_", var_id)]]    <- renderUI({ req(active_errors$coordinate_overlaps[[id_str]]); p(icon("triangle-exclamation"), active_errors$coordinate_overlaps[[id_str]], class = "text-danger small fw-bold mt-1 mb-0") })
      })
    })

    output$display_phase_2_errors <- renderUI({
      all_errors <- shiny::reactiveValuesToList(active_errors)
      consolidated_errors_vector <- c()
      
      if (!is.null(all_errors$empty_variables))         consolidated_errors_vector <- c(consolidated_errors_vector, all_errors$empty_variables)
      if (length(all_errors$duplicate_aliases) > 0)     consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$duplicate_aliases))
      if (length(all_errors$boundary_conflicts) > 0)    consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$boundary_conflicts))
      if (length(all_errors$permutations_empty) > 0)    consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$permutations_empty))
      if (length(all_errors$abundances_sum_error) > 0)  consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$abundances_sum_error))
      if (length(all_errors$predator_instars_errors) > 0) consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$predator_instars_errors))
      if (length(all_errors$prey_instars_errors) > 0)     consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$prey_instars_errors))
      if (length(all_errors$empty_destinations) > 0)    consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$empty_destinations))
      if (length(all_errors$coordinate_overlaps) > 0)   consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$coordinate_overlaps))
      
      if (length(consolidated_errors_vector) == 0) return(NULL)
      
      div(class = "alert alert-danger mb-4 text-body",
          style = "border-left: 5px solid var(--bs-danger); background-color: var(--bs-card-bg); border-color: var(--bs-border-color);",
          div(class = "d-flex align-items-center mb-2 fw-bold fs-6", icon("circle-xmark", class = "me-2 text-danger"), "Phase 2 General Validation Errors Detected"),
          tags$ul(style = "margin-bottom: 0; padding-left: 20px;", 
              lapply(consolidated_errors_vector, function(err_msg) tags$li(err_msg))
          )
      )
    })
    
    # ============================================================================
    # EXTRACTOR DE METADATOS INTEGRADO (Esquemas + Base Config)
    # ============================================================================
    base_config_path <- reactive({ global_store$phase_1$system_paths$base_configuration })
    detected_version <- reactive({ global_store$phase_1$system_paths$detected_schema_version })
    
    # 1. Compila el catálogo completo de parámetros leyendo los esquemas JSON del paquete weaverRHub
    current_schema_catalog <- reactive({
      req(detected_version())
      ver <- detected_version()
      
      # Carga las hojas terminales desde la matriz de recursos embebidos
      df <- load_all_version_schemas(ver, include_basic_strings = FALSE)
      
      # Filtra y valida las filas contra la existencia real de directorios en el volumen
      filter_catalog_by_available_config_json(df, base_config_path())
    })
    
    # 2. Resuelve de forma reactiva la lista exacta de archivos físicos y sus rutas internas válidas
    available_coordinates_map <- reactive({
      catalog <- current_schema_catalog()
      req(nrow(catalog) > 0, base_config_path() != "")
      
      # Creamos una lista donde cada archivo tiene asignado un vector con sus JSON paths válidos ($)
      files_to_paths_list <- list()
      
      for (i in seq_len(nrow(catalog))) {
        schema_file_name <- catalog$schema_file[i]
        json_path_token  <- catalog$json_path[i]
        
        # Obtenemos los archivos reales (ej: species/predator.json, species/prey.json) que usan este esquema
        f_destinations <- resolve_config_destinations_by_schema(schema_file_name, base_config_path())
        
        for (f_path in f_destinations) {
          if (is.null(files_to_paths_list[[f_path]])) {
            files_to_paths_list[[f_path]] <- character(0)
          }
          # Forzamos coerciones atómicas tempranas de texto plano
          files_to_paths_list[[f_path]] <- unique(c(as.character(files_to_paths_list[[f_path]]), as.character(json_path_token)))
        }

      }
      return(files_to_paths_list)
    })


    # --- SUBSURFACE HELPER: SINGLE DESTINATION COORDINATE PAIR ROW CONSTRUCTOR ---
    render_destination_pair_row_ui <- function(var_id, pair_row_id, display_idx) {
      ns_pair  <- session$ns
      comp_key <- paste0(var_id, "_", pair_row_id)
      
      # Recuperamos el mapa de archivos reales válidos
      coords_map <- available_coordinates_map()
      valid_files <- names(coords_map)
      
      # Capturamos de forma segura las selecciones previas si la tarjeta se está redibujando
      saved_file <- input[[paste0("pair_file_", comp_key)]] %||% ""
      saved_path <- input[[paste0("pair_path_", comp_key)]] %||% ""
      
      # Si ya hay un archivo seleccionado, filtramos las rutas internas de su esquema; si no, vacío
      path_choices <- if (saved_file != "" && saved_file %in% valid_files) coords_map[[saved_file]] else character(0)
      
      div(
        id = ns_pair(paste0("pair_row_container_", comp_key)),
        class = "row g-2 align-items-center mb-2 px-2 py-1 rounded border border-light bg-white shadow-xs",
        
        # Columna 1: Indicador numérico de fila
        div(class = "col-auto",
            tags$span(class = "badge bg-secondary font-monospace", style = "font-size: 0.75rem;", display_idx)
        ),
        
        # Columna 2: Selector del Archivo Real (Filtrado por existencia en la Base Config)
        div(class = "col-md-5",
            selectizeInput(
              inputId  = ns_pair(paste0("pair_file_", comp_key)),
              label    = NULL,
              choices  = c("Choose JSON target asset..." = "", valid_files),
              selected = saved_file,
              options  = list(placeholder = "Select configuration JSON file...")
            )
        ),
        
        # Columna 3: Selector de la Ruta Interna ($) filtrada según el Esquema Técnico de ese archivo
        div(class = "col-md-5",
            selectizeInput(
              inputId  = ns_pair(paste0("pair_path_", comp_key)),
              label    = NULL,
              choices  = c("Select relative path ($)..." = "", path_choices),
              selected = saved_path,
              options  = list(placeholder = "Select schema path...", create = FALSE)
            )
        ),
        
        # Columna 4: Botón de eliminación
        div(class = "col-auto text-end",
            actionButton(
              inputId = ns_pair(paste0("remove_pair_btn_", comp_key)),
              label   = NULL,
              icon    = icon("xmark"),
              class   = "btn-outline-danger btn-sm border-0",
              style   = "padding: 4px 8px;"
            )
        )
      )
    }


    # --- HELPER: SINGLE VARIABLE CARD UI CONSTRUCTOR ---
    render_single_variable_card_ui <- function(var_id) {
      var_ns_func <- session$ns
      id_str <- as.character(var_id)
      
      tags$div(
        id = var_ns_func(paste0("variable_card_container_", var_id)),
        class = "mb-3 border secondary-subtle shadow-sm card optimiser-input-card",
        style = "padding: 15px;",
        
        # Header Row containing the title tracking card and the removal trash hook button
        div(class = "d-flex justify-content-between align-items-center mb-3 pb-2 border-bottom",
            tags$span(class = "fw-bold text-primary fs-6",
                icon("gear", class = "me-1"), "Optimization Variable #", tags$code(id_str)
            ),
            actionButton(var_ns_func(paste0("remove_var_", var_id)), label = NULL, icon = icon("trash"), class = "btn-outline-danger btn-sm")
        ),
        
        # Row 1: Variable Name and Parameter Type Selectors
        div(class = "row g-3 mb-2",
            div(class = "col-md-6",
                textInput(var_ns_func(paste0("var_name_", var_id)), label = "Custom Variable Alias Identifier:", value = paste0("var_opt_", var_id))
            ),
            div(class = "col-md-6",
                selectInput(
                  inputId  = var_ns_func(paste0("var_type_", var_id)), 
                  label    = "Parameter Operational Class:", 
                  choices  = c("RealValued", "Integer", "Boolean", "Permutation", "Abundances", "Preference", "Profitability"),
                  selected = "RealValued"
                )
            )
        ),
        
        # Dynamic slots to inject typed numerical ranges or pairs matrix destinations later
        uiOutput(var_ns_func(paste0("var_body_conditional_ui_", var_id)))
      )
    }
    
    # ============================================================================
    # ATOMIC VARIABLE DESTRUCTION OBSERVER LOOP
    # ============================================================================
    observe({
      active_ids <- inserted_variables$ids
      req(length(active_ids) > 0)
      
      lapply(active_ids, function(var_id) {
        observeEvent(input[[paste0("remove_var_", var_id)]], {
          isolate({
            inserted_variables$ids <- setdiff(inserted_variables$ids, var_id)
            
            # CORRECCIÓN DE PERSISTENCIA: Limpia el nodo del almacén global
            v_name <- input[[paste0("var_name_", var_id)]]
            if (!is.null(v_name)) {
              global_store$phase_2$defined_parameters[[v_name]] <- NULL
            }
          })
        }, once = TRUE)
      })
    })

    # ============================================================================
    # UNIFIED PERSISTENT MATRIX RENDER ENGINE (Inspirado en la metodología de Fase 1)
    # ============================================================================
    
    # Sincronización analítica al cambiar de fase basada en el caché transaccional existente
    observeEvent(current_phase(), {
      req(current_phase() == 2)
      cached_params <- global_store$phase_2$defined_parameters
      
      if (!is.null(cached_params) && length(cached_params) > 0) {
        isolate({
          inserted_variables$ids <- seq_along(cached_params)
        })
      }
    })

    # Escuchador protegido y determinista para el botón de adición de variables
    observeEvent(input$add_variable_btn, {
      current_ids <- inserted_variables$ids
      next_id <- if(length(current_ids) == 0) 1L else max(current_ids) + 1L
      inserted_variables$ids <- c(current_ids, next_id)
    }, ignoreInit = TRUE)

    # El renderizador central calcula y dibuja la interfaz basándose en la memoria activa
    output$render_matrix_variables_workspace <- renderUI({
      active_ids    <- inserted_variables$ids
      cached_params <- global_store$phase_2$defined_parameters
      
      if (length(active_ids) == 0 && (is.null(cached_params) || length(cached_params) == 0)) {
        return(p(class = "text-muted small italic text-center py-4", "No optimization variables defined yet. Click 'Add Optimization Variable' to begin."))
      }
      
      if (length(active_ids) == 0 && length(cached_params) > 0) {
        active_ids <- seq_along(cached_params)
      }
      
      # Compilamos la lista de tarjetas inyectando los valores guardados de la sesión
      cards_layout_list <- lapply(seq_along(active_ids), function(idx) {
        var_id <- active_ids[idx]
        id_str <- as.character(var_id)
        
        saved_meta  <- NULL
        saved_alias <- paste0("var_opt_", var_id)
        
        if (!is.null(cached_params) && idx <= length(cached_params)) {
          saved_alias <- names(cached_params)[idx]
          saved_meta  <- cached_params[[saved_alias]]
        }
        
        # Recuperamos de forma persistente las firmas guardadas en el cliente o los valores del caché
        curr_alias <- input[[paste0("var_name_", var_id)]] %||% saved_alias
        curr_class <- input[[paste0("var_type_", var_id)]] %||% saved_meta$parameter_class %||% ""
        
        # Inicialización en caliente de los sub-estados de las filas de pares para evitar resets
        if (!is.null(saved_meta$tied_destinations_coordinates) && is.null(pair_rows_state[[id_str]])) {
          pair_rows_state[[id_str]] <- seq_along(saved_meta$tied_destinations_coordinates)
          next_pair_row_id_state[[id_str]] <- length(saved_meta$tied_destinations_coordinates) + 1L
        }
        
        var_ns_func <- session$ns
        
        # Re-construcción estática de la tarjeta de control adaptativa
        tags$div(
          id = var_ns_func(paste0("variable_card_container_", var_id)),
          class = "mb-3 border secondary-subtle shadow-sm card optimiser-input-card",
          style = "padding: 15px;",
          
          div(class = "d-flex justify-content-between align-items-center mb-3 pb-2 border-bottom",
              tags$span(class = "fw-bold text-primary fs-6",
                  icon("gear", class = "me-1"), "Optimization Variable #", tags$code(as.character(idx))
              ),
              actionButton(var_ns_func(paste0("remove_var_", var_id)), label = NULL, icon = icon("trash"), class = "btn-outline-danger btn-sm")
          ),
          
          div(class = "row g-3 mb-2",
              div(class = "col-md-6",
                  textInput(var_ns_func(paste0("var_name_", var_id)), label = "Custom Variable Alias Identifier:", value = curr_alias)
              ),
              div(class = "col-md-6",
                  selectInput(
                    inputId  = var_ns_func(paste0("var_type_", var_id)), 
                    label    = "Parameter Operational Class:", 
                    choices  = c("RealValued", "Integer", "Boolean", "Permutation", "Abundances", "Preference", "Profitability"),
                    selected = if(curr_class == "") "RealValued" else curr_class
                  )
              )
          ),
          
          uiOutput(var_ns_func(paste0("var_body_conditional_ui_", var_id)))
        )
      })
      
      tags$div(class = "persistent-variables-wrapper", do.call(tagList, cards_layout_list))
    })

    # ============================================================================
    # DYNAMIC CONDITIONAL INTERNAL CARD RENDERING WORKFLOW
    # ============================================================================
    # Monitoriza continuamente el tipo seleccionado en cada tarjeta y despliega
    # exactamente el mismo conjunto de campos numéricos y estructurales de la versión anterior.
    observe({
      var_ids <- inserted_variables$ids
      req(length(var_ids) > 0)
      
      lapply(var_ids, function(var_id) {
        id_str <- as.character(var_id)
        
        output[[paste0("var_body_conditional_ui_", var_id)]] <- renderUI({
          selected_class <- input[[paste0("var_type_", var_id)]]
          req(!is.null(selected_class), selected_class != "")
          
          # ------------------------------------------------------------------
          # 1. ESQUELETO BASE PARA EL CAMPO REIMPLEMENTADO DE DESTINOS PARES
          # ------------------------------------------------------------------
          destinations_pair_grid_layout <- tags$div(
            class = "p-3 mb-3 border rounded shadow-sm tied-destinations-container",
            tags$b(icon("network-wired"), " Target File & Internal JSON Paths Assignment:"),
            p(class = "text-muted small mb-2", "This variable's value will be tied identically across the following mapped criteria coordinates:"),
            
            uiOutput(session$ns(paste0("var_destinations_pairs_anchor_", var_id)))
          )
          
          # ------------------------------------------------------------------
          # 2. DEFINICIÓN DE CAMPOS SEGÚN CLASE (RECUPERADOS DE PHASE 3 ANTIGUA)
          # ------------------------------------------------------------------
          specific_fields_layout <- NULL
          
          # CASO A: INTEGER & REALVALUED
          if (selected_class %in% c("Integer", "RealValued")) {
            input_class <- if (selected_class == "Integer") "integer-input" else "decimal-input"
            step_attr   <- if (selected_class == "Integer") 1L else 0.01
            min_attr    <- if (selected_class == "Integer") 0L else 0.0
            
            specific_fields_layout <- tags$div(
              class = "numeric-range-container mt-2", `data-var-id` = var_id,
              bslib::layout_columns(
                col_widths = c(6, 6),
                div(class = input_class, numericInput(session$ns(paste0("min_v_", var_id)), "Minimum Continuous Bound:", value = min_attr, min = min_attr, step = step_attr)),
                div(class = input_class, numericInput(session$ns(paste0("max_v_", var_id)), "Maximum Tolerance Limit:", value = 1.0, min = min_attr, step = step_attr))
              ),
              uiOutput(session$ns(paste0("error_local_range_", var_id)))
            )
            
          # CASO B: BOOLEAN TOGGLE
          } else if (selected_class == "Boolean") {
            switch_input_id <- paste0("value_bool_v_", var_id)
            current_switch_value <- if (!is.null(input[[switch_input_id]])) input[[switch_input_id]] else FALSE
            
            specific_fields_layout <- bslib::layout_columns(
              col_widths = c(6, 6),
              div(class = "pt-2 pb-1 d-flex flex-column justify-content-center",
                  tags$label(class = "control-label fw-bold mb-1", "Boolean Optimization Constraint:"),
                  shinyWidgets::switchInput(
                    inputId  = session$ns(switch_input_id),
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
            
          # CASO C: CATEGORICAL MATRICES (PERMUTATION)
          } else if (selected_class == "Permutation") {
            cached_version <- global_store$phase_1$system_paths$detected_schema_version
            choices_from_schema <- c("Option_A", "Option_B", "Option_C")
            
            elements_input_id <- paste0("elements_v_", var_id)
            elements_current  <- if (elements_input_id %in% names(input)) input[[elements_input_id]] else choices_from_schema
            
            specific_fields_layout <- tags$div(
              class = "permutation-buttons-container mt-2", `data-var-id` = var_id,
              shinyWidgets::checkboxGroupButtons(
                inputId    = session$ns(elements_input_id),
                label      = "Target Categorical Selection Matrix (Click to toggle blocks):",
                choices    = choices_from_schema,
                selected   = elements_current,
                justified  = TRUE,
                individual = FALSE,
                status     = "primary"
              ),
              uiOutput(session$ns(paste0("error_local_permutation_", var_id)))
            )
            
          # CASO D: POPULATION DENSITIES VECTOR (ABUNDANCES)
          } else if (selected_class == "Abundances") {
            instar_count <- 5L 
            normalize_btn_id <- paste0("normalize_btn_v_", var_id)
            
            local({
              v_id <- var_id; i_count <- instar_count; n_btn_id <- normalize_btn_id
              observeEvent(input[[n_btn_id]], {
                vals <- numeric(i_count)
                for (k in seq_len(i_count)) {
                  vals[k] <- input[[paste0("density_elem_v_", v_id, "_", k)]] %||% 0.0
                }
                total <- sum(vals)
                norm_vals <- if (total == 0) rep(1 / i_count, i_count) else vals / total
                norm_vals <- round(norm_vals, 4)
                diff_residual <- 1.0 - sum(norm_vals)
                norm_vals[i_count] <- norm_vals[i_count] + diff_residual
                
                for (k in seq_len(i_count)) {
                  updateNumericInput(session, paste0("density_elem_v_", v_id, "_", k), value = norm_vals[k])
                }
              }, ignoreInit = TRUE)
            })
            
            vector_inputs <- lapply(seq_len(instar_count), function(v_idx) {
              sub_input_id <- paste0("density_elem_v_", var_id, "_", v_idx)
              current_val  <- input[[sub_input_id]] %||% 0.0
              div(class = "col",
                  div(class = "decimal-input", 
                      numericInput(session$ns(sub_input_id), label = paste("Instar Stage", v_idx), value = current_val, min = 0, step = 0.01)
                  )
              )
            })
            
            specific_fields_layout <- tags$div(
              class = "mt-3 p-3 border rounded abundances-vector-container shadow-sm",
              div(class = "d-flex justify-content-between align-items-center mb-2 pb-2 border-bottom border-secondary-subtle",
                  tags$label(class = "control-label fw-bold mb-0 text-primary", "Instar Densities Weight Vector Allocation (densitiesVector):"),
                  actionButton(session$ns(normalize_btn_id), "Normalize Matrix to 1.0", class = "btn btn-outline-success btn-sm fw-bold", icon = icon("wand-magic-sparkles"))
              ),
              p(class = "text-muted small mb-3", "The continuous biological distribution weights must aggregate vertically and equal exactly to 1.0:"),
              div(class = "row row-cols-1 row-cols-sm-3 row-cols-md-5 g-3 mb-1", do.call(tagList, vector_inputs)),
              uiOutput(session$ns(paste0("error_local_abundance_", var_id)))
            )
            
          # CASO E: TROPHIC INTERACTIONS (PREFERENCE & PROFITABILITY)
          } else if (selected_class %in% c("Preference", "Profitability")) {
            config_root <- global_store$phase_1$system_paths$base_configuration
            
            animal_names   <- c("Predator_Species_A", "Predator_Species_B")
            all_prey_names <- c("Predator_Species_A", "Predator_Species_B", "Resource_Patch_X")
            
            pred_sp_id  <- paste0("pref_predator_sp_v_", var_id)
            pred_ins_id <- paste0("pref_predator_instars_v_", var_id)
            prey_sp_id  <- paste0("pref_prey_sp_v_", var_id)
            prey_ins_id <- paste0("pref_prey_instars_v_", var_id)
            
            selected_pred_sp <- input[[pred_sp_id]]  %||% animal_names[1]
            selected_prey_sp <- input[[prey_sp_id]]  %||% all_prey_names[1]
            
            selected_pred_ins <- if (pred_ins_id %in% names(input)) input[[pred_ins_id]] else as.character(1:4)
            selected_prey_ins <- if (prey_ins_id %in% names(input)) input[[prey_ins_id]] else as.character(1:4)
            
            specific_fields_layout <- tags$div(
              class = "mt-3 p-3 border rounded preference-cell-container shadow-sm",
              tags$label(class = "control-label fw-bold mb-2 text-primary", paste0(selected_class, " Interactive Cell Matrix Mapping (LinksCell):")),
              p(class = "text-muted small mb-3", "Define boundaries bounds between targeted trophic predator levels and prey datasets fields:"),
              
              bslib::layout_columns(
                col_widths = c(6, 6),
                div(class = "p-3 border rounded bg-white shadow-sm", style = "border-left: 4px solid var(--bs-primary) !important;",
                  tags$h6(class = "fw-bold text-dark mb-3", icon("skull"), " Predator Specification Matrix"),
                  selectInput(session$ns(pred_sp_id), "Select Active Predator Species:", choices = animal_names, selected = selected_pred_sp),
                  selectizeInput(session$ns(pred_ins_id), "Target Predator Instars Stages:", choices = as.character(1:4), selected = selected_pred_ins, multiple = TRUE, options = list(plugins = list("remove_button")))
                ),
                div(class = "p-3 border rounded bg-white shadow-sm", style = "border-left: 4px solid #18bc9c !important;",
                  tags$h6(class = "fw-bold text-dark mb-3", icon("shrimp"), " Prey Target Specification Matrix"),
                  selectInput(session$ns(prey_sp_id), "Select Prey Entity (Animal/Resource):", choices = all_prey_names, selected = selected_prey_sp),
                  selectizeInput(session$ns(prey_ins_id), "Target Prey Instars Stages:", choices = as.character(1:4), selected = selected_prey_ins, multiple = TRUE, options = list(plugins = list("remove_button")))
                )
              ),
              div(class = "decimal-input mt-3",
                bslib::layout_columns(
                  col_widths = c(6, 6),
                  numericInput(session$ns(paste0("min_v_", var_id)), "Cell Interaction Min Probability:", value = 0.0, min = 0.0, max = 1.0, step = 0.01),
                  numericInput(session$ns(paste0("max_v_", var_id)), "Cell Interaction Max Probability:", value = 1.0, min = 0.0, max = 1.0, step = 0.01)
                ) # <- PARÉNTESIS CORREGIDO AQUÍ
              ),
              uiOutput(session$ns(paste0("error_local_trophic_overlap_", var_id)))
            )
          }

          # ------------------------------------------------------------------
          # 3. ENSAMBLAJE ESTRUCTURAL FINAL DE LA VARIABLE
          # ------------------------------------------------------------------
          tagList(
            destinations_pair_grid_layout,
            hr(class = "my-3"),
            specific_fields_layout
          )
        })


        # Initialize internal sub-rows lists states if not already cached in the memory cycle
        if (is.null(pair_rows_state[[id_str]])) {
          pair_rows_state[[id_str]] <- 1L
          next_pair_row_id_state[[id_str]] <- 2L
        }
        
        # --- SUB-HANDLER: ADD DESTINATION PAIR EVENT ---
        if (!id_str %in% registered_pair_handlers()) {
          observeEvent(input[[paste0("add_pair_to_var_", var_id)]], {
            current_rows <- pair_rows_state[[id_str]] %||% 1L
            next_row_id  <- next_pair_row_id_state[[id_str]] %||% (max(current_rows) + 1L)
            
            pair_rows_state[[id_str]] <- c(current_rows, as.integer(next_row_id))
            next_pair_row_id_state[[id_str]] <- as.integer(next_row_id) + 1L
          }, ignoreInit = TRUE)
          
          registered_pair_handlers(c(registered_pair_handlers(), id_str))
        }
        
        # --- SUB-RENDERER: UNROLLING INDIVIDUAL PAIRS CARDS GRID ---
        output[[paste0("var_destinations_pairs_anchor_", var_id)]] <- renderUI({
          active_rows <- as.integer(pair_rows_state[[id_str]] %||% 1L)
          
          rows_layouts <- lapply(seq_along(active_rows), function(idx) {
            p_row_id <- active_rows[idx]
            comp_key_row <- paste0(var_id, "_", p_row_id)
            
            observeEvent(input[[paste0("remove_pair_btn_", comp_key_row)]], {
              rows_now <- pair_rows_state[[id_str]] %||% 1L
              if (length(rows_now) > 1) {
                removeUI(selector = paste0("#", session$ns(paste0("pair_row_container_", comp_key_row))), immediate = TRUE)
                pair_rows_state[[id_str]] <- setdiff(rows_now, p_row_id)
              }
            }, once = TRUE, ignoreInit = TRUE)

            return(render_destination_pair_row_ui(var_id, p_row_id, display_idx = idx))
          })
          
          tags$div(
            class = "pairs-grid-subcontainer",
            do.call(tagList, rows_layouts),
            
            div(class = "mt-2 pt-2 border-top border-secondary-subtle text-start",
                actionButton(
                  inputId = session$ns(paste0("add_pair_to_var_", var_id)),
                  label   = "Add Destination Pair Matrix Element",
                  class   = "btn btn-outline-warning btn-xs fw-bold font-monospace text-dark",
                  icon    = icon("plus"),
                  style   = "font-size: 0.75rem; padding: 3px 8px;"
                )
            )
          )
        })
      })
    })

    # --- DESACOPLAMIENTO DE SEGURIDAD INTER-FIELD DEPENDENCY OBSERVER ---
    # Sincroniza los JSON paths del esquema en el cliente sin inyectar entornos en la UI
    observe({
      var_ids <- inserted_variables$ids
      req(length(var_ids) > 0)
      
      for (v_id in var_ids) {
        id_str <- as.character(v_id)
        active_rows <- as.integer(pair_rows_state[[id_str]] %||% integer(0))
        
        # CORRECCIÓN: Si no hay filas instanciadas en este ciclo, abortamos de forma segura
        if (length(active_rows) == 0) next
        
        for (p_row_id in active_rows) {
          local({
            ck <- paste0(v_id, "_", p_row_id)
            
            observeEvent(input[[paste0("pair_file_", ck)]], {
              req(!is.null(input[[paste0("pair_file_", ck)]]))
              
              selected_file <- input[[paste0("pair_file_", ck)]]
              coords_map    <- available_coordinates_map()
              
              if (!is.null(selected_file) && selected_file != "" && selected_file %in% names(coords_map)) {
                raw_paths <- as.character(coords_map[[selected_file]])
                updateSelectizeInput(
                  session  = session,
                  inputId  = paste0("pair_path_", ck),
                  choices  = c("Select relative path ($)..." = "", raw_paths),
                  selected = ""
                )
              } else {
                updateSelectizeInput(
                  session = session, 
                  inputId = paste0("pair_path_", ck), 
                  choices = c("Select relative path ($)..." = "")
                )
              }
            }, ignoreInit = TRUE)
          })
        }
      }
    })
    
    # ============================================================================
    # PHASE VALIDITY EVALUATOR (EXPOSED TO ORCHESTRATOR)
    # ============================================================================
    # El evaluador retorna TRUE únicamente cuando hay variables creadas y la suma
    # total de mensajes de error de la fase es estrictamente cero.
    phase_2_is_valid <- reactive({
      # Si la lista está vacía, devolvemos FALSE limpiamente sin romper los botones
      if (is.null(inserted_variables$ids) || length(inserted_variables$ids) == 0) {
        return(FALSE)
      }
      
      all_errors_list <- shiny::reactiveValuesToList(active_errors)
      consolidated_error_vectors <- na.omit(unlist(all_errors_list))
      
      return(length(consolidated_error_vectors) == 0)
    })
    
    commit_phase_2_data_transaction <- function() {
      var_ids <- inserted_variables$ids
      compiled_variables_blueprint <- list()
      
      for (v_id in var_ids) {
        id_str <- as.character(v_id)
        v_name <- trimws(input[[paste0("var_name_", v_id)]]) %||% paste0("var_opt_", v_id)
        v_type <- input[[paste0("var_type_", v_id)]]
        
        # Recuperamos las filas de pares asociadas a esta tarjeta
        active_rows <- as.integer(pair_rows_state[[id_str]] %||% 1L)
        
        compiled_coordinates_pairs <- lapply(active_rows, function(p_row_id) {
          comp_key <- paste0(v_id, "_", p_row_id)
          list(
            target_json_file  = input[[paste0("pair_file_", comp_key)]],
            internal_json_path = trimws(input[[paste0("pair_path_", comp_key)]])
          )
        })
        
        # Unificamos el bloque estructural conservando los campos del tipo
        compiled_variables_blueprint[[v_name]] <- list(
          parameter_class = v_type,
          tied_destinations_coordinates = compiled_coordinates_pairs,
          
          # Rangos y restricciones numéricas según aplique
          min_bound       = input[[paste0("min_v_", v_id)]],
          max_bound       = input[[paste0("max_v_", v_id)]],
          boolean_toggle  = input[[paste0("value_bool_v_", v_id)]],
          selected_enums  = input[[paste0("elements_v_", v_id)]]
        )
      }
      
      global_store$phase_2$defined_parameters <- compiled_variables_blueprint
      message("[Phase 2 Store] Advanced Parameter Tying data matrices committed successfully.")
    }
    
    return(list(
      is_valid    = phase_2_is_valid,
      commit_data = commit_phase_2_data_transaction
    ))
  })
}