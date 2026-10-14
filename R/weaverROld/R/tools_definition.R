# =========================================================================
# ROOT TOOL OBJECT DECLARATIONS (NAMESPACE COMPATIBLE)
# =========================================================================

#' Container object for the Weaver optimiser tool
#' @export
optimiser <- list(
  #' Launch the optimiser as a standalone Shiny application
  run_ui = function(...) {
    if (exists(".internal_optimiser_ui_execution")) {
      .internal_optimiser_ui_execution(...)
    } else {
      stop("The optimiser user interface has not been initialized correctly.")
    }
  },
  #' Build the optimiser UI container (page_navbar) for embedding into a host application
  #' @param tool_env environment with the loaded optimiser submodules (see .load_all_tool_submodules)
  #' @param ... forwarded to the internal UI builder (e.g. show_exit_button, with_loading_overlay)
  build_ui = function(tool_env, ...) {
    if (exists(".internal_optimiser_build_ui", envir = tool_env, inherits = FALSE)) {
      get(".internal_optimiser_build_ui", envir = tool_env, inherits = FALSE)(tool_env, ...)
    } else {
      stop("The optimiser UI builder has not been initialized correctly.")
    }
  },
  #' Attach the optimiser reactive orchestration (validations, tab gating, loading overlay, exit handling)
  #' to an existing Shiny session, e.g. from a host application such as the HUB.
  #' @param input,output,session Shiny server arguments from the host application
  #' @param tool_env environment with the loaded optimiser submodules
  #' @param on_exit optional callback invoked instead of stopApp() when the user requests exit
  attach_server = function(input, output, session, tool_env, on_exit = NULL) {
    if (exists(".build_optimiser_orchestrator", envir = tool_env, inherits = FALSE)) {
      get(".build_optimiser_orchestrator", envir = tool_env, inherits = FALSE)(input, output, session, tool_env, on_exit = on_exit)
    } else {
      stop("The optimiser server orchestrator has not been initialized correctly.")
    }
  }
)