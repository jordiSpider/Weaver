
resetOptimiser <- function() {
    baseConfigPath <<- NULL
    outputFolder <<- NULL
    numberOfCores <<- NULL
    parameters <<- NULL
    maxNumberOfGenerations <<- NULL
    evaluationFunctionSelected <<- NULL
    selectionOperatorSelected <<- NULL
    crossoverOperatorSelected <<- NULL
    mutationOperatorSelected <<- NULL
    replacementOperatorSelected <<- NULL
    fitnessObjective <<- NULL
    maximiseFitness <<- NULL
    burn_in <<- NULL
}

.onLoad <- function(...) {
    resetOptimiser()
}

setBaseConfigPath <- function(newBaseConfigPath) {
    if(nchar(newBaseConfigPath) == 0) {
        stop("Error: 'baseConfigPath' cannot be an empty string.")
    }

    baseConfigPath <<- newBaseConfigPath
}

setOutputFolder <- function(newOutputFolder) {
    if(nchar(newOutputFolder) == 0) {
        stop("Error: 'outputFolder' cannot be an empty string.")
    }
    
    outputFolder <<- newOutputFolder
}

setNumberOfCores <- function(newNumberOfCores) {
    if(newNumberOfCores <= 0) {
        stop("Error: 'numberOfCores' cannot be less than or equal to 0.")
    }

    numberOfCores <<- newNumberOfCores
}

addFixedParameter <- function(name, initialValues, range) {
    if(length(initialValues) == 1) {
        newParameter <- list(
            name = name, 
            initialValues = list(initialValues), 
            range = range
        )
    }
    else {
        newParameter <- list(
            name = name, 
            initialValues = initialValues, 
            range = range
        )
    }
    
    
    if(is.null(parameters)) {
        parameters <<- list(newParameter)
    }
    else {
        parameters <<- c(parameters, list(newParameter))
    }
}

addRandomParameter <- function(name, numberOfInitialValues, range) {
    newParameter <- list(
        name = name, 
        numberOfInitialValues = numberOfInitialValues, 
        range = range
    )
    
    
    if(is.null(parameters)) {
        parameters <<- list(newParameter)
    }
    else {
        parameters <<- c(parameters, list(newParameter))
    }
}

setMaxNumberOfGenerations <- function(newMaxNumberOfGenerations) {
    if(newMaxNumberOfGenerations <= 0) {
        stop("Error: 'maxNumberOfGenerations' cannot be less than or equal to 0.")
    }

    maxNumberOfGenerations <<- newMaxNumberOfGenerations
}

setEvaluationFunctionSelected <- function(newEvaluationFunctionSelected) {
    if(nchar(newEvaluationFunctionSelected) == 0) {
        stop("Error: 'evaluationFunctionSelected' cannot be an empty string.")
    }

    evaluationFunctionSelected <<- newEvaluationFunctionSelected
}

setSelectionOperatorSelected <- function(newSelectionOperatorSelected) {
    if(nchar(newSelectionOperatorSelected) == 0) {
        stop("Error: 'selectionOperatorSelected' cannot be an empty string.")
    }
    
    selectionOperatorSelected <<- newSelectionOperatorSelected
}

setCrossoverOperatorSelected <- function(newCrossoverOperatorSelected) {
    if(nchar(newCrossoverOperatorSelected) == 0) {
        stop("Error: 'crossoverOperatorSelected' cannot be an empty string.")
    }
    
    crossoverOperatorSelected <<- newCrossoverOperatorSelected
}

setMutationOperatorSelected <- function(newMutationOperatorSelected) {
    if(nchar(newMutationOperatorSelected) == 0) {
        stop("Error: 'mutationOperatorSelected' cannot be an empty string.")
    }
    
    mutationOperatorSelected <<- newMutationOperatorSelected
}

setReplacementOperatorSelected <- function(newReplacementOperatorSelected) {
    if(nchar(newReplacementOperatorSelected) == 0) {
        stop("Error: 'replacementOperatorSelected' cannot be an empty string.")
    }
    
    replacementOperatorSelected <<- newReplacementOperatorSelected
}

setFitnessObjective <- function(newFitnessObjective) {
    fitnessObjective <<- newFitnessObjective
}

setMaximiseFitness <- function(newMaximiseFitness) {
    maximiseFitness <<- newMaximiseFitness
}

setBurn_in <- function(newBurn_in) {
    if(newBurn_in <= 0) {
        stop("Error: 'burn_in' cannot be less than or equal to 0.")
    }

    burn_in <<- newBurn_in
}

checkRunParams <- function() {
    if(is.null(baseConfigPath)) {
        stop("Error: 'baseConfigPath' has not been defined. Use function 'setBaseConfigPath'.")
    }

    if(is.null(outputFolder)) {
        stop("Error: 'outputFolder' has not been defined. Use function 'setOutputFolder'.")
    }

    if(is.null(numberOfCores)) {
        stop("Error: 'numberOfCores' has not been defined. Use function 'setNumberOfCores'.")
    }

    if(is.null(parameters)) {
        stop("Error: 'parameters' has not been defined. Use function 'addParameter'.")
    }

    if(is.null(maxNumberOfGenerations)) {
        stop("Error: 'maxNumberOfGenerations' has not been defined. Use function 'setMaxNumberOfGenerations'.")
    }

    if(is.null(evaluationFunctionSelected)) {
        stop("Error: 'evaluationFunctionSelected' has not been defined. Use function 'setEvaluationFunctionSelected'.")
    }

    if(is.null(selectionOperatorSelected)) {
        stop("Error: 'selectionOperatorSelected' has not been defined. Use function 'setSelectionOperatorSelected'.")
    }

    if(is.null(crossoverOperatorSelected)) {
        stop("Error: 'crossoverOperatorSelected' has not been defined. Use function 'setCrossoverOperatorSelected'.")
    }

    if(is.null(mutationOperatorSelected)) {
        stop("Error: 'mutationOperatorSelected' has not been defined. Use function 'setMutationOperatorSelected'.")
    }

    if(is.null(replacementOperatorSelected)) {
        stop("Error: 'replacementOperatorSelected' has not been defined. Use function 'setReplacementOperatorSelected'.")
    }

    if(is.null(fitnessObjective)) {
        stop("Error: 'fitnessObjective' has not been defined. Use function 'setFitnessObjective'.")
    }

    if(is.null(maximiseFitness)) {
        stop("Error: 'maximiseFitness' has not been defined. Use function 'setMaximiseFitness'.")
    }

    if(is.null(burn_in)) {
        stop("Error: 'burn_in' has not been defined. Use function 'setBurn_in'.")
    }
}

generateRunParams <- function() {
    library(jsonlite)

    checkRunParams()
    
    
    run_params_content <- list(
        parameters = parameters,
        baseConfigPath = baseConfigPath,
        outputFolder = outputFolder,
        maxNumberOfGenerations = maxNumberOfGenerations,
        numberOfCores = numberOfCores,
        burn_in = burn_in,
        maximiseFitness = maximiseFitness, 
        evaluationFunctionSelected = evaluationFunctionSelected, 
        selectionOperatorSelected = selectionOperatorSelected, 
        crossoverOperatorSelected = crossoverOperatorSelected, 
        mutationOperatorSelected = mutationOperatorSelected, 
        replacementOperatorSelected = replacementOperatorSelected, 
        fitnessObjective = fitnessObjective
    )

    run_params_json_content <- toJSON(run_params_content, pretty = TRUE, auto_unbox = TRUE, digits = NA)

    run_params_path <- ".run_params_tmp.json"

    writeLines(run_params_json_content, run_params_path)

    return(run_params_path)
}

executeOptimiser <- function(run_params_path, overwrite) {
    args <- c("--run_params_path", run_params_path)
    
    if(overwrite) {
        args <- c(args, "--overwrite")
    }

    result <- system2("/mnt/c/Users/mcs20/Desktop/EEZA/Weaver/optimiser.out", args=args, stdout="", stderr="", wait=TRUE)
}

run <- function(overwrite) {
    run_params_path <- generateRunParams()

    executeOptimiser(run_params_path, overwrite);

    file.remove(run_params_path)
}