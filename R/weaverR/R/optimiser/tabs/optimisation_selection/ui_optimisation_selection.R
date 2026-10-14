library(shiny)
library(bslib)
library(shinyTree)

optimisation_selection_ui <- nav_panel(
  title = "2. Optimisation Selection",
  icon = icon("list-check"),
  value = "tab_selection",
  
  card(
    full_screen = TRUE,
    card_header(
      div(class = "d-flex justify-content-between align-items-center",
          span(icon("sliders"), " Target Parameters Exploration & Selection"),
          span(class = "badge bg-secondary", "Dynamic JSON Schema Extractor")
      )
    ),
    card_body(
      p("Explore the parameters extracted automatically from the schema corresponding to your execution version. Select the components you want to actively target and optimize:"),
      hr(),
      
      # We only keep the Hierarchical Tree, directly and cleanly
      div(class = "mt-2",
          p(tags$small(class = "text-muted", "Expand nodes to view the inner properties structure. Select elements to configure them in the next tab.")),
          shinyTree("tree_parameters", checkbox = TRUE, search = TRUE)
      )
    )
  )
)