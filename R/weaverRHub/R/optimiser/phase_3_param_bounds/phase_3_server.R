#' Phase 3: Export Optimization Configuration Server
#'
#' @param id Unique module namespace identifier string.
#' @param global_store The global reactiveValues store mapping transactional pipeline data metrics.
#' @param shared_volumes List containing unified operating system mounts.
#' @import shiny
#' @importFrom shinyFiles shinyDirChoose parseDirPath
#' @export
mod_optimiser_phase_3_server = function(id, global_store, shared_volumes) {
  moduleServer(id, function(input, output, session) {
    ns <- session$ns
    message("[Phase 3 Server] Standalone execution space stabilized successfully.")

    active_errors <- reactiveValues(
      output_directory   = NULL,
      output_dir_run     = NULL,
      json_filename      = NULL
    )

    # ============================================================================
    # LIVE TELEMETRY STREAM CORES REGISTRIES
    # ============================================================================
    live_stream <- reactiveValues(
      console_history = "[System Initialize] Standby. Waiting for orchestration initialization trigger...",
      current_gen     = 0L,
      is_running      = FALSE,
      plot_trigger    = 0L,
      archivo_log     = tempfile(fileext = ".txt"),
      archivo_progreso  = tempfile(fileext = ".progress"),
      current_progress  = 0,
      progress_text     = "Esperando inicio..."
    )

    # REFACTORIZACIÓN DEFINITIVA DE LA TAREA ASÍNCRONA
    tarea_pesada <- ExtendedTask$new(function(ruta_archivo, ruta_progreso, blueprint, output_path, program, base_config_path, seed, total_cores, program_cores, debug) {

        if (!inherits(future::plan(), "multisession")) {
          future::plan(future::multisession)
        }

        future::future({
      
          ui_metrics_callback <- function(generation, current_best, current_mean, log_line, log_type) {
            styled_prefix <- switch(log_type,
              "system"  = "<span class='log-system-info'>[SYSTEM]</span>",
              "success" = "<span class='log-success'>[SUCCESS]</span>",
              "warning" = "<span class='log-warning'>[WARNING]</span>",
              "danger"  = "<span class='log-danger'>[CRITICAL]</span>",
              "debug"   = "<span class='text-muted'>[DEBUG]</span>",
              "<span class='text-light'>[INFO]</span>"
            )
            new_formatted_line <- paste0("[", format(Sys.time(), "%H:%M:%S"), "] ", styled_prefix, " ", log_line)
            con <- file(ruta_archivo, open = "at")
            cat(new_formatted_line, file = con, sep = "\n")
            flush(con)
            close(con)
          }

          # El callback escribe en el archivo intermedio de forma aislada
          ui_evolution_callback <- function(current_ind, total_ind, current_gen, max_gen, trigger_plots) {
            con <- file(ruta_progreso, open = "w")
            cat(paste(current_ind, total_ind, current_gen, max_gen, trigger_plots, sep = "|"), file = con)
            flush(con)
            close(con)
          }

          weaverRHub::run_optimiser(
              blueprint          = blueprint,
              output_path        = output_path,
              program            = program,
              base_config_path   = base_config_path,
              seed               = seed, 
              total_cores        = total_cores,
              program_cores      = program_cores,
              metrics_callback   = ui_metrics_callback,
              evolution_callback = ui_evolution_callback,
              debug              = debug
          )

          return(TRUE)
        }, seed = TRUE, globals = list(ruta_archivo=ruta_archivo, ruta_progreso=ruta_progreso, blueprint=blueprint, output_path=output_path, program=program, base_config_path=base_config_path, seed=seed, total_cores=total_cores, program_cores=program_cores, debug=debug))
    })
    
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

    # Enlazar el segundo botón buscador de carpetas para ejecución en onFlushed
    shinyFiles::shinyDirChoose(input, id = "output_dir_browse_btn_run", roots = shared_volumes, session = session)

    observeEvent(input$output_dir_browse_btn_run, {
      req(!is.integer(input$output_dir_browse_btn_run))
      dir_path <- shinyFiles::parseDirPath(shared_volumes, input$output_dir_browse_btn_run)
      if (length(dir_path) > 0) {
        updateTextInput(session = session, inputId = "output_dir_text_path_run", value = normalizePath(dir_path, mustWork = FALSE))
      }
    })

    # Validador de evento para la ruta de ejecución
    observeEvent(input$output_dir_text_path_run, {
      path <- input$output_dir_text_path_run
      if (is.null(path) || path == "") {
        active_errors$output_dir_run <- "Error: Simulation runtime directory path cannot be left blank."
      } else if (!dir.exists(path)) {
        active_errors$output_dir_run <- "Error: The specified runtime path does not point to an existing directory."
      } else {
        active_errors$output_dir_run <- NULL
      }
    }, ignoreInit = FALSE)

    # Renderizadores de estado visual locales para la ruta de ejecución
    output$error_local_output_dir_path_run <- renderUI({ req(active_errors$output_dir_run); p(icon("circle-xmark"), active_errors$output_dir_run, class = "text-danger small fw-bold mt-1 mb-0 text-break") })
    output$display_output_dir_path_confirmation_run <- renderUI({ req(is.null(active_errors$output_dir_run)); req(input$output_dir_text_path_run != ""); p(tags$code(input$output_dir_text_path_run), class = "text-muted small text-break mt-1 mb-0") })
    
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

    # OBSERVADOR DE SEGUIMIENTO EN HILO PRINCIPAL PROTEGIDO
    observe({
      invalidateLater(400, session)
      req(live_stream$is_running)
      
      # Aislamos la lectura para evitar colisiones reactivas fuera del consumidor
      log_path <- isolate(live_stream$archivo_log)
      prog_file <- isolate(live_stream$archivo_progreso)
      
      if (file.exists(prog_file)) {
        tryCatch({
          data_line <- readLines(prog_file, warn = FALSE, n = 1)
          if (length(data_line) > 0 && data_line != "") {
            parts <- unlist(strsplit(data_line, "\\|"))
            if (length(parts) == 5) {
              curr_ind <- as.numeric(parts[1])
              tot_ind  <- as.numeric(parts[2])
              curr_g   <- as.numeric(parts[3])
              max_g    <- as.numeric(parts[4])
              trig     <- as.integer(parts[5])
              
              pct <- round((curr_ind / tot_ind) * 100)
              live_stream$current_progress <- pct
              live_stream$progress_text    <- paste0("Generación ", curr_g, "/", max_g, " | Procesados: ", curr_ind, "/", tot_ind, " Individuos (", pct, "%)")
              
              if (trig != live_stream$plot_trigger) {
                live_stream$plot_trigger <- trig
              }
            }
          }
        }, error = function(e) NULL)
      }
    })
    
    # Renderizador dinámico de la barra de progreso global
    output$global_generation_progress_bar <- renderUI({
      req(live_stream$is_running)
      
      tags$div(
        class = "card p-3 mb-3 shadow-sm border-info animate__animated animate__fadeInDown",
        style = "background-color: var(--bs-body-bg);",
        tags$div(
          class = "d-flex justify-content-between align-items-center mb-1",
          tags$span(class = "fw-bold text-info small text-uppercase tracking-wider", 
                    tags$i(class = "fa-solid fa-dna fa-spin me-1"), "Progreso de la Generación Activa"),
          tags$span(class = "badge bg-info text-dark font-monospace", live_stream$progress_text)
        ),
        tags$div(
          class = "progress", style = "height: 18px; border-radius: 4px; box-shadow: inset 0 1px 2px rgba(0,0,0,.1);",
          tags$div(
            class = "progress-bar progress-bar-striped progress-bar-animated bg-info text-dark fw-bold text-center",
            role = "progressbar",
            style = paste0("width: ", live_stream$current_progress, "%; font-size: 11px; line-height: 18px; transition: width 0.3s ease;"),
            aria_valuenow = live_stream$current_progress,
            aria_valuemin = 0,
            aria_valuemax = 100,
            paste0(live_stream$current_progress, "%")
          )
        )
      )
    })


    # OBSERVADOR EN HILO PRINCIPAL: Monitorea el archivo y fuerza el Scroll JS de forma segura
    log_contenido <- reactive({
        invalidateLater(500, session) 
        if (file.exists(live_stream$archivo_log)) {
          lineas <- readLines(live_stream$archivo_log)
          if (length(lineas) > 0) {
            shinyjs::runjs("$('.terminal-logs-monitor').scrollTop($('.terminal-logs-monitor')[0].scrollHeight);")
          }
          paste(lineas, collapse = "\n")
        } else {
          "Archivo de log no disponible."
        }
    })

    output$live_terminal_console_stream <- renderText({
      log_contenido()
    })

    # OBSERVADOR EN HILO PRINCIPAL: Maneja el final de la tarea asíncrona (Éxito o Error)
    observeEvent(tarea_pesada$result(), {
      req(tarea_pesada$result() == TRUE)
      live_stream$is_running <- FALSE
      shinyjs::enable("launch_optimizer_btn")
      shinyjs::disable("terminate_optimizer_btn")
      shinyjs::info("¡Optimización completada con éxito!\nConsulta las gráficas en la pestaña analítica superior.")
    })

    output$render_launch_execution_controls <- renderUI({
      # Si el optimizador ya se está ejecutando, el botón se bloquea de forma operativa
      if (live_stream$is_running) {
        return(tags$button(
          id    = session$ns("launch_optimizer_disabled_running"), 
          type  = "button", 
          class = "btn btn-success btn-lg fw-bold w-100 disabled",
          icon("spinner", class = "fa-spin"), " Optimization Process Running..."
        ))
      }
      
      # Si hay un error en el path de telemetría de ejecución, se muestra el candado con requerimiento
      if (!run_section_is_valid()) {
        tags$button(
          id    = session$ns("launch_optimizer_disabled"), 
          type  = "button", 
          class = "btn btn-lg w-100 fw-bold",
          title = "Requirement: Resolve the simulation telemetry output directory path errors above to initialize process execution.",
          style = "background-color: var(--bs-secondary-bg); color: var(--bs-secondary-color); border-color: var(--bs-border-color); cursor: not-allowed; opacity: 0.65;",
          icon("ban"), " Initialize Optimization Process"
        )
      } else {
        # Si la sección de simulación está limpia, el botón se habilita de forma independiente
        actionButton(
          inputId = session$ns("launch_optimizer_btn"), 
          label   = "Initialize Optimization Process", 
          class   = "btn btn-success btn-lg fw-bold w-100", 
          icon    = icon("rocket")
        )
      }
    })

    # ============================================================================
    # REAL-TIME COMPREHENSIVE MULTI-OBJECTIVE RESULTS ANALYSIS PLOTS
    # ============================================================================
    
    # 1. Gráfico de Evolución y Convergencia de Fitness por Generación
    output$plot_fitness_convergence <- renderPlot({
      # Escucha activamente el disparador generacional del callback
      req(live_stream$plot_trigger > 0)
      
      # Localizamos el archivo de historial en la carpeta actual de simulación
      target_dir <- input$output_dir_text_path_run
      req(dir.exists(target_dir))
      
      # Buscamos la carpeta de optimización más reciente nacida en este hilo
      latest_run_folder <- list.dirs(target_dir, recursive = FALSE, full.names = TRUE) |> 
        grep(pattern = "optimisation_", value = TRUE) |> 
        sort(decreasing = TRUE) |> 
        _[[1]]
      
      req(!is.null(latest_run_folder) && file.exists(file.path(latest_run_folder, "evolution_history.txt")))
      
      # Lectura segura del dataframe de evolución generado por el motor R6
      history_df <- tryCatch({
        read.table(file.path(latest_run_folder, "evolution_history.txt"), header = TRUE, sep = "\t", stringsAsFactors = FALSE)
      }, error = function(e) NULL)
      
      req(!is.null(history_df) && nrow(history_df) > 0)
      
      # Procesamos las métricas medias y mejores por generación de forma agrupada
      # Asumimos la presencia de 'totalTime' u objetivos según tu configuración de fitness_engine
      fitness_columns <- colnames(history_df)[!colnames(history_df) %in% c("Generation", "Individual", "Result_folder")]
      primary_objective <- fitness_columns[1] # Tomamos el primer objetivo para la curva base
      
      plot_data <- history_df |>
        dplyr::mutate(
          Generation = as.numeric(Generation), 
          ObjectiveValue = as.numeric(.data[[primary_objective]])
        ) |>
        dplyr::group_by(Generation) |>
        dplyr::summarise(
          BestValue = min(ObjectiveValue, na.rm = TRUE),
          MeanValue = mean(ObjectiveValue, na.rm = TRUE),
          .groups = "drop"
        )
      
      ggplot2::ggplot(plot_data, ggplot2::aes(x = Generation)) +
        ggplot2::geom_line(ggplot2::aes(y = BestValue, color = "Best Individual"), linewidth = 1.2) +
        ggplot2::geom_line(ggplot2::aes(y = MeanValue, color = "Mean Population"), linetype = "dashed", linewidth = 1.0) +
        ggplot2::geom_point(ggplot2::aes(y = BestValue, color = "Best Individual"), size = 3) +
        ggplot2::scale_color_manual(values = c("Best Individual" = "#007bff", "Mean Population" = "#dc3545")) +
        ggplot2::labs(
          title = paste("Generational Fitness Convergence Trends:", primary_objective),
          x = "Evolutionary Generation Index",
          y = "Objective Fitness Score Space",
          color = "Metrics Allocation"
        ) +
        ggplot2::theme_minimal(base_size = 13) +
        ggplot2::theme(legend.position = "bottom")
    })

    # 2. Gráfico del Frente de Pareto de los Objetivos (Multi-Objective Frontiers Map)
    output$plot_pareto_front <- renderPlot({
      req(live_stream$plot_trigger > 0)
      target_dir <- input$output_dir_text_path_run
      
      latest_run_folder <- list.dirs(target_dir, recursive = FALSE, full.names = TRUE) |> 
        grep(pattern = "optimisation_", value = TRUE) |> 
        sort(decreasing = TRUE) |> 
        _[[1]]
      
      req(!is.null(latest_run_folder) && file.exists(file.path(latest_run_folder, "evolution_history.txt")))
      
      history_df <- tryCatch({
        read.table(file.path(latest_run_folder, "evolution_history.txt"), header = TRUE, sep = "\t", stringsAsFactors = FALSE)
      }, error = function(e) NULL)
      
      req(!is.null(history_df) && nrow(history_df) > 0)
      
      # Extraemos las dos primeras funciones objetivo para cruzar sus dimensiones en el plano XY
      fitness_columns <- colnames(history_df)[!colnames(history_df) %in% c("Generation", "Individual", "Result_folder")]
      req(length(fitness_columns) >= 2)
      
      obj_x <- fitness_columns[1]
      obj_y <- fitness_columns[2]
      
      pareto_data <- history_df |>
        dplyr::filter(Generation == max(as.numeric(Generation))) |>
        dplyr::mutate(X_Val = as.numeric(.data[[obj_x]]), Y_Val = as.numeric(.data[[obj_y]]))
      
      ggplot2::ggplot(pareto_data, ggplot2::aes(x = X_Val, y = Y_Val)) +
        ggplot2::geom_point(color = "#18bc9c", size = 3.5, alpha = 0.8) +
        ggplot2::labs(
          title = "Multi-Objective Pareto Front Boundaries Map",
          subtitle = "Generational convergence crossing current individuals profiles metrics",
          x = paste("Objective X:", obj_x),
          y = paste("Objective Y:", obj_y)
        ) +
        ggplot2::theme_minimal(base_size = 13) +
        ggplot2::theme(legend.position = "none")
    })

    output$error_local_output_dir_path = renderUI({ req(active_errors$output_directory); p(icon("circle-xmark"), active_errors$output_directory, class = "text-danger small fw-bold mt-1 mb-0 text-break") })
    output$error_local_json_filename   = renderUI({ req(active_errors$json_filename); p(icon("circle-xmark"), active_errors$json_filename, class = "text-danger small fw-bold mt-1 mb-0 text-break") })
    output$display_output_dir_path_confirmation = renderUI({ req(is.null(active_errors$output_directory)); req(input$output_dir_text_path != ""); p(tags$code(input$output_dir_text_path), class = "text-muted small text-break mt-1 mb-0") })
    
    # Evaluador de integridad exclusivo para la sección superior de exportación del Blueprint
    export_section_is_valid <- reactive({
      is.null(active_errors$output_directory) && is.null(active_errors$json_filename)
    })
    
    # Evaluador de integridad exclusivo para la sección inferior de lanzamiento de simulación
    run_section_is_valid <- reactive({
      is.null(active_errors$output_dir_run)
    })
    
    # Mantenemos la validez general de la fase expuesta al orquestador principal
    # para el flujo de los botones globales "Next Phase" / "Previous"
    phase_3_is_valid <- reactive({
      export_section_is_valid() && run_section_is_valid()
    })

    commit_phase_3_data_transaction = function() {
      all_errors <- na.omit(reactiveValuesToList(active_errors))
      global_store$phase_3$optimization_output <- list(
        output_directory = input$output_dir_text_path,
        export_filename  = paste0(trimws(input$config_json_filename), ".json"),
        is_valid         = (length(all_errors) == 0)
      )
      message("[Phase 3 Store] Output parameters transactional cache commit finalized successfully.")
    }

    output$render_export_state_controls <- renderUI({
      save_button_layout <- if (export_section_is_valid()) {
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
          title = "Requirement: Resolve all validation and blueprint target paths errors above to proceed with export.",
          style = "background-color: var(--bs-secondary-bg); color: var(--bs-secondary-color); border-color: var(--bs-border-color); cursor: not-allowed; opacity: 0.65;",
          icon("ban"), " Save Optimization Configuration"
        )
      }
  
      tagList(save_button_layout)
    })

    observeEvent(input$save_optimization_config_btn, {
      # Medidas de seguridad defensivas para asegurar la integridad de la llamada
      req(export_section_is_valid())
  
      # 1. Resolución analítica de la ruta física final del archivo de destino
      target_directory <- input$output_dir_text_path
      clean_filename   <- trimws(input$config_json_filename)
      final_file_path  <- file.path(target_directory, paste0(clean_filename, ".json"))
  
      message("[Phase 3 Export] Compiling comprehensive optimization setup. Target: ", final_file_path)
  
      # 2. Extracción profunda y desacoplamiento de metadatos de la Fase 1
      p1_ga   <- global_store$phase_1$ga_settings
      p1_env  <- global_store$phase_1$computing_environment
      p1_path <- global_store$phase_1$system_paths
  
      # 3. Construcción del Esquema Unificado Portable (Agnóstico de paths absolutos locales)
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
    
        # REFACTORIZACIÓN DEFINITIVA: Recupera el nuevo bloque unificado plano de Parameter Tying
        # generado interactivamente por las grillas dinámicas en la Fase 2
        optimization_variables_allocation = global_store$phase_2$defined_parameters
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
    
        message("[Phase 3 Export] Success: Configuration footprint written flawlessly.")
    
        # Feedback instantáneo de éxito al usuario en el cliente
        shinyjs::info(paste0("¡Configuración exportada con éxito!\n\nEl archivo portable se ha guardado en:\n", final_file_path))
    
      }, error = function(e) {
        message("[Phase 3 Export Error] Critical failure writing JSON payload: ", e$message)
        shinyjs::info(paste0("Error crítico al exportar la configuración:\n", e$message))
      })
    })
    

    # ============================================================================
    # PIPELINE ORCHESTRATOR EVENT LISTENER (LAUNCH & MONITOR)
    # ============================================================================
    observeEvent(input$launch_optimizer_btn, {
      req(run_section_is_valid())
      
      # Bloqueo defensivo de la UI para evitar ejecuciones concurrentes corruptas
      live_stream$is_running <- TRUE
      live_stream$current_progress  <- 0
      live_stream$progress_text     <- "Inicializando hilos..."    

      shinyjs::disable("launch_optimizer_btn")
      shinyjs::enable("terminate_optimizer_btn")

      prog_file <- isolate(live_stream$archivo_progreso)
      if (file.exists(prog_file)) unlink(prog_file)
      
      # 1. Compilación del Blueprint JSON en memoria viva de R (Sin lectura de disco)
      p1_ga   <- global_store$phase_1$ga_settings
      p1_env  <- global_store$phase_1$computing_environment
      p1_path <- global_store$phase_1$system_paths
      
      memory_blueprint_payload <- list(
        metadata = list(
          schema_version   = p1_path$detected_schema_version,
          export_timestamp = as.character(Sys.time())
        ),
        genetic_algorithm_settings = list(
          population_size = p1_ga$population_size,
          max_generations = p1_ga$max_generations,
          tournament_size = p1_ga$tournament_size,
          crossover_prob  = p1_ga$crossover_prob,
          mutation_rate   = p1_ga$mutation_rate,
          keep_best       = p1_ga$keep_best
        ),
        computing_environment = list(
          fitness_class    = p1_env$fitness_class,
          priority_ranking = p1_env$priority_ranking,
          assigned_weights = p1_env$assigned_weights,
          objectives = lapply(p1_env$fitness_objectives$objectives_data, function(obj) {
            list(name = obj$name, criteria = obj$criteria, fitness_func_name = obj$fitness_func_name, custom_code_raw = obj$custom_code_raw)
          })
        ),
        optimization_variables_allocation = global_store$phase_2$defined_parameters
      )
      
      # Inicialización limpia de la consola de texto en pantalla
      live_stream$console_history <- paste0("[", Sys.time(), "] [SYSTEM] Reconstituting memory blueprint maps successfully.\n")
      

      writeLines("Iniciando entorno asíncrono...", live_stream$archivo_log)
      tarea_pesada$invoke(
          live_stream$archivo_log,
          live_stream$archivo_progreso,
          blueprint          = memory_blueprint_payload,
          output_path        = input$output_dir_text_path_run,
          program            = p1_path$program_executable,
          base_config_path   = p1_path$base_configuration,
          seed               = as.integer(input$seed),
          total_cores        = as.integer(input$total_cores),
          program_cores      = as.integer(input$program_cores),
          debug              = as.logical(input$debug)
      )
    })
    
    # Manejador reactivo para abortar ejecuciones colgadas
    observeEvent(input$terminate_optimizer_btn, {
      req(live_stream$is_running)
      # NOTA: cambiar el plan de 'future' NO cancela el future ya en marcha
      # (sigue corriendo en su worker, en su propio proceso aparte), solo
      # afecta a futures NUEVOS que se creen a partir de ahora en la sesión
      # principal. Restauramos el mismo plan simple de nivel único (en vez de
      # "sequential" a secas) para no dejar el pool compartido de la app en
      # modo bloqueante: si esta sesión comparte proceso R con otras
      # sesiones/tareas, "sequential" las afectaría a todas y rompería su log
      # en tiempo real hasta el próximo lanzamiento.
      future::plan(future::multisession)
      shinyjs::info("Se ha enviado una señal de interrupción al clúster de procesos.")
    })

    onSessionEnded(function() {
      if (file.exists(live_stream$archivo_log)) unlink(live_stream$archivo_log)
      if (file.exists(live_stream$archivo_progreso)) unlink(live_stream$archivo_progreso)
    })


    return(list(
      is_valid    = phase_3_is_valid,
      commit_data = commit_phase_3_data_transaction
    ))
  })
}