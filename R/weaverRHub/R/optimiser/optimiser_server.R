#' Simulation Optimiser Module Server (Refactored to 3-Phase State Machine)
#'
#' @description Orchestrates the simplified 3-phase sequential execution state machine, enforces
#' hardware limits, centralizes system volumes, and maps cross-phase data commits.
#' @param id Unique module namespace identifier string.
#' @import shiny
#' @export
mod_simulation_optimiser_server <- function(id) {
  moduleServer(id, function(input, output, session) {
    ns <- session$ns
    
    # Tracks integer validation phase progress status (1 to 3)
    current_phase <- reactiveVal(1) 
    
    # Compute the safe CPU ceiling ONCE upon application startup
    safe_cpu_max_ceiling <- max(1L, as.integer(future::availableCores()) - 1L)

    # Unified operating system mounts loaded strictly once
    shared_system_volumes <- shinyFiles::getVolumes()()

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
        mod_optimiser_phase_1_server("phase_1_instance", global_store = phase_data_store, max_cores_allowed = safe_cpu_max_ceiling, shared_volumes = shared_system_volumes)
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
    phase_3_outputs_store <- mod_optimiser_phase_3_server("phase_3_instance", global_store = phase_data_store, shared_volumes = shared_system_volumes)
    
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
      
      div(class = "d-flex justify-content-between w-100", prev_btn, next_btn)
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