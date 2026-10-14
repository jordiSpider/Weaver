#' Animal Species UI Tab
#'
#' This defines the **UI for the "Animal Species" tab** in the Weaver Configuration Generator Shiny app.
#'
#' @details
#' The tab includes:
#' 1. **Taxonomic Search Panel** (sidebar):
#'    - Selectize inputs for filtering species by taxonomy (Phylum, Class, Order, Family, Genus).
#'    - A selectInput to choose a specific animal species to generate.
#'    - An "Add" button to include the selected species in the main panel.
#'
#' 2. **Selected Animal Species Panel** (main panel):
#'    - Displays a tabset panel where each added species appears as a tab.
#'    - Tabs are populated dynamically by the server module (`animal_species_server`).
#'
#' @examples
#' \dontrun{
#' # Include this UI tab in the main UI function:
#' ui <- navbarPage(
#'   "Weaver Configuration Generator",
#'   animal_species_ui,
#'   resource_species_ui,
#'   configuration_ui,
#'   world_ui
#' )
#' }
#'
#' @import shiny
#' @export
animal_species_ui <- tabPanel(
    tags$div(icon("paw"), "Animal Species"),  # Tab title with icon
    sidebarLayout(
        sidebarPanel(
            h4("Taxonomic Search"),

            # Generate selectize inputs for each taxonomy field except the last column
            lapply(colnames(animal_species_register)[-length(colnames(animal_species_register))], function(field) {
                selectizeInput(field, paste0(field, ":"), choices = unique(animal_species_register[[field]]), multiple = TRUE)
            }),
            
            # Select a specific animal species to generate
            selectInput("AnimalSpecies", "Select the animal species to generate:", choices = unique(animal_species_register$Species)),

            # Button to add selected animal species to main panel
            actionButton("add_animal_species_button", "Add"),
        ),
        
        mainPanel(
            h4("Selected Animal Species:"),

            # Tabset panel where each added species appears dynamically
            tabsetPanel(
                id = "tabset_selected_animal_species"
            )
        )
    )
)
