#' User Interface for the Configuration Generator App
#'
#' This script defines the **UI (User Interface)** for the Configuration Generator
#' Shiny application. It sources the UI components of each tab (world, configuration,
#' animal species, and resource species) and combines them into a single
#' `fluidPage` layout with a tabbed interface.
#'
#' @details
#' The UI is composed of multiple tabs, each defined in its own file:
#' - `tabs/configuration/configuration_ui.r`: Configuration settings
#' - `tabs/world/world_ui.r`: World/environment parameters
#' - `tabs/animal_species/animal_species_ui.r`: Animal species definitions
#' - `tabs/resource_species/resource_species_ui.r`: Resource species definitions
#'
#' These modules are sourced into the current environment and then combined
#' inside a Shiny `tabsetPanel`. The main title of the application is
#' *"Configuration Generator"*.
#'
#' @import shiny
#' @export
#'
#' @examples
#' \dontrun{
#' # Run the Shiny app after sourcing this file and the server file:
#' shiny::runApp()
#' }

# ----------------------------------------------------------------------
# Load the tab UI components
# ----------------------------------------------------------------------

# Source the UI modules for each tab of the application.
source("tabs/world/world_ui.r")
source("tabs/configuration/configuration_ui.r")
source("tabs/animal_species/animal_species_ui.r")
source("tabs/resource_species/resource_species_ui.r")

# ----------------------------------------------------------------------
# Define the main UI layout
# ----------------------------------------------------------------------

# Create a fluid page layout containing a title panel and a tabset panel.
# Each tab corresponds to a separate functional section of the configuration app.
ui <- fluidPage(
  # Application title
  titlePanel("Configuration Generator"),
  
  # Tabset panel containing the individual tab UIs
  tabsetPanel(
    configuration_ui,
    world_ui,
    animal_species_ui,
    resource_species_ui
  )
)
