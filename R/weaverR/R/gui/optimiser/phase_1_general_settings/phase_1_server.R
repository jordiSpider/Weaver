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

    # Secure storage schema tracking card IDs and all their internal input fields structures
    inserted_objectives <- reactiveValues(
      ids             = c(),    # Integer vector tracking unique active sequential IDs (1, 2, 3...)
      names_map       = list(), # Character strings mapping indexed by string keys: "1", "2"...
      objectives_data = list(), # Nested metadata lists configuration snapshots indexed by string keys
      code_errors     = list()  # Syntax error tracking lists indexed by string keys
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
    # Trimmed objective names currently typed by the user, keyed by card id ("1", "2", ...).
    # NA means "the text input of that card is not bound yet", which is NOT the same as a blank "".
    current_objective_names <- reactive({
      ids <- inserted_objectives$ids
      if (length(ids) == 0) return(stats::setNames(character(0), character(0)))

      raw_names <- vapply(ids, function(id) {
        v <- input[[paste0("obj_name_", id)]]
        if (is.null(v) || length(v) != 1L || is.na(v)) NA_character_ else trimws(as.character(v))
      }, character(1))

      stats::setNames(raw_names, as.character(ids))
    })

    # IMPORTANT: this observer must NEVER read an `active_errors` slot that it also writes.
    # Doing `active_errors$x[[id]] <- value` performs a reactive READ of `x` followed by a WRITE,
    # which makes the observer depend on itself. Combined with a "reset to list()" at the start of
    # each run, the value flips list() -> list(err) on every execution, the observer invalidates
    # itself forever and the R process never returns to the event loop (the web app freezes).
    # Here every result is accumulated in plain local lists and written ONCE at the end.
    observe({
      active_card_ids <- inserted_objectives$ids
      obj_names       <- current_objective_names()

      if (length(active_card_ids) == 0) {
        active_errors$zero_objectives      <- "Error: At least one fitness objective must be defined to run the simulation."
        active_errors$duplicate_objectives <- list()
        active_errors$code_syntax_errors   <- list()
        active_errors$weights_validation   <- NULL
        return()
      }
      active_errors$zero_objectives <- NULL

      name_errors <- list()   # blank / duplicated identifiers, keyed by card id
      code_errors <- list()   # custom code problems, keyed by card id

      for (card_id in active_card_ids) {
        id_str      <- as.character(card_id)
        target_name <- obj_names[[id_str]]

        # RULE 1 + 2: blank and duplicated identifiers (skipped while the input is not bound yet)
        if (!is.na(target_name)) {
          if (!nzchar(target_name)) {
            name_errors[[id_str]] <- "Error: Objective name cannot be left blank."
          } else if (sum(obj_names == target_name, na.rm = TRUE) > 1L) {
            name_errors[[id_str]] <- paste0("Error: The identifier name '", target_name, "' is duplicated. Each objective must have a unique identifier.")
          }
        }

        # RULE 3: static analysis of custom fitness code
        in_fit_func <- input[[paste0("obj_fitness_function_", card_id)]]
        if (identical(in_fit_func, "Custom Block")) {
          raw_custom_code <- input[[paste0("obj_custom_code_", card_id)]]

          # NULL = the code editor has not been rendered/bound yet, so there is nothing to judge
          if (!is.null(raw_custom_code)) {
            if (!nzchar(trimws(raw_custom_code))) {
              code_errors[[id_str]] <- "Error: Custom fitness function body code cannot be left blank."
            } else if (!grepl("return(\\s*\\(|\\s+|$)", raw_custom_code)) {
              code_errors[[id_str]] <- "Error: Missing statement. The custom fitness function must explicitly contain a 'return()' statement to pass computed metrics."
            }
          }
        }
      }

      # Single write per slot. Because the lists are rebuilt from scratch on every run,
      # entries of removed cards disappear automatically (no stale errors left behind).
      active_errors$duplicate_objectives <- name_errors
      active_errors$code_syntax_errors   <- code_errors

      # WEIGHTED STRATEGY: the sum of coefficients is only meaningful with valid, unique names
      fitness_class <- input$fitness_class
      if (is.null(fitness_class) || fitness_class != "WeightedFitness" ||
          length(name_errors) > 0L || anyNA(obj_names)) {
        active_errors$weights_validation <- NULL
        return()
      }

      weight_values <- lapply(unname(obj_names), function(n) input[[paste0("weight_for_", n)]])

      if (any(vapply(weight_values, is.null, logical(1)))) {
        # Weight inputs are not rendered yet; this observer re-runs as soon as they bind.
        active_errors$weights_validation <- NULL
      } else if (any(vapply(weight_values, function(w) length(w) != 1L || is.na(w), logical(1)))) {
        active_errors$weights_validation <- "Error: All weighted strategy coefficients must be filled with a valid numeric value."
      } else {
        weights_sum <- sum(unlist(weight_values))
        if (abs(weights_sum - 1.0) > 1e-5) {
          active_errors$weights_validation <- paste0("Error: The sum of the weighted strategy coefficients is ", sprintf("%.17g", weights_sum), ". It must equal exactly 1.0.")
        } else {
          active_errors$weights_validation <- NULL
        }
      }
    })

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
        class = "mb-3 border secondary-subtle shadow-sm card",
        style = "padding: 12px;",
    
        # Header Row containing the dynamic title code and the destruction button
        div(class = "d-flex justify-content-between align-items-center mb-2 pb-2 border-bottom",
            tags$span(
              id = ns_func(paste0("objective_title_display_", card_id)), 
              class = "fw-bold text-primary", 
              tags$code(if (nzchar(trimws(final_name))) final_name else "(Blank Name)")
            ),
            actionButton(ns_func(paste0("remove_obj_", card_id)), label = NULL, icon = icon("trash"), class = "btn-outline-danger btn-sm")
        ),
    
        # Row 1: Objective Identification Name and Target Optimization Criteria
        div(class = "row g-2",
            div(class = "col-md-6",
                div(class = "optimiser-input-card",
                    textInput(ns_func(paste0("obj_name_", card_id)), label = tags$b("Objective Name:"), value = final_name),
                    tags$small(class = "text-muted d-block", "Unique alphanumeric identifier used to reference this specific ecosystem metric."),
                    uiOutput(ns_func(paste0("error_local_obj_name_", card_id)))
                )
            ),
            div(class = "col-md-6",
                div(class = "optimiser-input-card",
                    selectInput(ns_func(paste0("obj_criteria_", card_id)), label = tags$b("Criteria:"), choices = c("Maximize" = "MAX", "Minimize" = "MIN"), selected = final_criteria),
                    tags$small(class = "text-muted d-block", "Determines whether the algorithm should drive the target score upward or downward.")
                )
            )
        ),
    
        # Row 2: Unified Single-Input Selection Matrix for the Target Fitness Function
        div(class = "optimiser-input-card",
            selectInput(
              inputId  = ns_func(paste0("obj_fitness_function_", card_id)), 
              label    = tags$b("Fitness Function:"), 
              choices  = fitness_choices, 
              selected = final_fitness_func
            ),
            tags$small(class = "text-muted d-block", "The core mathematical evaluation routine or script template applied to measure this objective.")
        ),
    
        # Dynamic Fragment Placement (Predefined plain text descriptions or Custom R Code IDE view)
        uiOutput(ns_func(paste0("obj_body_conditional_ui_", card_id))),
    
        # Client-side instantaneous header synchronization script
        tags$script(HTML(paste0("
          $('#", ns_func(paste0("obj_name_", card_id)), "').on('input', function() {
            var inputRawText = $.trim($(this).val());
            var placeholderToken = inputRawText === '' ? '(Blank Name)' : inputRawText;
            $('#", ns_func(paste0("objective_title_display_", card_id)), "').empty().append($('<code>').text(placeholderToken));
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
  
      objective_errors <- unlist(all_errors$duplicate_objectives, use.names = FALSE)
      objective_errors <- objective_errors[!is.na(objective_errors) & nzchar(trimws(objective_errors))]
      if (length(objective_errors) > 0) consolidated_errors_vector <- c(consolidated_errors_vector, unique(objective_errors))

      syntax_errors <- unlist(all_errors$code_syntax_errors, use.names = FALSE)
      syntax_errors <- syntax_errors[!is.na(syntax_errors) & nzchar(trimws(syntax_errors))]
      if (length(syntax_errors) > 0) consolidated_errors_vector <- c(consolidated_errors_vector, unique(syntax_errors))
  
      if (length(all_errors$schema_version_mismatch) > 0) {
        consolidated_errors_vector <- c(consolidated_errors_vector, "Error: One or more JSON layout configuration files mismatch the active simulator runtime schema version.")
      }

      consolidated_errors_vector <- as.character(unlist(consolidated_errors_vector, use.names = FALSE))
      consolidated_errors_vector <- unique(consolidated_errors_vector[!is.na(consolidated_errors_vector) & nzchar(trimws(consolidated_errors_vector))])
  
      if (length(consolidated_errors_vector) == 0) return(NULL)
  
      div(class = "alert alert-danger mb-4", role = "alert",
          div(class = "d-flex align-items-center mb-2 fw-bold fs-6", icon("circle-xmark", class = "me-2"), "General Validation Errors Detected"),
          tags$ul(style = "margin-bottom: 0; padding-left: 20px;", 
              lapply(consolidated_errors_vector, function(err_msg) tags$li(err_msg))
          )
      )
    })
    
    # Area B: Render Local Context Errors underneath the impacted fields (Passive mapping)
    output$error_local_population_size     <- renderUI({ 
        req(active_errors$population_size) 
        div(class = "mt-2",
          p(icon("circle-xmark"), active_errors$population_size,     class = "text-danger small fw-bold mt-1 mb-0") 
        )
    })

    output$error_local_tournament_size     <- renderUI({ 
        req(active_errors$tournament_size)
        div(class = "mt-2",
          p(icon("circle-xmark"), active_errors$tournament_size,     class = "text-danger small fw-bold mt-1 mb-0") 
        )
    })

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
          objective_errors <- active_errors$duplicate_objectives
          localized_error_msg <- if (!is.null(objective_errors) && id_str %in% names(objective_errors)) objective_errors[[id_str]] else NULL

          req(!is.null(localized_error_msg))
          div(class = "mt-2",
            p(icon("circle-xmark"), localized_error_msg, class = "text-danger small fw-bold mt-1 mb-0")
          )
        })

        output[[paste0("error_local_obj_code_", card_id)]] <- renderUI({
          selected_function_source <- input[[paste0("obj_fitness_function_", card_id)]]
      
          req(!is.null(selected_function_source))
          req(selected_function_source == "Custom Block")

          syntax_errors <- active_errors$code_syntax_errors
          localized_error_msg <- if (!is.null(syntax_errors) && id_str %in% names(syntax_errors)) syntax_errors[[id_str]] else NULL

          req(!is.null(localized_error_msg))
          div(class = "mt-2",
            p(icon("circle-xmark"), localized_error_msg, class = "text-danger small fw-bold mt-1 mb-0")
          )
        })
      })
    })

    output$error_local_zero_objectives <- renderUI({
      req(active_errors$zero_objectives)
      
      # Displays the exact matching character string vector log in passive red text
      p(icon("circle-xmark"), active_errors$zero_objectives, class = "text-danger small fw-bold mt-1 mb-0")
    })

    output$display_fitness_errors <- renderUI({
      all_errors <- shiny::reactiveValuesToList(active_errors)
      fitness_errors <- c(
        unlist(all_errors$duplicate_objectives, use.names = FALSE),
        unlist(all_errors$code_syntax_errors, use.names = FALSE),
        all_errors$weights_validation
      )
      fitness_errors <- unique(fitness_errors[!is.na(fitness_errors) & nzchar(trimws(fitness_errors))])

      if (length(fitness_errors) == 0) return(NULL)

      div(class = "alert alert-danger mt-3 mb-0", role = "alert",
        tags$ul(class = "mb-0", lapply(fitness_errors, tags$li))
      )
    })

    # Area C: Success validations confirmations tags badges indicators
    output$schema_version_status <- renderUI({
      req(is.null(active_errors$program_text_path))
      ver <- schema_version()
      if (!is.null(ver)) p(icon("circle-check"), HTML(paste(" <b>Schema Compatibility Version:</b>", tags$span(class = "badge bg-success", ver))), class = "text-success small mt-1 mb-0")
      else if (!is.null(active_errors$schema_detection)) p(icon("triangle-exclamation"), active_errors$schema_detection, class = "text-danger small fw-bold mt-1 mb-0")
    })
    output$display_base_config_path <- renderUI({
      req(is.null(active_errors$base_config_text_path), nzchar(input$base_config_text_path))
      absolute_path <- normalizePath(path.expand(input$base_config_text_path), winslash = "/", mustWork = TRUE)
      p(tags$code(absolute_path), class = "text-muted small text-break mt-1 mb-0")
    })
    
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
        ns_func <- session$ns
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
              div(class = "mt-3 optimiser-input-card",
                  div(class = "mb-2", tags$span(tags$b("Custom Fitness Function Editor:"))),
          
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
                  ),

                  uiOutput(ns_func(paste0("error_local_obj_code_", card_id)))
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

      for (card_id in active_card_ids) {
        id_str      <- as.character(card_id)
        in_name     <- input[[paste0("obj_name_", card_id)]]
        in_criteria <- input[[paste0("obj_criteria_", card_id)]]
        in_fit_func <- input[[paste0("obj_fitness_function_", card_id)]]

        # A card whose inputs are not bound yet must not block the synchronisation of the others
        if (is.null(in_name) || is.null(in_criteria) || is.null(in_fit_func)) next

        raw_custom_code <- if (identical(in_fit_func, "Custom Block")) {
          input[[paste0("obj_custom_code_", card_id)]]
        } else {
          NULL
        }

        card_data_snapshot <- list(
          name              = trimws(in_name),
          criteria          = in_criteria,
          fitness_func_name = in_fit_func,
          custom_code_raw   = raw_custom_code
        )

        isolate({
          inserted_objectives$objectives_data[[id_str]] <- card_data_snapshot
          inserted_objectives$names_map[[id_str]]       <- trimws(in_name)
        })
      }
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
    # This panel depends ONLY on the fitness class and on the current objective names.
    # It must not read `active_errors` (every validation message would rebuild it) nor the
    # weight inputs / the global store without isolate() (each keystroke would rebuild it and
    # steal the focus of the numeric box being edited).
    output$fitness_strategy_specific_ui <- renderUI({
      req(input$fitness_class)
      current_names <- current_objective_names()

      if (length(current_names) == 0) {
        return(p(class = "text-muted small italic", "Fitness strategies configuration modules pending attachment."))
      }

      # Some card inputs are not bound yet (card just inserted / restored): wait for the next pass
      req(!anyNA(current_names))

      has_naming_errors <- any(!nzchar(current_names)) || anyDuplicated(unname(current_names)) > 0L
      if (has_naming_errors) {
        return(p(class = "text-muted small italic", "Please resolve naming and duplicate validation errors above to configure strategy parameters."))
      }

      obj_names <- unname(current_names)

      # --- CASE A: MULTI-OBJECTIVE PRIORITY RANKING (VERTICAL DISPOSITION WITH CACHE AWARENESS) ---
      if (input$fitness_class == "MultiObjectiveFitness") {
        cached_ranking <- isolate(global_store$phase_1$computing_environment$priority_ranking)
        if (!is.null(cached_ranking)) {
          cached_ranking <- as.character(unlist(cached_ranking, use.names = FALSE))
        }

        if (!is.null(cached_ranking) && length(cached_ranking) > 0) {
          # Keep only items that are still active (avoids ghost names) and append the new ones
          valid_cached_ranking <- cached_ranking[cached_ranking %in% obj_names]
          missing_items        <- setdiff(obj_names, valid_cached_ranking)
          obj_names            <- c(valid_cached_ranking, missing_items)
        }

        tagList(
          tags$b("MultiObjective Priority Ranking Hierarchy:"),
          p(class = "text-muted small mb-3",
            "Drag and drop the items below to arrange them vertically by order of criteria importance (The first item at the top of the list represents the highest priority, descending sequentially down to the lowest):"
          ),

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
        cached_weights <- isolate(global_store$phase_1$computing_environment$assigned_weights)

        tagList(
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

          tags$div(
            class = "vertical-weights-container",

            lapply(obj_names, function(name) {
              w_input_id <- paste0("weight_for_", name)

              # Priority: value already typed in this session > committed cache > 0
              live_value   <- isolate(input[[w_input_id]])
              cached_value <- cached_weights[[name]]

              current_input_value <- if (!is.null(live_value) && !is.na(live_value)) {
                live_value
              } else if (!is.null(cached_value)) {
                cached_value
              } else {
                0.0
              }

              div(class = "weight-row-item row g-0 align-items-center",
                  div(class = "col-md-7 text-truncate",
                      tags$span(icon("scale-balanced", class = "me-2 text-muted"), tags$code(name))
                  ),
                  div(class = "col-md-5 decimal-input text-end",
                      numericInput(session$ns(w_input_id), label = NULL, value = current_input_value, min = 0, max = 1, step = "any", width = "120px")
                  )
              )
            })
          ),

          # Weights error lives in its own slot (see error_local_weights) so that it never rebuilds this panel
          uiOutput(session$ns("error_local_weights"))
        )
      }
    })

    output$error_local_weights <- renderUI({
      req(active_errors$weights_validation)
      div(class = "mt-3 pt-2 border-top border-danger-subtle text-danger small fw-bold text-break d-block",
          p(icon("circle-xmark", class = "me-1"), active_errors$weights_validation, class = "mb-0")
      )
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
      
      # Preserve double-precision values and correct only the floating-point sum residual.
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
        program_executable      = if (nzchar(input$program_text_path)) {
          normalizePath(path.expand(input$program_text_path), winslash = "/", mustWork = FALSE)
        } else {
          input$program_text_path
        },
        base_configuration      = if (nzchar(input$base_config_text_path)) {
          normalizePath(path.expand(input$base_config_text_path), winslash = "/", mustWork = FALSE)
        } else {
          input$base_config_text_path
        },
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
