#' Animal Species Server Logic
#'
#' This file contains the server-side logic for the "Animal Species" tab in the Weaver Configuration Generator Shiny app.
#' It handles taxonomic filtering, dynamic updating of selectize inputs, growth curve management, and adding/removing species tabs.

library(dplyr)      # For filtering data frames
library(shinyBS)    # For collapsible panels
library(plotly)     # For plotting growth curves interactively

# Source script to obtain animal species information and growth curves
source(file.path(getwd(), "data", "parametrisation", "utilities", "codes", "functions", "run_animal_species_info_script.r"))


#' Filter Animal Species Register
#'
#' Filters the global `animal_species_register` dataframe based on the selected taxonomic inputs.
#'
#' @param input The Shiny input list.
#' @return A filtered data frame containing only rows that match selected taxonomic fields.
get_filtered_animal_species_register <- function(input) {
    df <- animal_species_register

    if (!is.null(input$Phylum) && length(input$Phylum) > 0) {
        df <- df %>% filter(Phylum %in% input$Phylum)
    }
    if (!is.null(input$Class) && length(input$Class) > 0) {
        df <- df %>% filter(Class %in% input$Class)
    }
    if (!is.null(input$Order) && length(input$Order) > 0) {
        df <- df %>% filter(Order %in% input$Order)
    }
    if (!is.null(input$Family) && length(input$Family) > 0) {
        df <- df %>% filter(Family %in% input$Family)
    }
    if (!is.null(input$Genus) && length(input$Genus) > 0) {
        df <- df %>% filter(Genus %in% input$Genus)
    }

    return(df)
}


#' Update All Selectize Inputs Dynamically
#'
#' Updates downstream taxonomic selectize inputs whenever a parent field changes.
#'
#' @param session The Shiny session object.
#' @param input The Shiny input list.
#' @param field_event The name of the field that triggered the update.
update_all_selectize_input_animal_species <- function(session, input, field_event) {
    fields <- colnames(animal_species_register)
    field_event_index <- which(fields == field_event)
    current_input <- reactiveValuesToList(input) 
    filtered_animal_species_register <- get_filtered_animal_species_register(current_input)

    for(index in seq(from = field_event_index+1, to = length(fields))) {
        if(index == length(fields)) {
            # Last field uses selectInput instead of selectizeInput
            filtered_animal_species_register <- get_filtered_animal_species_register(current_input)
            choices <- unique(filtered_animal_species_register[[fields[[index]]]])
            updateSelectInput(session, fields[[index]], choices = choices)
        }
        else {
            selected <- input[[fields[[index]]]]
            choices <- unique(filtered_animal_species_register[[fields[[index]]]])

            if(!all(selected %in% choices)) {
                selected <- NULL
                current_input[[fields[[index]]]] <- NULL
                filtered_animal_species_register <- get_filtered_animal_species_register(current_input)
                choices <- unique(filtered_animal_species_register[[fields[[index]]]])
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


#' Generate Growth Curve Plots for a Species
#'
#' Runs the species info script to obtain growth curves for all temperatures.
#'
#' @param species Character name of the animal species.
#' @return A list of growth curve plots indexed by temperature.
generate_animal_species_growth_curve_plots <- function(species) {
    env <- new.env()
    env$app <- list()
    env <- run_animal_species_info_script(species, env)
    return(env$growth_curve_plots)
}


# ReactiveValues to store selected animal species
selected_animal_species <- reactiveValues(values = character(0))

# ReactiveValues to store growth curve plots for each species
species_growth_curve_plots <- reactiveValues()


#' Animal Species Server Module
#'
#' Manages the server logic for the Animal Species tab, including:
#' - Filtering species by taxonomy
#' - Adding selected species as tabs
#' - Generating dynamic UI for growth curves and parameters
#' - Removing species tabs
#'
#' @param input Shiny input object.
#' @param output Shiny output object.
#' @param session Shiny session object.
#' @export
animal_species_server <- function(input, output, session) {
    
    # Observe taxonomic field changes to update downstream selectize inputs
    observe({
        lapply(colnames(animal_species_register)[-length(colnames(animal_species_register))], function(field) {
            observeEvent(input[[field]], {
                update_all_selectize_input_animal_species(session, input, field)
            })
        })
    })

    # Add selected animal species
    observeEvent(input$add_animal_species_button, {
        if (!is.null(input$AnimalSpecies) && !input$AnimalSpecies %in% selected_animal_species$values) {
            selected_animal_species$values <- c(selected_animal_species$values, input$AnimalSpecies)
            species_growth_curve_plots[[input$AnimalSpecies]] <- generate_animal_species_growth_curve_plots(input$AnimalSpecies)

            # Create dynamic sub-tabs for each temperature
            temp_tabs <- lapply(names(species_growth_curve_plots[[input$AnimalSpecies]]), function(temp) {
                tabPanel(
                    title = temp,
                    value = temp,
                    br(),
                    uiOutput(paste0("growth_curve_selector_", input$AnimalSpecies, "_", temp)),  # Selector dinámico de gráficas
                    plotlyOutput(paste0("plot_growth_curve_", input$AnimalSpecies, "_", temp))  # Salida de la gráfica
                )
            })

            # Insert main species tab with collapsible panels for Parameters and Growth Curve
            insertTab(inputId = "tabset_selected_animal_species",
                tabPanel(
                    title = tags$div(
                        tags$span(tags$em(input$AnimalSpecies)),
                        actionButton(paste0("remove_", input$AnimalSpecies), icon("xmark", style = "color: red;"))  # Botón
                    ),
                    value = input$AnimalSpecies,
                    br(),
                    div(
                        id = paste0("section_parameters_", input$AnimalSpecies),
                        bsCollapsePanel(
                            title = "Parameters",
                            "Contenido de la sección Parameters"
                        )
                    ),
                    div(
                        id = paste0("section_growth_curve_", input$AnimalSpecies),
                        bsCollapsePanel(
                            title = "Growth Curve",
                            do.call(tabsetPanel, c(list(id = paste0("tabset_temperature_", input$AnimalSpecies)), temp_tabs))
                        )
                    )
                )
            )
        }
    })

    # Render dynamic growth curve selectors and plots
    observe({
        lapply(names(species_growth_curve_plots), function(species) {
            lapply(names(species_growth_curve_plots[[species]]), function(temp) {
                output[[paste0("growth_curve_selector_", species, "_", temp)]] <- renderUI({
                    selectInput(paste0("growth_curve_selected_", species, "_", temp), "Select growth curve:", choices = c("", names(species_growth_curve_plots[[species]][[temp]])), selected="")
                })

                output[[paste0("plot_growth_curve_", species, "_", temp)]] <- renderPlotly({
                    growth_curve_selected <- input[[paste0("growth_curve_selected_", species, "_", temp)]]

                    if (!is.null(growth_curve_selected) && growth_curve_selected != "") {
                        species_growth_curve_plots[[species]][[temp]][[growth_curve_selected]]
                    }
                })
            })
        })
    })

    # Observe remove buttons to delete species tabs
    observe({
        lapply(selected_animal_species$values, function(value) {
            button_id <- paste0("remove_", value)

            observeEvent(input[[button_id]], {
                showModal(modalDialog(
                    title = "Confirm Deletion",
                    paste("Are you sure you want to delete the animal species \"", value, "\"?"),
                    footer = tagList(
                        modalButton("No"),
                        actionButton(paste0("confirm_remove_", value), "Yes")
                    )
                ))
            }, ignoreInit = TRUE)

            observeEvent(input[[paste0("confirm_remove_", value)]], {
                removeTab(inputId = "tabset_selected_animal_species", target = value)
                selected_animal_species$values <- selected_animal_species$values[selected_animal_species$values != value]  # Filtra el elemento a eliminar
                remove(species_growth_curve_plots[[value]], envir = species_growth_curve_plots)  # Elimina la especie de species_growth_curve_plots
                removeModal()
            }, ignoreInit = TRUE, once = TRUE)
        })
    })
}
