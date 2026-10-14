#' Phase 4: Launch Simulation & View Results UI
#'
#' @description Layout structure incorporating structural Bootstrap 5 input groups for 
#' standard text input file extensions and dedicated localized error slots.
#' @param id Unique module namespace identifier string.
#' @import shiny
#' @importFrom bslib card card_header card_body layout_columns navset_card_tab nav_panel
#' @importFrom shinyFiles shinyDirButton
#' @export
mod_optimiser_phase_4_ui = function(id) {
  ns <- NS(id)
  
  fluidPage(
    shinyjs::useShinyjs(),
    
    h5("Phase 4: Launch & View Results Workspace"),
    p(class = "text-muted", "Finalize the ecosystem environment parameters layout, trigger the sequential solver execution, and audit multi-objective optimization metrics."),
    hr(),
    
    # --- ROW 1: TWIN HORIZONTAL PARALLEL CARDS ---
    bslib::layout_columns(
      col_widths = c(6, 6),
      fill = FALSE,
      
      # 1.1 LEFT PARALLEL SIDE: Configuration Export & System Output Paths
      bslib::card(
        class = "mb-3 shadow-sm border-secondary-subtle card optimiser-input-card",
        bslib::card_header(
          tags$b(icon("folder-open"), " Output Environment & Export Target Settings")
        ),
        bslib::card_body(
          span(tags$b("Optimization Target Output Directory:")),
          div(class = "my-2 optimiser-input-card",
              textInput(ns("output_dir_text_path"), label = NULL, value = "", placeholder = "Enter absolute or relative path for process logs & telemetry dumps...", width = "100%"),
              div(class = "mt-2",
                  shinyFiles::shinyDirButton(ns("output_dir_browse_btn"), "Browse Output Directory...", title = "Select Target Output Directory", class = "btn-outline-primary w-100")
              )
          ),
          uiOutput(ns("error_local_output_dir_path")),
          uiOutput(ns("display_output_dir_path_confirmation")),
          
          hr(class = "my-3"),
          
          # CORRECCIÓN: Eliminada la clase 'integer-input' para que no interfiera el JS global
          div(class = "optimiser-input-card",
              tags$label(class = "control-label fw-bold mb-1", "Optimization State Export Filename:"),
              
              # Bootstrap Input Group adaptado para encapsular el textInput oficial de Shiny
              div(class = "input-group mb-1",
                  textInput(
                    inputId     = ns("config_json_filename"), 
                    label       = NULL, 
                    value       = "optimization_setup", 
                    placeholder = "Enter filename setup...", 
                    width       = "100%"
                  ),
                  tags$span(
                    class = "input-group-text font-monospace text-muted", 
                    style = "background-color: var(--bs-secondary-bg); border-color: var(--bs-border-color); font-weight: bold; margin-left: -1px; z-index: 5;", 
                    ".json"
                  )
              ),
              uiOutput(ns("error_local_json_filename"))
          ),

          uiOutput(ns("render_export_state_controls"))
        )
      ),
      
      # 1.2 RIGHT PARALLEL SIDE: Optimization Execution Control Panel
      bslib::card(
        class = "mb-3 shadow-sm border-secondary-subtle card optimiser-input-card",
        style = "border-left-color: #18bc9c !important;",
        bslib::card_header(
          tags$b(icon("play"), " Optimizer Process Execution Pipeline")
        ),
        bslib::card_body(
          p("Verify that all structural components from Phases 1, 2, and 3 are finalized before triggering the multi-objective algorithm execution loop."),
          
          div(class = "alert alert-info py-2 my-3", role = "alert",
              div(class = "d-flex align-items-center small", 
                  icon("circle-info", class = "me-2"), 
                  "Status: Execution pipeline idle. Ready to initialize parallel threads background workers."
              )
          ),
          
          div(class = "d-flex flex-column gap-2 mt-4",
              actionButton(ns("launch_optimizer_btn"), "Initialize Optimization Process", class = "btn btn-success btn-lg fw-bold w-100", icon = icon("rocket")),
              actionButton(ns("terminate_optimizer_btn"), "Force Stop Execution", class = "btn btn-outline-danger btn-sm w-100 disabled", icon = icon("ban"))
          )
        )
      )
    ),
    
    # --- ROW 2: FULL-WIDTH LOWER MONITORING & ANALYTICAL DASHBOARD ---
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
              
              tags$div(
                class = "terminal-logs-monitor font-monospace",
                style = "min-height: 250px; background-color: #1e1e1e; color: #a9b7c6; border-radius: 6px;",
                div(class = "log-system-info", "[System Initialize] Awaiting sequence start trigger event allocation..."),
                div(class = "log-system-info", "[Hardware Registry] Memory limits verified. Decoupled async threads pool standby.")
              )
          )
        ),
        
        bslib::nav_panel(
          title = "Results Analysis Dashboard",
          icon = icon("chart-line"),
          
          div(class = "p-2",
              p(class = "text-muted small mb-3", "Analyze multi-objective Pareto front convergence boundaries, fitness metrics over evolutionary steps, and final optimal hyper-parameters distribution structures:"),
              
              bslib::layout_columns(
                col_widths = c(6, 6),
                
                div(class = "border rounded p-4 text-center bg-light secondary-subtle d-flex flex-column justify-content-center align-items-center",
                    style = "min-height: 200px; border-style: dashed !important;",
                    icon("diagram-project", class = "fs-2 text-muted mb-2"),
                    span(class = "text-muted small fw-bold", "Pareto Front Optimization Frontiers Map Placeholder"),
                    span(class = "text-muted small italic", "(Generates automatically upon initial configuration convergence)")
                ),
                
                div(class = "border rounded p-4 text-center bg-light secondary-subtle d-flex flex-column justify-content-center align-items-center",
                    style = "min-height: 200px; border-style: dashed !important;",
                    icon("chart-gantt", class = "fs-2 text-muted mb-2"),
                    span(class = "text-muted small fw-bold", "Evolutionary Fitness Convergence Trends Placeholder"),
                    span(class = "text-muted small italic", "(Tracks generational optimization optimization increments over execution timelines)")
                )
              )
          )
        )
      )
    )
  )
}