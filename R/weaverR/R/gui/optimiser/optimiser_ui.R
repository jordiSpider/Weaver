#' Simulation Optimiser Module UI
#'
#' @description Structural shell layout for the updated 4-phase sequential sequential ecosystem optimization.
#' @param id Unique module namespace identifier string.
#' @import shiny
#' @export
mod_simulation_optimiser_ui <- function(id) {
  ns <- NS(id)
  
  bslib::page_sidebar(
    title = div(class = "d-flex align-items-center justify-content-between w-100",
      span("Ecosystem Optimiser Workspace"),
      div(class = "d-flex align-items-center gap-2",
        uiOutput(ns("render_navigation_buttons_controls")),
        tags$span(style = "border-left: 1px solid var(--bs-border-color); height: 24px; margin: 0 4px;"), 
        actionButton("btn_back_from_optimiser", "Return to Hub", icon = icon("arrow-left"), class = "btn-outline-secondary btn-sm")
      )
    ),
    sidebar = bslib::sidebar(
      title = "Workflow Navigation",
      uiOutput(ns("render_phase_wizard_stepper"))
    ),
    
    # Inject common CSS style definitions sheet
    get_shared_optimiser_styles(),
    
    # Dynamic placeholder mapping the active phase layout workspace
    uiOutput(ns("render_active_phase_ui"))
  )
}