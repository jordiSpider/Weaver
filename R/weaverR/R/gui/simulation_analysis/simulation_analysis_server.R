mod_simulation_analysis_server <- function(id, results_directory, on_return) {
  moduleServer(id, function(input, output, session) {
	analyses <- simulation_analysis_registry()
	for (analysis_name in names(analyses)) {
	  analyses[[analysis_name]]$server(
		paste0(analysis_name, "_analysis"),
		results_directory
	  )
	}

	observeEvent(input$btn_return_to_hub, on_return(), ignoreInit = TRUE)
  })
}
