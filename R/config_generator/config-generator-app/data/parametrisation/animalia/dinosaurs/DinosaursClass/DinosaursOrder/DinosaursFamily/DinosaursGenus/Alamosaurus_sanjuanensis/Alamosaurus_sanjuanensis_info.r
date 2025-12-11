# @@@@ species: Alamosaurus sanjuanensis

if (!"app" %in% ls()) {
	rm(list=ls(all=TRUE))
	
	script_path <- dirname(sys.frame(1)$ofile)
}

parametrisation_path <- sub("(.*/parametrisation)/.*", "\\1", script_path)


source(file.path(parametrisation_path, "utilities", "codes", "functions", "growth_curve_modelfitting", "gcmGAT.r"))



set.seed(123)



# Option to avoid plotting 
# options(allow_plotting = FALSE)

# Start reading ID_ALASAN_tempFromLab data frame

#@@BVazquez - documentar biblio sobre temperatura Cretaci
ID_ALASAN_tempFromLab<-data.frame(
			Variable = "tempFromLab",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "taken from methods"
)
# End of ID_ALASAN_tempFromLab data frame
tempFromLab<-21 ##data for calculations here
tempFromLab #@tempFromLab=21  ##data for Json for simulation

# Start reading ID_ALASAN_eggMassFromEquation data frame	
ID_ALASAN_eggMassFromEquation<-data.frame(
			Variable = "eggMassFromEquation", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Not available"
)
# End reading ID_ALASAN_eggMassFromEquation data frame

eggMassFromEquation<-FALSE
eggMassFromEquation #@eggMassFromEquation=FALSE

# Start reading ID_ALASAN_sexRatio data frame		  
ID_ALASAN_sexRatio<-data.frame(
			Variable = "sexRatio", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Not available. We assume fisherian 50:50 sex ratio"
)
# End of ID_ALASAN_sexRatio data frame

sexRatio<-0.5
sexRatio #@sexRatio=0.5

# Start reading ID_ALASAN_conversionToWetMass data frame
ID_ALASAN_conversionToWetMass<-data.frame(
			Variable = "conversionToWetMass", 
			DOI1 = "10.1007_BF00346269", #@@BVazquez
			Category1 = "Article",
			DataTreatment = "We performed a ratio between wetMass and dryMass for all stages of a close species (Phytoseiulus persimilis) creating a variable called conversiontToWetMass. Then, we performed a weighted mean for preadult stages, male adult stage, female adult stage and gravid females. At the end, we built the conversionToWetMass performing the mean of the weighted means for the four classified stages (preadult, male, female and gravid female)"
)
 # End of ID_ALASAN_conversionToWetMass data frame

conversionToWetMass<-3.3333
conversionToWetMass #@conversionToWetMass=3.3333


# Start reading ID_ALASAN_eggsPerBatch data frame
ID_ALASAN_eggsPerBatch<-data.frame(
			Variable = "eggsPerBatch",
			DOI1 = NA,
			ISSN = "9780444423726",#@@BVazquez
			Category1 = "Book",
			DataTreatment = "Develop text in scientific format for publication"
)
# End of ID_ALASAN_eggsPerBatch data frame

eggsPerBatch<-15
eggsPerBatch #@eggsPerBatch=15

# Start reading ID_ALASAN_ageVector data frame
ID_ALASAN_ageVector<-data.frame(
			Variable = "ageVector",
			DOI1 = "12.1009_BF005517.fake", #@BVazquez
			Category1 = "Thesis dissertation",
			DataTreatment = "Taken from Table 2.2.1.b for commercial (total fecundity)"
)		
# End of ID_ALASAN_ageVector data frame


##200 years lifespan according to Lehman and Woodward, but estimates differ according to authors
##as low as 45 years - thus, @@BVazquez, @@Jordi - simulations with different lifespan assumptions
##
longevity = 200

b=0.075 ##span in publication 0.065-0.08. So it can be used for simulations

##predict mass with the Lehman and Woodard model. 
t=1:longevity
M=32663*(1-exp(-b*t))^3 ##the minus "-" before the "b" sign was not included in the publication - mass in Kg
M[1]  ##this provides a mass for the newlyborn of zero

##Since growth is determinate, we need to decide the ageOfFirstMaturation
for(i in 1:length(M)){
	if(i>1){
		if(round(M[i]) == round(M[i-1])){
            maxMass=M[i-1]
            ageOfFirstMaturation = t[i-1]

			break
		}
	}
}


agesInYears=1:ageOfFirstMaturation

ageVector = agesInYears*371
#@@David: cómo se pone en R y para Json?

##per produir el ageVector llegible per transformar al Json
paste0(ageVector,collapse=",")

ageVector #@ageVector="371,742,1113,1484,1855,2226,2597,2968,3339,3710,4081,4452,4823,5194,5565,5936,6307,6678,7049,7420,7791,8162,8533,8904,9275,9646,10017,10388,10759,11130,11501,11872,12243,12614,12985,13356,13727,14098,14469,14840,15211,15582,15953,16324,16695,17066,17437,17808,18179,18550,18921,19292,19663,20034,20405,20776,21147,21518,21889,22260,22631,23002,23373,23744,24115,24486,24857,25228,25599,25970,26341,26712,27083,27454,27825,28196,28567,28938,29309,29680,30051,30422,30793,31164,31535,31906,32277,32648,33019,33390,33761,34132,34503,34874,35245,35616,35987,36358,36729,37100,37471,37842,38213,38584,38955,39326,39697,40068,40439,40810,41181,41552,41923,42294,42665,43036,43407,43778,44149,44520,44891,45262"

# Start reading ID_ALASAN_instarFirstReproduction data frame
ID_ALASAN_instarFirstReproduction<-data.frame(
			Variable = "instarFirstReproduction",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "Mentioned in the text"
)
# End of ID_ALASAN_instarFirstReproduction data frame

##search for instar of first resproduction by finding closest age in ageVector to
##ageOfFirstMaturation
instarFirstReproduction<-length(ageVector)
instarFirstReproduction #@instarFirstReproduction=122


# Start reading ID_ALASAN_femaleMaxReproductionEvents data frame
ID_ALASAN_femaleMaxReproductionEvents<-data.frame(
			Variable = "femaleMaxReproductionEvents", 
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "taken from Table 2.2.2.b for females (mean from commercial and wild)"
)
# End of ID_ALASAN_	 data frame

totalReproductiveTime=ageVector[length(ageVector)]-ageOfFirstMaturation
femaleMaxReproductionEvents<-round(totalReproductiveTime/371)
femaleMaxReproductionEvents #@femaleMaxReproductionEvents=139


# Start reading ID_ALASAN_coefficientForMassA data frame
ID_ALASAN_coefficientForMassA<-data.frame(
			Variable = "coefficientForMassA", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = "Campione and Evans... explain"
)
# End of ID_ALASAN_coefficientForMassA data frame

ML_titano<-read.table(file.path(script_path, "Titano_M_L.txt"), h=T)

ML_titano$dryMass=ML_titano$Wet_M_kg*(1/conversionToWetMass)

##Fit the model and print the resulting coefficients
glm.ab<-glm(dryMass~log(Body_Length_km),data=ML_titano,family=gaussian(link=log))
coef(glm.ab)
    # (Intercept) log(Body_Length_km) 
          # 19.651499            2.063347 
		  
plot(ML_titano$Body_Length_km,ML_titano$dryMass,pch=16)
lines(ML_titano$Body_Length_km,exp(predict(glm.ab)),lwd=3)

coefficientForMassA<-as.vector(exp(coef(glm.ab)[1]))
coefficientForMassA #@coefficientForMassA = 30816900

# Start reading ID_ALASAN_scaleForMassB data frame
ID_ALASAN_scaleForMassB<-data.frame(
			Variable = "scaleForMassB", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = " "
)
# End of ID_ALASAN_scaleForMassB data frame

scaleForMassB <- as.vector(coef(glm.ab)[2])

scaleForMassB #@scaleForMassB=2.063347

# Start reading ID_ALASAN_coefficientForMassAforMature data frame
ID_ALASAN_coefficientForMassAforMature<-data.frame(
			Variable = "coefficientForMassAforMature", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = "No metamorphosis. Mature individuals take the value of juveniles"
)
# End of ID_ALASAN_coefficientForMassAforMature data frame

coefficientForMassAforMature <- coefficientForMassA
coefficientForMassAforMature #@coefficientForMassAforMature=30816900

# Start reading ID_ALASAN_scaleForMassBforMature data frame
ID_ALASAN_scaleForMassBforMature<-data.frame(
			Variable = "scaleForMassBforMature", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = "No metamorphosis. Mature individuals take the value of juveniles"
)
# End of ID_ALASAN_scaleForMassBforMature data frame

scaleForMassBforMature = scaleForMassB
scaleForMassBforMature #@scaleForMassBforMature=2.063347





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







# Start reading ID_ALASAN_curveType data frame
ID_ALASAN_curveType<-data.frame(
			Variable = "curveType",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "we performed a model autofit adjustment using AIC criterion"
)			
# End of ID_ALASAN_curveType data frame

##Important: convert the Mass to dryMass before using the ab coefficients
dryMass=M*(1/conversionToWetMass)

L=(dryMass/coefficientForMassA)^(1/scaleForMassB)

early_stages<-read.table(file.path(script_path, "alamo observed v2.txt"),h=TRUE)

early_stages$age<-early_stages$age*371
early_stages$mass_Kg=early_stages$mass_Kg*(1/conversionToWetMass)
early_stages$length<-(early_stages$mass_Kg/coefficientForMassA)^(1/scaleForMassB)

lm1<-lm(length~age,data=early_stages)
summary(lm1)$r.squared
predict(lm1,data.frame(age=0))



# plot(early_stages$age,early_stages$length,xlim=c(0,max(early_stages$age)),
# ylim=c(0,max(early_stages$length)))

# abline(lm(length~age,data=early_stages))
# abline(v=0,lwd=3)


##range of egg diameter for Titanosaurids 15-25 cm. ref. Klein et al. 2011. ##Vila et al. 2010 PlosONe range: 16-18 cm.
##assuming a 2-fold for animal total length gives 30-50 cm. Thus we are in a lower end 32 cm.
##@@BVazquez @@Jordi: run simulations assuming eggs and lengths in this range 30-50cm for lengthAtBirth.
##Note: Werner & Griebeler 2013 equation  0.09*femaleWetMass^0.311 gives 32 Kg for egg mass, way off
##Note: Erickson & Tumanova 2000 - 11% of female body length - way off 2.2 m for the newborn

lengthAtBirthProvisional<-as.vector(predict(lm1,data.frame(age=0)))
lengthAtBirthProvisional
eggDryMassProvisional=coefficientForMassA*lengthAtBirthProvisional^scaleForMassB


# data_long<-data.frame(age=c(0,ageVector),length=lengthAtBirthProvisional)

M2=32663*(1-exp(-b*(1:ageOfFirstMaturation)))^3
L2=(M2/coefficientForMassA)^(1/scaleForMassB)
length=c(lengthAtBirthProvisional,L2)
age=c(0,ageVector)
data_long<-cbind.data.frame(age,length)



# Start reading ID_ALASAN_femaleWetMass data frame
ID_ALASAN_femaleWetMass<-data.frame(
			Variable = "femaleWetMass",
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "12.1009_BF005517.fake",
			Category1 = "Article",
			Category2 = "Article", 
			Category3 = "Thesis dissertation", 
			DataTreatment = c("We took the maximum value of the growth curve for length and then we transformed this value,
							to wet weight using Conversion to wet mass")
)
# End of ID_ALASAN_femaleWetMass data frame

femaleMaxLength=max(L)
femaleDryMass<-coefficientForMassA*femaleMaxLength^scaleForMassB
femaleWetMass<-as.vector(femaleDryMass*conversionToWetMass)
femaleWetMass  #@femaleWetMass=32662.97



alamosaurus_sanjuanensis_data <- list()

alamosaurus_sanjuanensis_data[[as.character(tempFromLab)]] = data_long


fitted<-gcmGAT(alamosaurus_sanjuanensis_data, x = "age", y = "length", use_app = TRUE)



age_mean <- mean(data_long$age)
  age_sd   <- sd(data_long$age)
  length_mean <- mean(data_long$length)
  length_sd   <- sd(data_long$length)
	data_long$age_scaled <- (data_long$age - age_mean) / age_sd
  data_long$length_scaled <- (data_long$length - length_mean) / length_sd	
best_model<-fitted[[as.character(tempFromLab)]]$best_model



##the logistic form is mor obvious in the Mass
# plot(data_long$age,M)



if (isTRUE(getOption("allow_plotting"))) {
	predicted_values <- predict(best_model, newdata = data.frame(age = seq(0, max(data_long$age), by = 0.01)))
	plot(seq(0, max(data_long$age), by = 0.01), predicted_values, type = "l", xlab = "age", ylab = "Length", main = "Predicted Line")
	points(data_long$age, data_long$length, col = "red", pch = 16)
	legend("bottomright", legend = c("Predicted Line", "Real Spots"), col = c("black", "red"), pch = c(NA, 16), lty = c(1, NA))
}else {
  	cat("Plotting is disabled.\n")
}

curveType<-fitted[[as.character(tempFromLab)]]$best_model_name
curveType #@curveType="Logistic"


# Start reading ID_ALASAN_lengthAtBirth data frame
ID_ALASAN_lengthAtBirth<-data.frame(
			Variable = "lengthAtBirth", 
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "We performed a growth curve with the data of lenghts choosing the model by AIC criterion. Then, we took the intercept as value"
)
# End of ID_ALASAN_lengthAtBirth data frame

lengthatbirth <- predict(fitted[[as.character(tempFromLab)]]$best_model, newdata = data.frame(age = 0)) 
lengthAtBirth #@lengthAtBirth=0.002697969

# Start reading ID_ALASAN_eggDryMass data frame
ID_ALASAN_eggDryMass<-data.frame(
			Variable = "eggDryMass",
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "12.1009_BF005517.fake",
			Category1 = "Article",
			Category2 = "Article", 
			Category3 = "Thesis dissertation", 
			DataTreatment = "we used the M-L equation with a and b factor calculated in R2C10 and R2C11 and L (lengthAtBirth) from R2C8. Then we used conversionToWetMass from R2C9"
)
# End of ID_ALASAN_eggDryMass data frame

eggDryMass<-as.vector(coefficientForMassA*lengthAtBirth^scaleForMassB)
eggDryMass #@eggDryMass=  514.0444 ##@@Jordi, @@BVazquez, this is for age=4 because the model is incomplete


# Start reading ID_ALASAN_lMin data frame
ID_ALASAN_lMin<-data.frame(
			Variable = "lMin",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "minimum value of adult length extracted directly from the data"
)
# End of ID_ALASAN_lMin data frame

lMin<-min(amblyseius_swirskii_lengths_diego$Adult_Length)
lMin #@lMin=0.322

# Start reading ID_ALASAN_lMax data frame
ID_ALASAN_lMax<-data.frame(
			Variable = "lMax",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "maximum value of adult length extracted directly from the data"
)
# End of ID_ALASAN_lMax data frame

lMax<-max(amblyseius_swirskii_lengths_diego$Adult_Length)
lMax #@lMax=0.396

# Start reading ID_ALASAN_growthK data frame
ID_ALASAN_growthK<-data.frame(
			Variable = "growth",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_ALASAN_growthK data frame

growthK<-fitted[[as.character(tempFromLab)]]$growth
#print(paste("growthK: ", growthK)) 
growthK #@growthK= 0.0562038233678397"

# Start reading ID_ALASAN_pupaPeriodTime  data frame
ID_ALASAN_pupaPeriodTime <-data.frame(
			Variable = "pupaPeriodTime ",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "mean of values for pre-oviposition time for commercial and wild taken from Table 2.2.2.b"
)
# End of ID_ALASAN_pupaPeriodTime  data frame

pupaPeriodTime <-3.05
pupaPeriodTime  #@pupaPeriodTime =3.05

# Start reading ID_ALASAN_longevitySinceMaturation data frame
ID_ALASAN_longevitySinceMaturation<-data.frame(
			Variable = "longevitySinceMaturation",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "taken from Table 2.2.1.b (mean from commercial and wild)"
)
# End of ID_ALASAN_longevitySinceMaturation data frame

ageforage<-strsplit(ageVector,",")
ageforage<-unlist(ageforage)
adult_dev<-ageforage[[length(ageforage)]]
adult_dev<-as.numeric(adult_dev)


ageOfFirstMaturation<- adult_dev+pupaPeriodTime # Summarise of ageVector and pupaPeriodTime 
												# SUM ONLY ADULT age AND PUPAPERIODTIME
												# Take into account capital breeding, if higher 
												# than 1, adult_dev + pupaPeriodTime + (num_capital_breeding - 1)*time_for_capital_breeding
ageOfFirstMaturation
agelayingeggs<-mean(22.64,24.28) # Values of oviposition time from the table (mean of commercial and wild)
agelayingeggs

longevitySinceMaturation <- as.numeric((ageOfFirstMaturation + agelayingeggs)/ageOfFirstMaturation)
longevitySinceMaturation<-round(longevitySinceMaturation,4) #@longevitySinceMaturation=3.324435
longevitySinceMaturation #@longevitySinceMaturation=4.3294

# Start reading ID_ALASAN_growthKmin data frame
ID_ALASAN_growthKmin<-data.frame(
			Variable = "growthKmin",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_ALASAN_growthKmin data frame

data_for_min<- aggregate(length ~ stage+age, data = data_long_nonaggregate, min)
data_for_min$length[1]<-data_long$length[1]

x1 <- data_for_min$age[1]
x2 <- data_for_min$age[length(data_for_min$age)]
y1 <- data_for_min$length[1]
y2 <- data_for_min$length[length(data_for_min$age)]

slope <- (y2 - y1) / (x2 - x1)

for(i in 2:length(data_for_min$age)-1) {
	data_for_min$length[i] <- y1 + (slope + (0.001*(i-1))) * (data_for_min$age[i] - x1)
}

# data_for_min


alamosaurus_sanjuanensis_data_min <- list()

alamosaurus_sanjuanensis_data_min[[as.character(tempFromLab)]] = data_for_min


force_model = list()
force_model[[as.character(tempFromLab)]] = curveType

fitted_min<-gcmGAT(
	alamosaurus_sanjuanensis_data_min, 
	x = "age", y = "length",
	force_model = force_model
)



growthKmin<-fitted_min[[as.character(tempFromLab)]]$growth
# print(paste("growthKmin: ", growthKmin)) 
growthKmin #@growthKmin=0.07119579

# Start reading ID_ALASAN_growthKmax data frame
ID_ALASAN_growthKmax<-data.frame(
			Variable = "growthKmax",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_ALASAN_growthKmax data frame


data_for_max<-aggregate(length ~ stage+age, data = data_long_nonaggregate, max)
data_for_max$length[1]<-data_long$length[1]

# data_for_max

alamosaurus_sanjuanensis_data_max <- list()

alamosaurus_sanjuanensis_data_max[[as.character(tempFromLab)]] = data_for_max


fitted_max<-gcmGAT(
	alamosaurus_sanjuanensis_data_min, 
	x = "age", y = "length",
	force_model = force_model
)


growthKmax<-fitted_max[[as.character(tempFromLab)]]$growth
# print(paste("growthKmax: ", growthKmax)) 
growthKmax #@growthKmax= 0.4920556


if (isTRUE(getOption("allow_plotting"))) {

predictions_min <- predict(fitted_min[[as.character(tempFromLab)]]$best_model, newdata = data_for_min)  
predictions_max <- predict(fitted_max[[as.character(tempFromLab)]]$best_model, newdata = data_for_max)  


data_long$predicted_min <- predictions_min
data_long$predicted_max <- predictions_max

ggplot() + 
  geom_line(data = data_for_min, aes(x = age, y = length, color = "Min")) +
  geom_point(data = data_for_min, aes(x = age, y = length, color = "Min")) +
  geom_line(data = data_long, aes(x = age, y = length, color = "Mean")) +
  geom_point(data = data_long, aes(x = age, y = length, color = "Mean")) +
  geom_line(data = data_for_max, aes(x = age, y = length, color = "Max")) +
  geom_point(data = data_for_max, aes(x = age, y = length, color = "Max")) +
  labs(x = "Development Time (age)", y = "Length", title = "Growth Curve") +
  scale_color_manual(values = c("Min" = "blue", "Mean" = "green", "Max" = "red")) +
  theme_minimal() +
  # Añadir la leyenda
  scale_color_manual(name = "Legend", values = c("Min" = "blue", "Mean" = "green", "Max" = "red"))
}else {
  cat("Plotting is disabled.\n")
}


# SECTION 2: In this section there are included all the parameters for WEAVER. 
# Some of them are directly inputed from the parametric values obtained in the previous section.
# The rest of the parameters are taken from the literature or there are set up by 
# programming criteria or by the user (this preferences must be understood before changing).

# =====================
# GENERAL PARAMETERS
# =====================

json.activatedHandling = FALSE  # If TRUE, predator handling time is considered for the simulation
json.activityUnderPredationRisk = 1.0  # Fraction of normal activity maintained under predation risk
jjson.poreUsePerInstar = c(4.0, 4.0, 4.0, 4.0)  # Pore use for each developmental instar
json.coefficientForHandlingTimeA = 28.407  # Coefficient 'A' for handling time equation
json.coefficientForHandlingTimeB = -0.664  # Coefficient 'B' for handling time equation
json.coefficientForHandlingTimeC = -6.365  # Coefficient 'C' for handling time equation
json.conversionToWetMass = c(
	conversionToWetMass, conversionToWetMass,
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

json.genetics.modules.correlationCoefficientRhoPerModule = c(0.1, 0.1, 0.1, 0.1)  # Correlation coefficients between traits in each genetic module
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

json.individualsPerInstar = c(100, 100, 100, 100)
# Number of individuals simulated per instar

json.requiredRepresentAllPossibleAlleles = FALSE

json.maleMaxReproductionEvents = 8   # Maximum number of reproduction events for a male
json.maleMobility = 1  # Mobility factor for males (compared to females or baseline)
json.maleReproductionFactor = 0.25  # Fraction of how many mates or eggs a male can fertilize compared to baseline
json.minRelativeHumidityThreshold = c(10, 10, 10, 10)  # Minimum humidity needed for survival or normal behavior


# =====================
# FINAL PARAMETERS
# =====================

json.name = "Alamosaurus_sanjuanensis"  # Name of the species or simulation run
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
json.probabilityDeathFromBackground = c(0.0, 0.0, 0.0, 0.0)  # Baseline mortality rate from non-predation causes
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

