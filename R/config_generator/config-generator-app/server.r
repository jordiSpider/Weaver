#' Server Logic for the Configuration Generator App
#'
#' This script defines the **server-side logic** for the Configuration Generator
#' Shiny application. It loads the server modules responsible for each major
#' section of the app — configuration, animal species, and resource species —
#' and integrates them into a unified server function.
#'
#' @details
#' The server modules are implemented separately for modularity and maintainability:
#' - `tabs/configuration/configuration_server.r`: Handles configuration settings logic.
#' - `tabs/animal_species/animal_species_server.r`: Manages behavior and data related to animal species.
#' - `tabs/resource_species/resource_species_server.r`: Controls logic for resource species definitions.
#'
#' Each of these modules receives the standard Shiny parameters:
#' - `input`: A list of input values from the UI.
#' - `output`: A list of outputs rendered to the UI.
#' - `session`: The current reactive session environment.
#'
#' The main `server()` function delegates execution to each module, passing along
#' the same reactive environment so that all tabs can share data and reactivity if needed.
#'
#' @import shiny
#' @export
#'
#' @examples
#' \dontrun{
#' # Run the app after sourcing both ui.R and server.R
#' shiny::runApp()
#' }

# ----------------------------------------------------------------------
# Load the server logic modules
# ----------------------------------------------------------------------

# Source the server modules for each tab of the application.
source("tabs/configuration/configuration_server.r")
source("tabs/animal_species/animal_species_server.r")
source("tabs/resource_species/resource_species_server.r")

# ----------------------------------------------------------------------
# Define the main Shiny server function
# ----------------------------------------------------------------------

# This function orchestrates the logic of the entire Shiny app by
# calling each module’s server function with the same input, output,
# and session objects.
server <- function(input, output, session) {
  configuration_server(input, output, session)
  animal_species_server(input, output, session)
  resource_species_server(input, output, session)
}
