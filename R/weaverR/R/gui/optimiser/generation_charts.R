read_completed_generation_history <- function(run_folder) {
  if (is.null(run_folder) || !dir.exists(run_folder)) return(NULL)
  snapshot_dir <- file.path(run_folder, "evolution_generations")
  if (!dir.exists(snapshot_dir)) return(NULL)
  snapshots <- list.files(snapshot_dir, pattern = "^generation-[0-9]{6}\\.tsv$", full.names = TRUE)
  snapshots <- snapshots[vapply(snapshots, function(path) {
    generation <- sub("^generation-([0-9]{6})\\.tsv$", "\\1", basename(path))
    file.exists(file.path(run_folder, "fitness_graphs", paste0("generation-", generation), "generation.complete"))
  }, logical(1))]
  if (!length(snapshots)) return(NULL)
  frames <- lapply(snapshots, function(path) tryCatch(read.delim(path, check.names = FALSE, stringsAsFactors = FALSE), error = function(e) NULL))
  frames <- Filter(function(frame) !is.null(frame) && nrow(frame) > 0, frames)
  if (!length(frames)) return(NULL)
  dplyr::bind_rows(frames) |>
    dplyr::mutate(Generation = as.integer(Generation))
}

summarise_generation_objective <- function(history, objective, criterion) {
  history[[objective]] <- suppressWarnings(as.numeric(history[[objective]]))
  history <- history[is.finite(history[[objective]]) & is.finite(history$Generation), , drop = FALSE]
  if (!nrow(history)) return(NULL)
  generations <- sort(unique(history$Generation))
  summary <- do.call(rbind, lapply(generations, function(generation) {
    values <- history[[objective]][history$Generation == generation]
    data.frame(
      Generation = generation,
      BestInGeneration = if (criterion == "MAX") max(values) else min(values),
      Mean = mean(values), Median = stats::median(values),
      Q1 = unname(stats::quantile(values, 0.25)), Q3 = unname(stats::quantile(values, 0.75))
    )
  }))
  summary$BestSoFar <- if (criterion == "MAX") cummax(summary$BestInGeneration) else cummin(summary$BestInGeneration)
  summary
}

objective_nondominated <- function(data, objective_names, criteria) {
  if (nrow(data) < 1L || length(objective_names) < 2L) return(rep(FALSE, nrow(data)))
  values <- as.matrix(data[, objective_names, drop = FALSE])
  storage.mode(values) <- "double"
  if (ncol(values) != 2L) stop("The objective-space view requires exactly two objectives.")
  for (column in seq_along(criteria)) if (criteria[[column]] == "MAX") values[, column] <- -values[, column]
  nondominated <- rep(FALSE, nrow(values))
  valid <- which(is.finite(values[, 1]) & is.finite(values[, 2]))
  if (!length(valid)) return(nondominated)
  ordered <- valid[order(values[valid, 1], values[valid, 2])]
  x_groups <- split(ordered, values[ordered, 1])
  best_y_at_smaller_x <- Inf
  for (group in x_groups) {
    group_min_y <- min(values[group, 2])
    if (best_y_at_smaller_x > group_min_y) nondominated[group[values[group, 2] == group_min_y]] <- TRUE
    best_y_at_smaller_x <- min(best_y_at_smaller_x, group_min_y)
  }
  nondominated
}
