
# Import optimisation tools
library(weaverOptimization)


# Set Weaver folder
setWeaver("/home/macarse/Escritorio/EEZA/Weaver")


# Evaluation function
# Calculates an individual's fitness.
# 
# @param optimizationResult Dictionary with the results of the individual's run.
# @return Individual's fitness.
evaluationFunction <- function(optimizationResult = "list") {
    selectedAnimalSpecies <- optimizationResult[["animalSpecies"]][["Consumer_fast_asexual"]]

    minX <- selectedAnimalSpecies[["amplitude"]][["min"]]
    maxX <- selectedAnimalSpecies[["amplitude"]][["max"]]

    amplitude <- (maxX-minX)/(maxX+minX)

    fitness <- amplitude
    
    return(fitness)
}


### Definition of operators

# Selection operator
# Selects the parents of the next population using the roulette method.
# 
# @param population Current population.
# @param maximiseFitness Indicates whether the objective is to maximise fitness.
# @return List of selected parents.
selectionOperator <- function(population, maximiseFitness) {
    # Define the number of parents to be selected.
    # In this case it is equal to the population size.
    numberOfParents <<- length(population[["individuals"]])

    # The fitness of each individual in the population is obtained.
    populationFitness <- sapply(population[["individuals"]], getFitness)
  
    # The probability of being selected is calculated for each individual.
    probabilities <- populationFitness / sum(populationFitness)

    # If the objective is to minimise the fitness, the complement of the probabilities is calculated.
    if(!maximiseFitness) {
        probabilities <- 1 - probabilities
    }
  
    # Select parents using the roulette method.
    parents <- sample(population[["individuals"]], size = numberOfParents, prob = probabilities, replace = TRUE)
  
    # Randomly sort parents
    parents <- sample(parents)

    return(parents)
}

# Crossover operator
# Crosses pairs of parents via chiasmus.
# 
# @param parents Pair of parents.
# @return Pair of children.
crossoverOperator <- function(parents) {
    firstParent <- parents[[1]]
    secondParent <- parents[[2]]

    # Define the number of chiasms
    numberOfChiasms <- 1

    # Random selection of chiasms
    chiasmsPoints <- sample(1:(length(names(firstParent[["parametersValue"]])) - 1), numberOfChiasms, replace=FALSE)
    chiasmsPoints <- sort(chiasmsPoints)

    # Sequential crossing of chiasms
    firstChildValue <- c()
    secondChildValue <- c()

    previousPoint <- 1
    for(pointIndex in 1:length(chiasmsPoints)) {
        if(pointIndex %% 2 == 0) {
            firstChildValue <- c(firstChildValue, secondParent[["parametersValue"]][previousPoint:chiasmsPoints[[pointIndex]]])
            secondChildValue <- c(secondChildValue, firstParent[["parametersValue"]][previousPoint:chiasmsPoints[[pointIndex]]])
        }
        else {
            firstChildValue <- c(firstChildValue, firstParent[["parametersValue"]][previousPoint:chiasmsPoints[[pointIndex]]])
            secondChildValue <- c(secondChildValue, secondParent[["parametersValue"]][previousPoint:chiasmsPoints[[pointIndex]]])
        }

        previousPoint <- chiasmsPoints[[pointIndex]] + 1
    }

    if(length(chiasmsPoints) %% 2 == 0) {
        firstChildValue <- c(firstChildValue, firstParent[["parametersValue"]][previousPoint:length(firstParent[["parametersValue"]])])
        secondChildValue <- c(secondChildValue, secondParent[["parametersValue"]][previousPoint:length(secondParent[["parametersValue"]])])
    }
    else {
        firstChildValue <- c(firstChildValue, secondParent[["parametersValue"]][previousPoint:length(secondParent[["parametersValue"]])])
        secondChildValue <- c(secondChildValue, firstParent[["parametersValue"]][previousPoint:length(firstParent[["parametersValue"]])])
    }

    # Creation of new individuals
    firstChild <- Individual(firstChildValue)
    secondChild <- Individual(secondChildValue)

    return(list(firstChild, secondChild))
}

# Mutation operator
# Mutation of one of an individual's parameters.
# 
# @param parameter Parameter to mutate.
# @return New parameter.
mutationOperator <- function(parameter) {
    # Define mutation probability
    mutationProbability <- 0.3

    # Decide whether or not to apply mutation
    if(runif(1) < mutationProbability) {
        newValue <- getRandomValue(parameter)
    }
    else {
        newValue <- parameter
    }

    return(newValue)
}

# Replacement operator
# Replacement by elitism of the current population by the new population.
# 
# @param oldPopulation Current population.
# @param descendants List of new individuals.
# @return New population.
replacementOperator <- function(oldPopulation, descendants) {
    # Define percentage of elitism.
    elitismPercentage <- 0.2

    # Define the size of the new population.
    populationSize <- length(oldPopulation)

    # The fitness of each individual in the current population is obtained.
    oldPopulationFitness <- sapply(oldPopulation, getFitness)

    # Order individuals in descending order of their fitness.
    orderedOldPopulation <- order(oldPopulationFitness, decreasing = TRUE)

    # Calculate the point at which elitism ends
    elitismPoint <- round(populationSize * elitismPercentage)

    # New population is created by a combination of elitism and random selection of descendants.
    newPopulation <- c(oldPopulation[orderedOldPopulation[1:elitismPoint]], sample(descendants, populationSize - elitismPoint, replace = FALSE))

    return(newPopulation)
}


# Definition of the parameters to optimise
initialSearchSpace <- SearchSpace(c(
        ParameterFromFixedValue(
            name="simulation_params.json:simulation/runDays", 
            value=c(5, 6, 7),
            valueRange=c(1, 10)
        ),
        ParameterFromFixedValue(
            name="simulation_params.json:simulation/encountersMatrixFilename", 
            value=c("prueba1", "prueba2"),
            valueRange=c("prueba1", "prueba2", "prueba3")
        ),
        ParameterFromFixedValue(
            name="species/Consumer_fast_asexual.json:ontogeneticLinks/resourceLinks/item_0/links/item_0/profitability", 
            value=c(0.7, 0.8),
            valueRange=c(0.1, 0.9)
        )
    )
)


# Execution of the optimisation process
runWeaverOptimization(
    baseConfigPath = "/home/macarse/Escritorio/EEZA/Weaver/config/base_arthropods_temp",
    outputFolder = "/home/macarse/Escritorio/EEZA/Weaver/pruebaOpti", numCores = 2, 
    initialSearchSpace = initialSearchSpace, 
    maxNumberOfGenerations = 2, fitnessObjective = 0.0005, maximiseFitness = FALSE, burn_in = 1, 
    evaluationFunction = evaluationFunction, 
    selectionOperator = selectionOperator, crossoverOperator = crossoverOperator, 
    mutationOperator = mutationOperator, replacementOperator = replacementOperator
)
