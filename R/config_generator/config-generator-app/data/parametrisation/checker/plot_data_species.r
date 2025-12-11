
if (!exists("called_from_main", inherits = FALSE)) {
  stop("This script must be sourced from a main script.")
}

#' Source a script while retaining its path
#'
#' This function sources an R script in a local environment, keeping the script's directory
#' available and exporting all functions to the parent environment.
#'
#' @param path Character. Path to the R script to source.
#' @return Invisibly returns the local environment containing sourced objects.
#' @examples
#' \dontrun{
#' source_with_script_path("scripts/load_or_install_package.r")
#' }
source_with_script_path <- function(path) {
	path <- normalizePath(path)

    parent_env <- parent.frame()
    local_env <- new.env(parent = parent_env)

	local_env$script_path <- dirname(path)

	local_env$called_from_main <- TRUE

	sys.source(path, envir = local_env)

	for (name in ls(local_env)) {
		obj <- local_env[[name]]
		if (is.function(obj)) {
			assign(name, obj, envir = parent_env)
		}
	}

	invisible(local_env)
}


source_with_script_path(file.path(script_path, "load_or_install_package.r"))
source_with_script_path(file.path(script_path, "tpc.r"))


#' Plot Thermal Performance Curves (TPCs) for a species
#'
#' Generates TPCs for all traits of a species and displays them interactively using Plotly.
#' Minimum and maximum trait values are plotted for each trait, with a dropdown to select
#' the trait of interest.
#'
#' @param data_species List containing species traits and metadata. Each trait may include:
#'   \describe{
#'     \item{min}{Minimum value of the trait.}
#'     \item{max}{Maximum value of the trait.}
#'     \item{temperature}{Optional temperature-dependence parameters.}
#'     \item{tempSizeRuleVector}{Optional vector for TSR calculation (for size-related traits).}
#'   }
#' @return Displays an interactive Plotly figure with TPCs for each trait.
#' @details
#' - Converts development time to development rate for plotting (`base.devRate = 1 / base.devTime`).
#' - Computes TPC values across a temperature range (-20°C to 50°C).
#' - Uses \code{use_Pawar_2018} for most traits or \code{use_TSR} for size-dependent traits.
#' - Initial plot shows only the first trait; use dropdown to switch.
#' @examples
#' \dontrun{
#' plot_data_species(my_species_data)
#' }
plot_data_species<-function(data_species){

    load_or_install_package("plotly")
    
	# Helper function to generate TPC for a single trait
	generate_tpc <- function(trait_name, data_species) {
		temperatures <- seq(-20, 50, by = 0.01)
		traitMin <- data_species$traits[[trait_name]]$min
		tpc_values_min <- sapply(temperatures, function(temp) {
			if (data_species$traits[[trait_name]]$temperature$dependent) {
				if (trait_name == "base.lengthAtMaturation") {
					return(use_TSR(
						temperature = temp, 
						tempSizeRuleVector = data_species$traits[[trait_name]]$temperature$tempSizeRuleVector, 
						coefficientForMassAforMature = data_species$coefficientForMassAforMature, 
						scaleForMassBforMature = data_species$scaleForMassBforMature,
						traitAtTempRef = traitMin,
						Tref = data_species$tempFromLab
					))
				} else {
					return(use_Pawar_2018(
                        trait_name = trait_name,
						temperature = temp,
						traitAtTopt = traitMin,
						Topt = data_species$traits[[trait_name]]$temperature$elements$temperatureOptimal$min,
						ED = data_species$traits[[trait_name]]$temperature$elements$energyDecay$min,
						E = data_species$traits[[trait_name]]$temperature$elements$activationEnergy$min,
						Tref = data_species$traits[[trait_name]]$temperature$elements$temperatureRef$min
					))
				}
			} else {
				return(traitMin)
			}
		})

		traitMax <- data_species$traits[[trait_name]]$max
		tpc_values_max <- sapply(temperatures, function(temp) {
			if (data_species$traits[[trait_name]]$temperature$dependent) {
				if (trait_name == "base.lengthAtMaturation") {
					return(use_TSR(
						temperature = temp, 
						tempSizeRuleVector = data_species$traits[[trait_name]]$temperature$tempSizeRuleVector, 
						coefficientForMassAforMature = data_species$coefficientForMassAforMature, 
						scaleForMassBforMature = data_species$scaleForMassBforMature,
						traitAtTempRef = traitMax,
						Tref = data_species$tempFromLab
					))
				} else {
					return(use_Pawar_2018(
                        trait_name = trait_name,
						temperature = temp,
						traitAtTopt = traitMax,
						Topt = data_species$traits[[trait_name]]$temperature$elements$temperatureOptimal$max,
						ED = data_species$traits[[trait_name]]$temperature$elements$energyDecay$max,
						E = data_species$traits[[trait_name]]$temperature$elements$activationEnergy$max,
						Tref = data_species$traits[[trait_name]]$temperature$elements$temperatureRef$max
					))
				}
			} else {
				return(traitMax)
			}
		})

		list(temperatures = temperatures, tpc_values_min = tpc_values_min, tpc_values_max = tpc_values_max)
	}

	# Convert development time to development rate
	data_species$traits[["base.devRate"]] = data_species$traits[["base.devTime"]]
	data_species$traits[["base.devRate"]]$min = 1.0 / data_species$traits[["base.devRate"]]$min
	data_species$traits[["base.devRate"]]$max = 1.0 / data_species$traits[["base.devRate"]]$max
	data_species$traits <- data_species$traits[order(names(data_species$traits))]

	traits <- names(data_species$traits)
	plots <- list()
	
	# Generate Plotly traces
	for (trait in traits) {
		tpc <- generate_tpc(trait, data_species)
		plots <- c(plots, list(
			list(
				x = tpc$temperatures,
				y = tpc$tpc_values_min,
				type = "scatter",
				mode = "lines",
				name = paste(trait, "(min)"),
				line = list(width = 3, color = "green"),
				visible = FALSE  
			),
			list(
				x = tpc$temperatures,
				y = tpc$tpc_values_max,
				type = "scatter",
				mode = "lines",
				name = paste(trait, "(max)"),
				line = list(width = 3, dash = "dash", color = "orange"),
				visible = FALSE 
			)
		))
	}

	# Show first trait by default
	plots[[1]]$visible <- TRUE
	plots[[2]]$visible <- TRUE
    
    # Create dropdown buttons
    buttons <- lapply(seq_along(traits), function(i) {
		list(
			method = "update",
			args = list(
				list(visible = rep(FALSE, length(plots))),  # Ocultar todas las trazas
				list(title = paste("TPC para el trait:", traits[i]))
			),
			label = traits[i]
		)
	})

	for (i in seq_along(traits)) {
		buttons[[i]]$args[[1]]$visible[(i - 1) * 2 + 1] <- TRUE 
		buttons[[i]]$args[[1]]$visible[(i - 1) * 2 + 2] <- TRUE 
	}
    
    # Build Plotly figure
    fig <- plot_ly()
    for (plot in plots) {
        fig <- fig %>% add_trace(
            x = plot$x,
            y = plot$y,
            ymin = plot$ymin,
            ymax = plot$ymax,
            type = plot$type,
            mode = plot$mode,
            fill = plot$fill,
            name = plot$name,
			line = plot$line,
            visible = plot$visible
        )
    }

    fig <- fig %>% layout(
            updatemenus = list(
                list(
                    buttons = buttons,
                    direction = "down",
                    showactive = TRUE
                )
            ),
            xaxis = list(title = "Temperature (°C)"),
            yaxis = list(title = "Trait Value"),
            title = "Thermal Performance Curves (TPC)"
        )

    print(fig)
}
