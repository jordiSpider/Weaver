# Generation-scoped reports shared by headless and Shiny optimiser runs.

optimiser_objective_metadata <- function(optimization_config, history) {
  configured <- optimization_config$computing_environment$objectives
  if (length(configured)) {
    objectives <- lapply(configured, function(objective) {
      list(name = as.character(objective$name), criteria = toupper(as.character(objective$criteria)))
    })
    names(objectives) <- vapply(objectives, `[[`, character(1), "name")
  } else {
    objectives <- list()
  }

  available <- intersect(names(objectives), names(history))
  objectives <- objectives[available]
  if (identical(optimization_config$computing_environment$fitness_class, "MultiObjectiveFitness")) {
    priority <- intersect(as.character(optimization_config$computing_environment$priority_ranking), names(objectives))
    objectives <- objectives[c(priority, setdiff(names(objectives), priority))]
  }
  if (identical(optimization_config$computing_environment$fitness_class, "WeightedFitness") && "Fitness" %in% names(history)) {
    objectives$Fitness <- list(name = "Fitness (weighted aggregate)", criteria = "MIN")
  }
  if (!length(objectives) && "Fitness" %in% names(history)) {
    objectives$Fitness <- list(name = "Fitness", criteria = "MIN")
  }
  objectives
}

optimiser_nondominated <- function(data, objectives) {
  if (nrow(data) < 1L || length(objectives) < 2L) return(rep(FALSE, nrow(data)))
  values <- as.matrix(data[, objectives, drop = FALSE])
  storage.mode(values) <- "double"
  criteria <- attr(objectives, "criteria")
  for (column in seq_along(criteria)) {
    if (identical(criteria[[column]], "MAX")) values[, column] <- -values[, column]
  }
  if (ncol(values) != 2L) stop("The objective-space report requires exactly two objectives.")
  nondominated <- rep(FALSE, nrow(values))
  valid <- which(is.finite(values[, 1]) & is.finite(values[, 2]))
  if (!length(valid)) return(nondominated)

  ordered <- valid[order(values[valid, 1], values[valid, 2])]
  x_groups <- split(ordered, values[ordered, 1])
  best_y_at_smaller_x <- Inf
  for (group in x_groups) {
    group_min_y <- min(values[group, 2])
    if (best_y_at_smaller_x > group_min_y) {
      nondominated[group[values[group, 2] == group_min_y]] <- TRUE
    }
    best_y_at_smaller_x <- min(best_y_at_smaller_x, group_min_y)
  }
  nondominated
}

save_generation_reports <- function(history, generation_data, optimization_config, report_root, generation, selected_individual) {
  objective_metadata <- optimiser_objective_metadata(optimization_config, history)
  if (!length(objective_metadata)) return(invisible(NULL))

  generation_dir <- file.path(report_root, sprintf("generation-%06d", as.integer(generation)))
  if (dir.exists(generation_dir)) {
    stop("Refusing to overwrite generation reports: ", generation_dir)
  }
  if (!dir.create(generation_dir, recursive = TRUE, showWarnings = FALSE)) {
    stop("Unable to create generation report directory: ", generation_dir)
  }

  objective_names <- names(objective_metadata)
  for (objective_index in seq_along(objective_names)) {
    objective <- objective_names[[objective_index]]
    criterion <- objective_metadata[[objective]]$criteria
    if (is.na(criterion) || !criterion %in% c("MIN", "MAX")) criterion <- "MIN"

    plot_history <- history
    plot_history[[objective]] <- suppressWarnings(as.numeric(plot_history[[objective]]))
    plot_history$Generation <- suppressWarnings(as.integer(plot_history$Generation))
    plot_history <- plot_history[is.finite(plot_history[[objective]]) & is.finite(plot_history$Generation), , drop = FALSE]
    if (!nrow(plot_history)) next

    summary <- do.call(rbind, lapply(sort(unique(plot_history$Generation)), function(gen) {
      values <- plot_history[[objective]][plot_history$Generation == gen]
      best <- if (criterion == "MAX") max(values) else min(values)
      data.frame(
        Generation = gen,
        BestInGeneration = best,
        BestSoFar = best,
        Mean = mean(values),
        Median = stats::median(values),
        Q1 = unname(stats::quantile(values, 0.25)),
        Q3 = unname(stats::quantile(values, 0.75))
      )
    }))
    summary$BestSoFar <- if (criterion == "MAX") cummax(summary$BestInGeneration) else cummin(summary$BestInGeneration)

    trajectory <- ggplot2::ggplot(summary, ggplot2::aes(x = Generation))
    if (nrow(summary) > 1L) {
      trajectory <- trajectory +
        ggplot2::geom_ribbon(ggplot2::aes(ymin = Q1, ymax = Q3), fill = "#4c78a8", alpha = 0.16) +
        ggplot2::geom_line(ggplot2::aes(y = Mean, colour = "Population mean"), linewidth = 0.8, linetype = "dashed") +
        ggplot2::geom_line(ggplot2::aes(y = BestInGeneration, colour = "Best this generation"), linewidth = 0.8, alpha = 0.65) +
        ggplot2::geom_line(ggplot2::aes(y = BestSoFar, colour = "Best so far"), linewidth = 1.15) +
        ggplot2::geom_point(ggplot2::aes(y = BestSoFar, colour = "Best so far"), size = 1.5)
    } else {
      trajectory <- trajectory +
        ggplot2::geom_point(ggplot2::aes(y = Mean, colour = "Population mean"), size = 2) +
        ggplot2::geom_point(ggplot2::aes(y = BestInGeneration, colour = "Best this generation"), size = 2) +
        ggplot2::geom_point(ggplot2::aes(y = BestSoFar, colour = "Best so far"), size = 2.5)
    }
    trajectory <- trajectory +
      ggplot2::scale_colour_manual(values = c("Population mean" = "#e45756", "Best this generation" = "#72b7b2", "Best so far" = "#1f4e79")) +
      ggplot2::labs(
        title = paste("Objective trajectory:", objective_metadata[[objective]]$name),
        subtitle = paste("Generation", generation, "|", if (criterion == "MAX") "maximize" else "minimize", "| shaded band: interquartile range"),
        x = "Generation", y = "Raw objective value", colour = NULL
      ) + ggplot2::theme_minimal(base_size = 11) + ggplot2::theme(legend.position = "bottom")
    objective_file_tag <- paste0(sprintf("%02d", objective_index), "-", gsub("[^A-Za-z0-9_-]", "_", objective))
    ggplot2::ggsave(file.path(generation_dir, paste0("trajectory-", objective_file_tag, ".svg")), trajectory, width = 8, height = 5)

    current <- generation_data
    current[[objective]] <- suppressWarnings(as.numeric(current[[objective]]))
    current <- current[is.finite(current[[objective]]), , drop = FALSE]
    if (nrow(current)) {
      population_plot <- ggplot2::ggplot(current, ggplot2::aes(x = "Population", y = .data[[objective]])) +
        ggplot2::geom_violin(fill = "#dceaf7", colour = "#2878a5", alpha = 0.7, trim = FALSE) +
        ggplot2::geom_boxplot(width = 0.12, outlier.shape = NA, alpha = 0.6) +
        ggplot2::geom_jitter(width = 0.08, height = 0, alpha = 0.65, size = 1.5, colour = "#1f4e79") +
        ggplot2::labs(title = paste("Generation", generation, "population:"), subtitle = objective_metadata[[objective]]$name, x = NULL, y = "Raw objective value") +
        ggplot2::theme_minimal(base_size = 11)
      ggplot2::ggsave(file.path(generation_dir, paste0("population-", objective_file_tag, ".svg")), population_plot, width = 6, height = 5)
    }
  }

  if (length(objective_names) >= 2L) {
    pair <- objective_names[1:2]
    current <- generation_data
    for (objective in pair) current[[objective]] <- suppressWarnings(as.numeric(current[[objective]]))
    current <- current[is.finite(current[[pair[[1]]]]) & is.finite(current[[pair[[2]]]]), , drop = FALSE]
    if (nrow(current)) {
      criteria <- vapply(objective_metadata[pair], `[[`, character(1), "criteria")
      attr(pair, "criteria") <- criteria
      current$Nondominated <- optimiser_nondominated(current, pair)
      current$Selected <- as.integer(current$Individual) == as.integer(selected_individual)
      objective_plot <- ggplot2::ggplot(current, ggplot2::aes(x = .data[[pair[[1]]]], y = .data[[pair[[2]]]])) +
        ggplot2::geom_point(ggplot2::aes(colour = Nondominated, shape = Selected), size = 2.6, alpha = 0.8) +
        ggplot2::scale_colour_manual(values = c(`FALSE` = "#9aa4af", `TRUE` = "#18a999"), labels = c(`FALSE` = "Dominated", `TRUE` = "Non-dominated")) +
        ggplot2::scale_shape_manual(values = c(`FALSE` = 16, `TRUE` = 8), labels = c(`FALSE` = "Other individual", `TRUE` = "Selected by fitness engine")) +
        ggplot2::labs(title = paste("Generation", generation, "objective space"), x = paste0(pair[[1]], " (", criteria[[1]], ")"), y = paste0(pair[[2]], " (", criteria[[2]], ")"), colour = NULL, shape = NULL) +
        ggplot2::theme_minimal(base_size = 11) + ggplot2::theme(legend.position = "bottom")
      ggplot2::ggsave(file.path(generation_dir, "objective-space.svg"), objective_plot, width = 7, height = 5)
    }
  }

  complete_marker <- file.path(generation_dir, "generation.complete")
  if (file.exists(complete_marker)) stop("Refusing to overwrite a completed generation report: ", generation_dir)
  writeLines(as.character(Sys.time()), complete_marker, useBytes = TRUE)
  invisible(generation_dir)
}
