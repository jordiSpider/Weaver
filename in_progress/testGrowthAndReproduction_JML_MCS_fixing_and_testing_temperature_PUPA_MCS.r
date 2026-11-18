##@@TODO: include timeStep everywhere (search_area, etc.). Note: do it for search_area for the very last value... otherwise it is affected by non-linearities
##@@TODO: Add Pawar_2018 and test TPCs for different time-dependent variables.
##@@TODO: Add the 4parameterLogisticCurve and test intraspecific variability, 
##correlations and temperature for Pardosa. Add the offset include by Mario in file 
##special_growth_example_log4param_fixedA.r

##@@WARNING: In this version we have rearranged search area and speed temperature dependencies - it was wrong before
##@@WARNING: In this version we have rearranged search area and speed temperature dependencies - it was wrong before
##@@TODO: VB rearrangement of equation vs optimization - is the difference due to the use of very tiny animals?
##@@See at initialization loop below the new rearrangement method by Mario
##@@Code: logistic does not need optimization, logistic fixed with rearrangement
##@@Parameters: timeStep and devTimeVector need to be co-parameterized
rm(list=ls(all=TRUE))


##@@Params: dinosaurs now very sensitive to vorMin and vorMax!!

yodzis<-function(M,newA=0.1,newB=0.75)  ##input fresh mass in mg, output in mg
{

	wetFood<-newA*M^newB ##defaults to 0.1
	
return(wetFood)
}


garland<-function(M)  ##input fresh mass in Kg, output in fresh Kg - coefficient modified from Garland 1983 from 152 to 0.152 to set everything to Kg... before input in Kg output in g.
{

	wetFood=0.152*M^0.738

return(wetFood)
}

calculateDryMass<-function(length, coefficient, scale)
{
	# @@Mario: exceptions for mature needed
	dryMass = coefficient*length^scale
return(dryMass)
} 

calculateDryLength<-function(M, coefficient, scale){
   # @@Mario: exceptions for mature needed
   L<-(M/coefficient)^(1/scale)
return(L)
}



##choose sim type 1-arthropods, 2-dinosaurs, 3-aquatic, 4-Amblyseius, 5-Chironomidae
  ##"Rscript" "json"
type=3
###With this file we remove the old choles() ways and work With
###a module of 3 variables
N=10


##@@Params: The crossing of the trajectories can be better achieved with all corr = 0
corrLk = 0.99
corrLdevT = 0.99
corrkdevT = 0.99

# foragingPlasticityBand = "proportional"  ##@@Mario, @@Params: new parameter, "proportional" "additive"



if(type==1){

	fileName="Consumer_fast_asexual_11012025.json"  
	simulationType = "arthropods" ##Von Bertalanffy growth
	kProduct=0.0##0.5
	LdistProd = 0.0 ##0.2
	devTimer = 0.0##0.3
	lowerThreshold = 1.0 ##@Parameters: always > 0
	upperThreshold = 35.0 ##For maximum averages in JJCasas data
	##@Code: @@Mario: this thresholds must be included for every temp-dependent trait
	##e.g. lowerTempThresholdVor
	maxPlastBV = 0.09 ##0.5 identical to Yodzis afer h
	h_enhancement = 0.0 ##0.2 identical to Yodzis afer h
	# henc_decrease = 0.5
	temperature = 25.0 ##"useTempFromLab"
	vorMin = 1.0
	vorMax = 1.0
	multiplierForFieldMetabolicRate = 3
	decreaseOnTraitsDueToEncounters = 10 ##as in Condition
	thisIndet = TRUE
	propMov = 0
	assimWithBodyMass = FALSE
	alterMaxIngestion = 0.15
	newB = 0.75##0.15
	# newAAdult = 0.4##0.02 ##they have to eat 5 times as much in order to meet energetic demand
	# newAJuv = 0.1
	newAJuv = 0.1#0.05  
	newAAdult = 0.1#0.15
	tempDependency = FALSE  ##optimal temp needs increases in voracity > Yodzis
	devTimeTempFromGillooly = FALSE
	propDecreaseDevTimeFromLabToOptimal = 0.1 ##means proportion decrease by 1 ºC difference
	capitalBreeding = TRUE
	numberOfCapitalBreeds = 2
	Debug=TRUE
	eggsPerBatchFromEquation = FALSE
	timeStep=1
	timeAddedToMeetLastRepro = 3*(1/timeStep)
	timeOfReproEventDuringCapitalBreeding = 371*(1/timeStep)
	fixLengthAtBirth = TRUE
	data_from = "json"
	aLogis = NA	
	includeMetabolicRate = TRUE

}

if(type==2){

	fileName="Tyrannosaurus_rex_2023_11012025.json" ##"Edmontosaurus_2023.json" 
	simulationType = "dinosaurs" ##Logistic growth
	kProduct=0.0##0.5
	LdistProd = 0.0 ##0.2
	devTimer = 0.0
	# maxPlastBV = 0.025 ##additive
    # maxPlastBV = 25.0 ##proportional
	temperature = 21
    maxPlastBV = 0.05 ##0.05  ##horizontal parabole at 0.0001
	h_enhancement = 0.0 ##0.2 ##0.02  ##horizontal parabole at 0.0001
	vorMin = 1.0 ##0.80 ##Trait values must be much lower for dinos
	vorMax = 1.0
    decreaseOnTraitsDueToEncounters = 10 ##as in Condition
	propMov = 0
	assimWithBodyMass = FALSE
	alterMaxIngestion = 0.01
	tempDependency = FALSE
	devTimeTempFromGillooly = FALSE
	propDecreaseDevTimeFromLabToOptimal = 0.1 ##means proportion decrease by 1 ºC difference
	timeStep=1
	capitalBreeding = FALSE
	numberOfCapitalBreeds = 2
	timeOfReproEventDuringCapitalBreeding = 371*(1/timeStep) ##fixed until further notice 
	Debug=TRUE
	eggsPerBatchFromEquation = FALSE
	timeAddedToMeetLastRepro = 3*(1/timeStep)
	fixLengthAtBirth=TRUE
	data_from = "json"
	aLogis = NA		
	includeMetabolicRate = TRUE

}

if(type==3){

	fileName="Isoperla_grammatica_11012025.json" 
	simulationType = "aquatic"  ##Exponential growth
	kProduct=0.0##0.5
	LdistProd = 0.0 ##0.2
	devTimer = 0.0 #0.3
	# xbase=5
	tprop=0.5
	optSteps=1000
	lowerThreshold = 1.0 ##@Parameters: always > 0
	upperThreshold = 17.22 ##17.22 - For maximum averages in JJCasas data
	##@Code: @@Mario: this thresholds must be included for every temp-dependent trait
	##e.g. lowerTempThresholdVor
	# maxPlastBV = 0.04 ##@Params: TODO, find this value here by optimization at tempFromLab
    maxPlastBV = 0.01
	h_enhancement = 0.0 ##0.5
	temperature = "useTempFromLab" ##14
	vorMin = 1.0#10
	vorMax = 1.0#12
	multiplierForFieldMetabolicRate = 3
	# newA = 0.6#0.8  ##they have to eat three as much in order to meet energetic demand
	decreaseOnTraitsDueToEncounters = 10 ##as in Condition
	propMov = 0
	assimWithBodyMass = FALSE
	alterMaxIngestion = 0.01
	newB=0.75 ##0.85
	newAAdult = 0.1 #0.3##0.02 ##they have to eat 5 times as much in order to meet energetic demand
	newAJuv = 0.1#0.05
	tempDependency = FALSE
	devTimeTempFromGillooly = FALSE
	propDecreaseDevTimeFromLabToOptimal = 0.1 ##means proportion decrease by 1 ºC difference
	capitalBreeding = TRUE
	numberOfCapitalBreeds = 2
	Debug=TRUE
	eggsPerBatchFromEquation = TRUE  ##eggs per batch will depend also on temperature then
	slopeForEggBatchFromEquation = 1.676  ##Mass is wetMass
	interceptForEggBatchFromEquation = 0
	fractToReduceEggs = 1.0 ###use it on eggDryMass here in order to work
	timeStep=1
	timeOfReproEventDuringCapitalBreeding = 3*(1/timeStep) ##fixed until further notice 
	timeAddedToMeetLastRepro = 1.5*(1/timeStep)
	fixLengthAtBirth=TRUE
	data_from = "json"
	aLogis = NA		
	includeMetabolicRate = TRUE

}

if(type==4){
##Important facts for tiny creatures laying large eggs: very sensitive to factoEggMass.. must be much smaller
	fileName="Amblyseius_swirskii_a_b_changed.json" ##"Orius_laevigatus.json"  ##"Amblyseius_swirskii_a_b_changed.json" 
	simulationType = "arthropods" ##Von Bertalanffy growth
	kProduct=0.0##0.5
	LdistProd = 0.0 ##0.2
	devTimer = 0.0##0.3##0.3
	tprop=0.5
	optSteps=1000
	lowerThreshold = 1.0 ##@Parameters: always > 0
	upperThreshold = 35.0 ##For maximum averages in JJCasas data
	##@Code: @@Mario: this thresholds must be included for every temp-dependent trait
	##e.g. lowerTempThresholdVor
	maxPlastBV = 0.09 ##0.5 identical to Yodzis afer h
	h_enhancement = 0.0 ##0.2 identical to Yodzis afer h
	# henc_decrease = 0.5
	temperature = "useTempFromLab"
	vorMin = 1.0
	vorMax = 1.0
	multiplierForFieldMetabolicRate = 3
	decreaseOnTraitsDueToEncounters = 10 ##as in Condition
	thisIndet = FALSE
	propMov = 0
	assimWithBodyMass = FALSE
	alterMaxIngestion = 0.15
	newB = 0.75##0.15
	# newAAdult = 0.4##0.02 ##they have to eat 5 times as much in order to meet energetic demand
	# newAJuv = 0.1
	newAJuv = 0.1  
	newAAdult = 0.1
	tempDependency = FALSE  ##optimal temp needs increases in voracity > Yodzis
	devTimeTempFromGillooly = FALSE
	propDecreaseDevTimeFromLabToOptimal = 0.1 ##means proportion decrease by 1 ºC difference
	capitalBreeding = FALSE
	numberOfCapitalBreeds = 0
	Debug=FALSE
	eggsPerBatchFromEquation = FALSE
	timeStep=0.05
	timeAddedToMeetLastRepro = 0.1 # 0.1*(1/timeStep) ##do not need to covary with timeStep
	fixLengthAtBirth=FALSE
	data_from = "json"
	aLogis = NA		
	includeMetabolicRate = TRUE
	

}


if(type==5){
##Important facts for tiny creatures laying large eggs: very sensitive to factoEggMass.. must be much smaller
	# fileName="Amblyseius_swirskii_con_Jordi_30012024.json"  
	simulationType = "aquatic" ##Von Bertalanffy growth
	kProduct=0.0##0.5
	LdistProd = 0.0 ##0.2
	devTimer = 0.0##0.3##0.3
	tprop=0.5
	optSteps=1000
	lowerThreshold = 1.0 ##@Parameters: always > 0
	upperThreshold = 35.0 ##For maximum averages in JJCasas data
	##@Code: @@Mario: this thresholds must be included for every temp-dependent trait
	##e.g. lowerTempThresholdVor
	maxPlastBV = 0.09 ##0.5 identical to Yodzis afer h
	h_enhancement = 0.0 ##0.2 identical to Yodzis afer h
	# henc_decrease = 0.5
	temperature = "useTempFromLab"
	vorMin = 1.0
	vorMax = 1.0
	multiplierForFieldMetabolicRate = 3
	decreaseOnTraitsDueToEncounters = 10 ##as in Condition
	thisIndet = FALSE
	propMov = 0
	assimWithBodyMass = FALSE
	newB = 0.75##0.15
	# newAAdult = 0.4##0.02 ##they have to eat 5 times as much in order to meet energetic demand
	# newAJuv = 0.1
	newAJuv = 0.05  
	newAAdult = 0.15
	tempDependency = FALSE  ##optimal temp needs increases in voracity > Yodzis
	devTimeTempFromGillooly = FALSE
	propDecreaseDevTimeFromLabToOptimal = 0.1 ##means proportion decrease by 1 ºC difference
	capitalBreeding = TRUE
	numberOfCapitalBreeds = 2
	Debug=TRUE
	fractToReduceEggs = 0.5 ###this is to test with more events of capital breeding, must be set to 1 for only 1 capital breed
	eggsPerBatchFromEquation = FALSE
	timeStep=0.1
	timeOfReproEventDuringCapitalBreeding = 3*(1/timeStep) ##fixed until further notice 
	timeAddedToMeetLastRepro = 1*(1/timeStep)
	fixLengthAtBirth=FALSE
	data_from = "Rscript"
	# aLogis = NA	
	includeMetabolicRate = TRUE

}


###Xmid is better estimated (and anchor) from eggDryMass, otherwise eggDryMass vaires freely
##TODO: @@Mario - is it done from factorEggMass?
###Four-parameter logistic does not make sense because the calculus of Xmid gives:
###(B-A)/(A-A) -> infinite... as A=lengthAtBirth
###Thus 4PL can be used to approximate eggDryMass but then use "Logistic" (3-parameters) throughout

##@@Params: LdistanceMin and LdistanceMin are now the range of adult lengths (not the Linf limits)

###@@Mario: Here we include a three variable module to play with devTimeVector also
###Thus now we remove that part of the code on choles() and convert the three parameters
###to traits

###@@Mario: lengthAtBirth should be used to calculate Xmid and vonBertTime0 but
###adding factorEggMassFromMom, as DONE here!!

##For all positive correlations the trend is maintained. For negative correlation btw L and k, the otherwise
##two correlations must be 0 in order to maintain this and only meaningful genetic correlation
# corrLk = -0.99
# corrLdevT = 0.0
# corrkdevT = 0.0


###Here the data will be read from the Json file directly
###Use first consumer_fast_asexual from Weaver-WSL on 01/07/2023

# setwd("C:/Users/jordi/Dropbox/EEZA_2023/Weaver_2023/trace_document/Weaver_to_R_code")
# setwd("/home/macarse/Escritorio/EEZA/Weaver")
setwd("C:/Devs/Github/Weaver/config")

# setwd("C:/Weaver_temporal_work")
getwd()

# dirWorld="C:/Users/jordi/Dropbox/EEZA_2023/Weaver_2023/Weaver-main5/Weaver-main/config/base_arthropods_temp"
# dirSp="C:/Users/jordi/Dropbox/EEZA_2023/Weaver_2023/Weaver-main5/Weaver-main/config/base_arthropods_temp/species"
dirWorld="C:/Devs/Github/Weaver/config"

# dirSp="C:/Users/jordi/Dropbox/EEZA_2023/Weaver_2023/TRACE_document/Weaver_to_R_code"
dirSp="C:/Devs/Github/Weaver/config"

# dirWorld="C:/Weaver_temporal_work"
# dirSp="C:/Weaver_temporal_work"


# worldName="world_params.json"

 #global functions
    interpol<-function(x,miny,maxy)    ##interpolation for vectors
    {
		y=miny+((x-min(x))*((maxy-miny)/(max(x)-min(x))))
		return(y)
    }

    interpol2<-function(x,minx,maxx,miny,maxy)  ##interpolation for values
    {
		y=miny+((x-minx)*((maxy-miny)/(maxx-minx)))
		return(y)
    }


	##make a function to find floats in json
	find_number_in_json<-function(path,json_data)
	{
		fields <- unlist(strsplit(path, "\\."))

		value <- json_data
		for(field in fields) {
			if (!is.null(value[[field]])) {
				value <- value[[field]]
			} 
			else {
				return(NULL)
			}
		}

		return(as.numeric(value))
	}

	##make a function to find chars in json
	find_char_in_json<-function(path,json_data)
	{
		fields <- unlist(strsplit(path, "\\."))

		value <- json_data
		for(field in fields) {
			if (!is.null(value[[field]])) {
				value <- value[[field]]
			} 
			else {
				return(NULL)
			}
		}

		return(value)
	}

	library(jsonlite)


if(data_from == "json")
{

	setwd(dirSp)
	anim<-read_json(fileName)  ##"Consumer_fast_asexual.json" Tyrannosaurus_rex_2023.json
	# str(anim)

	

	##############
	##          ##
	##  Traits  ##
	##          ##
	##############

	traits <- list(
		k = "growth",
		fEgg = "factorEggMass",
		eggDevTime = "eggDevTime",
		assim = "assim",
		Ldistance = "lengthAtMaturation",
		tank = "energy_tank",
		search_area = "search_area",
		speed = "speed",
		bMet = "met_rate",
		actE_met = "actE_met"
	)
	
	for(var_base_name in names(traits)) {
		var_min <- paste0(var_base_name, "Min")
		var_max <- paste0(var_base_name, "Max")


		traitLevel = find_char_in_json(paste("animal.genetics.traits.definition", traits[[var_base_name]], "definitionType", sep = "."), anim)

		if(traitLevel == "SpeciesLevel"){
			var_min_value = find_number_in_json(paste("animal.genetics.traits.definition", traits[[var_base_name]], "speciesLevelParams.value", sep = "."), anim) 
			var_max_value = var_min_value
		}
		else{
			var_min_value = find_number_in_json(paste("animal.genetics.traits.definition", traits[[var_base_name]], "individualLevelParams.ranges.min", sep = "."), anim) 
			var_max_value = find_number_in_json(paste("animal.genetics.traits.definition", traits[[var_base_name]], "individualLevelParams.ranges.max", sep = "."), anim) 
		}
		
		
		assign(var_min, var_min_value)
		assign(var_max, var_max_value)
	}


	# kProduct=0.0 ##@Parameters: parameterization changes depending on growth model, values must be chosen using this framework 
    kMin=kMin-kMin*kProduct
    kMax=kMax+kMax*kProduct

	if(Debug){
		kMin<-kMax<-(kMax+kMin)/2
	}


	LdistanceMin = LdistanceMin-LdistanceMin*LdistProd
	LdistanceMax = LdistanceMax+LdistanceMax*LdistProd

	if(Debug){
		LdistanceMin<-LdistanceMax<-(LdistanceMax+LdistanceMin)/2
	}


	if(Debug){
		fEggMin<-fEggMax<-0
	}



	tempOptVoracity = find_number_in_json("animal.genetics.traits.definition.voracity.temperature.temperatureOptimal.speciesLevelParams.value",anim)
	tempOptSearch = find_number_in_json("animal.genetics.traits.definition.search_area.temperature.temperatureOptimal.speciesLevelParams.value",anim)
	tempOptSpeed = find_number_in_json("animal.genetics.traits.definition.speed.temperature.temperatureOptimal.speciesLevelParams.value",anim)
	tempOptGrowth = find_number_in_json("animal.genetics.traits.definition.growth.temperature.temperatureOptimal.speciesLevelParams.value",anim)
	tempOptDev = 15


	scaleForSearchArea = find_number_in_json("animal.scaleForSearchArea",anim)
	scaleForSpeed = find_number_in_json("animal.scaleForSpeed",anim)


	actE_traits <- list(
		actE_vor = "voracity",
		actE_speed = "speed",
		actE_search = "search_area"
	)

	for(var_base_name in names(actE_traits)) {
		var_min <- paste0(var_base_name, "Min")
		var_max <- paste0(var_base_name, "Max")

		actE_traitLevel = find_char_in_json(paste("animal.genetics.traits.definition", actE_traits[[var_base_name]], "temperature.activationEnergy.definitionType", sep = "."), anim)

		if(actE_traitLevel == "SpeciesLevel"){
			var_min_value = find_number_in_json(paste("animal.genetics.traits.definition", actE_traits[[var_base_name]], "temperature.activationEnergy.speciesLevelParams.value", sep = "."), anim) 
			var_max_value = var_min_value
		}
		else{
			var_min_value = find_number_in_json(paste("animal.genetics.traits.definition", actE_traits[[var_base_name]], "temperature.activationEnergy.individualLevelParams.ranges.min", sep = "."), anim) 
			var_max_value = find_number_in_json(paste("animal.genetics.traits.definition", actE_traits[[var_base_name]], "temperature.activationEnergy.individualLevelParams.ranges.max", sep = "."), anim) 
		}
		
		
		assign(var_min, var_min_value)
		assign(var_max, var_max_value)
	}


	EdVoracity = find_number_in_json("animal.genetics.traits.definition.voracity.temperature.energyDecay.speciesLevelParams.value",anim) 
	EdSearch = find_number_in_json("animal.genetics.traits.definition.search_area.temperature.energyDecay.speciesLevelParams.value",anim) 
	EdSpeed = find_number_in_json("animal.genetics.traits.definition.speed.temperature.energyDecay.speciesLevelParams.value",anim)

	

	###############################
	##                           ##
	##  Reproduction parameters  ##
	##                           ##
	###############################

	femaleMaxReproductionEvents = find_number_in_json("animal.femaleMaxReproductionEvents",anim)

	eggsPerBatch = find_number_in_json("animal.growthModule.eggsPerBatch.value",anim)

	if(type %in% c(3,5)){
	  	eggsPerBatch = round(eggsPerBatch*fractToReduceEggs)
	}



	#########################
	##                     ##
	##  Growth parameters  ##
	##                     ##
	#########################

	devTimeVector = find_number_in_json("animal.growthModule.ageVector",anim)

	##@Parameters: WARNING: devInter needs to have mass in g to be calculated
	##this is to match devTimeVector 
	maxTime = max(devTimeVector)

	devTimeVector = devTimeVector*(1/timeStep)

	eggDryMass = find_number_in_json("animal.growthModule.eggDryMass.value",anim) ##

	if(type==3){
	  	eggDryMass=eggDryMass*fractToReduceEggs
	}

	devInter<-find_number_in_json("animal.devInter",anim)

	indeterminateGrowth = find_char_in_json("animal.growthModule.indeterminateGrowth.enabled",anim)
	if(indeterminateGrowth == "TRUE"){ 
		indeterminateGrowth = TRUE
	}else{
		indeterminateGrowth = FALSE
	}

	if(type==1){
	  	indeterminateGrowth = thisIndet
	}

	instarFirstReproduction<-find_number_in_json("animal.growthModule.indeterminateGrowth.indeterminateGrowthParams.instarFirstReproduction",anim)

	minPlastBV = find_number_in_json("animal.growthModule.minPlasticityKVonBertalanffy",anim) 

	betaScaleTank = find_number_in_json("animal.growthModule.betaScaleTank",anim) 

	longevitySinceMaturation = find_number_in_json("animal.genetics.traits.definition.longevitySinceMaturation.speciesLevelParams.value",anim)

	minPlasticityKVonBertalanffy = find_number_in_json("animal.growthModule.minPlasticityKVonBertalanffy",anim)
	
	pupaPeriodLength = find_number_in_json("animal.genetics.traits.definition.pupaPeriodTime.speciesLevelParams.value",anim)
	
	pupaPeriodTime = pupaPeriodLength
	
	if(type==3){
		pupaPeriodTime=3  ##This is to allow 3 days of maturation before first capital breed
	}

	##@@Mario: remember to change the units of any time variable or time-dependent variable
	pupaPeriodTime=pupaPeriodTime*(1/timeStep)



	#########################
	##                     ##
	##  Length parameters  ##
	##                     ##
	#########################

	growthCurve.type = find_char_in_json("animal.growthModule.growthModel",anim)

	conversionToWetMass = find_number_in_json("animal.conversionToWetMass",anim)

	femaleWetMass = find_number_in_json("animal.growthModule.femaleWetMass",anim) 

	alterMaxIngestion=alterMaxIngestion*femaleWetMass
	
	coefficientForMassA = find_number_in_json("animal.growthModule.coefficientForMassA",anim) ##because for holmetaboles ther are 2
	coefficientForMassAforMature = find_number_in_json("animal.growthModule.coefficientForMassAforMature",anim)
	
	scaleForMassB = find_number_in_json("animal.growthModule.scaleForMassB",anim) ##because for holmetaboles ther are 2
	scaleForMassBforMature = find_number_in_json("animal.growthModule.scaleForMassBforMature",anim)

	#########################################
	##                                     ##
	##  Implementar el nuevo tempSizeRule  ##
	##                                     ##
	#########################################

	tempSizeRuleConstant = 0.0


	####

	# tempSizeRuleVector = find_number_in_json("genetics.traits.definition.lengthAtMaturation.temperature.tempSizeRuleVector",anim)

	# tempSizeRuleVector <- lapply(seq(1, length(tempSizeRuleVector), by = 2), function(i) {
	# 	list(temperature = tempSizeRuleVector[i], dryMass = tempSizeRuleVector[i + 1])
	# })

	####

	
	 
	########################
	##                    ##
	##  Other parameters  ##
	##                    ##
	########################
	
	tempFromLab = find_number_in_json("animal.tempFromLab",anim) 

	if(temperature == "useTempFromLab"){
	   temperature = tempFromLab
	}
	
	if(simulationType == "dinosaurs"){
	  	step = (devTimeVector[2]-devTimeVector[1])*timeStep ##100 steps of 100 days for each data point
	}
	if(simulationType == "arthropods"){
	  	step = 1*timeStep
	}
	if(simulationType == "nematodes"){
	  	step = 1*timeStep
	}
	if(simulationType == "aquatic"){
	  	step = 1*timeStep
	}
	
	


    
	Lfemale = calculateDryLength(femaleWetMass/conversionToWetMass, coefficientForMassAforMature, scaleForMassBforMature)
	Lfemale

	
}else{

	if(type==5){
		##WARNING: MUST BE CHANGED TO 4-PARAMETER LOGISTIC
		growthCurve.type = "Logistic4P"  ##Chironomidae grow 4-parameter logistically even though it is not obvious at first sight
   
    }
	
	femaleMaxReproductionEvents = 2
	
	eggsPerBatch = (500+100)/2

	if(type %in% c(3,5)){
	  eggsPerBatch = round(eggsPerBatch*fractToReduceEggs)
	}

	kMin = 0.30
	kMax = 0.3067

	# kProduct=0.0 ##@Parameters: parameterization changes depending on growth model, values must be chosen using this framework 
    kMin=kMin-kMin*kProduct
    kMax=kMax+kMax*kProduct

	if(Debug & type %in% c(3,4,5)){
		kMin<-kMax<-(kMax+kMin)/2
	}

	conversionToWetMass = 9.193
	coefficientForMassA = 0.002
	scaleForMassB = 2.254

	devTimeVector = c(2.73,5.52,9.28,13.69)#Chironomidae_Cricotopus_necessary_parameters.xls
	
	longevitySinceMaturation = 3.0 ##enough to lay eggs and reproduce as a 
	# # capital breeder but needs to have enougth time: depends on parameter - timeOfReproEventDuringCapitalBreeding
		
	if(simulationType == "dinosaurs"){
	  step = (devTimeVector[2]-devTimeVector[1])*timeStep ##100 steps of 100 days for each data point
	}
	if(simulationType == "arthropods"){
	  step = 1*timeStep
	}
	if(simulationType == "nematodes"){
	  step = 1*timeStep
	}
	if(simulationType == "aquatic"){
	  step = 1*timeStep
	}

    
	devTimeVector = round(devTimeVector*(1/timeStep))
	maxTime = max(devTimeVector)
	aLogis =  -1.7224     
	# blogis = 6.7109     
	# xmid = 4.0316     
	
    # Lfemale = alogis+(blogis-alogis)/(1+exp((xmid-maxTime)*kMin))
	
	femaleWetMass = 1.15775

	Lfemale = calculateDryLength(femaleWetMass*(1/conversionToWetMass), coefficientForMassAforMature, scaleForMassBforMature)
	
	Lfemale

	
	eggDryMass = 5.041708e-05 #individual_growth_temp_JML_vs12_optimizator_adding_Aswirskii.r
	
	tempFromLab = 15

	if(type == 5){
		LdistanceMin = Lfemale-Lfemale*LdistProd
		# find_number_in_json("LdistanceMin",anim)  ##vonBertLdistanceMin 
		LdistanceMax = Lfemale+Lfemale*LdistProd
		# find_number_in_json("LdistanceMax",anim)   ##vonBertLdistanceMax
	}


	instarFirstReproduction<-length(devTimeVector)
	
	indeterminateGrowth = FALSE
	
	fEggMin = -0.001
	fEggMax = 0.001
	
	
	if(Debug){
	fEggMin<-fEggMax<-0
	}
	
	eggDevTimeMin = 1
	eggDevTimeMax = 1

	minPlastBV = 0
	
	
	if(type == 5){ ##we equate to type==3
	
		fileName="Isoperla_grammatica.json" 
		simulationType = "aquatic"  ##Exponential growth
		
			setwd(dirSp)
		anim<-read_json(fileName)  ##"Consumer_fast_asexual.json" Tyrannosaurus_rex_2023.json
		# str(anim)
		anim<-unlist(anim)


		tankMin = find_number_in_json("minTraitsRanges.energy_tank",anim) 
		tankMax = find_number_in_json("maxTraitsRanges.energy_tank",anim) 

		betaScaleTank = find_number_in_json("betaScaleTank",anim) 
		
		coefficientForMassAforMature = find_number_in_json("coefficientForMassAforMature",anim) 
		scaleForMassBforMature = find_number_in_json("scaleForMassBforMature",anim) 

		tempSizeRuleConstant = find_number_in_json("tempSizeRuleConstant",anim)
		
		tempOptVoracity = find_number_in_json("tempOptVoracity",anim)
		tempOptSearch = find_number_in_json("tempOptSearch",anim)
		tempOptSpeed = find_number_in_json("tempOptSpeed",anim)
		tempOptGrowth = 15 #find_number_in_json("tempOptGrowth",anim)
		tempOptDev = 15
		search_areaMin = find_number_in_json("minTraitsRanges.search_area",anim) 
		search_areaMax = find_number_in_json("maxTraitsRanges.search_area",anim) 
		scaleForSearchArea = find_number_in_json("scaleForSearchArea",anim)
		
		speedMin = find_number_in_json("minTraitsRanges.speed",anim) 
		speedMax = find_number_in_json("maxTraitsRanges.speed",anim) 
		scaleForSpeed = find_number_in_json("scaleForSpeed",anim)
		
		# longevitySinceMaturation = find_number_in_json("longevitySinceMaturation",anim)
		
		minPlasticityKVonBertalanffy = find_number_in_json("minPlasticityKVonBertalanffy",anim)
		
		pupaPeriodLength = find_number_in_json("pupaPeriodLength",anim)
		
		pupaPeriodTime = pupaPeriodLength

		##@@Mario: remember to change the units of any time variable or time-dependent variable
		pupaPeriodTime=pupaPeriodTime*(1/timeStep)

		assimMin = find_number_in_json("minTraitsRanges.assim",anim) 
		assimMax = find_number_in_json("maxTraitsRanges.assim",anim) 

		bMetMin = find_number_in_json("minTraitsRanges.met_rate",anim) 
		bMetMax = find_number_in_json("maxTraitsRanges.met_rate",anim) 

		actE_metMin = find_number_in_json("minTraitsRanges.actE_met",anim) 
		actE_metMax = find_number_in_json("maxTraitsRanges.actE_met",anim) 

		actE_vorMin = find_number_in_json("minTraitsRanges.actE_vor",anim) 
		actE_vorMax = find_number_in_json("maxTraitsRanges.actE_vor",anim) 

		actE_speedMin = find_number_in_json("minTraitsRanges.actE_speed",anim) 
		actE_speedMax = find_number_in_json("maxTraitsRanges.actE_speed",anim) 

		actE_searchMin = find_number_in_json("minTraitsRanges.actE_search",anim) 
		actE_searchMax = find_number_in_json("maxTraitsRanges.actE_search",anim) 

		EdVoracity = find_number_in_json("EdVoracity",anim) 
		EdSearch = find_number_in_json("EdSearch",anim) 
		EdSpeed = find_number_in_json("EdSpeed",anim)
		
		EdGrowth = 8 ##EdVoracity ##6
	    EdDev = 8##EdVoracity ##6
		actE=1.1
	}
	
	if(temperature == "useTempFromLab"){
	   temperature = tempFromLab
	}
} 




growthCurve<-function(Lmax = NULL,k = NULL,t = NULL, tmax = NULL, eggDryMass = NULL, 
aLogis = NULL, growthCurve.type = NULL)
{

	lengthAtBirth = calculateDryLength(eggDryMass, coefficientForMassA, scaleForMassB)
	
	if(growthCurve.type == "VonBertalanffy"){

		Linf = (Lmax*exp(k*tmax)-lengthAtBirth)/(exp(k*tmax)-1)		       
			
	    t0 = log(1-(lengthAtBirth/Linf))/k
		
		L = Linf*(1-exp(-k*(t-t0)))
	
	}

	if(growthCurve.type == "Logistic"){
		
		Linf = (Lmax*lengthAtBirth*(exp(k*tmax)-1))/(lengthAtBirth*exp(k*tmax)-Lmax)	
		
		xmid = log((Linf-lengthAtBirth)/lengthAtBirth)*(1/k)
		
		L = Linf/(1+exp((xmid-t)*k))
		
		
	}
	
	if(growthCurve.type == "Logistic4P"){
	   
		xmid = log((-lengthAtBirth + Lmax)*exp(k*tmax)/(lengthAtBirth*exp(k*tmax) - Lmax - aLogis*exp(k*tmax) + aLogis))/k
				
		Linf = lengthAtBirth*exp(k*xmid) + lengthAtBirth - aLogis*exp(k*xmid)
		
		# print(c(xmid,Linf))

	    L = aLogis+(Linf-aLogis)/(1+exp((xmid-t)*k))
	
	}
	
	if(growthCurve.type == "Exponential"){
	
		aExpo = log((Lmax-lengthAtBirth)/(exp(k*tmax)-1))

		bExpo = lengthAtBirth-exp(aExpo)
	
		L = exp(k*t + aExpo) + bExpo  
	
	}

return(L)
}



##define individuals
# N=100
minPseudo=1
maxPseudo=10
pseudoGrowth<-interpol(rnorm(N),minPseudo,maxPseudo)
pseudoGrowthMean = mean(pseudoGrowth)
pseudoGrowthSd = sd(pseudoGrowth)
pseudoGrowthMin = min(pseudoGrowth)
pseudoGrowthMax = max(pseudoGrowth)

pseudo01<-scale(pseudoGrowth)

##include k-Linf correlation values for Linf limits
minNormalizedPseudoGrowth = (pseudoGrowthMin - pseudoGrowthMean) / pseudoGrowthSd
maxNormalizedPseudoGrowth = (pseudoGrowthMax - pseudoGrowthMean) / pseudoGrowthSd

##truncLeftNorm=-3.5  @Mario: to be changed in code as inputs in species
##truncRightNorm=3.5  @Mario: to be changed in code as inputs in species
##@Mario: this was originating unnecessary out of range values
##@Mario: need to include as this in Weaver
minL = minNormalizedPseudoGrowth # * res_chol[1,2] + (-3.5)*res_chol[2,2]  ##(-3.5) *
maxL = maxNormalizedPseudoGrowth #* res_chol[1,2] + (3.5)*res_chol[2,2]   ##(3.5) * 

# minLinf = minNormalizedPseudoGrowth * res_chol[1,2] + min(rnorm(10000))*res_chol[2,2]  ##(-3.5) *
# maxLinf = maxNormalizedPseudoGrowth * res_chol[1,2] + max(rnorm(10000))*res_chol[2,2]   ##(3.5) * 



             # k    L    devT
sigma=matrix(c(1,corrLk,corrkdevT,   #k
			  corrLk,1,corrLdevT,    #L
			  corrkdevT,corrLdevT,1),nrow=3) #devT

##Cholesky decomposition
# res_chol<-chol(matrix(c(1,corrLk,corrLk,1),nrow=2))	

res_chol<-chol(sigma)

res_chol



v2<-rnorm(N)  ##aux variable to induce correlation
# v3<-rnorm(N)  ##aux variable to induce correlation

###For cor L,k

preL <- pseudo01*res_chol[1,2] + v2*res_chol[2,2]

##To avoid preLinf out of range:@Mario - study to include this everywhere in Weaver
##@Mario. At least include this below
while(sum(preL<minL) > 0 || sum(preL>maxL) > 0)
{

	v2<-rnorm(N)  ##aux variable to induce correlation

	##Cholesky decomposition
	# res_chol<-chol(matrix(c(1,LinfKcorr,LinfKcorr,1),nrow=2))

	##Induce level of correlation - @Mario: include as "preLinf" in code
	preL <- pseudo01*res_chol[1,2] + v2*res_chol[2,2]  ##the matrix is transposed relatively to that in C++

}

##test that the target correlation is close to LinfKcorr
cor(preL,pseudo01)



###For cor k,tmid
v3<-rnorm(N) 

predevT <- pseudo01*res_chol[1,3] + v3*res_chol[3,3]

##To avoid pretmidinf out of range:@Mario - study to include this everywhere in Weaver
##@Mario. At least include this below
while(sum(predevT<minL) > 0 || sum(predevT>maxL) > 0)
{

	v3<-rnorm(N)  ##aux variable to induce correlation

	##Cholesky decomposition
	# res_chol<-chol(matrix(c(1,LinfKcorr,LinfKcorr,1),nrow=2))

	##Induce level of correlation - @Mario: include as "pretmidinf" in code
	predevT <- pseudo01*res_chol[1,3] + v3*res_chol[3,3]  ##the matrix is transposed relatively to that in C++

}

##test that the target correlation is close to LinfKcorr
cor(predevT,pseudo01)

cor(cbind(preL,predevT,pseudo01))


Lmax = interpol2(preL, minL, maxL, LdistanceMin, LdistanceMax)
Lmax



## Why negative correlations produce out of range values
preL  
sum(preL<minL)
sum(preL>maxL)

###Does female length ressemble that of L? need to correct this
Lfemale = calculateDryLength(femaleWetMass/conversionToWetMass, coefficientForMassAforMature, scaleForMassBforMature)
Lfemale

Lmax
Lfemale

##Now, for TRACE document plot all the curves for different levels of LinfKcorr
#

##obtain variability in k values across individuals
# kMin=kMin+kMin*0.15
# kMax=kMax-kMin*0.15
# kMin
# kMax
ks<-interpol2(pseudoGrowth,pseudoGrowthMin,pseudoGrowthMax,kMin,kMax)

##@@At Mario
ks=ks*timeStep

maxtimes<-interpol(predevT,maxTime-devTimer*maxTime,maxTime+devTimer*maxTime) ##interpol(predevR,(1/maxTime)-(1/maxTime)*0.3,(1/maxTime)+(1/maxTime)*0.3)

maxtimes=maxtimes*(1/timeStep)

cor(cbind(preL,maxtimes,pseudo01))

if(fixLengthAtBirth){
	fEggMin<-fEggMax<-0
}

factorEggMassFromMom<-interpol(rnorm(N),fEggMin,fEggMax)

t=seq(0,max(maxtimes)+max(maxtimes)*0.3,step)

plot(t,t,type="n",xlim=c(0,max(t)+1),
ylim=c(0,Lfemale+Lfemale*0.9),ylab="Length_mm")


theLmaxes<-vector()
theKs<-vector()
theMaxt<-vector()
theLmins<-vector()
thets<-vector()
theLtops<-vector()
theLinfs<-vector()


		
for(xx in 1:N){

print(xx)
	
	thisLmax = Lmax[xx]
	k=ks[xx]
	factorEggMass<-factorEggMassFromMom[xx]

	t=seq(0,maxtimes[xx],step)
	
	thisk<-k
				
	##To calculate delta in length for eggDryMass and add it to aLogis (only for Logistic4P)
	preEggDryMass = eggDryMass
	eggDryMass = eggDryMass + eggDryMass*factorEggMass	
	diffEggLength = calculateDryLength(eggDryMass, coefficientForMassA, scaleForMassB) - calculateDryLength(preEggDryMass, coefficientForMassA, scaleForMassB)
	aLogis = aLogis + diffEggLength ##can either increase or decrease aLogis

	thisk=k
	
	thist=max(t)
  
  
	
    t=seq(0,thist,step)
	
	L<-growthCurve(Lmax=thisLmax,k=thisk,t=t,tmax=max(t),eggDryMass = eggDryMass, 
	aLogis = aLogis, growthCurve.type = growthCurve.type)
	lines(t,L,lwd=3,pch=19,col=round(runif(1,min=1,max=20)))
	
	# write.table(data.frame(t,L),"Opti_output.txt")
	
	theLmaxes[xx]=max(L)
	theLmins[xx]=min(L)
	theKs[xx]=thisk
	theMaxt[xx]<-max(t)
	thets[xx]<-thist
	
}


# lastOpti<-read.table("Opti_output.txt",h=T)
# lines(lastOpti[,1],lastOpti[,2],lwd=4,pch=19,col="red")


cor(data.frame(thets,theLmaxes,theKs))



##WARNING: @@Mario, careful about going out of limits... define well
# > range(theLmaxes)
# [1]  8.339408 16.943373
# > 
# > c(LdistanceMin,LdistanceMax)
# [1]  7.220707 16.848317


thePosition = 1

forTempK<-theKs[thePosition]
			
forTempLmax<-theLmaxes[thePosition]


thets[thePosition]


##@@Mario - this parameter because we assume that genetic variation In
##devTime affects equally to all instars
ratioDev = thets[thePosition]/devTimeVector[length(devTimeVector)]


# devTimeVector = devTimeVector*(1/timeStep)

assim = interpol(rnorm(N), assimMin, assimMax)

##@@Debug - have all dev times identical
if(Debug){
  ratioDev = 1 
  thets[1:length(thets)]<-devTimeVector[length(devTimeVector)]
  # assim = interpol(rnorm(N), (assimMin+assimMax)/2, (assimMin+assimMax)/2)
}



devTimeVector<-devTimeVector*ratioDev


###################################################################
###################################################################
###################################################################
###################################################################
###################################################################

#Let's study this and see how it would work:
#First, all the calculateGrowthCurves parafernalia needs to be included here:
##@Mario: define temperature as tempKelvin vs tempCent to make clear when each
##type is used
dells<-function(tempKelvin,  Topt,  ED,  activationEnergy) 
{
	newActivationEnergy = activationEnergy ##trait activation energy
	c = 1
	
	##TODO 270 por qué coge ese valor a veces?

	result = c*exp(-newActivationEnergy/(BOLTZMANN*tempKelvin))/(1+
	exp((-1/(BOLTZMANN*tempKelvin))*(ED-((ED/Topt)+(BOLTZMANN*
	log(newActivationEnergy/(ED-newActivationEnergy))))*tempKelvin)))

	result=result*1000000000000 ##to make numbers more readable: @Mario - include this note

return(result)
}

##@@Mario: include lower and uppper thresholds as an input for each animal species
# lowerThreshold = 0  ##@Parameters: always > 0
# upperThreshold = 35

##@@Mario: include this new function for all uses of Dell
##@@Params: "trait" is the value of the trait at Topt
useDell<-function(newT,actE,lowerThreshold,upperThreshold,Topt,Ed,trait)
{
    dellsNewT = dells(
    		newT,
			Topt,
			Ed,
			actE
			)

    dellsMaxT = dells(
    		Topt,
			Topt,
			Ed,
			actE)

    if(newT > Topt){
	    dellsMinT = dells(
			upperThreshold+273, ##Most extreme temperature
			Topt,
			Ed,
			actE)

	
		postDell = interpol2(
				dellsNewT,
				dellsMinT,
				dellsMaxT,
				0,
				trait)
	}
	
	
	
	
	if(newT <= Topt){
		
		dellsMinT = dells(
			lowerThreshold+273, ##Lowest temperature
			Topt,
			Ed,
			actE)
	
	postDell = interpol2(
    		dellsNewT,
			dellsMinT,
			dellsMaxT,
			0,
			trait)	
	}


return(postDell)
}


optTraitAfterT<-function(traitAtLabTemp,param,optT,optimizing="trait",optimized=NULL)
{

	if(optimizing == "trait"){

		traitAtOptimalT = traitAtLabTemp+traitAtLabTemp*param*abs((optT-273)-tempFromLab)
		
		traitAfterT<-useDell(newT=tempFromLab+273,actE=actE,lowerThreshold, ##warning, use tempFromLab here
		  upperThreshold,Topt=optT,Ed=EdGrowth,trait=traitAtOptimalT)
		
		y = traitAtLabTemp - traitAfterT
		
	}
	
	# if(optimizing == "ED"){
	
		# traitAfterT<-useDell(newT=tempFromLab+273,actE=0.67,lowerThreshold, ##warning, use tempFromLab here
     	  # upperThreshold,Topt=optT,Ed=param,trait=optimized)
		
		# y = traitAtLabTemp - traitAfterT
	
	# }
	
	
return(y)
}


sendDellOptimization<-function(minRange,maxRange,optT,traitLab,inverseTrait = FALSE)
{

	if(inverseTrait){ 

		traitLab=1/traitLab
		
	}

	length.out=10000

	##First round
	captaOpt<-as.data.frame(matrix(nrow=length.out,ncol=3))
	names(captaOpt)<-c("y","param","traitLab")

	pos=1
	for(i in seq(minRange,maxRange,length.out=length.out)){
		
		y<-optTraitAfterT(traitAtLabTemp=traitLab,param=i,optT=optT, optimizing = "trait")
		captaOpt[pos,c("y","param","traitLab")]<-c(y,i,traitLab)
		pos=pos+1

	}

	finalY = captaOpt[which(abs(captaOpt$y) == min(abs(captaOpt$y))),"y"][1]
	optimized = captaOpt[which(abs(captaOpt$y) == min(abs(captaOpt$y))),"param"][1]

	length.out=10000

	##Second round
	captaOpt<-as.data.frame(matrix(nrow=length.out,ncol=3))
	names(captaOpt)<-c("y","param","traitLab")

	pos=1
	for(i in seq(optimized-0.2*optimized,optimized+0.2*optimized,length.out=length.out)){
		
		y<-optTraitAfterT(traitAtLabTemp=traitLab,param=i,optT=optT, optimizing = "trait")
		captaOpt[pos,c("y","param","traitLab")]<-c(y,i,traitLab)
		pos=pos+1

	}

	finalY = captaOpt[which(abs(captaOpt$y) == min(abs(captaOpt$y))),"y"][1]
	optimized = captaOpt[which(abs(captaOpt$y) == min(abs(captaOpt$y))),"param"][1]
	
	
	OptimumTrait = traitLab + traitLab*optimized*abs((tempFromLab + 273) - optT)
	
	
	# ##Third round - ED
	# captaOpt<-as.data.frame(matrix(nrow=length.out,ncol=3))
	# names(captaOpt)<-c("y","param","traitLab")

	# pos=1
	# for(i in seq(1,3.5,length.out=length.out)){
		
		# y<-optTraitAfterT(traitAtLabTemp=traitLab,param=i,optT=optT, optimizing = "ED", optimized = OptimumTrait)
		# captaOpt[pos,c("y","param","traitLab")]<-c(y,i,traitLab)
		# pos=pos+1

	# }

	# finalY2 = captaOpt[which(abs(captaOpt$y) == min(abs(captaOpt$y))),"y"][1]
	# optimized2 = captaOpt[which(abs(captaOpt$y) == min(abs(captaOpt$y))),"param"][1]

	
return(c(optimized,finalY))
}


##@@Mario: this is a new version of calculateGrowthCurves
devTimesLengthMassesTemp<-function(devTimeVector1,devTimeVector2,temperature,
tempSizeRuleConstant,LmaxDev,k,eggDryMass = NULL, aLogis = NULL, growthCurve.type = NULL, 
ToptDev = NULL, actE = actE_met[1])
{

# devTimeVector1<-devTimeVector2<-devTimeVector
# temperature=temperature
# tempSizeRuleConstant=tempSizeRuleConstant
# LmaxDev=forTempLmax
# k=forTempK
# eggDryMass=eggDryMass 
# aLogis=aLogis
# growthCurve.type = growthCurve.type 
# ToptDev = ToptDev


	thisAnimalTempSizeRuleConstant = tempSizeRuleConstant
	
	if(!simulationType %in% "dinosaurs"){
		degreesDifference = abs(temperature - tempFromLab)
	}else{
		degreesDifference = 0 ##for simulations at different temperatures for dinosaurs
	}
	
	if(simulationType %in% "dinosaurs"){
	   thisAnimalTempSizeRuleConstant = 1
	}

	if(temperature > tempFromLab)
	{
		thisAnimalTempSizeRuleConstant = (-thisAnimalTempSizeRuleConstant)
	}

	##Here to calculate the new dev time we need to calculate the mass of the adult after TSR has been applied
	ageLastInstar = devTimeVector1[length(devTimeVector1)]
	
	maxLfromSpeciesLastInstar = LmaxDev 

	massLastInstarForDevT = calculateDryMass(maxLfromSpeciesLastInstar, coefficientForMassAforMature, scaleForMassBforMature)
	
	postTSRMassLastInstarForDevT = massLastInstarForDevT + thisAnimalTempSizeRuleConstant*degreesDifference*massLastInstarForDevT
	
	postTSRLengthLastInstarForDevT<-calculateDryLength(postTSRMassLastInstarForDevT, coefficientForMassAforMature, scaleForMassBforMature)
	
	if(tempDependency){ ##@Mario: include in Weaver
	
	  if(devTimeTempFromGillooly){
	  ##transform to g in wetMass to adjust to the equation by Gillooly et al. 2002 @Mario: change reference from 2012 to 2002
		  massLastInstarForDevTinG = conversionToWetMass * postTSRMassLastInstarForDevT / 1000 #@Mario: change * by / !!
		  
		  devTime = exp(-0.11 * (temperature / (1 + temperature / 273)) + devInter)*(massLastInstarForDevTinG^0.25)
		  
		  finalDevTimeProportion = devTime / ageLastInstar  ##this assumes that elongation or shortening is the same for all instars
	  
	  }else{

		propDecreaseDevTimeFromLabToOptimal<-sendDellOptimization(minRange=0.0001,maxRange=1.0,
		optT=ToptDev,
		traitLab = ageLastInstar, inverseTrait = TRUE)[1]
		
		invAgeLastInstar = 1/ageLastInstar
		
		invAgeAtTopt = invAgeLastInstar + invAgeLastInstar*propDecreaseDevTimeFromLabToOptimal*abs((tempFromLab + 273) - ToptDev)

		inverseDevTime = useDell(newT=temperature+273,actE=actE,lowerThreshold,
			upperThreshold,Topt=ToptDev,Ed=EdDev,trait=invAgeAtTopt) ##@@Params: need own parameters
			
		 finalDevTimeProportion = (1/inverseDevTime) / ageLastInstar  ##this assumes that elongation or shortening is the same for all instars

	  print(c("invAgeLastInstar: ",invAgeLastInstar,"inverseDevTime: ",inverseDevTime, "ageATOpt: ",1/invAgeAtTopt))
	  
	  }
										   
	}else{ 
	
		finalDevTimeProportion = 1 ##Dinosaurs
	
	}
		
			
	if(ageLastInstar == 0)##for the new borns
	{

		factorEggMassFromMom<-rnorm(N)
		factorEggMassFromMom<-interpol(factorEggMassFromMom,fEggMin,fEggMax)

		eggDryMassAtBirth = eggDryMass + eggDryMass * factorEggMassFromMom

		minMassAtCurrentAge = eggDryMassAtBirth - eggDryMassAtBirth*maxPlastBV##Dinosaurs
		maxMassAtCurrentAge = eggDryMassAtBirth + eggDryMassAtBirth*minPlastBV
		eggDryMassAtBirth = min(eggDryMassAtBirth, maxMassAtCurrentAge)
		eggDryMassAtBirth = max(eggDryMassAtBirth, minMassAtCurrentAge)

		tank_ini<-rnorm(N)
		tank_ini<-interpol(tank_ini,tankMin,tankMax)
			
		energy_tank =  tank_ini * eggDryMassAtBirth^betaScaleTank
		currentBodySize = eggDryMassAtBirth - energy_tank
		
	}


	##Now we calculate the vector of Ls using Linf and von Bertalanfy function on each age
	##And we use each L as the next_L in the growth equations to calculate the next_M
	finalDevTimeVector<-vector()
	lengthsVector<-vector()
	massesVector<-vector()

	numberOfInstars = length(devTimeVector2)+1
		

	if(tempDependency){ 
	
		##@@Mario: Apply here optimization to calculate the new Linf (as for genetic correlations above)
		thisLmax<-postTSRLengthLastInstarForDevT
		# tmax<-devTimeVector2[length(devTimeVector2)]*finalDevTimeProportion

		
	}else{
	
	  thisLmax = LmaxDev
	
	}
    
	tmax = devTimeVector2[length(devTimeVector2)]*finalDevTimeProportion
	

	##This generates the vectors for only the very first animal
	for (i in 1:(numberOfInstars-1))
	{
		
		finalDevTimeVector[i] = devTimeVector2[i]*finalDevTimeProportion
		
		lengthsVector[i] = growthCurve(Lmax=thisLmax,k=k,t=finalDevTimeVector[i],tmax=tmax,
		eggDryMass = eggDryMass, aLogis = aLogis, growthCurve.type = growthCurve.type)
			
		if(i == numberOfInstars-1){ ##is the last dev time in the vector - only for beetles, etc.
			massesVector[i] = calculateDryMass(lengthsVector[i], coefficientForMassAforMature, scaleForMassBforMature)
		}else{
			massesVector[i] = calculateDryMass(lengthsVector[i], coefficientForMassA, scaleForMassB)
		}

		##TSR is applied directly here so the k value is effectively changed at the individual level
		massesVector[i] = massesVector[i] + massesVector[i]*thisAnimalTempSizeRuleConstant*degreesDifference

        ##@Mario: this needs to be done in Weaver - return the TSR effects on lengths
		if(i == numberOfInstars-1){ ##is the last dev time in the vector - only for beetles, etc.
			lengthsVector[i] = calculateDryLength(massesVector[i], coefficientForMassAforMature, scaleForMassBforMature)
		}else{
			lengthsVector[i] = calculateDryLength(massesVector[i], coefficientForMassA, scaleForMassB)
		}

	}

	#@Mario: remove this from the code
	# for (i in 1:(numberOfInstars-1)) 
	# {
		# finalDevTimeVector[i] = devTimeVector*finalDevTimeProportion
	# }

	#this works but is not necessary - we finally leave it in Weaver
	instarFirstReproduction = find_number_in_json("instarFirstReproduction",anim) 

return(list(finalDevTimeVector=finalDevTimeVector,massesVector=massesVector,lengthsVector=lengthsVector,finalDevTimeProportion=finalDevTimeProportion))
}

# newT = 273 + temperature
##Optimo para las curvas de crecimiento=35. Para Vor Speed Search será 25
ToptVoracity = 273 + tempOptVoracity
ToptSearch = 273 + tempOptSearch
ToptSpeed = 273 + tempOptSpeed
ToptGrowth = 273 + tempOptGrowth
ToptDev = 273 + tempOptDev

bMet<-interpol(rnorm(N),bMetMin,bMetMax)
actE_met<-interpol(rnorm(N),actE_metMin,actE_metMax)
actE_vor<-0.665
actE_speed<-0.665
actE_search<-0.665
BOLTZMANN = 8.62E-5

vectorList<-devTimesLengthMassesTemp(devTimeVector1=devTimeVector,
devTimeVector2=devTimeVector,temperature,tempSizeRuleConstant,
LmaxDev=forTempLmax,k=forTempK,eggDryMass = eggDryMass, 
aLogis = aLogis, growthCurve.type = growthCurve.type, ToptDev = ToptDev, actE = actE_met[1])
vectorList$finalDevTimeVector

plot(vectorList[[1]],vectorList[[3]],pch=16,type="n",lwd=2,
xlim=c(0,theMaxt[thePosition]),ylim=c(0,forTempLmax))

lines(vectorList[[1]],vectorList[[3]],col="red",lwd=2)

lines(devTimeVector,growthCurve(Lmax=forTempLmax,k=forTempK,t=devTimeVector,tmax=max(devTimeVector),eggDryMass = eggDryMass, aLogis = aLogis, growthCurve.type = growthCurve.type),
lty=2,lwd=2,col="purple")


vectorList2<-devTimesLengthMassesTemp(devTimeVector1=devTimeVector,
devTimeVector2=devTimeVector,temperature,tempSizeRuleConstant,
LmaxDev=forTempLmax,k=forTempK,eggDryMass = eggDryMass, aLogis = aLogis, 
growthCurve.type = growthCurve.type, ToptDev = ToptDev, actE = actE_met[1])


lines(vectorList2[[1]],vectorList2[[3]],pch=16,col="red",lwd=2,lty=2)



##Next, then, continue with tuneTraits and Dells
##################################################
##################################################
##################################################
##################################################
if(Debug == TRUE){

bMetMin=(bMetMin+bMetMax)/2
bMetMax=bMetMin

actE_metMin=(actE_metMin+actE_metMax)/2
actE_metMax=actE_metMin

actE_vorMin=(actE_vorMin+actE_vorMax)/2
actE_vorMax=actE_vorMin

actE_speedMin=(actE_speedMin+actE_speedMax)/2
actE_speedMax=actE_speedMin

actE_searchMin=(actE_searchMin+actE_searchMax)/2
actE_searchMax=actE_searchMin


}

bMet<-interpol(rnorm(N),bMetMin,bMetMax)
actE_met<-interpol(rnorm(N),actE_metMin,actE_metMax)
actE_vor<-interpol(rnorm(N),actE_vorMin,actE_vorMax)
actE_speed<-interpol(rnorm(N),actE_speedMin,actE_speedMax)
actE_search<-interpol(rnorm(N),actE_searchMin,actE_searchMax)
BOLTZMANN = 8.62E-5
getTotalMetabolicDryMassLoss<-function(wetMass, bMet, actE_met, actE_search, temperature, proportionOfTimeTheAnimalWasMoving, search_area, simulationType)
{
	 totalMetabolicDryMassLoss = 0

	if(simulationType %in% "dinosaurs") {
	
			##here Grady et al. 2014 provide results in Watts (j/s) and M in g
			basalMetabolicTax = 0.002*(wetMass*1000)^bMet[1]

			#@Mario: remove this to publish this version, or search the literature for more info
			# fraction_with_stress = (Math_Functions::linearInterpolate(todayEncountersWithPredators, 0, getSpecies()->getMaxEncountersT(), 0, 0.407)/timeStepsPerDay)*basalMetabolicTax

			##TODO multiplicar por timeStep
			##24*3600 because the metab rates are given in J/s.
			basalMetabolicTax = basalMetabolicTax ##+ fraction_with_stress
			
			distanceMoved = proportionOfTimeTheAnimalWasMoving*search_area  ##km/day
			
			speedOfTravel = distanceMoved*(1000/(3600*24))  ##in m/s 
			
			field_met_tax = speedOfTravel*(10.7*wetMass^0.68) ##Calder 1996 in Ruxton and Hustonfor mammals - Jouls
			
			##field_met_tax = 0
			##Remove this and use cost of transport in Calders
			## loss_from_bmr = (1-proportionOfTimeTheAnimalWasMoving)*basalMetabolicTax*24*3600
			##TODO This 3 is a raw value from bibl.
			## field_met_tax = 3*basalMetabolicTax
			## loss_from_fmr = proportionOfTimeTheAnimalWasMoving*field_met_tax*24*3600

			loss_from_bmr = basalMetabolicTax*24*3600
			loss_from_fmr = field_met_tax

			##7 is NOT referred to weeks. Conversion from Jules.
			loss_from_bmr=loss_from_bmr/7##7 joule = 1mg 
			loss_from_fmr=loss_from_fmr/7
			##here we transform from mg to Kg to dinoWeaver            ##divided by 1000000 to change mg to kg
			totalMetabolicDryMassLoss = ((loss_from_bmr + loss_from_fmr)*0.000001) / conversionToWetMass
			totalMetabolicDryMassLoss=totalMetabolicDryMassLoss*timeStep
			
		}else{
				# # @Mario: change BOLZMANN to BOLTZMANN										
			 if(!tempDependency){ 
			   temperature = tempFromLab  ##this should only affect locally inthe function not globally
			 }
			 
			 basalMetabolicTax = exp(-7.2945+43.966*actE_met+bMet*log(wetMass)-actE_met*(1/((temperature+273.15)*BOLTZMANN)))
             #@Mario: remove stress or Jordi adds biblio
			 # fraction_with_stress = (Math_Functions::linearInterpolate(todayEncountersWithPredators, 0, getSpecies()->getMaxEncountersT(), 0, 0.407)/timeStepsPerDay)*basalMetabolicTax

			
			##This correction for smaller animals produces metabolic rates as small as 4 times less
			##print(c("before :",basalMetabolicTax))
			
			if(wetMass < 5.2){
				basalMetabolicTax = exp(-2.605722)*basalMetabolicTax^0.7961851 
			}
			##print(c("after :",basalMetabolicTax))

			##TODO multiplicar por timeStep
			##24 because the metab rates are given in days.
			basalMetabolicTax = basalMetabolicTax #+ fraction_with_stress
			 loss_from_bmr = (1-proportionOfTimeTheAnimalWasMoving)*basalMetabolicTax*24
			##TODO This 3 is a raw value from bibl.
			 field_met_tax = multiplierForFieldMetabolicRate*basalMetabolicTax
			 loss_from_fmr = proportionOfTimeTheAnimalWasMoving*field_met_tax*24

			##7 is NOT referred to weeks. Conversion from Jules.
			loss_from_bmr=loss_from_bmr/7
			loss_from_fmr=loss_from_fmr/7
			totalMetabolicDryMassLoss = (loss_from_bmr + loss_from_fmr) / conversionToWetMass
			totalMetabolicDryMassLoss=totalMetabolicDryMassLoss*timeStep
		
	    }
	

return(totalMetabolicDryMassLoss)
}



applyTSR<-function(t,tmax,kOpt, thisMature)
{
	
	thisAnimalTempSizeRuleConstant = tempSizeRuleConstant

	if(temperature > tempFromLab)
	{
	   thisAnimalTempSizeRuleConstant = (-thisAnimalTempSizeRuleConstant)
	}
	
	degreesDifference = abs(temperature - tempFromLab)


	if(tempDependency){

		midMass = calculateDryMass(forTempLmax, coefficientForMassAforMature, scaleForMassBforMature)

		##TSR is applied directly here so the k value is effectively changed at the individual level
		midMass = midMass + midMass*thisAnimalTempSizeRuleConstant*degreesDifference

		thisLmax = calculateDryLength(midMass, coefficientForMassAforMature, scaleForMassBforMature)

		kDell<-useDell(newT=temperature+273,actE=actE,lowerThreshold,
		upperThreshold,Topt=ToptGrowth,Ed=EdVoracity,trait=kOpt)  ##this kOpt has been previously obtained with tempDependency

	    preLength = growthCurve(Lmax=thisLmax,k=kDell,t,tmax=tmax,eggDryMass = eggDryMass, aLogis = aLogis, growthCurve.type = growthCurve.type)

	}else{
	
	   degreesDifference = 0
	   
	   thisAnimalTempSizeRuleConstant = 1
	
	   preLength = growthCurve(Lmax=forTempLmax,k=kOpt,t,tmax=tmax,eggDryMass = eggDryMass, aLogis = aLogis, growthCurve.type = growthCurve.type)
	
	}

	
	if(thisMature){
		newMass = calculateDryMass(preLength, coefficientForMassAforMature, scaleForMassBforMature)
	}
	else{
		newMass = calculateDryMass(preLength, coefficientForMassA, scaleForMassB)
	}
	
	##TSR is applied directly here so the k value is effectively changed at the individual level
	newMass = newMass + newMass*thisAnimalTempSizeRuleConstant*degreesDifference

	if(thisMature){
		newLength = calculateDryLength(newMass, coefficientForMassAforMature, scaleForMassBforMature)
	}
	else{
		newLength = calculateDryLength(newMass, coefficientForMassA, scaleForMassB)
	}

return(c(newMass=newMass,newLength=newLength))
}

viabilityPlot<-function(timePlot,currentAge,currentMasses,netGrowth)
{

	par(mfrow=c(1,3))

	if(type==1){
		plot(timePlot$currentAge,currentMasses,type="l",lwd=3,
		xlim=c(0,5),ylim=c(min(currentMasses)-0.1*min(currentMasses),
		min(currentMasses)+15*min(currentMasses)))
	}else{
		plot(timePlot$currentAge,currentMasses,type="l",lwd=3,
		xlim=c(0,5),ylim=c(min(currentMasses)-0.1*min(currentMasses),
		min(currentMasses)+1*min(currentMasses)))
	}
 
	# lines(timePlot$currentAge,currentMasses-timePlot$maxMetabolicDryMassLoss,lwd=3,col="black",lty=2)
	lines(timePlot$currentAge,timePlot$nextMassPredicted,lwd=3,col="green")
	# lines(timePlot$currentAge,timePlot$maxNextDinoMassPredicted,lwd=3,col="red")
	# lines(timePlot$currentAge,timePlot$finalVoracity+timePlot$currentMasses,lwd=3,col="blue")
	# lines(timePlot$currentAge,timePlot$vorAfterEncounters+timePlot$currentMasses,lwd=3,lty=2,col="purple")
	lines(timePlot$currentAge,netGrowth,lwd=3,lty=2,col="orange")


    if(type==1){
		plot(timePlot$currentAge,currentMasses,type="l",lwd=3,
		xlim=c(currentAge[(length(currentAge)-5)],currentAge[length(currentAge)]),
		ylim=c(currentMasses[length(currentMasses)]-0.1*currentMasses[length(currentMasses)],
		currentMasses[length(currentMasses)]+3*currentMasses[length(currentMasses)]))
	}else{
		plot(timePlot$currentAge,currentMasses,type="l",lwd=3,
		xlim=c(currentAge[(length(currentAge)-35)],currentAge[length(currentAge)]),
		ylim=c(currentMasses[length(currentMasses)]-0.2*currentMasses[length(currentMasses)],
		currentMasses[length(currentMasses)]+0.5*currentMasses[length(currentMasses)]))
	}
	
	# lines(timePlot$currentAge,currentMasses-timePlot$maxMetabolicDryMassLoss,lwd=3,col="black",lty=2)
	lines(timePlot$currentAge,timePlot$nextMassPredicted,lwd=3,col="green")
	# lines(timePlot$currentAge,timePlot$maxNextDinoMassPredicted,lwd=3,col="red")
	# lines(timePlot$currentAge,timePlot$finalVoracity+timePlot$currentMasses,lwd=3,col="blue")
	# lines(timePlot$currentAge,timePlot$vorAfterEncounters+timePlot$currentMasses,lwd=3,lty=2,col="purple")
	lines(timePlot$currentAge,netGrowth,lwd=3,lty=2,col="orange")

	plot(timePlot$currentAge,currentMasses,type="l",lwd=3,
	ylim=c(0,max(timePlot$maxNextDinoMassPredicted,na.rm=T)))

	# plot(timePlot$currentAge,timePlot$currentMasses,type="l",lwd=3,xlim=c(80,110),
	# ylim=c(0.007,0.012))



	# lines(timePlot$currentAge,currentMasses-timePlot$maxMetabolicDryMassLoss,lwd=3,col="black",lty=2)
	lines(timePlot$currentAge,timePlot$nextMassPredicted,lwd=3,col="green")
	# lines(timePlot$currentAge,timePlot$maxNextDinoMassPredicted,lwd=3,col="red")
	# lines(timePlot$currentAge,timePlot$finalVoracity+timePlot$currentMasses,lwd=3,col="blue")
	# lines(timePlot$currentAge,timePlot$vorAfterEncounters+timePlot$currentMasses,lwd=3,lty=2,col="purple")
	lines(timePlot$currentAge,netGrowth,lwd=3,lty=2,col="orange")



if(type==2){

	
	par(mfrow=c(1,3))
	plot(timePlot$currentAge,currentMasses,type="l",lwd=3,
	xlim=c(0,5),ylim=c(min(currentMasses),min(currentMasses)+0.2*min(currentMasses)))

	# lines(timePlot$currentAge,currentMasses-timePlot$maxMetabolicDryMassLoss,lwd=3,col="black",lty=2)
	lines(timePlot$currentAge,timePlot$nextMassPredicted,lwd=3,col="green")
	# lines(timePlot$currentAge,timePlot$maxNextDinoMassPredicted,lwd=3,col="red")
	# lines(timePlot$currentAge,timePlot$finalVoracity+timePlot$currentMasses,lwd=3,col="blue")
	# lines(timePlot$currentAge,timePlot$vorAfterEncounters+timePlot$currentMasses,lwd=3,lty=2,col="purple")
	lines(timePlot$currentAge,netGrowth,lwd=3,lty=2,col="orange")

	plot(timePlot$currentAge,currentMasses,type="l",lwd=3,
	xlim=c(currentAge[(length(currentAge)-6000)],currentAge[length(currentAge)]),
	ylim=c(currentMasses[length(currentMasses)]-0.1*currentMasses[length(currentMasses)],
	currentMasses[length(currentMasses)]+0.02*currentMasses[length(currentMasses)]))

	# lines(timePlot$currentAge,currentMasses-timePlot$maxMetabolicDryMassLoss,lwd=3,col="black",lty=2)
	lines(timePlot$currentAge,timePlot$nextMassPredicted,lwd=3,col="green")
	# lines(timePlot$currentAge,timePlot$maxNextDinoMassPredicted,lwd=3,col="red")
	# lines(timePlot$currentAge,timePlot$finalVoracity+timePlot$currentMasses,lwd=3,col="blue")
	# lines(timePlot$currentAge,timePlot$vorAfterEncounters+timePlot$currentMasses,lwd=3,lty=2,col="purple")
	lines(timePlot$currentAge,netGrowth,lwd=3,lty=2,col="orange")

# timePlot$finalVoracity[nrow(timePlot)]+timePlot$currentMasses[nrow(timePlot)]

	# Garland1983 = ((152*timePlot$wetMasses^0.738)/1000)/conversionToWetMass

	plot(timePlot$currentAge,currentMasses,type="l",lwd=3,
	ylim=c(0,max(timePlot$maxNextDinoMassPredicted,na.rm=T)))
	
	# plot(timePlot$currentAge,timePlot$currentMasses,type="l",lwd=3,xlim=c(0,800),
	# ylim=c(0,6))



	# lines(timePlot$currentAge,currentMasses-timePlot$maxMetabolicDryMassLoss,lwd=3,col="black",lty=2)
	lines(timePlot$currentAge,timePlot$nextMassPredicted,lwd=3,col="green")
	# lines(timePlot$currentAge,timePlot$maxNextDinoMassPredicted,lwd=3,col="red")
	# lines(timePlot$currentAge,timePlot$finalVoracity+timePlot$currentMasses,lwd=3,col="blue")
	# lines(timePlot$currentAge,timePlot$vorAfterEncounters+timePlot$currentMasses,lwd=3,lty=2,col="purple")
	lines(timePlot$currentAge,netGrowth,lwd=3,lty=2,col="orange")
	
}

# par(mfrow=c(1,2))

# plot(timePlot$currentMasses,timePlot$finalVoracity)
# plot(timePlot$currentMasses,timePlot$vorAfterEncounters)

par(mfrow=c(1,1))	


}



##New trait below, the logics is that k is estimated at tempFromLab, which may be
##far away from optimal temperature. The fraction are arbitrary, so better have
##documented growth rates at optimal temperature to guess these two traits
##These are fractions of k/degree Celsius to add to k until reaching the k at optimum T
##Here is Option 1)... See Option 2) below

actE_growthMin=0.67
actE_growthMax=0.67 ##@@Mario: Nuevo trait
actE_growth<-interpol(rnorm(N),actE_growthMin,actE_growthMax)

eggDevTime<-interpol(rnorm(N),eggDevTimeMin,eggDevTimeMax)


# pupaPeriodTime=1

if(tempDependency){  ##dependency of pupaPeriodTime on temperature
	
	propDecreasePupaDevRate<-sendDellOptimization(minRange=pupaPeriodTime - 0.99*pupaPeriodTime,maxRange=pupaPeriodTime + 0.99*pupaPeriodTime,
	optT=ToptSearch,
	traitLab = pupaPeriodTime, inverseTrait = TRUE)##[1]
	
	invPupaPeriodLength = 1/pupaPeriodTime
	
	invPupaPeriodLengthOptT = invPupaPeriodLength+invPupaPeriodLength*propDecreasePupaDevRate[1]*abs((tempFromLab + 273)-ToptSearch)
	
	inversePupaPeriodLength = useDell(newT=temperature+273,actE=actE,lowerThreshold,
	  upperThreshold,Topt=ToptSearch,Ed=EdDev,trait=invPupaPeriodLengthOptT) ##@@Params: need own parameters
	  
	  pupaPeriodTime = 1/inversePupaPeriodLength


	propDecreaseEggDevRate<-sendDellOptimization(minRange=eggDevTime[1] - 0.99*eggDevTime[1],maxRange=eggDevTime[1] + 0.99*eggDevTime[1],
	optT=ToptSearch,
	traitLab = eggDevTime[1], inverseTrait = TRUE)##[1]
	
	invEggDevTime = 1/eggDevTime[1]
	
	invEggDevTimeOptT = invEggDevTime+invEggDevTime*propDecreaseEggDevRate[1]*abs((tempFromLab + 273)-ToptSearch)
	
	inverseEggDevTime = useDell(newT=temperature+273,actE=actE,lowerThreshold,
	  upperThreshold,Topt=ToptSearch,Ed=EdDev,trait=invEggDevTimeOptT) ##@@Params: need own parameters
	  
	  eggDevTime[1] = 1/inverseEggDevTime

}

# propDecreasePupaDevRate
# pupaPeriodTime


tryParams<-function(subCombs)
{

	# # subCombs<-subCombs[1,]

	 if(type == 2){
		 # newA = subCombs[,1]  ##152
		 newA = 152
		 h_enhancement = subCombs[,1] ##0.02  ##horizontal parabole at 0.0001
	 }

	 if(type %in% c(1,3,4,5)){
		newAJuv = subCombs[,1]
		newAdult = subCombs[,2]
	 }

	# vorMin = subCombs[,3]
	# vorMax = vorMin
	voracity_ini = interpol(rnorm(N), vorMin, vorMax)
	
	search_area_ini = interpol(rnorm(N), search_areaMin, search_areaMax)
	speed_ini = interpol(rnorm(N), speedMin, speedMax)
	
	# decreaseOnTraitsDueToEncounters = subCombs[,3]
	
	if(tempDependency){
		propChangeVor<-sendDellOptimization(minRange= voracity_ini[1] - 0.99* voracity_ini[1],maxRange= voracity_ini[1] + 0.99* voracity_ini[1],
			optT=ToptVoracity,
			traitLab = voracity_ini[1], inverseTrait = FALSE)##[1]
				
		vorAtOptT = voracity_ini[1] + voracity_ini[1]*propChangeVor[1]*abs((ToptVoracity-273)-tempFromLab)
	}
	
	
	# rm(subCombs)


##Do not add age to aquatic fauna or other holometabolic insects

t<-thets[thePosition]
currentAge = 0:max(t)


kBef=forTempK
kBef
# forTempLinf
ageBef=currentAge


before<-devTimesLengthMassesTemp(devTimeVector1=devTimeVector,
devTimeVector2=devTimeVector,temperature=tempFromLab,tempSizeRuleConstant,
LmaxDev=forTempLmax,k=kBef,eggDryMass = eggDryMass, aLogis = aLogis, 
growthCurve.type = growthCurve.type, ToptDev = ToptDev)
	
lines(before$finalDevTimeVector,before$lengthsVector,lwd=3,col="red",lty=2)

if(tempDependency){

	growthTempPlastFinal<-sendDellOptimization(minRange=0.001,maxRange=1.0,optT=ToptGrowth,
	traitLab = forTempK, inverseTrait = FALSE)[1]
	
	kOpt = forTempK+forTempK*growthTempPlastFinal*abs(tempOptGrowth-tempFromLab)
		
	kDell<-useDell(newT=temperature+273,actE=actE,lowerThreshold,
		  upperThreshold,Topt=ToptGrowth,Ed=EdGrowth,trait=kOpt)
	

	
  	vectors<-devTimesLengthMassesTemp(devTimeVector1=devTimeVector,
	devTimeVector2=currentAge,temperature,tempSizeRuleConstant,
	LmaxDev=forTempLmax,k=kDell,eggDryMass = eggDryMass, aLogis = aLogis, 
    growthCurve.type = growthCurve.type, ToptDev = ToptDev)

	##These are vectors with temp modifications but in instars instead of days

}else{
	vectorsForCalc<-before
}


# print(c("forTempK: ", forTempK,"kBef: ", kBef,"kDell: ",kDell, "kOpt: ", kOpt))

if(!tempDependency){
 
 # temperature = 21 ##defaults for dinosaurs

	vectors<-devTimesLengthMassesTemp(devTimeVector1=devTimeVector,
	devTimeVector2=currentAge,temperature,tempSizeRuleConstant,
	LmaxDev=forTempLmax,k=forTempK,eggDryMass = eggDryMass, aLogis = aLogis, 
    growthCurve.type = growthCurve.type, ToptDev = ToptDev)
}


##Need to sum 0
sum(vectors$finalDevTimeVector<currentAge)

# which(vectors$finalDevTimeVector==currentAge)


###this includes only juveniles, or also adults for indeterminate growth
massesVector<-vectors$massesVector
lengthsVector<-vectors$lengthsVector
finalDevTimeVector<-vectors$finalDevTimeVector
currentAge<-vectors$finalDevTimeVector   ###update currentAge without rounding (there is now variation in devTimeVector across individuals and with temp)
finalDevTimeProportion=vectors$finalDevTimeProportion

# print(max(currentAge))
# stopifnot(exists("whatever"))


# vectors$massesVector[length(vectors$massesVector)]*conversionToWetMass


lines(finalDevTimeVector,lengthsVector,lwd=3,col="red")



   ##Dinosaurs currentAge = ((double)(timeStep-diapauseTimeSteps)/(double)timeStepsPerDay) - traits[Trait::pheno] + 1.0/timeStepsPerDay;

	##Jordi starts here::::
    ##forgot to declare this: Important to do it outside the if else..
    iniCurrentInstarMass = 0.0
    targetNextInstarMass = 0.0
    minTotalMetabolicDryMassLoss = 0.0
    finalJMinVB = 0.0
    finalVoracity = 0.0
    postTsearch = 0.0
	maxPostTsearch = 0.0					  
    postTspeed = 0.0
    finalSearch = 0.0
    finalSpeed = 0.0
	maxNextInstarMassFromVBPlasticity = 0.0		
	lengthAtBirth = 0
	xmid = 0						 

    ##minMassAtCurrentAge = currentBodySize + currentBodySize * getSpecies()->getMinPlasticityKVonBertalanffy();
	lengthAtBirth = calculateDryLength(eggDryMass, coefficientForMassAforMature, scaleForMassBforMature)
		
	# asymptoticSize = forTempLinf

	capta_all<-as.data.frame(matrix(nrow=length(currentAge),ncol=38))

	names(capta_all)<-c("currentAge",
	"dinoLengthPredicted",
	"dinoMassPredicted",
	"minMassAtCurrentAge",
	"mature",
	"nextDinoLengthPredicted",
	"nextMassPredicted",
	"massForNextReproduction",
	"ageForNextReproduction",
	"currentLength",
	"currentDryMass",
	"currentWetMass",
	"mr",
	"targetMasses",
	"voracity_ini",
	"postTvor",
	"preFinalVoracity",
	"finalVoracity",
	"postTsearch",
	"postTspeed",
	"finalSearch",
	"finalSpeed",
	"vorAfterEncounters",
	"searchAfterEncounters",
	"vorAfterGarland",
	"vorAfterYodzis",
	"maxMetabolicDryMassLoss","h","k","eggsPerBatch","reproCounter","netGrowth",
	"beyondMax","deficit","clutchMass","capitalBreeding","beyondCurve","predVor")


if(tempDependency){
	capta_all$k<-kDell
}else{
    capta_all$k<-forTempK
}

# currentAge

nextDinoLengthPredicted = vector() 
nextMassPredicted = vector()
massForNextReproduction=vector()
ageForNextReproduction=NA
currentDryMass<-vector()

slopeTarget = 0.0
interceptTarget = 0.0

mature<-vector()



if(tempDependency){

	forAgeFirstMatTemp<-devTimesLengthMassesTemp(devTimeVector1=devTimeVector,
		devTimeVector2=devTimeVector,temperature,tempSizeRuleConstant,
		LmaxDev=forTempLmax,k=kDell,eggDryMass = eggDryMass, aLogis = aLogis, 
    growthCurve.type = growthCurve.type, ToptDev = ToptDev)[[1]]
		
	if(indeterminateGrowth){
		ageOfFirstMaturation = forAgeFirstMatTemp[instarFirstReproduction-1]+pupaPeriodTime
	}else{
	    ageOfFirstMaturation = forAgeFirstMatTemp[length(forAgeFirstMatTemp)]+pupaPeriodTime
	}
	# ageOfFirstMaturation
}else{
	# forAgeFirstMatTemp<-devTimesLengthMassesTemp(devTimeVector1=devTimeVector,
		# devTimeVector2=devTimeVector,temperature,tempSizeRuleConstant,
		# forTempLinf,forTempK)[[1]]
	if(indeterminateGrowth){
		ageOfFirstMaturation = devTimeVector[instarFirstReproduction-1]+pupaPeriodTime
	}else{
	    ageOfFirstMaturation = devTimeVector[length(devTimeVector)]+pupaPeriodTime
	}
# ageOfFirstMaturation
}

##to test that longevity decreases at optimal temperature
longevity = round(longevitySinceMaturation*ageOfFirstMaturation)     ##longevitySinceMaturation)

# print(longevity)
# stopifnot(exists("whatever"))


if(!indeterminateGrowth){

	tmax=max(currentAge) ##apply growth parameters only until here
	
	# # iniMass<-massesVector[length(massesVector)]

	adultAge<-(currentAge[length(currentAge)]+1):longevity  
	currentAge<-c(currentAge,adultAge)
	
	# vectors<-devTimesLengthMassesTemp(devTimeVector1=devTimeVector,
	# devTimeVector2=currentAge,temperature,tempSizeRuleConstant,
	# forTempLinf,forTempK)
		
	# massesVector<-vectors$massesVector
	# lengthsVector<-vectors$lengthsVector
	# finalDevTimeVector<-vectors$finalDevTimeVector
	
	toAdd<-as.data.frame(matrix(nrow=length(adultAge),ncol=ncol(capta_all)))
	names(toAdd)<-names(capta_all)
	capta_all<-rbind(capta_all,toAdd)

}else{##if indeterminate growth add days if it lives longer than finalDevTimeVector

	
	# propAdultMass = vectorsForCalc$massesVector[instarFirstReproduction-1]/vectorsForCalc$massesVector[length(vectorsForCalc$massesVector)]
	
	if(longevity > currentAge[length(currentAge)]){
	
		tmax=max(currentAge)  ##apply growth parameters only until here
	
		adultAge<-(currentAge[length(currentAge)]+1):longevity 
		currentAge<-c(currentAge,adultAge)
		
		##This is because dinosaurs never have indeterminate growth
		##need to recalculate all vectors as the vector currentAge is now longer
	    # if(tempDependency){
			# vectors<-devTimesLengthMassesTemp(devTimeVector1=devTimeVector,
			# devTimeVector2=currentAge,temperature,tempSizeRuleConstant,
			# forTempLinf,kDell)
		# }else{
			# vectors<-devTimesLengthMassesTemp(devTimeVector1=devTimeVector,
			# devTimeVector2=currentAge,temperature,tempSizeRuleConstant,
			# forTempLinf,forTempK)
		# }
		
		# massesVector<-vectors$massesVector
		# lengthsVector<-vectors$lengthsVector
		# finalDevTimeVector<-vectors$finalDevTimeVector
		
		toAdd<-as.data.frame(matrix(nrow=length(adultAge),ncol=ncol(capta_all)))
	    names(toAdd)<-names(capta_all)
	    capta_all<-rbind(capta_all,toAdd)

	}
}

# currentAge<-round(currentAge)


##@@Mario: this is Animal::tuneTraits

capta_all$currentAge<-currentAge

	##death criterion fixed to the true plasticity bands - Dinosaurs
	# ##arthro

	##Forcing continuous growth in Dinosaurs - warning this involves heavy investment in growth - reproduction?
   	##@Mario: remove these dino prefixes, now everybody does the same thing
	dinoLengthPredicted<-vector()
	
	for(i in 1:length(currentAge)){
		 dinoLengthPredicted[i] = lengthsVector[i]##growthCurve(Linf=asymptoticSize,k=forTempK,t=finalDevTimeVector[i])
	}

	##@Mario: include calculateDryLength() - the reverse
	dinoMassPredicted = calculateDryMass(dinoLengthPredicted, coefficientForMassA, scaleForMassB)

capta_all$dinoLengthPredicted<-dinoLengthPredicted
capta_all$dinoMassPredicted<-dinoMassPredicted

	
	minMassAtCurrentAge<-vector()

	#@Check: this below is working anymore?
	##this below is for the cases of death from starvation - so it depends on the rule
	for(i in 1:length(currentAge)){
		if (currentAge[i]>(0.15*longevity)){ ##arthropod to prevent the tiny animals from dying too soon...
		 minMassAtCurrentAge[i] = dinoMassPredicted[i] - dinoMassPredicted[i] * minPlasticityKVonBertalanffy
		}else{
		 minMassAtCurrentAge[i] = dinoMassPredicted[i] - dinoMassPredicted[i] * 3 * minPlasticityKVonBertalanffy
		}
	}


capta_all$minMassAtCurrentAge<-minMassAtCurrentAge


for(i in 1:length(currentAge)){	
  if(currentAge[i] >= ageOfFirstMaturation){  ##round because it fails sometimes
	capta_all$mature[i]<-TRUE
  }else{
    capta_all$mature[i]<-FALSE
  }
}


##timeAddedToMeetLastRepro	- @@Mario: speciesLevelTrait - is to allow the tail of the last reproduction to occur 											 
if(!capitalBreeding){  

	timeOfReproEvent = ((longevity - ageOfFirstMaturation)/femaleMaxReproductionEvents)-((timeAddedToMeetLastRepro)*(1/timeStep))

}else{

##@@Jordi: 27032024 - need to remove pupaPeriodTime from both equations here, the do nothing.

	totalTimeBreedingCapitally = timeOfReproEventDuringCapitalBreeding*(numberOfCapitalBreeds-1) + pupaPeriodTime ##because the first one is on the pupaPerdiodLength
	timeOfReproEvent = ((longevity - totalTimeBreedingCapitally - ageOfFirstMaturation-pupaPeriodTime)/(femaleMaxReproductionEvents-numberOfCapitalBreeds))-((timeAddedToMeetLastRepro)*(1/timeStep))

}


if(tempDependency){ 

	proportionDecreaseInVor<-sendDellOptimization(minRange=0.001,maxRange=1.0,optT=ToptVoracity,
	traitLab = voracity_ini[1], inverseTrait = FALSE)[1]
	
	vorAtOptT = voracity_ini[1]  + voracity_ini[1]*proportionDecreaseInVor*abs(tempOptVoracity-tempFromLab)

	##@@Jordi @@Mario - Optimize trait for Johnson and Levis
	##that is, calculate search at Topt
	proportionDecreaseInSearch<-sendDellOptimization(minRange=0.001,maxRange=1.0,optT=ToptSearch,
	traitLab = search_area_ini[1], inverseTrait = FALSE)[1]
	
	searchAtOptT = search_area_ini[1]  + search_area_ini[1]*proportionDecreaseInSearch*abs(tempOptSearch-tempFromLab)

	
	##@@Jordi @@Mario - Optimize trait for Johnson and Levis
	##that is, calculate search at Topt
			
	proportionDecreaseInSpeed<-sendDellOptimization(minRange=0.001,maxRange=1.0,optT=ToptSpeed,
	traitLab = speed_ini[1], inverseTrait = FALSE)[1]
	
	speedAtOptT = speed_ini[1]  + speed_ini[1]*proportionDecreaseInSpeed*abs(tempOptSpeed-tempFromLab)

}



currentEggs = 0

reproCounter=0

eggs<-vector()

capitalBreedingVector<-vector()

capta_all$capitalBreeding = capitalBreeding 


turnoff = FALSE

capta_all$reproCounter=0

currentLength<-vector()

mr<-vector() 

targetMasses=vector()
minNextDinoMassPredicted=vector()
maxNextDinoMassPredicted=vector()
minTotalMetabolicDryMassLoss=vector()
finalVoracity=vector()


currentWetMass<-vector()
dryMasses<-vector()

count=0

actualMassAtTheBeginningOfReproEvent=0.0

turnOn = TRUE
i=1
while(i <= length(currentAge) & turnOn){	
# print(i)
# stopifnot(i<441)	

	if(i>1){ ##To update reproCounter and clutchMass through the lifetime of the individual
		capta_all$reproCounter[i]=capta_all$reproCounter[i-1]
		capta_all$clutchMass[i]=capta_all$clutchMass[i-1]
		capta_all$capitalBreeding[i]=capta_all$capitalBreeding[i-1]
		capta_all$massForNextReproduction[i]=capta_all$massForNextReproduction[i-1]
		##Update present and future dry masses
		capta_all$nextMassPredicted[i]=capta_all$nextMassPredicted[i-1]
		capta_all$currentDryMass[i]=capta_all$currentDryMass[i-1]
	}
	

	
	# if(capta_all$reproCounter[i] == femaleMaxReproductionEvents){
	# capta_all$currentDryMass[i] = capta_all$currentDryMass[i-1]
    # }


		if(i>1){
			theMass<-capta_all$netGrowth[i-1]
	    }else{
			theMass<-eggDryMass + eggDryMass*factorEggMassFromMom[1]
			capta_all$currentDryMass[i]<-theMass
		}
		
		theWetMass=theMass*conversionToWetMass
		
		# currentMass<-theMass


			nextAge = currentAge[i+1]###max(currentAge)*0.001 ##10% of the max age is added
			
			
		if(!capta_all$mature[i]){
			if(i < ageOfFirstMaturation-pupaPeriodTime){
				nextLength = applyTSR(t=nextAge,tmax=tmax,kOpt=forTempK, thisMature=capta_all$mature[i])[[2]]

				capta_all$nextMassPredicted[i] = calculateDryMass(nextLength, coefficientForMassA, scaleForMassB)
			}else{ # LifeStage == PUPA
				capta_all$nextMassPredicted[i] = capta_all$nextMassPredicted[i-1]
			}
				###This function does not affect the nextLength of dinosaurs
				
			# }
			# print(c(lengthsVector[i],nextLength))

			
		
		# if(!mature[i]){
		
 
			# if(i>1){
			
			
			
 
            if(i>1){
				capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i-1]
			}

	
		}else{
		
			##This is after the last reproduction as capital breeder, needs to reproduce as non-capital breeder next
			if(capta_all$capitalBreeding[i] & reproCounter < femaleMaxReproductionEvents & reproCounter >= numberOfCapitalBreeds){ ##Exception of capital breeders which comes from below at repro 0
			
					##I commented this because update occurs at the last capital reproductive event
					# if(!eggsPerBatchFromEquation){
						# clutchMass = eggsPerBatch*(eggDryMass + eggDryMass*factorEggMassFromMom[1]) 
						# capta_all$clutchMass[i]=clutchMass
					# }else{
						
						# wetMass = capta_all$currentDryMass[i-1]*conversionToWetMass	
					    # eggsPerBatch = interceptForEggBatchFromEquation + slopeForEggBatchFromEquation*wetMass  ##Mass is wetMass
						# clutchMass = eggsPerBatch*(eggDryMass + eggDryMass*factorEggMassFromMom[1]) 
					# capta_all$clutchMass[i]=clutchMass
					# }
					
					# ageForNextReproduction = currentAge[i] + timeOfReproEvent
					# capta_all$massForNextReproduction[i] = capta_all$currentDryMass[i-1] + clutchMass
					
			# capitalBreeding = FALSE
			capta_all$capitalBreeding[i] = FALSE

			
			}

			
			if(!indeterminateGrowth & !capta_all$capitalBreeding[i] & reproCounter < femaleMaxReproductionEvents){
							
				if(reproCounter==0 & currentAge[i] >= ageOfFirstMaturation & !turnoff){
				
					iniMass<-capta_all$currentDryMass[i-1]
					# currentMass<-currentMass
					# iniMass<-massesVector[length(massesVector)]
						
					###@@Mario: this is really in function Animal::grow but I do it here to calculate feeding targets
					if(!eggsPerBatchFromEquation){
						clutchMass = eggsPerBatch*(eggDryMass + eggDryMass*factorEggMassFromMom[1]) 
					capta_all$clutchMass[i]=clutchMass
					}else{
						wetMass = capta_all$currentDryMass[i-1]*conversionToWetMass	
					    eggsPerBatch = interceptForEggBatchFromEquation + slopeForEggBatchFromEquation*wetMass  ##Mass is wetMass
						clutchMass = eggsPerBatch*(eggDryMass + eggDryMass*factorEggMassFromMom[1]) 
					capta_all$clutchMass[i]=clutchMass
					}

					capta_all$massForNextReproduction[i] = clutchMass + capta_all$currentDryMass[i-1]
					
					###@@Jordi 27032024: remove pupaPeriodTime from here, or it is applied twice
					ageForNextReproduction = ageOfFirstMaturation+timeOfReproEvent
					turnoff = TRUE
					
				}
				
				##This is because we need to know if the animal comes from previous recent capitalBreeding or not
				if(capta_all$capitalBreeding[1]){
				  targetReproMass=endMass-(capta_all$clutchMass[i-1]*(numberOfCapitalBreeds-1))
				}else{
				  targetReproMass=capta_all$massForNextReproduction[i]
				}
				
				if(targetReproMass > capta_all$currentDryMass[i-1] & ageForNextReproduction > currentAge[i])  ##&& ageForNextReproduction >= currentAge[i]
				{
				
					# if(ageForNextReproduction > currentAge[i]){
					
						slopeTarget = (targetReproMass - capta_all$currentDryMass[i-1])/(ageForNextReproduction-currentAge[i])
						interceptTarget = capta_all$currentDryMass[i-1]-slopeTarget*currentAge[i]
						
						capta_all$nextMassPredicted[i] = interceptTarget + slopeTarget*(currentAge[i+1])

						# print(c(capta_all$massForNextReproduction[i],currentMass,capta_all$nextMassPredicted[i],currentAge[i],ageForNextReproduction))	

						capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i]
						
						##@@R: to avoid going beyond massForNextReproduction
						if(capta_all$currentDryMass[i]>targetReproMass)
						{
							# print(c(i,capta_all$currentDryMass[i]))

							capta_all$currentDryMass[i]=targetReproMass
							# print(c(i,capta_all$currentDryMass[i]))
							# letsStop=FALSE
							# stopifnot(letsStop)
							# ##i=313,  3.13274214 

						}


						##capta_all$nextMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	
						slopeTarget = 0.0
						interceptTarget = 0.0
						
					# }else{##force currentAge-1 for calculations as to avoid nans
					
						# slopeTarget = (targetReproMass - capta_all$currentDryMass[i-1])/(ageForNextReproduction-(currentAge[i-1]))
						# interceptTarget = capta_all$currentDryMass[i-1]-slopeTarget*(currentAge[i-1])
						# capta_all$nextMassPredicted[i] = interceptTarget + slopeTarget*currentAge[i]
						# ##capta_all$nextMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	

						# capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i]
						
						# ##I leave this just to know if it never goes this way
						# print(c(i,capta_all$currentDryMass[i]))
						# letsStop=FALSE
						# stopifnot(letsStop)
						# ##i=313,  3.13274214 
										
						# slopeTarget = 0.0
						# interceptTarget = 0.0
						
						
					# }
						
				}else{
				
				# if(i>12046){
					# print(c("currentMass: ",currentMass,"capta_all$massForNextReproduction[i]: ",capta_all$massForNextReproduction[i], "deficit: ", currentMass-capta_all$massForNextReproduction[i]))
				# }

				
					# ##if the animal is already bigger - capta_all$nextMassPredicted becomes 0
					if(targetReproMass <= capta_all$currentDryMass[i-1] & ageForNextReproduction <= currentAge[i]){
	
						##I leave this just to know if it never goes this way
						# print(c(i,capta_all$currentDryMass[i]))
						# letsStop=FALSE
						# stopifnot(letsStop)
						##i=313,  3.13274214 
						# [1] 314.000000   3.132742
						
						reproCounter = reproCounter + 1
						capta_all$reproCounter[i]=reproCounter
						
						
						# print("here I am")
						
						##@@R: update next reproductive event
						if(!capta_all$capitalBreeding[1]){
							capta_all$currentDryMass[i] = iniMass
						}else{
							capta_all$currentDryMass[i] = endMass - capta_all$clutchMass[i-1]-(capta_all$clutchMass[i-1]*(numberOfCapitalBreeds-1))
						}
						
						capta_all$massForNextReproduction[i] = capta_all$clutchMass[i-1] + capta_all$currentDryMass[i]
						ageForNextReproduction = currentAge[i]+timeOfReproEvent
						
						slopeTarget = (capta_all$massForNextReproduction[i] - capta_all$currentDryMass[i])/(ageForNextReproduction-(currentAge[i]))
						interceptTarget = capta_all$currentDryMass[i]-slopeTarget*(currentAge[i])
						capta_all$nextMassPredicted[i] = interceptTarget + slopeTarget*(currentAge[i+1])
						##capta_all$nextMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	

						# capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i]

										
						##this is to make the next target above the current one for predVor
						protoNextMass=interceptTarget + slopeTarget*(currentAge[i+2])
						
						slopeTarget = 0.0
						interceptTarget = 0.0

						
						# # capta_all$nextMassPredicted[i] = currentMass
						
						#@@Weaver: target voracity is 0
						# capta_all$nextMassPredicted[i] = interceptTarget + slopeTarget*(i+1) 
						
						# count=count+1
						# print(c(i,"here XXXXXXXXXXXXXXXXXXXXXXXXX",count,capta_all$nextMassPredicted[i]))

					
					}else{
					##here we force a positive slope to allow animals to keep feeding until the target is met 
						if(targetReproMass > capta_all$currentDryMass[i-1] && ageForNextReproduction < currentAge[i]){
							slopeTarget = (targetReproMass-capta_all$currentDryMass[i-1])/(ageForNextReproduction-currentAge[i])
							interceptTarget = targetReproMass-slopeTarget*ageForNextReproduction
							capta_all$nextMassPredicted[i] = interceptTarget + slopeTarget*ageForNextReproduction
							##capta_all$nextMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	

							capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i]
							
							slopeTarget = 0.0
							interceptTarget = 0.0
							
						##I leave this just to know if it never goes this way
						# print(c(i,capta_all$currentDryMass[i]))
						# letsStop=FALSE
						# stopifnot(letsStop)
						# ##i=313,  3.13274214 

							
			
							
						}
					}
				
				}
				
				
			}
			
			if(indeterminateGrowth){ ##there is indeterminate growth

				totFec = femaleMaxReproductionEvents*eggsPerBatch
				totalReproMass = sum(vectorsForCalc$massesVector[(length(vectorsForCalc$massesVector)-femaleMaxReproductionEvents+1):length(vectorsForCalc$massesVector)])
				
				
				if(reproCounter==0 & currentAge[i] >= ageOfFirstMaturation & !turnoff){
				
					# currentMass<-capta_all$netGrowth[i-1]
					###@@Jordi 27032024: remove pupaPeriodTime from here, or it is applied twice
					ageForNextReproduction = ageOfFirstMaturation+timeOfReproEvent
					nextLength = applyTSR(t=ageForNextReproduction,tmax=max(currentAge),kOpt=forTempK, thisMature=capta_all$mature[i])[[2]]
										
					fractFirstRepro = vectorsForCalc$massesVector[(length(vectorsForCalc$massesVector)-femaleMaxReproductionEvents+1)]/totalReproMass
					
					##For inderterminate growth, make smaller individuals lay fewer eggs assuming linearity
					currentEggs = fractFirstRepro*totFec
					
					# interpol2(currentMass,vectorsForCalc$massesVector[instarFirstReproduction-1],
					# vectorsForCalc$massesVector[length(vectorsForCalc$massesVector)],
					# eggsPerBatch*propAdultMass, eggsPerBatch)
					
					clutchMass = currentEggs*(eggDryMass + eggDryMass*factorEggMassFromMom[1]) 
					capta_all$clutchMass[i]=clutchMass
					
					capta_all$massForNextReproduction[i]<-calculateDryMass(nextLength, coefficientForMassAforMature, scaleForMassBforMature) + clutchMass
					
					turnoff = TRUE
				}
				
				# print(c(i,capta_all$massForNextReproduction[i],currentMass,ageForNextReproduction,currentAge[i]))	
								
				
				if(capta_all$massForNextReproduction[i] > capta_all$currentDryMass[i-1] & ageForNextReproduction >= currentAge[i] & reproCounter < femaleMaxReproductionEvents)
				{
				
					if(ageForNextReproduction > currentAge[i]){
						slopeTarget = (capta_all$massForNextReproduction[i] - capta_all$currentDryMass[i-1])/(ageForNextReproduction-currentAge[i])
						interceptTarget = capta_all$currentDryMass[i-1]-slopeTarget*currentAge[i]
						capta_all$nextMassPredicted[i] = interceptTarget + slopeTarget*(currentAge[i+1])

						capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i]


						##capta_all$nextMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	
						slopeTarget = 0.0
						interceptTarget = 0.0
					}else{##force currentAge-1 for calculations as to avoid nans
						slopeTarget = (capta_all$massForNextReproduction[i] - capta_all$currentDryMass[i-1])/(ageForNextReproduction-(currentAge[i-1]))
						interceptTarget = capta_all$currentDryMass[i-1]-slopeTarget*(currentAge[i-1])
						capta_all$nextMassPredicted[i] = interceptTarget + slopeTarget*currentAge[i]
						##capta_all$nextMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	

						capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i]

										
						slopeTarget = 0.0
						interceptTarget = 0.0
					}
						
				}else{
				
				  # if(reproCounter < femaleMaxReproductionEvents-1){ ##to avoid reading out of range
					# ##if the animal is already bigger i reproduces
					if(ageForNextReproduction <= currentAge[i] & capta_all$massForNextReproduction[i] <= capta_all$currentDryMass[i-1] & reproCounter < femaleMaxReproductionEvents-1){
							
						# stopifnot(exists("whatever"))
						
						reproCounter = reproCounter + 1
						capta_all$reproCounter[i]=reproCounter
						
						##@@R: update next reproductive event
						# currentMass = capta_all$massForNextReproduction[i] - clutchMass

						ageForNextReproduction = currentAge[i]+timeOfReproEvent

						nextLength = applyTSR(t=ageForNextReproduction,tmax=tmax,kOpt=forTempK, thisMature=capta_all$mature[i])[[2]]
						
						fractThisRepro = vectorsForCalc$massesVector[(length(vectorsForCalc$massesVector)-femaleMaxReproductionEvents+1)+reproCounter]/totalReproMass
					
						# print(fractThisRepro)
					
						##For inderterminate growth, make smaller individuals lay fewer eggs assuming linearity
						currentEggs = fractThisRepro*totFec

	
						##For inderterminate growth, make smaller individuals lay fewer eggs assuming linearity
						
						capta_all$currentDryMass[i]=capta_all$currentDryMass[i-1]-capta_all$clutchMass[i]
						
						clutchMass = currentEggs*(eggDryMass + eggDryMass*factorEggMassFromMom[1]) 
						capta_all$clutchMass[i]=clutchMass
						
						capta_all$massForNextReproduction[i]<-calculateDryMass(nextLength, coefficientForMassAforMature, scaleForMassBforMature) + clutchMass
			
						
					    
						# ##@@R: Decide whether taking the mass of the actual animal or update from currentAge
						# lengthForNextMass<-applyTSR(t=currentAge[i+1],tmax=tmax,kOpt=kOpt)[[2]]
					    # predictedNextMass<-calculateDryMass(lengthForNextMass)

						# capta_all$nextMassPredicted[i] = max(currentMass,predictedNextMass)
						
						slopeTarget = (capta_all$massForNextReproduction[i] - capta_all$currentDryMass[i])/(ageForNextReproduction-(currentAge[i]))
						interceptTarget = capta_all$currentDryMass[i]-slopeTarget*(currentAge[i])
						capta_all$nextMassPredicted[i] = interceptTarget + slopeTarget*(currentAge[i+1])
						##capta_all$nextMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	

						capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i]

										
						##this is to make the next target above the current one for predVor
						protoNextMass=interceptTarget + slopeTarget*(currentAge[i+2])

						
						
						# print(c("maxIsCurrent?: ",ifelse((currentMass-predictedNextMass)>0,"yes","no")))
						# print(c("pred/current: ",predictedNextMass/currentMass))
						
						# print(c("predictedNextMass: ",))
			
	
						#@@Weaver: target voracity is 0
						# capta_all$nextMassPredicted[i] = interceptTarget + slopeTarget*(i+1) 
					
					}else{
					
					
						if(capta_all$massForNextReproduction[i] > capta_all$currentDryMass[i-1]){  ##still needs to eat to meet reproductive target
						
								##we swap currentAge to ageForNextReproduction
								slopeTarget = (capta_all$massForNextReproduction[i] - capta_all$currentDryMass[i-1])/(currentAge[i]-ageForNextReproduction)
								interceptTarget = capta_all$currentDryMass[i-1]-slopeTarget*ageForNextReproduction
								capta_all$nextMassPredicted[i] = interceptTarget + slopeTarget*currentAge[i+1]

								capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i]


								##capta_all$nextMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	
								slopeTarget = 0.0
								interceptTarget = 0.0
							

						}else{
						##here we force a positive slope to allow animals to keep feeding until the target is met 
							# if(capta_all$massForNextReproduction[i] > currentMass && ageForNextReproduction < currentAge[i]){
								# slopeTarget = (capta_all$massForNextReproduction[i]-currentMass)/(ageForNextReproduction-currentAge[i])
								# interceptTarget = capta_all$massForNextReproduction[i]-slopeTarget*ageForNextReproduction
								
								
								##This is just to allow the last mass in the file capta_all
								capta_all$nextMassPredicted[i] = capta_all$nextMassPredicted[i-1]
								
								capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i]
								##capta_all$nextMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	
								
								# slopeTarget = 0.0
								# interceptTarget = 0.0
						}
					}
				}
			 }
				# }
				
	
			if(capta_all$capitalBreeding[i] & reproCounter < femaleMaxReproductionEvents){
			
				if(reproCounter == 0 & currentAge[i] >= ageOfFirstMaturation & !turnoff){
				
					endMass<-capta_all$currentDryMass[i-1]
					# currentMass<-currentMass
					capta_all$massForNextReproduction[i] = capta_all$currentDryMass[i-1]
					###@@Jordi 27032024: remove pupaPeriodTime from here, or it is applied twice
					ageForNextReproduction = ageOfFirstMaturation+timeOfReproEventDuringCapitalBreeding
					
					if(!eggsPerBatchFromEquation){
						clutchMass = eggsPerBatch*(eggDryMass + eggDryMass*factorEggMassFromMom[1]) 
					capta_all$clutchMass[i]=clutchMass
					}else{
						
						wetMass = capta_all$currentDryMass[i-1]*conversionToWetMass	
					    eggsPerBatch = interceptForEggBatchFromEquation + slopeForEggBatchFromEquation*wetMass  ##Mass is wetMass
						clutchMass = eggsPerBatch*(eggDryMass + eggDryMass*factorEggMassFromMom[1]) 
					
					capta_all$clutchMass[i]=clutchMass
					}
					
					##update egg sac
					# capta_all$currentDryMass[i-1]=capta_all$currentDryMass[i-1]-clutchMass
					# iniMass<-massesVector[length(massesVector)]
					
					turnoff = TRUE
					
					# reproCounter = reproCounter + 1
					# capitalBreeding = FALSE		
					
				}

				if(ageForNextReproduction > currentAge[i]){
				
				print(c(i,reproCounter,currentAge[i],ageForNextReproduction))
				
					capta_all$nextMassPredicted[i] = capta_all$currentDryMass[i-1]
					
					capta_all$currentDryMass[i] = capta_all$nextMassPredicted[i]
				
				}else{
				
				# stop("I am here")
				
				    reproCounter = reproCounter + 1
					capta_all$reproCounter[i]=reproCounter
			
					if(reproCounter < numberOfCapitalBreeds){
					
						capta_all$currentDryMass[i] = capta_all$currentDryMass[i-1] - capta_all$clutchMass[i]
						capta_all$nextMassPredicted[i] = capta_all$currentDryMass[i]
						capta_all$massForNextReproduction[i] = capta_all$currentDryMass[i]
						ageForNextReproduction = currentAge[i]+timeOfReproEventDuringCapitalBreeding
						
					}else{
					
						if(reproCounter < femaleMaxReproductionEvents)
						{
					    
							##Jordi 08092024 - need to remove clutch mass from current mass to return to original mass
							capta_all$currentDryMass[i] = capta_all$currentDryMass[i-1] - capta_all$clutchMass[i-1]
							
							# print(c(i,capta_all$currentDryMass[i]))
							# letsStop=FALSE
							# stopifnot(letsStop)
							# # 1.83632899
							##1.836329

							
							# # capta_all$nextMassPredicted[i] <- currentMass

							# if(!eggsPerBatchFromEquation){
								# clutchMass = eggsPerBatch*(eggDryMass + eggDryMass*factorEggMassFromMom[1]) 
							# capta_all$clutchMass[i]=clutchMass
							# }else{
								
								# wetMass = capta_all$currentDryMass[i]*conversionToWetMass	
								# eggsPerBatch = interceptForEggBatchFromEquation + slopeForEggBatchFromEquation*wetMass  ##Mass is wetMass
								# clutchMass = eggsPerBatch*(eggDryMass + eggDryMass*factorEggMassFromMom[1]) 
							# capta_all$clutchMass[i]=clutchMass
							# }

							##The mass for next reproduction is exactly the mass before laying the clutch
							##clutch mass at the beginning of the loop update will become identical to the previous one
							capta_all$massForNextReproduction[i] = endMass ##capta_all$currentDryMass[i-1]
							ageForNextReproduction = currentAge[i]+timeOfReproEvent
							
							slopeTarget = (capta_all$massForNextReproduction[i] - capta_all$currentDryMass[i])/(ageForNextReproduction-currentAge[i])
							interceptTarget = capta_all$currentDryMass[i] - slopeTarget*currentAge[i]
							capta_all$nextMassPredicted[i] = interceptTarget + slopeTarget*currentAge[i+1]
							
							# # capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i]

							##capta_all$nextMassPredicted = getSpecies()->getCoefficientForMassAforMature()*pow(nextDinoLengthPredicted,getSpecies()->getScaleForMassBforMature());	
							slopeTarget = 0.0
							interceptTarget = 0.0
						}else{
							capta_all$currentDryMass[i] = capta_all$currentDryMass[i-1] - capta_all$clutchMass[i]
							capta_all$nextMassPredicted[i] = capta_all$currentDryMass[i] 
							# # stop(c(i,"  I got here"))
						}

					}
					

				
				}
			
			}
			
			# if(reproCounter == femaleMaxReproductionEvents){
			  
				# capta_all$nextMassPredicted[i] = capta_all$currentDryMass[i-1]
				
				# capta_all$currentDryMass[i]<-capta_all$nextMassPredicted[i]

			
			# }

			if(i == length(currentAge)){ ##@@R: This is to avoid NAs as the last value of capta_all$nextMassPredicted is out of range
			  
				capta_all$nextMassPredicted[i] = capta_all$currentDryMass[i]
			
			}


		
		
			# print(c(i,"before: ",currentMass))

			# capta_all$massForNextReproduction[i]<-capta_all$massForNextReproduction[i]
			capta_all$ageForNextReproduction[i]<-ageForNextReproduction

			# print(c(i,capta_all$massForNextReproduction[i],currentMass,capta_all$nextMassPredicted[i]))	

		}	


# currentDryMass[i] = currentMass
		
# ##This line only to update the next timeStep
# currentMass = nextMassPredicted[i]
				
# print(c(i,"after: ",currentMass))



capta_all$eggsPerBatch[i]<-eggsPerBatch
# capta_all$mature[i]<-mature[i]

if(capta_all$mature[i]){
	nextDinoLengthPredicted[i]<-calculateDryLength(capta_all$nextMassPredicted[i], coefficientForMassAforMature, scaleForMassBforMature)
}
else{
	nextDinoLengthPredicted[i]<-calculateDryLength(capta_all$nextMassPredicted[i], coefficientForMassA, scaleForMassB)
}

capta_all$nextDinoLengthPredicted[i]<-nextDinoLengthPredicted[i]
capta_all$nextMassPredicted[i]<-capta_all$nextMassPredicted[i]

# data.frame(capta_all$nextMassPredicted,mature)

## currentLength[i] = lengthsVector[i]
currentDryMass[i] = capta_all$currentDryMass[i] ##calculateDryMass(lengthsVector[i])

if(capta_all$mature[i]){
	currentLength[i]<-calculateDryLength(currentDryMass[i], coefficientForMassAforMature, scaleForMassBforMature)
	capta_all$currentLength[i]<-calculateDryLength(capta_all$currentDryMass[i], coefficientForMassAforMature, scaleForMassBforMature)
}
else{
	currentLength[i]<-calculateDryLength(currentDryMass[i], coefficientForMassA, scaleForMassB)
	capta_all$currentLength[i]<-calculateDryLength(capta_all$currentDryMass[i], coefficientForMassA, scaleForMassB)
}


# capta_all$currentDryMass<-currentDryMass

   
capta_all$currentWetMass[i] = capta_all$currentDryMass[i]*conversionToWetMass
	# voracity_ini = interpol(rnorm(N), vorMin, vorMax)
	

capta_all$mr[i]<-getTotalMetabolicDryMassLoss(wetMass=capta_all$currentWetMass[i], bMet[1], actE_met[1], actE_search[1], temperature, 
	proportionOfTimeTheAnimalWasMoving = propMov, search_area=0, simulationType) 
												##preTsearch								


# LifeStage == PUPA
if(ageOfFirstMaturation-pupaPeriodTime <= i & i < ageOfFirstMaturation){
	capta_all$mr[i]<-0.0
}


##@Parameters: Important note for parameterization: Ed, Topt i lower,upper thresholds
##combine to make temperature go to zero  then Topt is not at the central point
##e.g., Topt=15, Ed=4, low = 0, upp = 20

	##@Mario: set this here, outside the ifs()
		minTotalMetabolicDryMassLoss[i] = capta_all$mr[i]
		

		if(!capta_all$mature[i])
		{

			

			minNextDinoMassPredicted[i] = capta_all$nextMassPredicted[i] - capta_all$nextMassPredicted[i] * minPlastBV
			
			if(type!=2){
				targetMasses[i] = (yodzis(theWetMass,newA=newAJuv,newB=newB)/conversionToWetMass)*timeStep
			}else{
				targetMasses[i] = (garland(theWetMass)/conversionToWetMass)*timeStep
			}

			# print(theWetMass)
		
			# if(i<1000){
				# print(c(i,capta_all$nextMassPredicted[i],theMass,garland(theWetMass)/conversionToWetMass))
			# }
		
			

			if(targetMasses[i]<0){##this happens when capta_all$nextMassPredicted = 0
				targetMasses[i] = 0		
			}

			#@Robustness: try this with or w/o adding losses to metabolism
			targetMasses[i] = targetMasses[i] #+ minTotalMetabolicDryMassLoss[i]	
		
		}
		else ##isMature == true
		{

			minNextDinoMassPredicted[i] = capta_all$nextMassPredicted[i] - capta_all$nextMassPredicted[i] * minPlastBV
			
			if(type!=2){
				if(!capta_all$capitalBreeding[i]){
					maxNextDinoMassPredicted[i] = theMass + (yodzis(theWetMass,newA=newAAdult,newB=newB)/conversionToWetMass)*timeStep
				}else{##if it is a capital breeder only feeds for maintenance
					maxNextDinoMassPredicted[i] = theMass ##+ minTotalMetabolicDryMassLoss[i] ##capital breeders feed for maintenance purposes only
				}
			}else{
				maxNextDinoMassPredicted[i] = theMass + (garland(theWetMass)/conversionToWetMass)*timeStep
			}
			
			targetMasses[i] = maxNextDinoMassPredicted[i] - theMass

			# print(theWetMass)

			if(targetMasses[i]<0){
					
				if(currentAge[i] > capta_all$ageForNextReproduction[i]){  ##did not reproduce on time because food is still needed
					
					targetMasses[i] = abs(maxNextDinoMassPredicted[i] - theMass)		
					
				}else{
				
					targetMasses[i]=0
					
				}

			}


			
			targetMasses[i] = targetMasses[i] 

		}

		preFinalVoracity = targetMasses[i] ##postTvor* Target mass has been fixed for zeroes before



		if(tempDependency){
		
			postTvor = useDell(newT=temperature+273,actE=actE_vor[1],lowerThreshold,
				  upperThreshold,Topt=ToptVoracity,Ed=EdVoracity,trait=vorAtOptT)
				  
				##@@Mario: include this for when temps go out of the threshold  
			postTvor = max(postTvor,0.0)
		
		}else{ ##for dinos

		  preFinalVoracity = targetMasses[i]
		  
		  postTvor = voracity_ini[1]

		}

	# print(preFinalVoracity)
	
	
	##predicted voracity to contrast with that coming from Yodzis/Garland and actual wetMass - 21082024
	capta_all$maxMetabolicDryMassLoss[i] = getTotalMetabolicDryMassLoss(wetMass=capta_all$currentWetMass[i], 
		bMet[1], actE_met[1], actE_search[1], temperature, 
		proportionOfTimeTheAnimalWasMoving = 1, 
		search_area=search_area_ini[1]*capta_all$currentWetMass[i]^scaleForSearchArea, 
		simulationType) 
		
	# if(i==1){
	   # predVor=(capta_all$nextMassPredicted[i]+capta_all$maxMetabolicDryMassLoss[i]-(eggDryMass+eggDryMass*factorEggMass[1]))/assim[1] ##where assim must be minimum assim (like feeding on the least profitable prey)
	# }else{
       # if(capta_all$reproCounter[i]>capta_all$reproCounter[i-1]){ ##reproduction has occurred - use the future target as to avoid negative values
		 # predVor=(protoNextMass+capta_all$maxMetabolicDryMassLoss[i]-(capta_all$nextMassPredicted[i]))/assim[1] 	
	   # }else{
		 # predVor=(capta_all$nextMassPredicted[i]+capta_all$maxMetabolicDryMassLoss[i]-(capta_all$nextMassPredicted[i-1]))/assim[1] ##where assim must be minimum assim (like feeding on the least profitable prey)	   
	   # }
	
	# }



	if(i==1){
	   predVor=(capta_all$nextMassPredicted[i]+capta_all$mr[i]-(eggDryMass+eggDryMass*factorEggMass[1]))/assim[1] ##where assim must be minimum assim (like feeding on the least profitable prey)
	}else{
	   ##if one of the two conditions below is TRUE the result is TRUE (because we have an "or" i.e. "|")
       if(!capta_all$capitalBreeding[i] | !capta_all$mature[i])
	   {
		   if(capta_all$reproCounter[i]>capta_all$reproCounter[i-1]){ ##reproduction has occurred - use the future target as to avoid negative values
			 predVor=(protoNextMass+capta_all$mr[i]-(capta_all$nextMassPredicted[i]))/assim[1] 	
			 # print(c(i,predVor))
			 rm(protoNextMass)
		   }else{
			 predVor=(capta_all$nextMassPredicted[i]+capta_all$mr[i]-(capta_all$nextMassPredicted[i-1]))/assim[1] ##where assim must be minimum assim (like feeding on the least profitable prey)	   
		   }
	   }else{##Mature capital breeders only
			predVor=0
	   }
	}
    preFinalVoracity=min(preFinalVoracity,predVor)	
	capta_all$beyondCurve[i]<-ifelse(preFinalVoracity == predVor,TRUE,FALSE)
	capta_all$predVor[i]<-predVor

	capta_all$preFinalVoracity[i]<-preFinalVoracity
	
	capta_all$postTvor[i]<-postTvor


	# LifeStage == PUPA
	if(ageOfFirstMaturation-pupaPeriodTime <= i & i < ageOfFirstMaturation){
		preFinalVoracity<-0.0
		capta_all$beyondCurve[i]<-TRUE
		predVor<-0.0
		capta_all$predVor[i]<-0.0
		capta_all$preFinalVoracity[i]<-preFinalVoracity
		postTvor<-0.0
		capta_all$postTvor[i]<-postTvor
	}


	# rm(postTvor)

	 
		# ##@@Mario: there is an error... set to trait before fixing the zeroes
		# setTrait(Trait::voracity, preFinalVoracity)
		# ##below we ensure that voracity does not get into negative values, if for instance voracity_ini is negative
		# # # @@Mario: this line must go before the one above
		# preFinalVoracity = max(getTrait(Trait::voracity),0.0)

		if(preFinalVoracity > 0 && capta_all$nextMassPredicted[i] > 0.00000001) ##when slope and interceptTarget = 0
		{

		devTimeVectorWithAgeOfFirstMaturation = c(devTimeVector[1:(length(devTimeVector)-1)],ageOfFirstMaturation)
		
		if(!capta_all$mature[i]){ ##not mature
			##enhanced h
			if(currentAge[i]>devTimeVectorWithAgeOfFirstMaturation[1]){ ###If at instar > II 

			
				##this below is to guess next value in devTimeVectorWithAgeOfFirstMaturation
				theNegatives<-currentAge[i]-round(devTimeVectorWithAgeOfFirstMaturation)
				minDiffToInstar<-max(theNegatives[theNegatives<=0])
				
				if(!is.infinite(minDiffToInstar)){ ##@@Mario: exception for the period between age of last instar and maturation, because pupaPeriodTime
					targetTime<-devTimeVectorWithAgeOfFirstMaturation[which(minDiffToInstar == (currentAge[i]-round(devTimeVectorWithAgeOfFirstMaturation)))]
					massAtNextInstar<-calculateDryMass(applyTSR(capta_all$currentAge[which(capta_all$currentAge == round(targetTime))],tmax=tmax,kOpt=forTempK, thisMature=capta_all$mature[i])[[2]], coefficientForMassA, scaleForMassB)
				}else{  ##force mass at massAtNextRepro as massAtNextInstar
					massAtNextInstar<-capta_all$massForNextReproduction[which(capta_all$massForNextReproduction[i] == min(capta_all$massForNextReproduction[i],na.rm=T))][1] 
				}

				##this below is to guess previous value in devTimeVectorWithAgeOfFirstMaturation
				thePositives<-currentAge[i]-round(devTimeVectorWithAgeOfFirstMaturation)
				minDiffFromInstar<-min(thePositives[thePositives>0])
				targetTime<-devTimeVectorWithAgeOfFirstMaturation[which(minDiffFromInstar == (currentAge[i]-round(devTimeVectorWithAgeOfFirstMaturation)))]
				massAtPreviousInstar<-capta_all$netGrowth[which(capta_all$currentAge == round(targetTime))]


				h = (1-(capta_all$netGrowth[i-1]-massAtPreviousInstar)/(massAtNextInstar-massAtPreviousInstar)) ##+h_enhancement
			
			}else{ ##If at instar I
			
						
				##this below is to guess next value in devTimeVectorWithAgeOfFirstMaturation
				# theNegatives<-currentAge[i]-round(devTimeVectorWithAgeOfFirstMaturation)
				# minDiffToInstar<-max(theNegatives[theNegatives<0])
				targetTime<-devTimeVectorWithAgeOfFirstMaturation[1]
				
				massAtNextInstar<-calculateDryMass(applyTSR(capta_all$currentAge[which(capta_all$currentAge == round(targetTime))],tmax=tmax,kOpt=forTempK, thisMature=capta_all$mature[i])[[2]], coefficientForMassA, scaleForMassB)



				if(i==1){#### | type==2 @@Jordi @@Mario Dinosaurs have h=1 while growing - continuous growth
					h=1
				}else{
					h = (1-(capta_all$netGrowth[i-1]-eggDryMass)/(massAtNextInstar-eggDryMass)) ##+h_enhancement
				}
			}
			
			##@@Mario: Juvenile dinosaurs should not have a year-to-year satitation - no deceleration
			if(type %in% 2){
			   h=1
			}

		
		}else{ ##is mature
		
			if(capta_all$reproCounter[i]==0){
			
			
				##this below is to guess previous value in devTimeVectorWithAgeOfFirstMaturation
				thePositives<-currentAge[i]-round(devTimeVectorWithAgeOfFirstMaturation)
				minDiffFromInstar<-min(thePositives[thePositives>0])
				targetTime<-devTimeVectorWithAgeOfFirstMaturation[which(minDiffFromInstar == (currentAge[i]-round(devTimeVectorWithAgeOfFirstMaturation)))]
				massAtPreviousInstar<-capta_all$netGrowth[which(capta_all$currentAge == round(targetTime))]

				massAtNextRepro<-capta_all$massForNextReproduction[i]
				
				h = (1-(capta_all$netGrowth[i-1]-massAtPreviousInstar)/(massAtNextRepro-massAtPreviousInstar)) ##+h_enhancement
				
				# print(c(i,h))	
				# print(c(capta_all$netGrowth[i-1],h))				

				
			}else{
			
				# ##@@R: just after the first reproductive event calculate the mass of the individual
				# if(capta_all$reproCounter[i-1]==0){ ##just after the very first reproductive event
				   # actualMassAtTheBeginningOfReproEvent = capta_all$currentMass[i] 
				# }

			
				##@@R: Is there a new reproductive event starting? if so, calculate the actual mass of the individual as the basis for "h"
				if(!capta_all$capitalBreeding[i] & !type %in% c(3,5)){
					if(capta_all$reproCounter[i]>capta_all$reproCounter[i-1])
					{
					   actualMassAtTheBeginningOfReproEvent = capta_all$netGrowth[i-1] - capta_all$clutchMass[i-1]
					}
				}else{ #if capital breeding
				
					posic<-min(which(!is.na(capta_all$clutchMass))) ##age of first clutch mass estimated
					actualMassAtTheBeginningOfReproEvent = capta_all$netGrowth[posic] - capta_all$clutchMass[posic]
				
				}
				
				if(!capta_all$capitalBreeding[i] & type %in% c(3,5)){  ##for capital breeders that have finished breeding capitally
				
					posic<-max(which(!is.na(capta_all$clutchMass))) ##age of last clutch mass estimated
					actualMassAtTheBeginningOfReproEvent = capta_all$netGrowth[posic-1] - capta_all$clutchMass[posic]
				
				
				}
			
			
			
				##search mass of previous reproduction using timings
				# ii=i-1
				# while(!capta_all$ageForNextReproduction[i] > capta_all$ageForNextReproduction[ii])
				# {
					# ii=ii-1
				# }
				
				massAtNextRepro<-capta_all$massForNextReproduction[i]
				# massAtPreviousRepro<-capta_all$massForNextReproduction[i][ii]
				
				# timeAtNextRepro<-capta_all$ageForNextReproduction[i]
				# timeAtPreviousRepro<-capta_all$ageForNextReproduction[ii]


			
				# ##search mass of previous reproduction
				# ii=i-1
				# while(!capta_all$massForNextReproduction[i] > capta_all$massForNextReproduction[i][ii])
				# {
					# ii=ii-1
				# }
				
				# massAtNextRepro<-capta_all$massForNextReproduction[i][i]
				# massAtPreviousRepro<-capta_all$massForNextReproduction[i][ii]
				
				# if(calculateDryMass(currentLength[i]) >= massAtPreviousRepro){
				
				h = (1-(capta_all$netGrowth[i-1]-actualMassAtTheBeginningOfReproEvent)/(massAtNextRepro-actualMassAtTheBeginningOfReproEvent)) ##+h_enhancement
				
				# }
				# else{  ##when mass deficit is larger than between reproductions: usually at the beginning of a reproductive event, add currentMass to massAtNextRepro to make denominator consequently larger
				    # h = (1-(massAtPreviousRepro-calculateDryMass(currentLength[i]))/((massAtNextRepro+calculateDryMass(currentLength[i]))-massAtPreviousRepro)) ##+h_enhancement
				# }
				
				
				##@@Note: h by time - perfect shape but it will not work because will produce small "h" values for very hungry animals when food is not available
				# if(currentAge[i] >= capta_all$ageForNextReproduction[ii]){##age at previous reproduction "ii"
					# h = (1-(currentAge[i]-timeAtPreviousRepro)/(timeAtNextRepro-timeAtPreviousRepro)) ##+h_enhancement
				# } #else{
				    # h = (1-(massAtPreviousRepro-calculateDryMass(currentLength[i]))/(massAtNextRepro-massAtPreviousRepro)) ##+h_enhancement
				# }

			
			# print(c("h: ",h))
			# print(c("next: ",massAtNextRepro))
			# print(c("previous: ",actualMassAtTheBeginningOfReproEvent))
			# print(c("current: ",calculateDryMass(currentLength[i])))
			
			}
			

		}
		
		
		if (i == length(currentAge))
		{
			h = 0 ##just to make it work
		}
		
		# h=1
		# print(c("timeStep is: ",timeStep, "this i before h: ",i))
		if(!exists("h")) print("no_h")
		if(h<0) h = 0 ##@@Mario
		
		##fix h
		# h=0.1
		
	capta_all$h[i]<-h
		
		##@@Mario: include this parameter and remove min/max
		plasticityDueToConditionVor = postTvor##is the percentage of h to calcualate s
		
		if(i>1){
		    deficit = capta_all$nextMassPredicted[i-1] - capta_all$netGrowth[i-1]
			if(!deficit<0)##because zeros are produced for some mathematical reason of the involved equations for dinos (type=2)
			{
				###@@Jordi, @@Mario: improvement, remove the effect of h when there is deficit or the result is negative most of the times
				# # finalVoracity = (preFinalVoracity*h + (deficit/assim[1]))*plasticityDueToConditionVor
				finalVoracity = (preFinalVoracity + (deficit/assim[1]))*plasticityDueToConditionVor
				
				# print(c(i,deficit,finalVoracity-preFinalVoracity))

			}else{
				finalVoracity = preFinalVoracity*h*plasticityDueToConditionVor
			}

			capta_all$deficit[i]<-deficit

			# print(c("i: ",i))
			# print(c("deficit: ",deficit))

		}else{
			finalVoracity = preFinalVoracity*h*plasticityDueToConditionVor
		}
		
		# # finalVoracity = preFinalVoracity*h*plasticityDueToConditionVor
		
		# # finalVoracity = preFinalVoracity
		
		# stopifnot(!is.na(finalVoracity))
		# stopifnot(!is.na(yodzis(theWetMass,newA=newAJuv,newB=newB)))
		

		# if(type %in% c(1,3,4,5)){
		# if(yodzis(theWetMass,newA=newAJuv,newB=newB)/conversionToWetMass < finalVoracity)
		# {
		   # finalVoracity = (yodzis(theWetMass,newA=newAJuv,newB=newB)/conversionToWetMass)*timeStep
		   # capta_all$beyondMax[i] = TRUE
		# }else{
		   # capta_all$beyondMax[i] = FALSE
		# }
		# }
		
		# stopifnot(!is.na(capta_all$beyondMax[i]))
		
		## finalVoracity = garland(theWetMass)/conversionToWetMass
		# if(type %in% 2){
		# if(garland(theWetMass)/conversionToWetMass < finalVoracity)
		# {
		   # finalVoracity = (garland(theWetMass)/conversionToWetMass)*timeStep
		   # capta_all$beyondMax[i] = TRUE
		# }else{
		   # capta_all$beyondMax[i] = FALSE
		# }
		# }

		
		finalVoracity = max(finalVoracity, 0.0)
		
		if(tempDependency){
		
			
			postTsearch = useDell(newT=temperature+273,actE=actE_search[1],lowerThreshold,
			  upperThreshold,Topt=ToptSearch,Ed=EdSearch,trait=searchAtOptT)
			  
			##@@Mario: include this for when temps go out of the threshold  
			postTsearch = ifelse(postTsearch<0.0,0,postTsearch)

			postTspeed = useDell(newT=temperature+273,actE=actE_speed[1],lowerThreshold,
			 upperThreshold,Topt=ToptSpeed,Ed=EdSpeed,trait=speedAtOptT)
			  
			##@@Mario: include this for when temps go out of the threshold  
			postTspeed = ifelse(postTspeed<0.0,0,postTspeed)
			
			# wetMass = conversionToWetMass*(calculateDryMass(L[1])
			##the two values below are now going to be used again
			postTsearch = postTsearch*capta_all$currentWetMass[i]^scaleForSearchArea
			
			# speed_ini = interpol(rnorm(N), speedMin, speedMax)
			
			if(simulationType %in% c("arthropods","dinosaurs")){
				postTspeed = (postTspeed*capta_all$currentWetMass[i]^scaleForSpeed)*(1-exp(-22*(capta_all$currentWetMass^(-0.6))))  ##25.5𝑀0.26(1−𝑒−22𝑀−0.6) Hirt et al. 2017
			}

			if(simulationType %in% c("aquatic")){
				postTspeed = (postTspeed*capta_all$currentWetMass[i]^scaleForSpeed)
			}


		
		}else{ ##sim without temperature
		
		# search_area_ini[1] = search_area_ini[1]

		  postTsearch = search_area_ini[1]*capta_all$currentWetMass[i]^scaleForSearchArea
		  
		  
		  
		  	if(simulationType %in% c("arthropods","dinosaurs")){
				postTspeed = (speed_ini[1]*capta_all$currentWetMass[i]^scaleForSpeed)*(1-exp(-22*(capta_all$currentWetMass[i]^(-0.6))))  ##25.5𝑀0.26(1−𝑒−22𝑀−0.6) Hirt et al. 2017
			}

			if(simulationType %in% c("aquatic")){
				postTspeed = (speed_ini[1]*capta_all$currentWetMass[i]^scaleForSpeed)
			}

			# print(postTspeed)

		}

		
	   ##BELOW WE INCLUDE CONDITION-DEPENDENT PLASTICITY FOR SEARCH AREA AND SPEED
	   plasticityDueToConditionSearch = 10
		   
	   print("before vs after... XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
	   # # print(postTsearch)
	   finalSearch = postTsearch*(1-exp(-plasticityDueToConditionSearch*h)) ##plasticityDueToConditionSearch
	   print(c(100*(postTsearch-finalSearch)/postTsearch,h))
	   print("*******************************************************************************")
 	   

	   plasticityDueToConditionSpeed = 10
	   
	   # print("before vs after... XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
	   # print(postTsearch)
	   finalSpeed = postTspeed*(1-exp(-plasticityDueToConditionSpeed*h)) ##plasticityDueToConditionSpeed
	   # print(c(100*(postTspeed-finalSpeed)/postTspeed,h))
	   # print("*******************************************************************************")



		##@@Mario: New equation also for tunning after encounters
		##NEED AN h FOR THE EQUATION BELOW - hEnc = encounters/max encounters in the populatio
		maxEncountersPerDayInPop = 2 ##@@Mario - need this counter for each species
		todayEncountersWithPredators = 0 ##@@Mario - test that this encounter works
		
		if(todayEncountersWithPredators>0){##@@Mario - include this exception
		
			hEnc = (1-(todayEncountersWithPredators/maxEncountersPerDayInPop)) 
			##@@Mario: study using an hRisk with the maximum perceived risk by
			##the individual over the simulation.
			if(hEnc<0) hEnc = 0 ##@@Mario
			##@@Mario: change decreaseOnTraitsDueToEncounters to activityUnderPredationRisk
			# decreaseOnTraitsDueToEncounters = 3.0 #@Parameters: @@Traits: this trait is boldness as will allow higher or lower foraging activity under predation risk
			
			if(hEnc >= 0){
				vorAfterEncounters = finalVoracity*(1-exp(-decreaseOnTraitsDueToEncounters*hEnc))
				searchAfterEncounters = finalSearch*(1-exp(-decreaseOnTraitsDueToEncounters*hEnc))
			}
		
			##@@Mario: this is to avoid highly bold individuals to feed beyond what vorariciy states
			# if(vorAfterEncounters<finalVoracity){
			  # finalVoracity=vorAfterEncounters
			# }
		
		}else{
		
			vorAfterEncounters = finalVoracity
			searchAfterEncounters = finalSearch
		}
		
		##@@@MARIO: timeStep needs to be applied after all possible transformations, otherwise, 
		##plasticityDuToConditionSearch scales non-linearly. These parameters must refer always to basic 1 day timeStep
		searchAfterEncounters = searchAfterEncounters*timeStep ##As done in Weaver as on 10/1/2025

		

		capta_all$finalVoracity[i] = finalVoracity
		 
		}else{ ##else of "if(preFinalVoracity..."

			capta_all$finalVoracity[i] = 0
			finalVoracity = 0
			vorAfterEncounters = finalVoracity

		}

	# capta_all$vorAfterEncounters[i]<-vorAfterEncounters ##+ capta_all$mr[i]


	capta_all$maxMetabolicDryMassLoss[i] = getTotalMetabolicDryMassLoss(wetMass=capta_all$currentWetMass[i], 
		bMet[1], actE_met[1], actE_search[1], temperature, 
		proportionOfTimeTheAnimalWasMoving = 1, search_area=searchAfterEncounters, 
		simulationType) 
		
	capta_all$vorAfterEncounters[i]<-vorAfterEncounters ##/assim[1] ##+ capta_all$maxMetabolicDryMassLoss[i]

	rm(vorAfterEncounters)
	# rm(h)

	if(includeMetabolicRate){
		if(i==1){
		   capta_all$netGrowth[i] <- capta_all$vorAfterEncounters[i]*assim[1]-capta_all$mr[i]+(eggDryMass+eggDryMass*factorEggMass[1])
		}else{
			if(i<nrow(capta_all) & capta_all$reproCounter[i]>capta_all$reproCounter[i-1]){ ##it reproduces
				if(capta_all$reproCounter[i-1]==0){
					capta_all$netGrowth[i] <- capta_all$vorAfterEncounters[i]*assim[1]-capta_all$mr[i]+capta_all$netGrowth[i-1]-capta_all$clutchMass[i]  ##remove clutch mass from this reproduction
				}else{
					capta_all$netGrowth[i] <- capta_all$vorAfterEncounters[i]*assim[1]-capta_all$mr[i]+capta_all$netGrowth[i-1]-capta_all$clutchMass[i-1]  ##remove clutch mass from previous reproduction
				}
			}else{
				capta_all$netGrowth[i] <- capta_all$vorAfterEncounters[i]*assim[1]-capta_all$mr[i]+capta_all$netGrowth[i-1]
			}
		}
	}else{

		if(i==1){
		   capta_all$netGrowth[i] <- capta_all$vorAfterEncounters[i]*assim[1]+(eggDryMass+eggDryMass*factorEggMass[1])
		}else{
			if(i<nrow(capta_all) & capta_all$reproCounter[i]>capta_all$reproCounter[i-1]){ ##it reproduces
				if(capta_all$reproCounter[i-1]==0){
					capta_all$netGrowth[i] <- capta_all$vorAfterEncounters[i]*assim[1]+capta_all$netGrowth[i-1]-capta_all$clutchMass[i]  ##remove clutch mass from this reproduction
				}else{
					capta_all$netGrowth[i] <- capta_all$vorAfterEncounters[i]*assim[1]+capta_all$netGrowth[i-1]-capta_all$clutchMass[i-1]  ##remove clutch mass from previous reproduction
				}
			}else{
				capta_all$netGrowth[i] <- capta_all$vorAfterEncounters[i]*assim[1]+capta_all$netGrowth[i-1]
			}
		}

	}
	
##To avoid unwanted exits from the function after having all reproductions accomplished
	# if(capta_all$reproCounter[i] == femaleMaxReproductionEvents){
		# capta_all$netGrowth[i] = capta_all$netGrowth[i-1]
	# }


	
	##back to the growth curve - very artificial but it is to test where the problem is
	# if(capta_all$netGrowth[i]>capta_all$nextMassPredicted[i])
	# {
		# capta_all$netGrowth[i]=capta_all$nextMassPredicted[i]
	# }
	
	rm(theMass)
	rm(theWetMass)
	
	# if(reproCounter == femaleMaxReproductionEvents)
	# {
		  # turnOn=FALSE
		  # # # i=1000000
	# }
	
	i=i+1

}




# > ageForNextReproduction
# [1] 363.1
# > timeOfReproEvent
# [1] 195.1


# postTsearch = postTsearch*timeStep

capta_all$voracity_ini<-voracity_ini[1]

capta_all$postTsearch<-postTsearch
capta_all$postTspeed<-postTspeed
capta_all$finalSearch<-finalSearch
capta_all$finalSpeed<-finalSpeed
capta_all$searchAfterEncounters<-searchAfterEncounters

capta_all$targetMasses<-targetMasses
capta_all$minNextDinoMassPredicted<-minNextDinoMassPredicted
capta_all$maxNextDinoMassPredicted<-maxNextDinoMassPredicted



wetMass<-capta_all$currentDryMass*conversionToWetMass
nextMass<-capta_all$nextMassPredicted*conversionToWetMass
maxNextMass<-capta_all$maxNextDinoMassPredicted*conversionToWetMass


timePlot<-capta_all


currentAge<-timePlot$currentAge
currentDryMass<-timePlot$currentDryMass


	##"Fake" allometry to test animals such as collembola with indeterminate growth
	if(assimWithBodyMass){
	    
		
		##This is because the net growth is strongly non-linear
		cutPos<-round(nrow(timePlot)*0.5)
		smaller<-timePlot$currentDryMass[1:cutPos]
		larger<-timePlot$currentDryMass[(cutPos+1):nrow(timePlot)]
		
		OptAssim<-function(a,dryMass,assim){
		
		   MSE = (sum(assim - a*dryMass^0.65)^2)/length(dryMass)
		
		return(MSE)
		}

		
		returnAssim<-function(currentDryMass)
		{
		
		ys<-vector()
		as<-vector()
		pos=1
		for(i in seq(0,1000,length.out=10000)){
			 as[pos]<-i
			 ys[pos]<-OptAssim(i,currentDryMass,assim[1])
			 # print(i,y)
			 pos=pos+1
		}
		
		# min(ys)
		finalA<-as[which(ys==min(ys))]
		
		finalAssim = finalA*currentDryMass^0.65
		
		return(finalAssim)
		}
		
		
		smallAssim<-returnAssim(smaller)
		smallVor<-timePlot$vorAfterEncounters[1:cutPos]
		smallMet<-timePlot$maxMetabolicDryMassLoss[1:cutPos]
				
		smallNetGrowth <- smallVor*smallAssim-smallMet+smaller
	
		largeAssim<-returnAssim(larger)
		largeVor<-timePlot$vorAfterEncounters[(cutPos+1):nrow(timePlot)]
		largeMet<-timePlot$maxMetabolicDryMassLoss[(cutPos+1):nrow(timePlot)]
				
		largeNetGrowth <- largeVor*largeAssim-largeMet+larger

		netGrowth<-c(smallNetGrowth,largeNetGrowth)
	    
	
	}else{
	
		netGrowth <- timePlot$vorAfterEncounters*assim[1]-timePlot$maxMetabolicDryMassLoss+timePlot$currentDryMass

    }
	
	# capta_all$netGrowth<-netGrowth

	viabilityPlot(timePlot,currentAge,capta_all$currentDryMass,capta_all$netGrowth)
	



	subTime<-timePlot[timePlot$finalVoracity > 0,]

	m1<-glm(finalVoracity~log(currentDryMass),data=subTime,family=gaussian(link=log))
	m2<-glm(vorAfterEncounters~log(currentDryMass),data=subTime,family=gaussian(link=log))
	summary(m1)
	summary(m2)

	exp(coef(m1)[1])
	coef(m1)[2]

	predA<-exp(coef(m2)[1])
	predB<-coef(m2)[2]

	predVor = predA*subTime$currentDryMass^predB

	##percentage of body size ingested
	predVor/subTime$currentDryMass


return(list(netGrowth=capta_all$netGrowth,nextMassPredicted=timePlot$nextMassPredicted,mature=mature,thisAnimalData=timePlot))
} ##End tryParams() function






# plot(log(timePlot$currentDryMass),log(timePlot$vorAfterEncounters))
# abline(lm(log(vorAfterEncounters)~log(currentDryMass),data=timePlot))







# 0.000417 j/h

# 1 mg = 7 Jules

# mg/day
# 0.000417*24/7
# [1] 0.001429714

# 0.001429714/0.030000  ##both are wet masses
# [1] 0.04765713  ##5% of body mass per day

# hist(subTime$maxMetabolicDryMassLoss/subTime$currentDryMass,breaks=50)
# > femaleWetMass
# [1] 0.02011001





if(simulationType %in% "arthropods"){

##@@Note: simulate at intermediate predation risk
##@@Note: always field metabolic rate at its maximum effort (distance moved)
##@@Note: very high values of voracity or boldness after encounters are not useful because they give a value of 1 to (1-exp(-k))
n_parm_vals = 10
n_combs = 1

	# h_enhancementS = seq(0,1,length.out=n_parm_vals) 
	
	newASjuv = seq(0.1,0.1,length.out=n_parm_vals) 
	
	newASadu = seq(0.1,0.1,length.out=n_parm_vals) 
	
	vorMinS = seq(vorMin,vorMax,length.out=n_parm_vals) 

 
	# newASjuv = seq(0.05,0.3,length.out=n_parm_vals) 

	# newASadu = seq(0.05,0.2,length.out=n_parm_vals) 
	
	# vorMinS = seq(95,100,length.out=n_parm_vals) 

	
	# vorMax = vorMin
		
	# decreaseOnTraitsDueToEncountersS = seq(1,5,length.out=n_parm_vals) #@Parameters: @@Traits: this trait is boldness

	allCombs<-expand.grid(x=newASjuv,y=newASadu,z=vorMinS)
 
	subCombs<-allCombs[sample(nrow(allCombs),n_combs),]
	
	vals<-list()
	
	for(i in 1:nrow(subCombs)){
	
	  vals[[i]]<-tryParams(subCombs[i,])
	
	# print(i)
	}


}



# min(MSE)
# [1] 2.925476e-09

# subCombs[which(MSE == min(MSE)),]
            # x y        z
# 836 0.5444444 4 4.555556

###optimization of Parameters

##for dinos
if(simulationType %in% "dinosaurs"){

##@@Note: simulate at intermediate predation risk
##@@Note: always field metabolic rate at its maximum effort (distance moved)
##@@Note: very high values of voracity or boldness after encounters are not useful because they give a value of 1 to (1-exp(-k))
n_parm_vals = 10
n_combs = 1

	#newAS instead of hEnhanceS does not work for Trex
	# newAS = seq(152,500,length.out=n_parm_vals) 
	
	hEnhanceS = seq(0,0,length.out=n_parm_vals) 
	
	vorMinS = seq(vorMin,vorMax,length.out=n_parm_vals) 
	
	# vorMax = vorMin
	
	decreaseOnTraitsDueToEncountersS = seq(1,5,length.out=n_parm_vals) #@Parameters: @@Traits: this trait is boldness

	allCombs<-expand.grid(x=hEnhanceS,y=vorMinS,z=decreaseOnTraitsDueToEncountersS)
 
	subCombs<-allCombs[sample(nrow(allCombs),n_combs),]
	
	vals<-list()
	
	for(i in 1:nrow(subCombs)){
	
	  vals[[i]]<-tryParams(subCombs[i,])
	
	print(i)
	}
}


# min(MSE)
# [1] 0.4665768
 
# subCombs[which(MSE == min(MSE)),]
    # x         y        z
# 530 1 0.2222222 3.222222



if(simulationType %in% "aquatic"){

##@@Note: simulate at intermediate predation risk
##@@Note: always field metabolic rate at its maximum effort (distance moved)
##@@Note: very high values of voracity or boldness after encounters are not useful because they give a value of 1 to (1-exp(-k))
n_parm_vals = 10
n_combs = 1

	# h_enhancementS = seq(0,1,length.out=n_parm_vals) 
	
	newASjuv = seq(0.1,0.1,length.out=n_parm_vals) 

	newASadu = seq(0.1,0.1,length.out=n_parm_vals) 
	
	vorMinS = seq(vorMin,vorMax,length.out=n_parm_vals) 
	
	# vorMax = vorMin
		
	# decreaseOnTraitsDueToEncountersS = seq(1,5,length.out=n_parm_vals) #@Parameters: @@Traits: this trait is boldness

	allCombs<-expand.grid(x=newASjuv, y=newASadu,z=vorMinS)
 
	subCombs<-allCombs[sample(nrow(allCombs),n_combs),]
	
	vals<-list()
	
	for(i in 1:nrow(subCombs)){
	
	  vals[[i]]<-tryParams(subCombs[i,])
	
	print(i)
	}


}



mse<-function(x,theMode){
  # # x<-vals[[1]]
 if(sum(x[[1]]>x[[2]])==theMode){ ##This is to include only positive values through te entire curve
   MSE<-sum((x[[1]]-x[[2]])^2)/length(x[[1]])
  }else{
   MSE=1
  }
return(MSE)
}


library(DescTools)

MSE<-vector()  
sums<-vector()

for(i in 1:nrow(subCombs)){
   
   presums<-vector()
  
   for(j in 1:length(vals[[i]]$netGrowth)){
      presums[j]<-vals[[i]]$netGrowth[j]>vals[[i]]$nextMassPredicted[j]
   }
   
   sums[i]<-sum(presums)

}



modeViabilityValue = Mode(sums)

modeViabilityValue = max(sums)


sums

modeViabilityValue

for(i in 1:nrow(subCombs)){

   MSE[i]<-mse(vals[[i]],modeViabilityValue)

}


subCombs<-subCombs[which(MSE != 1),]
MSE<-MSE[which(MSE != 1)]

print(min(MSE))

print(subCombs[which(MSE == min(MSE)),])


which(MSE == min(MSE))

finalList<-tryParams(subCombs[which(MSE == min(MSE))[1],])

viabilityPlot(finalList$thisAnimalData,finalList$thisAnimalData$currentAge,finalList$thisAnimalData$currentDryMass,finalList$thisAnimalData$netGrowth)

# plot(vals[[1]]$thisAnimalData$currentAge,vals[[1]]$thisAnimalData$h)
# abline(h=1,lwd=3,col="red",lty=2)

# plot(vals[[1]]$thisAnimalData$currentAge,vals[[1]]$thisAnimalData$vorAfterEncounters)

# plot(vals[[1]]$thisAnimalData$currentAge,vals[[1]]$thisAnimalData$targetMasses)

# plot(vals[[1]]$thisAnimalData$currentAge,vals[[1]]$thisAnimalData$vorAfterEncounters,xlim=c(0,20),ylim=c(0,0.0005))


# [1] "h: "              "2.20683464075587"
# [1] "next: "             "0.0136336288282066"
# [1] "previous: "         "0.0118447898995555"
# [1] "current: "          "0.0096859571137268"


# 1-((0.0136336288282066-0.0096859571137268)/(0.0136336288282066-0.0118447898995555))

# vals[[1]]$thisAnimalData$ageForNextReproduction
# vals[[1]]$thisAnimalData$currentDryMass
# vals[[1]]$thisAnimalData$currentAge
# vals[[1]]$thisAnimalData$clutchMass

# vals[[1]]$thisAnimalData$reproCounter
# vals[[1]]$thisAnimalData$massForNextReproduction
# vals[[1]]$thisAnimalData$beyondCurve

# plot(vals[[1]]$thisAnimalData$currentAge,vals[[1]]$thisAnimalData$deficit)

# plot(vals[[1]]$thisAnimalData$h,vals[[1]]$thisAnimalData$deficit)


# plot(vals[[1]]$thisAnimalData$currentAge,vals[[1]]$thisAnimalData$currentMass)

# plot(vals[[1]]$thisAnimalData$currentAge,vals[[1]]$thisAnimalData$netGrowth)


# plot(vals[[1]]$thisAnimalData$currentAge,vals[[1]]$thisAnimalData$massForNextReproduction)


sum(vals[[1]]$thisAnimalData$beyondMax %in% "predVor")/nrow(vals[[1]]$thisAnimalDat)


#This is to test Grady's 2014 Science equation BMR=0.6*Gmax - strong missmatch as observed at Metabolic_loss_according_to_Grady_2014.xlsx 
# forSlope<-data.frame(x=vals[[1]]$thisAnimalData$currentAge,y=vals[[1]]$thisAnimalData$currentDryMass)


# plot(capta_all$currentAge,capta_all$netGrowth)

# plot(capta_all$currentAge[1:11873],capta_all$netGrowth[1:11873])
# plot(capta_all$currentAge[11000:11873],capta_all$netGrowth[11000:11873])

# plot(capta_all$beyondMax[11000:11873],capta_all$beyondMax[11000:11873])


# vals[[1]]$thisAnimalData$beyondMax[11000:11873]

# plot(vals[[1]]$thisAnimalData$currentAge[1:11873],vals[[1]]$thisAnimalData$netGrowth[1:11873])

# plot(vals[[1]]$thisAnimalData$currentAge[1:12047],vals[[1]]$thisAnimalData$netGrowth[1:12047])

# plot(vals[[1]]$thisAnimalData$currentAge[1:12070],vals[[1]]$thisAnimalData$netGrowth[1:12070])

# vals[[1]]$thisAnimalData$mature[1:13000]

# vals[[1]]$thisAnimalData$beyondMax[11000:12070]
# vals[[1]]$thisAnimalData$beyondCurve[11000:12070]

# plot(vals[[1]]$thisAnimalData$currentAge[1:12070],vals[[1]]$thisAnimalData$deficit[1:12070])

# plot(vals[[1]]$thisAnimalData$currentAge[1:12070],vals[[1]]$thisAnimalData$finalVoracity[1:12070])

# plot(vals[[1]]$thisAnimalData$currentAge[1:12000],vals[[1]]$thisAnimalData$h[1:12000])


# vals[[1]]$thisAnimalData$beyondMax[4900:5100]


# plot(vals[[1]]$thisAnimalData$currentDryMass[1:12000],vals[[1]]$thisAnimalData$h[1:12000])

# plot(vals[[1]]$thisAnimalData$currentAge[11000:17000],vals[[1]]$thisAnimalData$h[11000:17000])

# plot(vals[[1]]$thisAnimalData$currentAge[11000:17000],vals[[1]]$thisAnimalData$netGrowth[11000:17000])

# plot(vals[[1]]$thisAnimalData$currentAge[11700:12000],vals[[1]]$thisAnimalData$netGrowth[11700:12000])

# plot(vals[[1]]$thisAnimalData$currentAge[11700:12000],vals[[1]]$thisAnimalData$finalVoracity[11700:12000])

vals[[1]]$thisAnimalData$beyondCurve[1:nrow(vals[[1]]$thisAnimalData)]
vals[[1]]$thisAnimalData$beyondMax[1:nrow(vals[[1]]$thisAnimalData)]

# plot(vals[[1]]$thisAnimalData$currentAge,vals[[1]]$thisAnimalData$finalVoracity)

# plot(vals[[1]]$thisAnimalData$currentAge,vals[[1]]$thisAnimalData$netGrowth)


# plot(vals[[1]]$thisAnimalData$currentAge[11500:nrow(vals[[1]]$thisAnimalData)],vals[[1]]$thisAnimalData$netGrowth[11500:nrow(vals[[1]]$thisAnimalData)],type="n")
# lines(vals[[1]]$thisAnimalData$currentAge[11500:nrow(vals[[1]]$thisAnimalData)],vals[[1]]$thisAnimalData$netGrowth[11500:nrow(vals[[1]]$thisAnimalData)],col="red",lwd=1)


# plot(vals[[1]]$thisAnimalData$currentAge[12000:12300],vals[[1]]$thisAnimalData$netGrowth[12000:12300],type="n")
# lines(vals[[1]]$thisAnimalData$currentAge[12000:12300],vals[[1]]$thisAnimalData$netGrowth[12000:12300],col="red",lwd=1)


# vals[[1]]$thisAnimalData$beyondCurve[12000:12300]
# vals[[1]]$thisAnimalData$beyondMax[12000:12300]

# lines(vals[[1]]$thisAnimalData$currentAge[12000:12300],vals[[1]]$thisAnimalData$netGrowth[12000:12300],col="red",lwd=1)
# lines(vals[[1]]$thisAnimalData$currentAge[12000:12300],vals[[1]]$thisAnimalData$currentDryMass[12000:12300],col="black",lwd=3)

# plot(vals[[1]]$thisAnimalData$currentAge[12000:12300],vals[[1]]$thisAnimalData$finalVoracity[12000:12300],type="n")
# lines(vals[[1]]$thisAnimalData$currentAge[12000:12300],vals[[1]]$thisAnimalData$finalVoracity[12000:12300],col="black",lwd=3)

# plot(vals[[1]]$thisAnimalData$currentAge[12000:12300],vals[[1]]$thisAnimalData$preFinalVoracity[12000:12300],type="n")
# lines(vals[[1]]$thisAnimalData$currentAge[12000:12300],vals[[1]]$thisAnimalData$preFinalVoracity[12000:12300],col="black",lwd=3)

# plot(vals[[1]]$thisAnimalData$currentAge[12000:12300],vals[[1]]$thisAnimalData$deficit[12000:12300],type="n")
# lines(vals[[1]]$thisAnimalData$currentAge[12000:12300],vals[[1]]$thisAnimalData$deficit[12000:12300],col="black",lwd=3)


# plot(vals[[1]]$thisAnimalData$preFinalVoracity[12000:12300],vals[[1]]$thisAnimalData$deficit[12000:12300],type="n")
# lines(vals[[1]]$thisAnimalData$preFinalVoracity[12000:12300],vals[[1]]$thisAnimalData$deficit[12000:12300],col="black",lwd=3)


# plot(vals[[1]]$thisAnimalData$preFinalVoracity[12000:12300],vals[[1]]$thisAnimalData$predVor[12000:12300])

 # capta_all$netGrowth[i] <- capta_all$vorAfterEncounters[i]*assim[1]-capta_all$mr[i]+capta_all$netGrowth[i-1]
 
 # predVor=(capta_all$nextMassPredicted[i]+capta_all$mr[i]-(capta_all$nextMassPredicted[i-1]))/assim[1] ##where assim must be minimum assim (like feeding on the least profitable prey)	   

# plot(vals[[1]]$thisAnimalData$vorAfterEncounters[12000:12300],vals[[1]]$thisAnimalData$predVor[12000:12300])
# plot(vals[[1]]$thisAnimalData$predVor[12000:12300],vals[[1]]$thisAnimalData$vorAfterEncounters[12000:12300])

# plot(vals[[1]]$thisAnimalData$currentAge,vals[[1]]$thisAnimalData$predVor)
