#' Execute Weaver Optimization Process (Unified Wrapper Pipeline)
#'
#' @param blueprint Estructura de lista R o cadena string raw con el JSON de configuración unificado.
#' @param output_path Ruta del directorio raíz donde se volcarán los resultados y telemetrías.
#' @param program Ruta absoluta al binario del simulador ejecutable (WeaverCLI.exe).
#' @param base_config_path Ruta al directorio que contiene los archivos base de configuración (.json y .csv).
#' @param seed Semilla para la reproducibilidad del algoritmo evolutivo.
#' @param total_cores Número total de núcleos disponibles para la ejecución.
#' @param program_cores Número de núcleos asignados al programa simulador.
#' @param metrics_callback Función de devolución de llamada: function(generation, current_best, current_mean, log_line, log_type).
#' @param debug Bandera para activar el modo de depuración.
#' @export
run_optimiser <- function(blueprint, output_path, program, base_config_path, seed, total_cores, program_cores, metrics_callback, evolution_callback, debug=FALSE) {
  # 1. Instanciación en un microsegundo de memoria de la clase R6 de forma autolimpiable
  engine_instance <- WeaverOptimiserClass$new()
  
  # 2. Delegación inmediata de la ejecución del algoritmo evolutivo
  engine_instance$run(
    blueprint = blueprint,
    output_path            = output_path,
    program     = program,
    base_config_path       = base_config_path,
    seed = seed,
    total_cores            = total_cores,
    program_cores          = program_cores,
    metrics_callback       = metrics_callback,
    evolution_callback     = evolution_callback,
    debug                  = debug
  )
}

#' Weaver Ecosystem Multi-Objective Parameter Tying Genetic Solver Engine
#' @description Core computational class processing stochastic matrix balances.
WeaverOptimiserClass <- R6::R6Class("WeaverOptimiserClass",
    private = list(
        # --- ATRIBUTOS DE CONFIGURACIÓN INTERNA ---
        optimise_preferences   = FALSE,
        optimise_profitability = FALSE,
        preferences_groups     = list(),
        profitability_groups   = list(),
        preferences_constraints = NULL,

        current_seed           = NULL, 
        
        # Almacén plano unificado de las variables de Parameter Tying creadas en la Fase 2
        parameters             = list(),  
        
        combination_history    = dplyr::tibble(id = integer()),
        combination_results    = list(),
        parameters_constraints_level = list(),
        program_cores          = NULL,
        debug                  = NULL,
        log_file               = NULL,
        evolution_history      = NULL,
        
        # Callback funcional inyectado dinámicamente para los logs y progreso
        metrics_dispatcher     = NULL,
        evolution_dispatcher   = NULL,

        set_debug_mode = function(debug) {
            private$debug = debug
        },

        Parameter = R6::R6Class("Parameter",
            public = list(
                initialize = function() {
                    stop("Cannot instantiate the base class directly")
                }
            )
        ),
        
        RealValued = R6::R6Class("RealValued", inherit = WeaverOptimiserClass$Parameter,
            private = list(
                name = character(0), 
                min = numeric(0),
                max = numeric(0), 
                fixed_destinations = list(),
                calculated_destinations = list(),
                constraints = list()
            ),
            public = list(
                initialize = function(name, min, max, fixed_destinations, calculated_destinations, constraints) {
                    private$name <- name

                    private$min <- min
                    private$max <- max

                    private$fixed_destinations <- fixed_destinations
                    private$calculated_destinations <- calculated_destinations
                
                    for(i in seq_along(private$fixed_destinations)) {
                        file_path <- strsplit(private$fixed_destinations[[i]]$file_path, "/")[[1]]
                        file_path <- do.call(file.path, as.list(file_path))

                        private$fixed_destinations[[i]]$file_path <- file_path
                    }

                    for(i in seq_along(private$calculated_destinations)) {
                        for(j in seq_along(private$calculated_destinations[[i]]$files)) {
                            file_path <- strsplit(private$calculated_destinations[[i]]$files[[j]]$file_path, "/")[[1]]
                            file_path <- do.call(file.path, as.list(file_path))

                            private$calculated_destinations[[i]]$files[[j]]$file_path <- file_path
                        }
                    }

                    private$constraints <- constraints
                },

                calculate_constraints_level = function(parameters) {
                    constraints_level = 0

                    for(constraint in private$constraints) {
                        constraints_level = max(constraints_level, parameters[[constraint$name]]$calculate_constraints_level(parameters) + 1)
                    }

                    return(constraints_level)
                },

                generate_random_value = function(parameters, individual) {
                    minAux = private$min
                    maxAux = private$max

                    for(constraint in private$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                minAux = individual[[constraint$name]]
                                maxAux = individual[[constraint$name]]
                            } else {
                                minAux = constraint$min
                                maxAux = constraint$max
                            }
                        }
                    }

                    return(runif(1, minAux, maxAux))
                },

                generate_mutation = function(parameters, individual) {
                    new_value <- self$generate_random_value(parameters, individual)
            
                    while (new_value == individual[[private$name]]) {
                        new_value <- self$generate_random_value(parameters, individual)
                    }

                    return(new_value)
                },

                constraints_breached = function(individual) {
                    minAux = private$min
                    maxAux = private$max

                    for(constraint in private$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                minAux = individual[[constraint$name]]
                                maxAux = individual[[constraint$name]]
                            } else {
                                minAux = constraint$min
                                maxAux = constraint$max
                            }
                        }
                    }

                    return(individual[[private$name]] < minAux || maxAux < individual[[private$name]])
                },

                show = function() {
                    lines <- c()
                    lines <- c(lines, paste0("  Parameter Name: \"", private$name, "\""))
                    lines <- c(lines, paste0("  Type: ", class(self)[1]))
                    lines <- c(lines, paste0("  Minimum: ", private$min))
                    lines <- c(lines, paste0("  Maximum: ", private$max))
                    lines <- c(lines, "  Fixed destinations:")
                    for(i in seq_along(private$fixed_destinations)) {
                        lines <- c(lines, "   /")
                        lines <- c(lines, paste0("  | File ", i))
                        lines <- c(lines, paste0("  | File path: \"", private$fixed_destinations[[i]]$file_path, "\""))
                        lines <- c(lines, paste0("  | JSON path: \"", private$fixed_destinations[[i]]$json_path, "\""))
                        lines <- c(lines, "   \\")
                    }
                    lines <- c(lines, "  Calculated destinations:")
                    for(i in seq_along(private$calculated_destinations)) {
                        lines <- c(lines, "   /")
                        lines <- c(lines, paste0("  | Destination ", i))
                        lines <- c(lines, "  | Function: function(individual, json_data)")
                        lines <- c(lines, paste0("  | ", paste(deparse(body(private$calculated_destinations[[i]][["function"]])), collapse = "\n  | ")))
                        lines <- c(lines, "  | Files:")
                        for(j in seq_along(private$calculated_destinations[[i]]$files)) {
                            lines <- c(lines, "  |  /")
                            lines <- c(lines, paste0("  | | File ", j))
                            lines <- c(lines, paste0("  | | File path: \"", private$calculated_destinations[[i]]$files[[j]]$file_path, "\""))
                            lines <- c(lines, paste0("  | | JSON path: \"", private$calculated_destinations[[i]]$files[[j]]$json_path, "\""))
                            lines <- c(lines, "  |  \\")
                        }
                        lines <- c(lines, "   \\")
                    }
                    return(lines)       
                }
            )
        ),

        Integer = R6::R6Class("Integer", inherit = WeaverOptimiserClass$Parameter,
            private = list(
                name = character(0),
                min = numeric(0),
                max = numeric(0),
                fixed_destinations = list(),
                calculated_destinations = list(),
                constraints = list()
            ),
            public = list(
                initialize = function(name, min, max, fixed_destinations, calculated_destinations, constraints) {
                    private$name <- name

                    private$min <- min
                    private$max <- max

                    private$fixed_destinations <- fixed_destinations
                    private$calculated_destinations <- calculated_destinations

                    for(i in seq_along(private$fixed_destinations)) {
                        file_path <- strsplit(private$fixed_destinations[[i]]$file_path, "/")[[1]]
                        file_path <- do.call(file.path, as.list(file_path))

                        private$fixed_destinations[[i]]$file_path <- file_path
                    }

                    for(i in seq_along(private$calculated_destinations)) {
                        for(j in seq_along(private$calculated_destinations[[i]]$files)) {
                            file_path <- strsplit(private$calculated_destinations[[i]]$files[[j]]$file_path, "/")[[1]]
                            file_path <- do.call(file.path, as.list(file_path))

                            private$calculated_destinations[[i]]$files[[j]]$file_path <- file_path
                        }
                    }

                    private$constraints <- constraints
                },

                calculate_constraints_level = function(parameters) {
                    constraints_level = 0

                    for(constraint in private$constraints) {
                        constraints_level = max(constraints_level, parameters[[constraint$name]]$calculate_constraints_level(parameters) + 1)
                    }

                    return(constraints_level)
                },
                
                generate_random_value = function(parameters, individual) {
                    minAux = private$min
                    maxAux = private$max

                    for(constraint in private$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                minAux = individual[[constraint$name]]
                                maxAux = individual[[constraint$name]]
                            } else {
                                minAux = constraint$min
                                maxAux = constraint$max
                            }
                        }
                    }

                    return(as.integer(sample(minAux:maxAux, 1)))
                },

                generate_mutation = function(parameters, individual) {
                    displacement <- sample(1:(private$max-private$min), 1)
                    current_pos <- (individual[[private$name]]-private$min+1)
                    new_pos <- (current_pos + displacement) %% (private$max-private$min+1)

                    return(as.integer(private$min + new_pos - 1))
                },

                constraints_breached = function(individual) {
                    minAux = private$min
                    maxAux = private$max

                    for(constraint in private$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                minAux = individual[[constraint$name]]
                                maxAux = individual[[constraint$name]]
                            } else {
                                minAux = constraint$min
                                maxAux = constraint$max
                            }
                        }
                    }

                    return(individual[[private$name]] < minAux || maxAux < individual[[private$name]])
                },

                show = function() {
                    lines <- c()
                    lines <- c(lines, paste0("  Parameter Name: \"", private$name, "\""))
                    lines <- c(lines, paste0("  Type: ", class(self)[1]))
                    lines <- c(lines, paste0("  Minimum: ", private$min))
                    lines <- c(lines, paste0("  Maximum: ", private$max))
                    lines <- c(lines, "  Fixed destinations:")
                    for(i in seq_along(private$fixed_destinations)) {
                        lines <- c(lines, "   /")
                        lines <- c(lines, paste0("  | File ", i))
                        lines <- c(lines, paste0("  | File path: \"", private$fixed_destinations[[i]]$file_path, "\""))
                        lines <- c(lines, paste0("  | JSON path: \"", private$fixed_destinations[[i]]$json_path, "\""))
                        lines <- c(lines, "   \\")
                    }
                    lines <- c(lines, "  Calculated destinations:")
                    for(i in seq_along(private$calculated_destinations)) {
                        lines <- c(lines, "   /")
                        lines <- c(lines, paste0("  | Destination ", i))
                        lines <- c(lines, "  | Function: function(individual, json_data)")
                        lines <- c(lines, paste0("  | ", paste(deparse(body(private$calculated_destinations[[i]][["function"]])), collapse = "\n  | ")))
                        lines <- c(lines, "  | Files:")
                        for(j in seq_along(private$calculated_destinations[[i]]$files)) {
                            lines <- c(lines, "  |  /")
                            lines <- c(lines, paste0("  | | File ", j))
                            lines <- c(lines, paste0("  | | File path: \"", private$calculated_destinations[[i]]$files[[j]]$file_path, "\""))
                            lines <- c(lines, paste0("  | | JSON path: \"", private$calculated_destinations[[i]]$files[[j]]$json_path, "\""))
                            lines <- c(lines, "  |  \\")
                        }
                        lines <- c(lines, "   \\")
                    }
                    return(lines)
                }
            )
        ),

        Boolean = R6::R6Class("Boolean", inherit = WeaverOptimiserClass$Parameter,
            private = list(
                name = character(0),
                fixed_destinations = list(),
                calculated_destinations = list(),
                constraints = list()
            ),
            public = list(
                initialize = function(name, fixed_destinations, calculated_destinations, constraints) {
                    private$name <- name

                    private$fixed_destinations <- fixed_destinations
                    private$calculated_destinations <- calculated_destinations

                    for(i in seq_along(private$fixed_destinations)) {
                        file_path <- strsplit(private$fixed_destinations[[i]]$file_path, "/")[[1]]
                        file_path <- do.call(file.path, as.list(file_path))

                        private$fixed_destinations[[i]]$file_path <- file_path
                    }

                    for(i in seq_along(private$calculated_destinations)) {
                        for(j in seq_along(private$calculated_destinations[[i]]$files)) {
                            file_path <- strsplit(private$calculated_destinations[[i]]$files[[j]]$file_path, "/")[[1]]
                            file_path <- do.call(file.path, as.list(file_path))

                            private$calculated_destinations[[i]]$files[[j]]$file_path <- file_path
                        }
                    }

                    private$constraints <- constraints
                },

                calculate_constraints_level = function(parameters) {
                    constraints_level = 0

                    for(constraint in private$constraints) {
                        constraints_level = max(constraints_level, parameters[[constraint$name]]$calculate_constraints_level(parameters) + 1)
                    }

                    return(constraints_level)
                },

                generate_random_value = function(parameters, individual) {
                    valuesAux = c(TRUE, FALSE)

                    for(constraint in private$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                valuesAux = individual[[constraint$name]]
                            } else {
                                valuesAux = constraint$values
                            }
                        }
                    }

                    return(sample(valuesAux, 1))
                },

                generate_mutation = function(parameters, individual) {
                    return(!individual[[private$name]])
                },

                constraints_breached = function(individual) {
                    valuesAux = c(TRUE, FALSE)

                    for(constraint in private$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                valuesAux = individual[[constraint$name]]
                            } else {
                                valuesAux = constraint$values
                            }
                        }
                    }

                    return(!individual[[private$name]] %in% valuesAux)
                },

                show = function() {
                    lines <- c()
                    lines <- c(lines, paste0("  Parameter Name: \"", private$name, "\""))
                    lines <- c(lines, paste0("  Type: ", class(self)[1]))
                    lines <- c(lines, "  Fixed destinations:")
                    for(i in seq_along(private$fixed_destinations)) {
                        lines <- c(lines, "   /")
                        lines <- c(lines, paste0("  | File ", i))
                        lines <- c(lines, paste0("  | File path: \"", private$fixed_destinations[[i]]$file_path, "\""))
                        lines <- c(lines, paste0("  | JSON path: \"", private$fixed_destinations[[i]]$json_path, "\""))
                        lines <- c(lines, "   \\")
                    }
                    lines <- c(lines, "  Calculated destinations:")
                    for(i in seq_along(private$calculated_destinations)) {
                        lines <- c(lines, "   /")
                        lines <- c(lines, paste0("  | Destination ", i))
                        lines <- c(lines, "  | Function: function(individual, json_data)")
                        lines <- c(lines, paste0("  | ", paste(deparse(body(private$calculated_destinations[[i]][["function"]])), collapse = "\n  | ")))
                        lines <- c(lines, "  | Files:")
                        for(j in seq_along(private$calculated_destinations[[i]]$files)) {
                            lines <- c(lines, "  |  /")
                            lines <- c(lines, paste0("  | | File ", j))
                            lines <- c(lines, paste0("  | | File path: \"", private$calculated_destinations[[i]]$files[[j]]$file_path, "\""))
                            lines <- c(lines, paste0("  | | JSON path: \"", private$calculated_destinations[[i]]$files[[j]]$json_path, "\""))
                            lines <- c(lines, "  |  \\")
                        }
                        lines <- c(lines, "   \\")
                    }
                    return(lines)
                }
            )
        ),

        Permutation = R6::R6Class("Permutation", inherit = WeaverOptimiserClass$Parameter,
            private = list(
                name = character(0),
                elements = NULL,
                fixed_destinations = list(),
                calculated_destinations = list(),
                constraints = list()
            ),
            public = list(
                initialize = function(name, elements, fixed_destinations, calculated_destinations, constraints) {
                    private$name <- name

                    private$elements <- elements

                    private$fixed_destinations <- fixed_destinations
                    private$calculated_destinations <- calculated_destinations

                    for(i in seq_along(private$fixed_destinations)) {
                        file_path <- strsplit(private$fixed_destinations[[i]]$file_path, "/")[[1]]
                        file_path <- do.call(file.path, as.list(file_path))

                        private$fixed_destinations[[i]]$file_path <- file_path
                    }

                    for(i in seq_along(private$calculated_destinations)) {
                        for(j in seq_along(private$calculated_destinations[[i]]$files)) {
                            file_path <- strsplit(private$calculated_destinations[[i]]$files[[j]]$file_path, "/")[[1]]
                            file_path <- do.call(file.path, as.list(file_path))

                            private$calculated_destinations[[i]]$files[[j]]$file_path <- file_path
                        }
                    }

                    private$constraints <- constraints
                },

                calculate_constraints_level = function(parameters) {
                    constraints_level = 0

                    for(constraint in private$constraints) {
                        constraints_level = max(constraints_level, parameters[[constraint$name]]$calculate_constraints_level(parameters) + 1)
                    }

                    return(constraints_level)
                },
                
                generate_random_value = function(parameters, individual) {
                    elementsAux = private$elements

                    for(constraint in private$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                elementsAux = individual[[constraint$name]]
                            } else {
                                elementsAux = constraint$elements
                            }
                        }
                    }

                    return(sample(elementsAux, 1))
                },

                generate_mutation = function(parameters, individual) {
                    return(sample(setdiff(private$elements, individual[[private$name]]), 1))
                },

                constraints_breached = function(individual) {
                    elementsAux = private$elements

                    for(constraint in private$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                elementsAux = individual[[constraint$name]]
                            } else {
                                elementsAux = constraint$elements
                            }
                        }
                    }

                    return(!individual[[private$name]] %in% elementsAux)
                },

                show = function() {
                    lines <- c()
                    lines <- c(lines, paste0("  Parameter Name: \"", private$name, "\""))
                    lines <- c(lines, paste0("  Type: ", class(self)[1]))
                    lines <- c(lines, paste0("  Elements: ", private$elements))
                    lines <- c(lines, "  Fixed destinations:")
                    for(i in seq_along(private$fixed_destinations)) {
                        lines <- c(lines, "   /")
                        lines <- c(lines, paste0("  | File ", i))
                        lines <- c(lines, paste0("  | File path: \"", private$fixed_destinations[[i]]$file_path, "\""))
                        lines <- c(lines, paste0("  | JSON path: \"", private$fixed_destinations[[i]]$json_path, "\""))
                        lines <- c(lines, "   \\")
                    }
                    lines <- c(lines, "  Calculated destinations:")
                    for(i in seq_along(private$calculated_destinations)) {
                        lines <- c(lines, "   /")
                        lines <- c(lines, paste0("  | Destination ", i))
                        lines <- c(lines, "  | Function: function(individual, json_data)")
                        lines <- c(lines, paste0("  | ", paste(deparse(body(private$calculated_destinations[[i]][["function"]])), collapse = "\n  | ")))
                        lines <- c(lines, "  | Files:")
                        for(j in seq_along(private$calculated_destinations[[i]]$files)) {
                            lines <- c(lines, "  |  /")
                            lines <- c(lines, paste0("  | | File ", j))
                            lines <- c(lines, paste0("  | | File path: \"", private$calculated_destinations[[i]]$files[[j]]$file_path, "\""))
                            lines <- c(lines, paste0("  | | JSON path: \"", private$calculated_destinations[[i]]$files[[j]]$json_path, "\""))
                            lines <- c(lines, "  |  \\")
                        }
                        lines <- c(lines, "   \\")
                    }
                    return(lines)
                }
            )
        ),

        Abundances = R6::R6Class("Abundances", inherit = WeaverOptimiserClass$Parameter,
            private = list(
                name = character(0),
                densitiesVector = numeric(0),
                min = numeric(0),
                max = numeric(0),
                fixed_destinations = list(),
                constraints = list()
            ),
            public = list(
                initialize = function(name, densitiesVector, min, max, fixed_destinations, constraints) {
                    private$name <- name

                    private$densitiesVector <- densitiesVector

                    private$min <- min
                    private$max <- max

                    private$fixed_destinations <- fixed_destinations

                    for(i in seq_along(private$fixed_destinations)) {
                        file_path <- strsplit(private$fixed_destinations[[i]]$file_path, "/")[[1]]
                        file_path <- do.call(file.path, as.list(file_path))

                        private$fixed_destinations[[i]]$file_path <- file_path
                    }

                    private$constraints <- constraints
                },

                calculate_constraints_level = function(parameters) {
                    constraints_level = 0

                    for(constraint in private$constraints) {
                        constraints_level = max(constraints_level, parameters[[constraint$name]]$calculate_constraints_level(parameters) + 1)
                    }

                    return(constraints_level)
                },
                
                generate_random_value = function(parameters, individual) {
                    minAux = private$min
                    maxAux = private$max

                    for(constraint in private$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                minAux = individual[[constraint$name]]
                                maxAux = individual[[constraint$name]]
                            } else {
                                minAux = constraint$min
                                maxAux = constraint$max
                            }
                        }
                    }

                    return(as.integer(sample(minAux:maxAux, 1)))
                },

                generate_mutation = function(parameters, individual) {
                    displacement <- sample(1:(private$max-private$min), 1)
                    current_pos <- (individual[[private$name]]-private$min+1)
                    new_pos <- (current_pos + displacement) %% (private$max-private$min+1)

                    return(as.integer(private$min + new_pos - 1))
                },

                constraints_breached = function(individual) {
                    minAux = private$min
                    maxAux = private$max

                    for(constraint in private$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                minAux = individual[[constraint$name]]
                                maxAux = individual[[constraint$name]]
                            } else {
                                minAux = constraint$min
                                maxAux = constraint$max
                            }
                        }
                    }

                    return(individual[[private$name]] < minAux || maxAux < individual[[private$name]])
                },

                show = function() {
                    lines <- c()
                    lines <- c(lines, paste0("  Parameter Name: \"", private$name, "\""))
                    lines <- c(lines, paste0("  Type: ", class(self)[1]))
                    lines <- c(lines, paste0("  Densities vector: [", toString(private$densitiesVector), "]"))
                    lines <- c(lines, paste0("  Minimum: ", private$min))
                    lines <- c(lines, paste0("  Maximum: ", private$max))
                    lines <- c(lines, "  Fixed destinations:")
                    for(i in seq_along(private$fixed_destinations)) {
                        lines <- c(lines, "   /")
                        lines <- c(lines, paste0("  | File ", i))
                        lines <- c(lines, paste0("  | File path: \"", private$fixed_destinations[[i]]$file_path, "\""))
                        lines <- c(lines, paste0("  | JSON path: \"", private$fixed_destinations[[i]]$json_path, "\""))
                        lines <- c(lines, "   \\")
                    }
                    return(lines)
                }
            )
        ),

        Preference = R6::R6Class("Preference", inherit = WeaverOptimiserClass$Parameter,
            private = list(
                name = character(0),
                cells = list(),
                min = numeric(0),
                max = numeric(0)
            ),
            public = list(
                initialize = function(name, cells, min, max) {
                    private$name <- name
                    
                    for(cell in cells) {
                        private$cells[[length(private$cells) + 1]] <- cell$generate_cells()
                    }

                    private$min <- min
                    private$max <- max
                },

                show = function() {
                    lines <- c()
                    lines <- c(lines, paste0("  Parameter Name: \"", private$name, "\""))
                    lines <- c(lines, paste0("  Type: ", class(self)[1]))
                    lines <- c(lines, "  Cells:")
                    lines <- c(lines, "   /")
                    for(cell in private$cells[[1]]) {
                        lines <- c(lines, paste0("  | Row: ", cell[[1]], " Column: ", cell[[2]]))
                    }
                    lines <- c(lines, "   \\")
                    lines <- c(lines, paste0("  Minimum: ", private$min))
                    lines <- c(lines, paste0("  Maximum: ", private$max))
                    return(lines)
                }
            )
        ),

        PreferenceCell = R6::R6Class("PreferenceCell",
            private = list(
                predator_species = character(0),
                predator_instars = numeric(0),
                prey_species = character(0),
                prey_instars = numeric(0)
            ),
            public = list(
                initialize = function(predator_species, predator_instars, prey_species, prey_instars) {
                    private$predator_species <- predator_species
                    private$predator_instars <- predator_instars
                    private$prey_species <- prey_species
                    private$prey_instars <- prey_instars
                },

                generate_cells = function() {
                    cells <- list()

                    columns_values <- paste(private$predator_species, private$predator_instars, sep = "$")
                    rows_values <- paste(private$prey_species, private$prey_instars, sep = "$")

                    for(row in rows_values) {
                        for(column in columns_values) {
                            cells[[length(cells) + 1]] <- list(row, column)
                        }
                    }

                    return(cells)
                }
            )
        ),

        Profitability = R6::R6Class("Profitability", inherit = WeaverOptimiserClass$Parameter,
            private = list(
                name = character(0),
                cells = list(),
                min = numeric(0),
                max = numeric(0)
            ),
            public = list(
                initialize = function(name, cells, min, max) {
                    private$name <- name

                    for(cell in cells) {
                        private$cells[[length(private$cells) + 1]] <- cell$generate_cells()
                    }

                    private$min <- min
                    private$max <- max
                },

                show = function() {
                    lines <- c()
                    lines <- c(lines, paste0("  Parameter Name: \"", private$name, "\""))
                    lines <- c(lines, paste0("  Type: ", class(self)[1]))
                    lines <- c(lines, "  Cells:")
                    lines <- c(lines, "   /")
                    for(cell in private$cells[[1]]) {
                        lines <- c(lines, paste0("  | Row: ", cell[[1]], " Column: ", cell[[2]]))
                    }
                    lines <- c(lines, "   \\")
                    lines <- c(lines, paste0("  Minimum: ", private$min))
                    lines <- c(lines, paste0("  Maximum: ", private$max))
                    return(lines)
                }
            )
        ),

        Objective = R6::R6Class("Objective",
            private = list(
                name = character(0),
                minimise = logical(0),
                fitness_function = NULL
            ),
            public = list(
                initialize = function(name, minimise, fitness_function) {
                    private$name <- name
                    private$minimise <- minimise
                    private$fitness_function <- fitness_function
                },

                get_name = function() {
                    return(private$name)
                },

                get_optimiser_value = function(value) {
                    if(private$minimise) {
                        return(value)
                    }
                    else {
                        return(1.0/value)
                    }
                },

                evaluate = function(result_folder) {
                    return(private$fitness_function(result_folder))
                }
            )
        ),

        Fitness = R6::R6Class("Fitness",
            public = list(
                get_display_info = function(values) {
                    stop("Abstract method 'get_display_info' must be implemented.")
                },

                get_header = function() {
                    stop("Abstract method 'get_header' must be implemented.")
                },

                get_row_info = function(values) {
                    stop("Abstract method 'get_row_info' must be implemented.")
                },

                display_generation_summary = function(best_fitness, mean_fitness) {
                    stop("Abstract method 'display_generation_summary' must be implemented.")
                }
            )
        ),
        
        WeightedFitness = R6::R6Class("WeightedFitness",
            inherit = WeaverOptimiserClass$Fitness,
            private = list(
                objectives = list(),
                weights = list(),

                calculate_fitness = function(values) {
                    return(sum(mapply(
                        `*`, 
                        private$weights, 
                        lapply(names(private$weights), function(name) {
                            private$objectives[[name]]$get_optimiser_value(values[[name]])
                        })
                    )))
                },

                compare_objectives = function(a, b) {
                    return(private$calculate_fitness(a) < private$calculate_fitness(b))  
                }
            ),
            public = list(
                initialize = function(objectives, weights) {
                    for (objective in objectives) {
                        private$objectives[[objective$get_name()]] = objective
                    }

                    private$weights <- weights
                },

                evaluate = function(result_folder) {
                    evaluation = list()

                    for (objetive in names(private$objectives)) {
                        evaluation[[objetive]] = private$objectives[[objetive]]$evaluate(result_folder)
                    }

                    return(evaluation)
                },

                get_best_index = function(values) {
                    return(Reduce(function(a, b) if (private$compare_objectives(values[[a]], values[[b]])) a else b, seq_along(values)))
                },

                get_display_info = function(values) {
                    return(c(
                        sapply(names(values), function(name) {
                            paste0("Name: ", name, " | Value: ", values[[name]], " | Weight: ", private$weights[[name]])
                        }),
                        paste0("Total Value: ", private$calculate_fitness(values))
                    ))
                },

                get_header = function() {
                    return(c(names(private$weights), "Fitness"))
                },

                get_row_info = function(values) {
                    return(c(unname(values[names(private$weights)]), private$calculate_fitness(values)))
                },

                display_generation_summary = function(best_fitness, mean_fitness) {
                    res <- c(paste("Best fitness:", private$calculate_fitness(best_fitness)))
                    res <- c(res, paste("Mean fitness:", private$calculate_fitness(mean_fitness)))

                    return(res)
                },

                calculate_mean_value = function(population) {
                    mean_value <- list()

                    for (objetive in names(private$weights)) {
                        mean_value[[objetive]] <- mean(sapply(population, function(values) { values[[objetive]] }))
                    } 

                    return(mean_value)
                },

                save_progress_graph = function(output_path, max_generations, best_fitness, mean_fitness) {
                    fitness_data <- data.frame(
                        Generation = 1:max_generations,
                        BestFitness = sapply(best_fitness, function(values) { private$calculate_fitness(values) }),
                        MeanFitness = sapply(mean_fitness, function(values) { private$calculate_fitness(values) })
                    )

                    if (nrow(fitness_data) == 1) {
                        p <- ggplot2::ggplot(fitness_data, ggplot2::aes(x = Generation)) +
                            ggplot2::geom_point(ggplot2::aes(y = BestFitness, color = "Best Individual"), size = 3) +
                            ggplot2::geom_point(ggplot2::aes(y = MeanFitness, color = "Mean Population"), size = 3, shape = 2) +
                            ggplot2::labs(title = "Fitness Progression", x = "Generation", y = "Fitness Value") +
                            ggplot2::scale_color_manual(values = c("Best Individual" = "blue", "Mean Population" = "red")) +
                            ggplot2::theme_minimal()
                    } else {
                        p <- ggplot2::ggplot(fitness_data, ggplot2::aes(x = Generation)) +
                            ggplot2::geom_line(ggplot2::aes(y = BestFitness, color = "Best Individual"), linewidth = 1.2) +
                            ggplot2::geom_line(ggplot2::aes(y = MeanFitness, color = "Mean Population"), linewidth = 1.2, linetype = "dashed") +
                            ggplot2::labs(title = "Fitness Progression", x = "Generation", y = "Fitness Value") +
                            ggplot2::scale_color_manual(values = c("Best Individual" = "blue", "Mean Population" = "red")) +
                            ggplot2::theme_minimal()
                    }
    
                    ggplot2::ggsave(file.path(output_path, "fitness_progress.svg"), plot = p, width = 8, height = 6)
                }
            )
        ),

        MultiObjectiveFitness = R6::R6Class("MultiObjectiveFitness",
            inherit = WeaverOptimiserClass$Fitness,
            private = list(
                objectives = list(),
                order = character(0),

                compare_objectives = function(a, b) {
                    for (objective in private$order) {
                        a_objective_value = private$objectives[[objective]]$get_optimiser_value(a[[objective]])
                        b_objective_value = private$objectives[[objective]]$get_optimiser_value(b[[objective]])

                        if(a_objective_value < b_objective_value) {
                            return(TRUE)
                        }
                        else if(a_objective_value > b_objective_value) {
                            return(FALSE)
                        }
                    }

                    return(FALSE)
                }
            ),
            public = list(
                initialize = function(objectives, order) {
                    for (objective in objectives) {
                        private$objectives[[objective$get_name()]] = objective
                    }

                    private$order <- order
                },

                evaluate = function(result_folder) {
                    evaluation = list()

                    for (objetive in names(private$objectives)) {
                        evaluation[[objetive]] = private$objectives[[objetive]]$evaluate(result_folder)
                    }

                    return(evaluation)
                },

                get_best_index = function(values) {
                    return(Reduce(function(a, b) if (private$compare_objectives(values[[a]], values[[b]])) a else b, seq_along(values)))
                },

                get_display_info = function(values) {
                    return(sapply(names(values), function(name) {
                        paste0("Name: ", name, " | Value: ", values[[name]])
                    }))
                },

                get_header = function() {
                    return(private$order)
                },

                get_row_info = function(values) {
                    return(unname(values[private$order]))
                },

                display_generation_summary = function(best_fitness, mean_fitness) {
                    res <- c("Best fitness:")
                    sapply(private$order, function(name) {
                        res <- c(res, paste0(" - Parameter: ", name, " | Value: ", best_fitness[[name]]))
                    })

                    res <- c(res, "Mean fitness:")
                    sapply(private$order, function(name) {
                        res <- c(res, paste0(" - Parameter: ", name, " | Value: ", mean_fitness[[name]]))
                    })

                    return(res)
                },

                calculate_mean_value = function(population) {
                    mean_value <- list()

                    for (objetive in private$order) {
                        mean_value[[objetive]] <- mean(sapply(population, function(values) { values[[objetive]] }))
                    } 

                    return(mean_value)
                },

                save_progress_graph = function(output_path, max_generations, best_fitness, mean_fitness) {
                    for (objetive in private$order) {
                        fitness_data <- data.frame(
                            Generation = 1:max_generations,
                            BestFitness = sapply(best_fitness, function(values) { values[[objetive]] }),
                            MeanFitness = sapply(mean_fitness, function(values) { values[[objetive]] })
                        )

                        if (nrow(fitness_data) == 1) {
                            p <- ggplot2::ggplot(fitness_data, ggplot2::aes(x = Generation)) +
                                ggplot2::geom_point(ggplot2::aes(y = BestFitness, color = "Best Individual"), size = 3) +
                                ggplot2::geom_point(ggplot2::aes(y = MeanFitness, color = "Mean Population"), size = 3, shape = 2) +
                                ggplot2::labs(title = "Fitness Progression", x = "Generation", y = "Fitness Value") +
                                ggplot2::scale_color_manual(values = c("Best Individual" = "blue", "Mean Population" = "red")) +
                                ggplot2::theme_minimal()
                        } else {
                            p <- ggplot2::ggplot(fitness_data, ggplot2::aes(x = Generation)) +
                                ggplot2::geom_line(ggplot2::aes(y = BestFitness, color = "Best Individual"), linewidth = 1.2) +
                                ggplot2::geom_line(ggplot2::aes(y = MeanFitness, color = "Mean Population"), linewidth = 1.2, linetype = "dashed") +
                                ggplot2::labs(title = "Fitness Progression", x = "Generation", y = "Fitness Value") +
                                ggplot2::scale_color_manual(values = c("Best Individual" = "blue", "Mean Population" = "red")) +
                                ggplot2::theme_minimal()
                        }
        
                        ggplot2::ggsave(file.path(output_path, paste0(objetive, "_progress.svg")), plot = p, width = 8, height = 6)
                    }
                }
            )
        ),

        # --- SISTEMA DE LOGS DESACOPLADO CON DESPACHADOR EN TIEMPO REAL ---
        # Reemplaza los antiguos cat() fijos por llamadas al callback inyectado
        dispatch_log = function(message_text, log_type = "info", generation = 0, best_fit = NULL, mean_fit = NULL) {
            if (!is.null(private$log_file)) {
                con_file <- file(private$log_file, open = "at")
                cat(paste0("[", Sys.time(), "]\t[", toupper(log_type), "]\t", message_text, "\n"), file = con_file)
                flush(con_file)
                close(con_file)
            }
            
            if (!is.null(private$metrics_dispatcher) && is.function(private$metrics_dispatcher)) {
                tryCatch({
                    private$metrics_dispatcher(
                        generation   = as.integer(generation),
                        current_best = best_fit,
                        current_mean = mean_fit,
                        log_line     = as.character(message_text),
                        log_type     = as.character(log_type)
                    )
                }, error = function(e) warning("Metrics callback failed: ", e$message))
            }
        },

        dispatch_debug_log = function(message_text, generation = 0, best_fit = NULL, mean_fit = NULL) {
            if(private$debug) {
                private$dispatch_log(message_text, "debug", generation, best_fit, mean_fit)
            }
        },

        dispatch_evolution = function(current_ind, total_ind, current_gen, max_gen, trigger_plots) {
            if (!is.null(private$evolution_dispatcher) && is.function(private$evolution_dispatcher)) {
                tryCatch({
                    private$evolution_dispatcher(
                        current_ind   = as.integer(current_ind),
                        total_ind     = as.integer(total_ind),
                        current_gen   = as.integer(current_gen),
                        max_gen       = as.integer(max_gen),
                        trigger_plots = as.integer(trigger_plots)
                    )
                }, error = function(e) warning("Evolution callback failed: ", e$message))
            }
        },

        display_population_debug = function(population) {
            sapply(1:length(population), function(i) {
                private$dispatch_debug_log(paste0("# $ Individual ", i))
                private$dispatch_debug_log("#  /")

                sapply(names(private$parameters_constraints_level), function(name) {
                    private$dispatch_debug_log(paste0("# | Parameter: ", name, " | Value: ", population[[i]][[name]]))
                })

                sapply(names(private$preferences_groups), function(name) {
                    private$dispatch_debug_log(paste0("# | Preference: ", name, " | Value: ", population[[i]][[name]]))
                })

                sapply(names(private$profitability_groups), function(name) {
                    private$dispatch_debug_log(paste0("# | Profitability: ", name, " | Value: ", population[[i]][[name]]))
                })

                private$dispatch_debug_log("#  \\")
            })
        },

        display_parameters = function() {
            info_lines <- c()
            info_lines <- c(info_lines, private$parameters[[1]]$show())

            if(length(private$parameters) > 1) {
                for(param in private$parameters[2:length(private$parameters)]) {
                    info_lines <- c(info_lines, "")

                    info_lines <- c(info_lines, "--------------------------------------------------")
                    info_lines <- c(info_lines, "")

                    info_lines <- c(info_lines, param$show())
                }
            }

            if(private$optimise_preferences) {
                for(param in private$preferences_groups) {
                    info_lines <- c(info_lines, "")
                    info_lines <- c(info_lines, "--------------------------------------------------")
                    info_lines <- c(info_lines, "")

                    info_lines <- c(info_lines, param$show())
                }
            }

            if(private$optimise_profitability) {
                for(param in private$profitability_groups) {
                    info_lines <- c(info_lines, "")
                    info_lines <- c(info_lines, "--------------------------------------------------")
                    info_lines <- c(info_lines, "")

                    info_lines <- c(info_lines, param$show())
                }
            }

            return(info_lines)
        },

        # --- NUEVO MÉTODO INTERNO: PARSER INTEGRADO DE BLUEPRINTS ---
        # Elimina add_parameters() y mapea las variables unificadas multidimensionales
        parse_parameters_from_json = function(variables_blueprint) {
            if (is.null(variables_blueprint) || length(variables_blueprint) == 0) {
                stop("Error crítico de inicialización: La sección 'optimization_variables_allocation' del JSON está vacía.")
            }
            
            for (v_name in names(variables_blueprint)) {
                v_meta  <- variables_blueprint[[v_name]]
                p_class <- v_meta$parameter_class
                
                # Transformamos la grilla de pares (Archivo, Ruta $) en la lista de destinos fijos de R
                fixed_dests <- lapply(v_meta$tied_destinations_coordinates, function(coord) {
                    list(file_path = coord$target_json_file, json_path = coord$internal_json_path)
                })
                
                # Instanciación dirigida in-situ según la taxonomía unificada
                if (p_class == "RealValued") {
                    private$parameters[[v_name]] <- private$RealValued$new(
                        name = v_name, 
                        min = as.numeric(v_meta$min_bound), 
                        max = as.numeric(v_meta$max_bound),
                        fixed_destinations = fixed_dests,
                        calculated_destinations = list(),
                        constraints = list()
                    )
                } else if (p_class == "Integer") {
                    private$parameters[[v_name]] <- private$Integer$new(
                        name = v_name, 
                        min = as.integer(v_meta$min_bound), 
                        max = as.integer(v_meta$max_bound),
                        fixed_destinations = fixed_dests,
                        calculated_destinations = list(),
                        constraints = list()
                    )
                } else if (p_class == "Boolean") {
                    private$parameters[[v_name]] <- private$Boolean$new(
                        name = v_name, 
                        fixed_destinations = fixed_dests,
                        calculated_destinations = list(),
                        constraints = list()
                    )
                } else if (p_class == "Permutation") {
                    private$parameters[[v_name]] <- private$Permutation$new(
                        name = v_name, 
                        elements = as.character(v_meta$selected_enums),
                        fixed_destinations = fixed_dests,
                        calculated_destinations = list(),
                        constraints = list()
                    )
                } else if (p_class == "Abundances") {
                    private$parameters[[v_name]] <- private$Abundances$new(
                        name = v_name, 
                        densitiesVector = as.numeric(v_meta$densities_vector),
                        min = as.integer(v_meta$min_bound), 
                        max = as.integer(v_meta$max_bound),
                        fixed_destinations = fixed_dests,
                        calculated_destinations = list(),
                        constraints = list()
                    )
                } else if (p_class == "Preference") {
                    stop("Without implementation")

                    # private$optimise_preferences <- TRUE

                    # private$preferences_groups[[param$name]] <- param

                } else if (p_class == "Profitability") {
                    stop("Without implementation")

                    # private$optimise_profitability <- TRUE

                    # private$profitability_groups[[param$name]] <- param
                } else {
                    stop("Unknown parameter class: ", p_class)
                }
                
                # Preparamos la columna correspondiente en el historial de combinaciones
                private$combination_history[[v_name]] <- logical(0) 
            }
        },

        # --- MOTOR DINÁMICO: RECONSTRUCTOR ESTRATEGIA DE APTITUD ---
        build_fitness_engine_from_blueprint = function(comp_env) {
            f_class <- comp_env$fitness_class
            
            # Recomponer funciones de aptitud (Predefinidas o bloques Custom inyectados)
            compiled_objectives <- lapply(names(comp_env$objectives), function(obj_name) {
                obj_meta <- comp_env$objectives[[obj_name]]
                
                f_body <- if (identical(obj_meta$fitness_func_name, "Custom Block")) {
                    eval(parse(text = paste0("function(result_folder) {\n", obj_meta$custom_code_raw, "\n}")))
                } else {
                    f_extracted <- WeaverOptimiserClass$predefined_fitness_functions[[obj_meta$fitness_func_name]]
                    
                    if (is.null(f_extracted)) {
                        stop("Error en Blueprint: La función fitness predefinida '", 
                             obj_meta$fitness_func_name, "' no está registrada en el motor.")
                    }
                    f_extracted
                }
                
                private$Objective$new(name = obj_meta$name, minimise = (obj_meta$criteria == "MIN"), fitness_function = f_body)
            })
            
            if (identical(f_class, "MultiObjectiveFitness")) {
                return(private$MultiObjectiveFitness$new(objectives = compiled_objectives, order = as.character(comp_env$priority_ranking)))
            } else if (identical(f_class, "WeightedFitness")) {
                return(private$WeightedFitness$new(objectives = compiled_objectives, weights = comp_env$assigned_weights))
            } else {
                stop("Unknown fitness class: ", f_class)
            }
        },

        add_combination_to_history = function(new_combination) {
            new_combination$id <- nrow(private$combination_history) + 1

            private$combination_history <- dplyr::bind_rows(private$combination_history, new_combination)

            return(new_combination)
        },

        combination_already_added = function(combination) {
            if (nrow(private$combination_history) == 0) return(FALSE)
            index <- which(
                Reduce(`&`, Map(function(col, val) private$combination_history[[col]] == val, 
                                names(combination), combination))
            )
            return(length(index) > 0)
        },

        set_combination_fitness = function(id, fitness, result_folder) {
            private$combination_results[[id]] <- list()
            private$combination_results[[id]]$fitness <- fitness
            private$combination_results[[id]]$result_folder <- result_folder
        },

        obtain_fitness_value = function(individual) {
            index <- which(Reduce(`&`, Map(function(col, val) private$combination_history[[col]] == val, names(individual), individual)))
            return(private$combination_results[[private$combination_history[[index, "id"]]]]$fitness)
        },

        obtain_result_folder = function(individual) {
            index <- which(Reduce(`&`, Map(function(col, val) private$combination_history[[col]] == val, names(individual), individual)))
            return(private$combination_results[[private$combination_history[[index, "id"]]]]$result_folder)
        },

        # --- ENTORNO ALGORÍTMICO GENERAL DEL SOLVER ---
        calculate_parameters_constraints_level = function() {
            for(name in names(private$parameters)) {
                private$parameters_constraints_level[[name]] <- private$parameters[[name]]$calculate_constraints_level(private$parameters)
            }

            private$parameters_constraints_level <- private$parameters_constraints_level[order(unlist(private$parameters_constraints_level))]
        },

        generate_individual = function() {
            individual <- list()

            for(name in names(private$parameters_constraints_level)) {
                individual[[name]] <- private$parameters[[name]]$generate_random_value(private$parameters, individual)

                private$dispatch_debug_log(paste0("* | Parameter: ", name, " | Value: ", individual[[name]]))
            }

            if(private$optimise_preferences) {
                preferences_sample <- as.list(xsample(
                    A = NULL, B = NULL, 
                    E = private$preferences_constraints$E, 
                    F = private$preferences_constraints$F, 
                    G = private$preferences_constraints$G, 
                    H = private$preferences_constraints$H,
                    iter = 5000,
                    output = 2
                )$X[2,])

                names(preferences_sample) <- names(private$preferences_groups)

                for(name in names(private$preferences_groups)) {
                    individual[[name]] = preferences_sample[[name]]
                
                    private$dispatch_debug_log(paste0("* | Preference: ", name, " | Value: ", individual[[name]]))
                }
            }

            if(private$optimise_profitability) {
                for(name in names(private$profitability_groups)) {
                    min_val <- max(0.0, private$profitability_groups[[name]]$min)
                    max_val <- min(1.0, private$profitability_groups[[name]]$max)

                    individual[[name]] <- runif(1, min = min_val, max = max_val)
                
                    private$dispatch_debug_log(paste0("* | Profitability: ", name, " | Value: ", individual[[name]]))
                }
            }

            return(individual)
        },

        initialize_population = function(population_size) {
            private$dispatch_log("Initializing stochastic starting population vectors...", log_type = "system")
            
            private$dispatch_debug_log("**************************************")
            private$dispatch_debug_log("**  Initialize population  **")
            private$dispatch_debug_log("*****************************")
            
            population <- lapply(1:population_size, function(x) {
                private$dispatch_debug_log(paste0("* $ Individual ", x))
                private$dispatch_debug_log("*  /")

                individual <- private$generate_individual()

                private$dispatch_debug_log("*  \\")

                return(individual)
            })

            private$dispatch_debug_log("**************************************")

            return(population)
        },

        create_config = function(individual, output_path, base_config_path) {
            # Se genera una subcarpeta única por combinación evaluada para evitar colisiones en hilos
            config_path <- file.path(output_path, "configs", paste0(basename(base_config_path), "_", individual$id))
            dir.create(config_path, recursive = TRUE, showWarnings = FALSE)
            file.copy(list.files(base_config_path, full.names = TRUE), config_path, recursive = TRUE)

            # Recorremos cada variable unificada aplicando Parameter Tying multidimensional
            for(name in names(private$parameters)) {
                param_obj <- private$parameters[[name]]
                
                if(inherits(param_obj, "Abundances")) {
                    # Distribución proporcional entera basada en el vector de densidades relativas
                    individualsPerInstar <- round(individual[[name]] * param_obj$densitiesVector / sum(param_obj$densitiesVector))
                    
                    for(i in seq_along(param_obj$fixed_destinations)) {
                        full_f_path <- file.path(config_path, param_obj$fixed_destinations[[i]]$file_path)
                        json_data <- jsonlite::fromJSON(full_f_path, simplifyVector = FALSE)
                        eval(parse(text = paste0("json_data$", param_obj$fixed_destinations[[i]]$json_path, " <- individualsPerInstar")))
                        write(jsonlite::toJSON(json_data, auto_unbox = TRUE, pretty = TRUE, null = "null", digits = 17), full_f_path)
                    }
                } else {
                    # LIGADURA DIRECTA: Se inyecta exactamente el mismo valor genético en todos sus destinos mapeados
                    for(i in seq_along(param_obj$fixed_destinations)) {
                        full_f_path <- file.path(config_path, param_obj$fixed_destinations[[i]]$file_path)
                        json_data <- jsonlite::fromJSON(full_f_path, simplifyVector = FALSE)
                        eval(parse(text = paste0("json_data$", param_obj$fixed_destinations[[i]]$json_path, " <- individual[[name]]")))
                        write(jsonlite::toJSON(json_data, auto_unbox = TRUE, pretty = TRUE, null = "null", digits = 17), full_f_path)
                    }

                    for(i in seq_along(param_obj$calculated_destinations)) {
                        for(j in seq_along(param_obj$calculated_destinations[[i]]$files)) {
                            full_f_path <- file.path(config_path, param_obj$calculated_destinations[[i]]$files[[j]]$file_path)
                            json_data <- jsonlite::fromJSON(full_f_path, simplifyVector = FALSE)
                            value <- param_obj$calculated_destinations[[i]][["function"]](individual, json_data)
                            eval(parse(text = paste0("json_data$", param_obj$calculated_destinations[[i]]$files[[j]]$json_path, " <- value")))
                            write(jsonlite::toJSON(json_data, auto_unbox = TRUE, pretty = TRUE, null = "null", digits = 17), full_f_path)
                        }
                    }
                }
            }

            if(private$optimise_preferences) {
                csv_path = file.path(config_path, "species", "ontogeneticLinksPreference.csv")

                preference_data <- read.csv(csv_path, header = TRUE, row.names = 1, sep = ",", check.names = FALSE)

                for(name in names(private$preferences_groups)) {
                    for(cell in private$preferences_groups[[name]]$cells[[1]]) {
                        preference_data[cell[[1]], cell[[2]]] = individual[[name]]
                    }
                }

                preference_data[] <- lapply(preference_data, function(col) {
                        if (is.numeric(col)) format(col, scientific = TRUE, digits = 17) else col
                    }
                )

                write.csv(preference_data, csv_path, quote = FALSE)
            }

            if(private$optimise_profitability) {
                csv_path = file.path(config_path, "species", "ontogeneticLinksProfitability.csv")

                profitability_data <- read.csv(csv_path, header = TRUE, row.names = 1, sep = ",", check.names = FALSE)

                for(name in names(private$profitability_groups)) {
                    for(cell in private$profitability_groups[[name]]$cells[[1]]) {
                        profitability_data[cell[[1]], cell[[2]]] = individual[[name]]
                    }
                }

                profitability_data[] <- lapply(profitability_data, function(col) {
                        if (is.numeric(col)) format(col, scientific = TRUE, digits = 17) else col
                    }
                )

                write.csv(profitability_data, csv_path, quote = FALSE)
            }

            return(config_path)
        },

        simulate_individual = function(individual, program, output_path, base_config_path) {
            config_path <- private$create_config(individual, output_path, base_config_path)
            
            # Normalización estricta de barras para entornos Windows
            if(.Platform$OS.type == "windows") {
                config_path <- gsub("/", "\\", config_path, fixed = TRUE)
            }

            results_output_path <- file.path(output_path, "results")
            if(.Platform$OS.type == "windows") {
                results_output_path <- gsub("/", "\\", results_output_path, fixed = TRUE)
            }

            output_file <- tempfile(fileext = ".txt")
            error_file  <- tempfile(fileext = ".txt")

            # Lanzamiento desacoplado del binario CLI nativo del simulador
            status <- system2(
                command = program, 
                args = c("--mode", "FromConfig", "-I", config_path, "-O", results_output_path, "--silent", "--parallel", private$program_cores),
                stdout = output_file, stderr = error_file, wait = TRUE
            ) 

            if(!(as.numeric(status) %in% c(0))) {
                stop(paste(
                    "Error while executing the command: ", program, 
                    "\nExit status: ", status, 
                    "\nConfig: ", config_path,
                    "\nStandard output:", paste(readLines(output_file, warn = FALSE), collapse = "\n"),
                    "\nError output:", paste(readLines(error_file, warn = FALSE), collapse = "\n")
                ))
            }
            
            # Extraemos la ruta de la carpeta de salida desde el log estándar del ejecutable
            result_line <- grep("Result folder:", readLines(output_file, warn = FALSE), value = TRUE)
            result_folder <- sub(".*Result folder:\\s*", "", result_line)
            return(trimws(result_folder))
        },

        evaluation_function = function(individual, program, output_path, base_config_path, progress_bar) {
            result_folder <- private$simulate_individual(individual, program, output_path, base_config_path)
            progress_bar(amount = 1)
            gc()
            return(result_folder)
        },

        evaluate_population = function(population, fitness_engine, program, output_path, base_config_path, current_gen, max_gen) {
            # Evitamos evaluar combinaciones ya exploradas en la memoria caché transaccional
            indexes_to_evaluate <- sapply(population, function(ind) !private$combination_already_added(ind))
            
            population_to_evaluate <- dplyr::distinct(dplyr::bind_rows(population[indexes_to_evaluate]))

            population_to_evaluate <- dplyr::bind_rows(
                lapply(seq_len(nrow(population_to_evaluate)), function(row) {
                    private$add_combination_to_history(population_to_evaluate[row, ])
                })
            )

            total_ind_to_process <- nrow(population_to_evaluate)

            if(total_ind_to_process > 0) {
                # Inicialización de la barra global en 0 individuos procesados
                private$dispatch_evolution(0, total_ind_to_process, current_gen, max_gen, current_gen)
                
                # Contador local persistente en el hilo principal para rastrear devoluciones paralelas
                ind_procesados_hilo_principal <- 0

                result_folder_vector <- progressr::with_progress({
                    # Creamos el progressor nativo de tu código original
                    progress_bar_raw <- progressr::progressor(total_ind_to_process)
                    
                    # ENVOLTURA INTERCEPTORA SEGURA EN EL PROCESO MAESTRO ASÍNCRONO
                    progress_bar_wrapper <- function(amount = 1) {
                        # 1. Ejecuta el progreso original requerido por future_lapply
                        progress_bar_raw(amount = amount)
                        
                        # 2. Incrementa el contador acumulativo
                        ind_procesados_hilo_principal <<- ind_procesados_hilo_principal + amount
                        
                        # 3. Dispara el despachador de evolución hacia el archivo .progress
                        private$dispatch_evolution(
                            current_ind   = ind_procesados_hilo_principal, 
                            total_ind     = total_ind_to_process, 
                            current_gen   = current_gen, 
                            max_gen       = max_gen, 
                            trigger_plots = current_gen
                        )
                    }
    
                    # LLAMADA ORIGINAL INTACTA (pasando nuestra envoltura interceptora)
                    future.apply::future_lapply(
                        split(population_to_evaluate, seq_len(nrow(population_to_evaluate))), 
                        private$evaluation_function, 
                        program, 
                        output_path, 
                        base_config_path, 
                        progress_bar_wrapper, 
                        future.seed = private$current_seed 
                    )
                })

                new_fitness_values <- lapply(result_folder_vector, function(rf) fitness_engine$evaluate(rf))

                lapply(seq_len(nrow(population_to_evaluate)), function(row) {
                    private$set_combination_fitness(population_to_evaluate[row,]$id, new_fitness_values[[row]], result_folder_vector[[row]])
                })
            } else {
                private$dispatch_evolution(1, 1, current_gen, max_gen, current_gen)
            }

            fitness_values <- lapply(population, function(ind) private$obtain_fitness_value(ind))
            result_folders <- lapply(population, function(ind) private$obtain_result_folder(ind))
            
            private$dispatch_debug_log("**************************************")
            private$dispatch_debug_log("**  Evaluate population  **")
            private$dispatch_debug_log("***************************")

            sapply(1:length(population), function(i) {
                private$dispatch_debug_log(paste0("* $ Individual ", i))
                private$dispatch_debug_log("*  /")

                private$dispatch_debug_log(paste0("* | Already evaluated: ", ifelse(indexes_to_evaluate[[i]], "No", "Yes") ))

                sapply(names(private$parameters_constraints_level), function(name) {
                    private$dispatch_debug_log(paste0("* | Parameter: ", name, " | Value: ", population[[i]][[name]]))
                })

                sapply(names(private$preferences_groups), function(name) {
                    private$dispatch_debug_log(paste0("* | Preference: ", name, " | Value: ", population[[i]][[name]]))
                })

                sapply(names(private$profitability_groups), function(name) {
                    private$dispatch_debug_log(paste0("* | Profitability: ", name, " | Value: ", population[[i]][[name]]))
                })

                private$dispatch_debug_log("* | Fitness:")
                private$dispatch_debug_log("* | /")

                sapply(fitness_engine$get_display_info(fitness_values[[i]]), function(info) {
                    private$dispatch_debug_log(paste0("* || ", info))
                })

                private$dispatch_debug_log("* | \\")
                private$dispatch_debug_log("*  \\")
            })

            private$dispatch_debug_log("**************************************")

            return(list(fitness_values = fitness_values, result_folders = result_folders))
        },

        selection = function(population, fitness_engine, fitness_values, k, size) {
            private$dispatch_debug_log("**************************************")
            private$dispatch_debug_log("**  Tournament Selection operator  **")
            private$dispatch_debug_log("*************************************")
        
            selected <- vector("list", size)

            for (i in seq_len(size)) {
                private$dispatch_debug_log(paste0("* $ Tournament ", i))
                private$dispatch_debug_log("*  /")

                candidates <- sample(seq_along(population), k)

                sapply(candidates, function(candidate) { 
                    private$dispatch_debug_log(paste0("* | Candidate: ", candidate)) 
                    private$dispatch_debug_log("* | Fitness:") 
                    private$dispatch_debug_log("* | /")

                    sapply(fitness_engine$get_display_info(fitness_values[[candidate]]), function(info) {
                        private$dispatch_debug_log(paste0("* || ", info))
                    })

                    private$dispatch_debug_log("* | \\")
                })

                best_candidate <- candidates[[fitness_engine$get_best_index(fitness_values[candidates])]]
                
                selected[[i]] <- population[[best_candidate]]

                private$dispatch_debug_log(paste0("* | Winner: ", best_candidate))

                private$dispatch_debug_log("*  \\")
            }

            private$dispatch_debug_log("**************************************")

            return(selected)
        },

        crossover = function(parent1, parent2) {
            if(length(parent1) > 1) {
                point <- sample(1:(length(private$parameters_constraints_level) - 1), 1)
                
                private$dispatch_debug_log(paste0("* | Crossover point: ", names(private$parameters_constraints_level)[[point]]))

                afterPoint <- names(private$parameters_constraints_level)[(point + 1):length(private$parameters_constraints_level)]
                
                offspring1 <- parent1
                offspring1[afterPoint] <- parent2[afterPoint]
                offspring1[names(private$preferences_groups)] <- parent2[names(private$preferences_groups)]
                offspring1[names(private$profitability_groups)] <- parent2[names(private$profitability_groups)]
                
                offspring2 <- parent2
                offspring2[afterPoint] <- parent1[afterPoint]
                offspring2[names(private$preferences_groups)] <- parent1[names(private$preferences_groups)]
                offspring2[names(private$profitability_groups)] <- parent1[names(private$profitability_groups)]

                private$dispatch_debug_log("* | Offspring 1: ")
                private$dispatch_debug_log("* | /")
                sapply(names(private$parameters_constraints_level), function(name) {
                    private$dispatch_debug_log(paste0("* || Parameter: ", name, " | Value: ", offspring1[[name]]))
                })
                sapply(names(private$preferences_groups), function(name) {
                    private$dispatch_debug_log(paste0("* || Preference: ", name, " | Value: ", offspring1[[name]]))
                })
                sapply(names(private$profitability_groups), function(name) {
                    private$dispatch_debug_log(paste0("* || Profitability: ", name, " | Value: ", offspring1[[name]]))
                })
                private$dispatch_debug_log("* | \\")

                private$dispatch_debug_log("* | Offspring 2: ")
                private$dispatch_debug_log("* | /")
                sapply(names(private$parameters_constraints_level), function(name) {
                    private$dispatch_debug_log(paste0("* || Parameter: ", name, " | Value: ", offspring2[[name]]))
                })
                sapply(names(private$preferences_groups), function(name) {
                    private$dispatch_debug_log(paste0("* || Preference: ", name, " | Value: ", offspring2[[name]]))
                })
                sapply(names(private$profitability_groups), function(name) {
                    private$dispatch_debug_log(paste0("* || Profitability: ", name, " | Value: ", offspring2[[name]]))
                })
                private$dispatch_debug_log("* | \\")

                return(list(offspring1, offspring2))
            } else {
                return(list(parent1, parent2))
            }
        },

        mutation = function(individual, mutation_rate) {
            for(name in names(private$parameters_constraints_level)) {
                private$dispatch_debug_log(paste0("* | Parameter: ", name))
                private$dispatch_debug_log("* | /")

                prob <- runif(1)

                private$dispatch_debug_log(paste0("* || Successful: ", ifelse(prob <= mutation_rate, "Yes", "No"), " | Prob: ", prob, " | Mutation rate: ", mutation_rate))

                if (prob <= mutation_rate) {
                    private$dispatch_debug_log(paste0("* || Old Value: ", individual[[name]]))
                    
                    individual[[name]] <- private$parameters[[name]]$generate_mutation(private$parameters, individual)
                
                    private$dispatch_debug_log(paste0("* || New Value: ", individual[[name]]))
                }

                private$dispatch_debug_log("* | \\")
            }

            if(private$optimise_preferences) {
                private$dispatch_debug_log("* | Preferences: ")
                private$dispatch_debug_log("* | /")

                prob <- runif(1)

                private$dispatch_debug_log(paste0("* || Successful: ", ifelse(prob <= mutation_rate, "Yes", "No"), " | Prob: ", prob, " | Mutation rate: ", mutation_rate))

                if (prob <= mutation_rate) {
                    private$dispatch_debug_log("* || Old Value: ")
                    private$dispatch_debug_log("* || /")

                    for(name in names(private$preferences_groups)) {
                        private$dispatch_debug_log(paste0("* ||| ", name, ": ", individual[[name]]))
                    }

                    private$dispatch_debug_log("* || \\")

                    preferences_sample <- as.list(xsample(
                        A = NULL, B = NULL, 
                        E = private$preferences_constraints$E, 
                        F = private$preferences_constraints$F, 
                        G = private$preferences_constraints$G, 
                        H = private$preferences_constraints$H,
                        iter = 5000,
                        output = 2
                    )$X[2,])

                    names(preferences_sample) <- names(private$preferences_groups)

                    individual[names(private$preferences_groups)] = preferences_sample
                
                    private$dispatch_debug_log("* || New Value: ")
                    private$dispatch_debug_log("* || /")

                    for(name in names(private$preferences_groups)) {
                        private$dispatch_debug_log(paste0("* ||| ", name, ": ", individual[[name]]))
                    }

                    private$dispatch_debug_log("* || \\")
                }
            }

            if(private$optimise_profitability) {
                private$dispatch_debug_log("* | Profitability: ")
                private$dispatch_debug_log("* | /")

                prob <- runif(1)

                private$dispatch_debug_log(paste0("* || Successful: ", ifelse(prob <= mutation_rate, "Yes", "No"), " | Prob: ", prob, " | Mutation rate: ", mutation_rate))

                if (prob <= mutation_rate) {
                    private$dispatch_debug_log("* || Old Value: ")
                    private$dispatch_debug_log("* || /")

                    for(name in names(private$profitability_groups)) {
                        private$dispatch_debug_log(paste0("* ||| ", name, ": ", individual[[name]]))
                    }

                    private$dispatch_debug_log("* || \\")

                    for(name in names(private$profitability_groups)) {
                        min_val <- max(0.0, private$profitability_groups[[name]]$min)
                        max_val <- min(1.0, private$profitability_groups[[name]]$max)

                        individual[[name]] <- runif(1, min = min_val, max = max_val)
                    }

                    private$dispatch_debug_log("* || New Value: ")
                    private$dispatch_debug_log("* || /")

                    for(name in names(private$profitability_groups)) {
                        private$dispatch_debug_log(paste0("* ||| ", name, ": ", individual[[name]]))
                    }

                    private$dispatch_debug_log("* || \\")
                }

                private$dispatch_debug_log("* | \\")
            }

            private$dispatch_debug_log("* | \\")

            return(individual)
        },

        repair = function(individual) {
            for(name in names(private$parameters_constraints_level)) {
                private$dispatch_debug_log(paste0("* | Parameter: ", name))
                private$dispatch_debug_log("* | /")
            
                if(private$parameters[[name]]$constraints_breached(individual)) {
                    private$dispatch_debug_log("* || Needs repair: Yes")
                    private$dispatch_debug_log(paste0("* || Incorrect value: ", individual[[name]]))
                    
                    individual[[name]] <- private$parameters[[name]]$generate_random_value(private$parameters, individual)
                
                    private$dispatch_debug_log(paste0("* || Repaired Value: ", individual[[name]]))    
                } else {
                    private$dispatch_debug_log("* || Needs repair: No")
                } 

                private$dispatch_debug_log("* | \\")
            }
            return(individual)
        },

        next_generation = function(population, fitness_engine, fitness_values, keepBest, tournament_size, crossover_prob, mutation_rate) {
            new_population <- list()
            
            # Elitismo: Conservación rígida de la mejor firma genética
            if(keepBest) {
                best_candidate <- fitness_engine$get_best_index(fitness_values)
                new_population[[1]] <- population[[best_candidate]]
            }
            
            size_allocation <- if(keepBest) length(population)-1 else length(population)
            selected_population <- private$selection(population, fitness_engine, fitness_values, k = tournament_size, size = size_allocation)

            private$dispatch_debug_log("######################################")
            private$dispatch_debug_log("##  Selected population  ##")
            private$dispatch_debug_log("###########################")

            private$display_population_debug(selected_population)

            private$dispatch_debug_log("######################################")
            
            private$dispatch_debug_log("**************************************")
            private$dispatch_debug_log("**  One Point Crossover operator  **")
            private$dispatch_debug_log("************************************")

            # Cruzamiento en un punto elástico
            for (i in seq(1, length(selected_population), by = 2)) {
                if((i + 1) <= length(selected_population)) {
                    private$dispatch_debug_log(paste0("* $ Crossover ", i%/%2+1))
                    private$dispatch_debug_log("*  /")

                    private$dispatch_debug_log("* | Parent 1: ")
                    private$dispatch_debug_log("* | /")
                    sapply(names(private$parameters_constraints_level), function(name) {
                        private$dispatch_debug_log(paste0("* || Parameter: ", name, " | Value: ", selected_population[[i]][[name]]))
                    })
                    sapply(names(private$preferences_groups), function(name) {
                        private$dispatch_debug_log(paste0("* || Preference: ", name, " | Value: ", selected_population[[i]][[name]]))
                    })
                    sapply(names(private$profitability_groups), function(name) {
                        private$dispatch_debug_log(paste0("* || Profitability: ", name, " | Value: ", selected_population[[i]][[name]]))
                    })
                    private$dispatch_debug_log("* | \\")

                    private$dispatch_debug_log("* | Parent 2: ")
                    private$dispatch_debug_log("* | /")
                    sapply(names(private$parameters_constraints_level), function(name) {
                        private$dispatch_debug_log(paste0("* || Parameter: ", name, " | Value: ", selected_population[[i+1]][[name]]))
                    })
                    sapply(names(private$preferences_groups), function(name) {
                        private$dispatch_debug_log(paste0("* || Preference: ", name, " | Value: ", selected_population[[i+1]][[name]]))
                    })
                    sapply(names(private$profitability_groups), function(name) {
                        private$dispatch_debug_log(paste0("* || Profitability: ", name, " | Value: ", selected_population[[i+1]][[name]]))
                    })
                    private$dispatch_debug_log("* | \\")

                    prob <- runif(1)

                    private$dispatch_debug_log(paste0("* | Successful: ", ifelse(prob <= crossover_prob, "Yes", "No"), " | Prob: ", prob, " | Crossover prob: ", crossover_prob))

                    if(prob <= crossover_prob) {
                        offspring <- private$crossover(selected_population[[i]], selected_population[[i+1]])
                    } else {
                        offspring <- selected_population[i:(i + 1)]
                    }

                    new_population[[length(new_population) + 1]] <- offspring[[1]]
                    new_population[[length(new_population) + 1]] <- offspring[[2]]
                
                    private$dispatch_debug_log("*  \\")
                } else {
                    new_population[[length(new_population) + 1]] <- selected_population[[i]]
                }
            }

            private$dispatch_debug_log("**************************************")

            private$dispatch_debug_log("######################################")
            private$dispatch_debug_log("##  New population after crossover  ##")
            private$dispatch_debug_log("######################################")

            private$display_population_debug(new_population)

            private$dispatch_debug_log("######################################")

            private$dispatch_debug_log("**************************************")
            private$dispatch_debug_log("**  Uniform Mutation operator  **")
            private$dispatch_debug_log("*********************************")

            # Mutación Uniforme e hilos de reparación acoplados
            population_to_mutate <- if(keepBest) seq(2, length(new_population)) else seq(1, length(new_population))

            for (i in population_to_mutate) {
                private$dispatch_debug_log(paste0("* $ Mutation ", i))
                private$dispatch_debug_log("*  /")

                new_population[[i]] <- private$mutation(new_population[[i]], mutation_rate = mutation_rate)
            
                private$dispatch_debug_log("*  \\")
            }

            private$dispatch_debug_log("**************************************")

            private$dispatch_debug_log("######################################")
            private$dispatch_debug_log("##  New population after mutation  ##")
            private$dispatch_debug_log("######################################")

            private$display_population_debug(new_population)

            private$dispatch_debug_log("######################################")

            private$dispatch_debug_log("**************************************")
            private$dispatch_debug_log("**  Repair parameters  **")
            private$dispatch_debug_log("*************************")

            for (i in seq_along(new_population)) {
                private$dispatch_debug_log(paste0("* $ Individual ", i))
                private$dispatch_debug_log("*  /")

                new_population[[i]] <- private$repair(new_population[[i]])
            
                private$dispatch_debug_log("*  \\")
            }

            private$dispatch_debug_log("**************************************")

            private$dispatch_debug_log("######################################")
            private$dispatch_debug_log("##  New population after repair  ##")
            private$dispatch_debug_log("###################################")

            private$display_population_debug(new_population)

            private$dispatch_debug_log("######################################")

            return(new_population)
        }
    ),
    
    public = list(
        # ============================================================================
        # PROCESADOR DE ORQUESTACIÓN GENERAL DEL ALGORITMO GENÉTICO
        # ============================================================================
        run = function(blueprint, output_path, program, base_config_path, seed, total_cores, program_cores, metrics_callback, evolution_callback, debug) {
            # Respaldo seguro de tipado numérico flotante largo de Weaver
            default_opts <- options()
            options(digits = 17)
            on.exit(options(default_opts))
            
            # 1. Guardamos la semilla de forma interna en el objeto R6
            private$current_seed <- as.integer(seed)

            # 2. Configuración del sistema RNG unificado (Esencial para scripts y UI)
            if (!is.null(private$current_seed)) {
                RNGkind("L'Ecuyer-CMRG") # Forzamos el tipo de generador apto para paralelismo
                set.seed(private$current_seed)
            }

            # NOTA IMPORTANTE: NO usamos future::availableCores() aquí. Esta
            # función, cuando el proceso ya está corriendo DENTRO de un future
            # anidado (p.ej. cuando la UI lanza la optimización en segundo
            # plano desde phase_3_server.R), aplica por seguridad una
            # protección anti-sobresuscripción de 'future' que limita el
            # resultado a 1 núcleo, independientemente del hardware real. Esa
            # protección se activa a nivel del proceso hijo en el momento en
            # que se lanza (antes de que este método pueda hacer nada), por
            # lo que ningún future::plan() definido aquí dentro puede
            # revertirla retroactivamente.
            #
            # parallelly::availableCores(which = "max") consulta directamente
            # el número real de núcleos lógicos que reporta el sistema
            # operativo, ignorando esa salvaguarda de anidación. Esto hace que
            # la validación funcione de forma idéntica tanto si 'run()' se
            # invoca manualmente desde un script (sin ningún future de por
            # medio) como si se invoca desde la UI (ya anidado dentro del
            # future en segundo plano de 'tarea_pesada'), sin necesidad de
            # tocar el plan de 'future' de la sesión principal de Shiny (y por
            # tanto sin arriesgar la reactividad en tiempo real del log).
            system_cores <- future::availableCores(methods = c("system", "Slurm"), constraints = "conservative")

            if (system_cores < total_cores) {
                stop(paste0("Error: The requested number of cores (", total_cores,
                    ") exceeds the maximum available on this system (", system_cores, ")."))
            }

            # 1. Enlace funcional del distribuidor desacoplado de telemetrías
            private$metrics_dispatcher <- metrics_callback
            private$evolution_dispatcher <- evolution_callback

            private$set_debug_mode(debug)

            if(.Platform$OS.type == "windows") {
                program <- gsub("\\", "/", program, fixed = TRUE)
                output_path <- gsub("\\", "/", output_path, fixed = TRUE)
                base_config_path <- gsub("\\", "/", base_config_path, fixed = TRUE)
            }

            ga_rules <- blueprint$genetic_algorithm_settings
            
            population_size <- as.integer(ga_rules$population_size)
            max_generations <- as.integer(ga_rules$max_generations)
            tournament_size <- as.integer(ga_rules$tournament_size)
            crossover_prob  <- as.numeric(ga_rules$crossover_prob)
            mutation_rate   <- as.numeric(ga_rules$mutation_rate)
            keepBest        <- as.logical(ga_rules$keep_best)

            # 4. Enrutamiento del workspace transaccional de salida en la máquina actual
            output_path <- file.path(output_path, paste0("optimisation_", format(Sys.time(), "%Y-%m-%d_%H-%M-%S")))
            dir.create(file.path(output_path, "configs"), recursive = TRUE, showWarnings = FALSE)
            dir.create(file.path(output_path, "results"), recursive = TRUE, showWarnings = FALSE)
            private$log_file <- file.path(output_path, "log.txt")
            
            # 5. Inicialización asíncrona del planificador de núcleos CPU de future
            private$program_cores = min(program_cores, total_cores)
            
            workers_optimiser <- max(1L, total_cores %/% private$program_cores)

            if (workers_optimiser > 1) {
                chosen_strategy <- if (.Platform$OS.type == "windows") "multisession" else "multicore"
    
                # MODIFICACIÓN CLAVE: Usamos .skip = TRUE (si la versión de future lo soporta) 
                # o forzamos la liberación del soft limit a través de las opciones locales
                options(parallelly.maxWorkers.localhost = "max") 
    
                future::plan(strategy = chosen_strategy, workers = workers_optimiser)
    
                private$dispatch_log(paste0("Parallel compute clusters verified. Processing threads pool: ", workers_optimiser), log_type = "success")
            } else {
                future::plan(strategy = "sequential")
                private$dispatch_log("Parallel bounds unavailable. Running sequential evolutionary loop maps.", log_type = "warning")
            }

            idle_cores <- total_cores - (workers_optimiser * private$program_cores)

            if (idle_cores > 0) {
                private$dispatch_log(paste0("Notice: ", idle_cores, " assigned core(s) will remain idle due to optimization allocation rules."), log_type = "warning")
            }


            # 3. Mapeo automatizado de Parameter Tying
            private$parse_parameters_from_json(blueprint$optimization_variables_allocation)
            
            fitness_engine <- private$build_fitness_engine_from_blueprint(blueprint$computing_environment)
            

            private$dispatch_log("Genetic algorithm started")

            # Display the parameters added to the genetic algorithm
            private$dispatch_log("── Parameters ──────────────────────────")
            private$dispatch_log("")

            for(parameter in private$parameters) {
                for(i in seq_along(parameter$fixed_destinations)) {
                    file_path <- file.path(base_config_path, parameter$fixed_destinations[[i]]$file_path)

                    if(!file.exists(file_path)) {
                        stop(paste0("Error: Destination file path '", file_path, "' does not exist."))
                    }

                    json_data <- fromJSON(file_path, simplifyVector = FALSE)
                
                    if(is.null(eval(parse(text = paste0("json_data$", parameter$fixed_destinations[[i]]$json_path))))) {
                        stop(paste0("Error: Destination json path '", parameter$fixed_destinations[[i]]$json_path, "' does not exist in the file '", file_path,"'."))
                    }
                }

                for(i in seq_along(parameter$calculated_destinations)) {
                    for(j in seq_along(parameter$calculated_destinations[[i]]$files)) {
                        file_path <- file.path(base_config_path, parameter$calculated_destinations[[i]]$files[[j]]$file_path)

                        if(!file.exists(file_path)) {
                            stop(paste0("Error: Destination file path '", file_path, "' does not exist."))
                        }

                        json_data <- fromJSON(file_path, simplifyVector = FALSE)

                        if(is.null(eval(parse(text = paste0("json_data$", parameter$calculated_destinations[[i]]$files[[j]]$json_path))))) {
                            stop(paste0("Error: Destination json path '", parameter$calculated_destinations[[i]]$files[[j]]$json_path, "' does not exist in the file '", file_path,"'."))
                        }
                    }
                }
            }

            private$parameters <- private$parameters[sample(length(private$parameters))]

            private$calculate_parameters_constraints_level()

            parameters_info_lines <- private$display_parameters()

            for (line in parameters_info_lines) {
                private$dispatch_log(line, log_type = "info")
            }


            if(private$optimise_preferences) {
                csv_path = file.path(base_config_path, "species", "ontogeneticLinksPreference.csv")
                preference_data <- read.csv(csv_path, header = TRUE, row.names = 1, sep = ",", check.names = FALSE)

                num_groups <- length(private$preferences_groups)
                num_cols <- ncol(preference_data)

                equality_constraints <- matrix(0, nrow = num_cols, ncol = num_groups+1)

                inequality_constraints <- matrix(0, nrow = 2*num_groups, ncol = num_groups+1) ## >=

                colums <- colnames(preference_data)
                rows <- rownames(preference_data)

                group_names <- names(private$preferences_groups)
                cell_owner_group <- list()

                for (g in seq_len(num_groups)) {
                    group <- group_names[[g]]

                    for (cell in private$preferences_groups[[group]]$cells[[1]]) {
                        cell_key <- paste(cell[[1]], cell[[2]], sep = "||")
                        current_owner <- cell_owner_group[[cell_key]]

                        if (!is.null(current_owner) && current_owner != g) {
                            stop(paste0(
                                "Error: Overlapping preference groups are not allowed. Cell ('",
                                cell[[1]], "', '", cell[[2]],
                                "') is assigned to both groups '",
                                group_names[[current_owner]], "' and '", group, "'."
                            ))
                        }

                        cell_owner_group[[cell_key]] <- g
                    }
                }

                for (j in 1:length(colums)) {

                    fixed_sum <- 0

                    for (i in 1:length(rows)) {

                        cell_key <- paste(rows[[i]], colums[[j]], sep = "||")
                        owner_group <- cell_owner_group[[cell_key]]

                        if (!is.null(owner_group)) {
                            equality_constraints[j, owner_group] <- equality_constraints[j, owner_group] + 1
                        } else {
                            fixed_sum <- fixed_sum + preference_data[rows[[i]], colums[[j]]]
                        }
                    }

                    if (fixed_sum > 1.0 + 1e-9) {
                        stop(paste0(
                            "Error: Unsolvable preference system. Column '", colums[[j]],
                            "' has immutable values summing ", round(fixed_sum, 6),
                            ", which exceeds 1.0."
                        ))
                    }

                    equality_constraints[[j, ncol(equality_constraints)]] <- 1.0 - fixed_sum
                }


                for (g in 1:num_groups) {
                    group <- group_names[[g]]

                    inequality_constraints[[(g-1)*2+1, g]] <- 1
                    inequality_constraints[[(g-1)*2+2, g]] <- -1

                    inequality_constraints[[(g-1)*2+1, ncol(inequality_constraints)]] <- private$preferences_groups[[group]]$min
                    inequality_constraints[[(g-1)*2+2, ncol(inequality_constraints)]] <- -private$preferences_groups[[group]]$max
                    
                }


                
                private$preferences_constraints <- list(
                    E = equality_constraints[, -ncol(equality_constraints), drop = FALSE],
                    F = equality_constraints[, ncol(equality_constraints)],
                    G = inequality_constraints[, -ncol(inequality_constraints), drop = FALSE],
                    H = inequality_constraints[, ncol(inequality_constraints)]
                )

                feasibility_test <- linprog(
                    E = private$preferences_constraints$E,
                    F = private$preferences_constraints$F,
                    G = private$preferences_constraints$G,
                    H = private$preferences_constraints$H,
                    Cost = rep(0, num_groups)
                )

                if (!identical(feasibility_test$IsFeasible, TRUE)) {
                    stop("Error: Unsolvable preference system. With the selected optimized cells and min/max bounds, there is no feasible solution that satisfies all predator column constraints while keeping non-optimized cells immutable.")
                }
            }

            if(private$optimise_profitability) {
                csv_path = file.path(base_config_path, "species", "ontogeneticLinksProfitability.csv")
                profitability_data <- read.csv(csv_path, header = TRUE, row.names = 1, sep = ",", check.names = FALSE)

                group_names <- names(private$profitability_groups)
                cell_owner_group <- list()

                for (g in seq_len(length(group_names))) {
                    group <- group_names[[g]]

                    min_val <- private$profitability_groups[[group]]$min
                    max_val <- private$profitability_groups[[group]]$max

                    if (min_val < 0.0 || max_val > 1.0 || min_val > max_val) {
                        stop(paste0(
                            "Error: Invalid profitability bounds in group '", group,
                            "'. Bounds must satisfy 0.0 <= min <= max <= 1.0."
                        ))
                    }

                    for (cell in private$profitability_groups[[group]]$cells[[1]]) {
                        if (!(cell[[1]] %in% rownames(profitability_data)) || !(cell[[2]] %in% colnames(profitability_data))) {
                            stop(paste0(
                                "Error: Profitability cell ('", cell[[1]], "', '", cell[[2]], "') does not exist in ontogeneticLinksProfitability.csv."
                            ))
                        }

                        cell_key <- paste(cell[[1]], cell[[2]], sep = "||")
                        current_owner <- cell_owner_group[[cell_key]]

                        if (!is.null(current_owner) && current_owner != g) {
                            stop(paste0(
                                "Error: Overlapping profitability groups are not allowed. Cell ('",
                                cell[[1]], "', '", cell[[2]],
                                "') is assigned to both groups '",
                                group_names[[current_owner]], "' and '", group, "'."
                            ))
                        }

                        cell_owner_group[[cell_key]] <- g
                    }
                }
            }


            private$dispatch_log("")
            private$dispatch_log("── Genetic Algorithm ───────────────────")
            private$dispatch_log("")


            # --- BUCLE EVOLUTIVO DEL ALGORITMO GENÉTICO ---
            header_names <- c("Generation", "Individual", names(private$parameters), names(private$preferences_groups), names(private$profitability_groups), fitness_engine$get_header(), "Result_folder")
            private$evolution_history <- data.frame(matrix(ncol = length(header_names), nrow = 0), stringsAsFactors = FALSE)
            colnames(private$evolution_history) <- header_names

            best_fitness <- c(); mean_fitness <- c()
            population <- private$initialize_population(population_size)

            for (generation in 1:max_generations) {
                private$dispatch_log("------------------------------------------------")
                private$dispatch_log(paste0("## Generation ", generation, " of ", max_generations," ##"))
                
                result <- private$evaluate_population(population, fitness_engine, program, output_path, base_config_path, current_gen = generation, max_gen = max_generations)
                
                fitness_values <- result$fitness_values
                result_folders <- result$result_folders

                # Volcado de filas al archivo de historial físico
                for (i in seq_along(population)) {
                    new_row <- unlist(list(
                        generation, i, 
                        unname(population[[i]][names(private$parameters)]), 
                        unname(population[[i]][names(private$preferences_groups)]), 
                        unname(population[[i]][names(private$profitability_groups)]), 
                        fitness_engine$get_row_info(fitness_values[[i]]), 
                        result_folders[[i]]
                    ))
                    new_row_df <- as.data.frame(t(new_row), stringsAsFactors = FALSE)
                    colnames(new_row_df) <- colnames(private$evolution_history)
                    private$evolution_history <- rbind(private$evolution_history, new_row_df)
                }
                write.table(private$evolution_history, file.path(output_path, "evolution_history.txt"), sep = "\t", row.names = FALSE, col.names = TRUE)

                curr_best_idx <- fitness_engine$get_best_index(fitness_values)
                best_fitness  <- c(best_fitness, list(fitness_values[[curr_best_idx]]))
                mean_fitness  <- c(mean_fitness, list(fitness_engine$calculate_mean_value(fitness_values)))
                
                summary_lines <- fitness_engine$display_generation_summary(best_fitness[[length(best_fitness)]], mean_fitness[[length(mean_fitness)]])
                
                for (line in summary_lines) {
                    private$dispatch_log(line, log_type = "info", generation = generation, best_fit = best_fitness[[length(best_fitness)]], mean_fit = mean_fitness[[length(mean_fitness)]])
                }

                if(generation < max_generations) {
                    population = private$next_generation(population, fitness_engine, fitness_values, keepBest, tournament_size, crossover_prob, mutation_rate)
                }

                private$dispatch_log("------------------------------------------------")

                gc()
            }

            best_candidate <- fitness_engine$get_best_index(fitness_values)
            private$dispatch_log(paste("Optimal eco-mathematical convergence achieved. Best run folder blueprint saved at:", result_folders[[best_candidate]]), log_type = "success")
            
            private$dispatch_log("------------------------------------------------")
            
            private$dispatch_log("Genetic algorithm completed")

            dir.create(file.path(output_path, "fitness_graphs"), showWarnings = FALSE)
            fitness_engine$save_progress_graph(file.path(output_path, "fitness_graphs"), max_generations, best_fitness, mean_fitness)
            
            private$dispatch_log("Weaver Engine process loops terminated successfully. Releasing system locks.", log_type = "system")
        }
    )
)

WeaverOptimiserClass$predefined_fitness_functions <- list(
    "totalTime" = function(result_folder) {
        read.table(file.path(result_folder, "breakConditionsInfo.txt") , header = TRUE, sep = "\t", stringsAsFactors = FALSE)$totalTime
    },
    "minAbundance" = function(result_folder) {
        df <- read.delim(file.path(result_folder, "extendedDailySummary.txt"))
        active_cols <- grep("_ACTIVE$", colnames(df), value = TRUE)
        species <- sub("_ACTIVE$", "", active_cols)
        sum_df <- as.data.frame(lapply(species, function(e) {
            rowSums(df[, paste0(e, "_", c("UNBORN", "ACTIVE", "REPRODUCING", "PUPA", "SATIATED", "HANDLING", "DIAPAUSE"))[paste0(e, "_", c("UNBORN", "ACTIVE", "REPRODUCING", "PUPA", "SATIATED", "HANDLING", "DIAPAUSE")) %in% colnames(df)]], na.rm = TRUE)
        }))
        min(apply(sum_df, 1, min, na.rm = TRUE), na.rm = TRUE)
    },
    "coefficientOfVariation" = function(result_folder) {
        read.table(file.path(result_folder, "breakConditionsInfo.txt"), header=TRUE, sep="\t", stringsAsFactors=FALSE)$coefficientOfVariation
    }
)

WeaverOptimiserClass$get_predefined_fitness_function_names <- function() {
    return(names(WeaverOptimiserClass$predefined_fitness_functions))
}