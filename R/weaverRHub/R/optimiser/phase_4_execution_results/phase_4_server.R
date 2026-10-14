#' Phase 4: Launch Simulation & View Results Server
#'
#' @param id Unique module namespace identifier string.
#' @param global_store The global reactiveValues store mapping transactional pipeline data metrics.
#' @param shared_volumes List containing unified operating system mounts.
#' @import shiny
#' @importFrom shinyFiles shinyDirChoose parseDirPath
#' @export
mod_optimiser_phase_4_server = function(id, global_store, shared_volumes) {
  moduleServer(id, function(input, output, session) {
    ns <- session$ns
    message("[Phase 4 Server] Standalone execution space stabilized successfully.")
    
    active_errors <- reactiveValues(
      output_directory   = NULL,
      json_filename      = NULL
    )
    
    # Inicialización del manejador de directorios nativo de shinyFiles
    shinyFiles::shinyDirChoose(
      input, 
      id      = "output_dir_browse_btn", 
      roots   = shared_volumes, 
      session = session
    )

    observeEvent(input$output_dir_browse_btn, {
      req(!is.integer(input$output_dir_browse_btn))
      dir_path <- shinyFiles::parseDirPath(shared_volumes, input$output_dir_browse_btn)

      if (length(dir_path) > 0) {
        updateTextInput(
          session = session, 
          inputId = "output_dir_text_path", 
          value   = normalizePath(dir_path, mustWork = FALSE)
        )
      }
    })
    
    # ============================================================================
    # ISOLATED EVENT LISTENERS VALIDATORS
    # ============================================================================
    observeEvent(input$output_dir_text_path, {
      path <- input$output_dir_text_path
      if (is.null(path) || path == "") {
        active_errors$output_directory <- "Error: Target output folder path cannot be left blank."
      } else if (!dir.exists(path)) {
        active_errors$output_directory <- "Error: The specified target path does not point to an existing directory."
      } else {
        active_errors$output_directory <- NULL
      }
    }, ignoreInit = FALSE)
    
    observeEvent(input$config_json_filename, {
      filename = trimws(input$config_json_filename)
      if (is.null(filename) || filename == "") {
        active_errors$json_filename <- "Error: Export target filename cannot be left blank."
      } else if (grepl("[\\/:*?\"<>|]", filename)) {
        active_errors$json_filename <- "Error: Filename contains illegal filesystem characters (e.g., /, \\, *, ?, <, >)."
      } else {
        active_errors$json_filename <- NULL
      }
    }, ignoreInit = FALSE)
    
    # ============================================================================
    # RENDERING SLOTS
    # ============================================================================
    output$error_local_output_dir_path = renderUI({ req(active_errors$output_directory); p(icon("circle-xmark"), active_errors$output_directory, class = "text-danger small fw-bold mt-1 mb-0 text-break") })
    output$error_local_json_filename   = renderUI({ req(active_errors$json_filename); p(icon("circle-xmark"), active_errors$json_filename, class = "text-danger small fw-bold mt-1 mb-0 text-break") })
    output$display_output_dir_path_confirmation = renderUI({ req(is.null(active_errors$output_directory)); req(input$output_dir_text_path != ""); p(tags$code(input$output_dir_text_path), class = "text-muted small text-break mt-1 mb-0") })
    
    phase_4_is_valid = reactive({
      all_errors_list <- shiny::reactiveValuesToList(active_errors)
      active_error_vectors <- na.omit(unlist(all_errors_list))
      return(length(active_error_vectors) == 0)
    })
    
    commit_phase_4_data_transaction = function() {
      all_errors <- na.omit(reactiveValuesToList(active_errors))
      global_store$phase_4$optimization_output <- list(
        output_directory = input$output_dir_text_path,
        export_filename  = paste0(trimws(input$config_json_filename), ".json"),
        is_valid         = (length(all_errors) == 0)
      )
      message("[Phase 4 Store] Output parameters transactional cache commit finalized successfully.")
    }

    output$render_export_state_controls <- renderUI({
      # Estructura condicional del botón de salvado para replicar el comportamiento de 'Next Phase'
      save_button_layout <- if (phase_4_is_valid()) {
        actionButton(
          inputId = session$ns("save_optimization_config_btn"), 
          label   = "Save Optimization Configuration", 
          class   = "btn btn-primary btn-sm mt-3 w-100", 
          icon    = icon("floppy-disk")
        )
      } else {
        tags$button(
          id    = session$ns("save_optimization_config_disabled"), 
          type  = "button", 
          class = "btn btn-sm mt-3 w-100",
          title = "Requirement: Resolve all validation and filesystem path errors above to proceed.",
          style = "background-color: var(--bs-secondary-bg); color: var(--bs-secondary-color); border-color: var(--bs-border-color); cursor: not-allowed; opacity: 0.65;",
          icon("ban"), " Save Optimization Configuration"
        )
      }
  
      tagList(
        save_button_layout
      )
    })

    observeEvent(input$save_optimization_config_btn, {
      # Medidas de seguridad defensivas para asegurar la integridad de la llamada
      req(phase_4_is_valid())
      req(!is.null(global_store$phase_1))
  
      # 1. Resolución analítica de la ruta física final del archivo de destino
      target_directory <- input$output_dir_text_path
      clean_filename   <- trimws(input$config_json_filename)
      final_file_path  <- file.path(target_directory, paste0(clean_filename, ".json"))
  
      message("[Phase 4 Export] Compiling comprehensive optimization setup. Target: ", final_file_path)
  
      # 2. Extracción profunda y desacoplamiento de metadatos de las fases previas
      p1_ga   <- global_store$phase_1$ga_settings
      p1_env  <- global_store$phase_1$computing_environment
      p1_path <- global_store$phase_1$system_paths
  
      # 3. Construcción del Esquema Unificado Portable (Independiente del entorno de hardware)
      optimization_schema_blueprint <- list(
        # Metadatos de control de la versión del simulador
        metadata = list(
          schema_version          = p1_path$detected_schema_version,
          export_timestamp        = as.character(Sys.time())
        ),
    
        # Parámetros del Algoritmo Genético (Fase 1)
        genetic_algorithm_settings = list(
          population_size = p1_ga$population_size,
          max_generations = p1_ga$max_generations,
          tournament_size = p1_ga$tournament_size,
          crossover_prob  = p1_ga$crossover_prob,
          mutation_rate   = p1_ga$mutation_rate,
          keep_best       = p1_ga$keep_best
        ),
    
        # Entorno de Computación y Estrategia de Fitness (Fase 1)
        computing_environment = list(
          fitness_class    = p1_env$fitness_class,
          random_seed      = p1_env$random_seed,
          priority_ranking = p1_env$priority_ranking,
          assigned_weights = p1_env$assigned_weights,
      
          # Estructura interna de los objetivos dinámicos configurados
          objectives = lapply(p1_env$fitness_objectives$objectives_data, function(obj) {
            list(
              name              = obj$name,
              criteria          = obj$criteria,
              fitness_func_name = obj$fitness_func_name,
              custom_code_raw   = obj$custom_code_raw
            )
          })
        ),
    
        defined_parameters = global_store$phase_3$defined_parameters
      )
  
      # 4. Escritura transaccional en el disco duro mediante codificación JSON estructurada
      tryCatch({
        jsonlite::write_json(
          x             = optimization_schema_blueprint, 
          path          = final_file_path, 
          pretty        = TRUE, 
          auto_unbox    = TRUE, 
          digits        = NA, 
          force         = TRUE
        )
    
        message("[Phase 4 Export] Success: Configuration footprint written flawlessly.")
    
        # Inyección visual opcional para dar feedback instantáneo de éxito al usuario en el cliente
        shinyjs::info(paste0("¡Configuración exportada con éxito!\n\nEl archivo portable se ha guardado en:\n", final_file_path))
    
      }, error = function(e) {
        message("[Phase 4 Export Error] Critical failure writing JSON payload: ", e$message)
        shinyjs::info(paste0("Error crítico al exportar la configuración:\n", e$message))
      })
    })
    
    return(list(
      is_valid    = phase_4_is_valid,
      commit_data = commit_phase_4_data_transaction
    ))
  })
}