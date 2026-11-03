mod_movement_analysis_server <- function(id, results_directory) {
  moduleServer(id, function(input, output, session) {
	cell_records <- reactive({
	  cell_file <- file.path(results_directory, "cells_each_day", "cells_day_0000000000.txt")
	  if (!file.exists(cell_file)) {
		return(list(error = paste("Simulation cell file not found:", cell_file)))
	  }

	  records <- tryCatch(
		read.delim(
		  cell_file,
		  header = TRUE,
		  sep = "\t",
		  check.names = FALSE,
		  stringsAsFactors = FALSE,
		  colClasses = "character",
		  quote = "",
		  comment.char = ""
		),
		error = function(error) error
	  )
	  if (inherits(records, "error")) {
		return(list(error = paste("Unable to read simulation cell file:", conditionMessage(records))))
	  }
	  if (nrow(records) == 0L) {
		return(list(error = "The simulation cell file contains no cell records."))
	  }

	  column_names <- tolower(trimws(names(records)))
	  required_columns <- match(c("x", "y", "obstacle"), column_names)
	  if (anyNA(required_columns)) {
		return(list(error = "The simulation cell file must contain X, Y and obstacle columns."))
	  }

	  available_mass_columns <- grep("_available_dry_mass$", column_names)
	  obstacle_column <- required_columns[[3L]]
	  if (length(available_mass_columns) > 0L && any(available_mass_columns <= obstacle_column + 1L)) {
		return(list(error = "The resource columns in the simulation cell file are incomplete."))
	  }
	  resource_species <- if (length(available_mass_columns) > 0L) {
		names(records)[available_mass_columns - 1L]
	  } else {
		character(0)
	  }
	  resource_mass_columns <- available_mass_columns - 1L
	  final_resource_column <- if (length(available_mass_columns) > 0L) max(available_mass_columns) else obstacle_column
	  first_animal_species_column <- final_resource_column + 1L
	  animal_species <- if (first_animal_species_column <= ncol(records)) {
		names(records)[seq.int(first_animal_species_column, ncol(records))]
	  } else {
		character(0)
	  }

	  x <- suppressWarnings(as.numeric(records[[required_columns[[1L]]]]))
	  y <- suppressWarnings(as.numeric(records[[required_columns[[2L]]]]))
	  obstacle <- suppressWarnings(as.numeric(records[[required_columns[[3L]]]]))
	  coordinates_are_valid <- all(is.finite(x)) && all(is.finite(y)) &&
		all(x >= 0) && all(y >= 0) && all(x < .Machine$integer.max) && all(y < .Machine$integer.max) &&
		all(x == floor(x)) && all(y == floor(y))
	  if (!coordinates_are_valid) {
		return(list(error = "The simulation cell file contains invalid X/Y cell coordinates."))
	  }
	  if (anyNA(obstacle) || any(!is.finite(obstacle)) || any(!obstacle %in% c(0, 1))) {
		return(list(error = "The obstacle column must contain only 0 or 1 values."))
	  }
	  if (anyDuplicated(data.frame(x = x, y = y))) {
		return(list(error = "The simulation cell file contains duplicate X/Y cell coordinates."))
	  }

	  resource_presence <- matrix(FALSE, nrow = nrow(records), ncol = length(resource_species))
	  if (length(resource_species) > 0L) {
		for (index in seq_along(resource_species)) {
		  total_mass <- suppressWarnings(as.numeric(records[[resource_mass_columns[[index]]]]))
		  if (anyNA(total_mass) || any(!is.finite(total_mass))) {
			return(list(error = sprintf("Resource mass column '%s' contains invalid values.", resource_species[[index]])))
		  }
		  resource_presence[, index] <- total_mass > 0
		}
	  }
	  resource_combinations <- vapply(seq_len(nrow(records)), function(row) {
		paste(resource_species[resource_presence[row, ]], collapse = " + ")
	  }, character(1))

	  list(
		x = as.integer(x),
		y = as.integer(y),
		obstacle = as.integer(obstacle),
		resource_combination = resource_combinations,
		resource_species = resource_species,
		animal_species = animal_species,
		file = cell_file
	  )
	})

	grid_data <- reactive({
	  records <- cell_records()
	  if (!is.null(records$error)) {
		return(records)
	  }

	  x_origin <- min(records$x)
	  y_origin <- min(records$y)
	  x_coordinates <- seq.int(x_origin, max(records$x))
	  y_coordinates <- seq.int(y_origin, max(records$y))
	  grid <- matrix(NA_integer_, nrow = length(y_coordinates), ncol = length(x_coordinates))
	  resource_combinations <- sort(unique(records$resource_combination[records$obstacle == 0L & nzchar(records$resource_combination)]))
	  resource_codes <- stats::setNames(seq_along(resource_combinations) + 1L, resource_combinations)
	  cell_codes <- ifelse(records$obstacle == 1L, 0L, 1L)
	  has_resources <- records$obstacle == 0L & nzchar(records$resource_combination)
	  cell_codes[has_resources] <- unname(resource_codes[records$resource_combination[has_resources]])
	  grid[cbind(records$y - y_origin + 1L, records$x - x_origin + 1L)] <- cell_codes
	  combination_colors <- if (length(resource_combinations) > 0L) {
		stats::setNames(grDevices::hcl.colors(length(resource_combinations), palette = "Dynamic"), resource_combinations)
	  } else {
		character(0)
	  }
	  category_labels <- c("Obstacle", "Free cell", resource_combinations)
	  category_colors <- c("#D9F0E3", "#263238", unname(combination_colors))
	  hover_labels <- matrix(NA_character_, nrow = nrow(grid), ncol = ncol(grid))
	  recorded <- !is.na(grid)
	  hover_labels[recorded] <- category_labels[grid[recorded] + 1L]

	  list(
		grid = grid,
		hover_labels = hover_labels,
		category_labels = category_labels,
		category_colors = category_colors,
		resource_combinations = resource_combinations,
		combination_colors = combination_colors,
		x = x_coordinates,
		y = y_coordinates,
		width = length(x_coordinates),
		height = length(y_coordinates),
		obstacle_count = sum(records$obstacle == 1L),
		free_count = sum(records$obstacle == 0L),
		missing_count = sum(is.na(grid)),
		resource_species = records$resource_species,
		animal_species = records$animal_species,
		file = records$file
	  )
	})

	simulation_time_steps <- reactive({
	  config_file <- file.path(results_directory, "config", "simulation_params.json")
	  if (!file.exists(config_file)) {
		return(list(error = paste("Simulation configuration not found:", config_file)))
	  }

	  config <- tryCatch(
		jsonlite::fromJSON(config_file, simplifyVector = FALSE),
		error = function(error) error
	  )
	  if (inherits(config, "error")) {
		return(list(error = paste("Unable to read simulation configuration:", conditionMessage(config))))
	  }

	  run_days <- suppressWarnings(as.numeric(config$simulation$runDays))
	  time_steps_per_day <- suppressWarnings(as.numeric(config$simulation$timeStepsPerDay))
	  record_each <- suppressWarnings(as.numeric(config$simulation$outputs$recordEach))
	  if (length(run_days) != 1L || !is.finite(run_days) || run_days < 0 ||
		  length(time_steps_per_day) != 1L || !is.finite(time_steps_per_day) || time_steps_per_day <= 0 ||
		  length(record_each) != 1L || !is.finite(record_each) || record_each < 1 || record_each != floor(record_each)) {
		return(list(error = "The simulation configuration has invalid runDays, timeStepsPerDay or recordEach values."))
	  }

	  list(
		maximum = as.integer(floor(run_days / time_steps_per_day + 0.5)),
		record_each = as.integer(record_each),
		error = NULL
	  )
	})

	time_step <- reactiveVal(0L)

	observeEvent(simulation_time_steps(), {
	  config <- simulation_time_steps()
	  if (is.null(config$error)) {
		current <- min(max(as.integer(time_step()), 0L), config$maximum)
		time_step(current)
		updateSliderInput(session, "time_step", min = 0, max = config$maximum, value = current, step = 1)
	  }
	}, ignoreInit = FALSE)

	observeEvent(input$time_step, {
	  selected <- suppressWarnings(as.integer(input$time_step))
	  if (length(selected) == 1L && !is.na(selected)) time_step(selected)
	}, ignoreInit = TRUE)

	observeEvent(input$previous_time_step, {
	  config <- simulation_time_steps()
	  if (is.null(config$error)) {
		selected <- max(0L, as.integer(time_step()) - 1L)
		time_step(selected)
		updateSliderInput(session, "time_step", value = selected)
	  }
	})

	observeEvent(input$next_time_step, {
	  config <- simulation_time_steps()
	  if (is.null(config$error)) {
		selected <- min(config$maximum, as.integer(time_step()) + 1L)
		time_step(selected)
		updateSliderInput(session, "time_step", value = selected)
	  }
	})

	output$time_step_controls <- renderUI({
	  config <- simulation_time_steps()
	  if (!is.null(config$error)) {
		return(div(class = "alert alert-warning mb-3", role = "alert", config$error))
	  }

	  tagList(
		div(class = "d-flex align-items-end gap-2 mb-3",
		  actionButton(session$ns("previous_time_step"), "Previous", class = "btn-outline-secondary"),
		  actionButton(session$ns("next_time_step"), "Next", class = "btn-outline-secondary"),
		  textOutput(session$ns("time_step_label"), inline = TRUE)
		),
		sliderInput(
		  session$ns("time_step"),
		  "Time step",
		  min = 0,
		  max = config$maximum,
		  value = isolate(time_step()),
		  step = 1,
		  ticks = TRUE,
		  sep = ""
		)
	  )
	})

	output$time_step_label <- renderText({
	  config <- simulation_time_steps()
	  sprintf("Time step %d of %d", time_step(), config$maximum)
	})

	animal_data <- reactive({
	  cell_metadata <- cell_records()
	  simulation_steps <- simulation_time_steps()
	  empty_animals <- data.frame(id = character(0), species = character(0), x = numeric(0), y = numeric(0))
	  if (!is.null(cell_metadata$error)) {
		return(list(animals = empty_animals, error = cell_metadata$error, cell_size = NULL))
	  }
	  if (!is.null(simulation_steps$error)) {
		return(list(animals = empty_animals, error = simulation_steps$error, cell_size = NULL))
	  }

	  selected_time_step <- as.integer(time_step())
	  is_final_time_step <- simulation_steps$maximum > 0L && selected_time_step == simulation_steps$maximum
	  file_time_step <- if (is_final_time_step) max(0L, selected_time_step - 1L) else selected_time_step
	  animal_directory <- if (is_final_time_step) "animals_each_day_end" else "animals_each_day_start"
	  animal_file <- file.path(
		results_directory,
		animal_directory,
		sprintf("animals_day_%010d.txt", file_time_step)
	  )
	  if (!file.exists(animal_file)) {
		return(list(
		  animals = empty_animals,
		  error = paste("Animal records file not found:", animal_file),
		  cell_size = NULL,
		  resource_species = cell_metadata$resource_species,
		  animal_species = cell_metadata$animal_species
		))
	  }

	  records <- tryCatch(
		read.delim(
		  animal_file,
		  header = TRUE,
		  sep = "\t",
		  check.names = FALSE,
		  stringsAsFactors = FALSE,
		  colClasses = "character",
		  quote = "",
		  comment.char = ""
		),
		error = function(error) error
	  )
	  if (inherits(records, "error")) {
		return(list(
		  animals = empty_animals,
		  error = paste("Unable to read animal records file:", conditionMessage(records)),
		  cell_size = NULL,
		  resource_species = cell_metadata$resource_species,
		  animal_species = cell_metadata$animal_species
		))
	  }

	  column_names <- tolower(trimws(names(records)))
	  required_columns <- match(c("id", "species", "x", "y", "poreuse"), column_names)
	  if (anyNA(required_columns)) {
		return(list(
		  animals = empty_animals,
		  error = "The animal records file must contain id, species, X, Y and poreUse columns.",
		  cell_size = NULL,
		  resource_species = cell_metadata$resource_species,
		  animal_species = cell_metadata$animal_species
		))
	  }

	  ids <- trimws(records[[required_columns[[1L]]]])
	  species <- trimws(records[[required_columns[[2L]]]])
	  x <- suppressWarnings(as.numeric(records[[required_columns[[3L]]]]))
	  y <- suppressWarnings(as.numeric(records[[required_columns[[4L]]]]))
	  pore_use <- suppressWarnings(as.numeric(records[[required_columns[[5L]]]]))
	  if (anyNA(ids) || any(!nzchar(ids)) || anyNA(species) || any(!nzchar(species)) || anyDuplicated(ids)) {
		return(list(
		  animals = empty_animals,
		  error = "The animal records file contains missing or duplicate animal IDs/species.",
		  cell_size = NULL,
		  resource_species = cell_metadata$resource_species,
		  animal_species = cell_metadata$animal_species
		))
	  }
	  if (anyNA(x) || anyNA(y) || any(!is.finite(x)) || any(!is.finite(y))) {
		return(list(
		  animals = empty_animals,
		  error = "The animal records file contains invalid continuous X/Y positions.",
		  cell_size = NULL,
		  resource_species = cell_metadata$resource_species,
		  animal_species = cell_metadata$animal_species
		))
	  }
	  if (anyNA(pore_use) || any(!is.finite(pore_use)) || any(pore_use <= 0)) {
		return(list(
		  animals = empty_animals,
		  error = "The animal records file contains invalid poreUse values.",
		  cell_size = NULL,
		  resource_species = cell_metadata$resource_species,
		  animal_species = cell_metadata$animal_species
		))
	  }

	  animals <- data.frame(id = ids, species = species, x = x, y = y, pore_use = pore_use, stringsAsFactors = FALSE)
	  config_file <- file.path(results_directory, "config", "landscape_params.json")
	  if (!file.exists(config_file)) {
		return(list(
		  animals = animals,
		  error = paste("Map scale configuration not found:", config_file),
		  cell_size = NULL,
		  resource_species = cell_metadata$resource_species,
		  animal_species = cell_metadata$animal_species
		))
	  }

	  landscape_config <- tryCatch(
		jsonlite::fromJSON(config_file, simplifyVector = FALSE),
		error = function(error) error
	  )
	  if (inherits(landscape_config, "error")) {
		return(list(
		  animals = animals,
		  error = paste("Unable to read map scale configuration:", conditionMessage(landscape_config)),
		  cell_size = NULL,
		  resource_species = cell_metadata$resource_species,
		  animal_species = cell_metadata$animal_species
		))
	  }

	  cell_size <- suppressWarnings(as.numeric(landscape_config$landscape$mapConfig$landscapeWideParams$minCellSize))
	  if (length(cell_size) != 1L || is.na(cell_size) || !is.finite(cell_size) || cell_size <= 0) {
		return(list(
		  animals = animals,
		  error = "The map scale configuration does not contain a valid minCellSize.",
		  cell_size = NULL,
		  resource_species = cell_metadata$resource_species,
		  animal_species = cell_metadata$animal_species
		))
	  }

	  list(
		animals = animals,
		error = NULL,
		cell_size = cell_size,
		resource_species = cell_metadata$resource_species,
		animal_species = cell_metadata$animal_species,
		file = animal_file,
		time_step = selected_time_step
	  )
	})

	selected_animal_ids <- reactiveVal(character(0))
	pinned_animal_id <- reactiveVal("")
	animals_initialized <- reactiveVal(FALSE)

	species_colors <- reactive({
	  species <- cell_records()$animal_species
	  colors <- grDevices::hcl.colors(max(length(species), 1L), palette = "Dark 3")	
	  stats::setNames(colors[seq_along(species)], species)
	})

	observeEvent(animal_data(), {
	  data <- animal_data()
	  animals <- data$animals
	  old_selection <- isolate(selected_animal_ids())
	  if (!isTRUE(isolate(animals_initialized()))) {
		selected_animal_ids(animals$id)
		animals_initialized(TRUE)
	  } else {
		selected_animal_ids(intersect(old_selection, animals$id))
	  }
	  if (!pinned_animal_id() %in% selected_animal_ids()) pinned_animal_id("")
	  updateSelectizeInput(session, "selected_animals", choices = stats::setNames(animals$id, paste(animals$species, animals$id, sep = " — ")), selected = animals$id, server = FALSE)
	  updateSelectizeInput(session, "selected_animals", selected = selected_animal_ids(), server = FALSE)
	  updateSelectInput(session, "pinned_animal", choices = c("None" = "", stats::setNames(animals$id, paste(animals$species, animals$id, sep = " — "))), selected = pinned_animal_id())
	}, ignoreInit = FALSE)

	observeEvent(input$toggle_all_animals, {
	  animals <- animal_data()$animals
	  if (length(selected_animal_ids()) == nrow(animals)) {
		selected_animal_ids(character(0))
		updateSelectizeInput(session, "selected_animals", selected = character(0), server = FALSE)
	  } else {
		selected_animal_ids(animals$id)
		updateSelectizeInput(session, "selected_animals", selected = animals$id, server = FALSE)
	  }
	})

	observeEvent(input$selected_animals, {
	  selected <- input$selected_animals
	  if (is.null(selected)) selected <- character(0)
	  selected_animal_ids(selected)
	  if (!length(selected) || !pinned_animal_id() %in% selected) {
		pinned_animal_id("")
		updateSelectInput(session, "pinned_animal", selected = "")
	  }
	}, ignoreInit = TRUE)

	observeEvent(input$pinned_animal, {
	  pinned <- input$pinned_animal
	  if (is.null(pinned)) pinned <- ""
	  pinned_animal_id(pinned)
	}, ignoreInit = TRUE)

	output$animal_controls <- renderUI({
	  data <- animal_data()
	  animals <- data$animals
	  if (nrow(animals) == 0L) {
		return(div(class = "text-muted mb-3", "No animal records are available for day 0."))
	  }

	  choices <- stats::setNames(animals$id, paste(animals$species, animals$id, sep = " — "))
	  tagList(
		div(class = "d-flex flex-wrap align-items-end gap-3 mb-3",
		  div(style = "min-width: 280px; flex: 1;",
			selectizeInput(session$ns("selected_animals"), "Visible animals", choices = choices, selected = isolate(selected_animal_ids()), multiple = TRUE, options = list(plugins = list("remove_button")))
		  ),
		div(class = "d-flex align-items-end gap-2",
			actionButton(session$ns("toggle_all_animals"), "Select/deselect all", class = "btn-outline-secondary"),
			selectInput(session$ns("pinned_animal"), "Pinned animal", choices = c("None" = "", choices), selected = pinned_animal_id(), selectize = FALSE)
		  )
		)
	  )
	})

	output$animal_summary <- renderUI({
	  data <- animal_data()
	  if (!is.null(data$error)) {
		return(div(class = "alert alert-warning mb-2", role = "alert", data$error))
	  }
	  if (length(data$animals$id) == 0L) {
		return(div(class = "text-muted mb-2", "No animals were recorded for day 0."))
	  }
	  species <- cell_records()$animal_species
	  species <- species[species %in% data$animals$species]
	  div(class = "d-flex flex-wrap gap-3 mb-2 text-muted",
		span(sprintf("Animals: %d", nrow(data$animals))),
		span(sprintf("Species: %s", paste(species, collapse = ", "))),
		if (nzchar(pinned_animal_id())) span("Pinned animal: ★")
	  )
	})

	output$movement_grid <- plotly::renderPlotly({
	  data <- grid_data()
	  if (!is.null(data$error)) {
		shiny::validate(shiny::need(FALSE, data$error))
	  }

	  color_count <- length(data$category_colors)
	  if (color_count == 1L) {
		colorscale <- list(list(0, data$category_colors[[1L]]), list(1, data$category_colors[[1L]]))
	  } else {
		colorscale <- unlist(lapply(seq_len(color_count), function(index) {
		  center <- (index - 1L) / (color_count - 1L)
		  half_bin <- 0.5 / (color_count - 1L)
		  lower <- if (index == 1L) 0 else center - half_bin
		  upper <- if (index == color_count) 1 else center + half_bin
		  list(list(lower, data$category_colors[[index]]), list(upper, data$category_colors[[index]]))
		}), recursive = FALSE)
	  }
	  plot <- plotly::plot_ly(
		x = data$x,
		y = data$y,
		z = data$grid,
		text = data$hover_labels,
		type = "heatmap",
		colorscale = colorscale,
		zmin = 0,
		zmax = color_count - 1L,
		showscale = FALSE,
		hoverongaps = FALSE,
		hovertemplate = "X: %{x}<br>Y: %{y}<br>%{text}<extra></extra>",
		xgap = 1,
		ygap = 1
	  )
	  animals <- animal_data()
	  if (is.null(animals$error) && nrow(animals$animals) > 0L) {
		visible_animals <- animals$animals[animals$animals$id %in% selected_animal_ids(), , drop = FALSE]
		visible_animals$x <- visible_animals$x / animals$cell_size - 0.5
		visible_animals$y <- visible_animals$y / animals$cell_size - 0.5
		visible_animals <- visible_animals[
			visible_animals$x >= min(data$x) - 0.5 & visible_animals$x <= max(data$x) + 0.5 &
			visible_animals$y >= min(data$y) - 0.5 & visible_animals$y <= max(data$y) + 0.5,
			, drop = FALSE
		]
		if (nrow(visible_animals) > 0L) {
			species <- cell_records()$animal_species
			species <- species[species %in% visible_animals$species]
			color_map <- species_colors()
			for (index in seq_along(species)) {
				species_animals <- visible_animals[visible_animals$species == species[[index]], , drop = FALSE]
				circle_angles <- seq(0, 2 * pi, length.out = 33L)
				circle_data <- lapply(seq_len(nrow(species_animals)), function(animal_index) {
				  animal <- species_animals[animal_index, , drop = FALSE]
				  radius <- animal$pore_use / animals$cell_size / 2
				  hover_label <- sprintf("Species: %s<br>ID: %s<br>poreUse: %s", animal$species, animal$id, animal$pore_use)
				  list(
					x = c(animal$x + radius * cos(circle_angles), NA_real_),
					y = c(animal$y + radius * sin(circle_angles), NA_real_),
					text = c(rep(hover_label, length(circle_angles)), NA_character_)
				  )
				})
				circle_x <- unlist(lapply(circle_data, `[[`, "x"), use.names = FALSE)
				circle_y <- unlist(lapply(circle_data, `[[`, "y"), use.names = FALSE)
				circle_text <- unlist(lapply(circle_data, `[[`, "text"), use.names = FALSE)
				plot <- plot |>
					plotly::add_trace(
						type = "scatter",
						mode = "lines",
						x = circle_x,
						y = circle_y,
						inherit = FALSE,
						xaxis = "x",
						yaxis = "y",
						name = species[[index]],
						legendgroup = species[[index]],
						line = list(color = unname(color_map[[species[[index]]]]), width = 1.5),
						text = circle_text,
						hovertemplate = "%{text}<extra></extra>",
						connectgaps = FALSE
					)
			}
		}
		pinned <- animals$animals[animals$animals$id == pinned_animal_id() & animals$animals$id %in% selected_animal_ids(), , drop = FALSE]
		if (nrow(pinned) == 1L) {
			pinned_x <- pinned$x / animals$cell_size - 0.5
			pinned_y <- pinned$y / animals$cell_size - 0.5
			if (pinned_x >= min(data$x) - 0.5 && pinned_x <= max(data$x) + 0.5 && pinned_y >= min(data$y) - 0.5 && pinned_y <= max(data$y) + 0.5) {
				plot <- plot |>
					plotly::add_trace(
						type = "scatter",
						mode = "markers",
						x = pinned_x,
						y = pinned_y,
						inherit = FALSE,
						xaxis = "x",
						yaxis = "y",
						name = paste("Pinned:", pinned$id),
						marker = list(symbol = "star", color = "#FFD700", size = 18, line = list(color = "#5D4037", width = 1)),
						text = paste("Pinned animal —", pinned$species, "ID:", pinned$id),
						hovertemplate = "%{text}<extra></extra>",
						showlegend = FALSE
					)
			}
		}
	  }
	  plot |>
		plotly::layout(
		  title = list(text = sprintf("World cell grid — time step %d", time_step()), x = 0.5),
		  xaxis = list(title = "X (cell)", range = c(min(data$x) - 0.5, max(data$x) + 0.5), constrain = "domain"),
		  yaxis = list(title = "Y (cell)", range = c(min(data$y) - 0.5, max(data$y) + 0.5), scaleanchor = "x", scaleratio = 1, constrain = "domain"),
		  margin = list(l = 60, r = 20, t = 55, b = 50),
		  paper_bgcolor = "#FFFFFF",
		  plot_bgcolor = "#B0BEC5",
		  dragmode = "zoom"
		) |>
		plotly::config(displayModeBar = TRUE, displaylogo = FALSE, scrollZoom = TRUE)
	})

	output$movement_grid_summary <- renderUI({
	  data <- grid_data()
	  if (!is.null(data$error)) {
		return(div(class = "alert alert-warning mb-0", role = "alert", data$error))
	  }

	  div(class = "d-flex flex-wrap gap-3 mt-2 text-muted",
		span(sprintf("Grid: %d × %d cells", data$width, data$height)),
		span(sprintf("Obstacles: %s", format(data$obstacle_count, big.mark = ","))),
		span(sprintf("Free cells: %s", format(data$free_count, big.mark = ","))),
		if (data$missing_count > 0L) span(sprintf("Unrecorded cells: %s", format(data$missing_count, big.mark = ",")))
	  )
	})

	output$resource_legend <- renderUI({
	  data <- grid_data()
	  if (!is.null(data$error) || length(data$resource_combinations) == 0L) return(NULL)

	  div(class = "d-flex flex-wrap gap-3 mb-3",
		lapply(data$resource_combinations, function(combination) {
		  div(class = "d-flex align-items-center gap-2",
			span(style = paste0("display:inline-block;width:16px;height:16px;background:", data$combination_colors[[combination]], ";border:1px solid #8FA99B;")),
			span(combination)
		  )
		})
	  )
	})

	grid_data
  })
}
