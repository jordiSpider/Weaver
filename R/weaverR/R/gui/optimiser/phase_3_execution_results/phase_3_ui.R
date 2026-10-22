#' Phase 3: Launch Simulation & View Results UI
#'
#' @description Full-width card layout. Decouples export settings from the live execution 
#' pipeline and embeds hardware thread controllers migrated from Phase 1.
#' @param id Unique module namespace identifier string.
#' @import shiny
#' @importFrom bslib card card_header card_body layout_columns navset_card_tab nav_panel
#' @importFrom shinyFiles shinyDirButton
#' @importFrom shinyWidgets switchInput
#' @export
mod_optimiser_phase_3_ui = function(id) {
  ns <- NS(id)
  
  fluidPage(
    shinyjs::useShinyjs(),
    
    h5("Phase 3: Launch Simulation & View Results"),
    p(class = "text-muted", "Finalize export target setups, configure runtime hardware boundaries, and trigger the multi-objective genetic solver."),
    hr(),
    
    # --- CARD 1 (FULL WIDTH): EXPORT SETTINGS ONLY ---
    bslib::layout_columns(
      col_widths = c(12),
      fill = FALSE,
      
      bslib::card(
        bslib::card_header(
          tags$b(icon("folder-open"), " Output Environment & Export Target Settings")
        ),
        bslib::card_body(
          div(class = "optimiser-input-card",
              div(class = "mb-2", tags$b("Optimization Template Output Directory:")),
              textInput(ns("output_dir_text_path"), label = NULL, value = "", placeholder = "Enter absolute or relative path for optimization template JSON files...", width = "100%"),
              div(class = "mt-1",
                  shinyFiles::shinyDirButton(ns("output_dir_browse_btn"), "Browse Output Directory...", title = "Select Target Export Directory", class = "btn-outline-primary w-100")
              ),
              div(class = "mt-3", tags$small(class = "text-muted d-block", "Specifies the absolute or relative filesystem path where the structured optimization template JSON file will be generated and saved.")),
              div(class = "mt-3",
                  uiOutput(ns("error_local_output_dir_path")),
                  uiOutput(ns("display_output_dir_path_confirmation"))
              )
          ),
          
          div(class = "optimiser-input-card",
              div(class = "mb-2", tags$b("Optimization State Export Filename:")),
              div(class = "input-group mb-3 align-items-stretch",
                div(class = "flex-grow-1 shiny-input-container", style = "width: 0; margin-bottom: 0;",
                    textInput(ns("config_json_filename"), label = NULL, value = "optimization_setup", placeholder = "Enter filename setup...", width = "100%")
                ),
                tags$span(class = "input-group-text font-monospace text-muted h-auto", 
                          style = "background-color: var(--bs-secondary-bg); border-color: var(--bs-border-color); font-weight: bold; margin-left: -1px;", 
                          ".json")
              ),
              div(class = "mt-3", tags$small(class = "text-muted d-block", "Defines the name of the output configuration file (with a fixed .json extension).")),
              div(class = "mt-3",
                uiOutput(ns("error_local_json_filename"))
              )
          ),

          uiOutput(ns("render_export_state_controls"))
        )
      )
    ),
    
    # --- CARD 2 (FULL WIDTH): LIVE EXECUTION PIPELINE & MIGRATED CONTROLS ---
    bslib::layout_columns(
      col_widths = c(12),
      fill = FALSE,
      
      bslib::card(
        bslib::card_header(
          tags$b(icon("play"), " Optimizer Process Execution Pipeline")
        ),
        bslib::card_body(
          div(class = "optimiser-input-card",
              div(class = "mb-2", tags$b("Simulation Telemetry Output Directory:")),
              textInput(ns("output_dir_text_path_run"), label = NULL, value = "", placeholder = "Enter absolute or relative path for process logs & runtime telemetry dumps...", width = "100%"),
              div(class = "mt-1",
                  shinyFiles::shinyDirButton(ns("output_dir_browse_btn_run"), "Browse Simulation Directory...", title = "Select Simulation Output Directory", class = "btn-outline-primary w-100")
              ),
              div(class = "mt-3", tags$small(class = "text-muted d-block", "Declares the physical target folder where the simulator dumps real-time background orchestration metrics, process diagnostics logs, and historical generational data vectors.")),
              div(class = "mt-3",
                  uiOutput(ns("error_local_output_dir_path_run")),
                  uiOutput(ns("display_output_dir_path_confirmation_run"))
              )
          ),
          
          # 2.2 MIGRATED GENERAL COMPUTATION CONFIGURATIONS
          bslib::layout_columns(
            col_widths = c(6, 6),
            div(class = "integer-input optimiser-input-card",
                numericInput(ns("seed"), tags$b("Random Seed:"), value = 0L, min = 0, step = 1),
                tags$small(class = "text-muted d-block", "Establishes the starting numerical seed for the pseudo-random number generator.")
            ),
            div(class = "optimiser-input-card",
                tags$b("Debug Mode:"),
                div(class = "d-flex align-items-center gap-3 mt-1",
                    div(style = "flex: 1 1 0%; min-width: 0;",
                        shinyWidgets::switchInput(ns("debug"), label = NULL, value = FALSE, inline = TRUE)
                    )
                ),
                tags$small(class = "text-muted d-block", "A boolean toggle switch that activates granular system trace logging.")
            )
          ),

          div(class = "integer-input optimiser-input-card",
            numericInput(ns("program_cores"), tags$b("Cores per Simulation Instance:"), value = 4L, min = 1, step = 1),
            tags$small(class = "text-muted d-block", "Allocates the exact number of physical CPU cores or computational execution hardware threads assigned to run each isolated, parallel simulation run.")
          ),
          
          bslib::layout_columns(
            col_widths = c(6, 6),
            fill = FALSE,
            div(class = "pt-2 optimiser-input-card",
                tags$b("Auto-detect Total System Cores:"),
                div(class = "d-flex align-items-center gap-3 mt-1",
                    div(style = "flex: 1 1 0%; min-width: 0;",
                        shinyWidgets::switchInput(ns("total_cores_auto"), label = NULL, value = TRUE, inline = TRUE)
                    )
                ),
                tags$small(class = "text-muted d-block", "A smart toggle that queries and locks onto the full threading capacities of the hosting machine.")
            ),

            conditionalPanel(
              condition = sprintf("input['%s'] == false", ns("total_cores_auto")),
              div(class = "integer-input optimiser-input-card",
                  numericInput(inputId = ns("total_cores"), label = tags$b("Total Allocated Cores:"), value = 8L, min = 1, step = 1),
                  tags$small(class = "text-muted d-block", "Provides custom limits to define the absolute hardware ceiling allocated.")
              )
            )
          ),
          
          div(class = "d-flex flex-column gap-2 mt-3",
              uiOutput(ns("render_launch_execution_controls")),
              actionButton(ns("terminate_optimizer_btn"), "Force Stop Execution", class = "btn btn-outline-danger btn-sm w-100 disabled", icon = icon("ban"))
          )
        )
      )
    ),

    # ============================================================================
    # GLOBAL GENERATIONAL PROGRESS BAR (ZONA DE VISUALIZACIÓN CONTINUA)
    # ============================================================================
    uiOutput(ns("global_generation_progress_bar")),
    
    # --- ROW 3: LIVE MONITORING TERMINAL CONSOLE ---
    bslib::layout_columns(
      col_widths = c(12),
      fill = FALSE,
      
      bslib::navset_card_tab(
        id = ns("analysis_tabs_container"),
        title = tags$b(icon("gauge-high"), " Live Telemetry Monitor & Analytical Dashboards"),
        sidebar = NULL,
        
        bslib::nav_panel(
          title = "Optimization Progress Logs",
          icon = icon("terminal"),
          div(class = "p-2",
              verbatimTextOutput(ns("live_terminal_console_stream"))
          )
        ),
        
        bslib::nav_panel(
          title = "Results Analysis Dashboard",
          icon = icon("chart-line"),
          div(class = "p-2",
              uiOutput(ns("analysis_generation_status")),
              
              bslib::layout_columns(
                col_widths = c(3, 3, 3, 3),
                selectInput(ns("analysis_generation"), "Generation completed", choices = NULL),
                selectInput(ns("analysis_objective"), "Objective", choices = NULL),
                selectInput(ns("analysis_objective_x"), "Objective space: X-axis", choices = NULL),
                selectInput(ns("analysis_objective_y"), "Objective space: Y-axis", choices = NULL)
              ),
              bslib::navset_card_tab(
                id = ns("generation_analysis_tabs"),
                bslib::nav_panel(
                  title = "Convergence",
                  icon = icon("chart-line"),
                  uiOutput(ns("fitness_convergence_panel"))
                ),
                bslib::nav_panel(
                  title = "Population distribution",
                  icon = icon("chart-column"),
                  uiOutput(ns("fitness_distribution_panel"))
                ),
                bslib::nav_panel(
                  title = "Objective space",
                  icon = icon("bullseye"),
                  p(class = "text-muted small", "The unselected points show the population front. The marker with a star identifies the individual selected by the configured fitness function."),
                  uiOutput(ns("objective_space_panel"))
                )
              )
          )
        )
      )
    )
  )
}
