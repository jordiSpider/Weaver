library(shiny)
library(shinyjs)
library(shinyFiles)
library(ggplot2)
library(dplyr)

# Initialize shared internal reactive state
console_buffer <- reactiveVal("")
evolution_data_reactive <- reactiveVal(NULL)

# --- shinyFiles Registration on the Server for the new script directory ---
# Bind the 'script_export_dir' ID to the system's disk tree
shinyDirChoose(input, "script_export_dir", roots = volumes, session = session)

# Reactive capture 1: user's manual typing in the text box
observeEvent(input$script_export_dir_text, {
  # Force normalization of Windows/Linux path separators
  clean_path <- normalizePath(input$script_export_dir_text, mustWork = FALSE)
  # Space to store in reactive state if needed
}, ignoreInit = TRUE)

# Reactive capture 2: selection from the folder tree with the 'Browse Directory...' button
observeEvent(input$script_export_dir, {
  req(!is.integer(input$script_export_dir))
  dir_path <- parseDirPath(volumes, input$script_export_dir)
  if (length(dir_path) > 0) {
    selected_path <- normalizePath(dir_path, mustWork = FALSE)
    # Instantly sync the value on screen
    updateTextInput(session, "script_export_dir_text", value = selected_path)
  }
})

# =========================================================================
# CONTROL LAYER: REACTIVE VALIDATION OF STARTUP PREREQUISITES
# =========================================================================
script_parameters_status <- reactive({
  target_dir <- input$script_export_dir_text
  filename <- input$script_export_filename
  
  if (is.null(target_dir) || target_dir == "") {
    return(list(valid = FALSE, message = "Please specify a destination folder for the auxiliary configuration script."))
  }
  if (!dir.exists(target_dir)) {
    return(list(valid = FALSE, message = "The specified directory does not exist on this machine. Select a valid folder path."))
  }
  if (is.null(filename) || nchar(trimws(filename)) == 0) {
    return(list(valid = FALSE, message = "The file output identifier cannot be left blank."))
  }
  
  return(list(valid = TRUE, message = "Parameters verified successfully. Ready to export script and initiate optimization run."))
})

observe({
  status <- script_parameters_status()
  
  if (!status$valid) {
    shinyjs::disable("btn_start_optimization")
    shinyjs::addClass(id = "script_export_dir_text", class = "is-invalid")
    shinyjs::addClass(id = "script_export_filename", class = "is-invalid")
  } else {
    shinyjs::enable("btn_start_optimization")
    shinyjs::removeClass(id = "script_export_dir_text", class = "is-invalid")
    shinyjs::removeClass(id = "script_export_filename", class = "is-invalid")
  }
})

output$display_script_parameters_status <- renderUI({
  status <- script_parameters_status()
  if (!status$valid) {
    return(p(icon("triangle-exclamation"), status$message, class = "text-danger small fw-bold mt-1 mb-0"))
  } else {
    return(p(icon("circle-check"), status$message, class = "text-success small mt-1 mb-0"))
  }
})

# =========================================================================
# DIRECT IN-MEMORY LAUNCH (NO INTERMEDIATE FILES)
# =========================================================================
observeEvent(input$btn_start_optimization, {
  req(script_parameters_status()$valid)

  showNotification("Starting optimization directly in the background...", type = "message")
  console_buffer("Initializing analytical computation threads...\n")

  # 1. Collect all active parameters from the user interface
  selected_ids <- active_selected_ids()
  catalog <- CURRENT_SCHEMA_CATALOG()

  # Instantiate a temporary engine just to structure the parameters configured in Tab 3
  # Note: add_parameters accepts vectors or lists of Parameter objects
  
  selected_params <- catalog[catalog$id %in% selected_ids, , drop = FALSE]

  params_list <- list()

  for (i in seq_len(nrow(selected_params))) {
      p_row <- selected_params[i, , drop = FALSE]
      param_id <- p_row$id[]
      json_path <- p_row$json_path[]
      schema_file <- p_row$schema_file[]
      param_type <- p_row$type[]
      enum_values <- p_row$enum_values[]
      
      param_class <- resolve_parameter_class(param_type, json_path, schema_file, enum_values)
      adjustment_ids <- adjustment_ids_state[[param_id]] %||% 1L
      
      for (adj_id in adjustment_ids) {
        dest_file <- input[[paste0("destinations_", param_id, "_", adj_id)]] %||% ""
        min_v <- input[[paste0("min_", param_id, "_", adj_id)]] %||% p_row$min[]
        max_v <- input[[paste0("max_", param_id, "_", adj_id)]] %||% p_row$max[]
        custom_name <- input[[paste0("name_", param_id, "_", adj_id)]]
        if (is.null(custom_name) || custom_name == "") custom_name <- paste0(param_id, "_", adj_id)
        
        if (param_class %in% c("RealValued", "Integer")) {
            params_list[[length(params_list)+1]] <- WeaverOptimiser[[param_class]]$new(
                name = custom_name,
                min = min_v,
                max = max_v,
                fixed_destinations = list(
                    list(
                        file_path = dest_file,
                        json_path = json_path
                    )
                ),
                calculated_destinations = list(),
                constraints = list()
            )
        }
        else if (param_class == "Abundances") {
          instar_count <- get_instar_count_from_destination(dest_file, paths$base_config_path)
          dens_vector <- c()
          if (instar_count > 0) {
            for (v in seq_len(instar_count)) {
              dens_vector <- c(dens_vector, input[[paste0("density_elem_", param_id, "_", adj_id, "_", v)]] %||% 0.0)
            }
          } else { dens_vector <- c(1.0) }
          
          params_list[[length(params_list)+1]] <- WeaverOptimiser[[param_class]]$new(
              name = custom_name,
              densitiesVector = dens_vector,
              min = min_v,
              max = max_v,
              fixed_destinations = list(
                  list(
                      file_path = dest_file,
                      json_path = json_path
                  )
              ),
              constraints = list()
          )
        }
        else if (param_class == "Preference") {
          pred_sp <- input[[paste0("pref_predator_sp_", param_id, "_", adj_id)]]
          pred_ins <- input[[paste0("pref_predator_instars_", param_id, "_", adj_id)]]
          prey_sp <- input[[paste0("pref_prey_sp_", param_id, "_", adj_id)]]
          prey_ins <- input[[paste0("pref_prey_instars_", param_id, "_", adj_id)]]
          
          params_list[[length(params_list)+1]] <- WeaverOptimiser[[param_class]]$new(
              name = custom_name,
              cells = list(
                  WeaverOptimiser$PreferenceCell$new(
                      predator_species = pred_sp,
                      predator_instars = pred_ins,
                      prey_species = prey_sp,
                      prey_instars = prey_ins
                  )
              ),
              min = min_v,
              max = max_v
          )
        }
      }
  }
  
  # 2. Prepare the exact arguments for the RUN method
  opt_args <- list(
    population_size = 45,
    max_generations = 20,
    fitness_class = weaverR::MultiObjectiveFitness$new(...), # Your objectives configuration
    program = paths$program,                                 # The validated WeaverCLI.exe
    output_path = paths$output_path,                         # User output directory
    base_config_path = paths$base_config_path,               # User base configuration
    keepBest = TRUE,
    tournament_size = 5,
    crossover_prob = 0.8,
    mutation_rate = 0.05,
    debug = isTRUE(input$debug),
    seed = input$seed %||% 0,
    total_cores = if(isTRUE(input$total_cores_auto)) NULL else input$total_cores,
    program_cores = input$program_cores %||% 4
  )
  
  # 3. BACKGROUND LAUNCH VIA CALLR (direct in-memory)
  # Runs the package function in an isolated R session so the UI does not freeze
  bg_process <- callr::r_bg(
    func = function(args, p_list) {
      library(weaverR)

      # Get the package's R6 engine and inject the built parameters
      engine <- weaverR:::.get_internal_optimiser_engine()
      engine$add_parameters(p_list)
      
      # Run the genetic algorithm passing the argument list
      do.call(engine$run, args)
    },
    args = list(args = opt_args, p_list = params_list),
    supervise = TRUE
  )
  
  # 4. HOT POLLING TIMER (logs and charts)
  # Will track the log.txt and evolution_history.txt files generated by the R6 engine
  log_started <- FALSE
  target_log_path <- NULL
  target_evo_path <- NULL
  
  obs_timer <- setInterval(1000, {
    # callr lets us check whether the background analytical process is still alive
    process_alive <- bg_process$is_alive()
    
    # Try to capture the real paths created by the R6 subprocess
    if (!log_started) {
      # Look for the most recent "optimisation_..." folder in the output directory
      all_dirs <- list.dirs(paths$output_path, recursive = FALSE, full.names = TRUE)
      active_dir <- tail(sort(all_dirs[grepl("optimisation_", all_dirs)]), 1)
      
      if (length(active_dir) > 0 && dir.exists(active_dir)) {
        target_log_path <- file.path(active_dir, "log.txt")
        target_evo_path <- file.path(active_dir, "evolution_history.txt")
        log_started <- TRUE
      }
    }
    
    # Read and dump the progress into the UI console
    if (log_started && file.exists(target_log_path)) {
      lines <- readLines(target_log_path, warn = FALSE)
      console_buffer(paste(lines, collapse = "\n"))
      shinyjs::runjs("$('#raw_console_viewport').scrollTop($('#raw_console_viewport').scrollHeight);")
    }
    
    # Update the real-time multi-objective breakdown charts
    if (log_started && file.exists(target_evo_path)) {
      try({
        df_evo <- read.table(target_evo_path, header = TRUE, sep = "\t", stringsAsFactors = FALSE)
        evolution_data_reactive(df_evo)
      }, silent = TRUE)
    }
    
    # If the process finished, destroy the Shiny timer to free memory
    if (!process_alive) {
      clearInterval(obs_timer)
      showNotification("Weaver optimization process finished.", type = "string")
    }
  })
})

# Bind the detailed multi-objective graphical outputs (exact mapping from the previous step)

detected_metrics_columns <- reactive({
df <- evolution_data_reactive()
if (is.null(df) || nrow(df) == 0) return(character(0))
all_cols <- colnames(df)
control_cols <- c("Generation", "Individual", "Result_folder", "id")
metrics <- setdiff(all_cols, control_cols)
active_selected <- active_selected_ids()
metrics <- metrics[!metrics %in% active_selected]
return(metrics)
})
output$multiobjective_plots_viewport <- renderUI({
metrics <- detected_metrics_columns()
if (length(metrics) == 0) return(p(class = "text-muted italic", "Waiting for multi-objective data matrix streams..."))
plot_output_list <- lapply(metrics, function(m_name) {
div(
class = "mb-4 p-2 border rounded bg-white shadow-sm",
tags$h6(class = "fw-bold text-secondary px-2 pt-1", icon("chart-area"), paste(" Objective Metric:", m_name)),
plotOutput(paste0("realtime_plot_for_metric_", m_name), height = "220px")
)
})
do.call(tagList, plot_output_list)
})
observe({
metrics <- detected_metrics_columns()
df <- evolution_data_reactive()
req(length(metrics) > 0, !is.null(df))
for (m_name in metrics) {
local({
current_metric <- m_name
output[[paste0("realtime_plot_for_metric_", current_metric)]] <- renderPlot({
df_clean <- evolution_data_reactive()
req(!is.null(df_clean))
df_clean[[current_metric]] <- as.numeric(df_clean[[current_metric]])
df_clean$Generation <- as.integer(df_clean$Generation)
is_minimise <- (current_metric == "coefficientOfVariation")
summary_trajectory <- df_clean %>%
group_by(Generation) %>%
summarise(Optimum = if(is_minimise) min(!!sym(current_metric), na.rm=TRUE) else max(!!sym(current_metric), na.rm=TRUE))
ggplot(df_clean, aes(x = Generation, y = !!sym(current_metric))) +
geom_point(color = "#3498db", alpha = 0.4, size = 1.5) +
geom_line(data = summary_trajectory, aes(x = Generation, y = Optimum, color = "Generational Elite"), linewidth = 1) +
scale_color_manual(values = c("Generational Elite" = "#e74c3c")) +
theme_minimal() +
theme(legend.position = "bottom", legend.title = element_blank()) +
labs(x = "Generation", y = paste("Value [", current_metric, "]"))
})
})
}
})
output$text_stream_logs <- renderText({ console_buffer() })
output$table_generation_summary <- renderTable({
df <- evolution_data_reactive()
metrics <- detected_metrics_columns()
req(!is.null(df), length(metrics) > 0)
df %>% group_by(Generation) %>% summarise(
Population = n(),
Best_Fitness_TotalTime = max(as.numeric(totalTime), na.rm = TRUE),
CV_Minimum = min(as.numeric(coefficientOfVariation), na.rm = TRUE)
)
})
output$table_best_solutions <- renderTable({
df <- evolution_data_reactive()
req(!is.null(df))
df %>% arrange(as.numeric(coefficientOfVariation)) %>% head(5)
})