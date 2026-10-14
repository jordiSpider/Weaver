#' Simulation Optimiser Module UI
#'
#' @description Structural shell layout for the updated 4-phase sequential sequential ecosystem optimization.
#' @param id Unique module namespace identifier string.
#' @import shiny
#' @export
mod_simulation_optimiser_ui <- function(id) {
  ns <- NS(id)
  
  bslib::page_sidebar(
    title = "Ecosystem Optimiser Workspace",
    sidebar = bslib::sidebar(
      title = "Workflow Navigation",
      uiOutput(ns("render_phase_wizard_stepper")),
      hr(),
      # Dynamic placeholder mapping responsive interactive steering action buttons
      uiOutput(ns("render_navigation_buttons_controls"))
    ),
    
    # Inject common CSS style definitions sheet
    get_shared_optimiser_styles(),
    
    # Dynamic placeholder mapping the active phase layout workspace
    uiOutput(ns("render_active_phase_ui"))
  )
}