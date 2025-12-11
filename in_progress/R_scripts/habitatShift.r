if (interactive()) {
    script_path <- sys.frame(1)$ofile
} else {
    args <- commandArgs(trailingOnly = FALSE)
    script_path <- sub("--file=", "", args[grep("--file=", args)])
}

script_path <- dirname(script_path)



library(data.table)

library(plotly)





show_world_info <- function(fig, config_path, resource_species, animal_species, time_step_interval) {
    read_cells_info <- function(config_path, time_step_interval) {
        cells_each_day_path <- file.path(config_path, "cells_each_day")

        cells_each_day_dir <- dir(cells_each_day_path, pattern = "\\.txt$", full.names = TRUE)

        cells_each_day_info <- rbindlist(lapply(cells_each_day_dir, function(file) {
            data <- fread(file)
            data[, timeStep := as.integer(sub(".*cells_day_([0-9]+)\\.txt", "\\1", file))]
            return(data)
        }))

        setorder(cells_each_day_info, timeStep)

        # Filtrar las filas de cells_info que tienen un timeStep en la secuencia generada
        cells_each_day_info <- cells_each_day_info[timeStep %in% seq(0, max(timeStep), by = time_step_interval)]
    
        return(cells_each_day_info)
    }


    read_animals_each_day_end_info <- function(config_path, time_step_interval) {
        animals_each_day_end_path <- file.path(config_path, "animals_each_day_start")

        animals_each_day_end_dir <- dir(animals_each_day_end_path, pattern = "\\.txt$", full.names = TRUE)

        animals_each_day_end_info <- rbindlist(lapply(animals_each_day_end_dir, function(file) {
            data <- fread(file)
            
            if (nrow(data) > 0) {
                data[, timeStep := as.integer(sub(".*animals_day_([0-9]+)\\.txt", "\\1", file))]
            } else {
                data[, timeStep := NA]
            }

            return(data)
        }))

        setorder(animals_each_day_end_info, timeStep)
    
        animals_each_day_end_info <- animals_each_day_end_info[timeStep %in% seq(0, max(timeStep), by = time_step_interval)]



        movements_info <- fread(file.path(config_path, "movements.txt"))

        setorder(movements_info, timeStep)

        movements_info <- movements_info[timeStep %in% animals_each_day_end_info$timeStep]



        return(list(animals_each_day_end_info=animals_each_day_end_info, movements_info=movements_info))
    }


    generate_cell_x_coords <- function(x) {
        return(list(x, x+1, x+1, x, x))
    }

    generate_cell_y_coords <- function(y) {
        return(list(y, y, y+1, y+1, y))
    }


    show_grid <- function(fig, grid_size) {
        for (x in 0:(grid_size - 1)) {
            for (y in 0:(grid_size - 1)) {
                fig <- fig %>%
                    add_trace(
                        type = "scatter",
                        mode = "lines",
                        x = generate_cell_x_coords(x),
                        y = generate_cell_y_coords(y),
                        fill = "toself",
                        fillcolor = "white",  # Color basado en el valor
                        line = list(color = "black"),
                        name = paste0("Cell (", x, ", ", y, ")"),  # Nombre con coordenadas
                        legendgroup = "Grid",  # Agrupar en la leyenda
                        showlegend = FALSE  # No mostrar cada celda en la leyenda
                    )
            }
        }

        fig <- fig %>% layout(
            xaxis = list(
                title = "X - Axis",
                range = c(0, grid_size),
                scaleanchor = "y",  # Asegurar que los ejes tengan la misma escala
                scaleratio = 1,
                showgrid = FALSE
            ),
            yaxis = list(
                title = "Y - Axis",
                range = c(0, grid_size),
                showgrid = FALSE
            ),
            showlegend = TRUE  # Mostrar la leyenda
        )

        return(fig)
    }


    show_resource_species_info <- function(fig, species_name, info, species_color) {
        grid_combinations <- expand.grid(X = unique(info$X), Y = unique(info$Y))
        
        cells_split <- split(info, by = c("X", "Y"))

        for (comb in 1:nrow(grid_combinations)) {
            fig <- fig %>%
                add_trace(
                    data = cells_split[[paste(grid_combinations$X[comb], grid_combinations$Y[comb], sep = ".")]],
                    type = "scatter",
                    mode = "lines",
                    x = ~generate_cell_x_coords(X),
                    y = ~generate_cell_y_coords(Y),
                    frame = ~timeStep,
                    fill = "toself",
                    fillcolor = ~color,  # Color basado en el valor
                    line = list(color = "black"),
                    legendgroup = species_name,  # Agrupar en la leyenda
                    showlegend = FALSE,  # No mostrar cada celda en la leyenda
                    text = ~paste("Species:", species_name, "<br>Dry mass:", dry_mass, "<br>Available dry mass:", available_dry_mass),  # Texto de la etiqueta
                    hoverinfo = 'text'
                )
        }

        fig <- fig %>%
            add_trace(
                type = "scatter",
                mode = "lines",
                x = c(0),
                y = c(0),
                fill = "toself",
                fillcolor = species_color,  # Color basado en el valor
                line = list(color = "black"),
                name = species_name,  # Nombre con coordenadas
                legendgroup = species_name,  # Agrupar en la leyenda
                showlegend = TRUE  # No mostrar cada celda en la leyenda
            )
        

        return(fig)
    }


    show_animal_species_info <- function(fig, species_name, info, species_color, grid_size) {
        fig <- fig %>% 
            add_trace(
                data = info$animals_each_day_end_info,
                x = ~X,
                y = ~Y,
                type = 'scatter',
                mode = 'lines+markers',
                frame = ~timeStep,
                line = list(color = species_color),
                marker = list(size = 10, color = species_color),
                name = species_name,
                legendgroup = species_name,  # Agrupar en la leyenda
                showlegend = TRUE,
                text = ~paste("Species:", species_name, "<br>Id:", id, "<br>X:", X, "<br>Y:", Y, "<br>Instar:", instar, "<br>Life stage:", state, "<br>Repro counter:", reproCounter),  # Texto de la etiqueta
                hoverinfo = 'text'
            )
        
        # Añadir trazas individuales para cada movimiento
        for (i in 1:nrow(info$movements_info)) {
            fig <- fig %>% 
                add_trace(
                    data = info$movements_info[i, ],
                    x = ~c(startPointX, endPointX),
                    y = ~c(startPointY, endPointY),
                    type = 'scatter',
                    mode = 'lines+markers',
                    frame = ~timeStep,
                    line = list(color = species_color),
                    marker = list(size = 10, color = species_color, symbol = 'x'),
                    name = species_name,
                    legendgroup = species_name,  # Agrupar en la leyenda
                    showlegend = TRUE  # Mostrar en la leyenda
                )
        }

        # Añadir círculos alrededor de los puntos
        for (i in 1:nrow(info$animals_each_day_end_info)) {
            fig <- fig %>%
                add_trace(
                    type = 'scatter',
                    mode = 'lines',
                    x = generate_circle_x_coords(info$animals_each_day_end_info$X[[i]], radius = info$animals_each_day_end_info$interaction_area[[i]], grid_size),
                    y = generate_circle_y_coords(info$animals_each_day_end_info$Y[[i]], radius = info$animals_each_day_end_info$interaction_area[[i]], grid_size),
                    frame = info$animals_each_day_end_info$timeStep[[i]],
                    line = list(color = species_color, dash = 'dot'),
                    legendgroup = species_name,
                    showlegend = FALSE
                )
        }

        return(fig)
    }

    generate_circle_x_coords <- function(x_center, radius, grid_size, n_points = 100) {
        theta <- seq(0, 2 * pi, length.out = n_points)
        x <- x_center + radius * cos(theta)

        x <- pmax(0, pmin(x, grid_size))

        return(x)
    }

    generate_circle_y_coords <- function(y_center, radius, grid_size, n_points = 100) {
        theta <- seq(0, 2 * pi, length.out = n_points)
        y <- y_center + radius * sin(theta)

        y <- pmax(0, pmin(y, grid_size))

        return(y)
    }




    colors <- setNames(as.list(rainbow(length(resource_species)+length(animal_species))), c(resource_species, animal_species)) 


    start_time <- Sys.time()
    cells_info <- read_cells_info(config_path, time_step_interval)



    resource_species_info <- setNames(lapply(resource_species, function(species) {
        filtered_df <- cells_info[get(species) > 0, .(timeStep, X, Y, dry_mass = get(species), available_dry_mass = get(paste0(species, "_available_dry_mass")))]
        
        color_palette <- colorRamp(c("white", colors[[species]]))

        # Normalizar los valores del recurso
        normalized_values <- filtered_df$dry_mass / max(filtered_df$dry_mass)

        filtered_df[, color := rgb(color_palette(normalized_values), maxColorValue = 255)]

        return(filtered_df)
    }), resource_species)
    end_time <- Sys.time()
    print(paste("Tiempo de ejecución de read_cells_info:", end_time - start_time))


    grid_size <- max(cells_info$X)+1

    start_time <- Sys.time()
    fig <- show_grid(fig, grid_size)
    end_time <- Sys.time()
    print(paste("Tiempo de ejecución de show_grid:", end_time - start_time))


    start_time <- Sys.time()
    for (species in resource_species) {
        fig <- show_resource_species_info(fig, species, resource_species_info[[species]], colors[[species]])
    }
    end_time <- Sys.time()
    print(paste("Tiempo de ejecución de show_resource_species_info:", end_time - start_time))



    start_time <- Sys.time()
    animals_info <- read_animals_each_day_end_info(config_path, time_step_interval)


    animal_species_info <- setNames(lapply(animal_species, function(species) {
        filtered_animals_each_day_end_info <- animals_info$animals_each_day_end_info[species == species, .(timeStep, id, X, Y, state, instar, reproCounter, interaction_area)]

        filtered_movements_info <- animals_info$movements_info[species == species, ]

        return(list(animals_each_day_end_info=filtered_animals_each_day_end_info, movements_info=filtered_movements_info))
    }), animal_species)
    end_time <- Sys.time()
    print(paste("Tiempo de ejecución de read_animals_each_day_end_info:", end_time - start_time))



    start_time <- Sys.time()
    for (species in animal_species) {
        fig <- show_animal_species_info(fig, species, animal_species_info[[species]], colors[[species]], grid_size)
    }
    end_time <- Sys.time()
    print(paste("Tiempo de ejecución de show_animal_species_info:", end_time - start_time))



    return(fig)
}




fig <- plot_ly()

config_path <- file.path(script_path, "example_output_2")

resource_species <- c("Resource_river", "Resource_ground")

animal_species <- c("Amblyseius_swirskii")

fig <- show_world_info(fig, config_path, resource_species, animal_species, time_step_interval = 50)

start_time <- Sys.time()
# Configurar la animación
fig <- fig %>% animation_opts(
  frame = 100,
  transition = 0,
  redraw = FALSE
)
end_time <- Sys.time()
print(paste("Tiempo de ejecución de animation_opts:", end_time - start_time))


start_time <- Sys.time()
print(fig)
end_time <- Sys.time()
print(paste("Tiempo de ejecución de print:", end_time - start_time))



