library(R6)
library(future)
library(future.apply)
library(progressr)
handlers("txtprogressbar")
library(ggplot2)
library(svglite)
library(dplyr)
library(limSolve)


WeaverOptimiserClass <- R6Class("WeaverOptimiserClass",
    private = list(
        optimise_preferences = FALSE,

        preferences_groups = list(),

        preferences_constraints = NULL,

        parameters = list(),  # Attribute to hold the vector of parameters

        combination_history = tibble(
            id = integer()
        ),

        combination_results = list(),

        parameters_constraints_level = list(),

        parallel = NULL,

        debug = NULL,

        log_file = NULL,

        evolution_history = NULL,

        add_combination_to_history = function(new_combination) {
            new_combination$id <- nrow(private$combination_history) + 1

            private$combination_history <- bind_rows(private$combination_history, new_combination)

            return(new_combination)
        },

        set_combination_fitness = function(id, fitness, result_folder) {
            private$combination_results[[id]] = list()

            private$combination_results[[id]]$fitness <- fitness
            private$combination_results[[id]]$result_folder <- result_folder
        },

        combination_already_added = function(combination) {
            index <- which(
                Reduce(`&`, Map(function(col, val) private$combination_history[[col]] == val, 
                                names(combination), combination))
            )

            return(length(index) > 0)
        },

        set_debug_mode = function(debug) {
            private$debug = debug
        },

        log_message = function(message) {
            cat(paste("[", Sys.time(), "]\t", message, "\n"), file = private$log_file, append = TRUE)
        },

        display_population_debug = function(population) {
            sapply(1:length(population), function(i) {
                self$display_debug(paste0("# $ Individual ", i))
                self$display_debug("#  /")

                sapply(names(private$parameters_constraints_level), function(name) {
                    self$display_debug(paste0("# | Parameter: ", name, " | Value: ", population[[i]][[name]]))
                })

                sapply(names(private$preferences_groups), function(name) {
                    self$display_debug(paste0("# | Preference: ", name, " | Value: ", population[[i]][[name]]))
                })

                self$display_debug("#  \\")
            })
        },

        calculate_parameters_constraints_level = function() {
            for(name in names(private$parameters)) {
                private$parameters_constraints_level[[name]] = private$parameters[[name]]$calculate_constraints_level(private$parameters)
            }

            private$parameters_constraints_level = private$parameters_constraints_level[order(unlist(private$parameters_constraints_level))]
        },

        count_decimal_digits = function(x) {
            if (x %% 1 == 0) return(0)  # Si es un número entero, devuelve 0
            str_x <- sub("0+$", "", as.character(x))  # Elimina ceros finales
            partes <- strsplit(str_x, "\\.")[[1]]  # Divide en parte entera y decimal
            if (length(partes) < 2) return(0)  # Si no hay parte decimal, devuelve 0
            nchar(partes[2])  # Cuenta los dígitos de la parte decimal
        },

        # Method to display parameters (optional)
        display_parameters = function() {
            private$parameters[[1]]$show()

            if(length(private$parameters) > 1) {
                for(param in private$parameters[2:length(private$parameters)]) {
                    self$display("")
                    self$display("--------------------------------------------------")
                    self$display("")

                    param$show()
                }
            }

            if(private$optimise_preferences) {
                for(param in private$preferences_groups) {
                    self$display("")
                    self$display("--------------------------------------------------")
                    self$display("")

                    param$show()
                }
            }
        },

        # Función para generar un individuo aleatorio
        generate_individual = function() {
            individual <- list()

            for(name in names(private$parameters_constraints_level)) {
                individual[[name]] = private$parameters[[name]]$generate_random_value(private$parameters, individual)

                self$display_debug(paste0("* | Parameter: ", name, " | Value: ", individual[[name]]))
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

                    self$display_debug(paste0("* | Preference: ", name, " | Value: ", individual[[name]]))
                }
            }

            return(individual)
        },

        # Función de inicialización de la población
        initialize_population = function(population_size) {
            self$display_debug("**************************************")
            self$display_debug("**  Initialize population  **")
            self$display_debug("*****************************")

            population <- lapply(1:population_size, function(x) {
                self$display_debug(paste0("* $ Individual ", x))
                self$display_debug("*  /")

                individual <- private$generate_individual()

                self$display_debug("*  \\")

                return(individual)
            })

            self$display_debug("**************************************")

            return(population)
        },

        create_config = function(individual, output_path, base_config_path) {
            library(jsonlite)

            config_path <- file.path(output_path, "configs", paste0(basename(base_config_path), "_", individual$id))

            dir.create(config_path, recursive = TRUE)

            file.copy(list.files(base_config_path, full.names = TRUE), config_path, recursive = TRUE)

            for(name in names(private$parameters)) {
                if(class(private$parameters[[name]])[1] == "Abundances") {
                    individualsPerInstar <- round(individual[[name]]*private$parameters[[name]]$densitiesVector/sum(private$parameters[[name]]$densitiesVector))

                    for(i in seq_along(private$parameters[[name]]$fixed_destinations)) {
                        json_data <- fromJSON(file.path(config_path, private$parameters[[name]]$fixed_destinations[[i]]$file_path), simplifyVector = FALSE)
                    
                        eval(parse(text = paste0("json_data$", private$parameters[[name]]$fixed_destinations[[i]]$json_path, " <- individualsPerInstar")))

                        write(toJSON(json_data, auto_unbox = TRUE, pretty = TRUE, null = "null", digits = 17), file.path(config_path, private$parameters[[name]]$fixed_destinations[[i]]$file_path))
                    }
                }
                else {
                    for(i in seq_along(private$parameters[[name]]$fixed_destinations)) {
                        json_data <- fromJSON(file.path(config_path, private$parameters[[name]]$fixed_destinations[[i]]$file_path), simplifyVector = FALSE)
                    
                        eval(parse(text = paste0("json_data$", private$parameters[[name]]$fixed_destinations[[i]]$json_path, " <- individual[[name]]")))

                        write(toJSON(json_data, auto_unbox = TRUE, pretty = TRUE, null = "null", digits = 17), file.path(config_path, private$parameters[[name]]$fixed_destinations[[i]]$file_path))
                    }

                    for(i in seq_along(private$parameters[[name]]$calculated_destinations)) {
                        for(j in seq_along(private$parameters[[name]]$calculated_destinations[[i]]$files)) {
                            json_data <- fromJSON(file.path(config_path, private$parameters[[name]]$calculated_destinations[[i]]$files[[j]]$file_path), simplifyVector = FALSE)

                            value <- private$parameters[[name]]$calculated_destinations[[i]][["function"]](individual, json_data)

                            eval(parse(text = paste0("json_data$", private$parameters[[name]]$calculated_destinations[[i]]$files[[j]]$json_path, " <- value")))

                            write(toJSON(json_data, auto_unbox = TRUE, pretty = TRUE, null = "null", digits = 17), file.path(config_path, private$parameters[[name]]$calculated_destinations[[i]]$files[[j]]$file_path))
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

            return(config_path)
        },

        simulate_individual = function(individual, program, output_path, base_config_path) {
            config_path <- private$create_config(individual, output_path, base_config_path)
            if(.Platform$OS.type == "windows") {
                config_path <- gsub("/", "\\", config_path, fixed = TRUE)
            }

            results_output_path <- file.path(output_path, "results")
            if(.Platform$OS.type == "windows") {
                results_output_path <- gsub("/", "\\", results_output_path, fixed = TRUE)
            }

            output_file <- tempfile(fileext = ".txt")
            error_file <- tempfile(fileext = ".txt")

            status <- system2(
                program, 
                args = c(
                    "--mode", "FromConfig",
                    "-I", config_path,
                    "-O", results_output_path,
                    "--silent"
                ),
                stdout = output_file,
                stderr = error_file,
                wait = TRUE
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

            result_folder <- sub(".*Result folder: ([^ ]+).*", "\\1", paste(readLines(output_file, warn = FALSE), collapse = " "))

            return(result_folder)
        },

        # Función de aptitud (fitness)
        evaluation_function = function(individual, program, output_path, base_config_path, progress_bar) {
            result_folder <- private$simulate_individual(individual, program, output_path, base_config_path)

            # Actualiza la barra de progreso
            progress_bar(amount = 1)

            gc()

            return(result_folder)
        },

        evaluate_population = function(population, fitness_class, program, output_path, base_config_path) {
            indexes_to_evaluate <- sapply(population, function(individual) !private$combination_already_added(individual))

            population_to_evaluate <- bind_rows(population[indexes_to_evaluate])
            population_to_evaluate <- population_to_evaluate %>% distinct()

            population_to_evaluate <- bind_rows(
                lapply(
                    seq_len(nrow(population_to_evaluate)),
                    function(row) {
                        private$add_combination_to_history(population_to_evaluate[row,])
                    }
                )
            )

            if(nrow(population_to_evaluate) > 0)
            {
                result_folder_vector <- with_progress({
                    progress_bar <- progressor(nrow(population_to_evaluate))

                    if(private$parallel) {
                        future_lapply(
                            split(population_to_evaluate, seq_len(nrow(population_to_evaluate))), 
                            private$evaluation_function, program, output_path, base_config_path, progress_bar
                        )
                    }
                    else {
                        lapply(
                            split(population_to_evaluate, seq_len(nrow(population_to_evaluate))), 
                            private$evaluation_function, program, output_path, base_config_path, progress_bar
                        )
                    }
                })

                new_fitness_values <- lapply(result_folder_vector, function(result_folder) fitness_class$evaluate(result_folder))

                lapply(
                    seq_len(nrow(population_to_evaluate)),
                    function(row) {
                        private$set_combination_fitness(population_to_evaluate[row,]$id, new_fitness_values[[row]], result_folder_vector[[row]])
                    }
                )
            }

            fitness_values <- lapply(population, function(individual) { return(private$obtain_fitness_value(individual)) })

            result_folders <- lapply(population, function(individual) { return(private$obtain_result_folder(individual)) })

            self$display_debug("**************************************")
            self$display_debug("**  Evaluate population  **")
            self$display_debug("***************************")

            sapply(1:length(population), function(i) {
                self$display_debug(paste0("* $ Individual ", i))
                self$display_debug("*  /")

                self$display_debug(paste0("* | Already evaluated: ", ifelse(indexes_to_evaluate[[i]], "No", "Yes") ))

                sapply(names(private$parameters_constraints_level), function(name) {
                    self$display_debug(paste0("* | Parameter: ", name, " | Value: ", population[[i]][[name]]))
                })

                sapply(names(private$preferences_groups), function(name) {
                    self$display_debug(paste0("* | Preference: ", name, " | Value: ", population[[i]][[name]]))
                })

                self$display_debug("* | Fitness:")
                self$display_debug("* | /")

                sapply(fitness_class$get_display_info(fitness_values[[i]]), function(info) {
                    self$display_debug(paste0("* || ", info))
                })

                self$display_debug("* | \\")
                self$display_debug("*  \\")
            })

            self$display_debug("**************************************")
            
            return(list(fitness_values = fitness_values, result_folders = result_folders))
        },

        obtain_fitness_value = function(individual) {
            index <- which(
                Reduce(`&`, Map(function(col, val) private$combination_history[[col]] == val, 
                                names(individual), individual))
            )

            return(private$combination_results[[private$combination_history[[index, "id"]]]]$fitness)
        },

        obtain_result_folder = function(individual) {
            index <- which(
                Reduce(`&`, Map(function(col, val) private$combination_history[[col]] == val, 
                                names(individual), individual))
            )

            return(private$combination_results[[private$combination_history[[index, "id"]]]]$result_folder)
        },

        selection = function(population, fitness_class, fitness_values, k, size) {
            self$display_debug("**************************************")
            self$display_debug("**  Tournament Selection operator  **")
            self$display_debug("*************************************")

            selected <- vector("list", size)

            for (i in seq(1, size)) {
                self$display_debug(paste0("* $ Tournament ", i))
                self$display_debug("*  /")

                candidates <- sample(seq_along(population), k)

                sapply(candidates, function(candidate) { 
                    self$display_debug(paste0("* | Candidate: ", candidate)) 
                    self$display_debug("* | Fitness:") 
                    self$display_debug("* | /")

                    sapply(fitness_class$get_display_info(fitness_values[[candidate]]), function(info) {
                        self$display_debug(paste0("* || ", info))
                    })

                    self$display_debug("* | \\")
                })

                best_candidate <- candidates[[fitness_class$get_best_index(fitness_values[candidates])]]
                
                selected[[i]] <- population[[best_candidate]]

                self$display_debug(paste0("* | Winner: ", best_candidate))

                self$display_debug("*  \\")
            }

            self$display_debug("**************************************")

            return(selected)
        },

        crossover = function(parent1, parent2) {
            if(length(parent1) > 1) {
                point <- sample(1:(length(private$parameters_constraints_level) - 1), 1)

                self$display_debug(paste0("* | Crossover point: ", names(private$parameters_constraints_level)[[point]]))

                afterPoint <- names(private$parameters_constraints_level)[(point + 1):length(private$parameters_constraints_level)]

                offspring1 <- parent1
                offspring1[afterPoint] <- parent2[afterPoint]
                offspring1[names(private$preferences_groups)] <- parent2[names(private$preferences_groups)]

                offspring2 <- parent2
                offspring2[afterPoint] <- parent1[afterPoint]
                offspring2[names(private$preferences_groups)] <- parent1[names(private$preferences_groups)]

                self$display_debug("* | Offspring 1: ")
                self$display_debug("* | /")
                sapply(names(private$parameters_constraints_level), function(name) {
                    self$display_debug(paste0("* || Parameter: ", name, " | Value: ", offspring1[[name]]))
                })
                sapply(names(private$preferences_groups), function(name) {
                    self$display_debug(paste0("* || Preference: ", name, " | Value: ", offspring1[[name]]))
                })
                self$display_debug("* | \\")

                self$display_debug("* | Offspring 2: ")
                self$display_debug("* | /")
                sapply(names(private$parameters_constraints_level), function(name) {
                    self$display_debug(paste0("* || Parameter: ", name, " | Value: ", offspring2[[name]]))
                })
                sapply(names(private$preferences_groups), function(name) {
                    self$display_debug(paste0("* || Preference: ", name, " | Value: ", offspring2[[name]]))
                })
                self$display_debug("* | \\")

                return(list(offspring1, offspring2))
            } else {
                return(list(parent1, parent2))
            }
        },

        mutation = function(individual, mutation_rate) {
            for(name in names(private$parameters_constraints_level)) {
                self$display_debug(paste0("* | Parameter: ", name))
                self$display_debug("* | /")

                prob <- runif(1)

                self$display_debug(paste0("* || Successful: ", ifelse(prob <= mutation_rate, "Yes", "No"), " | Prob: ", prob, " | Mutation rate: ", mutation_rate))

                if (prob <= mutation_rate) {
                    self$display_debug(paste0("* || Old Value: ", individual[[name]]))

                    individual[[name]] <- private$parameters[[name]]$generate_mutation(private$parameters, individual)

                    self$display_debug(paste0("* || New Value: ", individual[[name]]))
                }

                self$display_debug("* | \\")
            }


            self$display_debug("* | Preferences: ")
            self$display_debug("* | /")

            prob <- runif(1)

            self$display_debug(paste0("* || Successful: ", ifelse(prob <= mutation_rate, "Yes", "No"), " | Prob: ", prob, " | Mutation rate: ", mutation_rate))

            if (prob <= mutation_rate) {
                self$display_debug("* || Old Value: ")
                self$display_debug("* || /")

                for(name in names(private$preferences_groups)) {
                    self$display_debug(paste0("* ||| ", name, ": ", individual[[name]]))
                }

                self$display_debug("* || \\")

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

                self$display_debug("* || New Value: ")
                self$display_debug("* || /")

                for(name in names(private$preferences_groups)) {
                    self$display_debug(paste0("* ||| ", name, ": ", individual[[name]]))
                }

                self$display_debug("* || \\")
            }

            self$display_debug("* | \\")


            return(individual)
        },

        repair = function(individual) {
            for(name in names(private$parameters_constraints_level)) {
                self$display_debug(paste0("* | Parameter: ", name))
                self$display_debug("* | /")

                result <- private$parameters[[name]]$constraints_breached(individual)

                if(result) {
                    self$display_debug("* || Needs repair: Yes")
                    self$display_debug(paste0("* || Incorrect value: ", individual[[name]]))

                    individual[[name]] <- private$parameters[[name]]$generate_random_value(private$parameters, individual)

                    self$display_debug(paste0("* || Repaired Value: ", individual[[name]]))
                }
                else {
                    self$display_debug("* || Needs repair: No")
                } 

                self$display_debug("* | \\")
            }

            return(individual)
        },

        # Función para ejecutar una generación
        next_generation = function(population, fitness_class, fitness_values, keepBest, tournament_size, crossover_prob, mutation_rate) {
            new_population <- list()


            if(keepBest) {
                best_candidate <- fitness_class$get_best_index(fitness_values)

                new_population[[1]] <- population[[best_candidate]]
            }


            if(keepBest) {
                selected_population <- private$selection(population = population, fitness_class = fitness_class, fitness_values = fitness_values, k = tournament_size, size = length(population)-1)
            }
            else {
                selected_population <- private$selection(population = population, fitness_class = fitness_class, fitness_values = fitness_values, k = tournament_size, size = length(population))
            }


            self$display_debug("######################################")
            self$display_debug("##  Selected population  ##")
            self$display_debug("###########################")

            private$display_population_debug(selected_population)

            self$display_debug("######################################")
            

            self$display_debug("**************************************")
            self$display_debug("**  One Point Crossover operator  **")
            self$display_debug("************************************")

            for (i in seq(1, length(selected_population), by = 2)) {
                if((i + 1) <= length(selected_population)) {
                    self$display_debug(paste0("* $ Crossover ", i%/%2+1))
                    self$display_debug("*  /")

                    self$display_debug("* | Parent 1: ")
                    self$display_debug("* | /")
                    sapply(names(private$parameters_constraints_level), function(name) {
                        self$display_debug(paste0("* || Parameter: ", name, " | Value: ", selected_population[[i]][[name]]))
                    })
                    sapply(names(private$preferences_groups), function(name) {
                        self$display_debug(paste0("* || Preference: ", name, " | Value: ", selected_population[[i]][[name]]))
                    })
                    self$display_debug("* | \\")

                    self$display_debug("* | Parent 2: ")
                    self$display_debug("* | /")
                    sapply(names(private$parameters_constraints_level), function(name) {
                        self$display_debug(paste0("* || Parameter: ", name, " | Value: ", selected_population[[i+1]][[name]]))
                    })
                    sapply(names(private$preferences_groups), function(name) {
                        self$display_debug(paste0("* || Preference: ", name, " | Value: ", selected_population[[i+1]][[name]]))
                    })
                    self$display_debug("* | \\")

                    prob <- runif(1)

                    self$display_debug(paste0("* | Successful: ", ifelse(prob <= crossover_prob, "Yes", "No"), " | Prob: ", prob, " | Crossover prob: ", crossover_prob))

                    if(prob <= crossover_prob) {
                        offspring <- private$crossover(selected_population[[i]], selected_population[[i+1]])
                    } 
                    else {
                        offspring <- selected_population[i:(i + 1)]
                    }

                    new_population[[length(new_population) + 1]] <- offspring[[1]]
                    new_population[[length(new_population) + 1]] <- offspring[[2]]

                    self$display_debug("*  \\")
                }
                else {
                    new_population[[length(new_population) + 1]] <- selected_population[[i]]
                }
            }

            self$display_debug("**************************************")

            self$display_debug("######################################")
            self$display_debug("##  New population after crossover  ##")
            self$display_debug("######################################")

            private$display_population_debug(new_population)

            self$display_debug("######################################")


            self$display_debug("**************************************")
            self$display_debug("**  Uniform Mutation operator  **")
            self$display_debug("*********************************")

            population_to_mutate <- NULL

            if(keepBest) {
                population_to_mutate <- seq(2, length(new_population))
            }
            else {
                population_to_mutate <- seq(1, length(new_population))
            }

            for (i in population_to_mutate) {
                self$display_debug(paste0("* $ Mutation ", i))
                self$display_debug("*  /")

                new_population[[i]] <- private$mutation(new_population[[1]], mutation_rate = mutation_rate)

                self$display_debug("*  \\")
            }
            
            self$display_debug("**************************************")

            self$display_debug("######################################")
            self$display_debug("##  New population after mutation  ##")
            self$display_debug("#####################################")

            private$display_population_debug(new_population)

            self$display_debug("######################################")


            self$display_debug("**************************************")
            self$display_debug("**  Repair parameters  **")
            self$display_debug("*************************")

            for (i in seq_along(new_population)) {
                self$display_debug(paste0("* $ Individual ", i))
                self$display_debug("*  /")

                new_population[[i]] <- private$repair(new_population[[i]])

                self$display_debug("*  \\")
            }

            self$display_debug("**************************************")

            self$display_debug("######################################")
            self$display_debug("##  New population after repair  ##")
            self$display_debug("###################################")

            private$display_population_debug(new_population)

            self$display_debug("######################################")


            return(new_population)
        }
    ),
    public = list(
        display = function(message, save_log=TRUE) {
            cat(paste0(message, "\n"))
            flush.console()

            if(save_log) {
                private$log_message(message)
            }
        },

        display_debug = function(message) {
            if(private$debug) {
                private$log_message(message)
            }
        },

        # Base class Parameter
        Parameter = R6Class("Parameter",
            public = list(
                initialize = function() {
                    stop("Cannot instantiate the base class directly")
                }
            )
        ),

        # Subclass RealValued
        RealValued = R6Class("RealValued",
            inherit = WeaverOptimiserClass$Parameter,
            public = list(
                name = character(0),
                min = numeric(0),
                max = numeric(0),
                fixed_destinations = list(),
                calculated_destinations = list(),
                constraints = list(),
                
                initialize = function(name, min, max, fixed_destinations, calculated_destinations, constraints) {
                    self$name <- name

                    self$min <- min
                    self$max <- max

                    self$fixed_destinations <- fixed_destinations
                    self$calculated_destinations <- calculated_destinations

                    for(i in seq_along(self$fixed_destinations)) {
                        file_path <- strsplit(self$fixed_destinations[[i]]$file_path, "/")[[1]]
                        file_path <- do.call(file.path, as.list(file_path))

                        self$fixed_destinations[[i]]$file_path <- file_path
                    }

                    for(i in seq_along(self$calculated_destinations)) {
                        for(j in seq_along(self$calculated_destinations[[i]]$files)) {
                            file_path <- strsplit(self$calculated_destinations[[i]]$files[[j]]$file_path, "/")[[1]]
                            file_path <- do.call(file.path, as.list(file_path))

                            self$calculated_destinations[[i]]$files[[j]]$file_path <- file_path
                        }
                    }

                    self$constraints <- constraints
                },

                calculate_constraints_level = function(parameters) {
                    constraints_level = 0

                    for(constraint in self$constraints) {
                        constraints_level = max(constraints_level, parameters[[constraint$name]]$calculate_constraints_level(parameters) + 1)
                    }

                    return(constraints_level)
                },
                
                generate_random_value = function(parameters, individual) {
                    minAux = self$min
                    maxAux = self$max

                    for(constraint in self$constraints) {
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
            
                    while (new_value == individual[[self$name]]) {
                        new_value <- self$generate_random_value(parameters, individual)
                    }

                    return(new_value)
                },

                constraints_breached = function(individual) {
                    minAux = self$min
                    maxAux = self$max

                    for(constraint in self$constraints) {
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

                    return(individual[[self$name]] < minAux || maxAux < individual[[self$name]])
                },

                show = function() {
                    WeaverOptimiser$display(paste0("  Parameter Name: \"", self$name, "\""))
                    WeaverOptimiser$display(paste0("  Type: ", class(self)[1]))
                    WeaverOptimiser$display(paste0("  Minimum: ", self$min))
                    WeaverOptimiser$display(paste0("  Maximum: ", self$max))
                    WeaverOptimiser$display("  Fixed destinations:")
                    for(i in seq_along(self$fixed_destinations)) {
                        WeaverOptimiser$display("   /")
                        WeaverOptimiser$display(paste0("  | File ", i))
                        WeaverOptimiser$display(paste0("  | File path: \"", self$fixed_destinations[[i]]$file_path, "\""))
                        WeaverOptimiser$display(paste0("  | JSON path: \"", self$fixed_destinations[[i]]$json_path, "\""))
                        WeaverOptimiser$display("   \\")
                    }
                    WeaverOptimiser$display("  Calculated destinations:")
                    for(i in seq_along(self$calculated_destinations)) {
                        WeaverOptimiser$display("   /")
                        WeaverOptimiser$display(paste0("  | Destination ", i))
                        WeaverOptimiser$display("  | Function: function(individual, json_data)")
                        WeaverOptimiser$display(paste0("  | ", paste(deparse(body(self$calculated_destinations[[i]][["function"]])), collapse = "\n  | ")))
                        WeaverOptimiser$display("  | Files:")
                        for(j in seq_along(self$calculated_destinations[[i]]$files)) {
                            WeaverOptimiser$display("  |  /")
                            WeaverOptimiser$display(paste0("  | | File ", j))
                            WeaverOptimiser$display(paste0("  | | File path: \"", self$calculated_destinations[[i]]$files[[j]]$file_path, "\""))
                            WeaverOptimiser$display(paste0("  | | JSON path: \"", self$calculated_destinations[[i]]$files[[j]]$json_path, "\""))
                            WeaverOptimiser$display("  |  \\")
                        }
                        WeaverOptimiser$display("   \\")
                    }
                }
            )
        ),

        PreferenceCell = R6Class("PreferenceCell",
            public = list(
                predator_species = character(0),
                predator_instars = numeric(0),
                prey_species = character(0),
                prey_instars = numeric(0),

                initialize = function(predator_species, predator_instars, prey_species, prey_instars) {
                    self$predator_species <- predator_species
                    self$predator_instars <- predator_instars
                    self$prey_species <- prey_species
                    self$prey_instars <- prey_instars
                },

                generate_cells = function() {
                    cells <- list()

                    columns_values <- paste(self$predator_species, self$predator_instars, sep = "$")
                    rows_values <- paste(self$prey_species, self$prey_instars, sep = "$")

                    for(row in rows_values) {
                        for(column in columns_values) {
                            cells[[length(cells) + 1]] <- list(row, column)
                        }
                    }

                    return(cells)
                }
            )
        ),

        # Subclass Preference
        Preference = R6Class("Preference",
            inherit = WeaverOptimiserClass$Parameter,
            public = list(
                name = character(0),
                cells = list(),
                min = numeric(0),
                max = numeric(0),
                
                initialize = function(name, cells, min, max) {
                    self$name <- name
                    
                    for(cell in cells) {
                        self$cells[[length(self$cells) + 1]] <- cell$generate_cells()
                    }

                    self$min <- min
                    self$max <- max
                },

                show = function() {
                    WeaverOptimiser$display(paste0("  Parameter Name: \"", self$name, "\""))
                    WeaverOptimiser$display(paste0("  Type: ", class(self)[1]))
                    WeaverOptimiser$display("  Cells:")
                    WeaverOptimiser$display("   /")
                    for(cell in self$cells[[1]]) {
                        WeaverOptimiser$display(paste0("  | Row: ", cell[[1]], " Column: ", cell[[2]]))
                    }
                    WeaverOptimiser$display("   \\")
                    WeaverOptimiser$display(paste0("  Minimum: ", self$min))
                    WeaverOptimiser$display(paste0("  Maximum: ", self$max))
                }
            )
        ),

        # Subclass Permutation
        Permutation = R6Class("Permutation",
            inherit = WeaverOptimiserClass$Parameter,
            public = list(
                name = character(0),
                elements = NULL,
                fixed_destinations = list(),
                calculated_destinations = list(),
                constraints = list(),
                
                initialize = function(name, elements, fixed_destinations, calculated_destinations, constraints) {
                    self$name <- name

                    self$elements <- elements

                    self$fixed_destinations <- fixed_destinations
                    self$calculated_destinations <- calculated_destinations

                    for(i in seq_along(self$fixed_destinations)) {
                        file_path <- strsplit(self$fixed_destinations[[i]]$file_path, "/")[[1]]
                        file_path <- do.call(file.path, as.list(file_path))

                        self$fixed_destinations[[i]]$file_path <- file_path
                    }

                    for(i in seq_along(self$calculated_destinations)) {
                        for(j in seq_along(self$calculated_destinations[[i]]$files)) {
                            file_path <- strsplit(self$calculated_destinations[[i]]$files[[j]]$file_path, "/")[[1]]
                            file_path <- do.call(file.path, as.list(file_path))

                            self$calculated_destinations[[i]]$files[[j]]$file_path <- file_path
                        }
                    }

                    self$constraints <- constraints
                },

                calculate_constraints_level = function(parameters) {
                    constraints_level = 0

                    for(constraint in self$constraints) {
                        constraints_level = max(constraints_level, parameters[[constraint$name]]$calculate_constraints_level(parameters) + 1)
                    }

                    return(constraints_level)
                },
                
                generate_random_value = function(parameters, individual) {
                    elementsAux = self$elements

                    for(constraint in self$constraints) {
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
                    return(sample(setdiff(self$elements, individual[[self$name]]), 1))
                },

                constraints_breached = function(individual) {
                    elementsAux = self$elements

                    for(constraint in self$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                elementsAux = individual[[constraint$name]]
                            } else {
                                elementsAux = constraint$elements
                            }
                        }
                    }

                    return(!individual[[self$name]] %in% elementsAux)
                },

                show = function() {
                    WeaverOptimiser$display(paste0("  Parameter Name: \"", self$name, "\""))
                    WeaverOptimiser$display(paste0("  Type: ", class(self)[1]))
                    WeaverOptimiser$display(paste0("  Elements: ", self$elements))
                    WeaverOptimiser$display("  Fixed destinations:")
                    for(i in seq_along(self$fixed_destinations)) {
                        WeaverOptimiser$display("   /")
                        WeaverOptimiser$display(paste0("  | File ", i))
                        WeaverOptimiser$display(paste0("  | File path: \"", self$fixed_destinations[[i]]$file_path, "\""))
                        WeaverOptimiser$display(paste0("  | JSON path: \"", self$fixed_destinations[[i]]$json_path, "\""))
                        WeaverOptimiser$display("   \\")
                    }
                    WeaverOptimiser$display("  Calculated destinations:")
                    for(i in seq_along(self$calculated_destinations)) {
                        WeaverOptimiser$display("   /")
                        WeaverOptimiser$display(paste0("  | Destination ", i))
                        WeaverOptimiser$display("  | Function: function(individual, json_data)")
                        WeaverOptimiser$display(paste0("  | ", paste(deparse(body(self$calculated_destinations[[i]][["function"]])), collapse = "\n  | ")))
                        WeaverOptimiser$display("  | Files:")
                        for(j in seq_along(self$calculated_destinations[[i]]$files)) {
                            WeaverOptimiser$display("  |  /")
                            WeaverOptimiser$display(paste0("  | | File ", j))
                            WeaverOptimiser$display(paste0("  | | File path: \"", self$calculated_destinations[[i]]$files[[j]]$file_path, "\""))
                            WeaverOptimiser$display(paste0("  | | JSON path: \"", self$calculated_destinations[[i]]$files[[j]]$json_path, "\""))
                            WeaverOptimiser$display("  |  \\")
                        }
                        WeaverOptimiser$display("   \\")
                    }
                }
            )
        ),

        # Subclass Integer
        Integer = R6Class("Integer",
            inherit = WeaverOptimiserClass$Parameter,
            public = list(
                name = character(0),
                min = numeric(0),
                max = numeric(0),
                fixed_destinations = list(),
                calculated_destinations = list(),
                constraints = list(),
                
                initialize = function(name, min, max, fixed_destinations, calculated_destinations, constraints) {
                    self$name <- name

                    self$min <- min
                    self$max <- max

                    self$fixed_destinations <- fixed_destinations
                    self$calculated_destinations <- calculated_destinations

                    for(i in seq_along(self$fixed_destinations)) {
                        file_path <- strsplit(self$fixed_destinations[[i]]$file_path, "/")[[1]]
                        file_path <- do.call(file.path, as.list(file_path))

                        self$fixed_destinations[[i]]$file_path <- file_path
                    }

                    for(i in seq_along(self$calculated_destinations)) {
                        for(j in seq_along(self$calculated_destinations[[i]]$files)) {
                            file_path <- strsplit(self$calculated_destinations[[i]]$files[[j]]$file_path, "/")[[1]]
                            file_path <- do.call(file.path, as.list(file_path))

                            self$calculated_destinations[[i]]$files[[j]]$file_path <- file_path
                        }
                    }

                    self$constraints <- constraints
                },

                calculate_constraints_level = function(parameters) {
                    constraints_level = 0

                    for(constraint in self$constraints) {
                        constraints_level = max(constraints_level, parameters[[constraint$name]]$calculate_constraints_level(parameters) + 1)
                    }

                    return(constraints_level)
                },
                
                generate_random_value = function(parameters, individual) {
                    minAux = self$min
                    maxAux = self$max

                    for(constraint in self$constraints) {
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
                    displacement <- sample(1:(self$max-self$min), 1)
                    current_pos <- (individual[[self$name]]-self$min+1)
                    new_pos <- (current_pos + displacement) %% (self$max-self$min+1)

                    return(as.integer(self$min + new_pos - 1))
                },

                constraints_breached = function(individual) {
                    minAux = self$min
                    maxAux = self$max

                    for(constraint in self$constraints) {
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

                    return(individual[[self$name]] < minAux || maxAux < individual[[self$name]])
                },

                show = function() {
                    WeaverOptimiser$display(paste0("  Parameter Name: \"", self$name, "\""))
                    WeaverOptimiser$display(paste0("  Type: ", class(self)[1]))
                    WeaverOptimiser$display(paste0("  Minimum: ", self$min))
                    WeaverOptimiser$display(paste0("  Maximum: ", self$max))
                    WeaverOptimiser$display("  Fixed destinations:")
                    for(i in seq_along(self$fixed_destinations)) {
                        WeaverOptimiser$display("   /")
                        WeaverOptimiser$display(paste0("  | File ", i))
                        WeaverOptimiser$display(paste0("  | File path: \"", self$fixed_destinations[[i]]$file_path, "\""))
                        WeaverOptimiser$display(paste0("  | JSON path: \"", self$fixed_destinations[[i]]$json_path, "\""))
                        WeaverOptimiser$display("   \\")
                    }
                    WeaverOptimiser$display("  Calculated destinations:")
                    for(i in seq_along(self$calculated_destinations)) {
                        WeaverOptimiser$display("   /")
                        WeaverOptimiser$display(paste0("  | Destination ", i))
                        WeaverOptimiser$display("  | Function: function(individual, json_data)")
                        WeaverOptimiser$display(paste0("  | ", paste(deparse(body(self$calculated_destinations[[i]][["function"]])), collapse = "\n  | ")))
                        WeaverOptimiser$display("  | Files:")
                        for(j in seq_along(self$calculated_destinations[[i]]$files)) {
                            WeaverOptimiser$display("  |  /")
                            WeaverOptimiser$display(paste0("  | | File ", j))
                            WeaverOptimiser$display(paste0("  | | File path: \"", self$calculated_destinations[[i]]$files[[j]]$file_path, "\""))
                            WeaverOptimiser$display(paste0("  | | JSON path: \"", self$calculated_destinations[[i]]$files[[j]]$json_path, "\""))
                            WeaverOptimiser$display("  |  \\")
                        }
                        WeaverOptimiser$display("   \\")
                    }
                }
            )
        ),

        # Subclass Boolean
        Boolean = R6Class("Boolean",
            inherit = WeaverOptimiserClass$Parameter,
            public = list(
                name = character(0),
                fixed_destinations = list(),
                calculated_destinations = list(),
                constraints = list(),

                initialize = function(name, fixed_destinations, calculated_destinations, constraints) {
                    self$name <- name

                    self$fixed_destinations <- fixed_destinations
                    self$calculated_destinations <- calculated_destinations

                    for(i in seq_along(self$fixed_destinations)) {
                        file_path <- strsplit(self$fixed_destinations[[i]]$file_path, "/")[[1]]
                        file_path <- do.call(file.path, as.list(file_path))

                        self$fixed_destinations[[i]]$file_path <- file_path
                    }

                    for(i in seq_along(self$calculated_destinations)) {
                        for(j in seq_along(self$calculated_destinations[[i]]$files)) {
                            file_path <- strsplit(self$calculated_destinations[[i]]$files[[j]]$file_path, "/")[[1]]
                            file_path <- do.call(file.path, as.list(file_path))

                            self$calculated_destinations[[i]]$files[[j]]$file_path <- file_path
                        }
                    }

                    self$constraints <- constraints
                },

                calculate_constraints_level = function(parameters) {
                    constraints_level = 0

                    for(constraint in self$constraints) {
                        constraints_level = max(constraints_level, parameters[[constraint$name]]$calculate_constraints_level(parameters) + 1)
                    }

                    return(constraints_level)
                },

                generate_random_value = function(parameters, individual) {
                    valuesAux = c(TRUE, FALSE)

                    for(constraint in self$constraints) {
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
                    return(!individual[[self$name]])
                },

                constraints_breached = function(individual) {
                    valuesAux = c(TRUE, FALSE)

                    for(constraint in self$constraints) {
                        if(constraint$checker(individual)) {
                            if(constraint$sameValueAsParameter) {
                                valuesAux = individual[[constraint$name]]
                            } else {
                                valuesAux = constraint$values
                            }
                        }
                    }

                    return(!individual[[self$name]] %in% valuesAux)
                },

                show = function() {
                    WeaverOptimiser$display(paste0("  Parameter Name: \"", self$name, "\""))
                    WeaverOptimiser$display(paste0("  Type: ", class(self)[1]))
                    WeaverOptimiser$display("  Fixed destinations:")
                    for(i in seq_along(self$fixed_destinations)) {
                        WeaverOptimiser$display("   /")
                        WeaverOptimiser$display(paste0("  | File ", i))
                        WeaverOptimiser$display(paste0("  | File path: \"", self$fixed_destinations[[i]]$file_path, "\""))
                        WeaverOptimiser$display(paste0("  | JSON path: \"", self$fixed_destinations[[i]]$json_path, "\""))
                        WeaverOptimiser$display("   \\")
                    }
                    WeaverOptimiser$display("  Calculated destinations:")
                    for(i in seq_along(self$calculated_destinations)) {
                        WeaverOptimiser$display("   /")
                        WeaverOptimiser$display(paste0("  | Destination ", i))
                        WeaverOptimiser$display("  | Function: function(individual, json_data)")
                        WeaverOptimiser$display(paste0("  | ", paste(deparse(body(self$calculated_destinations[[i]][["function"]])), collapse = "\n  | ")))
                        WeaverOptimiser$display("  | Files:")
                        for(j in seq_along(self$calculated_destinations[[i]]$files)) {
                            WeaverOptimiser$display("  |  /")
                            WeaverOptimiser$display(paste0("  | | File ", j))
                            WeaverOptimiser$display(paste0("  | | File path: \"", self$calculated_destinations[[i]]$files[[j]]$file_path, "\""))
                            WeaverOptimiser$display(paste0("  | | JSON path: \"", self$calculated_destinations[[i]]$files[[j]]$json_path, "\""))
                            WeaverOptimiser$display("  |  \\")
                        }
                        WeaverOptimiser$display("   \\")
                    }
                }
            )
        ),

        # Subclass Abundances
        Abundances = R6Class("Abundances",
            inherit = WeaverOptimiserClass$Parameter,
            public = list(
                name = character(0),
                densitiesVector = numeric(0),
                min = numeric(0),
                max = numeric(0),
                fixed_destinations = list(),
                constraints = list(),
                
                initialize = function(name, densitiesVector, min, max, fixed_destinations, constraints) {
                    self$name <- name

                    self$densitiesVector <- densitiesVector

                    self$min <- min
                    self$max <- max

                    self$fixed_destinations <- fixed_destinations

                    for(i in seq_along(self$fixed_destinations)) {
                        file_path <- strsplit(self$fixed_destinations[[i]]$file_path, "/")[[1]]
                        file_path <- do.call(file.path, as.list(file_path))

                        self$fixed_destinations[[i]]$file_path <- file_path
                    }

                    self$constraints <- constraints
                },

                calculate_constraints_level = function(parameters) {
                    constraints_level = 0

                    for(constraint in self$constraints) {
                        constraints_level = max(constraints_level, parameters[[constraint$name]]$calculate_constraints_level(parameters) + 1)
                    }

                    return(constraints_level)
                },
                
                generate_random_value = function(parameters, individual) {
                    minAux = self$min
                    maxAux = self$max

                    for(constraint in self$constraints) {
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
                    displacement <- sample(1:(self$max-self$min), 1)
                    current_pos <- (individual[[self$name]]-self$min+1)
                    new_pos <- (current_pos + displacement) %% (self$max-self$min+1)

                    return(as.integer(self$min + new_pos - 1))
                },

                constraints_breached = function(individual) {
                    minAux = self$min
                    maxAux = self$max

                    for(constraint in self$constraints) {
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

                    return(individual[[self$name]] < minAux || maxAux < individual[[self$name]])
                },

                show = function() {
                    WeaverOptimiser$display(paste0("  Parameter Name: \"", self$name, "\""))
                    WeaverOptimiser$display(paste0("  Type: ", class(self)[1]))
                    WeaverOptimiser$display(paste0("  Densities vector: [", toString(self$densitiesVector), "]"))
                    WeaverOptimiser$display(paste0("  Minimum: ", self$min))
                    WeaverOptimiser$display(paste0("  Maximum: ", self$max))
                    WeaverOptimiser$display("  Fixed destinations:")
                    for(i in seq_along(self$fixed_destinations)) {
                        WeaverOptimiser$display("   /")
                        WeaverOptimiser$display(paste0("  | File ", i))
                        WeaverOptimiser$display(paste0("  | File path: \"", self$fixed_destinations[[i]]$file_path, "\""))
                        WeaverOptimiser$display(paste0("  | JSON path: \"", self$fixed_destinations[[i]]$json_path, "\""))
                        WeaverOptimiser$display("   \\")
                    }
                }
            )
        ),

        Objective = R6Class("Objective",
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

        Fitness = R6Class("Fitness",
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
                },

                compare_objectives = function(a, b) {
                    stop("Abstract method 'compare_objectives' must be implemented.")  
                }
            )
        ),

        WeightedFitness = R6Class("WeightedFitness",
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
                    return(Reduce(function(a, b) if (self$compare_objectives(values[[a]], values[[b]])) a else b, seq_along(values)))
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
                    WeaverOptimiser$display(paste("Best fitness:", private$calculate_fitness(best_fitness)))
                    WeaverOptimiser$display(paste("Mean fitness:", private$calculate_fitness(mean_fitness)))
                },

                compare_objectives = function(a, b) {
                    return(private$calculate_fitness(a) < private$calculate_fitness(b))  
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

                    ggplot(fitness_data, aes(x = Generation)) +
                        geom_line(aes(y = BestFitness, color = "Best Individual"), linewidth = 1.2) +
                        geom_line(aes(y = MeanFitness, color = "Mean Population"), linewidth = 1.2, linetype = "dashed") +
                        labs(title = "Fitness Progression", 
                            x = "Generation", 
                            y = "Fitness Value") +
                        scale_color_manual(values = c("Best Individual" = "blue", "Mean Population" = "red")) +
                        theme_minimal()
                    
                    ggsave(file.path(output_path, "fitness_progress.svg"), plot = last_plot(), width = 8, height = 6)
                }
            )
        ),

        MultiObjectiveFitness = R6Class("MultiObjectiveFitness",
            inherit = WeaverOptimiserClass$Fitness,
            private = list(
                objectives = list(),
                order = character(0)
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
                    return(Reduce(function(a, b) if (self$compare_objectives(values[[a]], values[[b]])) a else b, seq_along(values)))
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
                    WeaverOptimiser$display("Best fitness:")
                    sapply(private$order, function(name) {
                        WeaverOptimiser$display(paste0(" - Parameter: ", name, " | Value: ", best_fitness[[name]]))
                    })

                    WeaverOptimiser$display("Mean fitness:")
                    sapply(private$order, function(name) {
                        WeaverOptimiser$display(paste0(" - Parameter: ", name, " | Value: ", mean_fitness[[name]]))
                    })
                },

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
                        objetive_data <- data.frame(
                            Generation = 1:max_generations,
                            BestValue = sapply(best_fitness, function(values) { values[[objetive]] }),
                            MeanValue = sapply(mean_fitness, function(values) { values[[objetive]] })
                        )

                        ggplot(objetive_data, aes(x = Generation)) +
                            geom_line(aes(y = BestValue, color = "Best Value"), linewidth = 1.2) +
                            geom_line(aes(y = MeanValue, color = "Mean Value"), linewidth = 1.2, linetype = "dashed") +
                            labs(title = paste0("Objective \"", objetive, "\" Value Progression"), 
                                x = "Generation", 
                                y = "Value") +
                            scale_color_manual(values = c("Best Value" = "blue", "Mean Value" = "red")) +
                            theme_minimal()
                        
                        ggsave(file.path(output_path, paste0(objetive, "_progress.svg")), plot = last_plot(), width = 8, height = 6)
                    }
                }
            )
        ),

        # Method to add a parameter
        add_parameters = function(parameters) {
            for(param in parameters) {
                if((param$name %in% names(private$parameters))) {
                    stop("Error: Parameter '", param$name, "' already added.\n")
                } 

                if(class(param)[1] == "Preference") {
                    private$optimise_preferences <- TRUE

                    private$preferences_groups[[param$name]] <- param
                }
                else {
                    private$parameters[[param$name]] <- param
                }

                private$combination_history[[param$name]] <- NA
            }
        },

        run = function(population_size, max_generations, fitness_class, program, output_path, base_config_path, keepBest, tournament_size, crossover_prob, mutation_rate, seed, num_cores, debug) {
            library(jsonlite)
            
            default_opts <- options()
            options(digits = 17)
            
            
            if(keepBest) {
                if(population_size <= tournament_size) {
                    stop("Error: Population size must be greater than the tournament selection size.")
                }
            } else {
                if(population_size < tournament_size) {
                    stop("Error: Population size must be greater or equal than the tournament selection size.")
                }
            }
            
            
            output_path <- file.path(output_path, paste0("optimisation_", format(Sys.time(), "%Y-%m-%d_%H-%M-%S")))

            dir.create(file.path(output_path, "configs"), recursive=TRUE)
            dir.create(file.path(output_path, "results"), recursive=TRUE)


            private$log_file = file.path(output_path, "log.txt")


            private$set_debug_mode(debug)
            
            
            
            if(.Platform$OS.type == "windows") {
                program <- gsub("\\", "/", program, fixed = TRUE)
                output_path <- gsub("\\", "/", output_path, fixed = TRUE)
                base_config_path <- gsub("\\", "/", base_config_path, fixed = TRUE)
            }
            
            # Set the seed for reproducibility
            if (!is.null(seed)) {
                set.seed(seed)
            }

            if (num_cores > 1) {
                if(availableCores() == num_cores) {
                    stop("Error: The number of cores selected is equal to the total available cores. Please leave at least one core free for system processes.")
                }

                if(.Platform$OS.type == "windows") {
                    plan(multisession, workers = num_cores)
                }
                else {
                    plan(multicore, workers = num_cores)
                }

                private$parallel = TRUE
                self$display(paste0("Parallel mode: ON | Cores: ", num_cores))
            }
            else {
                private$parallel = FALSE
                self$display("Parallel mode: OFF")
            }


            self$display("Genetic algorithm started")


            # Display the parameters added to the genetic algorithm
            self$display("── Parameters ──────────────────────────")
            self$display("")


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

            private$display_parameters()



            if(private$optimise_preferences) {
                csv_path = file.path(base_config_path, "species", "ontogeneticLinksPreference.csv")
                preference_data <- read.csv(csv_path, header = TRUE, row.names = 1, sep = ",", check.names = FALSE)

                num_groups <- length(private$preferences_groups)
                num_cols <- ncol(preference_data)

                equality_constraints <- matrix(0, nrow = num_cols, ncol = num_groups+1)

                inequality_constraints <- matrix(0, nrow = 2*num_groups, ncol = num_groups+1) ## >=

                colums <- colnames(preference_data)
                rows <- rownames(preference_data)

                for (j in 1:length(colums)) {

                    fixed_sum <- 0

                    for (i in 1:length(rows)) {

                        cell_is_grouped <- FALSE

                        for (g in 1:length(private$preferences_groups)) {
                            group <- names(private$preferences_groups)[[g]]

                            if (any(sapply(private$preferences_groups[[group]]$cells[[1]], function(cell) all(cell == c(rows[[i]], colums[[j]]))))) {
                                equality_constraints[j, g] <- equality_constraints[j, g] + 1  # esa variable afecta a esta suma de columna
                                cell_is_grouped <- TRUE
                                break
                            }
                        }

                        if (!cell_is_grouped) {
                            fixed_sum <- fixed_sum + preference_data[rows[[i]], colums[[j]]]
                        }
                    }

                    equality_constraints[[j, ncol(equality_constraints)]] <- max(0.99 - fixed_sum, 0.0)
                }


                for (g in 1:num_groups) {
                    group <- names(private$preferences_groups)[[g]]

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
            }



            self$display("")
            self$display("── Genetic Algorithm ───────────────────")
            self$display("")


            header_names <- c("Generation", "Individual", names(private$parameters), names(private$preferences_groups), fitness_class$get_header(), "Result_folder")

            private$evolution_history <- data.frame(matrix(ncol = length(header_names), nrow = 0), stringsAsFactors = FALSE)
            colnames(private$evolution_history) <- header_names


            best_fitness <- c()
            mean_fitness <- c()

            
            population <- private$initialize_population(population_size)

            for (generation in 1:max_generations) {
                self$display("------------------------------------------------")
                self$display(paste0("## Generation ", generation, " of ", max_generations," ##"))

                result <- private$evaluate_population(population, fitness_class, program, output_path, base_config_path)

                fitness_values <- result$fitness_values
                result_folders <- result$result_folders

                for (i in 1:length(population)) {
                    new_row <- unlist(list(
                        generation, i, 
                        unname(population[[i]][names(private$parameters)]), 
                        unname(population[[i]][names(private$preferences_groups)]), 
                        fitness_class$get_row_info(fitness_values[[i]]),
                        result_folders[[i]]
                    ))

                    new_row_df <- as.data.frame(t(new_row), stringsAsFactors = FALSE)
                    colnames(new_row_df) <- colnames(private$evolution_history)

                    private$evolution_history <- rbind(private$evolution_history, new_row_df)
                }

                write.table(private$evolution_history, file.path(output_path, "evolution_history.txt"), sep = "\t", row.names = FALSE, col.names = TRUE)

                best_fitness <- c(best_fitness, list(fitness_values[[fitness_class$get_best_index(fitness_values)]]))

                mean_fitness <- c(mean_fitness, list(fitness_class$calculate_mean_value(fitness_values)))
                
                fitness_class$display_generation_summary(best_fitness[[length(best_fitness)]], mean_fitness[[length(mean_fitness)]])

                if(generation < max_generations) {
                    population <- private$next_generation(population, fitness_class, fitness_values, keepBest, tournament_size, crossover_prob, mutation_rate)
                }

                self$display("------------------------------------------------")

                # Liberar memoria explícitamente
                gc()
            }


            best_candidate <- fitness_class$get_best_index(fitness_values)

            self$display(paste("Best Solution:", result_folders[[best_candidate]]))

            self$display("------------------------------------------------")
            
            self$display("Genetic algorithm completed")


            dir.create(file.path(output_path, "fitness_graphs"))
            fitness_class$save_progress_graph(file.path(output_path, "fitness_graphs"), max_generations, best_fitness, mean_fitness)
        
            options(default_opts)
        }
    )
)

WeaverOptimiser <- WeaverOptimiserClass$new()

