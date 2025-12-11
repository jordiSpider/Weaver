
rm(list=ls(all=TRUE))

setwd(dirname(sys.frame(1)$ofile))

source("optimiser_package.R")



WeaverOptimiser$add_parameters(
    c(
        WeaverOptimiser$RealValued$new(
            name = "resourceMaximumCapacityDensityAlternative", 
            files = list(
                list(
                    file_path = "resource/patches/Alternative_patch.json", 
                    json_path = "patch$resourceMaximumCapacityDensity"
                ),
                list(
                    file_path = "resource/patches/Alternative_patch.json", 
                    json_path = "patch$initialBiomassDensity"
                )
            ), 
            min = 3.960043e-06 - 3.960043e-06*0.038, 
            max = 3.960043e-06 + 3.960043e-06*0.2,
            constraints = list()
        ),
        WeaverOptimiser$RealValued$new(
            name = "rMaxScaleBiofilm", 
            files = list(
                list(
                    file_path = "resource/Biofilm.json", 
                    json_path = "resource$rMaxScale"
                )
            ), 
            min = 2.0, 
            max = 5.0,
            constraints = list()
        ),
        WeaverOptimiser$RealValued$new(
            name = "rMaxScaleAlternative", 
            files = list(
                list(
                    file_path = "resource/Alternative.json", 
                    json_path = "resource$rMaxScale"
                )
            ), 
            min = 2.7, 
            max = 10.0,
            constraints = list()
        ),
        WeaverOptimiser$RealValued$new(
            name = "cellEvaluationBiomassCricotopus", 
            files = list(
                list(
                    file_path = "species/Cricotopus spx.json", 
                    json_path = "animal$cellEvaluationBiomass"
                )
            ), 
            min = 0.0, 
            max = 1.0,
            constraints = list()
        ),
        WeaverOptimiser$RealValued$new(
            name = "cellEvaluationRiskCricotopus", 
            files = list(
                list(
                    file_path = "species/Cricotopus spx.json", 
                    json_path = "animal$cellEvaluationRisk"
                )
            ), 
            min = 0.0, 
            max = 1.0,
            constraints = list()
        ),
        WeaverOptimiser$RealValued$new(
            name = "cellEvaluationBiomassIsoperla", 
            files = list(
                list(
                    file_path = "species/Isoperla grammatica.json", 
                    json_path = "animal$cellEvaluationBiomass"
                )
            ), 
            min = 0.0, 
            max = 1.0,
            constraints = list()
        )
    )
)

WeaverOptimiser$add_parameters(
    c(
        lapply(seq(1, 13), function(instar) {
            WeaverOptimiser$RealValued$new(
                name = paste0("probabilityDeathFromBackground_", instar, "Isoperla"), 
                files = list(
                    list(
                        file_path = "species/Isoperla grammatica.json", 
                        json_path = paste0("animal$probabilityDeathFromBackground[[", instar, "]]")
                    )
                ), 
                min = 0.001, 
                max = 0.005,
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
            files = list(
                list(
                    file_path = "species/Cricotopus spx.json", 
                    json_path = "animal$individualsPerInstar"
                )
            ), 
            densitiesVector = c(0.0830996365, 0.0071765379, 0.0032205200, 0.0017584120, 0.0009995447),
            min = 200, 
            max = 1200,
            constraints = list()
        )
    )
)



fitness_function <- function(result_folder) {
    breakConditionsInfo <- read.table(file.path(result_folder, "breakConditionsInfo.txt") , header = TRUE, sep = "\t", stringsAsFactors = FALSE)

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
        # Calcular suma por fila
        rowSums(df[, col_names], na.rm = TRUE)
    })

    # Convertir la lista en un data.frame: cada columna es la suma por especie
    sum_df <- as.data.frame(sum_per_species)
    names(sum_df) <- species

    # Obtener, para cada fila, el mínimo entre las especies
    min_per_row <- apply(sum_df, 1, min, na.rm = TRUE)

    # Finalmente, el mínimo global
    minAbundance <- min(min_per_row, na.rm = TRUE)

    return(list(
        coefficientOfVariation = breakConditionsInfo$coefficientOfVariation, 
        minAbundanceInv = 1.0/minAbundance,
        totalTimeInv = 1.0/breakConditionsInfo$totalTime
    ))
}



WeaverOptimiser$run(
    minimise = TRUE, 
    population_size = 45, 
    max_generations = 20,
    fitness_class = WeaverOptimiser$MultiObjectiveFitness$new(
        objectives_order = c("totalTimeInv", "minAbundanceInv", "coefficientOfVariation")
    ),
    fitness_function = fitness_function, 
    program = "C:\\Users\\mario\\Desktop\\Weaver\\bin\\Release\\Weaver-Non-GUI.exe",
    output_path = "E:\\pruebas_optimizacion", 
    base_config_path = "C:\\Users\\mario\\Desktop\\Weaver\\config_proto_stability", 
    keepBest = TRUE,
    tournament_size = 5,
    crossover_prob = 0.8, 
    mutation_rate = 0.05, 
    seed = 0, 
    num_cores = 45,
    debug = TRUE
)
