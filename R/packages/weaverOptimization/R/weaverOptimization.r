
###################
## Integer range class

IntegerRange <- function(values) {
    if(class(values) != "numeric") {
        stop("Error: The type of the vector must be 'numeric'.")
    }
    
    if(length(values) > 2) {
        stop("Error: A range of integers is defined as c(min, max).")
    }

    lowerValue <- as.integer(values[[1]])
    upperValue <- as.integer(values[[2]])

    return(list("lowerValue" = lowerValue, "upperValue" = upperValue))
}



###################
## Integer class

Integer <- function(value, valueRange) {
    return(list(list("type" = "Integer", "value" = value, "valueRange" = valueRange)))
}



###################
## Character range class

CharacterRange <- function(values) {
    if(class(values) != "character") {
        stop("Error: The type of the vector must be 'character'.")
    }

    return(list("enumValues" = values))
}



###################
## Character class

Character <- function(value, valueRange) {
    return(list(list("type" = "Character", "value" = value, "valueRange" = valueRange)))
}



###################
## Double range class

DoubleRange <- function(values) {
    if(class(values) != "numeric") {
        stop("Error: The type of the vector must be 'numeric'.")
    }

    if(length(values) > 2) {
        stop("Error: A range of doubles is defined as c(min, max).")
    }

    lowerValue <- values[[1]]
    upperValue <- values[[2]]

    return(list("lowerValue" = lowerValue, "upperValue" = upperValue))
}



###################
## Double class

Double <- function(value, valueRange) {
    return(list(list("type" = "Double", "value" = value, "valueRange" = valueRange)))
}



###################
## Logical range class

setClass("LogicalRangeClass", slots = list(enumValues = "logical"))

## Constructors

LogicalRange <- function(values) {
    if(class(values) != "logical") {
        stop("Error: The type of the vector must be 'logical'.")
    }

    return(list("enumValues" = values))
}



###################
## Logical class

Logical <- function(value = "logical", valueRange = "LogicalRangeClass") {
    return(list(list("type" = "Logical", "value" = value, "valueRange" = valueRange)))
}



###################
## To String functions

toString <- function(obj) {
    if(obj[["type"]] == "Integer") {
        string <- as.integer(obj[["value"]])
    }
    else if(obj[["type"]] == "Logical") {
        if(obj[["value"]] == TRUE) {
            string <- "True"
        }
        else {
            string <- "False"
        }
    }
    else if(obj[["type"]] == "Double") {
        string <- format(obj[["value"]], nsmall = 1)
    }
    else if(obj[["type"]] == "Character") {
        string <- paste("'", obj[["value"]], "'", sep="")
    }

    return(string)
}

toStringIndividual <- function(individual) {
    keysList <- names(individual[["parametersValue"]])

    string <- paste("\"{'", keysList[[1]], "': ", toString(individual[["parametersValue"]][[keysList[[1]]]][[1]]), sep="")

    if(length(keysList) > 1) {
        for(index in 2:length(keysList)) {
            elem <- paste("'", keysList[[index]], "': ", toString(individual[["parametersValue"]][[keysList[[index]]]][[1]]), sep="")
            string <- paste(string, elem, sep=", ")
        }
    }

    string <- paste(string, "}\"", sep="")

    return(string)
}



###################
## Random value generators

getIntegerRandomValue <- function(valueRange, numberOfValues, valuesToAvoid) {
    valueList <- c()
    
    while(length(valueList) < numberOfValues) {
        valueList <- sample(valueRange[["lowerValue"]]:valueRange[["upperValue"]], numberOfValues-length(valueList), replace = FALSE)
    
        valueList <- setdiff(valueList, valuesToAvoid)
    }
    
    integerList <- c()

    for(value in valueList) {
        integerList <- c(integerList, Integer(value, valueRange))
    }

    return(integerList)
}

getLogicalRandomValue <- function(valueRange, numberOfValues, valuesToAvoid) {
    diffEnumValues <- setdiff(valueRange[["enumValues"]], valuesToAvoid)
    
    if(numberOfValues > length(diffEnumValues)) {
        stop(paste("Error: The number of random values of the item '", name, "' is greater than the size of the range.", sep=""))
    }

    valueList <- sample(diffEnumValues, numberOfValues, replace = FALSE)

    logicalList <- c()

    for(value in valueList) {
        logicalList <- c(logicalList, Logical(value, valueRange))
    }

    return(logicalList)
}

getDoubleRandomValue <- function(valueRange, numberOfValues, valuesToAvoid) {
    valueList <- c(runif(1, min = valueRange[["lowerValue"]], max = valueRange[["upperValue"]]))
    while(length(valueList) < numberOfValues) {
        newValue <- runif(1, min = valueRange[["lowerValue"]], max = valueRange[["upperValue"]])

        if(is.na(match(newValue, valuesToAvoid))) {
            valueList <- unique(c(valueList, newValue))
        }
    }

    doubleList <- c()

    for(value in valueList) {
        doubleList <- c(doubleList, Double(value, valueRange))
    }

    return(doubleList)
}

getCharacterRandomValue <- function(valueRange, numberOfValues, valuesToAvoid) {
    diffEnumValues <- setdiff(valueRange[["enumValues"]], valuesToAvoid)
    
    if(numberOfValues > length(diffEnumValues)) {
        stop(paste("Error: The number of random values of the item '", name, "' is greater than the size of the range.", sep=""))
    }

    valueList <- sample(diffEnumValues, numberOfValues, replace = FALSE)

    characterList <- c()

    for(value in valueList) {
        characterList <- c(characterList, Character(value, valueRange))
    }

    return(characterList)
}

getRandomValue <- function(value, numberOfValues = 1, valuesToAvoid = c()) {

    if(value[[1]][["type"]] == "Integer") {
        return(getIntegerRandomValue(value[[1]][["valueRange"]], numberOfValues, valuesToAvoid))
    }
    else if(value[[1]][["type"]] == "Logical") {
        return(getLogicalRandomValue(value[[1]][["valueRange"]], numberOfValues, valuesToAvoid))
    }
    else if(value[[1]][["type"]] == "Double") {
        return(getDoubleRandomValue(value[[1]][["valueRange"]], numberOfValues, valuesToAvoid))
    }
    else if(value[[1]][["type"]] == "Character") {
        return(getCharacterRandomValue(value[[1]][["valueRange"]], numberOfValues, valuesToAvoid))
    }
}



###################
## Individual class

## Constructors

Individual <- function(parametersValue = list()) {
    return(list("parametersValue" = parametersValue, "fitness" = 0.0, "configPath" = "", "resultPath" = "", "resultInfo" = list()))
}

#### Getters

getFitness <- function(obj) {
    return(obj[["fitness"]])
}

#### Others

getModificationsInfo <- function(individual) {
    modificationsInfo <- list()

    for(name in names(individual[["parametersValue"]])) {
        modificationsInfo[[name]] <- individual[["parametersValue"]][[name]][[1]][["value"]]
    }

    return(modificationsInfo)
}

printInfo <- function(individual) {
    cat(paste(tab, tab, "- Fitness: ", individual[["fitness"]], "\n", sep=""))
    cat(paste(tab, tab, "- Config path: ", individual[["configPath"]], "\n", sep=""))

    modiLines <- strsplit(toJSON(getModificationsInfo(individual), pretty = TRUE), "\n")[[1]]
    modiLines <- c(modiLines[[1]], paste0(paste(tab, tab, sep=""), modiLines[2:length(modiLines)]))

    cat(paste(tab, tab, "- Modifications: ", paste(modiLines, collapse = "\n"), "\n", sep=""))

    cat(paste(tab, tab, "- Result path: ", individual[["resultPath"]], "\n", sep=""))

    infoLines <- strsplit(toJSON(individual[["resultInfo"]], pretty = TRUE), "\n")[[1]]
    infoLines <- c(infoLines[[1]], paste0(paste(tab, tab, sep=""), infoLines[2:length(infoLines)]))

    cat(paste(tab, tab, "- Result info: ", paste(infoLines, collapse = "\n"), "\n", sep=""))
}



###################
## Optimization parameter class

setClass("ParameterClass", slots = list(name = "character", type = "character", value = "list"))

## Constructors

ParameterFromFixedValue <- function(name = "character", valueList = "vector", valueRange = "vector") {
    type <- obtainParameterType(name)

    if(type == "string") {
        typeValueRange <- CharacterRange(valueRange)

        castValueList <- list()

        for(value in valueList) {
            if(is.na(match(value, typeValueRange[["enumValues"]]))) {
                stop(paste("Error: Parameter '", name,"' has an out-of-range value.", sep=""))
            }

            castValueList <- c(castValueList, Character(value, typeValueRange))
        }
    }
    else if (type == "boolean") {
        typeValueRange <- LogicalRange(valueRange)

        castValueList <- list()

        for(value in valueList) {
            if(is.na(match(value, typeValueRange[["enumValues"]]))) {
                stop(paste("Error: Parameter '", name,"' has an out-of-range value.", sep=""))
            }

            castValueList <- c(castValueList, Logical(value, typeValueRange))
        }
    }
    else if (type == "floater") {
        typeValueRange <- DoubleRange(valueRange)

        castValueList <- list()

        for(value in valueList) {
            if(value < typeValueRange[["lowerValue"]] || typeValueRange[["upperValue"]] < value) {
                stop(paste("Error: Parameter '", name,"' has an out-of-range value.", sep=""))
            }

            castValueList <- c(castValueList, Double(value, typeValueRange))
        }
    }
    else if (type == "unsigned_integer" || type == "integer") {
        typeValueRange <- IntegerRange(valueRange)

        castValueList <- list()

        for(value in valueList) {
            if(value < typeValueRange[["lowerValue"]] || typeValueRange[["upperValue"]] < value) {
                stop(paste("Error: Parameter '", name,"' has an out-of-range value.", sep=""))
            }

            castValueList <- c(castValueList, Integer(as.integer(value), typeValueRange))
        }
    }

    return(new("ParameterClass", name=name, type=type, value=castValueList))
}

ParameterFromRandomValue <- function(name = "character", numberOfValues = "numeric", valueRange = "vector") {
    type <- obtainParameterType(name)
    
    if(type == "string") {
        typeValueRange <- CharacterRange(valueRange)
    }
    else if (type == "boolean") {
        typeValueRange <- LogicalRange(valueRange)
    }
    else if (type == "floater") {
        typeValueRange <- DoubleRange(valueRange)
    }
    else if (type == "unsigned_integer" || type == "integer") {
        typeValueRange <- IntegerRange(valueRange)
    }

    value <- getRandomValue(typeValueRange, numberOfValues)

    return(new("ParameterClass", name=name, type=type, value=value))
}

#### Getters

getParameterName <- function(obj = "ParameterClass") {
    return(obj@name)
}

##

getValue <- function(obj = "ParameterClass") {
    return(obj@value)
}

#### Others

obtainParameterType <- function(parameterName = "character") {
    library(jsonlite)
    
    result <- strsplit(parameterName, ":")[[1]]

    json_file_path <- result[[1]]
    item_path_str <- result[[2]]

    result <- strsplit(json_file_path, "/")[[1]]

    if(length(result) > 1) {
        folder_path <- result[1:(length(result) - 1)]
    }
    else {
        folder_path <- list()
    }
    
    json_file <- result[[length(result)]]

    item_path <- strsplit(item_path_str, "/")[[1]]



    if(length(folder_path) == 0) {
        if(json_file == "simulation_params.json") {
            schema_file <- "simulation_params.schema.json"
        }
        else if (json_file == "world_params.json") {
            schema_file <- "world_params.schema.json"
        }
    }
    else {
        folder_path_str <- paste(folder_path, collapse = "/")

        if(folder_path_str == "moisture") {
            schema_file <- "moisture_patch.schema.json"
        }
        else if (folder_path_str == "obstacle") {
            schema_file <- "obstacle_patch.schema.json"
        }
        else if (folder_path_str == "resource") {
            schema_file <- "resource.schema.json"
        }
        else if (folder_path_str == "resource/patches") {
            schema_file <- "resource_patch.schema.json"
        }
        else if (folder_path_str == "species") {
            schema_file <- "species.schema.json"
        }
    }


    schema_content <- fromJSON(file.path(WeaverRootPath, "schema", schema_file))

    schema_item_content <- schema_content
    for(elem in item_path) {
        if(grepl("item_\\d+", elem)) {
            schema_item_content <- schema_item_content[["items"]]
        } 
        else {
            schema_item_content <- schema_item_content$properties[[elem]]
        }
    }

    return(schema_item_content$type)
}



###################
## Search space class

setClass("SearchSpaceClass", slots = list(parameters = "vector"))

## Constructors

SearchSpace <- function(parameters = "vector") {
    if(!exists("WeaverRootPath")) {
        stop("Error: The Weaver root folder must be selected using the 'setWeaver' function.")
    }

    return(new("SearchSpaceClass", parameters=parameters))
}

#### Getters

getCombinations <- function(obj = "SearchSpaceClass") {
    values <- list()
        
    for(param in obj@parameters) {
        paramName <- getParameterName(param)
        values[[paramName]] <- getValue(param)
    }

    expandedValues <- expand.grid(values)

    combinations <- vector("list", length = nrow(expandedValues))

    for(index in 1:nrow(expandedValues)) {
        combinations[[index]] <- as.list(expandedValues[index,])
    }

    return(combinations)
}



###################
## Population class

## Constructors

PopulationFromSearchSpace <- function(searchSpace = "SearchSpaceClass", generationNumber = "integer") {

    # Population name
    name <- paste("population_", generationNumber, sep="")

    # Generate the population's individual
    combinations <- getCombinations(searchSpace)

    individuals <- c()

    for(combi in combinations) {
        individuals <- c(individuals, list(Individual(combi)))
    }

    return(list("individuals" = individuals, "name" = name))
}

PopulationFromList <- function(individuals = "vector", generationNumber = "integer") {

    # Population name
    name <- paste("population_", generationNumber, sep="")

    return(list("individuals" = individuals, "name" = name))
}

#### Others

updateLog <- function(population, logFilePath) {
    library(jsonlite)

    cat(paste(tab, "- Updating log ... ", sep=""))

    newLogContent <- list()

    logContent <- fromJSON(logFilePath)

    for(index in seq_along(population[["individuals"]])) {
        individualName <- paste("individual_", index, sep="")

        newLogContent[[population[["name"]]]][[individualName]] <- list()

        newLogContent[[population[["name"]]]][[individualName]][["config"]] <- population[["individuals"]][[index]][["configPath"]]

        modifications <- list()

        for(name in names(population[["individuals"]][[index]][["parametersValue"]])) {
            modifications[[name]] <- population[["individuals"]][[index]][["parametersValue"]][[name]][[1]][["value"]]
        }

        newLogContent[[population[["name"]]]][[individualName]][["modifications"]] <- modifications

        newLogContent[[population[["name"]]]][[individualName]][["resultPath"]] <- population[["individuals"]][[index]][["resultPath"]]
        newLogContent[[population[["name"]]]][[individualName]][["resultInfo"]] <- population[["individuals"]][[index]][["resultInfo"]]
        
        newLogContent[[population[["name"]]]][[individualName]][["fitness"]] <- population[["individuals"]][[index]][["fitness"]]
    }

    logContent <- c(logContent, newLogContent)
    
    writeLines(toJSON(logContent, pretty = TRUE), logFilePath)

    cat("[DONE]\n")
}

evaluate <- function(population = "list", numCores = "integer", baseConfigPath = "character", 
                     generationOutputFolder = "character", logFilePath = "character", evaluationFunction = "function", 
                     burn_in = "integer", bestIndividual = "IndividualClass") {
    
    library(jsonlite)

    cat(paste(tab, "- Evaluating population ... ", sep=""))

    populationConfigsPath <- file.path(generationOutputFolder, "configs")

    for(index in seq_along(population)) {
        pythonScript <- file.path(WeaverRootPath, 'scripts/configs/createConfig.py')

        command<-paste('python3', pythonScript, baseConfigPath, toStringIndividual(population[[index]]), populationConfigsPath, index)
        system(command,wait=TRUE)
    }


    configPathsVector <- list.dirs(path=populationConfigsPath, full.names=TRUE, recursive=FALSE)


    cl <- makeCluster(numCores)

    resultFolder <- file.path(generationOutputFolder, "results")
    dir.create(resultFolder)

    clusterExport(cl=cl, varlist=c("WeaverRootPath", "resultFolder", "burn_in"), envir=environment())

    r <- parLapplyLB(cl, configPathsVector, function(config) {
            if(Sys.info()["sysname"] == "Windows") {
                executable <- file.path(WeaverRootPath, 'Weaver.exe')
            }
            else {
                executable <- file.path(WeaverRootPath, 'Weaver.out')
            }

            command<-paste(executable, '--silent', '-I', config, '-O', resultFolder, '--optimization-burn-in', burn_in)
            system(command, wait=TRUE)
        }
    )

    stopCluster(cl)


    resultPathsVector <- list.dirs(path=resultFolder, full.names=TRUE, recursive=FALSE)



    resultsVector <- c()

    for(resultPath in resultPathsVector) {
        optimizationResultFile <- file.path(resultPath, "optimization_result.json")

        optimizationResultContent <- fromJSON(optimizationResultFile)

        resultsVector <- c(resultsVector, list(optimizationResultContent))
    }


    for(index in seq_along(population)) {
        population[[index]][["configPath"]] <- configPathsVector[[index]]

        population[[index]][["resultPath"]] <- resultPathsVector[[index]]
        population[[index]][["resultInfo"]] <- resultsVector[[index]]

        individualFitness <- evaluationFunction(resultsVector[[index]])

        population[[index]][["fitness"]] <- individualFitness

        if(individualFitness > getFitness(bestIndividual)) {
            bestIndividual <- population[index][[1]]
        }
    }

    cat("[DONE]\n")

    return(list(bestIndividual, population))
}



################################
## Function to set Weaver folder

setWeaver <- function(rootPath) {
    if(class(rootPath) != "character") {
        stop("Error: The type of the vector must be 'character'.")
    }

    WeaverRootPath <<- rootPath
}



################################
## General functions

createGenerationFolder <- function(outputFolder, generationNumber) {
    cat(paste(tab, "- Creating generation folder ... ", sep=""))

    generationOutputFolder <- file.path(outputFolder, paste("generation_", generationNumber, sep=""))
    dir.create(generationOutputFolder, recursive = TRUE)

    cat("[DONE]\n")

    return(generationOutputFolder)
}

printBestIndividual <- function(bestIndividual) {
    cat("\n")

    cat(paste(tab, "- Best individual:\n", sep=""))

    printInfo(bestIndividual)

    cat("\n")
}

achieveObjectiveFitness <- function(fitness, fitnessObjective, maximiseFitness) {
    if(maximiseFitness) {
        return(fitness > fitnessObjective)
    }
    else {
        return(fitness < fitnessObjective)
    }
}

saveBestIndividual <- function(outputFolder, logFilePath, bestIndividual) {
    cat(paste(tab, "- Saving best individual ... ", sep=""))

    logContent <- fromJSON(logFilePath)

    newLogContent <- list()

    newLogContent[["bestIndividual"]] <- list()

    newLogContent[["bestIndividual"]][["config"]] <- bestIndividual[["configPath"]]

    modifications <- list()

    for(name in names(bestIndividual[["parametersValue"]])) {
        modifications[[name]] <- bestIndividual[["parametersValue"]][[name]][[1]][["value"]]
    }

    newLogContent[["bestIndividual"]][["modifications"]] <- modifications

    newLogContent[["bestIndividual"]][["resultPath"]] <- bestIndividual[["resultPath"]]
    newLogContent[["bestIndividual"]][["resultInfo"]] <- bestIndividual[["resultInfo"]]
    
    newLogContent[["bestIndividual"]][["fitness"]] <- bestIndividual[["fitness"]]

    logContent <- c(logContent, newLogContent)
    
    writeLines(toJSON(logContent, pretty = TRUE), logFilePath)


    dir.create(file.path(outputFolder, "best_individual", "config"), recursive = TRUE)
    for(file in list.files(bestIndividual[["configPath"]], full.names = TRUE)) {
        file.copy(from = file, to = file.path(outputFolder, "best_individual", "config"), recursive = TRUE)
    }
    

    dir.create(file.path(outputFolder, "best_individual", "result"), recursive = TRUE)
    for(file in list.files(bestIndividual[["resultPath"]], full.names = TRUE)) {
        file.copy(from = file, to = file.path(outputFolder, "best_individual", "result"), recursive = TRUE)
    }

    cat("[DONE]\n")
}



################################
## Operators

doSelectionOperator <- function(selectionOperator, population, maximiseFitness) {
    cat(paste(tab, "- Selecting parents ... ", sep=""))

    parents <- selectionOperator(population, maximiseFitness)

    cat("[DONE]\n")

    return(parents)
}

doCrossoverOperator <- function(crossoverOperator, parents) {
    cat(paste(tab, "- Crossing parents ... ", sep=""))

    pairVector <- split(parents, rep(1:(length(parents)/2), each = 2))

    cl <- makeCluster(detectCores())

    clusterExport(cl=cl, varlist=c("Individual"))

    childrens <- parLapplyLB(cl, pairVector, crossoverOperator)

    stopCluster(cl)

    descendants <- unlist(childrens, recursive = FALSE)

    cat("[DONE]\n")

    return(descendants)
}

doMutationOperator <- function(mutationOperator, descendants) {
    cat(paste(tab, "- Mutating descendants ... ", sep=""))

    cl <- makeCluster(detectCores())

    clusterExport(cl=cl, varlist=c("getRandomValue", "getIntegerRandomValue", "Integer", "getLogicalRandomValue",
        "Logical", "getDoubleRandomValue", "Double", "getCharacterRandomValue", "Character", "mutationOperator"), envir=environment()
    )

    mutationIndividual <- function(individual) {

        result <- lapply(individual[["parametersValue"]], mutationOperator)

        namesVector <- names(individual[["parametersValue"]])
        individual[["parametersValue"]] <- result
        names(individual[["parametersValue"]]) <- namesVector

        return(individual)
    }

    mutatedDescendants <- parLapplyLB(cl, descendants, mutationIndividual)

    stopCluster(cl)

    cat("[DONE]\n")

    return(mutatedDescendants)
}

doReplacementOperator <- function(replacementOperator, population, descendants, newGenerationNumber) {
    cat(paste(tab, "- Replacing population ... ", sep=""))

    newPopulationIndividuals <- replacementOperator(population, descendants)

    newPopulation <- PopulationFromList(newPopulationIndividuals, newGenerationNumber)

    cat("[DONE]\n")

    return(newPopulation)
}



###################
## Main function

runWeaverOptimization <- function(baseConfigPath, outputFolder, numCores, initialSearchSpace, maxNumberOfGenerations, 
        fitnessObjective, maximiseFitness, burn_in, evaluationFunction, selectionOperator, crossoverOperator, 
        mutationOperator, replacementOperator) {
    
    
    library(parallel)

    tab <<- "    "

    set.seed(1)

    
    if(dir.exists(outputFolder)) {
        stop("Error: The output directory already exists.")
    }
    else {
        dir.create(outputFolder, recursive = TRUE)
    }


    cat("##############################\n")
    cat("\n")

    cat(paste(tab, "- Initialising log ... ", sep=""))

    logFilePath <- file.path(outputFolder, "optimization_log.json")
    file.create(logFilePath)
    writeLines("{}", logFilePath)

    cat("[DONE]\n")

    cat("\n")


    bestIndividual <- Individual()


    currentGeneration <- 1

    cat("######################\n")
    cat(paste("## Generation ", currentGeneration, "/", maxNumberOfGenerations, "\n", sep=""))

    cat("\n")

    generationOutputFolder <- createGenerationFolder(outputFolder, currentGeneration)

    cat(paste(tab, "- Initialising first population ... ", sep=""))

    currentPopulation <- PopulationFromSearchSpace(initialSearchSpace, currentGeneration)

    cat("[DONE]\n")

    result <- evaluate(currentPopulation[["individuals"]], numCores, baseConfigPath, generationOutputFolder, logFilePath, evaluationFunction, burn_in, bestIndividual)

    bestIndividual <- result[[1]]
    currentPopulation[["individuals"]] <- result[[2]]

    updateLog(currentPopulation, logFilePath)

    printBestIndividual(bestIndividual)

    cat("######################\n")

    cat("\n")

    currentGeneration <- currentGeneration + 1

    while(currentGeneration <= maxNumberOfGenerations && !achieveObjectiveFitness(getFitness(bestIndividual), fitnessObjective, maximiseFitness)) {
        cat("######################\n")
        cat(paste("## Generation ", currentGeneration, "/", maxNumberOfGenerations, "\n", sep=""))

        cat("\n")

        parents <- doSelectionOperator(selectionOperator, currentPopulation, maximiseFitness)

        descendants <- doCrossoverOperator(crossoverOperator, parents)

        descendants <- doMutationOperator(mutationOperator, descendants)

        generationOutputFolder <- createGenerationFolder(outputFolder, currentGeneration)

        result <- evaluate(descendants, numCores, baseConfigPath, generationOutputFolder, logFilePath, evaluationFunction, burn_in, bestIndividual)

        bestIndividual <- result[[1]]
        descendants <- result[[2]]

        currentPopulation <- doReplacementOperator(replacementOperator, currentPopulation[["individuals"]], descendants, currentGeneration)

        updateLog(currentPopulation, logFilePath)

        printBestIndividual(bestIndividual)

        currentGeneration <- currentGeneration + 1

        cat("######################\n")

        cat("\n")
    }

    
    saveBestIndividual(outputFolder, logFilePath, bestIndividual)
    
    cat(paste(tab, "- Closing log ... [DONE]\n", sep=""))

    cat("\n")

    cat("## Finished optimization ##\n")

    cat("\n")

    cat("##############################\n")
}
