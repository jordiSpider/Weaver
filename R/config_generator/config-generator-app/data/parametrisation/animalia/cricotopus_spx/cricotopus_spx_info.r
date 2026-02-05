# @@@@ species: cricotopus_spx

if (!"app" %in% ls()) {
	rm(list=ls(all=TRUE))
	
	script_path <- dirname(sys.frame(1)$ofile)
}

parametrisation_path <- sub("(.*/parametrisation)/.*", "\\1", script_path)


source(file.path(parametrisation_path, "utilities", "codes", "functions", "growth_curve_modelfitting", "gcmGAT.r"))
source(file.path(parametrisation_path, "utilities", "codes", "functions", "TPCs", "calculateTSR.r"))
source(file.path(parametrisation_path, "utilities", "codes", "functions", "TPCs", "calculatePawarTPC.r"))
source(file.path(parametrisation_path, "utilities", "codes", "functions", "calculateIndividualsPerInstarCricotopus.r"))


set.seed(123)


# setwd(dirname(sys.frame(1)$ofile))

# Option to avoid plotting 
# options(allow_plotting = FALSE)


# Start reading ID_CRISPX_tempFromLab data frame
ID_CRISPX_tempFromLab<-data.frame(
			Variable = "tempFromLab",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = NA
)
# End of ID_CRISPX_tempFromLab data frame
tempFromLab<-10
tempFromLab #@tempFromLab=10


# Start reading ID_CRISPX_sexRatio data frame		  
ID_CRISPX_sexRatio<-data.frame(
			Variable = "sexRatio", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = NA
)
# End of ID_CRISPX_sexRatio data frame

sexRatio<-0.5
sexRatio #@sexRatio=0.5

# Start reading ID_CRISPX_conversionToWetMass data frame
ID_CRISPX_conversionToWetMass<-data.frame(
			Variable = "conversionToWetMass", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Taken from script testGrowthAndReproduction_JML_MCS_fixing_and_testing_temperature_vs2"
)

# End of ID_CRISPX_conversionToWetMass data frame

conversionToWetMass<-9.193
conversionToWetMass #@conversionToWetMass=9.193


# Start reading ID_CRISPX_eggsPerBatch data frame
ID_CRISPX_eggsPerBatch<-data.frame(
			Variable = "eggsPerBatch",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "group decision"
)
# End of ID_CRISPX_eggsPerBatch data frame

eggsPerBatch<-100
eggsPerBatch #@eggsPerBatch=250 #@eggsPerBatch=20

# Start reading ID_CRISPX_femaleMaxReproductionEvents data frame
ID_CRISPX_femaleMaxReproductionEvents<-data.frame(
			Variable = "femaleMaxReproductionEvents", 
			DOI1 = NA,
			Category1 = NA, 
			DataTreatment = "group decision"
)
# End of ID_CRISPX_femaleMaxReproductionEvents data frame

femaleMaxReproductionEvents<-1
femaleMaxReproductionEvents #@femaleMaxReproductionEvents=2 #@femaleMaxReproductionEvents=5
##@@TO DO: ASK

# Start reading ID_CRISPX_fieldDensity data frame
ID_CRISPX_fieldDensity<-data.frame(
			Variable = "fieldDensity", 
			DOI1 = NA,
			Category1 = NA, 
			DataTreatment = "extracted from json in the folder"
)
# End of ID_CRISPX_fieldDensity data frame

fieldDensity=2191  ##inds/sq_meter
fieldDensity=fieldDensity/1e+6 ##inds/sq_mm

# Start reading ID_CRISPX_ageVector data frame
ID_CRISPX_ageVector<-data.frame(
			Variable = "ageVector",
			DOI1 = NA ,
			Category1 = NA ,
			DataTreatment = NA 
)		
# End of ID_CRISPX_ageVector data frame

ageVector<-c(7.47, 13.39, 20.14, 29.04)
ageVector #@ageVector=c(7.47,13.39,20.14,29.04)

# Start reading ID_CRISPX_instarFirstReproduction data frame
ID_CRISPX_instarFirstReproduction<-data.frame(
			Variable = "instarFirstReproduction",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "Mentioned in the text"
)
# End of ID_CRISPX_instarFirstReproduction data frame

instarFirstReproduction<-length(ageVector)+1
instarFirstReproduction #@instarFirstReproduction=5

# Start reading ID_CRISPX_coefficientForMassA data frame
ID_CRISPX_coefficientForMassA<-data.frame(
			Variable = "coefficientForMassA", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Extracted from the script testGrowthAndReproduction_JML_MCS_fixing_and_testing_temperature_vs2"
)
# End of ID_CRISPX_coefficientForMassA data frame

coefficientForMassA<-0.002
coefficientForMassA #@coefficientForMassA=0.002


# Start reading ID_CRISPX_scaleForMassB data frame
ID_CRISPX_scaleForMassB<-data.frame(
			Variable = "scaleForMassB", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Extracted from the script testGrowthAndReproduction_JML_MCS_fixing_and_testing_temperature_vs2"
)
# End of ID_CRISPX_scaleForMassB data frame

scaleForMassB<-2.254
scaleForMassB #@scaleForMassB=2.254


# Start reading ID_CRISPX_coefficientForMassAforMature data frame
ID_CRISPX_coefficientForMassAforMature<-data.frame(
			Variable = "coefficientForMassAforMature", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Extracted from the script testGrowthAndReproduction_JML_MCS_fixing_and_testing_temperature_vs2"
)
# End of ID_CRISPX_coefficientForMassAforMature data frame

coefficientForMassAforMature<-coefficientForMassA
coefficientForMassAforMature #@coefficientForMassAforMature=0.002


# Start reading ID_CRISPX_scaleForMassBforMature data frame
ID_CRISPX_scaleForMassBforMature<-data.frame(
			Variable = "scaleForMassBforMature", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Extracted from the script testGrowthAndReproduction_JML_MCS_fixing_and_testing_temperature_vs2"
)
# End of ID_CRISPX_scaleForMassBforMature data frame

scaleForMassBforMature<-scaleForMassB
scaleForMassBforMature #@scaleForMassBforMature=2.254





##############################################################################
##############################################################################
##############################################################################


# Data described as in DataTreatment
data_all_models <- read.table(file.path(script_path, "data_all_models.txt"), header = TRUE, sep = "")


##############################################################################
##############################################################################


# Fit the model using the converted data
model_scope_radius_min <- glm(Mass ~ log(ScopeRadiusMin), family = gaussian(link = "log"), data = data_all_models)
model_scope_radius_max <- glm(Mass ~ log(ScopeRadiusMax), family = gaussian(link = "log"), data = data_all_models)

# Extract updated model coefficients
coeffMassForScopeRadiusMin <- exp(coef(model_scope_radius_min)[1])  # Intercept transformed back to original scale
scaleMassForScopeRadiusMin <- coef(model_scope_radius_min)[2]       # Slope

coeffMassForScopeRadiusMax <- exp(coef(model_scope_radius_max)[1])  # Intercept transformed back to original scale
scaleMassForScopeRadiusMax <- coef(model_scope_radius_max)[2]       # Slope

if(is.na(scaleMassForScopeRadiusMin)) {
	coeffMassForScopeRadiusMin <- mean(data_all_models$ScopeRadiusMin)
	coeffMassForScopeRadiusMin #@coeffMassForScopeRadiusMin=

	scaleMassForScopeRadiusMin <- 0.0 
	scaleMassForScopeRadiusMin #@scaleMassForScopeRadiusMin=
} else {
	coeffMassForScopeRadiusMin <- as.vector(t(coeffMassForScopeRadiusMin))
	coeffMassForScopeRadiusMin #@coeffMassForScopeRadiusMin=

	scaleMassForScopeRadiusMin <- as.vector(t(scaleMassForScopeRadiusMin))
	scaleMassForScopeRadiusMin <- round(as.numeric(scaleMassForScopeRadiusMin),4) 
	scaleMassForScopeRadiusMin #@scaleMassForScopeRadiusMin=
}

if(is.na(scaleMassForScopeRadiusMax)) {
	coeffMassForScopeRadiusMax <- mean(data_all_models$ScopeRadiusMax)
	coeffMassForScopeRadiusMax #@coeffMassForScopeRadiusMax=

	scaleMassForScopeRadiusMax <- 0.0 
	scaleMassForScopeRadiusMax #@scaleMassForScopeRadiusMax=
} else {
	coeffMassForScopeRadiusMax <- as.vector(t(coeffMassForScopeRadiusMax))
	coeffMassForScopeRadiusMax #@coeffMassForScopeRadiusMax=

	scaleMassForScopeRadiusMax <- as.vector(t(scaleMassForScopeRadiusMax))
	scaleMassForScopeRadiusMax <- round(as.numeric(scaleMassForScopeRadiusMax),4) 
	scaleMassForScopeRadiusMax #@scaleMassForScopeRadiusMax=
}

##############################################################################
##############################################################################


# Fit the model using the converted data
model_interaction_radius_min <- glm(Mass ~ log(InteractionRadiusMin), family = gaussian(link = "log"), data = data_all_models)
model_interaction_radius_max <- glm(Mass ~ log(InteractionRadiusMax), family = gaussian(link = "log"), data = data_all_models)

# Extract updated model coefficients
coeffMassForInteractionRadiusMin <- exp(coef(model_interaction_radius_min)[1])  # Intercept transformed back to original scale
scaleMassForInteractionRadiusMin <- coef(model_interaction_radius_min)[2]       # Slope

coeffMassForInteractionRadiusMax <- exp(coef(model_interaction_radius_max)[1])  # Intercept transformed back to original scale
scaleMassForInteractionRadiusMax <- coef(model_interaction_radius_max)[2]       # Slope

if(is.na(scaleMassForInteractionRadiusMin)) {
	coeffMassForInteractionRadiusMin <- mean(data_all_models$InteractionRadiusMin)
	coeffMassForInteractionRadiusMin #@coeffMassForInteractionRadiusMin=

	scaleMassForInteractionRadiusMin <- 0.0
	scaleMassForInteractionRadiusMin #@scaleMassForInteractionRadiusMin=
} else {
	coeffMassForInteractionRadiusMin <- as.vector(t(coeffMassForInteractionRadiusMin))
	coeffMassForInteractionRadiusMin #@coeffMassForInteractionRadiusMin=

	scaleMassForInteractionRadiusMin <- as.vector(t(scaleMassForInteractionRadiusMin))
	scaleMassForInteractionRadiusMin <- round(as.numeric(scaleMassForInteractionRadiusMin),4) 
	scaleMassForInteractionRadiusMin #@scaleMassForInteractionRadiusMin=
}

if(is.na(scaleMassForInteractionRadiusMax)) {
	coeffMassForInteractionRadiusMax <- mean(data_all_models$InteractionRadiusMax)
	coeffMassForInteractionRadiusMax #@coeffMassForInteractionRadiusMax=

	scaleMassForInteractionRadiusMax <- 0.0
	scaleMassForInteractionRadiusMax #@scaleMassForInteractionRadiusMax=
} else {
	coeffMassForInteractionRadiusMax <- as.vector(t(coeffMassForInteractionRadiusMax))
	coeffMassForInteractionRadiusMax #@coeffMassForInteractionRadiusMax=

	scaleMassForInteractionRadiusMax <- as.vector(t(scaleMassForInteractionRadiusMax))
	scaleMassForInteractionRadiusMax <- round(as.numeric(scaleMassForInteractionRadiusMax),4) 
	scaleMassForInteractionRadiusMax #@scaleMassForInteractionRadiusMax=
}


##############################################################################
##############################################################################


# Fit the model using the converted data
model_search_radius_min <- glm(Mass ~ log(SearchRadiusMin), family = gaussian(link = "log"), data = data_all_models)
model_search_radius_max <- glm(Mass ~ log(SearchRadiusMax), family = gaussian(link = "log"), data = data_all_models)

# Extract updated model coefficients
coeffMassForSearchRadiusMin <- exp(coef(model_search_radius_min)[1])  # Intercept transformed back to original scale
scaleMassForSearchRadiusMin <- coef(model_search_radius_min)[2]       # Slope

coeffMassForSearchRadiusMax <- exp(coef(model_search_radius_max)[1])  # Intercept transformed back to original scale
scaleMassForSearchRadiusMax <- coef(model_search_radius_max)[2]       # Slope

if(is.na(scaleMassForSearchRadiusMin)) {
	coeffMassForSearchRadiusMin <- mean(data_all_models$SearchRadiusMin)
	coeffMassForSearchRadiusMin #@coeffMassForSearchRadiusMin=

	scaleMassForSearchRadiusMin <- 0.0
	scaleMassForSearchRadiusMin #@scaleMassForSearchRadiusMin=
} else {
	coeffMassForSearchRadiusMin <- as.vector(t(coeffMassForSearchRadiusMin))
	coeffMassForSearchRadiusMin #@coeffMassForSearchRadiusMin=

	scaleMassForSearchRadiusMin <- as.vector(t(scaleMassForSearchRadiusMin))
	scaleMassForSearchRadiusMin <- round(as.numeric(scaleMassForSearchRadiusMin),4) 
	scaleMassForSearchRadiusMin #@scaleMassForSearchRadiusMin=
}

if(is.na(scaleMassForSearchRadiusMax)) {
	coeffMassForSearchRadiusMax <- mean(data_all_models$SearchRadiusMax)
	coeffMassForSearchRadiusMax #@coeffMassForSearchRadiusMax=

	scaleMassForSearchRadiusMax <- 0.0
	scaleMassForSearchRadiusMax #@scaleMassForSearchRadiusMax=
} else {
	coeffMassForSearchRadiusMax <- as.vector(t(coeffMassForSearchRadiusMax))
	coeffMassForSearchRadiusMax #@coeffMassForSearchRadiusMax=

	scaleMassForSearchRadiusMax <- as.vector(t(scaleMassForSearchRadiusMax))
	scaleMassForSearchRadiusMax <- round(as.numeric(scaleMassForSearchRadiusMax),4) 
	scaleMassForSearchRadiusMax #@scaleMassForSearchRadiusMax=
}


##############################################################################
##############################################################################


# Fit the model using the converted data
model_speed_min <- glm(Mass ~ log(SpeedMin), family = gaussian(link = "log"), data = data_all_models)
model_speed_max <- glm(Mass ~ log(SpeedMax), family = gaussian(link = "log"), data = data_all_models)

# Extract updated model coefficients
coeffMassForSpeedMin <- exp(coef(model_speed_min)[1])  # Intercept transformed back to original scale
scaleMassForSpeedMin <- coef(model_speed_min)[2]       # Slope

coeffMassForSpeedMax <- exp(coef(model_speed_max)[1])  # Intercept transformed back to original scale
scaleMassForSpeedMax <- coef(model_speed_max)[2]       # Slope

if(is.na(scaleMassForSpeedMin)) {
	coeffMassForSpeedMin <- mean(data_all_models$SpeedMin)
	coeffMassForSpeedMin #@coeffMassForSpeedMin=

	scaleMassForSpeedMin <- 0.0
	scaleMassForSpeedMin #@scaleMassForSpeedMin=
} else {
	coeffMassForSpeedMin <- as.vector(t(coeffMassForSpeedMin))
	coeffMassForSpeedMin #@coeffMassForSpeedMin=

	scaleMassForSpeedMin <- as.vector(t(scaleMassForSpeedMin))
	scaleMassForSpeedMin <- round(as.numeric(scaleMassForSpeedMin),4) 
	scaleMassForSpeedMin #@scaleMassForSpeedMin=
}

if(is.na(scaleMassForSpeedMax)) {
	coeffMassForSpeedMax <- mean(data_all_models$SpeedMax)
	coeffMassForSpeedMax #@coeffMassForSpeedMax=

	scaleMassForSpeedMax <- 0.0
	scaleMassForSpeedMax #@scaleMassForSpeedMax=
} else {
	coeffMassForSpeedMax <- as.vector(t(coeffMassForSpeedMax))
	coeffMassForSpeedMax #@coeffMassForSpeedMax=

	scaleMassForSpeedMax <- as.vector(t(scaleMassForSpeedMax))
	scaleMassForSpeedMax <- round(as.numeric(scaleMassForSpeedMax),4) 
	scaleMassForSpeedMax #@scaleMassForSpeedMax=
}


##############################################################################
##############################################################################
##############################################################################







# Start reading ID_CRISPX_curveType data frame
ID_CRISPX_curveType<-data.frame(
			Variable = "curveType",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "we performed a model autofit adjustment using AIC criterion"
)			
# End of ID_CRISPX_curveType data frame

cricotopus_table<-read.table(file.path(script_path, "Chironomidae_Cricotopus.txt"), header=TRUE)
colnames(cricotopus_table)[which(names(cricotopus_table) == "days")] <- "age"
colnames(cricotopus_table)[which(names(cricotopus_table) == "mm")] <- "length"

head(cricotopus_table)


cricotopus_data <- split(cricotopus_table, cricotopus_table$temperature)

#   temperature  age length
# 1          10 1.58   0.58
# 2          10 1.82   0.58
# 3          10 1.63   0.65
# 4          10 2.14   0.66
# 5          10 2.76   0.72
# 6          10 2.70   0.86


# crictopus_t10<-cricotopus_table[cricotopus_table$temperature==10,]
# cricotopus_t15<-cricotopus_table[cricotopus_table$temperature==15,]
# cricotopus_t20<-cricotopus_table[cricotopus_table$temperature==20,]


fitted <- gcmGAT(
	cricotopus_data, 
	x = "age", y = "length", 
	force_model = list(
		"10" = "Linear",
		"15" = "Logistic4P_FixedA",
		"20" = "Logistic4P_FixedA"
	), 
	use_app = TRUE
)



all_tables <- do.call(rbind, lapply(fitted, function(x) x$adjusted_models_table))
rownames(all_tables) <- NULL
# print(all_tables)

# The p value showed in the table is the p value of the likelihood ratio test between the best candidate
# and the model in the current row. Example: to choose the Exponential model for the temperature 10 
# instead of the linear gives a p-value of 1.97E-02.

#    Temperature             Model       AIC      AICc       RSS best_candidate     Loglik df p_value_LRT
# 1           10       Exponential -17.64830 -17.06859 3.0635959           TRUE  12.824151  4        <NA>
# 2           10          Logistic  13.16282  13.74253 4.6457640          FALSE  -2.581409  4    2.22E-16
# 3           10            Linear -14.20658 -13.86372 3.2973732          FALSE  10.103289  3    1.97E-02
# 4           15    VonBertalanffy -28.91474 -28.08141 1.5463700          FALSE  18.457371  4    6.04E-09
# 5           15          Logistic -37.32519 -36.49185 1.3194601          FALSE  22.662593  4    4.63E-07
# 6           15 Logistic4P_FixedA -60.73721 -59.46062 0.8168910           TRUE  35.368607  5        <NA>
# 7           15            Linear  43.87606  44.36586 6.3411331          FALSE -18.938030  3    2.60E-24
# 8           20    VonBertalanffy -65.44536 -64.59430 0.7415266          FALSE  36.722682  4    5.09E-03
# 9           20          Logistic -53.56676 -52.71570 0.9318256          FALSE  30.783382  4    8.94E-06
# 10          20 Logistic4P_FixedA -71.29123 -69.98688 0.6376752           TRUE  40.645613  5        <NA>
# 11          20            Linear -15.80580 -15.30580 2.0017552          FALSE  10.902898  3    1.21E-13


# Since the tempFromLab is 10, we will use the model for this temperature with the higher Loglik supported by p_value_LRT 

curveType<-fitted[[as.character(tempFromLab)]]$best_model_name
curveType #@curveType="Exponential"


# Start reading ID_CRISPX_growthModelThermalChanges data frame
ID_CRISPX_growthModelThermalChanges<-data.frame(
			Variable = "growthModelThermalChanges",
			DOI1 = NA,
			Category1 = NA, 
			DataTreatment = NA
)			
# End of ID_CRISPX_growthModelThermalChanges data frame

growthModelThermalChanges = lapply(setdiff(names(fitted), as.character(tempFromLab)), function(temp) {
	list(
		temperature=as.numeric(temp),
		model=fitted[[temp]]$best_model_name,
		A=ifelse(fitted[[temp]]$best_model_name %in% c("Logistic4P"), fitted[[temp]]$A, 0.0)  
	)
})


# Start reading ID_CRISPX_lengthAtBirth data frame
ID_CRISPX_lengthAtBirth<-data.frame(
			Variable = "lengthAtBirth", 
			DOI1 = NA,
			Category1 = NA, 
			DataTreatment = NA 
)
# End of ID_CRISPX_lengthAtBirth data frame
best_model<-fitted[[as.character(tempFromLab)]]$best_model
lengthAtBirth<-predict(best_model, newdata = data.frame(age = 0))
lengthAtBirth #@lengthAtBirth=0.4176354

# Start reading ID_CRISPX_eggDryMass data frame	
ID_CRISPX_eggDryMass<-data.frame(
			Variable = "eggDryMass", 
			DOI1 = NA,
			Category1 = NA, 
			DataTreatment = NA 
)
# End reading ID_CRISPX_eggDryMass data frame

eggDryMass<-coefficientForMassA*lengthAtBirth^(scaleForMassB)
eggDryMass #@eggDryMass=0.0002794523

# Start reading ID_CRISPX_femaleWetMass data frame
ID_CRISPX_femaleWetMass<-data.frame(
			Variable = "femaleWetMass",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = NA
)
# End of ID_CRISPX_femaleWetMass data frame

female_predicted_length <- predict(best_model, newdata = 
                           data.frame(age = max(cricotopus_data[[as.character(tempFromLab)]])))
femaledrymass<-coefficientForMassA*female_predicted_length^scaleForMassB
femaleWetMass<-femaledrymass*conversionToWetMass
femaleWetMass  #@femaleWetMass=0.4802704

# Start reading ID_CRISPX_lMin data frame
ID_CRISPX_lMin<-data.frame(
			Variable = "lMin",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = NA
)
# End of ID_CRISPX_lMin data frame

lMin<-female_predicted_length
lMin #@lMin=6.568956

# Start reading ID_CRISPX_lMax data frame
ID_CRISPX_lMax<-data.frame(
			Variable = "lMax",
			DOI1 = NA,
			Category1 = NA, 
			DataTreatment = NA
)
# End of ID_CRISPX_lMax data frame

lMax<-female_predicted_length
lMax #@lMax=6.568956

##@@NOTE: The values of lMin and lMax are the same as the predicted for the model due 
########  to the lack of variability for the adult's length

# Start reading ID_CRISPX_growthK data frame
ID_CRISPX_growthK<-data.frame(
			Variable = "growth",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = NA
)
# End of ID_CRISPX_growthK data frame


growthK<-fitted[[as.character(tempFromLab)]]$growth
growthK #@growthK= 0.00881962

# Start reading ID_CRISPX_pupaPeriodTime  data frame
ID_CRISPX_pupaPeriodTime <-data.frame(
			Variable = "pupaPeriodTime",
			DOI1 = NA,
			Category1 = NA, 
			DataTreatment = NA
)
# End of ID_CRISPX_pupaPeriodTime  data frame
lm(length~age, data=cricotopus_data[[as.character(tempFromLab)]])
pupaPeriodTime <- 1
pupaPeriodTime  #@pupaPeriodTime =1


# Start reading ID_CRISPX_longevitySinceMaturation data frame
ID_CRISPX_longevitySinceMaturation<-data.frame(
			Variable = "longevitySinceMaturation",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Taken from the JSON in the folder"
)
# End of ID_CRISPX_longevitySinceMaturation data frame


adult_time <- 10.0

longevitySinceMaturation <- 1.0 + (adult_time / (ageVector[length(ageVector)] + pupaPeriodTime))
longevitySinceMaturation #@longevitySinceMaturation= 


# Start reading ID_CRISPX_growthKmin data frame
ID_CRISPX_growthKmin<-data.frame(
			Variable = "growthKmin",
			DOI1 =  NA,
			Category1 = NA,
			DataTreatment = NA
)
# End of ID_CRISPX_growthKmin data frame

growthKmin<-growthK


# Start reading ID_CRISPX_growthKmax data frame
ID_CRISPX_growthKmax<-data.frame(
			Variable = "growthKmax",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = NA
)
# End of ID_CRISPX_growthKmax data frame

growthKmax<-growthK

##@@NOTE: like for lMin and lMax, we dont have a range of values for length to calculate 
########  different growthK so we will use the same value for growthKmin and growthKmax

# Start reading ID_CRISPX_assiM data frame
ID_CRISPX_assiM<-data.frame(
			Variable = "assiM",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = NA
)
# End of ID_CRISPX_assiM data frame

assiM<-0.316



# SECTION 2: In this section there are included all the parameters for WEAVER. 
# Some of them are directly inputed from the parametric values obtained in the previous section.
# The rest of the parameters are taken from the literature or there are set up by 
# programming criteria or by the user (this preferences must be understood before changing).

# =====================
# GENERAL PARAMETERS
# =====================

json.activatedHandling = FALSE  # If TRUE, predator handling time is considered for the simulation
json.activityUnderPredationRisk = 1.0  # Fraction of normal activity maintained under predation risk
json.poreUsePerInstar = c(1.0, 1.0, 1.0, 1.0, 1.0)  # Pore use for each developmental instar
json.coefficientForHandlingTimeA = 28.407  # Coefficient 'A' for handling time equation
json.coefficientForHandlingTimeB = -0.664  # Coefficient 'B' for handling time equation
json.coefficientForHandlingTimeC = -6.365  # Coefficient 'C' for handling time equation
json.conversionToWetMass = c(
	conversionToWetMass, conversionToWetMass, conversionToWetMass,
	conversionToWetMass, conversionToWetMass
)  # Factor to convert from dry mass (or another measure) to wet mass
json.daysWithoutFoodForMetabolicDownregulation = 2.0  # Days without food before metabolic downregulation
json.debug.surviveWithoutFood = FALSE  # Debug flag to allow survival without food if TRUE
json.debug.forcePresenceAllResourcesInvolved = FALSE  # If TRUE, forces all resource types to be present in a simulation
json.defaultHuntingMode = "grazer"  # Default hunting mode if no other is specified
json.femaleMaxReproductionEvents = femaleMaxReproductionEvents  # Maximum number of reproduction events for a female
json.forDensitiesA = 74.8  # Parameter 'A' for population density functions
json.forDensitiesB = -0.75  # Parameter 'B' for population density functions
json.forDensitiesEggSize = 0.005  # Egg size used in some density-related calculations
json.forDensitiesGrowth = 1.4  # Growth rate factor in density-related calculations

# =====================
# GENETICS PARAMETERS
# =====================

json.genetics.modules.correlationCoefficientRhoPerModule = c(0.1, 0.1)  # Correlation coefficients between traits in each genetic module
json.genetics.modules.traitsPerModule = 3  # Number of traits per genetic module
json.genetics.numberOfAllelesPerLocus = 10  # How many alleles exist at each locus
json.genetics.numberOfChiasmasPerChromosome = 2  # Number of chiasmas (crossover points) in meiosis per chromosome
json.genetics.numberOfLociPerTrait = 20  # Number of loci affecting each trait

# These parameters alter allele frequencies or values in the population over time.

json.genetics.modifyAlleles = list(
	list(
		affectedLocus = list(
			0.29999999999999999,
			0.59999999999999998,
			0.90000000000000002
		),
		affectedTraits = list(
			list(
				trait = "base$energy_tank",
				element = "value"
			)
		),
		affectedAlleles = 0.9,
		timesAlleles = 30
	),
	list(
		affectedLocus = list(
			0.29999999999999999,
			0.59999999999999998,
			0.90000000000000002
		),
		affectedTraits = list(
			list(
				trait = "base$energy_tank",
				element = "value"
			)
		),
		affectedAlleles = 0.7,
		timesAlleles = 30
	)
)

# =====================
# TRAITS PARAMETERS
# =====================
# The "individualLevelParams" typically means these values can vary among individuals.
# The "speciesLevelParams" typically means a constant value for the entire species.

## base - actE_met // Literature in: Ehnes et al. 2011 Ecology Letters (Mesostigmata, soil fauna)
json.traits.base.actE_met.limits.max = 0.0  # Upper limit on individual variation
json.traits.base.actE_met.limits.min = 0.0  # Lower limit on individual variation
json.traits.base.actE_met.ranges.max = 0.379  # Upper range boundary for sampling initial trait values
json.traits.base.actE_met.ranges.min = 0.379  # Lower range boundary for sampling initial trait values
json.traits.base.actE_met.restrictValue = 0.01  # Precision or incremental step for this trait

json.traits.base.actE_met.temperature.dependent = FALSE
json.traits.base.actE_met.temperature.activationEnergy = 0.0
json.traits.base.actE_met.temperature.energyDecay = 0.0
json.traits.base.actE_met.temperature.temperatureOptimal = 0.0
json.traits.base.actE_met.temperature.temperatureRef = 0.0

## base - assim // Literature in: Taken from Pergamasus longicornis in Bowman 2019
json.traits.base.assim.limits.max = 0.0  # Upper limit on assimilation rate
json.traits.base.assim.limits.min = 0.0  # Lower limit on assimilation rate
json.traits.base.assim.ranges.max = assiM  # Upper range boundary for sampling assimilation
json.traits.base.assim.ranges.min = assiM  # Lower range boundary for sampling assimilation
json.traits.base.assim.restrictValue = 0.0  # Precision or incremental step

json.traits.base.assim.temperature.dependent = FALSE
json.traits.base.assim.temperature.activationEnergy = 0.0
json.traits.base.assim.temperature.energyDecay = 0.0
json.traits.base.assim.temperature.temperatureOptimal = 0.0
json.traits.base.assim.temperature.temperatureRef = 0.0

## base - devTime
# If the user does not want genetic variation, introduce the last value 
# of ageVector for both ranges (min and max). In case of having data 
# from literature you can modify these values. 

devTime_TPC = calculatePawarTPC(
	trait_name="base.devTime",
	traits=data.frame(
		temperature=as.numeric(names(fitted)),
		traitValue=sapply(names(fitted), function(temp) {
			return(max(cricotopus_data[[temp]]$age))
		})
	),
	Tref = tempFromLab
)


json.traits.base.devTime.limits.max = 0.0  # If set NULL, there is no variation over the maximum range.
json.traits.base.devTime.limits.min = 0.0  # If set NULL, there is no variation over the minimum range.
json.traits.base.devTime.ranges.max = devTime_TPC$traitAtTopt  # If max and min = max(ageVector), no genetic variation
json.traits.base.devTime.ranges.min = devTime_TPC$traitAtTopt  # If max and min = max(ageVector), no genetic variation
json.traits.base.devTime.restrictValue = 0.0  # If NULL, No increment restriction will be applied

json.traits.base.devTime.temperature.dependent = TRUE
json.traits.base.devTime.temperature.activationEnergy = devTime_TPC$E
json.traits.base.devTime.temperature.energyDecay = devTime_TPC$ED
json.traits.base.devTime.temperature.temperatureOptimal = devTime_TPC$Topt
json.traits.base.devTime.temperature.temperatureRef = tempFromLab

## base - eggDevTime
json.traits.base.eggDevTime.limits.max = 0.0  # Max for individual egg development time
json.traits.base.eggDevTime.limits.min = 0.0  # Min for individual egg development time
json.traits.base.eggDevTime.ranges.max = 21.0  # Upper boundary for sampling
json.traits.base.eggDevTime.ranges.min = 21.0  # Lower boundary for sampling
json.traits.base.eggDevTime.restrictValue = 0.0  # Increment step

json.traits.base.eggDevTime.temperature.dependent = FALSE
json.traits.base.eggDevTime.temperature.activationEnergy = 0.0
json.traits.base.eggDevTime.temperature.energyDecay = 0.0
json.traits.base.eggDevTime.temperature.temperatureOptimal = 0.0
json.traits.base.eggDevTime.temperature.temperatureRef = 0.0

## base - energy_tank
json.traits.base.energy_tank.limits.max = 0.001111111  # Maximum capacity for the "energy tank"
json.traits.base.energy_tank.limits.min = 0.0021875  # Minimum capacity
json.traits.base.energy_tank.ranges.max = 0.36  # Upper boundary for sampling
json.traits.base.energy_tank.ranges.min = 0.32  # Lower boundary for sampling
json.traits.base.energy_tank.restrictValue = 0.01  # Increment step

## base - factorEggMass
json.traits.base.factorEggMass.limits.max = 0.0 
json.traits.base.factorEggMass.limits.min = 0.0
json.traits.base.factorEggMass.ranges.max = 0.0
json.traits.base.factorEggMass.ranges.min = 0.0
json.traits.base.factorEggMass.restrictValue = 0.0  

json.traits.base.factorEggMass.temperature.dependent = FALSE
json.traits.base.factorEggMass.temperature.activationEnergy = 0.0
json.traits.base.factorEggMass.temperature.energyDecay = 0.0
json.traits.base.factorEggMass.temperature.temperatureOptimal = 0.0
json.traits.base.factorEggMass.temperature.temperatureRef = 0.0

## base - dummy
json.traits.base.dummy.limits.max = 0.0 
json.traits.base.dummy.limits.min = 0.0
json.traits.base.dummy.ranges.max = 1.0
json.traits.base.dummy.ranges.min = 0.0
json.traits.base.dummy.restrictValue = 0.0 

json.traits.base.dummy.temperature.dependent = FALSE
json.traits.base.dummy.temperature.activationEnergy = 0.0
json.traits.base.dummy.temperature.energyDecay = 0.0
json.traits.base.dummy.temperature.temperatureOptimal = 0.0
json.traits.base.dummy.temperature.temperatureRef = 0.0

## base - growth
growthK_TPC = calculatePawarTPC(
	trait_name="base.growth",
	traits=data.frame(
		temperature=as.numeric(names(fitted)),
		traitValue=sapply(names(fitted), function(temp) {
			return(fitted[[temp]]$growth)
		})
	),
	Tref = tempFromLab
)

json.traits.base.growth.limits.max = 0.0  # Maximum growth rate limit (0 means no constraint or zero?)
json.traits.base.growth.limits.min = 0.0  # Minimum growth rate limit
json.traits.base.growth.ranges.max = growthK_TPC$traitAtTopt  # Upper boundary for sampling growth rate
json.traits.base.growth.ranges.min = growthK_TPC$traitAtTopt  # Lower boundary for sampling growth rate
json.traits.base.growth.restrictValue = 0.01  # Increment step

json.traits.base.growth.temperature.dependent = TRUE
json.traits.base.growth.temperature.activationEnergy = growthK_TPC$E
json.traits.base.growth.temperature.energyDecay = growthK_TPC$ED
json.traits.base.growth.temperature.temperatureOptimal = growthK_TPC$Topt
json.traits.base.growth.temperature.temperatureRef = tempFromLab

## base - coeffMassForScopeRadius
json.traits.base.coeffMassForScopeRadius.limits.max = 0.075  
json.traits.base.coeffMassForScopeRadius.limits.min = 0.3 
json.traits.base.coeffMassForScopeRadius.ranges.max = coeffMassForScopeRadiusMax 
json.traits.base.coeffMassForScopeRadius.ranges.min = coeffMassForScopeRadiusMin
json.traits.base.coeffMassForScopeRadius.restrictValue = 0.0  

json.traits.base.coeffMassForScopeRadius.temperature.dependent = FALSE
json.traits.base.coeffMassForScopeRadius.temperature.activationEnergy = 0.0
json.traits.base.coeffMassForScopeRadius.temperature.energyDecay = 0.0
json.traits.base.coeffMassForScopeRadius.temperature.temperatureOptimal = 0.0
json.traits.base.coeffMassForScopeRadius.temperature.temperatureRef = 0.0

## base - scaleMassForScopeRadius
json.traits.base.scaleMassForScopeRadius.limits.max = 0.0  
json.traits.base.scaleMassForScopeRadius.limits.min = 0.0 
json.traits.base.scaleMassForScopeRadius.ranges.max = scaleMassForScopeRadiusMax 
json.traits.base.scaleMassForScopeRadius.ranges.min = scaleMassForScopeRadiusMin
json.traits.base.scaleMassForScopeRadius.restrictValue = 0.0  

json.traits.base.scaleMassForScopeRadius.temperature.dependent = FALSE
json.traits.base.scaleMassForScopeRadius.temperature.activationEnergy = 0.0
json.traits.base.scaleMassForScopeRadius.temperature.energyDecay = 0.0
json.traits.base.scaleMassForScopeRadius.temperature.temperatureOptimal = 0.0
json.traits.base.scaleMassForScopeRadius.temperature.temperatureRef = 0.0

## base - coeffMassForInteractionRadius
json.traits.base.coeffMassForInteractionRadius.limits.max = 0.0  
json.traits.base.coeffMassForInteractionRadius.limits.min = 0.0 
json.traits.base.coeffMassForInteractionRadius.ranges.max = coeffMassForInteractionRadiusMax 
json.traits.base.coeffMassForInteractionRadius.ranges.min = coeffMassForInteractionRadiusMin
json.traits.base.coeffMassForInteractionRadius.restrictValue = 0.0  

json.traits.base.coeffMassForInteractionRadius.temperature.dependent = FALSE
json.traits.base.coeffMassForInteractionRadius.temperature.activationEnergy = 0.0
json.traits.base.coeffMassForInteractionRadius.temperature.energyDecay = 0.0
json.traits.base.coeffMassForInteractionRadius.temperature.temperatureOptimal = 0.0
json.traits.base.coeffMassForInteractionRadius.temperature.temperatureRef = 0.0

## base - scaleMassForInteractionRadius
json.traits.base.scaleMassForInteractionRadius.limits.max = 0.0  
json.traits.base.scaleMassForInteractionRadius.limits.min = 0.0 
json.traits.base.scaleMassForInteractionRadius.ranges.max = scaleMassForInteractionRadiusMax 
json.traits.base.scaleMassForInteractionRadius.ranges.min = scaleMassForInteractionRadiusMin
json.traits.base.scaleMassForInteractionRadius.restrictValue = 0.0  

json.traits.base.scaleMassForInteractionRadius.temperature.dependent = FALSE
json.traits.base.scaleMassForInteractionRadius.temperature.activationEnergy = 0.0
json.traits.base.scaleMassForInteractionRadius.temperature.energyDecay = 0.0
json.traits.base.scaleMassForInteractionRadius.temperature.temperatureOptimal = 0.0
json.traits.base.scaleMassForInteractionRadius.temperature.temperatureRef = 0.0

## base - coeffMassForSearchRadius
json.traits.base.coeffMassForSearchRadius.limits.max = 0.0  
json.traits.base.coeffMassForSearchRadius.limits.min = 0.0 
json.traits.base.coeffMassForSearchRadius.ranges.max = coeffMassForSearchRadiusMax 
json.traits.base.coeffMassForSearchRadius.ranges.min = coeffMassForSearchRadiusMin
json.traits.base.coeffMassForSearchRadius.restrictValue = 0.0  

json.traits.base.coeffMassForSearchRadius.temperature.dependent = TRUE
json.traits.base.coeffMassForSearchRadius.temperature.activationEnergy = json.traits.base.growth.temperature.activationEnergy
json.traits.base.coeffMassForSearchRadius.temperature.energyDecay = json.traits.base.growth.temperature.energyDecay
json.traits.base.coeffMassForSearchRadius.temperature.temperatureOptimal = json.traits.base.growth.temperature.temperatureOptimal
json.traits.base.coeffMassForSearchRadius.temperature.temperatureRef = json.traits.base.growth.temperature.temperatureRef

## base - scaleMassForSearchRadius
json.traits.base.scaleMassForSearchRadius.limits.max = 0.0  
json.traits.base.scaleMassForSearchRadius.limits.min = 0.0 
json.traits.base.scaleMassForSearchRadius.ranges.max = scaleMassForSearchRadiusMax 
json.traits.base.scaleMassForSearchRadius.ranges.min = scaleMassForSearchRadiusMin
json.traits.base.scaleMassForSearchRadius.restrictValue = 0.0  

json.traits.base.scaleMassForSearchRadius.temperature.dependent = FALSE
json.traits.base.scaleMassForSearchRadius.temperature.activationEnergy = 0.0
json.traits.base.scaleMassForSearchRadius.temperature.energyDecay = 0.0
json.traits.base.scaleMassForSearchRadius.temperature.temperatureOptimal = 0.0
json.traits.base.scaleMassForSearchRadius.temperature.temperatureRef = 0.0

## base - coeffMassForSpeed
json.traits.base.coeffMassForSpeed.limits.max = 0.0  
json.traits.base.coeffMassForSpeed.limits.min = 0.0 
json.traits.base.coeffMassForSpeed.ranges.max = coeffMassForSpeedMax 
json.traits.base.coeffMassForSpeed.ranges.min = coeffMassForSpeedMin
json.traits.base.coeffMassForSpeed.restrictValue = 0.0  

json.traits.base.coeffMassForSpeed.temperature.dependent = TRUE
json.traits.base.coeffMassForSpeed.temperature.activationEnergy = json.traits.base.growth.temperature.activationEnergy
json.traits.base.coeffMassForSpeed.temperature.energyDecay = json.traits.base.growth.temperature.energyDecay
json.traits.base.coeffMassForSpeed.temperature.temperatureOptimal = json.traits.base.growth.temperature.temperatureOptimal
json.traits.base.coeffMassForSpeed.temperature.temperatureRef = json.traits.base.growth.temperature.temperatureRef

## base - scaleMassForSpeed
json.traits.base.scaleMassForSpeed.limits.max = 0.0  
json.traits.base.scaleMassForSpeed.limits.min = 0.0 
json.traits.base.scaleMassForSpeed.ranges.max = scaleMassForSpeedMax 
json.traits.base.scaleMassForSpeed.ranges.min = scaleMassForSpeedMin
json.traits.base.scaleMassForSpeed.restrictValue = 0.0  

json.traits.base.scaleMassForSpeed.temperature.dependent = FALSE
json.traits.base.scaleMassForSpeed.temperature.activationEnergy = 0.0
json.traits.base.scaleMassForSpeed.temperature.energyDecay = 0.0
json.traits.base.scaleMassForSpeed.temperature.temperatureOptimal = 0.0
json.traits.base.scaleMassForSpeed.temperature.temperatureRef = 0.0

## base - lengthAtMaturation
json.traits.base.lengthAtMaturation.limits.max = 0.0  # Maximum length limit (0 could mean no constraint)
json.traits.base.lengthAtMaturation.limits.min = 0.0  # Minimum length limit
json.traits.base.lengthAtMaturation.ranges.max = lMax  # Upper boundary for sampling length
json.traits.base.lengthAtMaturation.ranges.min = lMin  # Lower boundary for sampling length
json.traits.base.lengthAtMaturation.restrictValue = 0.0  # Increment step

json.traits.base.lengthAtMaturation.temperature.dependent = TRUE
json.traits.base.lengthAtMaturation.temperature.tempSizeRuleVector = calculateTSR(fitted, cricotopus_data, coefficientForMassAforMature, scaleForMassBforMature)

## base - longevitySinceMaturation
json.traits.base.longevitySinceMaturation.limits.max = 0.0  
json.traits.base.longevitySinceMaturation.limits.min = 0.0 
json.traits.base.longevitySinceMaturation.ranges.max = longevitySinceMaturation 
json.traits.base.longevitySinceMaturation.ranges.min = longevitySinceMaturation
json.traits.base.longevitySinceMaturation.restrictValue = 0.0  

json.traits.base.longevitySinceMaturation.temperature.dependent = FALSE
json.traits.base.longevitySinceMaturation.temperature.activationEnergy = 0.0
json.traits.base.longevitySinceMaturation.temperature.energyDecay = 0.0
json.traits.base.longevitySinceMaturation.temperature.temperatureOptimal = 0.0
json.traits.base.longevitySinceMaturation.temperature.temperatureRef = 0.0

## base - memoryDepth
json.traits.base.memoryDepth.limits.max = 0.0  
json.traits.base.memoryDepth.limits.min = 0.0 
json.traits.base.memoryDepth.ranges.max = 5.0 
json.traits.base.memoryDepth.ranges.min = 5.0
json.traits.base.memoryDepth.restrictValue = 0.01

json.traits.base.memoryDepth.temperature.dependent = FALSE
json.traits.base.memoryDepth.temperature.activationEnergy = 0.0
json.traits.base.memoryDepth.temperature.energyDecay = 0.0
json.traits.base.memoryDepth.temperature.temperatureOptimal = 0.0
json.traits.base.memoryDepth.temperature.temperatureRef = 0.0

## base - met_rate
json.traits.base.met_rate.limits.max = 0.00213419
json.traits.base.met_rate.limits.min = 0.007060985
json.traits.base.met_rate.ranges.max = 0.7005 
json.traits.base.met_rate.ranges.min = 0.6805
json.traits.base.met_rate.restrictValue = 0.01 

json.traits.base.met_rate.temperature.dependent = FALSE
json.traits.base.met_rate.temperature.activationEnergy = 0.0
json.traits.base.met_rate.temperature.energyDecay = 0.0
json.traits.base.met_rate.temperature.temperatureOptimal = 0.0
json.traits.base.met_rate.temperature.temperatureRef = 0.0

## base - pupaPeriodTime
json.traits.base.pupaPeriodTime.limits.max = 0.0
json.traits.base.pupaPeriodTime.limits.min = 0.0
json.traits.base.pupaPeriodTime.ranges.max = pupaPeriodTime 
json.traits.base.pupaPeriodTime.ranges.min = pupaPeriodTime
json.traits.base.pupaPeriodTime.restrictValue = 0.0 

json.traits.base.pupaPeriodTime.temperature.dependent = FALSE
json.traits.base.pupaPeriodTime.temperature.activationEnergy = 0.0
json.traits.base.pupaPeriodTime.temperature.energyDecay = 0.0
json.traits.base.pupaPeriodTime.temperature.temperatureOptimal = 0.0
json.traits.base.pupaPeriodTime.temperature.temperatureRef = 0.0

## base - shock_resistance
json.traits.base.shock_resistance.limits.max = 0.0009756098  # Maximum limit for shock resistance
json.traits.base.shock_resistance.limits.min = 0.0025        # Minimum limit for shock resistance
json.traits.base.shock_resistance.ranges.max = 410.0 # Upper boundary for sampling shock resistance
json.traits.base.shock_resistance.ranges.min = 400.0 # Lower boundary for sampling shock resistance
json.traits.base.shock_resistance.restrictValue = 0.01  # Increment step

json.traits.base.shock_resistance.temperature.dependent = FALSE
json.traits.base.shock_resistance.temperature.activationEnergy = 0.0
json.traits.base.shock_resistance.temperature.energyDecay = 0.0
json.traits.base.shock_resistance.temperature.temperatureOptimal = 0.0
json.traits.base.shock_resistance.temperature.temperatureRef = 0.0

## base - voracityProportion
json.traits.base.voracityProportion.limits.max = 0.0
json.traits.base.voracityProportion.limits.min = 0.0
json.traits.base.voracityProportion.ranges.max = 1.0
json.traits.base.voracityProportion.ranges.min = 1.0
json.traits.base.voracityProportion.restrictValue = 0.0

json.traits.base.voracityProportion.temperature.dependent = TRUE
json.traits.base.voracityProportion.temperature.activationEnergy = json.traits.base.growth.temperature.activationEnergy
json.traits.base.voracityProportion.temperature.energyDecay = json.traits.base.growth.temperature.energyDecay
json.traits.base.voracityProportion.temperature.temperatureOptimal = json.traits.base.growth.temperature.temperatureOptimal
json.traits.base.voracityProportion.temperature.temperatureRef = json.traits.base.growth.temperature.temperatureRef

## base - eggFertility
json.traits.base.eggFertility.limits.max = 0.0 
json.traits.base.eggFertility.limits.min = 0.0 
json.traits.base.eggFertility.ranges.max = 1.0 
json.traits.base.eggFertility.ranges.min = 1.0  
json.traits.base.eggFertility.restrictValue = 0.0 

json.traits.base.eggFertility.temperature.dependent = FALSE
json.traits.base.eggFertility.temperature.activationEnergy = 0.0
json.traits.base.eggFertility.temperature.energyDecay = 0.0
json.traits.base.eggFertility.temperature.temperatureOptimal = 0.0
json.traits.base.eggFertility.temperature.temperatureRef = 0.0

## decisions - escapeProbabilityWeight - Pvelocity
json.traits.decisions.escapeProbabilityWeight.Pvelocity.limits.max = 0.0 
json.traits.decisions.escapeProbabilityWeight.Pvelocity.limits.min = 0.0 
json.traits.decisions.escapeProbabilityWeight.Pvelocity.ranges.max = 0.33 
json.traits.decisions.escapeProbabilityWeight.Pvelocity.ranges.min = 0.33 
json.traits.decisions.escapeProbabilityWeight.Pvelocity.restrictValue = 0.0

json.traits.decisions.escapeProbabilityWeight.Pvelocity.temperature.dependent = FALSE
json.traits.decisions.escapeProbabilityWeight.Pvelocity.temperature.activationEnergy = 0.0
json.traits.decisions.escapeProbabilityWeight.Pvelocity.temperature.energyDecay = 0.0
json.traits.decisions.escapeProbabilityWeight.Pvelocity.temperature.temperatureOptimal = 0.0
json.traits.decisions.escapeProbabilityWeight.Pvelocity.temperature.temperatureRef = 0.0

## decisions - escapeProbabilityWeight - PattackDistance
json.traits.decisions.escapeProbabilityWeight.PattackDistance.limits.max = 0.0 
json.traits.decisions.escapeProbabilityWeight.PattackDistance.limits.min = 0.0 
json.traits.decisions.escapeProbabilityWeight.PattackDistance.ranges.max = 0.33 
json.traits.decisions.escapeProbabilityWeight.PattackDistance.ranges.min = 0.33 
json.traits.decisions.escapeProbabilityWeight.PattackDistance.restrictValue = 0.0

json.traits.decisions.escapeProbabilityWeight.PattackDistance.temperature.dependent = FALSE
json.traits.decisions.escapeProbabilityWeight.PattackDistance.temperature.activationEnergy = 0.0
json.traits.decisions.escapeProbabilityWeight.PattackDistance.temperature.energyDecay = 0.0
json.traits.decisions.escapeProbabilityWeight.PattackDistance.temperature.temperatureOptimal = 0.0
json.traits.decisions.escapeProbabilityWeight.PattackDistance.temperature.temperatureRef = 0.0

## decisions - predationProbabilityWeight - Preach
json.traits.decisions.predationProbabilityWeight.Preach.limits.max = 0.0 
json.traits.decisions.predationProbabilityWeight.Preach.limits.min = 0.0 
json.traits.decisions.predationProbabilityWeight.Preach.ranges.max = 0.25 
json.traits.decisions.predationProbabilityWeight.Preach.ranges.min = 0.25 
json.traits.decisions.predationProbabilityWeight.Preach.restrictValue = 0.0

json.traits.decisions.predationProbabilityWeight.Preach.temperature.dependent = FALSE
json.traits.decisions.predationProbabilityWeight.Preach.temperature.activationEnergy = 0.0
json.traits.decisions.predationProbabilityWeight.Preach.temperature.energyDecay = 0.0
json.traits.decisions.predationProbabilityWeight.Preach.temperature.temperatureOptimal = 0.0
json.traits.decisions.predationProbabilityWeight.Preach.temperature.temperatureRef = 0.0

## decisions - predationProbabilityWeight - Ppdf
json.traits.decisions.predationProbabilityWeight.Ppdf.limits.max = 0.0 
json.traits.decisions.predationProbabilityWeight.Ppdf.limits.min = 0.0 
json.traits.decisions.predationProbabilityWeight.Ppdf.ranges.max = 0.25 
json.traits.decisions.predationProbabilityWeight.Ppdf.ranges.min = 0.25 
json.traits.decisions.predationProbabilityWeight.Ppdf.restrictValue = 0.0

json.traits.decisions.predationProbabilityWeight.Ppdf.temperature.dependent = FALSE
json.traits.decisions.predationProbabilityWeight.Ppdf.temperature.activationEnergy = 0.0
json.traits.decisions.predationProbabilityWeight.Ppdf.temperature.energyDecay = 0.0
json.traits.decisions.predationProbabilityWeight.Ppdf.temperature.temperatureOptimal = 0.0
json.traits.decisions.predationProbabilityWeight.Ppdf.temperature.temperatureRef = 0.0

## decisions - predationProbabilityWeight - PvorPred
json.traits.decisions.predationProbabilityWeight.PvorPred.limits.max = 0.0 
json.traits.decisions.predationProbabilityWeight.PvorPred.limits.min = 0.0 
json.traits.decisions.predationProbabilityWeight.PvorPred.ranges.max = 0.25 
json.traits.decisions.predationProbabilityWeight.PvorPred.ranges.min = 0.25 
json.traits.decisions.predationProbabilityWeight.PvorPred.restrictValue = 0.0

json.traits.decisions.predationProbabilityWeight.PvorPred.temperature.dependent = FALSE
json.traits.decisions.predationProbabilityWeight.PvorPred.temperature.activationEnergy = 0.0
json.traits.decisions.predationProbabilityWeight.PvorPred.temperature.energyDecay = 0.0
json.traits.decisions.predationProbabilityWeight.PvorPred.temperature.temperatureOptimal = 0.0
json.traits.decisions.predationProbabilityWeight.PvorPred.temperature.temperatureRef = 0.0

## decisions - edibilityValueWeight - Pp
json.traits.decisions.edibilityValueWeight.Pp.limits.max = 0.0 
json.traits.decisions.edibilityValueWeight.Pp.limits.min = 0.0 
json.traits.decisions.edibilityValueWeight.Pp.ranges.max = 0.5 
json.traits.decisions.edibilityValueWeight.Pp.ranges.min = 0.5 
json.traits.decisions.edibilityValueWeight.Pp.restrictValue = 0.0

json.traits.decisions.edibilityValueWeight.Pp.temperature.dependent = FALSE
json.traits.decisions.edibilityValueWeight.Pp.temperature.activationEnergy = 0.0
json.traits.decisions.edibilityValueWeight.Pp.temperature.energyDecay = 0.0
json.traits.decisions.edibilityValueWeight.Pp.temperature.temperatureOptimal = 0.0
json.traits.decisions.edibilityValueWeight.Pp.temperature.temperatureRef = 0.0

## decisions - preferences - experienceInfluenceWithEdibles
json.traits.decisions.preferences.experienceInfluenceWithEdibles.limits.max = 0.0 
json.traits.decisions.preferences.experienceInfluenceWithEdibles.limits.min = 0.0 
json.traits.decisions.preferences.experienceInfluenceWithEdibles.ranges.max = 0.99 
json.traits.decisions.preferences.experienceInfluenceWithEdibles.ranges.min = 0.99 
json.traits.decisions.preferences.experienceInfluenceWithEdibles.restrictValue = 0.0

json.traits.decisions.preferences.experienceInfluenceWithEdibles.temperature.dependent = FALSE
json.traits.decisions.preferences.experienceInfluenceWithEdibles.temperature.activationEnergy = 0.0
json.traits.decisions.preferences.experienceInfluenceWithEdibles.temperature.energyDecay = 0.0
json.traits.decisions.preferences.experienceInfluenceWithEdibles.temperature.temperatureOptimal = 0.0
json.traits.decisions.preferences.experienceInfluenceWithEdibles.temperature.temperatureRef = 0.0

## probabilityDensityFunction - muForPDF
json.traits.probabilityDensityFunction.muForPDF.limits.max = 0.0 
json.traits.probabilityDensityFunction.muForPDF.limits.min = 0.0 
json.traits.probabilityDensityFunction.muForPDF.ranges.max = 1.44
json.traits.probabilityDensityFunction.muForPDF.ranges.min = 1.44
json.traits.probabilityDensityFunction.muForPDF.restrictValue = 0.0

json.traits.probabilityDensityFunction.muForPDF.temperature.dependent = FALSE
json.traits.probabilityDensityFunction.muForPDF.temperature.activationEnergy = 0.0
json.traits.probabilityDensityFunction.muForPDF.temperature.energyDecay = 0.0
json.traits.probabilityDensityFunction.muForPDF.temperature.temperatureOptimal = 0.0
json.traits.probabilityDensityFunction.muForPDF.temperature.temperatureRef = 0.0

## probabilityDensityFunction - sigmaForPDF
json.traits.probabilityDensityFunction.sigmaForPDF.limits.max = 0.0 
json.traits.probabilityDensityFunction.sigmaForPDF.limits.min = 0.0 
json.traits.probabilityDensityFunction.sigmaForPDF.ranges.max = 2.34
json.traits.probabilityDensityFunction.sigmaForPDF.ranges.min = 2.34
json.traits.probabilityDensityFunction.sigmaForPDF.restrictValue = 0.0

json.traits.probabilityDensityFunction.sigmaForPDF.temperature.dependent = FALSE
json.traits.probabilityDensityFunction.sigmaForPDF.temperature.activationEnergy = 0.0
json.traits.probabilityDensityFunction.sigmaForPDF.temperature.energyDecay = 0.0
json.traits.probabilityDensityFunction.sigmaForPDF.temperature.temperatureOptimal = 0.0
json.traits.probabilityDensityFunction.sigmaForPDF.temperature.temperatureRef = 0.0

## cellValue - cellEvaluationBiomass
json.traits.cellValue.cellEvaluationBiomass.limits.max = 0.0 
json.traits.cellValue.cellEvaluationBiomass.limits.min = 0.0 
json.traits.cellValue.cellEvaluationBiomass.ranges.max = 0.332698376233407 
json.traits.cellValue.cellEvaluationBiomass.ranges.min = 0.332698376233407 
json.traits.cellValue.cellEvaluationBiomass.restrictValue = 0.0

json.traits.cellValue.cellEvaluationBiomass.temperature.dependent = FALSE
json.traits.cellValue.cellEvaluationBiomass.temperature.activationEnergy = 0.0
json.traits.cellValue.cellEvaluationBiomass.temperature.energyDecay = 0.0
json.traits.cellValue.cellEvaluationBiomass.temperature.temperatureOptimal = 0.0
json.traits.cellValue.cellEvaluationBiomass.temperature.temperatureRef = 0.0

## cellValue - cellEvaluationProConspecific
json.traits.cellValue.cellEvaluationProConspecific.limits.max = 0.0 
json.traits.cellValue.cellEvaluationProConspecific.limits.min = 0.0 
json.traits.cellValue.cellEvaluationProConspecific.ranges.max = 0.5 
json.traits.cellValue.cellEvaluationProConspecific.ranges.min = 0.5
json.traits.cellValue.cellEvaluationProConspecific.restrictValue = 0.0

json.traits.cellValue.cellEvaluationProConspecific.temperature.dependent = FALSE
json.traits.cellValue.cellEvaluationProConspecific.temperature.activationEnergy = 0.0
json.traits.cellValue.cellEvaluationProConspecific.temperature.energyDecay = 0.0
json.traits.cellValue.cellEvaluationProConspecific.temperature.temperatureOptimal = 0.0
json.traits.cellValue.cellEvaluationProConspecific.temperature.temperatureRef = 0.0

## cellValue - conspecificWeight
json.traits.cellValue.conspecificWeight.limits.max = 0.0 
json.traits.cellValue.conspecificWeight.limits.min = 0.0 
json.traits.cellValue.conspecificWeight.ranges.max = 0.0 
json.traits.cellValue.conspecificWeight.ranges.min = 0.0 
json.traits.cellValue.conspecificWeight.restrictValue = 0.0

json.traits.cellValue.conspecificWeight.temperature.dependent = FALSE
json.traits.cellValue.conspecificWeight.temperature.activationEnergy = 0.0
json.traits.cellValue.conspecificWeight.temperature.energyDecay = 0.0
json.traits.cellValue.conspecificWeight.temperature.temperatureOptimal = 0.0
json.traits.cellValue.conspecificWeight.temperature.temperatureRef = 0.0

# =====================
# TRAITS ORDER
# =====================
# This defines the order in which individual-level traits are processed or stored.

json.traits.individualLevelTraitsOrder = list(
	list(
		trait = "base$dummy",
		element = "value"
	),
	list(
		trait = "base$energy_tank",
		element = "value"
	),
	list(
		trait = "base$met_rate",
		element = "value"
	),
	list(
		trait = "base$shock_resistance",
		element = "value"
	),
	list(
		trait = "base$coeffMassForScopeRadius",
		element = "value"
	)
)

# =====================
# GROWTH MODULE PARAMETERS
# =====================

json.growthModule.assignedForMolt = 0.99  # Fraction of resources allocated to molting
json.growthModule.betaScaleTank = 1.01  # Scaling factor for the energy tank in the growth module
json.growthModule.capitalBreeding.enabled = TRUE  # If TRUE, uses stored resources for reproduction
json.growthModule.capitalBreeding.capitalBreedingParams.numberOfCapitalBreeds = 1
json.growthModule.capitalBreeding.capitalBreedingParams.timeOfReproEventDuringCapitalBreeding = 0.0
json.growthModule.coefficientForMassA = coefficientForMassA  # Coefficient A in mass-growth equations
json.growthModule.coefficientForMassAforMature = coefficientForMassAforMature  # Same, but used post-maturation
json.growthModule.ageVector = ageVector # Taken from Section 1, already parametrised
json.growthModule.eggDryMass.eggDryMassFromEquation = FALSE  # If TRUE, egg mass is calculated from an equation
json.growthModule.eggDryMass.equationParameters.coefficient = 0.0  # Coefficient for egg dry mass equation
json.growthModule.eggDryMass.equationParameters.scale = 0.0  # Scale parameter for egg dry mass equation
json.growthModule.eggDryMass.value = eggDryMass  # Default fixed egg dry mass if not from equation
json.growthModule.eggsPerBatch.eggsPerBatchFromEquation = FALSE  # If TRUE, eggs per batch is from an equation
json.growthModule.eggsPerBatch.equationParameters.interceptForEggBatchFromEquation = 0.0
json.growthModule.eggsPerBatch.equationParameters.slopeForEggBatchFromEquation = 0.0
json.growthModule.eggsPerBatch.value = eggsPerBatch  # Fixed number of eggs per batch
json.growthModule.excessInvestInSize = 0.5  # Fraction of surplus resources invested in growth rather than reproduction
json.growthModule.femaleWetMass = femaleWetMass  # Typical female wet mass
json.growthModule.growthModel.defaultAtTempFromLab.model = curveType  # Growth model used (taken from SECTION 1)
json.growthModule.growthModel.defaultAtTempFromLab.A = ifelse(curveType %in% c("Logistic4P") , fitted[[as.character(tempFromLab)]]$A, 0.0)  
json.growthModule.growthModel.temperature.dependent = TRUE
json.growthModule.growthModel.temperature.growthModelThermalChanges = growthModelThermalChanges
json.growthModule.habitatShift = list(length(ageVector) + 1)
json.growthModule.habitatShiftFactor = 1e+6
json.growthModule.instarFirstReproduction = instarFirstReproduction
json.growthModule.maxPlasticityKVonBertalanffy = 0.25  # Max plasticity for growth rate K in Von Bertalanffy model
json.growthModule.minPlasticityKVonBertalanffy = 0.3   # Min plasticity for growth rate K
json.growthModule.moltingAgeThreshold = 0.0  # Age threshold at which molting might occur
json.growthModule.scaleForMassB = scaleForMassB  # Scale factor B in mass-growth equations
json.growthModule.scaleForMassBforMature = scaleForMassBforMature  # Same, but used post-maturation

# =====================
# ADDITIONAL PARAMETERS
# =====================

json.habitatShiftAfterBreeding = TRUE  # If TRUE, individuals shift habitat after breeding
json.habitatShiftAfterBreedingFactor = 1e+6
json.habitatShiftBeforeBreeding = TRUE # If TRUE, individuals shift habitat before breeding
json.habitatShiftBeforeBreedingFactor = 1e+6

json.individualsPerInstar = calculateIndividualsPerInstarCricotopus(
	totalNumberOfIndividuals = 2191,
	fieldDensity, ageVector, coefficientForMassA, coefficientForMassAforMature, 
	scaleForMassB, scaleForMassBforMature, conversionToWetMass, 
	fitted[[as.character(tempFromLab)]]$best_model
) # Number of individuals simulated per instar

json.requiredRepresentAllPossibleAlleles = FALSE

json.maleMaxReproductionEvents = 8   # Maximum number of reproduction events for a male
json.maleMobility = 1  # Mobility factor for males (compared to females or baseline)
json.maleReproductionFactor = 0.25  # Fraction of how many mates or eggs a male can fertilize compared to baseline
json.minRelativeHumidityThreshold = c(10, 10, 10, 10, 10)  # Minimum humidity needed for survival or normal behavior


# =====================
# FINAL PARAMETERS
# =====================

json.name = "Cricotopus_spx"  # Name of the species or simulation run
json.YodzisA = 0.1  # Coefficient used in an adult trait or growth equation
json.YodzisB = 0.75       # Another coefficient for adult trait or growth equation
json.numberOfSimulatedAttacks = 10000  # Number of attacks to simulate in predation sub-model
json.percentageCostForMetabolicDownregulationSearchArea = 0.5  # Cost factor for search area under metabolic downregulation
json.percentageCostForMetabolicDownregulationSpeed = 0.5       # Cost factor for speed under metabolic downregulation
json.percentageCostForMetabolicDownregulationVoracity = 0.5    # Cost factor for voracity under metabolic downregulation
json.percentageMetabolicDownregulation = 0.0  # Overall fraction of metabolic downregulation
json.plasticityDueToConditionSearch = 0.05    # Plasticity factor for search trait based on condition
json.plasticityDueToConditionSpeed = 0.05     # Plasticity factor for speed trait based on condition
json.preserveLeftovers = TRUE  # If TRUE, unconsumed resources or partial prey remain available
json.probabilityDeathFromBackground = c(0.0, 0.0, 0.0, 0.0, 0.0)  # Baseline mortality rate from non-predation causes
json.sexRatio = sexRatio  # Fraction of female offspring (or ratio of females to males)

json.decisions.predationProbability.additiveMechanism = TRUE
json.decisions.predationProbability.isRemainingWeightNull = FALSE
json.decisions.predationProbability.escapeProbability.additiveMechanism = TRUE
json.decisions.predationProbability.escapeProbability.isRemainingWeightNull = FALSE
json.decisions.predationProbability.escapeProbability.cVelocity = 0.0
json.decisions.predationProbability.useGlobalMaximumPreyVoracity = TRUE
json.decisions.edibilityValue.additiveMechanism = TRUE
json.decisions.edibilityValue.isRemainingWeightNull = FALSE
json.decisions.edibilityValue.qualityResourceAssessment = FALSE
json.decisions.decisionWithinPatch.killProbability = 1.0  # Probability that a predation event kills the prey if attacked
json.decisions.decisionAmongPatches.weightIndividualToGlobalAssessment = 0.0
json.decisions.sensoryModel.beta = 2.5

json.sexualType = "diploid"  # Reproduction type (e.g., haplodiploid, diploid, etc.)
json.statisticsIndividualsPerInstar = 1000  # Number of individuals used in statistics per instar
json.tempFromLab = tempFromLab  # Reference temperature from lab conditions




################################################################
################################################################
################################################################


source(file.path(parametrisation_path, "utilities", "codes", "functions", "calculateMaximumResourceCapacityDensityBiofilm.r"))


maximumResourceCapacityDensityBiofilm = calculateMaximumResourceCapacityDensityBiofilm(
	json.individualsPerInstar, landscapeArea=1e+6,
	ageVector, coefficientForMassA, coefficientForMassAforMature, 
	scaleForMassB, scaleForMassBforMature, conversionToWetMass, 
	fitted[[as.character(tempFromLab)]]$best_model
)

print("maximumResourceCapacityDensityBiofilm:")
print(maximumResourceCapacityDensityBiofilm)
