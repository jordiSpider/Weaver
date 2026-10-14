#' Configuration Setup Tab UI for the Configuration Generator App
#'
#' This script defines the **UI for the "Configuration Setup" tab** in the Shiny
#' Configuration Generator application. The tab allows users to enter configuration
#' metadata, choose a save directory, and generate configuration files.
#'
#' @details
#' The tab uses a `tabPanel` containing:
#' - A cog icon and the title "Configuration Setup".
#' - Text inputs for entering the configuration name and version.
#' - A shinyDirButton for selecting the save directory, with the selected path
#'   displayed using `verbatimTextOutput`.
#' - Two action buttons:
#'   1. "Generate Configuration" (primary style) to generate the configuration.
#'   2. "Generate Configuration with Bibliography" (info style) to generate
#'      a configuration including bibliography information.
#'
#' @import shiny
#' @import shinyFiles
#' @importFrom shiny.tags tags
#' @importFrom shiny.icon icon
#' @export
#'
#' @examples
#' \dontrun{
#' # Include this tab in a Shiny tabsetPanel:
#' ui <- fluidPage(
#'   tabsetPanel(configuration_ui)
#' )
#' }

library(shinyFiles)

configuration_ui <- tabPanel(
    # Tab title with a cog icon
    tags$div(icon("cog"), " Configuration Setup"),  # Ícono en el título

    # Add a line break
    tags$br(),

    # Text input for configuration name
    textInput("config_name", "Configuration Name:", placeholder = "Enter the name of the configuration"),

    # Text input for version
    textInput("version", "Version (YYYY.MM.DD):", placeholder = "Enter the version of the configuration"),

    # Directory selection section
    tags$label("Select Save Directory:"),
    fluidRow(
        column(1, shinyDirButton("save_directory_chooser", label = "Choose Folder", title = "Select Directory")),
        column(11, verbatimTextOutput("save_directory_selected", placeholder=TRUE))
    ),

    # Add a line break
    tags$br(),

    # Action buttons to generate configuration
    fluidRow(
        column(
            12, 
            actionButton(
                "generate_config", 
                "Generate Configuration", 
                icon = icon("cogs"), 
                class = "btn-primary"
            ), 
            actionButton(
                "generate_config_bib", 
                "Generate Configuration with Bibliography", 
                icon = icon("file-alt"), 
                class = "btn-info"
            )
        )
    )
)
