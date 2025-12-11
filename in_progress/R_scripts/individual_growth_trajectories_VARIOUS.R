
library(ggplot2)
library(plotly)
library(htmlwidgets)


####################
#                  #
#      Inputs      #
#                  #
####################

output_path <- "/home/macarse/Escritorio/EEZA/Weaver/output/encarni_config__2025-3-11_12-38-47"

ids_out=c()



#######################
#                     #
#      Read data      #
#                     #
#######################

output_path <- normalizePath(output_path)




animals_each_day_end_path <- file.path(output_path, "animals_each_day_end")

animals_each_day_end_dir <- dir(animals_each_day_end_path, pattern = "\\.txt$", full.names = TRUE)

animals_each_day_end_info <- do.call(rbind, lapply(animals_each_day_end_dir, function(file) {
    data <- read.table(file,h=T)

    if(nrow(data) > 0){
        data$timeStep <- as.integer(sub(".*animals_day_([0-9]+)\\.txt", "\\1", file))

        data$label <- paste("Time Step: ", data$timeStep,
            "\nLife Stage: ", data$state,
            "\nInstar: ", data$instar,
            "\nAge: ", data$age,
            "\nRepro Counter: ", data$reproCounter,
            "\nDry Mass: ", data$currentDryMass, 
            "\nBody Size: ", data$currentBodySize, 
            "\nEnergy Tank: ", data$currentEnergyTank)
    }

    return(data)
}))

animals_each_day_end_info <- animals_each_day_end_info[order(animals_each_day_end_info$timeStep), ]

names(animals_each_day_end_info)




animals_each_day_voracities_path <- file.path(output_path, "animals_each_day_voracities")

animals_each_day_voracities_dir <- dir(animals_each_day_voracities_path, pattern = "\\.txt$", full.names = TRUE)

animals_each_day_voracities_info <- do.call(rbind, lapply(animals_each_day_voracities_dir, function(file) {
    data <- read.table(file,h=T)
    
    if(nrow(data) > 0){
        data$timeStep <- as.integer(sub(".*animals_voracities_day_([0-9]+)\\.txt", "\\1", file))

        data$label <- paste("Time Step: ", data$timeStep,
            "\nLife Stage: ", data$state,
            "\nInstar: ", data$instar,
            "\nRepro Counter: ", data$reproCounter,
            "\nDry Mass: ", data$dryMass, 
            "\nBody Size: ", data$body_size, 
            "\nEnergy Tank: ", data$energy_tank)
    }

    return(data)
}))

animals_each_day_voracities_info <- animals_each_day_voracities_info[animals_each_day_voracities_info$timeStep <= max(animals_each_day_end_info$timeStep), ]

animals_each_day_voracities_info <- animals_each_day_voracities_info[order(animals_each_day_voracities_info$timeStep), ]

names(animals_each_day_voracities_info)




ids<-unique(animals_each_day_end_info$id)

ids<-ids[order(ids)]

ids<-ids[!ids %in% ids_out]

ids




animals_info <- lapply(ids, function(current_id) {
    animals_each_day_end_info[animals_each_day_end_info$id == current_id, ]
})

names(animals_info) <- ids




animals_voracities_info <- lapply(ids, function(current_id) {
    animals_each_day_voracities_info[animals_each_day_voracities_info$id == current_id, ]
})

names(animals_voracities_info) <- ids




animals_moulting <- lapply(ids, function(current_id) {
    data <- animals_each_day_end_info[animals_each_day_end_info$id == current_id, c("instar", "timeStep")]
    data <- data[(diff(data$instar) != 0) == TRUE, ]
    data$timeStep <- data$timeStep + 1

    return(data)
})

names(animals_moulting) <- ids




animals_reproduction <- lapply(ids, function(current_id) {
    data <- animals_each_day_end_info[animals_each_day_end_info$id == current_id, c("reproCounter", "timeStep")]
    data <- data[(diff(data$reproCounter) != 0) == TRUE, ]
    data$timeStep <- data$timeStep + 1

    return(data)
})

names(animals_reproduction) <- ids



##############################
#                            #
#      Display graphics      #
#                            #
##############################


selected_id = 615

if(!selected_id %in% ids){
    stop(paste("No ID available. Available IDs:", paste(ids, collapse = ", ")))
}



# grafico_ggplot <- ggplot(animals_voracities_info[[as.character(selected_id)]], aes(x = timeStep)) +
#     # Área de energy_tank (desde el valor de body_size)
#     geom_area(aes(y = body_size + energy_tank, fill = "Energy Tank"), alpha = 1) +
#     # Área de body_size (desde 0)
#     geom_area(aes(y = body_size, fill = "Body Size"), alpha = 1) +
#     # Línea de dryMass
#     geom_line(aes(y = dryMass, color = "Dry Mass"), linewidth = 1) +
#     # Puntos de dryMass
#     geom_point(aes(y = dryMass, color = "Dry Mass", text = label), size = 2) +
#      # Línea de nextMass
#     geom_line(aes(y = nextDinoMass, color = "Next Mass"), linewidth = 1) +
#     # Puntos de nextMass
#     geom_point(aes(y = nextDinoMass, color = "Next Mass"), size = 2) +
#     # Añadir líneas verticales donde se detecta el cambio
#     geom_vline(data = animals_moulting[[as.character(selected_id)]], aes(xintercept = timeStep, color = "Moulting"), 
#         linetype = "dashed", linewidth = 1) +
#     # Añadir líneas verticales donde se detecta el cambio
#     geom_vline(data = animals_reproduction[[as.character(selected_id)]], aes(xintercept = timeStep, color = "Reproduction"), 
#         linetype = "dashed", linewidth = 1) +
#     labs(
#         title = "Dry Mass, Body Size, y Energy Tank",
#         x = "Time Step",
#         y = "Dry Mass"
#     ) +
#     scale_color_manual(values = c("Dry Mass" = "blue", "Next Mass" = "orange","Reproduction" = "yellow", "Moulting" = "pink")) +
#     scale_fill_manual(values = c("Body Size" = "red", "Energy Tank" = "green")) +
#     theme_minimal()


grafico_ggplot <- ggplot(animals_info[[as.character(selected_id)]], aes(x = timeStep)) +
    # Área de energy_tank (desde el valor de body_size)
    geom_area(aes(y = currentBodySize + currentEnergyTank, fill = "Energy Tank"), alpha = 1) +
    # Área de body_size (desde 0)
    geom_area(aes(y = currentBodySize, fill = "Body Size"), alpha = 1) +
    # Línea de dryMass
    geom_line(aes(y = currentDryMass, color = "Dry Mass"), linewidth = 1) +
    # Puntos de dryMass
    geom_point(aes(y = currentDryMass, color = "Dry Mass", text = label), size = 2) +
     # Línea de nextMass
    #geom_line(aes(y = nextDinoMass, color = "Next Mass"), linewidth = 1) +
    # Puntos de nextMass
    #geom_point(aes(y = nextDinoMass, color = "Next Mass"), size = 2) +
    # Añadir líneas verticales donde se detecta el cambio
    geom_vline(data = animals_moulting[[as.character(selected_id)]], aes(xintercept = timeStep, color = "Moulting"), 
        linetype = "dashed", linewidth = 1) +
    # Añadir líneas verticales donde se detecta el cambio
    geom_vline(data = animals_reproduction[[as.character(selected_id)]], aes(xintercept = timeStep, color = "Reproduction"), 
        linetype = "dashed", linewidth = 1) +
    labs(
        title = "Dry Mass, Body Size, y Energy Tank",
        x = "Time Step",
        y = "Dry Mass"
    ) +
    scale_color_manual(values = c("Dry Mass" = "blue", "Next Mass" = "orange","Reproduction" = "yellow", "Moulting" = "pink")) +
    scale_fill_manual(values = c("Body Size" = "red", "Energy Tank" = "green")) +
    theme_minimal()


grafico_interactivo <- ggplotly(grafico_ggplot, tooltip = "text")

output_file <- "grafico_interactivo.html"
saveWidget(grafico_interactivo, output_file, selfcontained = FALSE)

browseURL(output_file)





grafico_ggplot2 <- ggplot(animals_voracities_info[[as.character(selected_id)]], aes(x = timeStep)) +
    geom_line(aes(y = after_encounters_voracity, color = "after_encounters_voracity"), linewidth = 1) +
    geom_point(aes(y = after_encounters_voracity, color = "after_encounters_voracity"), size = 2) +
    geom_line(aes(y = food_mass, color = "food_mass"), linewidth = 1) +
    geom_point(aes(y = food_mass, color = "food_mass"), size = 2) +
    scale_color_manual(values = c("after_encounters_voracity" = "blue", "food_mass" = "red")) +
    theme_minimal()

grafico_interactivo2 <- ggplotly(grafico_ggplot2, tooltip = "text")

output_file <- "grafico_interactivo2.html"
saveWidget(grafico_interactivo2, output_file, selfcontained = FALSE)

browseURL(output_file)



grafico_ggplot3 <- ggplot(animals_voracities_info[[as.character(selected_id)]], aes(x = timeStep)) +
    geom_line(aes(y = after_encounters_search, color = "after_encounters_search"), linewidth = 1) +
    geom_point(aes(y = after_encounters_search, color = "after_encounters_search"), size = 2) +
    geom_line(aes(y = steps, color = "steps"), linewidth = 1) +
    geom_point(aes(y = steps, color = "steps"), size = 2) +
    scale_color_manual(values = c("after_encounters_search" = "blue", "steps" = "red")) +
    theme_minimal()

grafico_interactivo3 <- ggplotly(grafico_ggplot3, tooltip = "text")

output_file <- "grafico_interactivo3.html"
saveWidget(grafico_interactivo3, output_file, selfcontained = FALSE)

browseURL(output_file)



# aux <- animals_each_day_voracities_info[animals_each_day_voracities_info$id %in% c(29880, 29889),c("timeStep", "dryMass", "id")]


# grafico_ggplot <- ggplot(animals_each_day_voracities_info, aes(x = timeStep, y = dryMass, color = id, group = id)) +
#     geom_line() +
#     geom_point() +
#     labs(title = "Serie Temporal", x = "timeStep", y = "dryMass") +
#     theme_minimal()


# # Convertir el gráfico de ggplot2 a interactivo con plotly
# grafico_interactivo <- ggplotly(grafico_ggplot)


# dropdown_buttons <- lapply(ids, function(current_id) {
#     list(
#         label = paste("Mostrar Categoría",current_id),
#         method = "update",
#         args = list(
#         list(visible = ids == current_id),  # Solo se muestran los puntos de la categoría A
#         list(title = paste("Datos de Categoría",current_id))
#         )
#     )
# })




# grafico_interactivo <- grafico_interactivo %>%
#     layout(
#         title = "Gráfico interactivo con Dropdown",
#         updatemenus = list(
#         list(
#             type = "dropdown",
#             active = 0,
#             buttons = dropdown_buttons,
#             direction = "down",
#             showactive = TRUE,
#             x = 0.1, y = 1.1
#         )
#         )
#     )

# # Mostrar el gráfico interactivo
# output_file <- "grafico_interactivo.html"
# saveWidget(grafico_interactivo, output_file, selfcontained = FALSE)

# browseURL(output_file)


################################################################################
################################################################################
################################################################################


# # Crear un data.frame de ejemplo con varias categorías
# df <- data.frame(
#   day = rep(1:10, 3),
#   value = c(rnorm(10, 10, 2), rnorm(10, 20, 3), rnorm(10, 30, 4)),
#   category = rep(c("A", "B", "C"), each = 10)
# )

# # Lista para almacenar los gráficos
# plot_list <- list()

# # Crear un gráfico para cada categoría
# for(cat in unique(df$category)) {
#   data_cat <- subset(df, category == cat)
  
#   # Crear el gráfico para cada categoría con ggplot
#   p <- ggplot(data_cat, aes(x = day, y = value, color = category, group = category)) +
#     geom_line() +  # Línea para cada grupo (categoría)
#     geom_point() +  # Puntos en la línea
#     labs(title = paste("Categoría", cat)) +
#     theme_minimal()
  
#   # Convertirlo a plotly y agregarlo a la lista
#   plot_list[[cat]] <- ggplotly(p)
# }

# # Combinar todos los gráficos en un solo gráfico con subplot
# combined_plot <- subplot(plot_list, nrows = 1, shareX = TRUE, shareY = TRUE)

# # Mostrar el gráfico combinado
# combined_plot


# # Mostrar el gráfico interactivo
# output_file <- "grafico_interactivo.html"
# saveWidget(combined_plot, output_file, selfcontained = FALSE)

# browseURL(output_file)

