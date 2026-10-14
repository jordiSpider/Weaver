ui_environment_computing_card <- card(
  card_header(tags$b("Environment & Parallel Computing")),
  card_body(
    # Shared CSS is injected once by the orchestrator / main.R header.

    # 1. Primary fitness strategy selector
    selectInput("fitness_class", tags$b("Fitness Class:"), 
                choices = c("MultiObjectiveFitness", "WeightedFitness")),
    hr(),

    # 2. Dynamic objectives section
    tags$div(
      class = "d-flex justify-content-between align-items-center mb-2",
      tags$span(tags$b("Defined Fitness Objectives:")),
      actionButton("add_objective_btn", "Add Objective", icon = icon("plus"), class = "btn-sm btn-primary")
    ),

    # Static anchor container for independent insertUI() injections
    tags$div(id = "objectives_anchor"),

    hr(),

    # 3. Strategy-specific section (weights or re-orderable hierarchy)
    uiOutput("fitness_strategy_specific_ui"),

    hr(),

    # --- Original hardware and seed configuration ---
    div(class = "integer-input",
        numericInput("seed", tags$b("Random Seed:"), value = 0, min = 0, step = 1)
    ),
    div(class = "integer-input",
        numericInput("program_cores", tags$b("Cores per Simulation Instance:"), value = 4, min = 1, step = 1)
    ),
    switchInput("total_cores_auto", tags$b("Auto-detect Total System Cores"), value = TRUE),
    conditionalPanel(
      condition = "input.total_cores_auto == false",
      div(class = "integer-input",
          numericInput("total_cores", tags$b("Total Allocated Cores:"), value = 8, min = 1, step = 1)
      )
    ),
    switchInput("debug", tags$b("Debug Mode"), value = FALSE)
  )
)