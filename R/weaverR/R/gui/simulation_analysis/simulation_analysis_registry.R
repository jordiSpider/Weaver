simulation_analysis_registry <- function() {
  list(
	movement = list(
	  label = "Movement",
	  ui = mod_movement_analysis_ui,
	  server = mod_movement_analysis_server
	)
  )
}
