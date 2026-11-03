mod_simulation_analysis_ui <- function(id, results_directory) {
  ns <- NS(id)
  analyses <- simulation_analysis_registry()
  analysis_panels <- lapply(names(analyses), function(analysis_name) {
	analysis <- analyses[[analysis_name]]
	bslib::nav_panel(
	  title = analysis$label,
	  value = analysis_name,
	  analysis$ui(ns(paste0(analysis_name, "_analysis")))
	)
  })

  tagList(
	div(class = "d-flex align-items-center justify-content-between w-100 mb-3",
	  span("Simulation Results Analysis"),
	  actionButton(ns("btn_return_to_hub"), "Return to Hub", icon = icon("arrow-left"), class = "btn-outline-secondary btn-sm")
	),
	p(class = "text-muted", "Results folder: ", tags$code(results_directory)),
	do.call(bslib::navset_tab, c(list(id = ns("analysis_sections")), analysis_panels))
  )
}
