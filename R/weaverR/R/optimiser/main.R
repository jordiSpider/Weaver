library(shiny)
library(bslib)
library(shinyjs)

# =========================================================================
# INTERNAL COMPILED METHOD BINDINGS
# =========================================================================

.internal_optimiser_run_execution <- function(...) {
  engine <- .get_internal_optimiser_engine()
  do.call(engine$run, list(...))
}

#' Build the optimiser page_navbar UI
#'
#' Shared by the standalone app and by host applications (e.g. the HUB) that
#' want to embed the optimiser as a sub-view instead of a separate process.
#'
#' @param tool_env environment produced by `.load_all_tool_submodules()`
#' @param show_exit_button whether to render the built-in "Exit Program" button
#'   (hosts that provide their own navigation/exit affordance can disable it)
#' @param with_loading_overlay whether to include the boot-time loading overlay
.internal_optimiser_build_ui <- function(tool_env, show_exit_button = TRUE, with_loading_overlay = TRUE) {
  header_tags <- tagList(
    useShinyjs(),
    tags$head(optimiser_shared_css())
  )

  if (with_loading_overlay) {
    header_tags <- tagList(
      header_tags,
      tags$div(
        id = "loading-overlay",
        tags$div(class = "spinner-loader"),
        tags$div(class = "loading-text", "Validating environment parameters...")
      )
    )
  }

  page_navbar(
    id = "main_navbar",
    title = "Weaver Optimiser Control Panel",
    theme = bs_theme(version = 5, bootswatch = "flatly"),
    header = header_tags,

    # Direct references to the UI objects loaded from the tabs/ subfolders
    tool_env$global_parameters_ui,
    tool_env$optimisation_selection_ui,
    tool_env$parameter_specifics_ui,
    tool_env$execution_outputs_ui,

    nav_spacer(),
    if (show_exit_button) {
      nav_item(
        actionButton(inputId = "btn_exit", label = "Exit Program", class = "btn-danger btn-sm", icon = icon("power-off"), style = "margin-top: 4px;")
      )
    }
  )
}

.internal_optimiser_ui_execution <- function(...) {
  # Build an isolated environment to compile this tool's submodules
  tool_env <- new.env(parent = globalenv())
  .load_all_tool_submodules(tool_env)

  ui_app <- .internal_optimiser_build_ui(tool_env, show_exit_button = TRUE, with_loading_overlay = TRUE)

  server_app <- function(input, output, session) {
    .build_optimiser_orchestrator(input, output, session, tool_env)
  }

  shiny::shinyApp(ui = ui_app, server = server_app, ...)
}

.build_optimiser_orchestrator <- function(input, output, session, tool_env, on_exit = NULL) {
    paths <- reactiveValues(program = "", base_config_path = "", output_path = "")
    active_selected_ids <- reactiveVal(character(0))
    selection_reset_nonce <- reactiveVal(0)

    # The per-tab server files (sourced into tool_env by .load_all_tool_submodules())
    # contain top-level reactive constructs (observeEvent, reactive, output$<- ) that
    # reference `input`, `output`, `session`, `paths`, `active_selected_ids` and
    # `selection_reset_nonce` as free variables. Since tool_env's enclosing scope is
    # globalenv(), those names must be bound directly inside tool_env before the
    # reactive graph flushes, otherwise they fail at runtime with
    # "object 'input' not found" style errors.
    tool_env$input <- input
    tool_env$output <- output
    tool_env$session <- session
    tool_env$paths <- paths
    tool_env$active_selected_ids <- active_selected_ids
    tool_env$selection_reset_nonce <- selection_reset_nonce

    # Clean, in-memory invocation of each tab's server logic
    if (!is.null(tool_env$server_global_parameters_logic)) tool_env$server_global_parameters_logic(input, output, session)
    if (!is.null(tool_env$server_optimisation_selection_logic)) tool_env$server_optimisation_selection_logic(input, output, session)
    if (!is.null(tool_env$server_parameter_specifics_logic)) tool_env$server_parameter_specifics_logic(input, output, session)
    if (!is.null(tool_env$server_execution_outputs_logic)) tool_env$server_execution_outputs_logic(input, output, session)

    global_errors_list <- reactive({
      errors <- list()
      ga_status      <- ga_validation_status()
      fitness_status <- fitness_validation_status()
      cores_status   <- cores_validation_status()
      struct_status  <- weaver_structure_status()

      if (!ga_status$valid) errors[["Population & Tournament Size"]] <- ga_status$message
      if (!fitness_status$valid) errors[["Fitness Logic & Weights"]] <- fitness_status$message

      if (!is.null(input$fitness_class) && input$fitness_class == "WeightedFitness") {
        ids <- inserted_objectives$ids
        if (length(ids) > 0) {
          obj_names <- unlist(lapply(ids, function(id) inserted_objectives$names_map[[as.character(id)]]))
          vals <- numeric(length(obj_names))
          is_empty <- FALSE
          for (i in seq_along(obj_names)) {
            w_val <- input[[paste0("weight_for_", obj_names[i])]]
            if (is.null(w_val) || is.na(w_val)) is_empty <- TRUE else vals[i] <- w_val
          }
          if (is_empty) {
            errors[["Strategy Coefficients (Weights)"]] <- "All strategy coefficients must be filled with a valid numeric value."
          } else {
            total_sum <- sum(vals)
            if (abs(total_sum - 1.0) > 1e-5) errors[["Strategy Coefficients (Weights)"]] <- paste0("The sum of the coefficients is ", round(total_sum, 4), ". It must equal exactly 1.0.")
          }
        }
      }

      if (!cores_status$valid) errors[["Cores per Instance Allocation"]] <- cores_status$message

      if (paths$program == "") {
        errors[["Simulator Executable Path"]] <- "No file selected."
      } else if (!grepl("\\.[eE][xX][eE]$", paths$program)) {
        errors[["Simulator Executable Path"]] <- "Invalid executable (.exe) file."
      }

      if (paths$base_config_path == "") {
        errors[["Base Configuration Directory"]] <- "No directory selected."
      } else if (!dir.exists(paths$base_config_path)) {
        errors[["Base Configuration Directory"]] <- "Directory does not exist."
      } else if (!struct_status$valid) {
        errors[["Weaver Structural Format"]] <- "Missing mandatory assets in directory."
      }

      if (paths$output_path == "") {
        errors[["Optimization Output Directory"]] <- "No directory selected."
      } else if (!dir.exists(paths$output_path)) {
        errors[["Optimization Output Directory"]] <- "Directory does not exist."
      }

      return(errors)
    })

    global_has_errors <- reactive({ length(global_errors_list()) > 0 })

    output$global_errors_container <- renderUI({
      err_list <- global_errors_list()
      if (length(err_list) == 0) return(NULL)
      error_items_ui <- lapply(names(err_list), function(p) tags$li(tags$b(paste0(p, ": ")), tags$span(err_list[[p]])))
      div(class = "alert alert-danger shadow-sm mb-4", tags$ul(error_items_ui))
    })

    observeEvent(input$btn_exit, {
      if (is.function(on_exit)) {
        on_exit()
      } else {
        shinyjs::runjs("window.open('', '_self'); window.close();")
        stopApp()
      }
    })

    observe({
      if (global_has_errors() && length(active_selected_ids()) > 0) {
        active_selected_ids(character(0))
        shiny::isolate({ selection_reset_nonce(selection_reset_nonce() + 1) })
      }
    })

    observe({
      if (global_has_errors()) {
        shinyjs::disable(selector = '#main_navbar li a[data-value="tab_selection"]')
        if (identical(input$main_navbar, "tab_selection")) updateNavbarPage(session, "main_navbar", selected = "1. Global Parameters")
      } else {
        shinyjs::enable(selector = '#main_navbar li a[data-value="tab_selection"]')
      }
    })

    observe({
      if (length(active_selected_ids()) == 0) {
        shinyjs::disable(selector = '#main_navbar li a[data-value="tab_specifics"]')
        if (identical(input$main_navbar, "tab_specifics")) updateNavbarPage(session, "main_navbar", selected = "tab_selection")
      } else {
        shinyjs::enable(selector = '#main_navbar li a[data-value="tab_specifics"]')
      }
    })

    observe({
      if (length(active_selected_ids()) > 0 && !specifics_has_errors()) {
        shinyjs::show(selector = '#main_navbar li a[data-value="tab_execution"]')
      } else {
        shinyjs::hide(selector = '#main_navbar li a[data-value="tab_execution"]')
        if (identical(input$main_navbar, "tab_execution")) updateNavbarPage(session, "main_navbar", selected = "tab_specifics")
      }
    })

    # Remove the boot-time loading overlay once the initial reactive graph has
    # settled (low priority so it always runs after the validations above).
    observe({
      global_has_errors()
      specifics_has_errors()
      shinyjs::delay(400, {
        shinyjs::runjs("$('#loading-overlay').css('opacity', '0'); setTimeout(function(){ $('#loading-overlay').remove(); }, 400);")
      })
    }, priority = -100)
}
