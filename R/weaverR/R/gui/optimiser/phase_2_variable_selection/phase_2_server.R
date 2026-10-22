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
    trophic_group_ids          <- reactiveValues()
    next_trophic_group_id      <- reactiveValues()
    registered_pair_handlers   <- reactiveVal(character(0)) # Avoids double observer injection loops
    registered_destination_handlers <- reactiveVal(character(0))
    registered_trophic_handlers <- reactiveVal(character(0))

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
      empty_destinations       = list(),
      coordinate_overlaps      = list()
    )

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
        active_errors$empty_destinations       <- list(); active_errors$coordinate_overlaps <- list()
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
        active_trophic_groups <- as.integer(trophic_group_ids[[id_str]] %||% 1L)
        
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
        class_ready <- !is.null(param_class) && !is.na(param_class) && param_class != ""
        
        needs_destinations <- !class_ready || !param_class %in% c("Preference", "Profitability")
        if (needs_destinations && !is.null(active_rows) && length(active_rows) > 0) {
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
        
        if (needs_destinations && !has_at_least_one_valid_pair && class_ready) {
          tmp_empty_dests[[id_str]] <- paste0("Error [Variable #", v_id, "]: Mapped destinations coordinate list cannot be empty. Associate at least one File and Internal Path pair.")
        }
        
        # --- REPLICACIÓN FIEL DE ERRORES SEGÚN CLASE PARAMÉTRICA ---
        if (class_ready) {
          
          # Rangos (Integer, RealValued, Abundances, Preference, Profitability)
          if (param_class %in% c("Integer", "RealValued", "Abundances", "Preference", "Profitability")) {
            min_val <- input[[paste0("min_v_", v_id)]]
            max_val <- input[[paste0("max_v_", v_id)]]
            
            if (!is.null(min_val) && !is.null(max_val) && !is.na(min_val) && !is.na(max_val)) {
              if (min_val > max_val) {
                tmp_ranges[[id_str]] <- paste0("Error [Variable #", v_id, " - Range]: Continuous minimum bound cannot exceed the maximum bound.")
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
                tmp_abundances[[id_str]] <- paste0("Error [Variable #", v_id, " - Abundances]: The sum of the density vector coefficients is ", sprintf("%.17g", total_sum), ". It must equal exactly 1.0.")
              }
            }
          }
          
          # Matrices Tróficas (Preference & Profitability)
          if (param_class %in% c("Preference", "Profitability")) {
            for (group_id in active_trophic_groups) {
              pred_species <- input[[paste0("pref_predator_sp_v_", v_id, "_", group_id)]]
              pred_instars <- input[[paste0("pref_predator_instars_v_", v_id, "_", group_id)]]
              prey_species <- input[[paste0("pref_prey_sp_v_", v_id, "_", group_id)]]
              prey_instars <- input[[paste0("pref_prey_instars_v_", v_id, "_", group_id)]]
              error_key <- paste(id_str, group_id, sep = "_")

              if (is.null(pred_species) || !nzchar(trimws(pred_species)) || is.null(pred_instars) || length(pred_instars) == 0) {
                tmp_preds[[error_key]] <- paste0("Error [Variable #", v_id, ", Cell Group #", group_id, "]: Select a predator and at least one predator instar.")
              }
              if (is.null(prey_species) || !nzchar(trimws(prey_species)) || is.null(prey_instars) || length(prey_instars) == 0) {
                tmp_preys[[error_key]] <- paste0("Error [Variable #", v_id, ", Cell Group #", group_id, "]: Select a prey and at least one prey instar.")
              }
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
      if (length(all_errors$empty_destinations) > 0)    consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$empty_destinations))
      if (length(all_errors$coordinate_overlaps) > 0)   consolidated_errors_vector <- c(consolidated_errors_vector, unlist(all_errors$coordinate_overlaps))
      
      if (length(consolidated_errors_vector) == 0) return(NULL)
      
      div(class = "alert alert-danger mb-4", role = "alert",
          div(class = "d-flex align-items-center mb-2 fw-bold fs-6", icon("circle-xmark", class = "me-2"), "General Validation Errors Detected"),
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

    # Catálogo biológico derivado de los JSON de la base_config.
    trophic_species_catalog <- reactive({
      config_root <- base_config_path()
      req(!is.null(config_root), nzchar(config_root))

      read_species_files <- function(directory, kind) {
        json_files <- list.files(directory, pattern = "\\.json$", full.names = TRUE, recursive = FALSE)
        lapply(json_files, function(json_file) {
          data <- tryCatch(jsonlite::fromJSON(json_file, simplifyVector = FALSE), error = function(e) NULL)
          if (is.null(data)) return(NULL)

          if (identical(kind, "animal")) {
            name <- data$animal$name
            instar_count <- length(data$animal$individualsPerInstar)
          } else {
            name <- data$resource$name
            instar_count <- 1L
          }

          if (is.null(name) || length(name) != 1L || is.na(name) || !nzchar(trimws(name))) return(NULL)
          list(name = as.character(name), instar_count = max(1L, as.integer(instar_count)))
        })
      }

      species <- Filter(Negate(is.null), read_species_files(file.path(config_root, "species"), "animal"))
      resources <- Filter(Negate(is.null), read_species_files(file.path(config_root, "resource"), "resource"))

      # Mantener el primer archivo si hay nombres repetidos y conservar el
      # recuento de instares de cada animal para sus selectores dependientes.
      species <- species[!duplicated(vapply(species, `[[`, character(1), "name"))]
      resources <- resources[!duplicated(vapply(resources, `[[`, character(1), "name"))]
      list(
        predators = species,
        prey = c(species, resources)
      )
    })
    
    # 1. Compila el catálogo completo de parámetros leyendo los esquemas JSON del paquete weaverR
    current_schema_catalog <- reactive({
      req(detected_version())
      ver <- detected_version()
      
      # Carga las hojas terminales desde la matriz de recursos embebidos
      df <- load_all_version_schemas(ver, include_basic_strings = FALSE)
      
      # Filtra y valida las filas contra la existencia real de directorios en el volumen
      filter_catalog_by_available_config_json(df, base_config_path())
    })
    
    # 2. Resuelve de forma reactiva la lista exacta de archivos físicos y sus rutas internas válidas
    available_coordinates_map <- function(parameter_class) {
      config_root <- base_config_path()
      req(!is.null(config_root), nzchar(config_root))
      catalog <- filter_schema_catalog_by_parameter_class(current_schema_catalog(), parameter_class)
      if (nrow(catalog) == 0L) return(list())
      
      # Creamos una lista donde cada archivo tiene asignado un vector con sus JSON paths válidos ($)
      files_to_paths_list <- list()
      
      for (schema_file_name in unique(catalog$schema_file)) {
        schema_rows <- catalog[catalog$schema_file == schema_file_name, , drop = FALSE]
        # Resolve real configuration assets that use this schema.
        f_destinations <- resolve_config_destinations_by_schema(schema_file_name, config_root)
        
        for (f_path in f_destinations) {
          concrete_paths <- expand_schema_paths_for_config(
            schema_rows$json_path,
            file.path(config_root, f_path)
          )
          if (length(concrete_paths) == 0L) next
          if (is.null(files_to_paths_list[[f_path]])) files_to_paths_list[[f_path]] <- character(0)
          files_to_paths_list[[f_path]] <- unique(c(files_to_paths_list[[f_path]], concrete_paths))
        }

      }
      files_to_paths_list
    }

    permutation_choices_for_variable <- function(var_id) {
      id_str <- as.character(var_id)
      active_rows <- as.integer(pair_rows_state[[id_str]] %||% 1L)
      complete_destinations <- lapply(active_rows, function(pair_row_id) {
        pair_key <- paste0(var_id, "_", pair_row_id)
        target_file <- normalize_config_relative_path(input[[paste0("pair_file_", pair_key)]] %||% "")
        target_path <- trimws(as.character(input[[paste0("pair_path_", pair_key)]] %||% ""))
        if (!nzchar(target_file) || !nzchar(target_path)) return(NULL)
        list(file = target_file, path = target_path)
      })
      complete_destinations <- Filter(Negate(is.null), complete_destinations)
      if (length(complete_destinations) == 0L) return(character(0))

      config_root <- base_config_path()
      schema_catalog <- current_schema_catalog()
      choices <- character(0)

      for (destination in complete_destinations) {
        schema_files <- unique(schema_catalog$schema_file)
        matching_schema_files <- schema_files[vapply(
          schema_files,
          function(schema_file) destination$file %in% normalize_config_relative_path(
            resolve_config_destinations_by_schema(schema_file, config_root)
          ),
          logical(1)
        )]

        for (schema_file in matching_schema_files) {
          schema_rows <- schema_catalog[
            schema_catalog$schema_file == schema_file &
              !is.na(schema_catalog$enum_values) & nzchar(schema_catalog$enum_values),
            , drop = FALSE
          ]
          config_file <- file.path(config_root, destination$file)

          for (row_idx in seq_len(nrow(schema_rows))) {
            concrete_paths <- expand_schema_paths_for_config(
              schema_rows$json_path[[row_idx]],
              config_file
            )
            if (destination$path %in% concrete_paths) {
              choices <- c(
                choices,
                strsplit(schema_rows$enum_values[[row_idx]], "|", fixed = TRUE)[[1]]
              )
            }
          }
        }
      }

      unique(choices[nzchar(choices)])
    }

    # Resolve cached template metadata by the current card position. Legacy
    # aliases may differ from the stored key or be duplicated.
    cached_variable_for_id <- function(var_id) {
      cached_params <- global_store$phase_2$defined_parameters
      if (is.null(cached_params) || length(cached_params) == 0L) return(NULL)

      card_index <- match(as.integer(var_id), as.integer(inserted_variables$ids))
      if (!is.na(card_index) && card_index <= length(cached_params)) {
        return(cached_params[[card_index]])
      }
      NULL
    }


    # --- SUBSURFACE HELPER: SINGLE DESTINATION COORDINATE PAIR ROW CONSTRUCTOR ---
    render_destination_pair_row_ui <- function(var_id, pair_row_id, display_idx) {
      ns_pair  <- session$ns
      comp_key <- paste0(var_id, "_", pair_row_id)
      
      # Recuperamos el mapa de archivos reales válidos
      saved_meta <- cached_variable_for_id(var_id)
      parameter_class <- input[[paste0("var_type_", var_id)]] %||% saved_meta$parameter_class %||% ""
      coords_map <- available_coordinates_map(parameter_class)
      valid_files <- names(coords_map)
      
      # Capturamos de forma segura las selecciones previas si la tarjeta se está redibujando
      saved_destinations <- saved_meta$tied_destinations_coordinates %||% list()
      saved_destination <- if (length(saved_destinations) >= pair_row_id) saved_destinations[[pair_row_id]] else list()
      saved_file <- normalize_config_relative_path(isolate(input[[paste0("pair_file_", comp_key)]] %||% saved_destination$target_json_file %||% ""))
      saved_path <- isolate(input[[paste0("pair_path_", comp_key)]] %||% saved_destination$internal_json_path %||% "")
      if (!saved_file %in% valid_files) saved_file <- ""
      
      # Si ya hay un archivo seleccionado, filtramos las rutas internas de su esquema; si no, vacío
      path_choices <- if (saved_file != "" && saved_file %in% valid_files) coords_map[[saved_file]] else character(0)
      if (!saved_path %in% path_choices) saved_path <- ""
      initial_path_choices <- c("(Black)" = "", if (nzchar(saved_path)) saved_path)
      
      div(
        id = ns_pair(paste0("pair_row_container_", comp_key)),
        # Se añade d-flex para controlar los bloques principales en horizontal
        class = "d-flex align-items-center gap-2 mb-2 px-2 py-1 rounded border border-secondary-subtle bg-body-tertiary shadow-xs",
  
        # Columna 1: Indicador numérico de fila
        div(class = "flex-shrink-0 d-flex align-items-center",
            tags$span(class = "badge bg-secondary font-monospace", style = "font-size: 0.75rem;", display_idx)
        ),
        
        div(class = "flex-grow-1 d-flex flex-column gap-1", style = "min-width: 0;",
            # Selector 1: Archivo Real
            selectizeInput(
              inputId  = ns_pair(paste0("pair_file_", comp_key)),
              label    = NULL,
              choices  = c("Choose JSON target asset..." = "", valid_files),
              selected = saved_file,
              options  = list(placeholder = "Select configuration JSON file..."),
              width    = "100%" # Forzar a que ocupe todo el ancho del contenedor flex
            ),
            # Selector 2: Ruta Interna
            selectizeInput(
              inputId  = ns_pair(paste0("pair_path_", comp_key)),
              label    = NULL,
              choices  = initial_path_choices,
              selected = saved_path,
              options  = list(placeholder = "Select schema path...", create = FALSE),
              width    = "100%" # Forzar a que ocupe todo el ancho del contenedor flex
            )
        ),
        
        # Columna 4: Botón de eliminación (Extremo derecho y centrado verticalmente)
        div(class = "flex-shrink-0 d-flex align-items-center text-end",
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
    
    # ============================================================================
    # ATOMIC VARIABLE DESTRUCTION OBSERVER LOOP
    # ============================================================================
    observe({
      active_ids <- inserted_variables$ids
      req(length(active_ids) > 0)
      
      lapply(active_ids, function(var_id) {
        observeEvent(input[[paste0("remove_var_", var_id)]], {
          isolate({
            card_index <- match(as.integer(var_id), as.integer(inserted_variables$ids))
            cached_params <- global_store$phase_2$defined_parameters
            if (!is.na(card_index) && card_index <= length(cached_params)) {
              cached_params[[card_index]] <- NULL
              global_store$phase_2$defined_parameters <- cached_params
            }
            inserted_variables$ids <- setdiff(inserted_variables$ids, var_id)
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
          saved_meta <- cached_params[[idx]]
          cached_names <- names(cached_params)
          if (!is.null(cached_names) && length(cached_names) >= idx && !is.na(cached_names[[idx]]) && nzchar(cached_names[[idx]])) {
            saved_alias <- cached_names[[idx]]
          }
          saved_alias <- saved_meta$variable_alias %||% saved_alias
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
          class = "mb-3 border secondary-subtle shadow-sm card",
          style = "padding: 15px;",
          
          div(class = "d-flex justify-content-between align-items-center mb-3 pb-2 border-bottom",
              tags$span(class = "fw-bold",
                  icon("gear", class = "me-1"), "Optimization Variable #", tags$code(as.character(idx))
              ),
              actionButton(var_ns_func(paste0("remove_var_", var_id)), label = NULL, icon = icon("trash"), class = "btn-outline-danger btn-sm")
          ),
          
          div(class = "row g-3 mb-2",
              div(class = "col-md-6",
                div(class = "optimiser-input-card",
                  textInput(var_ns_func(paste0("var_name_", var_id)), label = tags$b("Custom Variable Alias Identifier:"), value = curr_alias),
                  tags$small(class = "text-muted d-block", "A unique identifier name used to map and track this specific variable across the optimization runtime.")
                )
              ),
              div(class = "col-md-6",
                div(class = "optimiser-input-card",
                  selectInput(
                    inputId  = var_ns_func(paste0("var_type_", var_id)), 
                    label    = tags$b("Variable Type:"), 
                    choices  = c("RealValued", "Integer", "Boolean", "Permutation", "Abundances", "Preference", "Profitability"),
                    selected = if(curr_class == "") "RealValued" else curr_class
                  ),
                  tags$small(class = "text-muted d-block mb-2", "Select the specific type of variable to optimize, determining its mathematical constraints and value range.")
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

          # Este renderUI se evalúa fuera del renderizador de tarjetas, por lo
          # que recupera aquí su propia metadata persistida cuando exista.
          cached_params <- global_store$phase_2$defined_parameters
          current_alias <- trimws(input[[paste0("var_name_", var_id)]] %||% "")
          cache_index <- match(var_id, inserted_variables$ids)
          saved_meta <- if (!is.null(cached_params) && current_alias %in% names(cached_params)) {
            cached_params[[current_alias]]
          } else if (!is.null(cached_params) && !is.na(cache_index) && cache_index <= length(cached_params)) {
            cached_params[[cache_index]]
          } else list()
          
          # ------------------------------------------------------------------
          # 1. ESQUELETO BASE PARA EL CAMPO REIMPLEMENTADO DE DESTINOS PARES
          # ------------------------------------------------------------------
          destinations_pair_grid_layout <- tags$div(
            class = "p-3 mb-3 border rounded shadow-sm tied-destinations-container",
            tags$b(icon("network-wired"), " Target File & Internal JSON Paths Assignment:"),
            tags$small(class = "text-muted d-block mb-3", "Map this optimization variable to its corresponding parameter locations by specifying the target configuration files and their internal JSON property keys."),
            
            uiOutput(session$ns(paste0("var_destinations_pairs_anchor_", var_id)))
          )
          if (selected_class %in% c("Preference", "Profitability")) {
            destinations_pair_grid_layout <- NULL
          }
          
          # ------------------------------------------------------------------
          # 2. DEFINICIÓN DE CAMPOS SEGÚN CLASE (RECUPERADOS DE PHASE 3 ANTIGUA)
          # ------------------------------------------------------------------
          specific_fields_layout <- NULL
          
          # CASO A: INTEGER & REALVALUED
          if (selected_class %in% c("Integer", "RealValued")) {
            input_class <- if (selected_class == "Integer") "integer-input" else "decimal-input"
            step_attr   <- if (selected_class == "Integer") 1L else "any"
            min_attr    <- if (selected_class == "Integer") 0L else 0.0
            
            specific_fields_layout <- tags$div(
              class = "numeric-range-container mt-2", `data-var-id` = var_id,
              bslib::layout_columns(
                col_widths = c(6, 6),
                div(class = paste("optimiser-input-card", input_class), 
                  numericInput(session$ns(paste0("min_v_", var_id)), tags$b("Minimum Bound:"), value = min_attr, min = min_attr, step = step_attr),
                  tags$small(class = "text-muted d-block", "The lower boundary constraint. The optimizer will not test values below this threshold.")
                ),
                div(class = paste("optimiser-input-card", input_class), 
                  numericInput(session$ns(paste0("max_v_", var_id)), tags$b("Maximum Bound:"), value = 1.0, min = min_attr, step = step_attr),
                  tags$small(class = "text-muted d-block", "The upper boundary constraint. The optimizer will not test values above this threshold.")
                )
              ),
              uiOutput(session$ns(paste0("error_local_range_", var_id)))
            )
            
          # CASO B: BOOLEAN TOGGLE
          } else if (selected_class == "Boolean") {
            switch_input_id <- paste0("value_bool_v_", var_id)
            current_switch_value <- if (!is.null(input[[switch_input_id]])) input[[switch_input_id]] else FALSE
            
            specific_fields_layout <- bslib::layout_columns(
              col_widths = c(6, 6),
              div(class = "optimiser-input-card",
                  tags$b("Boolean Optimization Constraint:"),
                  div(class = "d-flex align-items-center gap-3 mt-1",
                    div(style = "flex: 1 1 0%; min-width: 0;",
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
                    )
                  ),
                  tags$small(class = "text-muted d-block", "Set the exact boolean value for this variable in the optimization process.")
              )
            )
            
          # CASO C: CATEGORICAL MATRICES (PERMUTATION)
          } else if (selected_class == "Permutation") {
            specific_fields_layout <- tags$div(
              class = "permutation-buttons-container optimiser-input-card mt-2", `data-var-id` = var_id,
              tags$b("Target Categorical Selection:"),
              tags$small(class = "text-muted d-block mb-3", "Select the specific categories to include in the permutation matrix for optimization."),
              uiOutput(session$ns(paste0("permutation_choices_", var_id))),
              uiOutput(session$ns(paste0("error_local_permutation_", var_id)))
            )
            
          # CASO D: POPULATION DENSITIES VECTOR (ABUNDANCES)
          } else if (selected_class == "Abundances") {
            instar_count <- 5L 
            normalize_btn_id <- paste0("normalize_btn_v_", var_id)
            selected_min <- input[[paste0("min_v_", var_id)]] %||% saved_meta$min_bound %||% 200L
            selected_max <- input[[paste0("max_v_", var_id)]] %||% saved_meta$max_bound %||% 1200L
            
            local({
              v_id <- var_id; i_count <- instar_count; n_btn_id <- normalize_btn_id
              observeEvent(input[[n_btn_id]], {
                vals <- numeric(i_count)
                for (k in seq_len(i_count)) {
                  vals[k] <- input[[paste0("density_elem_v_", v_id, "_", k)]] %||% 0.0
                }
                total <- sum(vals)
                norm_vals <- if (total == 0) rep(1 / i_count, i_count) else vals / total
                diff_residual <- 1.0 - sum(norm_vals)
                norm_vals[i_count] <- norm_vals[i_count] + diff_residual
                
                for (k in seq_len(i_count)) {
                  updateNumericInput(session, paste0("density_elem_v_", v_id, "_", k), value = norm_vals[k])
                }
              }, ignoreInit = TRUE)
            })
            
            vector_inputs <- lapply(seq_len(instar_count), function(v_idx) {
              sub_input_id <- paste0("density_elem_v_", var_id, "_", v_idx)
              saved_density <- saved_meta$densities_vector
              saved_val <- if (!is.null(saved_density) && length(saved_density) >= v_idx) saved_density[[v_idx]] else NULL
              current_val <- input[[sub_input_id]] %||% saved_val %||% 0.0
              div(class = "col",
                  div(class = "decimal-input", 
                      numericInput(session$ns(sub_input_id), label = tags$b(paste("Instar Stage", v_idx)), value = current_val, min = 0, step = "any")
                  )
              )
            })
            
            specific_fields_layout <- tags$div(
              class = "mt-3 p-3 border rounded abundances-vector-container optimiser-input-card shadow-sm",
              div(class = "d-flex justify-content-between align-items-center mb-1 pb-2 border-secondary-subtle",
                  tags$b("Instar Densities Weight Vector Allocation:"),
                  actionButton(session$ns(normalize_btn_id), "Normalize Matrix to 1.0", class = "btn btn-outline-success btn-sm fw-bold", icon = icon("wand-magic-sparkles"))
              ),
              p(class = "text-muted small mb-3", "The continuous biological distribution weights must aggregate vertically and equal exactly to 1.0:"),
              div(class = "row row-cols-1 row-cols-sm-3 row-cols-md-5 g-3 mb-1", do.call(tagList, vector_inputs)),
              bslib::layout_columns(
                col_widths = c(6, 6),
                numericInput(session$ns(paste0("min_v_", var_id)), tags$b("Minimum Abundance Bound:"), value = selected_min, min = 0, step = 1),
                numericInput(session$ns(paste0("max_v_", var_id)), tags$b("Maximum Abundance Bound:"), value = selected_max, min = 0, step = 1)
              ),
              uiOutput(session$ns(paste0("error_local_abundance_", var_id)))
            )
            
          # CASO E: TROPHIC INTERACTIONS (PREFERENCE & PROFITABILITY)
          } else if (selected_class %in% c("Preference", "Profitability")) {
            saved_cells <- saved_meta$cells %||% list()
            if (is.null(trophic_group_ids[[id_str]])) {
              restored_count <- max(1L, length(saved_cells))
              trophic_group_ids[[id_str]] <- seq_len(restored_count)
              next_trophic_group_id[[id_str]] <- restored_count + 1L
            }
            selected_min <- input[[paste0("min_v_", var_id)]] %||% saved_meta$min_bound %||% 0.0
            selected_max <- input[[paste0("max_v_", var_id)]] %||% saved_meta$max_bound %||% 1.0
            
            specific_fields_layout <- tags$div(
              class = "mt-3 p-3 border rounded preference-cell-container shadow-sm",
              tags$label(class = "control-label fw-bold mb-2 text-primary", " Interactive Cell Matrix Mapping:"),
              tags$p(class = "text-muted small mb-3", 
                "Define one or more predator/prey cell groups. Predator instars map to CSV columns and prey instars map to rows:",
              ),
              uiOutput(session$ns(paste0("trophic_groups_ui_", var_id))),
              div(class = "mt-2",
                actionButton(session$ns(paste0("add_trophic_group_", var_id)), "Add Cell Group", icon = icon("plus"), class = "btn btn-outline-primary btn-sm")
              ),
              div(class = "decimal-input mt-3",
                bslib::layout_columns(
                  col_widths = c(6, 6),
                  numericInput(session$ns(paste0("min_v_", var_id)), tags$b("Minimum Cell Bound:"), value = selected_min, min = 0.0, max = 1.0, step = "any"),
                  numericInput(session$ns(paste0("max_v_", var_id)), tags$b("Maximum Cell Bound:"), value = selected_max, min = 0.0, max = 1.0, step = "any")
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
            specific_fields_layout
          )
        })

        output[[paste0("permutation_choices_", var_id)]] <- renderUI({
          req(identical(input[[paste0("var_type_", var_id)]], "Permutation"))
          choices_from_schema <- permutation_choices_for_variable(var_id)

          if (length(choices_from_schema) == 0L) {
            active_rows <- as.integer(pair_rows_state[[id_str]] %||% 1L)
            has_complete_destination <- any(vapply(active_rows, function(pair_row_id) {
              pair_key <- paste0(var_id, "_", pair_row_id)
              target_file <- input[[paste0("pair_file_", pair_key)]] %||% ""
              target_path <- input[[paste0("pair_path_", pair_key)]] %||% ""
              nzchar(trimws(as.character(target_file))) && nzchar(trimws(as.character(target_path)))
            }, logical(1)))

            message <- if (has_complete_destination) {
              "No enum options were found in the schema for the selected destination."
            } else {
              "The schema options cannot be extracted until the Target File and the Internal JSON Path for the variable have been set."
            }
            return(p(class = "text-muted small mb-0", message))
          }

          elements_input_id <- paste0("elements_v_", var_id)
          elements_current <- input[[elements_input_id]]
          if (is.null(elements_current)) elements_current <- choices_from_schema

          shinyWidgets::checkboxGroupButtons(
            inputId    = session$ns(elements_input_id),
            label      = NULL,
            choices    = choices_from_schema,
            selected   = intersect(elements_current, choices_from_schema),
            justified  = TRUE,
            individual = FALSE,
            status     = "primary"
          )
        })

        output[[paste0("trophic_groups_ui_", var_id)]] <- renderUI({
          req(input[[paste0("var_type_", var_id)]] %in% c("Preference", "Profitability"))
          saved_meta <- cached_variable_for_id(var_id) %||% list()
          saved_cells <- saved_meta$cells %||% list()
          if (is.null(trophic_group_ids[[id_str]])) {
            restored_count <- max(1L, length(saved_cells))
            trophic_group_ids[[id_str]] <- seq_len(restored_count)
            next_trophic_group_id[[id_str]] <- restored_count + 1L
          }
          species_catalog <- trophic_species_catalog()
          animal_names <- vapply(species_catalog$predators, `[[`, character(1), "name")
          all_prey_names <- vapply(species_catalog$prey, `[[`, character(1), "name")
          group_ids <- as.integer(trophic_group_ids[[id_str]] %||% 1L)

          group_uis <- lapply(seq_along(group_ids), function(group_index) {
            group_id <- group_ids[[group_index]]
            saved_cell <- if (length(saved_cells) >= group_index) saved_cells[[group_index]] else list()
            pred_sp_id <- paste0("pref_predator_sp_v_", var_id, "_", group_id)
            pred_ins_id <- paste0("pref_predator_instars_v_", var_id, "_", group_id)
            prey_sp_id <- paste0("pref_prey_sp_v_", var_id, "_", group_id)
            prey_ins_id <- paste0("pref_prey_instars_v_", var_id, "_", group_id)

            default_predator <- if (length(animal_names)) animal_names[[1]] else ""
            default_prey <- if (length(all_prey_names)) all_prey_names[[1]] else ""
            selected_pred_sp <- input[[pred_sp_id]] %||% saved_cell$predator_species %||% default_predator
            selected_prey_sp <- input[[prey_sp_id]] %||% saved_cell$prey_species %||% default_prey
            if (length(animal_names) && !selected_pred_sp %in% animal_names) selected_pred_sp <- default_predator
            if (length(all_prey_names) && !selected_prey_sp %in% all_prey_names) selected_prey_sp <- default_prey

            predator_index <- match(selected_pred_sp, animal_names)
            prey_index <- match(selected_prey_sp, all_prey_names)
            predator_entry <- if (!is.na(predator_index)) species_catalog$predators[[predator_index]] else list(instar_count = 0L)
            prey_entry <- if (!is.na(prey_index)) species_catalog$prey[[prey_index]] else list(instar_count = 0L)
            predator_instar_choices <- if (predator_entry$instar_count > 0L) as.character(seq_len(predator_entry$instar_count)) else character(0)
            prey_instar_choices <- if (prey_entry$instar_count > 0L) as.character(seq_len(prey_entry$instar_count)) else character(0)
            selected_pred_ins <- input[[pred_ins_id]] %||% as.character(saved_cell$predator_instars %||% predator_instar_choices)
            selected_prey_ins <- input[[prey_ins_id]] %||% as.character(saved_cell$prey_instars %||% prey_instar_choices)
            selected_pred_ins <- intersect(as.character(selected_pred_ins), predator_instar_choices)
            selected_prey_ins <- intersect(as.character(selected_prey_ins), prey_instar_choices)

            div(class = "mb-3 p-3 border rounded bg-body-tertiary shadow-sm",
              div(class = "d-flex justify-content-between align-items-center mb-3 pb-2 border-bottom",
                tags$h6(class = "fw-bold text-primary mb-0", icon("table-cells"), " Cell Group ", group_index),
                actionButton(
                  session$ns(paste0("remove_trophic_group_", var_id, "_", group_id)),
                  label = NULL, icon = icon("trash"), class = "btn-outline-danger btn-sm",
                  disabled = length(group_ids) == 1L
                )
              ),
              bslib::layout_columns(
                col_widths = c(6, 6),
                div(class = "p-3 border rounded bg-white shadow-sm", style = "border-left: 4px solid var(--bs-primary) !important;",
                  tags$h6(class = "fw-bold text-dark mb-3", icon("skull"), " Predator Columns"),
                  selectInput(session$ns(pred_sp_id), "Predator Species:", choices = animal_names, selected = selected_pred_sp),
                  selectizeInput(session$ns(pred_ins_id), "Predator Instars (CSV columns):", choices = predator_instar_choices, selected = selected_pred_ins, multiple = TRUE, options = list(plugins = list("remove_button")))
                ),
                div(class = "p-3 border rounded bg-white shadow-sm", style = "border-left: 4px solid #18bc9c !important;",
                  tags$h6(class = "fw-bold text-dark mb-3", icon("shrimp"), " Prey Rows"),
                  selectInput(session$ns(prey_sp_id), "Prey Species:", choices = all_prey_names, selected = selected_prey_sp),
                  selectizeInput(session$ns(prey_ins_id), "Prey Instars (CSV rows):", choices = prey_instar_choices, selected = selected_prey_ins, multiple = TRUE, options = list(plugins = list("remove_button")))
                )
              )
            )
          })

          do.call(tagList, group_uis)
        })

        if (!id_str %in% registered_trophic_handlers()) {
          local({
            variable_id <- var_id
            variable_key <- id_str
            observeEvent(input[[paste0("add_trophic_group_", variable_id)]], {
              current_groups <- as.integer(trophic_group_ids[[variable_key]] %||% 1L)
              next_group <- as.integer(next_trophic_group_id[[variable_key]] %||% (max(current_groups) + 1L))
              trophic_group_ids[[variable_key]] <- c(current_groups, next_group)
              next_trophic_group_id[[variable_key]] <- next_group + 1L
            }, ignoreInit = TRUE)
          })
          registered_trophic_handlers(c(registered_trophic_handlers(), id_str))
        }

        group_ids_for_handlers <- as.integer(trophic_group_ids[[id_str]] %||% 1L)
        for (group_id in group_ids_for_handlers) {
          handler_key <- paste(id_str, group_id, sep = "_")
          if (handler_key %in% registered_trophic_handlers()) next
          local({
            variable_key <- id_str
            variable_id <- var_id
            current_group_id <- group_id
            remove_input_id <- paste0("remove_trophic_group_", variable_id, "_", current_group_id)
            observeEvent(input[[remove_input_id]], {
              current_groups <- as.integer(trophic_group_ids[[variable_key]] %||% 1L)
              if (length(current_groups) > 1L) trophic_group_ids[[variable_key]] <- setdiff(current_groups, current_group_id)
            }, ignoreInit = TRUE, once = TRUE)
          })
          registered_trophic_handlers(c(registered_trophic_handlers(), handler_key))
        }


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
            
            div(class = "mt-2 pt-2 border-secondary-subtle text-start",
                actionButton(
                  inputId = session$ns(paste0("add_pair_to_var_", var_id)),
                  label   = "Add Destination",
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
            var_id <- v_id
            pair_id <- p_row_id
            ck <- paste0(var_id, "_", pair_id)
            handler_key <- ck
            file_input_id <- paste0("pair_file_", ck)
            registered_handlers <- isolate(registered_destination_handlers())
            
            if (!handler_key %in% registered_handlers) {
              sync_state <- new.env(parent = emptyenv())
              sync_state$initialized <- FALSE
              sync_state$last_file <- NULL

              observeEvent(input[[file_input_id]], {
                selected_file <- input[[file_input_id]]
                req(!is.null(selected_file))
                selected_file <- normalize_config_relative_path(selected_file)
              
                cached_variable <- cached_variable_for_id(var_id)
                parameter_class <- input[[paste0("var_type_", var_id)]] %||% cached_variable$parameter_class %||% ""
                coords_map    <- available_coordinates_map(parameter_class)
              
                if (!is.null(selected_file) && selected_file != "" && selected_file %in% names(coords_map)) {
                  raw_paths <- as.character(coords_map[[selected_file]])
                  cached_destinations <- cached_variable$tied_destinations_coordinates %||% list()
                  cached_destination <- if (length(cached_destinations) >= pair_id) cached_destinations[[pair_id]] else list()
                  current_path <- input[[paste0("pair_path_", ck)]] %||% ""
                  cached_file <- normalize_config_relative_path(cached_destination$target_json_file %||% "")
                  cached_path <- as.character(cached_destination$internal_json_path %||% "")
                  cached_path_is_valid <- identical(as.character(selected_file), cached_file) &&
                    length(cached_path) == 1L && cached_path %in% raw_paths

                  update_args <- list(
                    session = session,
                    inputId = paste0("pair_path_", ck),
                    choices = c("(Black)" = "", raw_paths),
                    server = TRUE
                  )

                  if (!sync_state$initialized) {
                    if (length(current_path) == 1L && current_path %in% raw_paths) {
                      update_args$selected <- current_path
                    } else if (cached_path_is_valid) {
                      update_args$selected <- cached_path
                    }
                  } else {
                    same_file <- identical(as.character(selected_file), sync_state$last_file)
                    selected_path <- if (same_file && length(current_path) == 1L && current_path %in% raw_paths) {
                      current_path
                    } else if (cached_path_is_valid) {
                      cached_path
                    } else {
                      ""
                    }
                    update_args$selected <- selected_path
                  }

                  do.call(updateSelectizeInput, update_args)
                } else {
                  updateSelectizeInput(
                    session = session, 
                    inputId = paste0("pair_path_", ck), 
                    choices = c("(Black)" = ""),
                    selected = "",
                    server  = TRUE
                  )
                }

                sync_state$initialized <- TRUE
                sync_state$last_file <- as.character(selected_file)
              }, ignoreInit = FALSE)

              registered_destination_handlers(c(registered_destination_handlers(), handler_key))
            }
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
      compiled_variables_config <- list()
      
      for (v_id in var_ids) {
        id_str <- as.character(v_id)
        v_name <- trimws(input[[paste0("var_name_", v_id)]]) %||% paste0("var_opt_", v_id)
        v_type <- input[[paste0("var_type_", v_id)]]
        
        # Recuperamos las filas de pares asociadas a esta tarjeta
        active_rows <- as.integer(pair_rows_state[[id_str]] %||% 1L)
        if (v_type %in% c("Preference", "Profitability")) active_rows <- integer(0)
        
        compiled_coordinates_pairs <- lapply(active_rows, function(p_row_id) {
          comp_key <- paste0(v_id, "_", p_row_id)
          list(
            target_json_file  = input[[paste0("pair_file_", comp_key)]],
            internal_json_path = trimws(input[[paste0("pair_path_", comp_key)]])
          )
        })
        
        # Unificamos el bloque estructural conservando los campos del tipo
        variable_config <- list(
          parameter_class = v_type,
          tied_destinations_coordinates = compiled_coordinates_pairs,
          
          # Rangos y restricciones numéricas según aplique
          min_bound       = input[[paste0("min_v_", v_id)]],
          max_bound       = input[[paste0("max_v_", v_id)]],
          boolean_toggle  = input[[paste0("value_bool_v_", v_id)]],
          selected_enums  = input[[paste0("elements_v_", v_id)]]
        )

        # Preference y Profitability representan una o más regiones de la matriz
        # trófica. Guardamos la definición de cada celda para que el JSON pueda
        # reconstruir los PreferenceCell del motor (especies e instares).
        if (identical(v_type, "Preference") || identical(v_type, "Profitability")) {
          group_ids <- as.integer(trophic_group_ids[[id_str]] %||% 1L)
          variable_config$cells <- lapply(group_ids, function(group_id) {
            predator_instars <- suppressWarnings(as.numeric(input[[paste0("pref_predator_instars_v_", v_id, "_", group_id)]]))
            prey_instars <- suppressWarnings(as.numeric(input[[paste0("pref_prey_instars_v_", v_id, "_", group_id)]]))
            list(
              predator_species = as.character(input[[paste0("pref_predator_sp_v_", v_id, "_", group_id)]]),
              predator_instars = predator_instars,
              prey_species = as.character(input[[paste0("pref_prey_sp_v_", v_id, "_", group_id)]]),
              prey_instars = prey_instars
            )
          })
          names(variable_config$cells) <- NULL
        }

        if (identical(v_type, "Abundances")) {
          variable_config$densities_vector <- as.numeric(vapply(seq_len(5L), function(v_idx) {
            input[[paste0("density_elem_v_", v_id, "_", v_idx)]] %||% NA_real_
          }, numeric(1)))
        }

        compiled_variables_config[[v_name]] <- variable_config
      }
      
      global_store$phase_2$defined_parameters <- compiled_variables_config
      message("[Phase 2 Store] Advanced Parameter Tying data matrices committed successfully.")
    }
    
    return(list(
      is_valid    = phase_2_is_valid,
      commit_data = commit_phase_2_data_transaction
    ))
  })
}
