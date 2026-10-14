#' Resource Species Server Logic for the Configuration Generator App
#'
#' This script defines the **server-side logic** for managing the "Resource Species"
#' tab in the Shiny Configuration Generator application. It handles filtering of
#' the resource species registry, updating the selectize inputs, adding new resource
#' species tabs, and removing them dynamically.
#'
#' @details
#' The server module includes the following components:
#' 
#' 1. `get_filtered_resource_species_register(input)`:
#'    Filters the `resource_species_register` data frame based on the selected
#'    values in the input fields (e.g., `World`), returning only matching rows.
#'
#' 2. `update_all_selectize_input_resource_species(session, input, field_event)`:
#'    Updates all selectize input fields downstream from the changed field (`field_event`),
#'    ensuring that choices are consistent with the current filter. Handles both
#'    selectize inputs and the last column select input.
#'
#' 3. `selected_resource_species`:
#'    A reactiveValues object storing the currently selected resource species.
#'
#' 4. `resource_species_server(input, output, session)`:
#'    Main server function for the Resource Species tab. It:
#'    - Observes changes in filter fields and updates dependent selectize inputs.
#'    - Handles the "Add" button to dynamically insert a new tab for the selected resource species.
#'    - Handles removal of resource species tabs with a confirmation modal dialog.
#'
#' @import shiny
#' @importFrom shinyjs updateSelectizeInput
#' @importFrom shinyjs updateSelectInput
#' @importFrom shiny.tags tags
#' @export
#'
#' @examples
#' \dontrun{
#' # Include this server module in the main server function:
#' resource_species_server(input, output, session)
#' }

# ----------------------------------------------------------------------
# Filter the resource species register based on input selections
# ----------------------------------------------------------------------
get_filtered_resource_species_register <- function(input) {
    df <- resource_species_register

    if (!is.null(input$World) && length(input$World) > 0) {
        df <- df %>% filter(World %in% input$World)
    }

    return(df)
}

# ----------------------------------------------------------------------
# Update all selectize inputs for resource species based on a field event
# ----------------------------------------------------------------------
update_all_selectize_input_resource_species <- function(session, input, field_event) {
    fields <- colnames(resource_species_register)
    field_event_index <- which(fields == field_event)
    current_input <- reactiveValuesToList(input) 
    filtered_resource_species_register <- get_filtered_resource_species_register(current_input)

    for(index in seq(from = field_event_index+1, to = length(fields))) {
        if(index == length(fields)) {
            # Last field: use selectInput
            filtered_resource_species_register <- get_filtered_resource_species_register(current_input)
            choices <- unique(filtered_resource_species_register[[fields[[index]]]])
            updateSelectInput(session, fields[[index]], choices = choices)
        }
        else {
            # Intermediate selectize fields
            selected <- input[[fields[[index]]]]
            choices <- unique(filtered_resource_species_register[[fields[[index]]]])

            if(!all(selected %in% choices)) {
                selected <- NULL
                current_input[[fields[[index]]]] <- NULL
                filtered_resource_species_register <- get_filtered_resource_species_register(current_input)
                choices <- unique(filtered_resource_species_register[[fields[[index]]]])
            }

            updateSelectizeInput(
                session, fields[[index]], 
                choices = choices,
                selected = selected,
                server = TRUE
            )
        }
    }
}

# ----------------------------------------------------------------------
# Reactive object to store selected resource species
# ----------------------------------------------------------------------
selected_resource_species <- reactiveValues(values = character(0))

# ----------------------------------------------------------------------
# Main server function for Resource Species tab
# ----------------------------------------------------------------------
resource_species_server <- function(input, output, session) {
    # Observe changes in all filter fields and update dependent selectize inputs
    observe({
        lapply(colnames(resource_species_register)[-length(colnames(resource_species_register))], function(field) {
            observeEvent(input[[field]], {
                update_all_selectize_input_resource_species(session, input, field)
            })
        })
    })

    # Handle adding a new resource species
    observeEvent(input$add_resource_species_button, {
        if (!is.null(input$ResourceSpecies) && !input$ResourceSpecies %in% selected_resource_species$values) {
            selected_resource_species$values <- c(selected_resource_species$values, input$ResourceSpecies)

            insertTab(inputId = "tabset_selected_resource_species",
                tabPanel(
                    title = tags$div(
                        tags$span(tags$em(input$ResourceSpecies)),      # Display the species name
                        actionButton(paste0("remove_", input$ResourceSpecies), icon("xmark", style = "color: red;"))  # Botón
                    ),
                    value = input$ResourceSpecies,
                    br(),
                    div(
                        id = paste0("section_parameters_", input$ResourceSpecies),
                        bsCollapsePanel(
                            title = "Parameters",
                            "Contenido de la sección Parameters"
                        )
                    )
                )
            )
        }
    })

    # Handle removal of resource species dynamically
    observe({
        lapply(selected_resource_species$values, function(value) {
            button_id <- paste0("remove_", value)

            # Show confirmation modal when remove button is clicked
            observeEvent(input[[button_id]], {
                showModal(modalDialog(
                    title = "Confirm Deletion",
                    paste("Are you sure you want to delete the resource species \"", value, "\"?"),
                    footer = tagList(
                        modalButton("No"),
                        actionButton(paste0("confirm_remove_", value), "Yes")
                    )
                ))
            }, ignoreInit = TRUE)

            # Confirm deletion and remove the tab
            observeEvent(input[[paste0("confirm_remove_", value)]], {
                removeTab(inputId = "tabset_selected_resource_species", target = value)
                selected_resource_species$values <- selected_resource_species$values[selected_resource_species$values != value]  # Filtra el elemento a eliminar
                removeModal()
            }, ignoreInit = TRUE, once = TRUE)
        })
    })
}
