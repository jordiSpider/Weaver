#' Phase 1: Define General Optimisation Parameters Server (Performance Optimized)
#'
#' @description Centralizes error management through decoupled event listeners to enforce
#' computational efficiency and minimize redundant directory I/O operations.
#' @param id Unique module namespace identifier string.
#' @param global_store The global reactiveValues store mapping pre-existing cached parameters from phase_data_store.
#' @param max_cores_allowed The maximum number of CPU cores allowed for parallel processing.
#' @param shared_volumes List containing unified operating system mounts.
#' @import shiny
#' @export
mod_optimiser_phase_1_server <- function(id, global_store, max_cores_allowed, shared_volumes) {
  moduleServer(id, function(input, output, session) {
    message("[Phase 1 Server] Centralized event-driven validation architecture deployed successfully.")
    
    # ============================================================================
    # UNIFIED CENTRAL ERROR DATA REGISTRY (High-scalability store)
    # ============================================================================
    # Add new validation logs simply by pushing keys into this store array
    active_errors <- reactiveValues(
      population_size       = NULL,
      tournament_size       = NULL,
      program_text_path     = NULL,
      base_config_text_path = NULL,
      schema_detection      = NULL,
      zero_objectives       = NULL,
      duplicate_objectives  = list(),
      code_syntax_errors    = list(),
      weights_validation    = NULL,
      schema_version_mismatch = list() 
    )
    
    # ============================================================================
    # 1. ADAPTIVE TOURNAMENT SIZE FLUID CONSTRAINT CONTROLLER
    # ============================================================================
    observeEvent(input$keepBest, {
      req(!is.null(input$keepBest))
      new_minimum_bound <- if (input$keepBest) 3L else 2L
      if (!is.na(input$tournament_size) && input$tournament_size < new_minimum_bound) {
        updateNumericInput(session, "tournament_size", min = new_minimum_bound, value = new_minimum_bound)
      } else {
        updateNumericInput(session, "tournament_size", min = new_minimum_bound)
      }
    })

    # ============================================================================
    # 2. ADAPTIVE PROGRAM CORES FLUID CONSTRAINT CONTROLLER
    # ============================================================================
    observeEvent(input$total_cores, {
      req(!is.null(input$total_cores))
      updateNumericInput(session, "program_cores", max = input$total_cores)
      if (input$program_cores > input$total_cores) {
        updateNumericInput(session, "program_cores", value = input$total_cores)
      }
    })

    # ============================================================================
    # 2. ADAPTIVE PROGRAM CORES FLUID CONSTRAINT CONTROLLER
    # ============================================================================
    observeEvent(input$total_cores_auto, {
      req(!is.null(input$total_cores_auto))

      if(input$total_cores_auto) {
        updateNumericInput(session, "total_cores", value = max_cores_allowed)
      }
    })
    
    # ============================================================================
    # 3. ISOLATED EVENT LISTENERS VALIDATORS (Genetic Algorithm Parameters)
    # ============================================================================
    observeEvent({
      input$population_size
      input$tournament_size
      input$keepBest
    }, {
      pop  <- input$population_size
      tour <- input$tournament_size
      keep <- input$keepBest
      
      if (is.na(pop) || is.na(tour)) {
        active_errors$population_size <- NULL
        active_errors$tournament_size <- NULL
        return()
      }
      
      if (keep) {
        if (pop <= tour) {
          err_msg <- "Error: Population size must be greater than the tournament selection size."
          active_errors$population_size <- err_msg
          active_errors$tournament_size <- err_msg
        } else {
          active_errors$population_size <- NULL
          active_errors$tournament_size <- NULL
        }
      } else {
        if (pop < tour) {
          err_msg <- "Error: Population size must be greater or equal than the tournament selection size."
          active_errors$population_size <- err_msg
          active_errors$tournament_size <- err_msg
        } else {
          active_errors$population_size <- NULL
          active_errors$tournament_size <- NULL
        }
      }
    }, ignoreInit = FALSE)

    # ============================================================================
    # 3. ISOLATED EVENT LISTENERS VALIDATORS (System Environment Paths)
    # ============================================================================
    
    # Validator: Simulator Executable Path Binaries Mount Verification
    observeEvent(input$program_text_path, {
      path <- input$program_text_path
      if (is.null(path) || path == "") {
        active_errors$program_text_path <- "Error: Simulator executable path cannot be left blank."
      } else if (!grepl("\\.[eE][xX][eE]$", path)) {
        active_errors$program_text_path <- "Error: The specified path does not point to a valid executable (.exe) file."
      } else if (!file.exists(path)) {
        active_errors$program_text_path <- "Error: Simulator executable file not found at specified path."
      } else {
        active_errors$program_text_path <- NULL
      }
    }, ignoreInit = FALSE)
    
    # ============================================================================
    # 3. ISOLATED EVENT LISTENERS VALIDATORS (Base Configuration Layout Structure)
    # ============================================================================
    observeEvent(input$base_config_text_path, {
      path <- input$base_config_text_path
      
      if (is.null(path) || path == "") {
        active_errors$base_config_text_path <- "Error: Base configuration directory path cannot be left blank."
        active_errors$schema_version_mismatch <- list()
        return()
      }
      if (!dir.exists(path)) {
        active_errors$base_config_text_path <- "Error: The specified base configuration path does not point to an existing directory."
        active_errors$schema_version_mismatch <- list()
        return()
      }
      
      # Strict validation of folders structure and parameter matrices presence
      required_dirs <- c("habitat_domain", "moisture", "obstacle", "resource", "resource/patches", "species")
      required_files <- c("simulation_params.json", "landscape_params.json")
      required_csv   <- c("ontogeneticLinksPreference.csv", "ontogeneticLinksProfitability.csv")
      
      missing <- c()
      for (d in required_dirs) { if (!dir.exists(file.path(path, d))) missing <- c(missing, paste0("Folder missing: ", d)) }
      for (f in required_files) { if (!file.exists(file.path(path, f))) missing <- c(missing, paste0("File missing at root: ", f)) }
      if (!"Folder missing: species" %in% missing) {
        for (c in required_csv) { if (!file.exists(file.path(path, "species", c))) missing <- c(missing, paste0("File missing: species/", c)) }
      }
      
      if (length(missing) > 0) {
        active_errors$base_config_text_path <- missing
        active_errors$schema_version_mismatch <- list()
      } else {
        active_errors$base_config_text_path <- NULL
      }
    }, ignoreInit = FALSE)
    
    observe({
      path <- input$base_config_text_path
      exec_version <- schema_version()
      
      # CONDITIONAL SAFETY LIEN: Si falta la ruta, las carpetas están rotas o el exe no tiene versión, abortamos
      if (is.null(path) || path == "" || !dir.exists(path) || !is.null(active_errors$base_config_text_path)) {
        active_errors$schema_version_mismatch <- list()
        return()
      }
      if (is.null(exec_version) || exec_version == "") {
        active_errors$schema_version_mismatch <- list()
        return()
      }
      
      version_mismatches <- c()
      
      # Realizamos el escaneo recursivo de los ficheros JSON en segundo plano de forma segura
      all_json_files <- list.files(path, pattern = "\\.[jJ][sS][oO][nN]$", recursive = TRUE, full.names = TRUE)
      
      for (json_file in all_json_files) {
        tryCatch({
          json_data <- jsonlite::fromJSON(json_file, simplifyVector = TRUE)
          file_version_token <- json_data[["version"]]
          
          if (!is.null(file_version_token)) {
            if (as.character(file_version_token) != as.character(exec_version)) {
              relative_path <- sub(paste0(normalizePath(path, mustWork = FALSE), "/"), "", normalizePath(json_file, mustWork = FALSE), fixed = TRUE)
              version_mismatches <- c(version_mismatches, paste0("File: ", relative_path, " [Expected: ", exec_version, " | Found: ", file_version_token, "]"))
            }
          }
        }, error = function(e) {
          relative_path <- sub(paste0(normalizePath(path, mustWork = FALSE), "/"), "", normalizePath(json_file, mustWork = FALSE), fixed = TRUE)
          version_mismatches <- c(version_mismatches, paste0("File: ", relative_path, " [Parse failed: Malformed syntax]"))
        })
      }
      
      # Guardamos el resultado final en el slot correspondiente
      if (length(version_mismatches) > 0) {
        active_errors$schema_version_mismatch <- version_mismatches
      } else {
        active_errors$schema_version_mismatch <- list()
      }
    })

    # ============================================================================
    # 2.2 ISOLATED EVENT LISTENERS VALIDATORS (Ecosystem Objectives Constraints)
    # ============================================================================
    observe({
      active_card_ids <- inserted_objectives$ids
      
      if (length(active_card_ids) == 0) {
        active_errors$zero_objectives <- "Error: At least one fitness objective must be defined to run the simulation."
        active_errors$duplicate_objectives <- list()
        active_errors$code_syntax_errors <- list()
        active_errors$weights_validation <- NULL
        return()
      } else {
        active_errors$zero_objectives <- NULL
      }
      
      # Gather and cross-examine names inputs across the active dynamic UI array registers
      raw_names_list <- list()
      for (card_id in active_card_ids) {
        id_str <- as.character(card_id)
        current_name_val <- input[[paste0("obj_name_", card_id)]]
        
        # Safe buffer protection during intermediate initialization renders layers
        if (!is.null(current_name_val)) {
          raw_names_list[[id_str]] <- trimws(current_name_val)
        } else {
          raw_names_list[[id_str]] <- paste0("objective_", card_id)
        }
      }
      
      # Initialize fresh temporary local maps to isolate duplications
      active_errors$duplicate_objectives <- list()
      vector_of_names <- unlist(raw_names_list)
      
      # RULE 2: Deep scan each text string to isolate duplicate identifier overlaps
      for (card_id in active_card_ids) {
        id_str <- as.character(card_id)
        target_name <- raw_names_list[[id_str]]
        
        if (is.null(target_name) || target_name == "") {
          active_errors$duplicate_objectives[[id_str]] <- "Error: Objective name cannot be left blank."
        } else {
          # Count occurrences of the current name across the full vector string space
          occurrences_count <- sum(vector_of_names == target_name)
          if (occurrences_count > 1) {
            active_errors$duplicate_objectives[[id_str]] <- paste0("Error: The identifier name '", target_name, "' is duplicated. Each objective must have a unique identifier.")
          } else {
            active_errors$duplicate_objectives[[id_str]] <- NULL
          }
        }


        in_fit_func <- input[[paste0("obj_fitness_function_", card_id)]]

        if (!is.null(in_fit_func) && in_fit_func == "Custom Block") {
          raw_custom_code <- input[[paste0("obj_custom_code_", card_id)]]
      
          if (!is.null(raw_custom_code) && trimws(raw_custom_code) != "") {
        
            # --- STATIC CODE ANALYSIS VECTOR ---
            # Look for the explicit presence of a return statement using text regex patterns.
            # It captures both 'return(val)' and 'return' declarations effectively.
            has_return_statement <- grepl("return(\\s*\\(|\\s+|$)", raw_custom_code)
        
            if (!has_return_statement) {
              active_errors$code_syntax_errors[[id_str]] <- "Error: Missing statement. The custom fitness function must explicitly contain a 'return()' statement to pass computed metrics."
            } else {
              active_errors$code_syntax_errors[[id_str]] <- NULL
            }
        
          } else {
            active_errors$code_syntax_errors[[id_str]] <- "Error: Custom fitness function body code cannot be left blank."
          }
        } else {
          active_errors$code_syntax_errors[[id_str]] <- NULL
        }
      }


      req(!is.null(input$fitness_class))
      if (input$fitness_class == "WeightedFitness") {
        obj_names <- unlist(lapply(active_card_ids, function(id) {
          name_val <- input[[paste0("obj_name_", id)]]
          if (!is.null(name_val) && trimws(name_val) != "") trimws(name_val) else paste0("objective_", id)
        }))
        
        current_weights <- numeric(length(obj_names))
        is_any_weight_empty <- FALSE
        
        for (i in seq_along(obj_names)) {
          w_val <- input[[paste0("weight_for_", obj_names[i])]]
          if (is.null(w_val) || is.na(w_val)) {
            is_any_weight_empty <- TRUE
          } else {
            current_weights[i] <- w_val
          }
        }
        
        if (is_any_weight_empty) {
          active_errors$weights_validation <- "Error: All weighted strategy coefficients must be filled with a valid numeric value."
        } else {
          weights_sum <- sum(current_weights)
          if (abs(weights_sum - 1.0) > 1e-5) {
            active_errors$weights_validation <- paste0("Error: The sum of the weighted strategy coefficients is ", round(weights_sum, 4), ". It must equal exactly 1.0.")
          } else {
            active_errors$weights_validation <- NULL
          }
        }
      } else {
        active_errors$weights_validation <- NULL
      }
    })

    # Secure storage schema tracking card IDs and all their internal input fields structures
    inserted_objectives <- reactiveValues(
      ids             = c(),    # Integer vector tracking unique active sequential IDs (1, 2, 3...)
      names_map       = list(), # Character strings mapping indexed by string keys: "1", "2"...
      objectives_data = list(), # Nested metadata lists configuration snapshots indexed by string keys
      code_errors     = list()  # Syntax error tracking lists indexed by string keys
    )
    
    # --- SUBSURFACE HELPER: SINGLE OBJECTIVE CARD UI CONSTRUCTOR (STATE-AWARE) ---
    render_single_objective_card_ui <- function(card_id, saved_meta = NULL) {
      ns_func <- session$ns
      id_str  <- as.character(card_id)
  
      # Enforce robust default fallbacks if cache metadata does not exist yet
      final_name     <- if (!is.null(saved_meta$name)) saved_meta$name else paste0("objective_", card_id)
      final_criteria <- if (!is.null(saved_meta$criteria)) saved_meta$criteria else "MAX"
  
      # Restore the chosen key from structural cache memory or default to the first map entry
      final_fitness_func <- if (!is.null(saved_meta$fitness_func_name)) saved_meta$fitness_func_name else WeaverOptimiserClass$get_predefined_fitness_function_names()
  
      # Streamlined structural merge: predefined key entries from R list appended with the dynamic custom option
      fitness_choices <- c(WeaverOptimiserClass$get_predefined_fitness_function_names(), "Custom Block")
  
      tags$div(
        id = ns_func(paste0("objective_card_container_", card_id)),
        class = "mb-3 border secondary-subtle shadow-sm card optimiser-input-card",
        style = "padding: 12px;",
    
        # Header Row containing the dynamic title code and the destruction button
        div(class = "d-flex justify-content-between align-items-center mb-2 pb-2 border-bottom",
            tags$span(
              id = ns_func(paste0("objective_title_display_", card_id)), 
              class = "fw-bold text-primary", 
              tags$code(final_name)
            ),
            actionButton(ns_func(paste0("remove_obj_", card_id)), label = NULL, icon = icon("trash"), class = "btn-outline-danger btn-sm")
        ),
    
        # Row 1: Objective Identification Name and Target Optimization Criteria
        div(class = "row g-2 mb-2",
            div(class = "col-md-6",
                textInput(ns_func(paste0("obj_name_", card_id)), label = "Objective Name:", value = final_name),
                uiOutput(ns_func(paste0("error_local_obj_name_", card_id)))
            ),
            div(class = "col-md-6",
                selectInput(ns_func(paste0("obj_criteria_", card_id)), label = "Criteria:", choices = c("Maximize" = "MAX", "Minimize" = "MIN"), selected = final_criteria)
            )
        ),
    
        # Row 2: Unified Single-Input Selection Matrix for the Target Fitness Function
        selectInput(
          inputId  = ns_func(paste0("obj_fitness_function_", card_id)), 
          label    = "Fitness Function:", 
          choices  = fitness_choices, 
          selected = final_fitness_func
        ),
    
        # Dynamic Fragment Placement (Predefined plain text descriptions or Custom R Code IDE view)
        uiOutput(ns_func(paste0("obj_body_conditional_ui_", card_id))),
        uiOutput(ns_func(paste0("error_local_obj_code_", card_id))),
    
        # Dedicated local syntax error placeholder slot
        uiOutput(ns_func(paste0("obj_code_error_display_", card_id))),
    
        # Client-side instantaneous header synchronization script
        tags$script(HTML(paste0("
          $('#", ns_func(paste0("obj_name_", card_id)), "').on('input', function() {
            var inputRawText = $.trim($(this).val());
            var placeholderToken = inputRawText === '' ? '(Blank Name)' : inputRawText;
            $('#", ns_func(paste0("objective_title_display_", card_id)), "').html('<code>' + placeholderToken + '</code>');
          });
        ")))
      )
    }
    
    # ============================================================================
    # 2.3 SINGLE-INSTANCE EVENT LISTENER REGISTRATION (FLAG PROTECTED)
    # ============================================================================
    # Check if the button event handler has already been instantiated in this session context.
    # If it hasn't, register the observer once and flip the flag to prevent duplicates.
    if (!exists("is_add_btn_initialized", envir = session$userData) || 
        !isTRUE(session$userData$is_add_btn_initialized)) {
      
      observeEvent(input$add_objective_btn, {
        next_id <- if(length(inserted_objectives$ids) == 0) 1L else max(inserted_objectives$ids) + 1L
        id_str  <- as.character(next_id)
        
        inserted_objectives$ids <- c(inserted_objectives$ids, next_id)
        inserted_objectives$names_map[[id_str]] <- paste0("objective_", next_id)
        
        insertUI(
          selector = paste0("#", session$ns("objectives_anchor")),
          where    = "beforeEnd",
          ui       = render_single_objective_card_ui(next_id),
          immediate = TRUE
        )
      }, ignoreInit = TRUE)
      
      # Set the initialization flag inside the user session storage environment
      session$userData$is_add_btn_initialized <- TRUE
      message("[Memory Control] Objective insertion handler initialized strictly once for this session.")
    } else {
      message("[Memory Control] Objective insertion handler bypass triggered. Single-instance active.")
    }
    
    # --- EVENT LISTENER: ATOMIC CARD DESTRUCTION TRIGGER ---
    observe({
      lapply(inserted_objectives$ids, function(card_id) {
        observeEvent(input[[paste0("remove_obj_", card_id)]], {
          id_str <- as.character(card_id)
          
          # Purge card structural wrapper node matching the integer ID from the DOM tree
          removeUI(selector = paste0("#", session$ns(paste0("objective_card_container_", card_id))), immediate = TRUE)
          
          # Clean dynamic dictionary records storage mappings safely
          inserted_objectives$ids         <- setdiff(inserted_objectives$ids, card_id)
          inserted_objectives$names_map[[id_str]]       <- NULL
          inserted_objectives$objectives_data[[id_str]] <- NULL
          inserted_objectives$code_errors[[id_str]]     <- NULL
        }, once = TRUE)
      })
    })

    # ============================================================================
    # 4. BACKGROUND ASYNC SIMULATOR SCHEMA VERSION EXTRACTOR
    # ============================================================================
    schema_version <- reactive({
      req(is.null(active_errors$program_text_path))
      current_executable_path <- input$program_text_path
      if (is.null(current_executable_path) || current_executable_path == "") return(NULL)
      
      tryCatch({
        execution_response <- system2(command = current_executable_path, args = c("--schema_version"), stdout = TRUE, stderr = TRUE, timeout = 5)
        collapsed_str <- paste(execution_response, collapse = " ")
        if (grepl("Schema version:", collapsed_str)) {
          active_errors$schema_detection <- NULL
          return(sub(".*Schema version:\\s*", "", collapsed_str))
        }
        active_errors$schema_detection <- "Error: Unexpected simulator layout schema response format."
        return(NULL)
      }, error = function(e) {
        active_errors$schema_detection <- "Error executing --schema_version command line utility."
        return(NULL)
      })
    })

    # ============================================================================
    # 5. ZERO-COMPUTE PASSIVE RENDERING SLOTS (High-performance rendering)
    # ============================================================================
    
    # Area A: Render Unified General Phase Errors Container Alert Dashboard Panel Bar
    output$display_phase_general_errors <- renderUI({
      all_errors <- shiny::reactiveValuesToList(active_errors)
      consolidated_errors_vector <- c()
  
      if (!is.null(all_errors$population_size))   consolidated_errors_vector <- c(consolidated_errors_vector, all_errors$population_size)
      if (!is.null(all_errors$tournament_size))   consolidated_errors_vector <- c(consolidated_errors_vector, all_errors$tournament_size)
      if (!is.null(all_errors$program_text_path)) consolidated_errors_vector <- c(consolidated_errors_vector, all_errors$program_text_path)
      if (!is.null(all_errors$schema_detection))  consolidated_errors_vector <- c(consolidated_errors_vector, all_errors$schema_detection)
      if (!is.null(all_errors$zero_objectives))   consolidated_errors_vector <- c(consolidated_errors_vector, all_errors$zero_objectives)

      if (!is.null(all_errors$weights_validation)) consolidated_errors_vector <- c(consolidated_errors_vector, all_errors$weights_validation)
  
      if (!is.null(all_errors$base_config_text_path)) {
        err_content <- all_errors$base_config_text_path
        if (length(err_content) > 1) {
          consolidated_errors_vector <- c(consolidated_errors_vector, "Error: Required Weaver configuration layout components are missing inside base path.")
        } else {
          consolidated_errors_vector <- c(consolidated_errors_vector, err_content)
        }
      }
  
      if (length(all_errors$duplicate_objectives) > 0) {
        consolidated_errors_vector <- c(consolidated_errors_vector, unique(unlist(all_errors$duplicate_objectives)))
      }

      if (length(all_errors$code_syntax_errors) > 0) {
        consolidated_errors_vector <- c(consolidated_errors_vector, unique(unlist(all_errors$code_syntax_errors)))
      }
  
      if (length(all_errors$schema_version_mismatch) > 0) {
        consolidated_errors_vector <- c(consolidated_errors_vector, "Error: One or more JSON layout configuration files mismatch the active simulator runtime schema version.")
      }
  
      if (length(consolidated_errors_vector) == 0) return(NULL)
  
      div(class = "alert alert-danger mb-4", role = "alert",
          div(class = "d-flex align-items-center mb-2 fw-bold fs-6", icon("circle-xmark", class = "me-2"), "General Phase Validation Errors Detected"),
          tags$ul(style = "margin-bottom: 0; padding-left: 20px;", 
              lapply(consolidated_errors_vector, function(err_msg) tags$li(err_msg))
          )
      )
    })
    
    # Area B: Render Local Context Errors underneath the impacted fields (Passive mapping)
    output$error_local_population_size     <- renderUI({ req(active_errors$population_size);     p(icon("circle-xmark"), active_errors$population_size,     class = "text-danger small fw-bold mt-1 mb-0") })
    output$error_local_tournament_size     <- renderUI({ req(active_errors$tournament_size);     p(icon("circle-xmark"), active_errors$tournament_size,     class = "text-danger small fw-bold mt-1 mb-0") })
    output$error_local_program_text_path   <- renderUI({ req(active_errors$program_text_path);   p(icon("circle-xmark"), active_errors$program_text_path,   class = "text-danger small fw-bold mt-1 mb-0") })
    
    # Custom local view for the configuration directory unrolling the dotted lines
    output$error_local_base_config_text_path <- renderUI({ 
      # CORRECCIÓN ENCONTRADA: '||' reemplazado por comprobaciones individuales is.null y de longitud seguras
      has_structure_error <- !is.null(active_errors$base_config_text_path)
      has_version_mismatch <- length(active_errors$schema_version_mismatch) > 0
      
      # Si no hay ninguna de las dos anomalías activas, detenemos el renderizado de forma segura
      req(has_structure_error || has_version_mismatch)
      
      err_content    <- active_errors$base_config_text_path
      version_issues <- active_errors$schema_version_mismatch
      
      div(class = "text-danger small fw-bold text-break mt-1 mb-0",
          # Escenario A: El directorio base tiene carpetas o archivos json mandatorios ausentes
          if (has_structure_error) {
            if (length(err_content) > 1) {
              tagList(
                p(icon("circle-xmark"), " Error: The directory layout structure breaks mandatory Weaver formats. Missing elements:"),
                tags$ul(style = "margin-top: 5px; margin-bottom: 5px; padding-left: 20px;", 
                       lapply(err_content, function(item) tags$li(item)))
              )
            } else {
              p(icon("circle-xmark"), err_content)
            }
          } else { NULL },
          
          # Escenario B: Las carpetas existen, pero hay archivos JSON que infringen la firma del simulador
          if (has_version_mismatch) {
            tagList(
              p(icon("circle-xmark"), " Error: Schema compatibility integrity failure. The following asset configurations mismatch your execution binary runtime version:"),
              tags$ul(style = "margin-top: 5px; margin-bottom: 0; padding-left: 20px; font-family: 'Fira Code', monospace; font-size: 0.8rem;", 
                     lapply(version_issues, function(item) tags$li(item)))
            )
          } else { NULL }
      )
    })
    
    # --- METADATA OBJECTIVES LOCALIZED PASSIVE ERROR RENDERERS ---
    # Dynamically binds text strings underneath the individual text boxes inside sub-cards
    observe({
      active_card_ids <- inserted_objectives$ids
      req(length(active_card_ids) > 0)
      
      lapply(active_card_ids, function(card_id) {
        id_str <- as.character(card_id)

        output[[paste0("error_local_obj_name_", card_id)]] <- renderUI({
          # Read localized failure token from the master register map array
          localized_error_msg <- active_errors$duplicate_objectives[[id_str]]

          req(!is.null(localized_error_msg))
          p(icon("circle-xmark"), localized_error_msg, class = "text-danger small fw-bold mt-1 mb-0")
        })

        output[[paste0("error_local_obj_code_", card_id)]] <- renderUI({
          selected_function_source <- input[[paste0("obj_fitness_function_", card_id)]]
      
          req(!is.null(selected_function_source))
          req(selected_function_source == "Custom Block")

          localized_error_msg <- active_errors$code_syntax_errors[[id_str]]

          req(!is.null(localized_error_msg))
          p(icon("circle-xmark"), localized_error_msg, class = "text-danger small fw-bold mt-1 mb-0")
        })
      })
    })

    output$error_local_zero_objectives <- renderUI({
      req(active_errors$zero_objectives)
      
      # Displays the exact matching character string vector log in passive red text
      p(icon("circle-xmark"), active_errors$zero_objectives, class = "text-danger small fw-bold mt-1 mb-0")
    })

    # Area C: Success validations confirmations tags badges indicators
    output$schema_version_status <- renderUI({
      req(is.null(active_errors$program_text_path))
      ver <- schema_version()
      if (!is.null(ver)) p(icon("circle-check"), HTML(paste(" <b>Schema Compatibility Version:</b>", tags$span(class = "badge bg-success", ver))), class = "text-success small mt-1 mb-0")
      else if (!is.null(active_errors$schema_detection)) p(icon("triangle-exclamation"), active_errors$schema_detection, class = "text-danger small fw-bold mt-1 mb-0")
    })
    output$display_base_config_path <- renderUI({ req(is.null(active_errors$base_config_text_path)); p(tags$code(input$base_config_text_path), class = "text-muted small text-break mt-1 mb-0") })
    
    # ============================================================================
    # 6. SHINYFILES SYSTEM VOLUMES MOUNT & SLIDERS LINKAGE WORKFLOWS
    # ============================================================================
    observeEvent(input$crossover_prob,     { updateNumericInput(session, "crossover_prob_num", value = input$crossover_prob) })
    observeEvent(input$crossover_prob_num, { updateSliderInput(session, "crossover_prob", value = input$crossover_prob_num) })
    observeEvent(input$mutation_rate,      { updateNumericInput(session, "mutation_rate_num", value = input$mutation_rate) })
    observeEvent(input$mutation_rate_num,  { updateSliderInput(session, "mutation_rate", value = input$mutation_rate_num) })
    
    shinyFiles::shinyFileChoose(input, "program_file", roots = shared_volumes, session = session, pattern = "^.*\\.([eE][xX][eE])$")
    shinyFiles::shinyDirChoose(input, "base_config_dir", roots = shared_volumes, session = session)
    shinyFiles::shinyDirChoose(input, "output_dir", roots = shared_volumes, session = session)
    
    observeEvent(input$program_file,    { req(!is.integer(input$program_file));    file_info <- shinyFiles::parseFilePaths(shared_volumes, input$program_file);    if (nrow(file_info) > 0) updateTextInput(session, "program_text_path", value = normalizePath(file_info$datapath, mustWork = FALSE)) })
    observeEvent(input$base_config_dir, { req(!is.integer(input$base_config_dir)); dir_path  <- shinyFiles::parseDirPath(shared_volumes, input$base_config_dir);   if (length(dir_path) > 0)  updateTextInput(session, "base_config_text_path", value = normalizePath(dir_path, mustWork = FALSE)) })
    
    # ============================================================================
    # DYNAMIC CONDITIONAL INTERNAL CARD RENDERING WORKFLOW
    # ============================================================================
    observe({
      active_card_ids <- inserted_objectives$ids
  
      lapply(active_card_ids, function(card_id) {
        id_str <- as.character(card_id)
    
        output[[paste0("obj_body_conditional_ui_", card_id)]] <- renderUI({
          selected_function_source <- input[[paste0("obj_fitness_function_", card_id)]]
          req(!is.null(selected_function_source))
      
          if (selected_function_source == "Custom Block") {
            # Isolate the exact dynamic snapshot matching this sequential card entry node
            saved_snapshot <- inserted_objectives$objectives_data[[id_str]]
        
            default_code_body <- if (!is.null(saved_snapshot$custom_code_raw)) {
              saved_snapshot$custom_code_raw
            } else {
              "    # Write your custom R body code here\n    return(1)"
            }
        
            tagList(
              div(class = "mt-2 mb-1", tags$span("Custom Fitness Function Editor:")),
          
              # Function Header Signatures Simulation Layer
              div(class = "code-editor-header-wrapper", 
                  "fitness_function <- function(result_folder) {"
              ),
          
              # Isolated Text Area Input containing the core styling hooks
              div(class = "code-editor-textarea-container",
                  textAreaInput(
                    inputId = session$ns(paste0("obj_custom_code_", card_id)), 
                    label   = NULL, 
                    value   = default_code_body, 
                    rows    = 4, 
                    width   = "100%"
                  )
              ),
          
              # Function Closure Key Simulation Layer
              div(class = "code-editor-footer-wrapper", 
                  "}"
              )
            )
          } else {
            NULL
          }
        })
      })
    })

    # ============================================================================
    # METADATA FIELDS REAL-TIME SYNCHRONIZATION ENGINE
    # ============================================================================
    observe({
      active_card_ids <- inserted_objectives$ids
      req(length(active_card_ids) > 0)
  
      lapply(active_card_ids, function(card_id) {
        id_str      <- as.character(card_id)
        in_name     <- input[[paste0("obj_name_", card_id)]]
        in_criteria <- input[[paste0("obj_criteria_", card_id)]]
        in_fit_func <- input[[paste0("obj_fitness_function_", card_id)]]
    
        req(!is.null(in_name), !is.null(in_criteria), !is.null(in_fit_func))
    
        # Initialize buffers to segregate parameters safely across iterations
        raw_custom_code <- NULL
    
        if (in_fit_func == "Custom Block") {
          # Extract character lines written exclusively within this specific sub-card scope
          raw_custom_code <- input[[paste0("obj_custom_code_", card_id)]]
      
          # Inject a dynamic empty functional baseline blueprint for future processing pipelines
          function() {
            # Execution layout sequence pending future runtime compiler adjustments
          }
        }
    
        # Save parameters inside the dedicated container mapping entry slots
        card_data_snapshot <- list(
          name              = trimws(in_name),
          criteria          = in_criteria,
          fitness_func_name = in_fit_func,
          custom_code_raw   = raw_custom_code       # Safely isolates this specific code string from other cards
        )
    
        isolate({
          inserted_objectives$objectives_data[[id_str]] <- card_data_snapshot
          inserted_objectives$names_map[[id_str]]       <- trimws(in_name)
        })
      })
    })

    # ============================================================================
    # DOM-STABILIZED MEMORY RESTORATION INITIALIZER
    # ============================================================================
    output$local_client_scripts <- renderUI({
      isolate({
        cached_objectives <- global_store$phase_1$computing_environment$fitness_objectives
        
        if (!is.null(cached_objectives) && length(cached_objectives$ids) > 0) {
          # Deeply restore reactiveValues lists from orchestrator master cache layers
          inserted_objectives$ids             <- cached_objectives$ids
          inserted_objectives$names_map       <- cached_objectives$names_map
          inserted_objectives$objectives_data <- cached_objectives$objectives_data
          inserted_objectives$code_errors     <- cached_objectives$code_errors
          
          message(paste("[DOM Control] Interface stabilized. Re-injecting sequential objective cards. Count:", length(cached_objectives$ids)))
          
          for (saved_id in cached_objectives$ids) {
            id_key_string <- as.character(saved_id)
            saved_metadata_snapshot <- cached_objectives$objectives_data[[id_key_string]]
            
            insertUI(
              selector = paste0("#", session$ns("objectives_anchor")),
              where    = "beforeEnd",
              ui       = render_single_objective_card_ui(saved_id, saved_meta = saved_metadata_snapshot),
              immediate = TRUE
            )
          }
          
          # --- CASE A: RESTORE MULTI-OBJECTIVE RANKING ---
          cached_ranking <- global_store$phase_1$computing_environment$priority_ranking
          if (!is.null(cached_ranking)) {
            shiny::updateTextInput(session, "multiobjective_order", value = cached_ranking)
          }
          
          # --- CASE B: RESTORE INDIVIDUAL WEIGHTS COEFFICIENTS ---
          cached_weights <- global_store$phase_1$computing_environment$assigned_weights
          if (!is.null(cached_weights) && length(cached_weights) > 0) {
            obj_names <- names(cached_weights)
            
            for (name in obj_names) {
              saved_weight_value <- cached_weights[[name]]
              if (!is.null(saved_weight_value)) {
                shiny::updateNumericInput(
                  session = session, 
                  inputId = paste0("weight_for_", name), 
                  value   = saved_weight_value
                )
              }
            }
          }
        }
      })
      NULL
    })
    
    # ============================================================================
    # RENDERING OF THE LOWER STRATEGY PARAMETERS PANEL (DYNAMIC MATRIX)
    # ============================================================================
    output$fitness_strategy_specific_ui <- renderUI({
      req(input$fitness_class)
      active_card_ids <- inserted_objectives$ids
      
      # Abort or notify if no components are active
      if (length(active_card_ids) == 0) {
        return(p(class = "text-muted small italic", "Fitness strategies configuration modules pending attachment."))
      }
      
      # Cross-examine baseline validation status to ensure names are clean and unique before setup
      has_naming_errors <- length(unlist(shiny::reactiveValuesToList(active_errors)$duplicate_objectives)) > 0
      if (has_naming_errors) {
        return(p(class = "text-muted small italic", "Please resolve naming and duplicate validation errors above to configure strategy parameters."))
      }
      
      # Extract pristine unique target keys consolidated from map stores
      obj_names <- unlist(lapply(active_card_ids, function(id) inserted_objectives$names_map[[as.character(id)]]))
      
      # --- CASE A: MULTI-OBJECTIVE PRIORITY RANKING (VERTICAL DISPOSITION WITH CACHE AWARENESS) ---
      if (input$fitness_class == "MultiObjectiveFitness") {
        # 1. Gather the pristine unique target names mapped in current state
        obj_names <- unlist(lapply(active_card_ids, function(id) inserted_objectives$names_map[[as.character(id)]]))
        
        # 2. Interrogate the transactional master store cache layer to see if a priority vector exists
        cached_ranking <- global_store$phase_1$computing_environment$priority_ranking
        
        if (!is.null(cached_ranking) && length(cached_ranking) > 0) {
          # Filter cached ranking to ensure it only contains items currently active (avoids ghost names)
          valid_cached_ranking <- cached_ranking[cached_ranking %in% obj_names]
          
          # Find any new objectives added after the transaction that aren't in the cache yet
          missing_items <- setdiff(obj_names, valid_cached_ranking)
          
          # Re-stitch the vector: respect the exact cached order and append new elements at the bottom
          obj_names <- c(valid_cached_ranking, missing_items)
        }
        
        tagList(
          tags$b("MultiObjective Priority Ranking Hierarchy:"),
          p(class = "text-muted small mb-3", 
            "Drag and drop the items below to arrange them vertically by order of criteria importance (The first item at the top of the list represents the highest priority, descending sequentially down to the lowest):"
          ),
          
          # Wrapped inside a custom class to intercept and enforce vertical flex structures
          tags$div(
            class = "vertical-order-container",
            shinyjqui::orderInput(
              inputId     = session$ns("multiobjective_order"),
              label       = NULL,
              items       = obj_names,
              item_class  = "primary", 
              placeholder = "Drag items here to register active priority vectors..."
            )
          )
        )
        
      # --- CASE B: LINEAR WEIGHTED COMBINATION COEFFICIENTS (UNIFIED STRUCTURE) ---
      } else if (input$fitness_class == "WeightedFitness") {
        tagList(
          # Row 1: Aligned Header matching exactly the label structure of Case A
          div(class = "d-flex justify-content-between align-items-center mb-2",
              tags$b("Weighted Strategy Matrix Configuration:"),
              actionButton(
                inputId = session$ns("normalize_weights_btn"), 
                label   = "Normalize to 1.0", 
                class   = "btn btn-outline-primary btn-sm fw-bold", 
                icon    = icon("wand-magic-sparkles")
              )
          ),
          p(class = "text-muted small mb-3", 
            "Assign relative continuous weights for the linear summation. The entire sum space must aggregate vertically and equal exactly to 1.0:"
          ),

          # Row 2: Vertical box layout with clean unrolled elements
          tags$div(
            class = "vertical-weights-container",
            
            lapply(obj_names, function(name) {
              w_input_id <- paste0("weight_for_", name)
              
              cached_weight_snapshot <- global_store$phase_1$computing_environment$assigned_weights[[name]]
              
              current_input_value <- if (!is.null(cached_weight_snapshot)) {
                cached_weight_snapshot
              } else if (!is.null(input[[w_input_id]])) {
                input[[w_input_id]]
              } else {
                0.0
              }
              
              div(class = "weight-row-item row g-0 align-items-center",
                  div(class = "col-md-7 text-truncate", 
                      tags$span(icon("scale-balanced", class = "me-2 text-muted"), tags$code(name))
                  ),
                  div(class = "col-md-5 decimal-input text-end",
                      numericInput(session$ns(w_input_id), label = NULL, value = current_input_value, min = 0, max = 1, step = 0.01, width = "120px")
                  )
              )
            })
          ),

          # Row 3: Corrected Localized contextual message layout positioning
          if (!is.null(active_errors$weights_validation)) {
            div(class = "mt-3 pt-2 border-top border-danger-subtle text-danger small fw-bold text-break d-block",
                p(icon("circle-xmark", class = "me-1"), active_errors$weights_validation, class = "mb-0")
            )
          }
        )
      }
    })

    # ============================================================================
    # AUTOMATED MATHEMATICAL WEIGHTS NORMALIZATION EVENT OBSERVER
    # ============================================================================
    observeEvent(input$normalize_weights_btn, {
      active_card_ids <- inserted_objectives$ids
      req(length(active_card_ids) > 0)
      
      obj_names <- unlist(lapply(active_card_ids, function(id) inserted_objectives$names_map[[as.character(id)]]))
      w_count <- length(obj_names)
      vals <- numeric(w_count)
      
      for (k in seq_len(w_count)) {
        current_val <- input[[paste0("weight_for_", obj_names[k])]]
        vals[k] <- if (!is.null(current_val) && !is.na(current_val)) current_val else 0.0
      }
      
      total_sum <- sum(vals)
      # Balanced uniform distribution backup fallback if sum maps to absolute zero
      norm_vals <- if (total_sum == 0) rep(1 / w_count, w_count) else vals / total_sum
      
      # Formulate structural decimal quantization
      norm_vals <- round(norm_vals, 2)
      residual_diff <- 1.0 - sum(norm_vals)
      norm_vals[w_count] <- norm_vals[w_count] + residual_diff # Correct floating discrepancies
      
      # Dispatch clean updates back into the UI numeric nodes
      for (k in seq_len(w_count)) {
        updateNumericInput(session, paste0("weight_for_", obj_names[k]), value = norm_vals[k])
      }
    }, ignoreInit = TRUE)

    # ============================================================================
    # 7. TRANSACTIONAL PHASE VALIDITY EVALUATOR
    # ============================================================================
    # Reactive expression that returns TRUE only when the entire phase registry has zero errors
    phase_1_is_valid <- reactive({
      all_errors_list <- reactiveValuesToList(active_errors)
      # Filter out NULL elements to check if any active error string exists
      active_error_vectors <- na.omit(unlist(all_errors_list))
      
      # Phase is valid only if the error registry count equals exactly zero
      return(length(active_error_vectors) == 0)
    })
    
    # ============================================================================
    # 8. TRANSACTIONAL COMMIT RETAINER (SAVING ONLY ON PROGRESS ADVANCE COMMIT)
    # ============================================================================
    # This encapsulated function is exposed to the orchestrator. It triggers 
    # exclusively when the user clicks 'Next Phase', preventing background corruption loops.
    commit_phase_1_data_transaction <- function() {
      all_errors <- na.omit(reactiveValuesToList(active_errors))
      
      # Cambiar las tres asignaciones 'current_state$' por 'global_store$phase_1$'
      global_store$phase_1$ga_settings <- list(
        population_size = input$population_size, 
        max_generations = input$max_generations, 
        tournament_size = input$tournament_size, 
        crossover_prob  = input$crossover_prob, 
        mutation_rate   = input$mutation_rate, 
        keep_best       = input$keepBest, 
        is_valid        = (length(all_errors) == 0)
      )

      global_store$phase_1$computing_environment <- list(
        fitness_class      = input$fitness_class, 
        fitness_objectives = shiny::reactiveValuesToList(inserted_objectives),

        priority_ranking   = if (input$fitness_class == "MultiObjectiveFitness") input$multiobjective_order else NULL,
        assigned_weights   = if (input$fitness_class == "WeightedFitness") {
          active_card_ids <- inserted_objectives$ids
          obj_names <- unlist(lapply(active_card_ids, function(id) inserted_objectives$names_map[[as.character(id)]]))
          stats::setNames(lapply(obj_names, function(n) input[[paste0("weight_for_", n)]]), obj_names)
        } else { NULL }
      )
      
      global_store$phase_1$system_paths <- list(
        program_executable      = input$program_text_path, 
        base_configuration      = input$base_config_text_path, 
        detected_schema_version = schema_version()
      )
      
      message("[Phase 1 Store] Master Transactional cache commit finalized successfully.")
    }
    
    # ============================================================================
    # 9. MODULE OUTPUT RETURNS REGISTRY
    # ============================================================================
    # We return these reactive handles so the main orchestrator can monitor health 
    # and execute the commit sequence on step actions.
    return(list(
      is_valid    = phase_1_is_valid,
      commit_data = commit_phase_1_data_transaction
    ))
  })
}