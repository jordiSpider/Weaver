# Server-side defaults for the public optimiser wrapper.
# Kept outside the shared engine so web callbacks never depend on progressr.

server_default_metrics_callback <- function(generation, current_best, current_mean, log_line, log_type) {
  cat(sprintf("[%s]\t[%s]\t%s\n", format(Sys.time(), "%Y-%m-%d %H:%M:%S"), toupper(log_type), log_line))
  flush.console()
}

server_default_evolution_callback <- function(current_ind, total_ind, current_gen, max_gen, trigger_plots) {
  # Generation lifecycle events remain available to server scripts as concise
  # console messages; per-individual progress is reported by progressr below.
  if (current_ind == 0L || current_ind == total_ind) {
    state <- if (current_ind == 0L) "starting" else "completed"
    message(sprintf("Generation %d/%d %s.", current_gen, max_gen, state))
  }
}

server_default_output_path_callback <- function(run_output_path) {
  message("Optimisation output: ", run_output_path)
}

server_default_individual_callback <- function(current_ind, total_ind, current_gen, max_gen) {
  invisible(NULL)
}

with_server_progress <- function(optimization_config, execute_engine) {
  if (is.character(optimization_config) && length(optimization_config) == 1L) {
    optimization_config <- jsonlite::fromJSON(optimization_config, simplifyVector = FALSE)
  }

  settings <- optimization_config$genetic_algorithm_settings
  population_size <- as.integer(settings$population_size)
  max_generations <- as.integer(settings$max_generations)
  total_steps <- population_size * max_generations

  if (length(total_steps) != 1L || is.na(total_steps) || !is.finite(total_steps) || total_steps < 1L) {
    stop("The optimization config must define positive population_size and max_generations to report progress.")
  }

  progressr::with_progress({
    progress <- progressr::progressor(steps = total_steps)
    individual_callback <- function(current_ind, total_ind, current_gen, max_gen) {
      progress(
        amount = 1,
        message = sprintf("Generation %d/%d, individual %d/%d", current_gen, max_gen, current_ind, total_ind)
      )
    }
    execute_engine(individual_callback)
  }, handlers = progressr::handler_txtprogressbar(), enable = TRUE)
}
