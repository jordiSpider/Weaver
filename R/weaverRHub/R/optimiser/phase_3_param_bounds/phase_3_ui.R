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
    
    h5("Phase 3: Launch Simulation & View Results Workspace"),
    p(class = "text-muted", "Finalize export target setups, configure runtime hardware boundaries, and trigger the multi-objective genetic solver."),
    hr(),
    
    # --- CARD 1 (FULL WIDTH): EXPORT SETTINGS ONLY ---
    bslib::layout_columns(
      col_widths = c(12),
      fill = FALSE,
      
      bslib::card(
        class = "mb-3 shadow-sm border-secondary-subtle card optimiser-input-card",
        bslib::card_header(
          tags$b(icon("folder-open"), " Output Environment & Export Target Settings")
        ),
        bslib::card_body(
          span(tags$b("Optimization Target Output Directory (Export Blueprint Target):")),
          div(class = "my-2 optimiser-input-card",
              textInput(ns("output_dir_text_path"), label = NULL, value = "", placeholder = "Enter absolute or relative path for configuration blueprint JSON dumps...", width = "100%"),
              div(class = "mt-2",
                  shinyFiles::shinyDirButton(ns("output_dir_browse_btn"), "Browse Output Directory...", title = "Select Target Export Directory", class = "btn-outline-primary w-100")
              )
          ),
          uiOutput(ns("error_local_output_dir_path")),
          uiOutput(ns("display_output_dir_path_confirmation")),
          
          hr(class = "my-3"),
          
          div(class = "optimiser-input-card",
              tags$label(class = "control-label fw-bold mb-1", "Optimization State Export Filename:"),
              div(class = "input-group mb-1",
                  textInput(ns("config_json_filename"), label = NULL, value = "optimization_setup", placeholder = "Enter filename setup...", width = "100%"),
                  tags$span(class = "input-group-text font-monospace text-muted", style = "background-color: var(--bs-secondary-bg); border-color: var(--bs-border-color); font-weight: bold; margin-left: -1px; z-index: 5;", ".json")
              ),
              uiOutput(ns("error_local_json_filename"))
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
        class = "mb-3 shadow-sm border-secondary-subtle card optimiser-input-card",
        style = "border-left-color: #18bc9c !important;",
        bslib::card_header(
          tags$b(icon("play"), " Optimizer Process Execution Pipeline")
        ),
        bslib::card_body(
          # 2.1 MIGRATED DIRECTORY CONTROLLER FOR EXECUTION OUTPUTS
          span(tags$b("Simulation Telemetry Output Directory (Runtime Logs Dump):")),
          div(class = "my-2 optimiser-input-card",
              textInput(ns("output_dir_text_path_run"), label = NULL, value = "", placeholder = "Enter absolute or relative path for process logs & runtime telemetry dumps...", width = "100%"),
              div(class = "mt-2",
                  shinyFiles::shinyDirButton(ns("output_dir_browse_btn_run"), "Browse Simulation Directory...", title = "Select Simulation Output Directory", class = "btn-outline-primary w-100")
              )
          ),
          uiOutput(ns("error_local_output_dir_path_run")),
          uiOutput(ns("display_output_dir_path_confirmation_run")),
          
          hr(class = "my-3"),
          
          # 2.2 MIGRATED GENERAL COMPUTATION CONFIGURATIONS
          bslib::layout_columns(
            col_widths = c(6, 6),
            div(class = "integer-input optimiser-input-card",
                numericInput(ns("seed"), tags$b("Random Seed:"), value = 0L, min = 0, step = 1)
            ),
            shinyWidgets::switchInput(ns("debug"), tags$b("Debug Mode"), value = FALSE)
          ),

          hr(class = "my-3"),

          div(class = "integer-input optimiser-input-card",
            numericInput(ns("program_cores"), tags$b("Cores per Simulation Instance:"), value = 4L, min = 1, step = 1)
          ),
          
          bslib::layout_columns(
            col_widths = c(6, 6),
            fill = FALSE,
            div(class = "pt-2",
                shinyWidgets::switchInput(ns("total_cores_auto"), tags$b("Auto-detect Total System Cores"), value = TRUE)
            ),
            conditionalPanel(
              condition = sprintf("input['%s'] == false", ns("total_cores_auto")),
              div(class = "integer-input optimiser-input-card",
                  numericInput(inputId = ns("total_cores"), label = tags$b("Total Allocated Cores:"), value = 8L, min = 1, step = 1)
              )
            )
          ),
          
          div(class = "alert alert-info py-2 my-3", role = "alert",
              div(class = "d-flex align-items-center small", 
                  icon("circle-info", class = "me-2"), 
                  "Status: Execution pipeline idle. Ready to initialize parallel threads background workers."
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
              p(class = "text-muted small mb-2", "Monitors live process diagnostics stream, evolutionary generation indices, and error conditions vectors parsed directly from system outputs:"),
              verbatimTextOutput(ns("live_terminal_console_stream"))
          )
        ),
        
        bslib::nav_panel(
          title = "Results Analysis Dashboard",
          icon = icon("chart-line"),
          div(class = "p-2",
              p(class = "text-muted small mb-3", "Analyze multi-objective Pareto front convergence boundaries, fitness metrics over evolutionary steps, and final optimal hyper-parameters distribution structures:"),
              
              bslib::layout_columns(
                col_widths = c(6, 6),
                plotOutput(ns("plot_pareto_front"), height = "350px"),
                plotOutput(ns("plot_fitness_convergence"), height = "350px")
              )
          )
        )
      )
    )
  )
}