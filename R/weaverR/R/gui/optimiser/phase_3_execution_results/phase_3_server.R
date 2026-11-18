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
      archivo_run_path = tempfile(fileext = ".run-path"),
      archivo_cancelacion = tempfile(fileext = ".stop"),
      archivo_sesion_cerrada = tempfile(fileext = ".session-closed"),
      directorio_eventos = tempfile(pattern = "weaver_progress_events_"),
      run_path         = NULL,
      objective_names  = character(),
      objective_criteria = list(),
      current_progress  = 0,
      progress_text     = "Waiting for the start..."
    )

    # REFACTORIZACIÓN DEFINITIVA DE LA TAREA ASÍNCRONA
    tarea_pesada <- ExtendedTask$new(function(ruta_archivo, ruta_progreso, ruta_run_path, ruta_eventos, ruta_cancelacion, ruta_sesion_cerrada, optimization_config, output_path, program, base_config_path, seed, total_cores, program_cores, debug) {
        engine_runner <- get("run_optimiser_controlled", envir = asNamespace("weaverR"), inherits = FALSE)

        if (!inherits(future::plan(), "multisession")) {
          future::plan(future::multisession)
        }

        future::future({
          on.exit({
            if (file.exists(ruta_sesion_cerrada)) {
              unlink(c(ruta_archivo, ruta_progreso, ruta_run_path, ruta_cancelacion, ruta_sesion_cerrada))
              if (dir.exists(ruta_eventos)) unlink(ruta_eventos, recursive = TRUE, force = TRUE)
            }
          }, add = TRUE)
      
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

          ui_output_path_callback <- function(run_output_path) {
            writeLines(run_output_path, ruta_run_path, useBytes = TRUE)
          }

          # Workers never overwrite shared progress state: each completion writes
          # its own marker so parallel evaluations cannot lose increments.
          ui_individual_callback <- function(current_ind, total_ind, current_gen, max_gen) {
            generation_dir <- file.path(ruta_eventos, sprintf("generation-%06d", current_gen))
            dir.create(generation_dir, recursive = TRUE, showWarnings = FALSE)
            marker_path <- file.path(generation_dir, sprintf("individual-%06d.done", current_ind))
            file.create(marker_path)
            invisible(NULL)
          }

          task_result <- tryCatch({
            run_result <- engine_runner(
              optimization_config = optimization_config,
              output_path        = output_path,
              program            = program,
              base_config_path   = base_config_path,
              seed               = seed, 
              total_cores        = total_cores,
              program_cores      = program_cores,
              metrics_callback   = ui_metrics_callback,
              evolution_callback = ui_evolution_callback,
              debug              = debug,
              output_path_callback = ui_output_path_callback,
              individual_callback = ui_individual_callback,
              cancellation_file = ruta_cancelacion,
              use_server_progress = FALSE
            )
            list(ok = TRUE, cancelled = identical(run_result$status, "cancelled"), run = run_result)
          }, error = function(e) list(ok = FALSE, error = conditionMessage(e)))

          return(task_result)
        }, seed = TRUE, globals = list(engine_runner=engine_runner, ruta_archivo=ruta_archivo, ruta_progreso=ruta_progreso, ruta_run_path=ruta_run_path, ruta_eventos=ruta_eventos, ruta_cancelacion=ruta_cancelacion, ruta_sesion_cerrada=ruta_sesion_cerrada, optimization_config=optimization_config, output_path=output_path, program=program, base_config_path=base_config_path, seed=seed, total_cores=total_cores, program_cores=program_cores, debug=debug))
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
    
    output$display_output_dir_path_confirmation_run <- renderUI({ 
        req(is.null(active_errors$output_dir_run), nzchar(input$output_dir_text_path_run))
        absolute_path <- normalizePath(path.expand(input$output_dir_text_path_run), winslash = "/", mustWork = TRUE)
        p(tags$code(absolute_path), class = "text-muted small text-break mt-1 mb-0") 
    })
    
    output$display_output_dir_path_confirmation <- renderUI({ 
        req(is.null(active_errors$output_directory), nzchar(input$output_dir_text_path))
        absolute_path <- normalizePath(path.expand(input$output_dir_text_path), winslash = "/", mustWork = TRUE)
        p(tags$code(absolute_path), class = "text-muted small text-break mt-1 mb-0") 
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

    # OBSERVADOR DE SEGUIMIENTO EN HILO PRINCIPAL PROTEGIDO
    observe({
      invalidateLater(400, session)
      
      # Aislamos la lectura para evitar colisiones reactivas fuera del consumidor
      log_path <- isolate(live_stream$archivo_log)
      prog_file <- isolate(live_stream$archivo_progreso)
      run_path_file <- isolate(live_stream$archivo_run_path)
      if (file.exists(run_path_file)) {
        path <- readLines(run_path_file, warn = FALSE, n = 1)
        if (length(path) && nzchar(path) && !identical(live_stream$run_path, path)) live_stream$run_path <- path
      }
      
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
              
              if (is.finite(tot_ind) && tot_ind > 0 && is.finite(curr_g) && curr_g > 0) {
                generation_dir <- file.path(isolate(live_stream$directorio_eventos), sprintf("generation-%06d", as.integer(curr_g)))
                completed_individuals <- if (dir.exists(generation_dir)) {
                  length(list.files(generation_dir, pattern = "^individual-[0-9]{6}\\.done$"))
                } else {
                  0L
                }
                completed_individuals <- min(as.integer(tot_ind), completed_individuals)
                generation_pct <- max(0, min(100, round(100 * completed_individuals / tot_ind)))
                overall_pct <- if (is.finite(max_g) && max_g > 0) {
                  max(0, min(100, round(100 * ((curr_g - 1) + generation_pct / 100) / max_g)))
                } else {
                  generation_pct
                }
                live_stream$current_gen <- curr_g
                live_stream$current_progress <- overall_pct
                live_stream$progress_text <- paste0(
                  "Generation ", curr_g, "/", max_g,
                  " | Completed individuals: ", completed_individuals, "/", tot_ind, " (", generation_pct, "%)"
                )
              }
              
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
      tags$div(
        class = "card p-3 mb-3 shadow-sm border-info animate__animated animate__fadeInDown",
        style = "background-color: var(--bs-body-bg);",
        tags$div(
          class = "d-flex justify-content-between align-items-center mb-1",
          tags$span(class = "fw-bold text-info small text-uppercase tracking-wider", 
                    tags$i(class = paste("fa-solid fa-dna me-1", if (live_stream$is_running) "fa-spin")), "Optimisation Progress"),
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
          "Log file not available."
        }
    })

    output$live_terminal_console_stream <- renderText({
      log_contenido()
    })

    # OBSERVADOR EN HILO PRINCIPAL: Maneja el final de la tarea asíncrona (Éxito o Error)
    observeEvent(tarea_pesada$result(), {
      result <- tarea_pesada$result()
      live_stream$is_running <- FALSE
      shinyjs::enable("launch_optimizer_btn")
      shinyjs::disable("terminate_optimizer_btn")
      if (isTRUE(result$ok)) {
        if (isTRUE(result$cancelled)) {
          live_stream$progress_text <- paste0("Parada solicitada | ", result$run$completed_generations, " generaciones completas")
          shinyjs::info("Optimización detenida. Se han conservado únicamente las generaciones completadas.")
        } else {
          live_stream$current_progress <- 100
          live_stream$progress_text <- paste0("Optimización completada | Generación ", live_stream$current_gen)
          shinyjs::info("¡Optimización completada con éxito! Consulta las gráficas en la pestaña de análisis.")
        }
      } else {
        live_stream$progress_text <- "Optimización finalizada con errores. Consulta el log."
        shinyjs::info(paste0("La optimización ha fallado: ", result$error))
      }
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
    # GENERATION-SCOPED INTERACTIVE ANALYSIS
    # ============================================================================

    completed_generation_files <- reactiveVal(character())

    observe({
      invalidateLater(800, session)
      run_folder <- live_stream$run_path
      if (is.null(run_folder)) {
        if (length(completed_generation_files())) completed_generation_files(character())
        return()
      }
      snapshot_dir <- file.path(run_folder, "evolution_generations")
      snapshots <- if (dir.exists(snapshot_dir)) {
        list.files(snapshot_dir, pattern = "^generation-[0-9]{6}\\.tsv$", full.names = TRUE)
      } else character()
      snapshots <- snapshots[vapply(snapshots, function(path) {
        generation <- sub("^generation-([0-9]{6})\\.tsv$", "\\1", basename(path))
        file.exists(file.path(run_folder, "fitness_graphs", paste0("generation-", generation), "generation.complete"))
      }, logical(1))]
      if (!identical(snapshots, completed_generation_files())) completed_generation_files(snapshots)
    })

    observe({
      files <- completed_generation_files()
      generations <- as.integer(sub("^generation-([0-9]{6})\\.tsv$", "\\1", basename(files)))
      choices <- if (length(generations)) {
        stats::setNames(as.character(generations), paste("Generation", generations, "(complete)"))
      } else {
        character()
      }
      current <- isolate(input$analysis_generation)
      selected <- if (length(generations) && !is.null(current) && current %in% as.character(generations)) current else if (length(generations)) as.character(max(generations)) else character()
      updateSelectInput(session, "analysis_generation", choices = choices, selected = selected)
    })

    output$analysis_generation_status <- renderUI({
      files <- completed_generation_files()
      if (!length(files)) {
        return(div(
          class = "alert alert-info py-2 small",
          role = "status",
          icon("hourglass-half", class = "me-2"),
          strong("Looking forward to the first complete generation."),
          " The graphs will be available as soon as their assessment is complete."
        ))
      }

      latest_generation <- max(as.integer(sub("^generation-([0-9]{6})\\.tsv$", "\\1", basename(files))))
      div(
        class = "alert alert-light border py-2 small",
        role = "status",
        icon("chart-line", class = "me-2"),
        paste0(length(files), " full generation(s). Last available: ", latest_generation, ".")
      )
    })

    read_generation_snapshot <- function(path) {
      tryCatch(read.delim(path, check.names = FALSE, stringsAsFactors = FALSE), error = function(e) NULL)
    }

    analysis_history <- reactive({
      files <- completed_generation_files()
      req(length(files) > 0)
      frames <- lapply(files, read_generation_snapshot)
      frames <- Filter(function(frame) !is.null(frame) && nrow(frame) > 0, frames)
      req(length(frames) > 0)
      dplyr::bind_rows(frames) |>
        dplyr::mutate(Generation = as.integer(Generation))
    })

    available_analysis_objectives <- reactive({
      history <- analysis_history()
      configured <- intersect(live_stream$objective_names, names(history))
      if ("Fitness" %in% names(history) && !"Fitness" %in% configured) configured <- c(configured, "Fitness")
      configured[vapply(configured, function(name) any(is.finite(suppressWarnings(as.numeric(history[[name]])))), logical(1))]
    })

    observe({
      objectives <- available_analysis_objectives()
      current <- isolate(input$analysis_objective)
      selected <- if (!is.null(current) && current %in% objectives) current else if (length(objectives)) objectives[[1]] else character()
      updateSelectInput(session, "analysis_objective", choices = objectives, selected = selected)
      current_x <- isolate(input$analysis_objective_x)
      current_y <- isolate(input$analysis_objective_y)
      selected_x <- if (!is.null(current_x) && current_x %in% objectives) current_x else if (length(objectives)) objectives[[1]] else character()
      selected_y <- if (!is.null(current_y) && current_y %in% objectives) current_y else if (length(objectives) > 1L) objectives[[2]] else selected_x
      updateSelectInput(session, "analysis_objective_x", choices = objectives, selected = selected_x)
      updateSelectInput(session, "analysis_objective_y", choices = objectives, selected = selected_y)
    })

    generation_wait_message <- function() {
      div(class = "alert alert-info", role = "status", icon("hourglass-half", class = "me-2"),
        "Waiting for the first generation to complete so that the graph can be generated.")
    }

    output$fitness_convergence_panel <- renderUI({
      if (!length(completed_generation_files())) return(generation_wait_message())
      plotly::plotlyOutput(ns("plot_fitness_convergence"), height = "420px")
    })

    output$fitness_distribution_panel <- renderUI({
      if (!length(completed_generation_files())) return(generation_wait_message())
      plotly::plotlyOutput(ns("plot_fitness_distribution"), height = "420px")
    })

    output$objective_space_panel <- renderUI({
      if (!length(completed_generation_files())) {
        return(generation_wait_message())
      }

      objectives <- available_analysis_objectives()
      if (length(objectives) < 2L) {
        return(div(class = "alert alert-warning", role = "status", icon("triangle-exclamation", class = "me-2"),
          "The objective space requires at least two objectives."))
      }

      x_objective <- input$analysis_objective_x
      y_objective <- input$analysis_objective_y
      if (is.null(x_objective) || is.null(y_objective)) {
        return(div(class = "alert alert-info", role = "status", icon("circle-info", class = "me-2"),
          "Selecciona los objetivos para los ejes X e Y."))
      }
      if (identical(x_objective, y_objective)) {
        return(div(class = "alert alert-warning", role = "alert", icon("triangle-exclamation", class = "me-2"),
          strong("Selecciona dos objetivos distintos."), " El objetivo de X y el de Y no pueden ser el mismo."))
      }

      plotly::plotlyOutput(ns("plot_pareto_front"), height = "480px")
    })

    selected_generation_data <- reactive({
      files <- completed_generation_files()
      req(length(files) > 0)
      generation <- suppressWarnings(as.integer(input$analysis_generation))
      available <- as.integer(sub("^generation-([0-9]{6})\\.tsv$", "\\1", basename(files)))
      if (length(generation) != 1L || is.na(generation) || !generation %in% available) generation <- max(available)
      data <- read_generation_snapshot(files[[match(generation, available)]])
      req(!is.null(data), nrow(data) > 0)
      data$Generation <- as.integer(data$Generation)
      data
    })

    objective_criterion <- function(objective) {
      criterion <- live_stream$objective_criteria[[objective]]
      if (is.null(criterion) && identical(objective, "Fitness")) criterion <- "MIN"
      if (is.null(criterion) || !toupper(criterion) %in% c("MIN", "MAX")) "MIN" else toupper(criterion)
    }

    output$plot_fitness_convergence <- plotly::renderPlotly({
      history <- analysis_history()
      objective <- input$analysis_objective
      req(length(objective) == 1L, objective %in% names(history))
      criterion <- objective_criterion(objective)
      summary <- summarise_generation_objective(history, objective, criterion)
      req(!is.null(summary), nrow(summary) > 0)
      p <- plotly::plot_ly(data = summary, x = ~Generation)
      if (nrow(summary) > 1L) {
        p <- plotly::add_ribbons(
          p, ymin = ~Q1, ymax = ~Q3, name = "Interquartile range",
          fillcolor = "rgba(76,120,168,0.16)", line = list(color = "transparent"),
          hoverinfo = "skip", showlegend = TRUE
        )
      }
      p <- plotly::add_trace(p, y = ~Mean, type = "scatter", mode = "lines+markers", name = "Population mean",
        line = list(color = "#e45756", dash = "dash"), marker = list(size = 6),
        text = ~paste("Generation:", Generation, "<br>Mean:", signif(Mean, 17)), hoverinfo = "text")
      p <- plotly::add_trace(p, y = ~BestInGeneration, type = "scatter", mode = "lines+markers", name = "Best in generation",
        line = list(color = "#72b7b2", width = 2), marker = list(size = 6),
        text = ~paste("Generation:", Generation, "<br>Best:", signif(BestInGeneration, 17)), hoverinfo = "text")
      p <- plotly::add_trace(p, y = ~BestSoFar, type = "scatter", mode = "lines+markers", name = "Best so far",
        line = list(color = "#1f4e79", width = 3), marker = list(size = 8),
        text = ~paste("Generation:", Generation, "<br>Best so far:", signif(BestSoFar, 17)), hoverinfo = "text")
      plotly::layout(p,
        title = list(text = paste("Evolution of", objective, "— goal:", tolower(criterion))),
        xaxis = list(title = "Generation"), yaxis = list(title = "Raw objective value"),
        legend = list(orientation = "h", x = 0, y = -0.2), hovermode = "x unified"
      )
    })

    output$plot_fitness_distribution <- plotly::renderPlotly({
      data <- selected_generation_data()
      objective <- input$analysis_objective
      req(length(objective) == 1L, objective %in% names(data))
      data$ObjectiveValue <- suppressWarnings(as.numeric(data[[objective]]))
      data <- data[is.finite(data$ObjectiveValue), , drop = FALSE]
      req(nrow(data) > 0)
      data$IndividualLabel <- paste("Individual", data$Individual)
      data$Population <- "Population"
      plotly::plot_ly(
        data = data, x = ~Population, y = ~ObjectiveValue, type = "box", boxpoints = "all",
        jitter = 0.35, pointpos = 0, name = objective,
        text = ~paste(IndividualLabel, "<br>Value:", signif(ObjectiveValue, 17)), hoverinfo = "text"
      ) |>
        plotly::layout(
          title = list(text = paste("Generation", unique(data$Generation), "population distribution —", objective_criterion(objective))),
          xaxis = list(title = ""), yaxis = list(title = "Raw objective value")
        )
    })

    output$plot_pareto_front <- plotly::renderPlotly({
      data <- selected_generation_data()
      objectives <- available_analysis_objectives()
      x_objective <- input$analysis_objective_x
      y_objective <- input$analysis_objective_y
      req(length(objectives) >= 2L, x_objective %in% objectives, y_objective %in% objectives, x_objective != y_objective)
      data[[x_objective]] <- suppressWarnings(as.numeric(data[[x_objective]]))
      data[[y_objective]] <- suppressWarnings(as.numeric(data[[y_objective]]))
      data <- data[is.finite(data[[x_objective]]) & is.finite(data[[y_objective]]), , drop = FALSE]
      req(nrow(data) > 0)
      criteria <- vapply(c(x_objective, y_objective), objective_criterion, character(1))
      data$Nondominated <- objective_nondominated(data, c(x_objective, y_objective), criteria)
      data$SelectedByEngine <- if ("SelectedByFitnessEngine" %in% names(data)) as.logical(data$SelectedByFitnessEngine) else FALSE
      data$IndividualLabel <- paste("Individual", data$Individual)
      p <- plotly::plot_ly()
      dominated <- data[!data$Nondominated & !data$SelectedByEngine, , drop = FALSE]
      frontier <- data[data$Nondominated & !data$SelectedByEngine, , drop = FALSE]
      selected <- data[data$SelectedByEngine, , drop = FALSE]
      if (nrow(dominated)) {
        p <- plotly::add_markers(p, x = dominated[[x_objective]], y = dominated[[y_objective]],
          text = paste(dominated$IndividualLabel, "<br>", x_objective, ":", signif(dominated[[x_objective]], 17), "<br>", y_objective, ":", signif(dominated[[y_objective]], 17)),
          hoverinfo = "text", name = "Dominated", marker = list(color = "#9aa4af", size = 9, symbol = "circle"))
      }
      if (nrow(frontier)) {
        p <- plotly::add_markers(p, x = frontier[[x_objective]], y = frontier[[y_objective]],
          text = paste(frontier$IndividualLabel, "<br>", x_objective, ":", signif(frontier[[x_objective]], 17), "<br>", y_objective, ":", signif(frontier[[y_objective]], 17)),
          hoverinfo = "text", name = "Non-dominated", marker = list(color = "#18a999", size = 10, symbol = "diamond"))
      }
      if (nrow(selected)) {
        p <- plotly::add_markers(p, x = selected[[x_objective]], y = selected[[y_objective]],
          text = paste(selected$IndividualLabel, "(selected by fitness engine)<br>", x_objective, ":", signif(selected[[x_objective]], 17), "<br>", y_objective, ":", signif(selected[[y_objective]], 17)),
          hoverinfo = "text", name = "Selected by engine", marker = list(color = "#f2a900", size = 15, symbol = "star"))
      }
      plotly::layout(p,
        title = list(text = paste("Generation", unique(data$Generation), "objective space")),
        xaxis = list(title = paste(x_objective, "(", criteria[[1]], ")")),
        yaxis = list(title = paste(y_objective, "(", criteria[[2]], ")")),
        legend = list(orientation = "h", x = 0, y = -0.2)
      )
    })
    output$error_local_output_dir_path = renderUI({ req(active_errors$output_directory); p(icon("circle-xmark"), active_errors$output_directory, class = "text-danger small fw-bold mt-1 mb-0 text-break") })
    output$error_local_json_filename   = renderUI({ req(active_errors$json_filename); p(icon("circle-xmark"), active_errors$json_filename, class = "text-danger small fw-bold mt-1 mb-0 text-break") })
    
    # Evaluador de integridad exclusivo para la sección superior de exportación del optimization template
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
          title = "Requirement: Resolve all validation and optimization template target path errors above to proceed with export.",
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
      optimization_template <- list(
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
        # Se exporta el optimization template completo de phase 2, incluidas las `cells`
        # de Preference/Profitability (especies e instares) y sus límites.
        optimization_variables_allocation = global_store$phase_2$defined_parameters
      )
  
      # 4. Escritura transaccional en el disco duro mediante codificación JSON estructurada
      tryCatch({
        jsonlite::write_json(
          x             = optimization_template, 
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
      live_stream$progress_text     <- "Starting threads..."    
      live_stream$run_path <- NULL
      completed_generation_files(character())

      shinyjs::disable("launch_optimizer_btn")
      shinyjs::enable("terminate_optimizer_btn")

      prog_file <- isolate(live_stream$archivo_progreso)
      if (file.exists(prog_file)) unlink(prog_file)
      if (file.exists(live_stream$archivo_run_path)) unlink(live_stream$archivo_run_path)
      if (file.exists(live_stream$archivo_cancelacion)) unlink(live_stream$archivo_cancelacion)
      if (file.exists(live_stream$archivo_sesion_cerrada)) unlink(live_stream$archivo_sesion_cerrada)
      if (dir.exists(live_stream$directorio_eventos)) unlink(live_stream$directorio_eventos, recursive = TRUE, force = TRUE)
      dir.create(live_stream$directorio_eventos, recursive = TRUE, showWarnings = FALSE)
      
      # 1. Compilación del optimization config JSON en memoria viva de R (Sin lectura de disco)
      p1_ga   <- global_store$phase_1$ga_settings
      p1_env  <- global_store$phase_1$computing_environment
      p1_path <- global_store$phase_1$system_paths
      configured_objectives <- p1_env$fitness_objectives$objectives_data
      live_stream$objective_names <- vapply(configured_objectives, function(obj) as.character(obj$name), character(1))
      live_stream$objective_criteria <- stats::setNames(
        vapply(configured_objectives, function(obj) as.character(obj$criteria), character(1)),
        live_stream$objective_names
      )
      
      optimization_config <- list(
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
        # Mantiene idéntico el payload en memoria al JSON exportado, incluidas
        # las celdas tróficas añadidas a Preference/Profitability.
        optimization_variables_allocation = global_store$phase_2$defined_parameters
      )
      
      # Inicialización limpia de la consola de texto en pantalla
      live_stream$console_history <- paste0("[", Sys.time(), "] [SYSTEM] Reconstituting optimization config successfully.\n")
      

      tarea_pesada$invoke(
          live_stream$archivo_log,
          live_stream$archivo_progreso,
          live_stream$archivo_run_path,
          live_stream$directorio_eventos,
          live_stream$archivo_cancelacion,
          live_stream$archivo_sesion_cerrada,
          optimization_config = optimization_config,
          output_path        = input$output_dir_text_path_run,
          program            = p1_path$program_executable,
          base_config_path   = p1_path$base_configuration,
          seed               = as.integer(input$seed),
          total_cores        = as.integer(input$total_cores),
          program_cores      = as.integer(input$program_cores),
          debug              = as.logical(input$debug)
      )
    })
    
    # Señal de cancelación cooperativa: cada worker la consulta y termina su
    # simulador actual. La generación en curso se descarta en bloque.
    observeEvent(input$terminate_optimizer_btn, {
      req(live_stream$is_running)
      request_optimiser_stop(isolate(live_stream$archivo_cancelacion))
      live_stream$progress_text <- "Solicitando parada segura..."
      shinyjs::disable("terminate_optimizer_btn")
      shinyjs::info("Parada solicitada. Se terminarán los simuladores en curso y se descartará la generación incompleta.")
    })

    onSessionEnded(function() {
      temp_paths <- shiny::isolate(list(
        log = live_stream$archivo_log,
        progress = live_stream$archivo_progreso,
        run_path = live_stream$archivo_run_path,
        cancellation = live_stream$archivo_cancelacion,
        session_closed = live_stream$archivo_sesion_cerrada,
        events = live_stream$directorio_eventos
      ))
      if (isTRUE(shiny::isolate(live_stream$is_running))) {
        # Keep the stop marker alive until the background worker has observed it.
        writeLines("closed", temp_paths$session_closed, useBytes = TRUE)
        try(request_optimiser_stop(temp_paths$cancellation), silent = TRUE)
      } else {
        unlink(c(temp_paths$log, temp_paths$progress, temp_paths$run_path, temp_paths$cancellation, temp_paths$session_closed))
        if (dir.exists(temp_paths$events)) unlink(temp_paths$events, recursive = TRUE, force = TRUE)
      }
    })


    return(list(
      is_valid    = phase_3_is_valid,
      commit_data = commit_phase_3_data_transaction
    ))
  })
}
