mod_movement_analysis_ui <- function(id) {
	ns <- NS(id)
  bslib::card(
	bslib::card_header("Movement analysis"),
	bslib::card_body(
		div(class = "d-flex flex-wrap gap-3 mb-3",
		div(class = "d-flex align-items-center gap-2",
		  span(style = "display:inline-block;width:16px;height:16px;background:#D9F0E3;border:1px solid #8FA99B;"),
		  span("Obstacle")
		),
		div(class = "d-flex align-items-center gap-2",
		  span(style = "display:inline-block;width:16px;height:16px;background:#263238;"),
		  span("Free cell")
		),
		div(class = "d-flex align-items-center gap-2",
		  span(style = "display:inline-block;width:16px;height:16px;background:#B0BEC5;"),
		  span("Missing cell record")
		)
	  ),
	  uiOutput(ns("resource_legend")),
	  div(class = "row g-3 align-items-start",
		div(class = "col-12 col-lg-3",
		  uiOutput(ns("time_step_controls")),
		  uiOutput(ns("animal_controls")),
		  uiOutput(ns("animal_summary"))
		),
		div(class = "col-12 col-lg-9",
		  plotly::plotlyOutput(ns("movement_grid"), height = "650px"),
		  uiOutput(ns("movement_grid_summary"))
		)
	  )
	)
  )
}
