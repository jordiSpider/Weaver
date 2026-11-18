#' Obstacle Generation Simulator Server
#'
#' @param id Unique module namespace identifier.
#' @import shiny
#' @export
mod_obstacle_simulator_server <- function(id) {
  moduleServer(id, function(input, output, session) {
	generated_mask <- reactive({
	  generate_obstacle_porosity(
		map_width_cells = input$map_width_cells,
		map_height_cells = input$map_height_cells,
		cell_size = input$cell_size,
		bottom_left_x = input$bottom_left_x,
		bottom_left_y = input$bottom_left_y,
		top_right_x = input$top_right_x,
		top_right_y = input$top_right_y,
		fraction = input$fraction,
		minimum_pore_width = input$minimum_pore_width,
		feature_size = input$feature_size,
		octaves = input$octaves,
		seed = input$seed
	  )
	})

	output$obstacle_plot <- renderPlot({
	  mask <- generated_mask()
	  image(
		x = (seq_len(ncol(mask)) - 0.5) * input$cell_size,
		y = (seq_len(nrow(mask)) - 0.5) * input$cell_size,
		z = t(mask),
		col = c("#B0BEC5", "#D9F0E3", "#263238"),
		breaks = c(-1.5, -0.5, 0.5, 1.5),
		xlab = "Map X (physical units)",
		ylab = "Map Y (physical units)",
		main = "Full map: grey = outside patch, light = obstacle, dark = pore",
		useRaster = TRUE,
		asp = 1
	  )
	}, res = 96)

	output$generation_status <- renderUI({
	  mask <- generated_mask()
	  patch_cells <- sum(mask >= 0L)
	  pore_fraction <- if (patch_cells > 0L) sum(mask == 1L) / patch_cells else 0
	  tags$p(class = "text-muted", sprintf("Map: %d × %d cells | Cells in patch: %d | Generated porous fraction: %.1f%%", ncol(mask), nrow(mask), patch_cells, pore_fraction * 100))
	})
  })
}
