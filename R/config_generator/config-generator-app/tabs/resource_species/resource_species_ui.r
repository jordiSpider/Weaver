#' Resource Species Tab UI for the Configuration Generator App
#'
#' This script defines the **UI for the "Resource Species" tab** in the Shiny
#' Configuration Generator application. The tab allows users to filter,
#' select, and manage resource species from the global registry.
#'
#' @details
#' The tab uses a `tabPanel` containing:
#' - A tab title with a seedling icon and the label "Resource Species".
#' - A `sidebarLayout` dividing the tab into a sidebar and main panel.
#'
#' **Sidebar Panel:**
#' - A header (`h4`) for "Taxonomic Search".
#' - A set of `selectizeInput` widgets for each column in the
#'   `resource_species_register` (except the last column), allowing multiple selections.
#' - A `selectInput` to choose the specific resource species to generate.
#' - An `actionButton` to add the selected resource species.
#'
#' **Main Panel:**
#' - A header (`h4`) for "Selected Resource Species".
#' - A `tabsetPanel` (with ID `tabset_selected_resource_species`) to display
#'   the currently selected resource species dynamically.
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
#'   tabsetPanel(resource_species_ui)
#' )
#' }

resource_species_ui <- tabPanel(
    # Tab title with a seedling icon
    tags$div(icon("seedling"), "Resource Species"),
    
    # Layout with sidebar and main panel
    sidebarLayout(
        # Sidebar panel for filtering and adding resource species
        sidebarPanel(
            h4("Taxonomic Search"),
            
            # Generate a selectizeInput for each column except the last
            lapply(colnames(resource_species_register)[-length(colnames(resource_species_register))], function(field) {
                selectizeInput(
                    field, 
                    paste0(field, ":"), 
                    choices = unique(resource_species_register[[field]]), 
                    multiple = TRUE
                )
            }),
            
            # Select specific resource species to generate
            selectInput(
                "ResourceSpecies", 
                "Select the resource species to generate:", 
                choices = unique(resource_species_register$Species)
            ),

            # Button to add the selected species
            actionButton("add_resource_species_button", "Add"),
        ),

        # Main panel to display selected resource species
        mainPanel(
            h4("Selected Resource Species:"),
            tabsetPanel(
                id = "tabset_selected_resource_species"
            )
        )
    )
)
