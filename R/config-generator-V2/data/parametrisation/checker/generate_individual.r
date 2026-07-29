
if (!exists("called_from_main", inherits = FALSE)) {
  stop("This script must be sourced from a main script.")
}

#' Source a script while retaining its path
#'
#' This function sources an R script in a local environment, keeping the script's directory
#' available and exporting all functions to the parent environment.
#'
#' @param path Character. Path to the R script to source.
#' @return Invisibly returns the local environment containing sourced objects.
#' @examples
#' \dontrun{
#' source_with_script_path("scripts/load_or_install_package.r")
#' }
source_with_script_path <- function(path) {
	path <- normalizePath(path)

    parent_env <- parent.frame()
    local_env <- new.env(parent = parent_env)

	local_env$script_path <- dirname(path)

	local_env$called_from_main <- TRUE

	sys.source(path, envir = local_env)

	for (name in ls(local_env)) {
		obj <- local_env[[name]]
		if (is.function(obj)) {
			assign(name, obj, envir = parent_env)
		}
	}

	invisible(local_env)
}


source_with_script_path(file.path(script_path, "tpc.r"))
source_with_script_path(file.path(script_path, "growthCurve.r"))


#' Generate developmental lengths and masses for an individual
#'
#' Computes the length and dry mass trajectories for an individual across a series of developmental times.
#'
#' @param timeStep Numeric. Time step used for the developmental calculations.
#' @param finalAgeVector Numeric vector. Vector of ages at which lengths and masses are computed.
#' @param thisLmax Numeric. Maximum length of the individual.
#' @param thisGrowth Numeric. Growth parameter for the individual.
#' @param eggDryMass Numeric or NULL. Dry mass of the egg, if applicable.
#' @param A Numeric or NULL. Growth model parameter.
#' @param type Character or NULL. Type of growth model.
#' @param coefficientForMassA Numeric. Coefficient used to convert length to dry mass for immature stages.
#' @param coefficientForMassAforMature Numeric. Coefficient used to convert length to dry mass for mature stages.
#' @param scaleForMassB Numeric. Exponent used to convert length to dry mass for immature stages.
#' @param scaleForMassBforMature Numeric. Exponent used to convert length to dry mass for mature stages.
#'
#' @return A list with two elements:
#' \describe{
#'   \item{finalMassesVector}{Numeric vector of dry masses at each developmental age.}
#'   \item{finalLengthsVector}{Numeric vector of lengths at each developmental age.}
#' }
#' 
#' @export
devTimesLengthMassesTemp<-function(timeStep, finalAgeVector,thisLmax,thisGrowth,eggDryMass = NULL, A = NULL, type = NULL, coefficientForMassA, coefficientForMassAforMature, scaleForMassB, scaleForMassBforMature)
{
    finalLengthsVector<-vector()
    finalMassesVector<-vector()
    
    for (i in 1:length(finalAgeVector))
    {
        
        finalLengthsVector[i] = growthCurve(Lmax=thisLmax,k=thisGrowth,t=finalAgeVector[i]*timeStep,tmax=finalAgeVector[length(finalAgeVector)]*timeStep,
            eggDryMass = eggDryMass, A = A, type = type, coefficientForMassA, scaleForMassB)
            
        if(i == length(finalAgeVector)){ 
            finalMassesVector[i] = calculateDryMass(finalLengthsVector[i], coefficientForMassAforMature, scaleForMassBforMature)
        }else{
            finalMassesVector[i] = calculateDryMass(finalLengthsVector[i], coefficientForMassA, scaleForMassB)
        }

    }

    return(list(finalMassesVector=finalMassesVector, finalLengthsVector=finalLengthsVector))
}

#' Generate an individual with trait values and growth trajectory
#'
#' Creates a single individual based on species-level data, including traits, temperature-dependent adjustments,
#' growth curves, and reproductive parameters.
#'
#' @param data_species List. Output of \code{\link{read_data_species}}, containing species traits and parameters.
#' @param temperature Numeric. Environmental temperature to adjust temperature-dependent traits.
#' @param timeStep Numeric. Time step used for developmental calculations.
#' @param timeAddedToMeetLastRepro Numeric. Adjustment time to ensure the last reproduction occurs within lifespan.
#' @param N Integer. Number of individuals for trait sampling (used for correlation generation).
#' @param corrLk Numeric. Correlation coefficient between length and growth rate.
#' @param corrLdevT Numeric. Correlation coefficient between length and developmental time.
#' @param corrkdevT Numeric. Correlation coefficient between growth rate and developmental time.
#' @param silent Logical. If TRUE, suppresses plotting of growth curves.
#'
#' @return A list representing a single individual with the following elements:
#' \describe{
#'   \item{gender}{Character. "FEMALE" or "MALE".}
#'   \item{traits}{List of trait values sampled from species distributions.}
#'   \item{traitsTemperature}{List of temperature-adjusted trait values.}
#'   \item{finalAgeVector}{Numeric vector of developmental ages in time steps.}
#'   \item{finalLengthsVector}{Numeric vector of lengths corresponding to \code{finalAgeVector}.}
#'   \item{finalMassesVector}{Numeric vector of dry masses corresponding to \code{finalAgeVector}.}
#'   \item{ageOfFirstMaturation}{Numeric. Age of first reproduction.}
#'   \item{longevity}{Numeric. Lifespan of the individual.}
#'   \item{currentAge}{Numeric vector of ages from 0 to longevity.}
#'   \item{timeOfReproEvent}{Numeric. Interval between reproductive events.}
#'   \item{capitalBreeding, indeterminateGrowth, eggsPerBatch, ...}{Other species-specific parameters used in individual calculations.}
#' }
#'
#' @export
generateIndividual<-function(data_species, temperature, timeStep, timeAddedToMeetLastRepro, N, corrLk, corrLdevT, corrkdevT, silent){

    if(data_species$capitalBreeding & data_species$indeterminateGrowth){
        stop("Error: capital breeding and indeterminate growth are not compatible")
    }

    minPseudo=1
    maxPseudo=10
    pseudoGrowth<-interpol(rnorm(N),minPseudo,maxPseudo)
    pseudoGrowthMean = mean(pseudoGrowth)
    pseudoGrowthSd = sd(pseudoGrowth)
    pseudoGrowthMin = min(pseudoGrowth)
    pseudoGrowthMax = max(pseudoGrowth)

    pseudo01<-scale(pseudoGrowth)

    minNormalizedPseudoGrowth = (pseudoGrowthMin - pseudoGrowthMean) / pseudoGrowthSd
    maxNormalizedPseudoGrowth = (pseudoGrowthMax - pseudoGrowthMean) / pseudoGrowthSd

    minL = minNormalizedPseudoGrowth 
    maxL = maxNormalizedPseudoGrowth 

    sigma=matrix(c(1,corrLk,corrkdevT,
                corrLk,1,corrLdevT, 
                corrkdevT,corrLdevT,1),nrow=3) 

    res_chol<-chol(sigma)

    print("res_chol:")
    print(res_chol)

    v2<-rnorm(N)  

    preL <- pseudo01*res_chol[1,2] + v2*res_chol[2,2]

    while(sum(preL<minL) > 0 || sum(preL>maxL) > 0)
    {
        v2<-rnorm(N) 

        preL <- pseudo01*res_chol[1,2] + v2*res_chol[2,2] 
    }

    print("cor(preL,pseudo01):")
    print(cor(preL,pseudo01))

    v3<-rnorm(N) 

    predevT <- pseudo01*res_chol[1,3] + v3*res_chol[3,3]

    while(sum(predevT<minL) > 0 || sum(predevT>maxL) > 0)
    {
        v3<-rnorm(N)

        predevT <- pseudo01*res_chol[1,3] + v3*res_chol[3,3] 
    }

    print("cor(predevT,pseudo01):")
    print(cor(predevT,pseudo01))

    print("cor(cbind(preL,predevT,pseudo01)):")
    print(cor(cbind(preL,predevT,pseudo01)))

    print("preL:")
    print(preL)  

    print("sum(preL<minL):")
    print(sum(preL<minL))

    print("sum(preL>maxL):")
    print(sum(preL>maxL))

    Lfemale = calculateDryLength(data_species$femaleWetMass/data_species$conversionToWetMass[[length(data_species$conversionToWetMass)]], data_species$coefficientForMassAforMature, data_species$scaleForMassBforMature)

    print("Lfemale:")
    print(Lfemale)

    traits <- list()

    for(trait in names(data_species$traits)){
        traits[[trait]] = list()

        if(trait == "base.growth"){
            traits[[trait]]$value = interpol2(pseudoGrowth,pseudoGrowthMin,pseudoGrowthMax,data_species$traits[[trait]]$min,data_species$traits[[trait]]$max)
        }else if(trait == "base.devTime"){
            traits[[trait]]$value = interpol(predevT,data_species$traits[[trait]]$min,data_species$traits[[trait]]$max)
        }else if(trait == "base.lengthAtMaturation"){
            traits[[trait]]$value = interpol2(preL, minL, maxL, data_species$traits[[trait]]$min, data_species$traits[[trait]]$max)
        }
        else{
            traits[[trait]]$value = interpol(rnorm(N),data_species$traits[[trait]]$min,data_species$traits[[trait]]$max)
        }

        if(data_species$traits[[trait]]$temperature$dependent){
            if(trait != "base.lengthAtMaturation"){
                traits[[trait]]$temperature = list()

                for(element in names(data_species$traits[[trait]]$temperature$elements)){
                    traits[[trait]]$temperature[[element]] = interpol(rnorm(N),data_species$traits[[trait]]$temperature$elements[[element]]$min,data_species$traits[[trait]]$temperature$elements[[element]]$max)
                }
            }
        }
    }

    print("cor(cbind(preL,traits[[\"base.devTime\"]]$value,pseudo01)):")
    print(cor(cbind(preL,traits[["base.devTime"]]$value,pseudo01)))
    
    print("Lmax:")
    print(traits[["base.lengthAtMaturation"]]$value)

    print("cor(data.frame(traits[[\"base.devTime\"]]$value,traits[[\"base.lengthAtMaturation\"]]$value,traits[[\"base.growth\"]]$value)):")
    print(cor(data.frame(traits[["base.devTime"]]$value,traits[["base.lengthAtMaturation"]]$value,traits[["base.growth"]]$value)))


    if(!silent){
        tMax = round(traits[["base.devTime"]]$value / timeStep) * timeStep

        growthCurvePlot <- plot_ly() %>%
            layout(
                xaxis = list(title = "Age", range = c(0, max(tMax))),
                yaxis = list(title = "Length", range = c(0, max(traits[["base.lengthAtMaturation"]]$value))),
                title = "Growth curves for different individuals"
            )

        for (xx in 1:N) {
            t = seq(0, tMax[xx], timeStep)
            
            eggDryMass = data_species$eggDryMass + data_species$eggDryMass*traits[["base.factorEggMass"]]$value[xx]	
            
            L<-growthCurve(Lmax=traits[["base.lengthAtMaturation"]]$value[xx],k=traits[["base.devTime"]]$value[xx],t=t,tmax=tMax[xx],eggDryMass = eggDryMass, 
                A = data_species$growthModel$defaultAtTempFromLab$A, type = data_species$growthModel$defaultAtTempFromLab$model, coefficientForMassA = data_species$coefficientForMassA, scaleForMassB = data_species$scaleForMassB)
            
            growthCurvePlot <- growthCurvePlot %>%
                add_lines(x = t, y = L, name = paste("Individual", xx),
                        line = list(width = 3))
        }
        
        print(growthCurvePlot)
    }


    thePosition = 1

    individual <- list()

    individual$gender <- "FEMALE"

    individual[["traits"]] <- list()

    for(trait in names(traits)){
        individual$traits[[trait]] = list()

        individual$traits[[trait]]$value = traits[[trait]]$value[thePosition]

        if(data_species$traits[[trait]]$temperature$dependent){
            if(trait != "base.lengthAtMaturation"){
                individual$traits[[trait]]$temperature = list()

                for(element in names(traits[[trait]]$temperature)){
                    individual$traits[[trait]]$temperature[[element]] = traits[[trait]]$temperature[[element]][thePosition]
                }
            }
        }
    }

    baseDevTimeProportion = individual$traits[["base.devTime"]]$value / data_species$ageVectorDays[length(data_species$ageVectorDays)]

    individual$minPlasticityKVonBertalanffy = data_species$minPlasticityKVonBertalanffy

    individual$capitalBreeding = data_species$capitalBreeding 

    individual$timeOfReproEventDuringCapitalBreeding = round(data_species$timeOfReproEventDuringCapitalBreeding / timeStep)

    individual$numberOfCapitalBreeds = data_species$numberOfCapitalBreeds 

    individual$eggDryMass = data_species$eggDryMass

    individual$indeterminateGrowth = data_species$indeterminateGrowth

    individual$eggsPerBatch = data_species$eggsPerBatch

    individual$femaleMaxReproductionEvents = data_species$femaleMaxReproductionEvents

    individual$coefficientForMassA = data_species$coefficientForMassA
    
    individual$scaleForMassB = data_species$scaleForMassB

    individual$coefficientForMassAforMature = data_species$coefficientForMassAforMature
    
    individual$scaleForMassBforMature = data_species$scaleForMassBforMature

    individual$conversionToWetMass = data_species$conversionToWetMass

    individual$tempFromLab = data_species$tempFromLab

    individual$YodzisA = data_species$YodzisA 

    individual$YodzisB = data_species$YodzisB

    individual$plasticityDueToConditionSearch = data_species$plasticityDueToConditionSearch

    individual$plasticityDueToConditionSpeed = data_species$plasticityDueToConditionSpeed

    individual$activityUnderPredationRisk = data_species$activityUnderPredationRisk

    individual$eggsPerBatchFromEquation = data_species$eggsPerBatchFromEquation

    individual$eggsPerBatch = data_species$eggsPerBatch

    individual$interceptForEggBatchFromEquation = data_species$interceptForEggBatchFromEquation

    individual$slopeForEggBatchFromEquation = data_species$slopeForEggBatchFromEquation

    
    individual[["traitsTemperature"]] <- list()

    for(trait in names(individual$traits)){
        if(data_species$traits[[trait]]$temperature$dependent){
            if(trait == "base.lengthAtMaturation"){
                individual$traitsTemperature[[trait]] = use_TSR(
                    temperature=temperature, 
                    tempSizeRuleVector=data_species$traits[[trait]]$temperature$tempSizeRuleVector, 
                    coefficientForMassAforMature=data_species$coefficientForMassAforMature, 
                    scaleForMassBforMature=data_species$scaleForMassBforMature, 
                    traitAtTempRef=individual$traits[[trait]]$value,
                    Tref=data_species$tempFromLab
                )
            }
            else{
                individual$traitsTemperature[[trait]] = use_Pawar_2018(
                    trait_name=trait,
                    temperature=temperature, 
                    traitAtTopt=individual$traits[[trait]]$value,  
                    Topt=individual$traits[[trait]]$temperature$temperatureOptimal,  
                    ED=individual$traits[[trait]]$temperature$energyDecay,  
                    E=individual$traits[[trait]]$temperature$activationEnergy, 
                    Tref=individual$traits[[trait]]$temperature$temperatureRef
                )
            }
        }
        else{
            individual$traitsTemperature[[trait]] = individual$traits[[trait]]$value
        }
    }

    individual$growthModel <- list()

    if(length(data_species$growthModel$growthModelThermalChanges) > 0)
    {
        growthModelThermalChangesOrder <- list()

        if(temperature < data_species$tempFromLab)
        {
            growthModelThermalChangesOrder <- data_species$growthModel$growthModelThermalChanges[order(sapply(data_species$growthModel$growthModelThermalChanges, function(x) x$temperature), decreasing = TRUE)]
        
        
            index = 1
            found = FALSE

            while(!found)
            {
                if(index > length(growthModelThermalChangesOrder))
                {
                    found = TRUE
                }
                else
                {
                    if(growthModelThermalChangesOrder[[index]]$temperature < temperature)
                    {
                        found = TRUE
                    }
                    else
                    {
                        index = index + 1
                    }
                }
            }

            if(index == 1)
            {
                individual$growthModel = data_species$growthModel$defaultAtTempFromLab
            }
            else
            {
                individual$growthModel <- list(
                    "model" = growthModelThermalChangesOrder[[(index-1)]]$model,
                    "A" = growthModelThermalChangesOrder[[(index-1)]]$A
                )
            }
        }
        else
        {
            growthModelThermalChangesOrder <- data_species$growthModel$growthModelThermalChanges[order(sapply(data_species$growthModel$growthModelThermalChanges, function(x) x$temperature))]
        
        
            index = 1
            found = FALSE

            while(!found)
            {
                if(index > length(growthModelThermalChangesOrder))
                {
                    found = TRUE
                }
                else
                {
                    if(growthModelThermalChangesOrder[[index]]$temperature > temperature)
                    {
                        found = TRUE
                    }
                    else
                    {
                        index = index + 1
                    }
                }
            }

            if(index == 1)
            {
                individual$growthModel = data_species$growthModel$defaultAtTempFromLab
            }
            else
            {
                individual$growthModel <- list(
                    "model" = growthModelThermalChangesOrder[[(index-1)]]$model,
                    "A" = growthModelThermalChangesOrder[[(index-1)]]$A
                )
            }
        }
    }
    else
    {
        individual$growthModel = data_species$growthModel$defaultAtTempFromLab
    }

    finalDevTimeProportion = individual$traitsTemperature[["base.devTime"]] / (data_species$ageVectorDays[length(data_species$ageVectorDays)] * baseDevTimeProportion)

    individual$finalAgeVector = round((data_species$ageVectorDays * baseDevTimeProportion * finalDevTimeProportion) / timeStep)

    vectorList<-devTimesLengthMassesTemp(timeStep=timeStep, finalAgeVector=individual$finalAgeVector,
        thisLmax=individual$traitsTemperature[["base.lengthAtMaturation"]],
        thisGrowth=individual$traitsTemperature[["base.growth"]],
        eggDryMass = data_species$eggDryMass, 
        A = individual$growthModel$A, type = individual$growthModel$model, 
        coefficientForMassA=data_species$coefficientForMassA, coefficientForMassAforMature=data_species$coefficientForMassAforMature, 
        scaleForMassB=data_species$scaleForMassB, scaleForMassBforMature=data_species$scaleForMassBforMature)

    individual$finalMassesVector = vectorList$finalMassesVector

    individual$finalLengthsVector = vectorList$finalLengthsVector

    if(!silent){
        ageVector = round((data_species$ageVectorDays * baseDevTimeProportion) / timeStep) * timeStep

        devTimesLengthMassesPlot <- plot_ly() %>%
            layout(
                xaxis = list(title = "Age", range = c(0, max(individual$finalAgeVector*timeStep))),
                yaxis = list(title = "Length", range = c(0, max(individual$finalLengthsVector)))
            ) %>%
            add_lines(
                x = individual$finalAgeVector*timeStep, y = individual$finalLengthsVector,
                name = "Temperature Curve",
                line = list(color = "red", width = 3)
            ) %>%
            add_lines(
                x = ageVector, y = growthCurve(Lmax=individual$traits[["base.lengthAtMaturation"]]$value,
                    k=individual$traits[["base.growth"]]$value,t=ageVector,
                    tmax=ageVector[length(ageVector)],eggDryMass = data_species$eggDryMass, A = data_species$growthModel$defaultAtTempFromLab$A, 
                    type = data_species$growthModel$defaultAtTempFromLab$model, data_species$coefficientForMassA, data_species$scaleForMassB),
                name = "Genetic Curve",
                line = list(color = "purple", width = 3, dash = "dash")
            )

        print(devTimesLengthMassesPlot)
    }

    individual$ageOfFirstMaturation = individual$finalAgeVector[data_species$instarFirstReproduction-1]+round(individual$traitsTemperature[["base.pupaPeriodTime"]] / timeStep)

    individual$longevity = round(individual$traitsTemperature[["base.longevitySinceMaturation"]]*individual$ageOfFirstMaturation)

    individual$currentAge = seq(0, individual$longevity)
											 
    if(!data_species$capitalBreeding){  

        timeOfReproEvent = floor((individual$longevity - individual$ageOfFirstMaturation)/data_species$femaleMaxReproductionEvents)

    }else{

        if(data_species$femaleMaxReproductionEvents == data_species$numberOfCapitalBreeds) {
            
            timeOfReproEvent = 0

        }
        else {
            
            totalTimeBreedingCapitally = round(individual$timeOfReproEventDuringCapitalBreeding*(data_species$numberOfCapitalBreeds-1)) ##because the first one is on the pupaPerdiodLength
            timeOfReproEvent = floor((individual$longevity - totalTimeBreedingCapitally - individual$ageOfFirstMaturation)/(data_species$femaleMaxReproductionEvents-data_species$numberOfCapitalBreeds))
        
        }
    }

    if(timeOfReproEvent < 0){
        stop(paste("Error: 'timeOfReproEvent' must have a positive value. Current value:", timeOfReproEvent))
    }

    individual$timeOfReproEvent = timeOfReproEvent

    individual$betaScaleTank = data_species$betaScaleTank

    individual$assignedForMolt = data_species$assignedForMolt

    individual$excessInvestInSize = data_species$excessInvestInSize
    

    return(individual)
}
