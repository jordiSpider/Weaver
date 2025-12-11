rm(list=ls(all=TRUE))

# Ajustar el directorio de trabajo de forma robusta
setwd(dirname(sys.frame(1)$ofile))

library(ggplot2)
library(ggnewscale)
library(ggforce)
library(dplyr)
library(purrr)
library(tidyr)
library(data.table)
library(jsonlite)
library(viridis)


# Crear datos en formato largo
set.seed(42)







visualizeWorld <- function(config_path, animal_species, resource_species, time_step_interval, x_axis_size, y_axis_size) {
    
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

    process_resource_cells_info <- function(info) {
        # Realizar un pivot_longer en cells_info
        info <- info %>%
            split(1:nrow(.)) %>%  # Separa cada fila en una lista
            map_dfr(~ {
                data.frame(
                X = .x$X,
                Y = .x$Y,
                Resource = resource_species,
                DryMass = sapply(resource_species, function(resource) {.x[[resource]]}),
                AvailableDryMass = sapply(resource_species, function(resource) {.x[[paste0(resource, "_available_dry_mass")]]})
                )
            })

        # Calcular el número de filas por coordenada
        info <- info %>%
            filter(AvailableDryMass > 0) %>%  
            group_by(X, Y) %>%
            mutate(
                xmin = X * cell_size + ((row_number() - 1) / n_distinct(Resource)) * cell_size,  
                xmax = X * cell_size + ((row_number()) / n_distinct(Resource)) * cell_size,  
                ymin = Y * cell_size,  # Alineamos con la base de la celda
                ymax = Y * cell_size + cell_size
            )
        
        return(info)
    }

    plot_resource_cells_info <- function(plot, resource_species) {
        # Crear una paleta de colores distinta para los recursos
        resource_colors <- setNames(viridis(length(resource_species), option = "D"), resource_species)

        for(resource in resource_species) {
            plot <- plot + 
                geom_rect(data=resource_cells_info %>% filter(Resource == resource), 
                            aes(xmin=xmin, xmax=xmax, ymin=ymin, ymax=ymax, fill=AvailableDryMass)) +
                scale_fill_gradient(low="white", high=resource_colors[[resource]], name=resource) +
                new_scale_fill()
        }

        return(plot)
    }

    plot_animal_cells_info <- function(plot, animal_cells_info, animal_species) {
        # Realizar un pivot_longer en animal_cells_info
        animal_cells_info_long <- animal_cells_info %>%
            pivot_longer(
                cols = all_of(animal_species),
                names_to = "Animal",
                values_to = "Abundance"
            ) %>%
            filter(Abundance > 0)  # Filtrar animales con abundancia mayor a 0

        # Crear una paleta de colores distinta para los animales
        animal_colors <- setNames(viridis(length(animal_species), option = "C"), animal_species)

        # Agregar puntos al gráfico
        max_abundance <- max(animal_cells_info_long$Abundance, na.rm = TRUE)  # Obtener la abundancia máxima
        animal_cells_info_long <- animal_cells_info_long %>%
            mutate(
                scaled_size = (Abundance / max_abundance) * cell_size  # Escalar el radio dentro del área de la celda
            )

        print(animal_cells_info_long)

        for(animal in animal_species) {
            plot <- plot + 
                geom_point(data=animal_cells_info_long %>% filter(Animal == animal), 
                            aes(x=X * cell_size + cell_size / 2, y=Y * cell_size + cell_size / 2, size = scaled_size, color=animal_colors[[animal]]))
        }

        # plot <- plot +
        #     geom_point(
        #         data = animal_cells_info_long,
        #         aes(
        #             x = X * cell_size + cell_size / 2,
        #             y = Y * cell_size + cell_size / 2,
        #             size = scaled_size,
        #             color = Animal
        #         ),
        #         alpha = 1.0
        #     ) +
        #     coord_fixed(ratio = 1) +
        #     scale_size_continuous(name = "Abundance") +
        #     scale_color_manual(name = "Animal", values = animal_colors)  # Usar colores distintos para cada animal

        return(plot)
    }


    cells_info <- read_cells_info(config_path, time_step_interval)

    cells_info <- cells_info %>%
    filter(timeStep == 0)

    # Dividir cells_info en dos data frames separados
    animal_cells_info <- cells_info %>%
      select(X, Y, timeStep, one_of(animal_species))

    resource_cells_info <- cells_info %>%
      select(X, Y, timeStep, one_of(resource_species), one_of(paste0(resource_species, "_available_dry_mass")))


    colors <- setNames(viridis(length(resource_species)), resource_species) 


    world_params <- fromJSON(file.path(config_path, "config", "world_params.json"), simplifyVector = FALSE)

    cell_size <- world_params$world$mapConfig$worldWideParams$minCellSize
    grid_size <- world_params$world$mapConfig$worldWideParams$numberOfCellsPerAxis


    x_axis_num_cells = min(ceiling(x_axis_size/cell_size), grid_size)
    y_axis_num_cells = min(ceiling(y_axis_size/cell_size), grid_size)



    resource_cells_info <- process_resource_cells_info(resource_cells_info)
    



    plot <- ggplot()

    plot <- plot_resource_cells_info(plot, resource_species)

    plot <- plot + 
        # Dibujar un borde negro alrededor de cada celda
        geom_rect(data=expand.grid(X = 0:(x_axis_num_cells - 1), Y = 0:(y_axis_num_cells - 1)), 
                aes(xmin=X*cell_size, xmax=(X+1)*cell_size, ymin=Y*cell_size, ymax=(Y+1)*cell_size), 
                fill=NA, color="black") +  # Borde negro sin relleno
        labs(title="Distribución de Recursos en el Mismo Gráfico") +
        scale_x_continuous(expand=c(0,0), breaks=seq(0, x_axis_num_cells*cell_size, cell_size)) + 
        scale_y_continuous(expand=c(0,0), breaks=seq(0, y_axis_num_cells*cell_size, cell_size)) +
        scale_size_identity(breaks=seq(0, y_axis_num_cells*cell_size, cell_size)) +
        coord_fixed() +
        theme(
            axis.text.x = element_text(angle = 45, hjust = 1),  # Girar texto de los ticks del eje X
            axis.text = element_text(size=8)  # Reducir tamaño del texto de los ejes
        )

    # Agregar los puntos de abundancia de los animales al gráfico
    plot <- plot_animal_cells_info(plot, animal_cells_info, animal_species)

    # Mostrar la gráfica
    print(plot)
}



config_path <- "C:/Users/mcs20/Desktop/EEZA/Weaver/output/encarni_graph"

animal_species <- c("Cricotopus_spx", "Isoperla_grammatica")

resource_species <- c("Biofilm", "Plant1", "Plant2")


time_step_interval <- 1


visualizeWorld(config_path, animal_species, resource_species, time_step_interval, 1250, 1000)


