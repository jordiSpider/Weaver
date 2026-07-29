#' Viability Plot for Individual Simulations
#'
#' Generates interactive plots of voracity, mass, and egg-related traits for simulated individuals over time.
#' Each simulation can be visualized separately, with category-based buttons to switch between Voracity, Masses, and Eggs.
#'
#' @param data_species List. Species-level parameters including growth, reproduction, and other traits.
#' @param simulations List. A list of simulation outputs for each individual. Each element must contain:
#'   \itemize{
#'     \item \code{currentAge}: Numeric vector of ages (in simulation steps).
#'     \item \code{targetMasses}: Numeric vector of target masses.
#'     \item \code{deficit}: Numeric vector of mass deficits.
#'     \item \code{predVor}: Numeric vector of predicted voracity.
#'     \item \code{preFinalVoracity}: Numeric vector of preliminary final voracity.
#'     \item \code{currentBodySize}: Numeric vector of body size.
#'     \item \code{currentDryMass}: Numeric vector of dry mass.
#'     \item \code{nextMassPredicted}: Numeric vector of next predicted mass.
#'     \item \code{netGrowth}: Numeric vector of net growth.
#'     \item \code{currentEnergyTank}: Numeric vector of energy tank.
#'     \item \code{massForNextReproduction}: Numeric vector of mass required for next reproduction.
#'     \item \code{numEggs}: Numeric vector of eggs laid.
#'     \item \code{targetEggs}: Numeric vector of target eggs.
#'   }
#' @param individual List. Individual-level parameters (e.g., traits, growth, reproduction).
#' @param timeStep Numeric. Conversion factor from simulation steps to real-world time units (e.g., days).
#' @param xAxisRange Numeric or NULL. Maximum value for the x-axis (age). If NULL, the range is determined automatically.
#'
#' @return None. The function prints interactive \code{plotly} figures for each simulation.
#' 
#' @importFrom plotly plot_ly add_trace layout config
#' @export
viability_plot<-function(data_species, simulations, individual, timeStep, xAxisRange)
{
	for (simulation_name in names(simulations)) {
		currentAgeDays <- simulations[[simulation_name]]$currentAge * timeStep

		voracity_traces <- list(
			list(x = currentAgeDays, y = simulations[[simulation_name]]$targetMasses, name = "targetMasses", line = list(width = 3, color = "dark")),
			list(x = currentAgeDays, y = simulations[[simulation_name]]$deficit, name = "deficit", line = list(width = 3, color = "green")),
			list(x = currentAgeDays, y = simulations[[simulation_name]]$predVor, name = "predVor", line = list(width = 3, color = "orange")),
			list(x = currentAgeDays, y = simulations[[simulation_name]]$preFinalVoracity, name = "preFinalVoracity", line = list(width = 3, dash = "dash", color = "red"))
		)

		masses_traces <- list(
			list(x = currentAgeDays, y = simulations[[simulation_name]]$currentBodySize, name = "Body Size", line = list(width = 3, color = "blue")),
			list(x = currentAgeDays, y = simulations[[simulation_name]]$currentDryMass, name = "Current Mass", line = list(width = 3, color = "red")),
			list(x = currentAgeDays, y = simulations[[simulation_name]]$nextMassPredicted, name = "Next Mass Predicted", line = list(width = 3, color = "purple")),
			list(x = currentAgeDays, y = simulations[[simulation_name]]$netGrowth, name = "Net Growth", line = list(width = 3, dash = "dash", color = "orange")),
			list(x = currentAgeDays, y = simulations[[simulation_name]]$currentBodySize + simulations[[simulation_name]]$currentEnergyTank, name = "Body Size + EnergyTank", line = list(width = 3, dash = "dash", color = "green")),
			list(x = currentAgeDays, y = simulations[[simulation_name]]$massForNextReproduction, name = "Mass For Next Reproduction", line = list(width = 3, dash = "dash", color = "pink"))
		)

		eggs_traces <- list(
			list(x = currentAgeDays, y = simulations[[simulation_name]]$numEggs, name = "Eggs", line = list(width = 3, color = "blue")),
			list(x = currentAgeDays, y = simulations[[simulation_name]]$targetEggs, name = "Target Eggs", line = list(width = 3, dash = "dash", color = "red"))
		)

		all_groups <- list(
			Voracity = voracity_traces,
			Masses = masses_traces,
			Eggs = eggs_traces
		)

		all_traces <- list()
		visibility_map <- list()
		group_lengths <- c()

		for (group_name in names(all_groups)) {
			traces <- all_groups[[group_name]]
			all_traces <- append(all_traces, traces)
			group_lengths <- c(group_lengths, length(traces))
		}

		n_total <- length(all_traces)

		default_visible <- rep(FALSE, n_total)

		start <- 1
		for (j in seq_along(names(all_groups))) {
			len <- group_lengths[j]
			vis <- rep(FALSE, n_total)
			vis[start:(start + len - 1)] <- TRUE
			visibility_map[[names(all_groups)[j]]] <- vis
			start <- start + len
		}

		p <- plot_ly()

		for (k in seq_along(all_traces)) {
			trace <- all_traces[[k]]


			p <- add_trace(
				p,
				x = trace$x,
				y = trace$y,
				name = trace$name,
				type = "scatter",
				mode = "lines",
				line = trace$line,
				visible = visibility_map[[names(visibility_map)[1]]][[k]]  # todas invisibles inicialmente
			)
		}

		buttons <- lapply(names(visibility_map), function(name) {
			list(
			method = "restyle",
			args = list("visible", as.list(visibility_map[[name]])),
			label = name
			)
		})

		if(is.null(xAxisRange))
		{
			p <- layout(
				p,
				title = paste("Simulation:", simulation_name),
				xaxis = list(title = "Age"),
				yaxis = list(title = "Value"),
				updatemenus = list(
				list(
					buttons = buttons,
					direction = "down",
					showactive = TRUE,
					x = 0.1,
					xanchor = "left",
					y = 1.1,
					yanchor = "top"
				)
				)
			)
		}
		else
		{
			p <- layout(
				p,
				title = paste("Simulation:", simulation_name),
				xaxis = list(title = "Age", range = c(0, xAxisRange)),
				yaxis = list(title = "Value"),
				dragmode = "pan",
				updatemenus = list(
				list(
					buttons = buttons,
					direction = "down",
					showactive = TRUE,
					x = 0.1,
					xanchor = "left",
					y = 1.1,
					yanchor = "top"
				)
				)
			) %>%
			config(
				displayModeBar = TRUE,
				modeBarButtonsToRemove = c("zoom2d", "zoomIn2d", "zoomOut2d", "autoScale2d", "resetScale2d"),
				scrollZoom = FALSE
			)
		}

		print(p)
	}
}
