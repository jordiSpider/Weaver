#' Phase 2: Define Optimization Variables UI (Merged Workspace)
#'
#' @description Implements a clean workspace to append unified optimization variables.
#' Each variable governs a specific parameter class and maps to multiple target pairs 
#' of JSON Configuration Files and internal JSON Paths.
#' @param id Unique module namespace identifier string.
#' @import shiny
#' @importFrom bslib card card_header card_body
#' @export
mod_optimiser_phase_2_ui <- function(id) {
  ns <- NS(id)
  
  fluidPage(
    shinyjs::useShinyjs(),
    
    h5("Phase 2: Define Optimization Variables"),
    p(class = "text-muted", "Define the optimisation variables and set the values for all their elements."),
    hr(),

    uiOutput(ns("display_phase_2_errors")),
    
    # Main broker dashboard card to orchestrate variables creation
    bslib::card(
      bslib::card_header(
        div(class = "d-flex justify-content-between align-items-center w-100",
            tags$b(icon("sliders"), " Optimization Parameter Variables Broker"),
            actionButton(ns("add_variable_btn"), "Add Optimization Variable", icon = icon("plus"), class = "btn-sm btn-primary")
        )
      ),
      bslib::card_body(
        # Delegamos la persistencia estática al servidor inspirándonos en la Fase 1
        uiOutput(ns("render_matrix_variables_workspace"))
      )
    )
  )
}