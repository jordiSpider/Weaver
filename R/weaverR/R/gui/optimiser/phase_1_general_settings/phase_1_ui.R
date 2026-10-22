#' Phase 1: Define General Optimisation Parameters UI (State Persistence Enabled)
#'
#' @description Layout matrix displaying forms and reserving clean reactive rendering slots.
#' Dynamically binds current session data matrix states to prevent field resets upon tab shifts.
#' @param id Unique module namespace identifier string.
#' @param current_state List mapping pre-existing cached parameters from phase_data_store$phase_1.
#' @param max_cores_allowed The maximum number of CPU cores allowed for parallel processing.
#' @import shiny
#' @importFrom bslib card card_header card_body layout_columns
#' @importFrom shinyWidgets switchInput
#' @importFrom shinyFiles shinyFilesButton shinyDirButton
#' @export
mod_optimiser_phase_1_ui <- function(id, current_state, max_cores_allowed) {
  ns <- NS(id)
  
  # --- EXTRACT OR FALLBACK TO SCIENTIFIC DEFAULT VALUES ---
  # Safely checks if previous data vectors exist in cache memory, avoiding null object pointer leakage
  ga_cache   <- current_state$ga_settings
  comp_cache <- current_state$computing_environment
  path_cache <- current_state$system_paths
  
  val_pop        <- if (!is.null(ga_cache$population_size)) ga_cache$population_size else 2L
  val_max_gen    <- if (!is.null(ga_cache$max_generations)) ga_cache$max_generations else 1L
  val_tour       <- if (!is.null(ga_cache$tournament_size)) ga_cache$tournament_size else 2L
  val_cross      <- if (!is.null(ga_cache$crossover_prob)) ga_cache$crossover_prob else 0.8
  val_mut        <- if (!is.null(ga_cache$mutation_rate)) ga_cache$mutation_rate else 0.05
  val_keep       <- if (!is.null(ga_cache$keep_best)) ga_cache$keep_best else FALSE
  
  val_fit_class  <- if (!is.null(comp_cache$fitness_class)) comp_cache$fitness_class else "MultiObjectiveFitness"

  val_cores_tot  <- if (!is.null(comp_cache$total_cores)) comp_cache$total_cores else max_cores_allowed
  
  val_path_exec  <- if (!is.null(path_cache$program_executable)) path_cache$program_executable else ""
  val_path_conf  <- if (!is.null(path_cache$base_configuration)) path_cache$base_configuration else ""
  val_path_out   <- if (!is.null(path_cache$optimization_output)) path_cache$optimization_output else ""

  tags$div(id = ns("phase_1_workspace_container"),
    fluidPage(
      h5("Phase 1: General Optimisation Settings"),
      p(class = "text-muted", "Configure algorithm metrics, computational boundaries, and local environment files vectors."),
      hr(),
      
      # Consolidated Phase General Errors Panel Block Bar
      uiOutput(ns("display_phase_general_errors")),
      
      bslib::layout_columns(
        col_widths = c(12, 12, 12),
        fill = FALSE,
        
        # CARD 1: GENETIC ALGORITHM PARAMETERS
        bslib::card(
          bslib::card_header(tags$b("Genetic Algorithm Parameters")),
          bslib::card_body(
            div(class = "integer-input optimiser-input-card",
                numericInput(ns("population_size"), tags$b("Population Size:"), value = val_pop, min = 2, step = 1),
                tags$small(class = "text-muted d-block", "Total number of candidate solutions generated per generation."),
                uiOutput(ns("error_local_population_size"))
            ),
            div(class = "integer-input optimiser-input-card",
                numericInput(ns("max_generations"), tags$b("Max Generations:"), value = val_max_gen, min = 1, step = 1),
                tags$small(class = "text-muted d-block", "The maximum limit of iterations before the optimization process stops.")
            ),
            div(class = "integer-input optimiser-input-card",
                numericInput(ns("tournament_size"), tags$b("Tournament Size:"), value = val_tour, min = 2, step = 1),
                tags$small(class = "text-muted d-block", "Number of individuals randomly chosen to compete for selection."),
                uiOutput(ns("error_local_tournament_size"))
            ),
            
            # Crossover Probability Layout
            div(class = "optimiser-input-card",
                tags$b("Crossover Probability:"),
                div(class = "d-flex align-items-center gap-3 mt-1",
                    div(style = "flex: 1 1 0%; min-width: 0;",
                        sliderInput(ns("crossover_prob"), label = NULL, min = 0, max = 1, value = val_cross, step = 1e-6, width = "100%")
                    ),
                    div(style = "width: 150px; flex: 0 0 150px;", class = "decimal-input",
                        numericInput(ns("crossover_prob_num"), label = NULL, value = val_cross, min = 0, max = 1, step = "any")
                    )
                ),
                tags$small(class = "text-muted d-block", "The probability that two parent solutions will exchange data to generate a new offspring.")
            ),
            
            # Mutation Rate Layout
            div(class = "optimiser-input-card",
                tags$b("Mutation Rate:"),
                div(class = "d-flex align-items-center gap-3 mt-1",
                    div(style = "flex: 1 1 0%; min-width: 0;",
                        sliderInput(ns("mutation_rate"), label = NULL, min = 0, max = 1, value = val_mut, step = 1e-6, width = "100%")
                    ),
                    div(style = "width: 150px; flex: 0 0 150px;", class = "decimal-input",
                        numericInput(ns("mutation_rate_num"), label = NULL, value = val_mut, min = 0, max = 1, step = "any")
                    )
                ),
                tags$small(class = "text-muted d-block", "The probability of introducing random changes into a solution to maintain genetic diversity.")
            ),

            div(class = "optimiser-input-card",
                tags$b("Keep Best Individual:"),
                div(class = "d-flex align-items-center gap-3 mt-1",
                    div(style = "flex: 1 1 0%; min-width: 0;",
                        shinyWidgets::switchInput(ns("keepBest"), label = NULL, value = val_keep, inline = TRUE)
                    )
                ),
                tags$small(class = "text-muted d-block", "Elitism strategy. Guarantees the best individual from the current generation survives unchanged into the next.")
            )
          )
        ),
        
        # CARD 2: ENVIRONMENT & PARALLEL COMPUTING
        bslib::card(
          bslib::card_header(tags$b("Environment & Parallel Computing")),
          bslib::card_body(
            div(class = "optimiser-input-card",
                selectInput(ns("fitness_class"), tags$b("Fitness Class:"), choices = c("MultiObjectiveFitness", "WeightedFitness"), selected = val_fit_class),
                tags$small(class = "text-muted d-block", "Defines the evaluation methodology used to measure and score solution performance.")
            ),
            hr(),
            tags$div(class = "d-flex justify-content-between align-items-center",
                     tags$span(tags$b("Defined Fitness Objectives:")),
                     actionButton(ns("add_objective_btn"), "Add Objective", icon = icon("plus"), class = "btn-sm btn-primary")
            ),
            tags$small(class = "text-muted d-block", "Targets and metrics that the algorithm will actively try to maximize or minimize."),
            uiOutput(ns("error_local_zero_objectives")),
            
            tags$div(id = ns("objectives_anchor")),
            hr(),
            uiOutput(ns("fitness_strategy_specific_ui")),
            uiOutput(ns("display_fitness_errors"))
          )
        ),
        
        # CARD 3: SYSTEM ENVIRONMENT PATHS
        bslib::card(
          bslib::card_header(tags$b("System Environment Paths")),
          bslib::card_body(
            div(class = "optimiser-input-card",
                div(class = "mb-2", tags$b("Simulator Executable Path:")),
                textInput(ns("program_text_path"), label = NULL, value = val_path_exec, placeholder = "Enter absolute or relative path to the executable...", width = "100%"),
                div(class = "mt-1",
                    shinyFiles::shinyFilesButton(ns("program_file"), "Browse...", title = "Select Simulator Executable (WeaverCLI.exe)", multiple = FALSE, class = "btn-outline-primary w-100")
                ),
                div(class = "mt-3", tags$small(class = "text-muted d-block", "The local file path pointing to the simulator core engine runner.")),
                div(class = "mt-3",
                  uiOutput(ns("error_local_program_text_path")),
                  htmlOutput(ns("schema_version_status"))
                )
            ),

            div(class = "optimiser-input-card",
                div(class = "mb-2", tags$b("Base Configuration Directory:")),
                textInput(ns("base_config_text_path"), label = NULL, value = val_path_conf, placeholder = "Enter absolute or relative path to base configuration...", width = "100%"),
                div(class = "mt-1",
                    shinyFiles::shinyDirButton(ns("base_config_dir"), "Browse Directory...", title = "Select Base Configuration Directory", class = "btn-outline-primary w-100")
                ),
                div(class = "mt-3", tags$small(class = "text-muted d-block", "The location of the folder containing the simulator’s start-up settings.")),
                div(class = "mt-3",
                  uiOutput(ns("error_local_base_config_text_path")),
                  uiOutput(ns("display_base_config_path"))
                )
            )
          )
        )
      ),

      uiOutput(ns("local_client_scripts"))
    )
  )
}
