# @@@@ species: orius_laevigatus

if (!"app" %in% ls()) {
	rm(list=ls(all=TRUE))
	
	script_path <- dirname(sys.frame(1)$ofile)
}

parametrisation_path <- sub("(.*/parametrisation)/.*", "\\1", script_path)


source(file.path(parametrisation_path, "utilities", "codes", "functions", "growth_curve_modelfitting", "gcmGAT.r"))



set.seed(123)


# Option to avoid plotting 
# options(allow_plotting = FALSE)

# Start reading ID_ORILAE_tempFromLab data frame	
ID_ORILAE_tempFromLab<-data.frame(
			Variable = "tempFromLab", 
			DOI1 = "10.1046_j.1570-7458.1997.00118.x",
			Category1 = "Article",
			DataTreatment = "Extracted from text."
)
# End reading ID_ORILAE_tempFromLab data frame

tempFromLab<-26
tempFromLab #@tempFromLab=26


# Start reading ID_ORILAE_eggMassFromEquation data frame	
ID_ORILAE_eggMassFromEquation<-data.frame(
			Variable = "eggMassFromEquation", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Common knowledge"
)
# End reading ID_ORILAE_eggMassFromEquation data frame

eggMassFromEquation<-FALSE
eggMassFromEquation #@eggMassFromEquation=FALSE


# Start reading ID_ORILAE_sexRatio data frame		  
ID_ORILAE_sexRatio <-data.frame(
			Variable = "sexRatio", 
			DOI1 = "10.1016_j.biocontrol.2013.11.009",
			Category1 = "Article",
			DataTreatment = "Female proportion taken from a mean calculated using all the numbers of founding couples from second generation"
)
# End reading ID_ORILAE_sexRatio data frame		  

data_for_sex_ratio<-read.table(file.path(script_path, "sex_ratio_from_castane_et_al.txt"),header=TRUE)
selected_ratios<-data_for_sex_ratio[1,2:4]
ratios_vector<-as.vector(t(selected_ratios))
sexRatio<-mean(ratios_vector)
sexRatio #@sexRatio=0.47


# Start reading ID_ORILAE_conversionToWetMass data frame	
ID_ORILAE_conversionToWetMass<-data.frame(
			Variable = "conversionToWetMass", 
			DOI1 = "ERN_2022_unpublishºedWeightOriusL.fake",
			Category1 = "Unpublished",
			DataTreatment = "Length and weight data taken by measuring a commercial population:  Agrobio SL (ORIcontrol, Almeria, Spain). We performed a multifitting function to find the best model for M-L relation using AIC criterion" 
)
# End reading ID_ORILAE_conversionToWetMass data frame		  

weight<-read.table(file.path(script_path, "weights_orius_estefania.txt"),header=TRUE)
colnames(weight)[colnames(weight) == "weigth"] <- "weight"
weight$weight<-weight$weight*100 # Estefania had an scale error of 10 units, the mean weight in literature is from 1 to 2 mg
# weight_filtered<-weight[!weight$stage %in% c("male", "N3", "N5"),]
fresh<-weight[weight$type_of_sample %in% "fresh",]
dry<-weight[weight$type_of_sample %in% "dry","weight"]
weight_final<-data.frame(fresh,dry=dry)
colnames(weight_final)[colnames(weight_final) %in% "weight"]<-"fresh"
weight_final$type_of_sample<-NULL
weight_final$ratio<-with(weight_final,fresh/dry)
weight_final
sd_estefania_weight<-aggregate(weight~type_of_sample+stage,data=weight,FUN=sd)
mean_estefania_weight<-aggregate(weight~type_of_sample+stage,data=weight,FUN=mean)

# n_estefania_weight<-aggregate(x=weight$weight, by= list(weight$type_of_sample,weight$stage),FUN=length)
# n_estefania_weight<-n_estefania_weight$x

# se_weights_estefania<-sd_estefania_weight$weight/sqrt(n_estefania_weight)
# final_data_frame<-data.frame(mean_estefania_weight,sd_estefania_weight,se_weights_estefania)
# final_data_frame$ratio_SE_weight<-with(final_data_frame,weight/se_weights_estefania)
#There is a huge heterogenity in the ratios mean/SE due probably to the huge 
#difference in water across the stages (mainly early stages vs adult). We also
#must take into account that the HR in the room was neglected (we suppose). 
#At the end, we conclude to take the global mean
# weight_final $water<-with(weight_final, fresh-dry)

# # Crear el gráfico
# with(weight_final, {
  # plot(water_weight ~ stage, 
       # xlab = "Stage", 
       # ylab = "Water Weight", 
       # main = "Water Weight vs Stage",
       # pch = 16,               # Puntos sólidos
       # col = ifelse(stage == "N3", "red", "black"))  # N3 en rojo, el resto en negro
# })

conversionToWetMass<-mean(weight_final$ratio)
conversionToWetMass #conversionToWetMass=1.538544


# Start reading ID_ORILAE_eggsPerBatch data frame	
ID_ORILAE_eggsPerBatch<-data.frame(
			Variable = "eggsPerBatch", 
			DOI1 = "10.1111_j.1570-7458.1973.tb00263.x",
			Category1 = "Article",
			DataTreatment = "taken from the table 1 of the document"
)
# End reading ID_ORILAE_eggsPerBatch data frame

eggsPerBatch<-9.8
eggsPerBatch<-round(eggsPerBatch)
eggsPerBatch #@eggsPerBatch=10

# Start reading ID_ORILAE_femaleMaxReproductionEvents data frame	
ID_ORILAE_femaleMaxReproductionEvents<-data.frame(
			Variable = "femaleMaxReproductionEvents", 
			DOI1 = "10.1046/j.1570-7458.1997.00118.x",
			DOI2 = "10.1111_j.1570-7458.1973.tb00263.x",
			Category1 = "Article",
			Category2 = "Article",
			DataTreatment = "We divide the max fecundity by eggs per batch data taken from the table 1 of the document from DOI2"
)
# End reading ID_ORILAE_femaleMaxReproductionEvents data frame

rep_data<-read.table(file.path(script_path, "rep_param_orius_laevigatus.txt"),header=TRUE)
maxfecundity<-mean(rep_data$fecundity)
maxfecundity<-round(maxfecundity)

femaleMaxReproductionEvents<- maxfecundity/eggsPerBatch
femaleMaxReproductionEvents<-round(femaleMaxReproductionEvents)
femaleMaxReproductionEvents #@femaleMaxReproductionEvents=15

# Start reading ID_ORILAE_ageVector data frame	
ID_ORILAE_ageVector<-data.frame(
			Variable = "ageVector", 
			DOI1 = "10.3390_insects13030250",
			Category1 = "Article",
			DataTreatment = "Step 1: We calculated the mean from Gallego 2022 using the data from 3 to 10 eggs given due to the big difference of the treatment with 1 egg related to the rest of the stages."
)
# End reading ID_ORILAE_ageVector data frame

age<-read.table(file.path(script_path, "gallego_development_orius.txt"), header=TRUE)

format_age <- colMeans(age[, -1])

result <- data.frame(
  stages = names(format_age),
  age = format_age
)

rownames(result)<-NULL
result$age<-as.numeric(result$age)
cumulative_result <- data.frame(
  stages = result$stages,
  age = c(0, cumsum(result$age)[-length(result$age)])
)
result$age<-as.numeric(result$age)
result <- rbind(cumulative_result, data.frame(stages="adult", age=sum(result$age)))
result$age<-as.numeric(result$age)

age<-round(result$age,1)
age<-age[2:length(age)]
ageVector<-age
ageVector #@ageVector=c(2.0, 3.1, 4.7, 6.5, 10.1) ###Warning: decimals must be adjusted to timeStep in simulation


# Start reading ID_ORILAE_instarFirstReproduction data frame	
ID_ORILAE_instarFirstReproduction<-data.frame(
			Variable = "instarFirstReproduction", 
			DOI1 = "10.1093/jee/toaa078",
			Category1 = "Article",
			DataTreatment = "Extracted from text" 
)
# End reading ID_ORILAE_instarFirstReproduction data frame

instarFirstReproduction<-length(ageVector)+1
instarFirstReproduction #@instarFirstReproduction=6

# Start reading ID_ORILAE_coefficientForMassA data frame	
ID_ORILAE_coefficientForMassA<-data.frame(
			Variable = "coefficientForMassA", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			DOI2 = "ERN_2022_unpublishedWeightOriusL.fake",
			Category1 = "Unpublished",
			Category2 = "Unpublished",
			DataTreatment = "Length and weight data taken by measuring a commercial population:  Agrobio SL (ORIcontrol, Almeria, Spain). We performed a multifitting function to find the best model for M-L relation using AIC criterion" 
)
# End reading ID_ORILAE_coefficientForMassA data frame		  

length<-read.table(file.path(script_path, "lengths_orius_estefania.txt"),header=TRUE)

mean_length <- aggregate(length ~ stage, data = length, FUN = mean)
dry_data <- subset(weight, type_of_sample == "dry")
mean_weight <- aggregate(weight ~ stage, data = dry_data, FUN = mean)

m_l_table <- merge(mean_weight, mean_length, by = "stage", all = TRUE)
colnames(m_l_table)[colnames(m_l_table) == "weight"] <- "dryMass"

order <- c("N1", "N2", "N3", "N4", "N5", "male", "female")

sort_by_stage <- function(data, order) {
  sorted_data <- data
  for (i in 1:nrow(sorted_data)) {
    stage_value <- sorted_data[i, "stage"]
    stage_index <- match(stage_value, order)
    sorted_data[i, "stage_numeric"] <- stage_index
  }
  sorted_data <- sorted_data[order(sorted_data$stage_numeric), ]
  sorted_data <- sorted_data[, !(names(sorted_data) %in% c("stage_numeric"))]
  return(sorted_data)
}

sorted_table <- sort_by_stage(m_l_table, order)

drymass_length_table<-as.data.frame(sorted_table)

# We deleted the points for N3, N5 and males because of a possible missmatching of male/female identification during diferentiation
# Estefania, the author and who took the data, she comments this in the data table 

stages <- drymass_length_table$stage
drymass_length_table<-drymass_length_table[!drymass_length_table$stage %in% c("N3","male"),] 
str(drymass_length_table)
model_ml<-glm(dryMass~log(length), data=na.omit(drymass_length_table), family = gaussian(link = "log"))

coefficientForMassA<-as.vector(t(exp(coef(model_ml)[1])))
coefficientForMassA #@coefficientForMassA=1.778455


# Start reading ID_ORILAE_scaleForMassB data frame	
ID_ORILAE_scaleForMassB<-data.frame(
			Variable = "scaleForMassB", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			DOI2 = "ERN_2022_unpublishedWeightOriusL.fake",
			Category1 = "Unpublished",
			Category2 = "Unpublished",
			DataTreatment = "Length and weight data taken by measuring a commercial population: Agrobio SL (ORIcontrol, Almeria, Spain). We performed a multifitting function to find the best model for M-L relation using AIC criterion" 
)
# End reading ID_ORILAE_scaleForMassB data frame		  

scaleForMassB<-as.vector(t(coef(model_ml)[2]))
scaleForMassB #@scaleForMassB=2.937436


# Start reading ID_ORILAE_coefficientForMassAforMature data frame	
ID_ORILAE_coefficientForMassAforMature<-data.frame(
			Variable = "coefficientForMassAforMature", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			DOI2 = "ERN_2022_unpublishedWeightOriusL.fake",
			Category1 = "Unpublished",
			Category2 = "Unpublished",
			DataTreatment = "Length and weight data taken by measuring a commercial population:  Agrobio SL (ORIcontrol, Almeria, Spain). We performed a multifitting function to find the best model for M-L relation using AIC criterion" 
)
# End reading ID_ORILAE_coefficientForMassAforMature data frame		  

coefficientForMassAforMature<-coefficientForMassA
coefficientForMassAforMature #@coefficientForMassAforMature=0.01792461


# Start reading ID_ORILAE_scaleForMassBforMature data frame	
ID_ORILAE_scaleForMassBforMature<-data.frame(
			Variable = "scaleForMassBforMature", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			DOI2 = "ERN_2022_unpublishedWeightOriusL.fake",
			Category1 = "Unpublished",
			Category2 = "Unpublished",
			DataTreatment = "Length and weight data taken by measuring a commercial population:  Agrobio SL (ORIcontrol, Almeria, Spain). We performed a multifitting function to find the best model for M-L relation using AIC criterion" 
)
# End reading ID_ORILAE_scaleForMassBforMature data frame		  

scaleForMassBforMature<-scaleForMassB
scaleForMassBforMature #@scaleForMassBforMature=2.937436





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







# Start reading ID_ORILAE_curveType data frame	
ID_ORILAE_curveType<-data.frame(
			Variable = "curveType", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			DOI2 = "10.1046_j.1570-7458.1997.00118.x",
			Category1 = "Unpublished",
			Category2 = "Article",
			DataTreatment = "Step 1: obtaining the best model for the growth curve. Step 2: extracting the name of the model."
)
# End reading ID_ORILAE_curveType data frame
drymass_length_table
age_table<-result
mean_length<-mean_length[!mean_length$stage %in% "male",]
colnames(mean_length)[colnames(mean_length) == "stage"] <- "stages"
mean_length$stages[mean_length$stage == "female"] <- "adult"

full_growth_curve_table<-merge(mean_length,age_table,by="stages")
full_growth_curve_table<-full_growth_curve_table[order(full_growth_curve_table$age),]


orius_laevigatus_data <- list()

orius_laevigatus_data[[as.character(tempFromLab)]] = full_growth_curve_table


fitted<-gcmGAT(orius_laevigatus_data, x = "age", y = "length", use_app = TRUE)


curveType<-fitted[[as.character(tempFromLab)]]$best_model_name
curveType #@curveType="VonBertalanffy"

# Start reading ID_ORILAE_lengthAtBirth data frame		 
ID_ORILAE_lengthAtBirth<-data.frame(
			Variable = "lengthAtBirth", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			DOI2 = "10.3390_insects13030250",
			Category1 = "Unpublished",
			Category2 = "Article",
			DataTreatment = "Step 1: We calculated the mean for each stage (except male) and we built a stages-length table. Step 2: We calculated the mean from Gallego 2022 using the data from 3 to 10 eggs given due to the big difference of the treatment with 1 egg related to the rest of the stages. After that we ran a function to fit the best model and we obtained the intercept of the growth curve model."
)
# End reading ID_ORILAE_lengthAtBirth data frame		  

best_model<-fitted[[as.character(tempFromLab)]]$best_model


lengthAtBirth=predict(fitted[[as.character(tempFromLab)]]$best_model, newdata = data.frame(age = 0))
lengthAtBirth #@lengthAtBirth=  0.3548516

# Start reading ID_ORILAE_eggDryMass data frame	
ID_ORILAE_eggDryMass<-data.frame(
			Variable = "eggDryMass", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			DOI2 = "ERN_2022_unpublishedWeightOriusLOriusL.fake",
			DOI3 = "10.3390_insects13030250",
			Category1 = "Unpublished",
			Category2 = "Unpublished",
			Category3 = "Article",
			DataTreatment = "Step 1: calculate the M-L curve and extract the a and b factors. Step 2: obtain the growth curve based on developing times and lengths. Step 3: calculating the factor to convert from dry to wet mass and viceversa. Step 4: calculating the wet mass of the new born using the a and b factors and the length of the new born taking into account the M-L relation. Step 5: convert the wetMass of the new born to dry mass with conversionToWetMass value, obtaining finally eggDryMass."
)
# End reading ID_ORILAE_eggDryMass data frame		  

eggDryMass<-coefficientForMassA*lengthAtBirth^scaleForMassB
eggDryMass #@eggDryMass=  0.08119889

# Start reading ID_ORILAE_femaleWetMass data frame	
ID_ORILAE_femaleWetMass<-data.frame(
			Variable = "femaleWetMass", 
			DOI1 = "ERN_2022_unpublishedLengthOriusLOriusL.fake",
			DOI2 = "ERN_2022_unpublishedWeightOriusLOriusL.fake",
			DOI3 = "10.3390_insects13030250",
			Category1 = "Unpublished",
			DataTreatment = "Step 1: we took the length of the female at the maximum value of devtime. Step 2: we calculated factors a and b for M-L regression. Step 3: we calculated the ratio for conversion to wet mass. Step 4: we used the M~a*L^b equation to calculate the femaleWetMass directly from the growth curve."
)
# End reading ID_ORILAE_femaleWetMass data frame


female_predicted_length <- predict(best_model, newdata = 
                           data.frame(age = max(full_growth_curve_table$age)))
femaleDryMass<-coefficientForMassA*female_predicted_length^scaleForMassB
femaleWetMass<-femaleDryMass*conversionToWetMass
femaleWetMass  #@femaleWetMass=0.02661989

female_predicted_length <- predict(best_model, newdata = data.frame(age=max(full_growth_curve_table$age)))
femaleDryMass<-coefficientForMassA*female_predicted_length^scaleForMassB
femaleWetMass<-femaleDryMass*conversionToWetMass
femaleWetMass #@femaleWetMass= 2.704925


# Start reading ID_ORILAE_lMin data frame	
ID_ORILAE_lMin<-data.frame(
			Variable = "lMin", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			Category1 = "Unpublished",
			DataTreatment = "Obtained from data using min function"
)
# End reading ID_ORILAE_lMin data frame		  

length<-read.table(file.path(script_path, "lengths_orius_estefania.txt"),header=TRUE)
length_female<-subset(length, stage == "female")
lMin<-min(length_female$length)
lMin #@lMin=0.93

# Start reading ID_ORILAE_lMax data frame	
ID_ORILAE_lMax<-data.frame(
			Variable = "lMax", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			Category1 = "Unpublished",
			DataTreatment = "Obtained from data using max function"
)
# End reading ID_ORILAE_lMax data frame		  

length<-read.table(file.path(script_path, "lengths_orius_estefania.txt"),header=TRUE)
length_female<-subset(length, stage == "female")
lMax<-max(length_female$length)
lMax #@lMax=1.11


# Start reading ID_ORILAE_growthK data frame	
ID_ORILAE_growthK<-data.frame(
			Variable = "growthK", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			DOI2 = "10.3390_insects13030250",
			Category1 = "Unpublished",
			Category2 = "Article",
			DataTreatment = "Step 1: We calculated the mean for each stage (except male) and we built a stages-length table. Step 2: We calculated the mean from Gallego 2022 using the data from 3 to 10 eggs given due to the big difference of the treatment with 1 egg related to the rest of the stages. After that we ran a function to fit the best model and we obtained the structure of the growth curve model. Step 3: we extracted the k value with the function coef"
)
# End reading ID_ORILAE_growthK data frame

growthK<-fitted[[as.character(tempFromLab)]]$growth
growthK #@growth=0.1741377

# Start reading ID_ORILAE_pupaPeriodTime data frame	
ID_ORILAE_pupaPeriodTime<-data.frame(
			Variable = "pupaPeriodTime", 
			DOI1 = "10.1046_j.1570-7458.1997.00118.x",
			Category1 = "Article",
			DataTreatment = "We perfomed a mean of the preoviposition period"
)
# End reading ID_ORILAE_pupaPeriodTime data frame

pupaPeriodTime<-mean(rep_data$preoviposition_period)
pupaPeriodTime<-round(pupaPeriodTime,1)
pupaPeriodTime #@pupaPeriodTime=2.7

# Start reading ID_ORILAE_longevitySinceMaturation data frame	
ID_ORILAE_longevitySinceMaturation<-data.frame(
			Variable = "longevitySinceMaturation", 
			DOI1 = "10.3390_insects13030250",
			DOI2 = "10.1046_j.1570-7458.1997.00118.x",
			Category1 = "Article",
			Category2 = "Article",
			DataTreatment = "Step 1: We calculated the mean from Gallego 2022 using the data from 3 to 10 eggs given due to the big difference of the treatment with 1 egg related to the rest of the stages."
)
# End reading ID_ORILAE_longevitySinceMaturation data frame

ageoffirstmaturation<-max(age) + pupaPeriodTime
agelayingeggs<-mean(rep_data$oviposition_period) 

longevitySinceMaturation <- (ageoffirstmaturation + agelayingeggs)/ageoffirstmaturation
longevitySinceMaturation=round(longevitySinceMaturation,1)
longevitySinceMaturation #@longevitySinceMaturation=5


# Start reading ID_ORILAE_growthKmax data frame	
ID_ORILAE_growthKmax<-data.frame(
			Variable = "growthKmax", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			DOI2 = "10.3390_insects13030250",
			Category1 = "Unpublished",
			Category2 = "Article",
			DataTreatment = "Step 1: We calculated the mean for each stage (except male) and we built a stages-length table. Step 2: We calculated the mean from Gallego 2022 using the data from 3 to 10 eggs given due to the big difference of the treatment with 1 egg related to the rest of the stages. After that we ran a function to fit the best model and we obtained the structure of the growth curve model. Step 3: we extracted the k value with the function coef"
)
# End reading ID_ORILAE_growthKmax data frame


length_max <- aggregate(length ~ stage, data = length, max)
length_max <- length_max[-2,]
colnames(length_max)[1]<-"stages"
length_max$stages[1]<-"adult"

full_growth_curve_table_max<-merge(length_max,age_table,by="stages")
full_growth_curve_table_max<-full_growth_curve_table_max[order(full_growth_curve_table_max$age),]
full_growth_curve_table_max$length[1]=full_growth_curve_table$length[1]
full_growth_curve_table_max


orius_laevigatus_data_max <- list()

orius_laevigatus_data_max[[as.character(tempFromLab)]] = full_growth_curve_table_max


force_model = list()
force_model[[as.character(tempFromLab)]] = curveType

fitted_max<-gcmGAT(
	orius_laevigatus_data_max, 
	x = "age", y = "length",
	force_model = force_model
)


best_model_max<-fitted_max[[as.character(tempFromLab)]]$best_model



growthKmax<-fitted_max[[as.character(tempFromLab)]]$growth
growthKmax #@growthKmax=0.1563878

# Start reading ID_ORILAE_growthKmin data frame	
ID_ORILAE_growthKmin<-data.frame(
			Variable = "growthKmin", 
			DOI1 = "ERN_2022_unpublishedLengthOriusL.fake",
			DOI2 = "10.3390_insects13030250",
			Category1 = "Unpublished",
			Category2 = "Article",
			DataTreatment = "Step 1: We calculated the mean for each stage (except male) and we built a stages-length table. Step 2: We calculated the mean from Gallego 2022 using the data from 3 to 10 eggs given due to the big difference of the treatment with 1 egg related to the rest of the stages. After that we ran a function to fit the best model and we obtained the structure of the growth curve model. Step 3: we extracted the k value with the function coef"
)
# End reading ID_ORILAE_growthKmin data frame

length_min <- aggregate(length ~ stage, data = length, min)
length_min <- length_min[-2,]
colnames(length_min)[1]<-"stages"
length_min$stages[1]<-"adult"

full_growth_curve_table_min<-merge(length_min,age_table,by="stages")
full_growth_curve_table_min<-full_growth_curve_table_min[order(full_growth_curve_table_min$age),]
full_growth_curve_table_min$length[1]=full_growth_curve_table$length[1]
full_growth_curve_table_min


orius_laevigatus_data_min <- list()

orius_laevigatus_data_min[[as.character(tempFromLab)]] = full_growth_curve_table_min

fitted_min<-gcmGAT(
	orius_laevigatus_data_min, 
	x = "age", y = "length",
	force_model = force_model
)



best_model_min<-fitted_min[[as.character(tempFromLab)]]$best_model


growthKmin<-fitted_min[[as.character(tempFromLab)]]$growth
growthKmin #@growth= 0.1015599


# library(ggplot2)

# predictions_min <- predict(fitted_min$best_model, newdata = full_growth_curve_table_min)  # Valores inferiores
# predictions_max <- predict(fitted_max$best_model, newdata = full_growth_curve_table_max)  # Valores superiores

# # Añadir las predicciones al data frame
# full_growth_curve_table$predicted_min <- predictions_min
# full_growth_curve_table$predicted_max <- predictions_max

# # Crear el gráfico
# ggplot() + 
  # geom_ribbon(data = full_growth_curve_table, 
              # aes(x = age, ymin = predicted_min, ymax = predicted_max), 
              # fill = "lightgray", alpha = 0.5) +  # Área de fondo
  # geom_line(data = full_growth_curve_table_min, aes(x = age, y = length, color = "Min")) +
  # geom_point(data = full_growth_curve_table_min, aes(x = age, y = length, color = "Min")) +
  # geom_line(data = full_growth_curve_table, aes(x = age, y = length, color = "Mean")) +
  # geom_point(data = full_growth_curve_table, aes(x = age, y = length, color = "Mean")) +
  # geom_line(data = full_growth_curve_table_max, aes(x = age, y = length, color = "Max")) +
  # geom_point(data = full_growth_curve_table_max, aes(x = age, y = length, color = "Max")) +
  # labs(x = "Development Time (age)", y = "Length", title = "Growth Curve") +
  # scale_color_manual(values = c("Min" = "blue", "Mean" = "green", "Max" = "red")) +
  # theme_minimal() +
  # # Añadir la leyenda
  # scale_color_manual(name = "Legend", values = c("Min" = "blue", "Mean" = "green", "Max" = "red"))


# SECTION 2: In this section there are included all the parameters for WEAVER. 
# Some of them are directly inputed from the parametric values obtained in the previous section.
# The rest of the parameters are taken from the literature or there are set up by 
# programming criteria or by the user (this preferences must be understood before changing).

# =====================
# GENERAL PARAMETERS
# =====================

json.activatedHandling = FALSE  # If TRUE, predator handling time is considered for the simulation
json.activityUnderPredationRisk = 1.0  # Fraction of normal activity maintained under predation risk
json.poreUsePerInstar = c(1.0, 1.0, 1.0, 1.0, 1.0, 1.0)  # Pore use for each developmental instar
json.coefficientForHandlingTimeA = 28.407  # Coefficient 'A' for handling time equation
json.coefficientForHandlingTimeB = -0.664  # Coefficient 'B' for handling time equation
json.coefficientForHandlingTimeC = -6.365  # Coefficient 'C' for handling time equation
json.conversionToWetMass = c(
	conversionToWetMass, conversionToWetMass, conversionToWetMass,
	conversionToWetMass, conversionToWetMass, conversionToWetMass
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

json.genetics.modules.correlationCoefficientRhoPerModule = c(0.1, 0.1, 0.1, 0.1, 0.1)  # Correlation coefficients between traits in each genetic module
json.genetics.modules.traitsPerModule = 3  # Number of traits per genetic module
json.genetics.numberOfAllelesPerLocus = 10  # How many alleles exist at each locus
json.genetics.numberOfChiasmasPerChromosome = 2  # Number of chiasmas (crossover points) in meiosis per chromosome
json.genetics.numberOfLociPerTrait = 20  # Number of loci affecting each trait

# These parameters alter allele frequencies or values in the population over time.

json.genetics.modifyAlleles = list()

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
json.traits.base.assim.limits.max = 0.08000000000000089  # Upper limit on assimilation rate
json.traits.base.assim.limits.min = 0.4100000000000042  # Lower limit on assimilation rate
json.traits.base.assim.ranges.max = 0.82  # Upper range boundary for sampling assimilation
json.traits.base.assim.ranges.min = 0.81  # Lower range boundary for sampling assimilation
json.traits.base.assim.restrictValue = 0.01  # Precision or incremental step

json.traits.base.assim.temperature.dependent = FALSE
json.traits.base.assim.temperature.activationEnergy = 0.0
json.traits.base.assim.temperature.energyDecay = 0.0
json.traits.base.assim.temperature.temperatureOptimal = 0.0
json.traits.base.assim.temperature.temperatureRef = 0.0

## base - devTime
# If the user does not want genetic variation, introduce the last value 
# of ageVector for both ranges (min and max). In case of having data 
# from literature you can modify these values. 
json.traits.base.devTime.limits.max = 0.0  # If set NULL, there is no variation over the maximum range.
json.traits.base.devTime.limits.min = 0.0  # If set NULL, there is no variation over the minimum range.
json.traits.base.devTime.ranges.max = ageVector[[length(ageVector)]]  # If max and min = max(ageVector), no genetic variation
json.traits.base.devTime.ranges.min = ageVector[[length(ageVector)]]  # If max and min = max(ageVector), no genetic variation
json.traits.base.devTime.restrictValue = 0.0  # If NULL, No increment restriction will be applied

json.traits.base.devTime.temperature.dependent = FALSE
json.traits.base.devTime.temperature.activationEnergy = 0.0
json.traits.base.devTime.temperature.energyDecay = 0.0
json.traits.base.devTime.temperature.temperatureOptimal = 0.0
json.traits.base.devTime.temperature.temperatureRef = 0.0

## base - eggDevTime
json.traits.base.eggDevTime.limits.max = 0.07  # Max for individual egg development time
json.traits.base.eggDevTime.limits.min = 0.01  # Min for individual egg development time
json.traits.base.eggDevTime.ranges.max = 3.0  # Upper boundary for sampling
json.traits.base.eggDevTime.ranges.min = 2.0  # Lower boundary for sampling
json.traits.base.eggDevTime.restrictValue = 0.01  # Increment step

json.traits.base.eggDevTime.temperature.dependent = FALSE
json.traits.base.eggDevTime.temperature.activationEnergy = 0.0
json.traits.base.eggDevTime.temperature.energyDecay = 0.0
json.traits.base.eggDevTime.temperature.temperatureOptimal = 0.0
json.traits.base.eggDevTime.temperature.temperatureRef = 0.0

## base - energy_tank
json.traits.base.energy_tank.limits.max = 0.010000000000000014  # Maximum capacity for the "energy tank"
json.traits.base.energy_tank.limits.min = 0.017500000000000012  # Minimum capacity
json.traits.base.energy_tank.ranges.max = 0.36  # Upper boundary for sampling
json.traits.base.energy_tank.ranges.min = 0.32  # Lower boundary for sampling
json.traits.base.energy_tank.restrictValue = 0.01  # Increment step

## base - factorEggMass
json.traits.base.factorEggMass.limits.max = 0.495 
json.traits.base.factorEggMass.limits.min = 0.495
json.traits.base.factorEggMass.ranges.max = 0.001
json.traits.base.factorEggMass.ranges.min = -0.001
json.traits.base.factorEggMass.restrictValue = 0.01  

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
json.traits.base.growth.limits.max = 0.0  # Maximum growth rate limit (0 means no constraint or zero?)
json.traits.base.growth.limits.min = 0.0  # Minimum growth rate limit
json.traits.base.growth.ranges.max = growthKmax  # Upper boundary for sampling growth rate
json.traits.base.growth.ranges.min = growthKmin  # Lower boundary for sampling growth rate
json.traits.base.growth.restrictValue = 0.01  # Increment step

json.traits.base.growth.temperature.dependent = FALSE
json.traits.base.growth.temperature.activationEnergy = 0.0
json.traits.base.growth.temperature.energyDecay = 0.0
json.traits.base.growth.temperature.temperatureOptimal = 0.0
json.traits.base.growth.temperature.temperatureRef = 0.0

## base - coeffMassForScopeRadius
json.traits.base.coeffMassForScopeRadius.limits.max = 0.0  
json.traits.base.coeffMassForScopeRadius.limits.min = 0.0 
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

json.traits.base.coeffMassForSearchRadius.temperature.dependent = FALSE
json.traits.base.coeffMassForSearchRadius.temperature.activationEnergy = 0.0
json.traits.base.coeffMassForSearchRadius.temperature.energyDecay = 0.0
json.traits.base.coeffMassForSearchRadius.temperature.temperatureOptimal = 0.0
json.traits.base.coeffMassForSearchRadius.temperature.temperatureRef = 0.0

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

json.traits.base.coeffMassForSpeed.temperature.dependent = FALSE
json.traits.base.coeffMassForSpeed.temperature.activationEnergy = 0.0
json.traits.base.coeffMassForSpeed.temperature.energyDecay = 0.0
json.traits.base.coeffMassForSpeed.temperature.temperatureOptimal = 0.0
json.traits.base.coeffMassForSpeed.temperature.temperatureRef = 0.0

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
json.traits.base.lengthAtMaturation.restrictValue = 0.01  # Increment step

json.traits.base.lengthAtMaturation.temperature.dependent = FALSE
json.traits.base.lengthAtMaturation.temperature.tempSizeRuleVector = list()

## base - longevitySinceMaturation
json.traits.base.longevitySinceMaturation.limits.max = 0.0  
json.traits.base.longevitySinceMaturation.limits.min = 0.0 
json.traits.base.longevitySinceMaturation.ranges.max = longevitySinceMaturation 
json.traits.base.longevitySinceMaturation.ranges.min = longevitySinceMaturation
json.traits.base.longevitySinceMaturation.restrictValue = 0.01  

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
json.traits.base.met_rate.limits.max = 0.07474999999999991 
json.traits.base.met_rate.limits.min = 0.24024999999999977
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
json.traits.base.pupaPeriodTime.restrictValue = 0.01 

json.traits.base.pupaPeriodTime.temperature.dependent = FALSE
json.traits.base.pupaPeriodTime.temperature.activationEnergy = 0.0
json.traits.base.pupaPeriodTime.temperature.energyDecay = 0.0
json.traits.base.pupaPeriodTime.temperature.temperatureOptimal = 0.0
json.traits.base.pupaPeriodTime.temperature.temperatureRef = 0.0

## base - shock_resistance
json.traits.base.shock_resistance.limits.max = 0.04  # Maximum limit for shock resistance
json.traits.base.shock_resistance.limits.min = 0.1   # Minimum limit for shock resistance
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
json.traits.base.voracityProportion.ranges.min = 0.98
json.traits.base.voracityProportion.restrictValue = 0.01

json.traits.base.voracityProportion.temperature.dependent = FALSE
json.traits.base.voracityProportion.temperature.activationEnergy = 0.0
json.traits.base.voracityProportion.temperature.energyDecay = 0.0
json.traits.base.voracityProportion.temperature.temperatureOptimal = 0.0
json.traits.base.voracityProportion.temperature.temperatureRef = 0.0

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
json.traits.cellValue.cellEvaluationBiomass.ranges.max = 0.5 
json.traits.cellValue.cellEvaluationBiomass.ranges.min = 0.5 
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
json.traits.cellValue.conspecificWeight.ranges.max = 0.5 
json.traits.cellValue.conspecificWeight.ranges.min = 0.5 
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
		trait = "base$energy_tank",
		element = "value"
	),
	list(
		trait = "base$growth",
		element = "value"
	),
	list(
		trait = "base$eggDevTime",
		element = "value"
	),
	list(
		trait = "base$factorEggMass",
		element = "value"
	),
	list(
		trait = "base$assim",
		element = "value"
	),
	list(
		trait = "base$voracity",
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
		trait = "base$lengthAtMaturation",
		element = "value"
	),
	list(
		trait = "base$dummy",
		element = "value"
	),
	list(
		trait = "base$coeffMassForSearchRadius",
		element = "value"
	),
	list(
		trait = "base$scaleMassForSearchRadius",
		element = "value"
	),
	list(
		trait = "base$coeffMassForSpeed",
		element = "value"
	),
	list(
		trait = "base$scaleMassForSpeed",
		element = "value"
	)
)

# =====================
# GROWTH MODULE PARAMETERS
# =====================

json.growthModule.assignedForMolt = 0.99  # Fraction of resources allocated to molting
json.growthModule.betaScaleTank = 1.01  # Scaling factor for the energy tank in the growth module
json.growthModule.capitalBreeding.enabled = FALSE  # If TRUE, uses stored resources for reproduction
json.growthModule.capitalBreeding.capitalBreedingParams.numberOfCapitalBreeds = 0
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
json.growthModule.growthModel.temperature.dependent = FALSE
json.growthModule.growthModel.temperature.growthModelThermalChanges = list()
json.growthModule.habitatShift = numeric(0)
json.growthModule.habitatShiftFactor = 0.0
json.growthModule.instarFirstReproduction = instarFirstReproduction
json.growthModule.maxPlasticityKVonBertalanffy = 0.25  # Max plasticity for growth rate K in Von Bertalanffy model
json.growthModule.minPlasticityKVonBertalanffy = 0.3   # Min plasticity for growth rate K
json.growthModule.moltingAgeThreshold = 0.0  # Age threshold at which molting might occur
json.growthModule.scaleForMassB = scaleForMassB  # Scale factor B in mass-growth equations
json.growthModule.scaleForMassBforMature = scaleForMassBforMature  # Same, but used post-maturation

# =====================
# ADDITIONAL PARAMETERS
# =====================

json.habitatShiftAfterBreeding = FALSE  # If TRUE, individuals shift habitat after breeding
json.habitatShiftAfterBreedingFactor = 0.0
json.habitatShiftBeforeBreeding = FALSE # If TRUE, individuals shift habitat before breeding
json.habitatShiftBeforeBreedingFactor = 0.0

json.individualsPerInstar = c(25, 25, 25, 25, 25, 25) 
# Number of individuals simulated per instar

json.requiredRepresentAllPossibleAlleles = FALSE

json.maleMaxReproductionEvents = 8   # Maximum number of reproduction events for a male
json.maleMobility = 1  # Mobility factor for males (compared to females or baseline)
json.maleReproductionFactor = 0.25  # Fraction of how many mates or eggs a male can fertilize compared to baseline
json.minRelativeHumidityThreshold = c(10, 10, 10, 10, 10, 10)  # Minimum humidity needed for survival or normal behavior


# =====================
# FINAL PARAMETERS
# =====================

json.name = "Orius_laevigatus"  # Name of the species or simulation run
json.YodzisA = 0.1  # Coefficient used in an adult trait or growth equation
json.YodzisB = 0.75       # Another coefficient for adult trait or growth equation
json.numberOfSimulatedAttacks = 10000  # Number of attacks to simulate in predation sub-model
json.percentageCostForMetabolicDownregulationSearchArea = 0.5  # Cost factor for search area under metabolic downregulation
json.percentageCostForMetabolicDownregulationSpeed = 0.5       # Cost factor for speed under metabolic downregulation
json.percentageCostForMetabolicDownregulationVoracity = 0.5    # Cost factor for voracity under metabolic downregulation
json.percentageMetabolicDownregulation = 0.0  # Overall fraction of metabolic downregulation
json.plasticityDueToConditionSearch = 0.05    # Plasticity factor for search trait based on condition
json.plasticityDueToConditionSpeed = 0.05     # Plasticity factor for speed trait based on condition
json.preserveLeftovers = FALSE  # If TRUE, unconsumed resources or partial prey remain available
json.probabilityDeathFromBackground = c(0.0, 0.0, 0.0, 0.0, 0.0, 0.0)  # Baseline mortality rate from non-predation causes
json.sexRatio = sexRatio  # Fraction of female offspring (or ratio of females to males)

json.decisions.predationProbability.additiveMechanism = FALSE
json.decisions.predationProbability.isRemainingWeightNull = FALSE
json.decisions.predationProbability.escapeProbability.additiveMechanism = FALSE
json.decisions.predationProbability.escapeProbability.isRemainingWeightNull = FALSE
json.decisions.predationProbability.escapeProbability.cVelocity = 0.0
json.decisions.predationProbability.useGlobalMaximumPreyVoracity = TRUE
json.decisions.edibilityValue.additiveMechanism = FALSE
json.decisions.edibilityValue.isRemainingWeightNull = FALSE
json.decisions.edibilityValue.qualityResourceAssessment = TRUE
json.decisions.decisionWithinPatch.killProbability = 1.0  # Probability that a predation event kills the prey if attacked
json.decisions.decisionAmongPatches.weightIndividualToGlobalAssessment = 0.0
json.decisions.sensoryModel.beta = 2.5

json.sexualType = "haplodiploid"  # Reproduction type (e.g., haplodiploid, diploid, etc.)
json.statisticsIndividualsPerInstar = 1000  # Number of individuals used in statistics per instar
json.tempFromLab = tempFromLab  # Reference temperature from lab conditions

# =====================
# ONTOGENETIC LINKS 
# =====================

#@ TODO: Introduce here the table with the preferences and profitabilities (trophic network).

json.ontogenetic_links <- data.frame(
    Species_name_prey = character(0),
    Instar_predator = numeric(0),
    Instar_prey = numeric(0), 
    Preferences = numeric(0), 
    Profitability = numeric(0),  
    stringsAsFactors = FALSE 
)


json.ontogenetic_links <- rbind(
	json.ontogenetic_links,
	do.call(rbind, lapply(1:length(json.individualsPerInstar), function(instar_predator) {
		do.call(rbind, lapply(1:4, function(instar_prey) {
			data.frame(
				Species_name_prey = "Amblyseius_swirskii",
				Instar_predator = instar_predator,
				Instar_prey = instar_prey,
				Preferences = 1.0/4.0,
				Profitability = 0.1,
				stringsAsFactors = FALSE
			)
		}))
	}))
)
