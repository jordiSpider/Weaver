library(shiny)
library(bslib)
library(shinyWidgets)
library(shinyFiles) 

global_parameters_ui <- nav_panel(
  title = "1. Global Parameters",
  icon = icon("gear"),
  value = "tab_global_parameters",
  
  # Contenedor para inyectar de golpe el panel de alertas de Bootstrap
  uiOutput("global_errors_container"),
  
  layout_columns(
    col_widths = c(12, 12, 12),
    fill = FALSE, 
    
    ui_genetic_algorithm_card,
    ui_environment_computing_card,
    ui_system_paths_card
  )
)