#' Simulation Optimiser Module Server (Refactored to 3-Phase State Machine)
#'
#' @description Orchestrates the simplified 3-phase sequential execution state machine, enforces
#' hardware limits, centralizes system volumes, and maps cross-phase data commits.
#' @param id Unique module namespace identifier string.
#' @import shiny
#' @export
mod_simulation_optimiser_server <- function(id, initial_optimization_config = NULL, shared_volumes) {
  moduleServer(id, function(input, output, session) {
    ns <- session$ns

    if (!is.null(initial_optimization_config)) {
      ga <- initial_optimization_config$genetic_algorithm_settings %||% list()
      environment <- initial_optimization_config$computing_environment %||% list()
      objective_entries <- environment$objectives %||% list()
      objective_ids <- seq_along(objective_entries)
      objective_names <- vapply(objective_entries, function(objective) as.character(objective$name %||% ""), character(1))
      objective_keys <- as.character(objective_ids)
      objective_data <- lapply(seq_along(objective_entries), function(index) {
        objective <- objective_entries[[index]]
        list(
          name = objective_names[index],
          criteria = as.character(objective$criteria %||% "MAX"),
          fitness_func_name = as.character(objective$fitness_func_name %||% ""),
          custom_code_raw = objective$custom_code_raw %||% NULL
        )
      })
      names(objective_data) <- objective_keys
      names(objective_names) <- objective_keys
      priority_ranking <- environment$priority_ranking
      if (!is.null(priority_ranking)) {
        priority_ranking <- as.character(unlist(priority_ranking, use.names = FALSE))
      }
      phase_data_store_initial <- list(
        ga_settings = list(
          population_size = ga$population_size,
          max_generations = ga$max_generations,
          tournament_size = ga$tournament_size,
          crossover_prob = ga$crossover_prob,
          mutation_rate = ga$mutation_rate,
          keep_best = ga$keep_best
        ),
        computing_environment = list(
          fitness_class = environment$fitness_class,
          fitness_objectives = list(ids = objective_ids, names_map = as.list(objective_names), objectives_data = objective_data, code_errors = list()),
          priority_ranking = priority_ranking,
          assigned_weights = environment$assigned_weights %||% list()
        ),
        system_paths = list(
           program_executable = initial_optimization_config$.__runtime_paths$executable %||% NULL,
           base_configuration = initial_optimization_config$.__runtime_paths$base_config %||% NULL,
           detected_schema_version = initial_optimization_config$metadata$schema_version
        )
      )
    } else {
      phase_data_store_initial <- NULL
    }
    
    # Tracks integer validation phase progress status (1 to 3)
    current_phase <- reactiveVal(1) 
    
    # Compute the safe CPU ceiling ONCE upon application startup
    safe_cpu_max_ceiling <- max(1L, as.integer(future::availableCores()) - 1L)

    # Unified operating system mounts loaded strictly once

    # REFACTORED STORAGE SCHEMA: Grouped to match the new 3-phase structure
    phase_data_store <- reactiveValues(
      # Phase 1: General Optimisation parameters
      phase_1 = list(
        ga_settings = list(population_size = NULL, max_generations = NULL, tournament_size = NULL, crossover_prob = NULL, mutation_rate = NULL, keep_best = NULL, is_valid = NULL),
        computing_environment = list(fitness_class = NULL, fitness_objectives = NULL, priority_ranking = NULL, assigned_weights = NULL),
        system_paths = list(program_executable = NULL, base_configuration = NULL, detected_schema_version = NULL)
      ),
      # New Phase 2: Dynamic Tied Optimization Parameters Matrix List
      phase_2 = list(
        defined_parameters = list()
      ),
      # New Phase 3: Launch & View Results (Previous Phase 4)
      phase_3 = list(
        optimization_output = NULL, 
        terminal_execution_logs = NULL,
        execution_success_status = NULL
      )
    )
    if (!is.null(phase_data_store_initial)) {
      phase_data_store$phase_1 <- phase_data_store_initial
      phase_data_store$phase_2$defined_parameters <- initial_optimization_config$optimization_variables_allocation %||% list()
    }
    
    # --- STEPPER WIZARD VISUAL INDICATOR RENDERING (3 STEPS) ---
    output$render_phase_wizard_stepper <- renderUI({
      active <- current_phase()
      div(class = "wizard-steps-list",
        p(class = if(active == 1) "fw-bold text-success" else "text-muted", "1. General Optimisation Settings"),
        p(class = if(active == 2) "fw-bold text-success" else "text-muted", "2. Define Optimization Variables"),
        p(class = if(active == 3) "fw-bold text-success" else "text-muted", "3. Launch & View Results")
      )
    })
    
    # --- PHASE ACTIVE VIEWPORT SWITCHER ---
    output$render_active_phase_ui <- renderUI({
      switch(as.character(current_phase()),
        "1" = mod_optimiser_phase_1_ui(ns("phase_1_instance"), current_state = phase_data_store$phase_1, max_cores_allowed = safe_cpu_max_ceiling),
        "2" = mod_optimiser_phase_2_ui(ns("phase_2_instance")), # New merged view
        "3" = mod_optimiser_phase_3_ui(ns("phase_3_instance"))  # Previous Phase 4 UI
      )
    })
    
    # --- SUB-PHASES CHILD SERVER ENGINES INVOCATION MATRIX ---
    phase_1_outputs <- reactive({
      if (current_phase() == 1) {
        mod_optimiser_phase_1_server("phase_1_instance", global_store = phase_data_store, max_cores_allowed = safe_cpu_max_ceiling, shared_volumes = shared_volumes)
      } else { NULL }
    })

    # Se instancia de forma permanente fija para mantener vivos los observadores del botón add y retener la memoria
    phase_2_outputs_store <- mod_optimiser_phase_2_server(
      id            = "phase_2_instance", 
      global_store  = phase_data_store,
      current_phase = current_phase
    )
    
    phase_2_outputs <- reactive({
      if (current_phase() == 2) {
        phase_2_outputs_store
      } else { NULL }
    })

    # Continuous background worker mapping for the final Launch stage
    phase_3_outputs_store <- mod_optimiser_phase_3_server("phase_3_instance", global_store = phase_data_store, shared_volumes = shared_volumes)
    
    phase_3_outputs <- reactive({
      if (current_phase() == 3) {
        phase_3_outputs_store
      } else { NULL }
    })

    # --- DYNAMIC PROGRAMMATIC NAVIGATION BUTTONS RENDERING ---
    output$render_navigation_buttons_controls <- renderUI({
      prev_btn <- if (current_phase() > 1) {
        actionButton(ns("btn_prev_phase"), "Previous Phase", icon = icon("chevron-left"), class = "btn-outline-primary btn-sm")
      } else {
        actionButton(ns("btn_disabled_prev"), "Previous Phase", class = "btn-outline-secondary btn-sm disabled")
      }
      
      next_btn <- if (current_phase() == 1) {
        is_ready <- !is.null(phase_1_outputs()) && isTRUE(phase_1_outputs()$is_valid())
        if (is_ready) {
          actionButton(ns("btn_next_phase"), "Next Phase", class = "btn-primary btn-sm")
        } else {
          tags$button(id = ns("btn_next_phase_disabled"), type = "button", class = "btn btn-sm", title = "Requirement: Resolve all validation errors above to proceed.", style = "background-color: var(--bs-secondary-bg); color: var(--bs-secondary-color); border-color: var(--bs-border-color); cursor: not-allowed; opacity: 0.65;", "Next Phase")
        }
      } else if (current_phase() == 2) {
        is_ready <- !is.null(phase_2_outputs()) && isTRUE(phase_2_outputs()$is_valid())
        if (is_ready) {
          actionButton(ns("btn_next_phase"), "Next Phase", class = "btn-primary btn-sm")
        } else {
          tags$button(
            id = ns("btn_next_phase_disabled"), 
            type = "button", 
            class = "btn btn-sm", 
            title = "Requirement: Define at least one optimization variable with valid destinations to proceed.", 
            style = "background-color: var(--bs-secondary-bg); color: var(--bs-secondary-color); border-color: var(--bs-border-color); cursor: not-allowed; opacity: 0.65;", 
            "Next Phase"
          )
        }
      } else if (current_phase() == 3) {
        actionButton(ns("btn_disabled_next"), "Next Phase", class = "btn-outline-secondary btn-sm disabled")
      } else {
        actionButton(ns("btn_next_phase"), "Next Phase", class = "btn-primary btn-sm")
      }
      
      div(class = "d-flex align-items-center gap-2", prev_btn, next_btn)
    })

    # ============================================================================
    # TRANSACTIONAL COMMIT ON ADVANCE TRIGGER
    # ============================================================================
    observeEvent(input$btn_next_phase, {
      if (current_phase() == 1) {
        req(phase_1_outputs())
        message("[Orchestrator] Invoking Phase 1 transactional commit...")
        phase_1_outputs()$commit_data()
      } else if (current_phase() == 2) {
        req(phase_2_outputs())
        message("[Orchestrator] Invoking Phase 2 merged configuration commit...")
        phase_2_outputs()$commit_data()
      }
      
      if (current_phase() < 3) {
        current_phase(current_phase() + 1)
      }
    })
    
    observeEvent(input$btn_prev_phase, {
      if (current_phase() > 1) {
        current_phase(current_phase() - 1)
      }
    })
  })
}
