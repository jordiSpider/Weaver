
rm(list=ls(all=TRUE))

setwd(dirname(sys.frame(1)$ofile))

source("optimiser_package.R")



WeaverOptimiser$add_parameters(
    c(
        WeaverOptimiser$RealValued$new(
            name = "resourceMaximumCapacityDensityAlternative", 
            min = 3.960043e-06 - 3.960043e-06*0.038, 
            max = 3.960043e-06 + 3.960043e-06*0.2,
            fixed_destinations = list(
                list(
                    file_path = "resource/patches/Alternative_patch.json", 
                    json_path = "patch$source$resourceMaximumCapacityDensity"
                ),
                list(
                    file_path = "resource/patches/Alternative_patch.json", 
                    json_path = "patch$source$growthDynamics$growthRateParams$initialValue"
                )
            ), 
            calculated_destinations = list(),
            constraints = list()
        ),
        # WeaverOptimiser$RealValued$new(
        #     name = "rMaxScaleBiofilm", 
        #     files = list(
        #         list(
        #             file_path = "resource/patches/Biofilm_patch.json", 
        #             json_path = "patch$source$growthDynamics$growthRateParams$rMaxScale"
        #         )
        #     ), 
        #     min = 2.0, 
        #     max = 5.0,
        #     constraints = list()
        # ),
        WeaverOptimiser$RealValued$new(
            name = "rMaxScaleAlternative", 
            min = 2.7, 
            max = 10.0,
            fixed_destinations = list(
                list(
                    file_path = "resource/patches/Alternative_patch.json", 
                    json_path = "patch$source$growthDynamics$growthRateParams$rMaxScale"
                )
            ),
            calculated_destinations = list(),
            constraints = list()
        ),
        WeaverOptimiser$RealValued$new(
            name = "cellEvaluationBiomassCricotopus", 
            min = 0.0, 
            max = 1.0,
            fixed_destinations = list(
                list(
                    file_path = "species/Cricotopus spx.json", 
                    json_path = "animal$genetics$traits$definition$cellValue$cellEvaluationBiomass$speciesLevelParams$value"
                )
            ), 
            calculated_destinations = list(),
            constraints = list()
        ),
        WeaverOptimiser$RealValued$new(
            name = "cellEvaluationBiomassIsoperla", 
            min = 0.0, 
            max = 1.0,
            fixed_destinations = list(
                list(
                    file_path = "species/Isoperla grammatica.json", 
                    json_path = "animal$genetics$traits$definition$cellValue$cellEvaluationBiomass$speciesLevelParams$value"
                )
            ), 
            calculated_destinations = list(),
            constraints = list()
        )
    )
)

WeaverOptimiser$add_parameters(
    c(
        lapply(seq(1, 13), function(instar) {
            WeaverOptimiser$RealValued$new(
                name = paste0("probabilityDeathFromBackground_", instar, "Isoperla"), 
                min = 0.001, 
                max = 0.005,
                fixed_destinations = list(
                    list(
                        file_path = "species/Isoperla grammatica.json", 
                        json_path = paste0("animal$probabilityDeathFromBackground[[", instar, "]]")
                    )
                ), 
                calculated_destinations = list(),
                constraints = list()
            )
        })
    )
)


oneMinValue = 0.0000000001
secondMinValue = 0.001

WeaverOptimiser$add_parameters(
    c(
        WeaverOptimiser$Preference$new(
            name = "preferenceIsoperlaCricotopus",
            cells = list(
                WeaverOptimiser$PreferenceCell$new(
                    predator_species = "Isoperla_grammatica",
                    predator_instars = seq(1, 13),
                    prey_species = "Cricotopus_spx",
                    prey_instars = seq(1, 4)
                )
            ),
            min = oneMinValue,
            max = secondMinValue
        ),
        WeaverOptimiser$Preference$new(
            name = "preferenceIsoperlaAlternative",
            cells = list(
                WeaverOptimiser$PreferenceCell$new(
                    predator_species = "Isoperla_grammatica",
                    predator_instars = seq(1, 13),
                    prey_species = "Alternative",
                    prey_instars = c(1)
                )
            ),
            min = 0.99-secondMinValue,
            max = 0.99-oneMinValue
        )
    )
)

WeaverOptimiser$add_parameters(
    c(
        WeaverOptimiser$Abundances$new(
            name = "cricotopusAbundances", 
            densitiesVector = c(0.0830996365, 0.0071765379, 0.0032205200, 0.0017584120, 0.0009995447),
            min = 200, 
            max = 1200,
            fixed_destinations = list(
                list(
                    file_path = "species/Cricotopus spx.json", 
                    json_path = "animal$individualsPerInstar"
                )
            ),
            constraints = list()
        )
    )
)

# WeaverOptimiser$add_parameters(
#     c(
#         WeaverOptimiser$RealValued$new(
#             name = "isoperlaSearchArea", 
#             min = 44.26, 
#             max = 177.06,
#             fixed_destinations = list(
#                 list(
#                     file_path = "species/Isoperla grammatica.json", 
#                     json_path = "animal$genetics$traits$definition$search_area$speciesLevelParams$value"
#                 )
#             ), 
#             calculated_destinations = list(
#                 list(
#                     "function" = function(individual, json_data) {
#                         search_area_ini <- individual[["isoperlaSearchArea"]]

#                         femaleWetMass = json_data$animal$growthModule$femaleWetMass

#                         search_area = search_area_ini*femaleWetMass^0.25

#                         scope_area = exp(0.75+log(search_area))

#                         scope_area_ini = scope_area/(femaleWetMass^0.25)

#                         return(scope_area_ini)
#                     },
#                     files = list(
#                         list(
#                             file_path = "species/Isoperla grammatica.json", 
#                             json_path = "animal$genetics$traits$definition$scope_area$speciesLevelParams$value"
#                         )
#                     )
#                 ),
#                 list(
#                     "function" = function(individual, json_data) {
#                         search_area_ini <- individual[["isoperlaSearchArea"]]

#                         femaleWetMass = json_data$animal$growthModule$femaleWetMass

#                         search_area = search_area_ini*femaleWetMass^0.25

#                         interaction_area = exp(0.35+log(search_area))

#                         interaction_area_ini = interaction_area/(femaleWetMass^0.25)

#                         return(interaction_area_ini)
#                     },
#                     files = list(
#                         list(
#                             file_path = "species/Isoperla grammatica.json", 
#                             json_path = "animal$genetics$traits$definition$interaction_area$speciesLevelParams$value"
#                         )
#                     )
#                 )
#             ),
#             constraints = list()
#         )
#     )
# )



fitness_function_totalTime <- function(result_folder) {
    
    breakConditionsInfo <- read.table(file.path(result_folder, "breakConditionsInfo.txt") , header = TRUE, sep = "\t", stringsAsFactors = FALSE)

    return(breakConditionsInfo$totalTime)
}

fitness_function_minAbundance <- function(result_folder) {

    df <- read.delim(file.path(result_folder, "extendedDailySummary.txt"))

    # Mostrar los nombres de las columnas
    cols <- colnames(df)

    active_cols <- grep("_ACTIVE$", cols, value = TRUE)

    # Extrae solo la parte de la especie, eliminando el sufijo "_ACTIVE"
    species <- sub("_ACTIVE$", "", active_cols)

    lifeStages <- c("UNBORN", "ACTIVE", "REPRODUCING", "PUPA", "SATIATED", "HANDLING", "DIAPAUSE")

    sum_per_species <- lapply(species, function(e) {
        # Construir los nombres de columna para esta especie
        col_names <- paste0(e, "_", lifeStages)

        # Solo conservar los nombres que están realmente en el dataframe
        existing_cols <- col_names[col_names %in% colnames(df)]

        # Calcular suma por fila
        rowSums(df[, existing_cols], na.rm = TRUE)
    })

    # Convertir la lista en un data.frame: cada columna es la suma por especie
    sum_df <- as.data.frame(sum_per_species)
    names(sum_df) <- species

    # Obtener, para cada fila, el mínimo entre las especies
    min_per_row <- apply(sum_df, 1, min, na.rm = TRUE)

    # Finalmente, el mínimo global
    minAbundance <- min(min_per_row, na.rm = TRUE)

    return(minAbundance)
}

fitness_function_coefficientOfVariation <- function(result_folder) {

    breakConditionsInfo <- read.table(file.path(result_folder, "breakConditionsInfo.txt") , header = TRUE, sep = "\t", stringsAsFactors = FALSE)

    return(breakConditionsInfo$coefficientOfVariation)
}



WeaverOptimiser$run(
    population_size = 45, 
    max_generations = 20,
    fitness_class = WeaverOptimiser$MultiObjectiveFitness$new(
        objectives = list(
            WeaverOptimiser$Objective$new(
                name = "totalTime",
                minimise = FALSE,
                fitness_function = fitness_function_totalTime
            ),
            WeaverOptimiser$Objective$new(
                name = "minAbundance",
                minimise = FALSE,
                fitness_function = fitness_function_minAbundance
            ),
            WeaverOptimiser$Objective$new(
                name = "coefficientOfVariation", 
                minimise = TRUE,
                fitness_function = fitness_function_coefficientOfVariation
            )
        ),
        order = c("totalTime", "minAbundance", "coefficientOfVariation")
    ), 
    program = "C:\\Users\\mcs20\\Desktop\\EEZA\\Weaver\\bin\\Weaver-Non-GUI-fast.exe",
    output_path = "C:\\Users\\mcs20\\Desktop\\EEZA\\Weaver\\output", 
    base_config_path = "C:\\Users\\mcs20\\Desktop\\EEZA\\Weaver\\config\\config_Roman_app", 
    keepBest = TRUE,
    tournament_size = 5,
    crossover_prob = 0.8, 
    mutation_rate = 0.05, 
    seed = 0, 
    num_cores = 45,
    debug = TRUE
)
