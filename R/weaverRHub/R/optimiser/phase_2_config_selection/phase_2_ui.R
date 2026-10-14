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
    
    h5("Phase 2: Define Optimization Variables Matrix"),
    p(class = "text-muted", "Construct generic optimization variables, select their technical data types, and assign a matrix list of paired JSON configuration files and relative internal target paths ($ separator)."),
    hr(),

    uiOutput(ns("display_phase_2_errors")),
    
    # Main broker dashboard card to orchestrate variables creation
    bslib::card(
      class = "mb-3 shadow-sm border-secondary-subtle card optimiser-input-card",
      bslib::card_header(
        div(class = "d-flex justify-content-between align-items-center w-100",
            tags$b(icon("sliders"), " Optimization Parameter Variables Broker"),
            actionButton(ns("add_variable_btn"), "Add Optimization Variable", icon = icon("plus"), class = "btn-sm btn-primary")
        )
      ),
      bslib::card_body(
        p(class = "text-muted small", 
          "Click the button above to declare an independent decision variable for the Genetic Algorithm. You can map a single variable to replicate identical optimized values across separate keys and JSON fields concurrently."
        ),
        hr(class = "my-2"),
        
        # Delegamos la persistencia estática al servidor inspirándonos en la Fase 1
        uiOutput(ns("render_matrix_variables_workspace"))
      )
    )
  )
}