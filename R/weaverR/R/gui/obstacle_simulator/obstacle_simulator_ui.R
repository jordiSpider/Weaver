#' Obstacle Generation Simulator UI
#'
#' @param id Unique module namespace identifier.
#' @import shiny
#' @export
mod_obstacle_simulator_ui <- function(id) {
  ns <- NS(id)

  bslib::layout_columns(
	col_widths = c(4, 8),
	fill = FALSE,
	bslib::card(
	  bslib::card_header("Generation parameters"),
	  bslib::card_body(
		numericInput(ns("map_width_cells"), "Map width (cells, X)", value = 100, min = 2, step = 1),
		tags$small(class = "text-muted d-block", "Number of cells across the full map. Larger maps show a wider area."),
		numericInput(ns("map_height_cells"), "Map height (cells, Y)", value = 100, min = 2, step = 1),
		tags$small(class = "text-muted d-block", "Number of cells along the full map height. Larger maps show a taller area."),
		numericInput(ns("cell_size"), "Cell size", value = 50, min = 1, step = 1),
		tags$small(class = "text-muted d-block", "Physical width and height of each square map cell. Smaller cells make the grid finer; larger cells make it coarser."),
		numericInput(ns("bottom_left_x"), "Patch bottom-left X", value = 0, step = 50),
		tags$small(class = "text-muted d-block", "Physical X coordinate of the patch's bottom-left corner; measured from the map origin (0, 0)."),
		numericInput(ns("bottom_left_y"), "Patch bottom-left Y", value = 0, step = 50),
		tags$small(class = "text-muted d-block", "Physical Y coordinate of the patch's bottom-left corner; measured from the map origin (0, 0)."),
		numericInput(ns("top_right_x"), "Patch top-right X", value = 1250, step = 50),
		tags$small(class = "text-muted d-block", "Physical X coordinate of the opposite corner. Must be greater than bottom-left X."),
		numericInput(ns("top_right_y"), "Patch top-right Y", value = 1000, step = 50),
		tags$small(class = "text-muted d-block", "Physical Y coordinate of the opposite corner. Must be greater than bottom-left Y."),
		numericInput(ns("fraction"), "Porous fraction", value = 0.3, min = 0, max = 1, step = 0.05),
		tags$small(class = "text-muted d-block", "Target share of cells made into pores. Small: fewer pores. Large: more pores; 0 gives no pores and 1 targets all covered cells."),
		numericInput(ns("minimum_pore_width"), "Minimum pore width", value = 200, min = 1, step = 10),
		tags$small(class = "text-muted d-block", "Small: narrower pores can be generated. Large: pores must be wider, so fewer may fit. Applied in both directions and rounded up to whole cells; must fit inside the patch."),
		numericInput(ns("feature_size"), "Feature size", value = 230, min = 1, step = 10),
		tags$small(class = "text-muted d-block", "Scale of the Perlin noise. Small: smaller, more frequent patterns. Large: broader, smoother patterns."),
		numericInput(ns("octaves"), "Octaves", value = 5L, min = 1, step = 1),
		tags$small(class = "text-muted d-block", "Number of noise layers. Small: simpler, smoother patterns. Large: more fine-scale variation; must be a positive whole number."),
		numericInput(ns("seed"), "Noise seed", value = 0L, min = 0, step = 1),
		tags$small(class = "text-muted d-block", "Changes the noise pattern, not its overall target pore fraction. Reuse the same seed and settings to reproduce the mask; use a non-negative whole number."),
		tags$small(class = "text-muted d-block", "Coordinates, cell size, pore width and feature size use physical units. The preview shows the full map; cells outside the patch are shown in grey.")
	  )
	),
	bslib::card(
	  bslib::card_header("Generated obstacle preview"),
	  bslib::card_body(
		plotOutput(ns("obstacle_plot"), height = "600px"),
		uiOutput(ns("generation_status"))
	  )
	)
  )
}
