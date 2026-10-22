#' Launch the Modular Ecosystem Simulation Hub with Adaptive Outward Safeguards
#'
#' @description
#' Initializes the central application hub. Enforces navigation protection exclusively 
#' when exiting active tool workspaces. Inward navigation from the main directory remains seamless.
#'
#' @param ... Additional arguments passed to shiny::runApp.
#' @import shiny
#' @importFrom bslib page_navbar bs_theme card card_header card_footer nav_panel nav_spacer nav_item nav_select page_sidebar sidebar input_dark_mode
#' @export
run_hub <- function(...) {
  
  # ============================================================================
  # 1. USER INTERFACE (UI) REGISTRATION FRAMEWORK
  # ============================================================================
  hub_ui <- bslib::page_navbar(
    title = "Weaver EcoHub",
    id = "main_hub_nav",
    theme = bslib::bs_theme(
      version = 5,
      bootswatch = "minty",
      primary = "#1E3F20"
    ),
    
    # Injection of JavaScript browser tab destruction callback hook
    header = tagList(
      get_shared_hub_styles(),
      tags$head(
      tags$script(
        "Shiny.addCustomMessageHandler('close_browser_window', function(message) {
          window.close();
        });"
      ),
      # Hide standard navbar tabs to enforce strict programmatic navigation routing
      tags$style(".navbar-nav .nav-link { display: none !important; }")
      )
    ),
    
    # Anchor Tab 1: Central Directory (Home)
    bslib::nav_panel(
      title = "Home",
      value = "home_tab",
      icon = icon("leaf"),
      uiOutput("render_home_view")
    ),
    
    # Anchor Tab 2: Simulation Optimiser Compute Stack
    bslib::nav_panel(
      title = "Optimiser Workspace",
      value = "optimiser_tab",
      icon = icon("cogs"),
      fluidPage(
        uiOutput("render_optimiser_module")
      )
    ),
    
    # Anchor Tab 3: Ecosystem Input Parameter Matrix Builder
    bslib::nav_panel(
      title = "Input Manager Workspace",
      value = "input_manager_tab",
      icon = icon("sliders-h"),
      fluidPage(
        uiOutput("render_input_module")
      )
    ),

    # Anchor Tab 4: Obstacle Generation Simulator
    bslib::nav_panel(
      title = "Obstacle Simulator Workspace",
      value = "obstacle_simulator_tab",
      icon = icon("th"),
      fluidPage(
        uiOutput("render_obstacle_simulator_module")
      )
    ),
    
    # Right-aligned globally inherited theme and system lifecycle controls
    bslib::nav_spacer(),
    bslib::nav_item(bslib::input_dark_mode(id = "hub_theme_toggle")),
    bslib::nav_item(actionButton("btn_terminate_hub", "Exit Hub", icon = icon("power-off"), class = "btn-danger btn-sm my-1 ms-2 font-weight-bold"))
  )

  # ============================================================================
  # 2. SERVER ROUTING INTERCEPTORS & LIFECYCLE ENGINE
  # ============================================================================
  hub_server <- function(input, output, session) {
    message("[weaverR] Core ecosystem state infrastructure framework listening.")
    shared_system_volumes <- shinyFiles::getVolumes()()
    shinyFiles::shinyFileChoose(input, "optimization_template_file_picker", roots = shared_system_volumes, session = session, pattern = "^.*\\.[jJ][sS][oO][nN]$")
    shinyFiles::shinyFileChoose(input, "simulator_executable_picker", roots = shared_system_volumes, session = session, pattern = "^.*\\.([eE][xX][eE])$")
    shinyFiles::shinyDirChoose(input, "base_config_picker", roots = shared_system_volumes, session = session)
    observeEvent(input$optimization_template_file_picker, {
      selected <- shinyFiles::parseFilePaths(shared_system_volumes, input$optimization_template_file_picker)
      if (nrow(selected) > 0) updateTextInput(session, "optimization_template_path", value = normalizePath(selected$datapath, mustWork = FALSE))
    })
    observeEvent(input$simulator_executable_picker, {
      selected <- shinyFiles::parseFilePaths(shared_system_volumes, input$simulator_executable_picker)
      if (nrow(selected) > 0) updateTextInput(session, "simulator_executable_path", value = normalizePath(selected$datapath, mustWork = FALSE))
    })
    observeEvent(input$base_config_picker, {
      selected <- shinyFiles::parseDirPath(shared_system_volumes, input$base_config_picker)
      if (length(selected) > 0) updateTextInput(session, "base_config_path", value = normalizePath(selected, mustWork = FALSE))
    })
    
    # Core state machine vectors mapping active panels and tracking target shifts
    active_view <- reactiveVal("home_tab")
    pending_destination <- reactiveVal(NULL)
    pending_optimization_template_validation <- reactiveVal(NULL)
    loaded_optimization_config <- reactiveVal(NULL)
    optimization_variables_validation_in_progress <- reactiveVal(FALSE)
    optimization_variables_validation_generation <- reactiveVal(0L)
    debounced_template_paths <- debounce(reactive(list(
      template = input$optimization_template_path,
      executable = input$simulator_executable_path,
      config = input$base_config_path
    )), millis = 500)

    observeEvent(list(input$optimization_template_path, input$simulator_executable_path, input$base_config_path), {
      optimization_variables_validation_generation(isolate(optimization_variables_validation_generation()) + 1L)
      optimization_variables_validation_in_progress(FALSE)
      pending_optimization_template_validation(NULL)
    }, ignoreInit = TRUE)

    optimization_variables_validation_task <- ExtendedTask$new(function(template_path, executable_path, config_path, request_id) {
      if (!inherits(future::plan(), "multisession")) {
        future::plan(future::multisession)
      }

      schema_loader <- get("load_all_version_schemas", envir = asNamespace("weaverR"), inherits = FALSE)
      schema_filter <- get("filter_schema_catalog_by_parameter_class", envir = asNamespace("weaverR"), inherits = FALSE)
      schema_destinations <- get("resolve_config_destinations_by_schema", envir = asNamespace("weaverR"), inherits = FALSE)
      schema_paths <- get("expand_schema_paths_for_config", envir = asNamespace("weaverR"), inherits = FALSE)
      path_normalizer <- get("normalize_config_relative_path", envir = asNamespace("weaverR"), inherits = FALSE)

      future::future({
        validate_template <- function() {
          or_null <- function(value, default) if (is.null(value)) default else value
          field_errors <- list(optimization_template = NULL, executable = NULL, config = NULL)
          config_errors <- character(0)
          optimization_template <- NULL
          optimization_template_version <- NULL

          if (!nzchar(template_path)) {
            field_errors$optimization_template <- "Select an optimization template JSON file."
          } else if (!grepl("\\.[jJ][sS][oO][nN]$", template_path)) {
            field_errors$optimization_template <- "The optimization template must use the .json extension."
          } else if (!file.exists(template_path)) {
            field_errors$optimization_template <- "The selected optimization template file does not exist."
          } else {
            template_result <- tryCatch(jsonlite::fromJSON(template_path, simplifyVector = FALSE), error = function(e) e)
            if (inherits(template_result, "error")) {
              field_errors$optimization_template <- paste("The optimization template is not valid JSON:", conditionMessage(template_result))
            } else {
              optimization_template <- template_result
              optimization_template_version <- optimization_template$metadata$schema_version
              if (is.null(optimization_template_version) || length(optimization_template_version) != 1L || !nzchar(as.character(optimization_template_version))) {
                field_errors$optimization_template <- "The optimization template does not declare metadata.schema_version."
              }
            }
          }

          executable_version <- NULL
          if (!nzchar(executable_path)) {
            field_errors$executable <- "Select a simulator executable."
          } else if (!grepl("\\.[eE][xX][eE]$", executable_path)) {
            field_errors$executable <- "The simulator executable must use the .exe extension."
          } else if (!file.exists(executable_path)) {
            field_errors$executable <- "The selected simulator executable does not exist."
          } else {
            version_output <- tryCatch({
              result <- system2(executable_path, "--schema_version", stdout = TRUE, stderr = TRUE, timeout = 5)
              status <- attr(result, "status")
              if (!is.null(status) && status != 0L) stop("The executable returned a non-zero status.")
              trimws(paste(result, collapse = " "))
            }, error = function(e) "")
            detected_version <- sub("^[[:space:]]*Schema version:[[:space:]]*", "", version_output, ignore.case = TRUE)
            if (!nzchar(detected_version)) {
              field_errors$executable <- "The executable schema version could not be detected."
            } else {
              executable_version <- detected_version
            }
          }

          if (is.null(field_errors$optimization_template) && !is.null(optimization_template_version) && !is.null(executable_version) &&
              !identical(as.character(optimization_template_version), executable_version)) {
            field_errors$optimization_template <- paste0("Schema version mismatch: template ", optimization_template_version, "; executable ", executable_version, ".")
          }

          config_path_is_valid <- nzchar(config_path) && dir.exists(config_path)
          if (!nzchar(config_path)) {
            field_errors$config <- "Select a base configuration directory."
          } else if (!dir.exists(config_path)) {
            field_errors$config <- "The selected base configuration directory does not exist."
          } else {
            required_dirs <- c("habitat_domain", "moisture", "obstacle", "resource", "resource/patches", "species")
            required_files <- c("simulation_params.json", "landscape_params.json")
            required_csv <- c("ontogeneticLinksPreference.csv", "ontogeneticLinksProfitability.csv")
            for (directory in required_dirs) {
              if (!dir.exists(file.path(config_path, directory))) config_errors <- c(config_errors, paste0("Missing folder: ", directory))
            }
            for (file_name in required_files) {
              if (!file.exists(file.path(config_path, file_name))) config_errors <- c(config_errors, paste0("Missing root file: ", file_name))
            }
            if (dir.exists(file.path(config_path, "species"))) {
              for (file_name in required_csv) {
                if (!file.exists(file.path(config_path, "species", file_name))) config_errors <- c(config_errors, paste0("Missing file: species/", file_name))
              }
            }
            if (length(config_errors) > 0) field_errors$config <- "The base configuration is missing required components."
          }

          config_version_errors <- character(0)
          if (config_path_is_valid && length(config_errors) == 0 && !is.null(executable_version)) {
            config_json_files <- list.files(config_path, pattern = "\\.[jJ][sS][oO][nN]$", recursive = TRUE, full.names = TRUE)
            for (json_file in config_json_files) {
              tryCatch({
                json_data <- jsonlite::fromJSON(json_file, simplifyVector = TRUE)
                file_version <- json_data[["version"]]
                if (!is.null(file_version) && as.character(file_version) != executable_version) {
                  relative_path <- substring(normalizePath(json_file, mustWork = FALSE), nchar(normalizePath(config_path, mustWork = FALSE)) + 2L)
                  config_version_errors <- c(config_version_errors, paste0(relative_path, " [Expected: ", executable_version, "; found: ", file_version, "]"))
                }
              }, error = function(e) {
                config_version_errors <<- c(config_version_errors, paste0(basename(json_file), " [Malformed JSON]"))
              })
            }
            if (length(config_version_errors) > 0) field_errors$config <- "One or more configuration JSON files are malformed or use a different schema version."
          }

          if (any(!vapply(field_errors, is.null, logical(1))) || length(config_errors) > 0 || length(config_version_errors) > 0) {
            return(list(
              valid = FALSE,
              field_errors = field_errors,
              config_errors = c(config_errors, config_version_errors),
              omitted = character(0),
              executable_version = executable_version
            ))
          }

          optimization_config <- optimization_template
          variables <- or_null(optimization_config$optimization_variables_allocation, list())
          catalog <- tryCatch(schema_loader(executable_version, include_basic_strings = FALSE), error = function(e) data.frame())
          coordinate_map <- list()
          supported_parameter_classes <- c("RealValued", "Integer", "Boolean", "Permutation", "Abundances")
          if (!is.null(catalog) && nrow(catalog) > 0) {
            for (parameter_class in supported_parameter_classes) {
              class_catalog <- schema_filter(catalog, parameter_class)
              for (schema_file in unique(class_catalog$schema_file)) {
                destinations <- schema_destinations(schema_file, config_path)
                rows <- class_catalog[class_catalog$schema_file == schema_file, , drop = FALSE]
                for (destination in destinations) {
                  concrete_paths <- schema_paths(rows$json_path, file.path(config_path, destination))
                  if (is.null(coordinate_map[[parameter_class]])) coordinate_map[[parameter_class]] <- list()
                  coordinate_map[[parameter_class]][[destination]] <- unique(c(
                    coordinate_map[[parameter_class]][[destination]],
                    concrete_paths
                  ))
                }
              }
            }
          }

          read_taxon_entries <- function(directory, kind) {
            if (!dir.exists(directory)) return(list())
            json_files <- list.files(directory, pattern = "\\.[jJ][sS][oO][nN]$", full.names = TRUE, recursive = FALSE)
            entries <- lapply(json_files, function(json_file) {
              data <- tryCatch(jsonlite::fromJSON(json_file, simplifyVector = FALSE), error = function(e) NULL)
              if (is.null(data)) return(NULL)
              entity <- if (identical(kind, "animal")) data$animal else data$resource
              name <- entity$name
              if (is.null(name) || length(name) != 1L || is.na(name) || !nzchar(trimws(as.character(name)))) return(NULL)
              instar_count <- if (identical(kind, "animal")) length(entity$individualsPerInstar) else 1L
              list(name = as.character(name), instar_count = max(1L, as.integer(instar_count)))
            })
            entries <- Filter(Negate(is.null), entries)
            if (length(entries) == 0L) return(entries)
            entries[!duplicated(vapply(entries, `[[`, character(1), "name"))]
          }

          predator_entries <- read_taxon_entries(file.path(config_path, "species"), "animal")
          prey_entries <- c(predator_entries, read_taxon_entries(file.path(config_path, "resource"), "resource"))
          predator_names <- if (length(predator_entries)) vapply(predator_entries, `[[`, character(1), "name") else character(0)
          prey_names <- if (length(prey_entries)) vapply(prey_entries, `[[`, character(1), "name") else character(0)
          preference_matrix <- tryCatch(
            read.csv(file.path(config_path, "species", "ontogeneticLinksPreference.csv"), header = TRUE, row.names = 1, sep = ",", check.names = FALSE),
            error = function(e) NULL
          )
          profitability_matrix <- tryCatch(
            read.csv(file.path(config_path, "species", "ontogeneticLinksProfitability.csv"), header = TRUE, row.names = 1, sep = ",", check.names = FALSE),
            error = function(e) NULL
          )

          trophic_cells_are_valid <- function(variable, parameter_class) {
            cells <- variable$cells
            min_bound <- suppressWarnings(as.numeric(unlist(variable$min_bound, use.names = FALSE)))
            max_bound <- suppressWarnings(as.numeric(unlist(variable$max_bound, use.names = FALSE)))
            if (is.null(cells) || length(cells) == 0L || length(min_bound) != 1L || length(max_bound) != 1L ||
                !is.finite(min_bound) || !is.finite(max_bound) || min_bound > max_bound) return(FALSE)
            matrix_data <- if (identical(parameter_class, "Preference")) preference_matrix else profitability_matrix
            if (is.null(matrix_data)) return(FALSE)
            all(vapply(cells, function(cell) {
              if (!is.list(cell)) return(FALSE)
              predator_species <- as.character(unlist(cell$predator_species, use.names = FALSE))
              prey_species <- as.character(unlist(cell$prey_species, use.names = FALSE))
              predator_instars <- suppressWarnings(as.numeric(unlist(cell$predator_instars, use.names = FALSE)))
              prey_instars <- suppressWarnings(as.numeric(unlist(cell$prey_instars, use.names = FALSE)))
              if (length(predator_species) != 1L || is.na(predator_species) || !nzchar(predator_species) ||
                  length(prey_species) != 1L || is.na(prey_species) || !nzchar(prey_species) ||
                  length(predator_instars) == 0L || any(!is.finite(predator_instars)) || any(predator_instars < 1) ||
                  any(predator_instars != floor(predator_instars)) ||
                  length(prey_instars) == 0L || any(!is.finite(prey_instars)) || any(prey_instars < 1) ||
                  any(prey_instars != floor(prey_instars))) return(FALSE)
              predator_index <- match(predator_species, predator_names)
              prey_index <- match(prey_species, prey_names)
              if (is.na(predator_index) || is.na(prey_index) ||
                  any(predator_instars > predator_entries[[predator_index]]$instar_count) ||
                  any(prey_instars > prey_entries[[prey_index]]$instar_count)) return(FALSE)
              row_names <- paste(prey_species, prey_instars, sep = "$")
              column_names <- paste(predator_species, predator_instars, sep = "$")
              all(row_names %in% rownames(matrix_data)) && all(column_names %in% colnames(matrix_data))
            }, logical(1)))
          }

          omitted <- character(0)
          valid_variables <- list()
          for (variable_name in names(variables)) {
            variable <- variables[[variable_name]]
            parameter_class <- as.character(or_null(variable$parameter_class, ""))
            if (parameter_class %in% c("Preference", "Profitability")) {
              valid <- trophic_cells_are_valid(variable, parameter_class)
            } else {
              destinations <- or_null(variable$tied_destinations_coordinates, list())
              class_coordinates <- or_null(coordinate_map[[parameter_class]], list())
              valid <- length(destinations) > 0 && all(vapply(destinations, function(destination) {
                target <- path_normalizer(or_null(destination$target_json_file, ""))
                path <- as.character(or_null(destination$internal_json_path, ""))
                length(target) == 1L && nzchar(target) && length(path) == 1L && nzchar(path) &&
                  !is.null(class_coordinates[[target]]) && path %in% class_coordinates[[target]]
              }, logical(1)))
            }
            if (valid) valid_variables[[variable_name]] <- variable else omitted <- c(omitted, variable_name)
          }

          optimization_config$optimization_variables_allocation <- valid_variables
          optimization_config$.__runtime_paths <- list(executable = executable_path, base_config = config_path)
          list(
            valid = TRUE,
            field_errors = field_errors,
            config_errors = character(0),
            omitted = omitted,
            executable_version = executable_version,
            optimization_config = optimization_config
          )
        }

        validation_result <- tryCatch(
          list(ok = TRUE, validation = validate_template()),
          error = function(error) list(ok = FALSE, error = conditionMessage(error))
        )
        validation_result$request_id <- request_id
        validation_result
      }, seed = TRUE, globals = list(
        template_path = template_path,
        executable_path = executable_path,
        config_path = config_path,
        request_id = request_id,
        schema_loader = schema_loader,
        schema_filter = schema_filter,
        schema_destinations = schema_destinations,
        schema_paths = schema_paths,
        path_normalizer = path_normalizer
      ))
    })
    
    # --- RENDER ENGINE ROUTERS (Forces clean instantiation / absolute state destruction) ---
    output$render_home_view <- renderUI({
      req(active_view() == "home_tab")
      fluidPage(
        div(class = "my-4",
          h2("Ecosystem Simulation Tools Directory"),
          p(class = "text-muted", "Select a specialized module below.")
        ),
        fluidRow(
          column(width = 4,
            bslib::card(
              bslib::card_header(span(icon("cogs"), class = "me-2"), "Simulation Optimiser Engine"),
              p("Calibrate ecological parameters and optimize ecosystem stability models natively using heuristic GA loops."),
              bslib::card_footer(actionButton("btn_goto_optimiser", "Open Workspace", class = "btn-primary w-100"))
            )
          ),
          column(width = 4,
            bslib::card(
              bslib::card_header(span(icon("sliders-h"), class = "me-2"), "Input Configuration Manager"),
              p("Construct environmental baseline constraints, initial population matrices, and abiotical factor files dynamically."),
              bslib::card_footer(actionButton("btn_goto_inputs", "Open Workspace", class = "btn-primary w-100"))
            )
          ),
          column(width = 4,
            bslib::card(
              bslib::card_header(span(icon("th"), class = "me-2"), "Obstacle Generation Simulator"),
              p("Explore how porosity parameters affect the generated obstacle pattern."),
              bslib::card_footer(actionButton("btn_goto_obstacle_simulator", "Open Workspace", class = "btn-primary w-100"))
            )
          )
        )
      )
    })
    
    output$render_optimiser_module <- renderUI({
      req(active_view() == "optimiser_tab")
      mod_simulation_optimiser_ui("optimiser_core_session")
    })
    
    output$render_input_module <- renderUI({
      req(active_view() == "input_manager_tab")
      mod_input_configuration_ui("input_manager_core_session")
    })

    output$render_obstacle_simulator_module <- renderUI({
      req(active_view() == "obstacle_simulator_tab")
      tagList(
        div(class = "d-flex align-items-center justify-content-between w-100 mb-3",
          span("Obstacle Generation Simulator"),
          actionButton("btn_back_from_obstacle_simulator", "Return to Hub", icon = icon("arrow-left"), class = "btn-outline-secondary btn-sm")
        ),
        mod_obstacle_simulator_ui("obstacle_simulator_core_session")
      )
    })
    
    # --- ISOLATED MODULE LIFECYCLE SERVERS INVOCATION ---
    observeEvent(active_view(), {
      if (active_view() == "optimiser_tab") {
        mod_simulation_optimiser_server("optimiser_core_session", initial_optimization_config = loaded_optimization_config(), shared_volumes = shared_system_volumes)
      } else if (active_view() == "input_manager_tab") {
        mod_input_configuration_server("input_manager_core_session")
      } else if (active_view() == "obstacle_simulator_tab") {
        mod_obstacle_simulator_server("obstacle_simulator_core_session")
      }
    }, ignoreInit = TRUE)
    
    # --- INTERCEPT NAVIGATION ATTEMPTS (Enforces conditional confirmation) ---
    route_to_target <- function(target) {
      # If current view is Home, allow seamless transition into any tool without warning
      if (active_view() == "home_tab") {
        message(paste("[Navigation Router] Seamless entry authorized into:", target))
        active_view(target)
        bslib::nav_select(id = "main_hub_nav", selected = target)
      } else {
        # Trigger modal warning only when exiting an active tool workspace
        pending_destination(target)
        showModal(modalDialog(
          title = "Confirm Environment Tab Navigation Swap",
          span(icon("exclamation-triangle"), class = "text-warning me-2"),
          "Warning: Moving to another environment immediately destroys your current workspace. Unsaved configuration metrics or running models parameters will be completely lost.",
          footer = tagList(
            modalButton("Cancel Operation"),
            actionButton("btn_confirm_navigation", "Discard Progress & Move", class = "btn-danger")
          ),
          easyClose = FALSE
        ))
      }
    }
    
    # Navigation Action Directives Mapping
    observeEvent(input$btn_goto_optimiser, {
      showModal(modalDialog(
        title = "Start the Optimiser",
        p("Choose how to initialise the optimisation workspace."),
        footer = div(class = "w-100",
          div(class = "row g-2 mb-2",
            div(class = "col-6",
              actionButton("btn_start_blank_optimiser", "Start from a blank template", class = "btn-outline-primary w-100")
            ),
            div(class = "col-6",
              actionButton("btn_start_existing_optimiser", "Load an existing template", class = "btn-primary w-100")
            )
          ),
          div(class = "d-grid", modalButton("Cancel"))
        ),
        easyClose = FALSE
      ))
    })

    observeEvent(input$btn_start_blank_optimiser, {
      removeModal()
      loaded_optimization_config(NULL)
      route_to_target("optimiser_tab")
    })

    observeEvent(input$btn_start_existing_optimiser, {
      removeModal()
      showModal(modalDialog(
        title = "Load an Optimiser Template",
        tags$style(HTML(".modal-dialog:has(#optimization_template_path) { max-width: 92vw; width: 92vw; } .modal-dialog:has(#optimization_template_path) .modal-content, .modal-dialog:has(#optimization_template_path) .modal-body, .modal-dialog:has(#optimization_template_path) .form-group { width: 100%; max-width: 100%; } .modal-dialog:has(#optimization_template_path) .form-control { width: 100%; } .modal-dialog .optimiser-input-card { border-left: 4px solid var(--bs-primary) !important; margin-bottom: 15px; padding-left: 15px; }")),
        
        div(class = "optimiser-input-card",
          div(class = "mb-2", tags$b("Optimization template JSON path:")),
          textInput("optimization_template_path", label = NULL, value = "", width = "100%"),
          div(class = "mt-1",
            shinyFiles::shinyFilesButton("optimization_template_file_picker", "Browse...", title = "Select Optimization Template JSON", multiple = FALSE, class = "btn-outline-primary w-100")
          ),
          div(class = "mt-3", tags$small(class = "text-muted d-block", "")),
          div(class = "mt-3",
            uiOutput("optimization_template_path_error"),
            uiOutput("display_optimization_template_path")
          )
        ),

        div(class = "optimiser-input-card",
          div(class = "mb-2", tags$b("Simulator executable path:")),
          textInput("simulator_executable_path", label = NULL, value = "", width = "100%"),
          div(class = "mt-1",
            shinyFiles::shinyFilesButton("simulator_executable_picker", "Browse...", title = "Select Simulator Executable", multiple = FALSE, class = "btn-outline-primary w-100")
          ),
          div(class = "mt-3", tags$small(class = "text-muted d-block", "")),
          div(class = "mt-3",
            uiOutput("simulator_executable_error"),
            uiOutput("simulator_schema_status")
          )
        ),

        div(class = "optimiser-input-card",
          div(class = "mb-2", tags$b("Base configuration directory:")),
          textInput("base_config_path", label = NULL, value = "", width = "100%"),
          div(class = "mt-1",
            shinyFiles::shinyDirButton("base_config_picker", "Browse Directory...", title = "Select Base Configuration Directory", class = "btn-outline-primary w-100")
          ),
          div(class = "mt-3", tags$small(class = "text-muted d-block", "")),
          div(class = "mt-3",
            uiOutput("base_config_error"),
            uiOutput("display_base_config_path")
          )
        ),

        div(class = "mt-3 pt-2",
          uiOutput("optimization_template_compatibility_status"),
          uiOutput("optimization_template_validation_loader")
        ),

        footer = tagList(
          modalButton("Cancel"),
          uiOutput("optimization_template_load_controls")
        ),
        easyClose = FALSE
      ))
    })

    observeEvent(list(
      input$optimization_template_path,
      input$simulator_executable_path,
      input$base_config_path
    ), {
      optimization_variables_validation_generation(isolate(optimization_variables_validation_generation()) + 1L)
      optimization_variables_validation_in_progress(FALSE)
      pending_optimization_template_validation(NULL)
    }, ignoreInit = TRUE)

    observeEvent(debounced_template_paths(), {
      template_paths <- debounced_template_paths()
      if (any(vapply(template_paths, is.null, logical(1)))) return()

      request_id <- isolate(optimization_variables_validation_generation()) + 1L
      optimization_variables_validation_generation(request_id)
      optimization_variables_validation_in_progress(FALSE)
      pending_optimization_template_validation(NULL)

      optimization_template_path <- trimws(template_paths$template)
      executable_path <- trimws(template_paths$executable)
      config_path <- trimws(template_paths$config)
      field_errors <- list(optimization_template = NULL, executable = NULL, config = NULL)

      if (!nzchar(optimization_template_path)) field_errors$optimization_template <- "Select an optimization template JSON file."
      if (!nzchar(executable_path)) field_errors$executable <- "Select a simulator executable."
      if (!nzchar(config_path)) field_errors$config <- "Select a base configuration directory."

      if (any(!vapply(field_errors, is.null, logical(1)))) {
        pending_optimization_template_validation(list(
          valid = FALSE,
          field_errors = field_errors,
          config_errors = character(0),
          omitted = character(0)
        ))
        return()
      }

      optimization_variables_validation_in_progress(TRUE)
      tryCatch(
        optimization_variables_validation_task$invoke(
          template_path = optimization_template_path,
          executable_path = executable_path,
          config_path = config_path,
          request_id = request_id
        ),
        error = function(error) {
          optimization_variables_validation_in_progress(FALSE)
          pending_optimization_template_validation(list(
            valid = FALSE,
            field_errors = list(optimization_template = paste("Template validation failed:", conditionMessage(error)), executable = NULL, config = NULL),
            config_errors = character(0),
            omitted = character(0)
          ))
        }
      )
    })

    observeEvent(optimization_variables_validation_task$result(), {
      result <- optimization_variables_validation_task$result()
      if (!identical(result$request_id, isolate(optimization_variables_validation_generation()))) return()

      optimization_variables_validation_in_progress(FALSE)
      if (!isTRUE(result$ok)) {
        pending_optimization_template_validation(list(
          valid = FALSE,
          field_errors = list(optimization_template = paste("Template variable validation failed:", result$error), executable = NULL, config = NULL),
          config_errors = character(0),
          omitted = character(0)
        ))
        return()
      }

      validation <- result$validation
      pending_optimization_template_validation(validation)
    })

    output$display_optimization_template_path <- renderUI({
      state <- pending_optimization_template_validation()
      req(is.null(state$field_errors$optimization_template), nzchar(input$optimization_template_path))
      absolute_path <- normalizePath(path.expand(input$optimization_template_path), winslash = "/", mustWork = TRUE)
      p(tags$code(absolute_path), class = "text-muted small text-break mt-1 mb-0")
    })

    output$display_base_config_path <- renderUI({
      state <- pending_optimization_template_validation()
      req(is.null(state$field_errors$config), nzchar(input$base_config_path))
      absolute_path <- normalizePath(path.expand(input$base_config_path), winslash = "/", mustWork = TRUE)
      p(tags$code(absolute_path), class = "text-muted small text-break mt-1 mb-0")
    })

    output$optimization_template_path_error <- renderUI({
      state <- pending_optimization_template_validation()
      if (is.null(state)) return(NULL)
      error <- state$field_errors$optimization_template
      if (is.null(state) || is.null(error)) return(NULL)
      div(class = "weaver-validation-error", icon("circle-xmark"), error)
    })
    output$simulator_executable_error <- renderUI({
      state <- pending_optimization_template_validation()
      error <- state$field_errors$executable
      if (is.null(state) || is.null(error)) return(NULL)
      div(class = "weaver-validation-error", icon("circle-xmark"), error)
    })
    output$simulator_schema_status <- renderUI({
      state <- pending_optimization_template_validation()
      if (is.null(state) || is.null(state$executable_version)) return(NULL)
      p(class = "weaver-validation-status text-success", icon("circle-check"),
        HTML(paste0(" <b>Schema Compatibility Version:</b> ", tags$span(class = "badge bg-success", state$executable_version))))
    })
    output$base_config_error <- renderUI({
      state <- pending_optimization_template_validation()
      if (is.null(state)) return(NULL)
      errors <- state$config_errors %||% character(0)
      config_error <- state$field_errors$config
      if (is.null(config_error) && length(errors) == 0) return(NULL)
      if (length(errors) > 1L) {
        div(class = "weaver-validation-error",
          p(icon("circle-xmark"), " The directory layout structure breaks mandatory Weaver formats. Missing or invalid elements:"),
          tags$ul(lapply(errors, tags$li)))
      } else {
        div(class = "weaver-validation-error", icon("circle-xmark"), if (length(errors)) errors[[1]] else config_error)
      }
    })
    output$optimization_template_compatibility_status <- renderUI({
      state <- pending_optimization_template_validation()
      if (is.null(state) || !isTRUE(state$valid) || length(state$omitted) == 0) return(NULL)
      div(class = "alert alert-warning",
        p("Some template variables have no valid destination in this base configuration and will be omitted:"),
        tags$ul(lapply(state$omitted, tags$li)))
    })
    output$optimization_template_validation_loader <- renderUI({
      if (!optimization_variables_validation_in_progress()) return(NULL)
      div(
        class = "d-flex align-items-center justify-content-center",
        style = "position: fixed; inset: 0; z-index: 2000; background: rgba(0, 0, 0, 0.45);",
        div(
          class = "bg-body rounded shadow p-4 text-center",
          span(icon("spinner", class = "fa-spin fa-2x text-primary")),
          p(class = "mt-3 mb-0", "Validating template variables...")
        )
      )
    })
    output$optimization_template_load_controls <- renderUI({
      state <- pending_optimization_template_validation()
      if (is.null(state) || !isTRUE(state$valid)) return(NULL)
      label <- if (length(state$omitted) > 0) "Continue with compatible variables" else "Load optimization template"
      actionButton("btn_load_optimization_template", label, class = "btn-primary")
    })

    observeEvent(input$btn_load_optimization_template, {
      validation <- pending_optimization_template_validation()
      req(isTRUE(validation$valid))
      loaded_optimization_config(validation$optimization_config)
      removeModal()
      route_to_target("optimiser_tab")
    })
    observeEvent(input$btn_goto_inputs, { route_to_target("input_manager_tab") })
    observeEvent(input$btn_goto_obstacle_simulator, { route_to_target("obstacle_simulator_tab") })
    observeEvent(input$btn_back_from_optimiser, { route_to_target("home_tab") })
    observeEvent(input$btn_back_from_inputs, { route_to_target("home_tab") })
    observeEvent(input$btn_back_from_obstacle_simulator, { route_to_target("home_tab") })
    
    # Transactional execution commit upon explicit user consent to discard progress
    observeEvent(input$btn_confirm_navigation, {
      req(pending_destination())
      removeModal()
      
      target <- pending_destination()
      message(paste("[Navigation Router] State destruction confirmed. Purging memory space. Swapping to:", target))
      
      active_view(target)
      bslib::nav_select(id = "main_hub_nav", selected = target)
      pending_destination(NULL)
    })
    
    # --- APP SHUTDOWN ORCHESTRATION ---
    observeEvent(input$btn_terminate_hub, {
      message("[Hub Lifecycle] Termination sequence initialized by user trigger.")
      session$sendCustomMessage(type = 'close_browser_window', message = list())
      stopApp()
    })
  }

  # ============================================================================
  # 3. CONTAINER RUN ENGINE INITIALIZATION
  # ============================================================================
  shiny::runApp(appDir = shiny::shinyApp(ui = hub_ui, server = hub_server), ...)
}
