#' World Tab UI for the Configuration Generator App
#'
#' This script defines the **UI for the "World" tab** in the Shiny Configuration
#' Generator application. The tab allows users to view and modify general
#' world parameters.
#'
#' @details
#' The tab uses a `tabPanel` containing:
#' - A globe icon and the title "World".
#' - A `fluidPage` with a section header and descriptive text.
#'
#' The `h3` header displays "Configuración del Mundo" (World Configuration),
#' and the `p` paragraph provides instructions for the user.
#'
#' @import shiny
#' @importFrom shiny.tags tags
#' @importFrom shiny.icon icon
#' @export
#'
#' @examples
#' \dontrun{
#' # Include this tab in a Shiny tabsetPanel:
#' ui <- fluidPage(
#'   tabsetPanel(world_ui)
#' )
#' }

world_ui <- tabPanel(
  # Tab title with a globe icon
  tags$div(icon("globe"), "World"),

  # Page content for the World tab
  fluidPage(
    # Section header
    h3("Configuración del Mundo"),

    # Descriptive paragraph
    p("Aquí puedes definir los parámetros generales del mundo.")
  )
)
