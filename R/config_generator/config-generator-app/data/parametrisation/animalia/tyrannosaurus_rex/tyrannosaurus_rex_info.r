# @@@@ species: tyrannosaurus_rex

if (!"app" %in% ls()) {
	rm(list=ls(all=TRUE))

	script_path <- dirname(sys.frame(1)$ofile)
}

parametrisation_path <- sub("(.*/parametrisation)/.*", "\\1", script_path)


library(ggplot2)
library(dplyr)

source(file.path(parametrisation_path, "utilities", "codes", "functions", "growth_curve_modelfitting", "gcmGAT.r"))
source(file.path(parametrisation_path, "utilities", "codes", "functions", "calculateIndividualsPerInstarTyrannosaurusRex.r"))


set.seed(123)

# Option to avoid plotting 
# options(allow_plotting = FALSE)

# Start reading ID_TYRREX_tempFromLab data frame

#@@BVazquez - documentar biblio sobre temperatura Cretaci
ID_TYRREX_tempFromLab<-data.frame(
			Variable = "tempFromLab",
			DOI1 = "10.5194/cp-12-1181-2016",
			Category1 = "Article",
			DataTreatment = "taken from supplementary simulations"
)
# End of ID_TYRREX_tempFromLab data frame
tempFromLab<-21.4 ##data for calculations here
tempFromLab #@tempFromLab=21.4  ##data for Json for simulation

# Start reading ID_TYRREX_eggClutchFromEquation data frame	
ID_TYRREX_eggClutchFromEquation<-data.frame(
			Variable = "eggClutchFromEquation", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Not available"  ##@David - is not common knowledge is equation not available
)
# End reading ID_TYRREX_eggClutchFromEquation data frame

eggClutchFromEquation<-"false"
eggClutchFromEquation #@eggClutchFromEquation="false"

# Start reading ID_TYRREX_eggMassFromEquation data frame	
ID_TYRREX_eggMassFromEquation<-data.frame(
			Variable = "eggMassFromEquation", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Not available"
)
# End reading ID_TYRREX_eggMassFromEquation data frame

eggMassFromEquation<-"false"
eggMassFromEquation #@eggMassFromEquation="false"

# Start reading ID_TYRREX_sexRatio data frame		  
ID_TYRREX_sexRatio<-data.frame(
			Variable = "sexRatio", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Not available. We assume fisherian 50:50 sex ratio"
)
# End of ID_TYRREX_sexRatio data frame

sexRatio<-0.5
sexRatio #@sexRatio=0.5

# Start reading ID_TYRREX_conversionToWetMass data frame
ID_TYRREX_conversionToWetMass<-data.frame(
			Variable = "conversionToWetMass", 
			DOI1 = "10.1007_BF00346269", #@@BVazquez
			Category1 = "Article",
			DataTreatment = "We performed a ratio between wetMass and dryMass for all stages of a close species (Phytoseiulus persimilis) creating a variable called conversionToWetMass. Then, we performed a weighted mean for preadult stages, male adult stage, female adult stage and gravid females. At the end, we built the conversionToWetMass performing the mean of the weighted means for the four classified stages (preadult, male, female and gravid female)"
)
 # End of ID_TYRREX_conversionToWetMass data frame

conversionToWetMass<-3.3333
conversionToWetMass #@conversionToWetMass=3.3333


# Start reading ID_TYRREX_indeterminateGrowth data frame	
ID_TYRREX_indeterminateGrowth<-data.frame(
			Variable = "indeterminateGrowth", 
			DOI1 = NA, ##document sources
			Category1 = NA,
			DataTreatment = "Write a paragraph in scientific format for publishing. Add references." 
)
# End reading ID_TYRREX_indeterminateGrowth data frame

indeterminateGrowth<-"true"
indeterminateGrowth #@indeterminateGrowth="true"

# Start reading ID_TYRREX_eggsPerBatch data frame
ID_TYRREX_eggsPerBatch<-data.frame(
			Variable = "eggsPerBatch",
			DOI1 = NA,
			ISSN = "9780444423726",#@@BVazquez
			Category1 = "Book",
			DataTreatment = "Develop text in scientific format for publication"
)
# End of ID_TYRREX_eggsPerBatch data frame

eggsPerBatch<-20 ##It appears to be a mean value from the literature of hadrosaurs (16-22) and it is present in past .json files for trex, however, in MOREDINO it is calculated in several ways, the one from griebeler and werner 2013 shows promise for eggDryMass: 0.09·DryMass^0.311, then from clutch mass(=0,384*(DryMass^0,496)): clutchMass/eggDryMass
eggsPerBatch #@eggsPerBatch=20 

# Start reading ID_TYRREX_coefficientForMassA data frame
ID_TYRREX_coefficientForMassA<-data.frame(
			Variable = "coefficientForMassA", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = "Campione and Evans... explain"
)
# End of ID_TYRREX_coefficientForMassA data frame

trex_M_A<-read.table(file.path(script_path, "Trex_rev_FMNH PR 2081.txt"), h=T)

trex_M_A$dryMass=trex_M_A$DryMass_Kg

##Fit the model that correlates Tyrannosaurus FC with dryMass (obtained from literature) and print the resulting coefficients as in dryMass=A(FC)^b
 glm.ab<-glm(DryMass_Kg~log(FC_Km),data=trex_M_A,family=gaussian(link="log"))
 summary(glm.ab)
             # Estimate Std. Error t value Pr(>|t|)    
# (Intercept) 2.836e+01  2.837e-04   99977   <2e-16 ***
# log(FC_Km)  2.754e+00  3.781e-05   72841   <2e-16 ***
# coefficientForMassA=exp(2.836e+01)
# scaleForMassB=2.754e+00 
# plot(trex_M_A$Age_D,trex_M_A$DryMass_Kg,pch=16)
# lines(trex_M_A$Age_D,exp(predict(glm.ab)),lwd=3)

plot(trex_M_A$FC_Km, trex_M_A$DryMass_Kg, pch = 16,xlab = "FC_Km", ylab = "DryMass_Kg")
plot(FC_Km ~ Age_Y, data = trex_M_A, pch = 16, xlab = "Age", ylab = "FC_Km")

coefficientForMassA<-as.vector(exp(coef(glm.ab)[1]))
# library(bit64)  
# coefficientForMassA=as.integer64(coefficientForMassA)
coefficientForMassA #@coefficientForMassA = 2081745097398 
#PREVIOUSLY 2072962791264.0

# Start reading ID_TYRREX_scaleForMassB data frame
ID_TYRREX_scaleForMassB<-data.frame(
			Variable = "scaleForMassB", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = " "
)
# End of ID_TYRREX_scaleForMassB data frame

scaleForMassB <- as.vector(coef(glm.ab)[2])
scaleForMassB #@scaleForMassB=2.754081
# PREVIOUSLY 2.754

# Start reading ID_TYRREX_coefficientForMassAforMature data frame
ID_TYRREX_coefficientForMassAforMature<-data.frame(
			Variable = "coefficientForMassAforMature", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = "No metamorphosis. Mature individuals take the value of juveniles"
)
# End of ID_TYRREX_coefficientForMassAforMature data frame

coefficientForMassAforMature <- coefficientForMassA
coefficientForMassAforMature #@coefficientForMassAforMature=2081745097398

# Start reading ID_TYRREX_scaleForMassBforMature data frame
ID_TYRREX_scaleForMassBforMature<-data.frame(
			Variable = "scaleForMassBforMature", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = "No metamorphosis. Mature individuals take the value of juveniles"
)
# End of ID_TYRREX_scaleForMassBforMature data frame

scaleForMassBforMature = scaleForMassB
scaleForMassBforMature #@scaleForMassBforMature=2.754081


# Start reading ID_TYRREX_pupaPeriodTime  data frame
ID_TYRREX_pupaPeriodTime <-data.frame(
			Variable = "pupaPeriodTime ",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "mean of values for pre-oviposition time for commercial and wild taken from Table 2.2.2.b"
)
# End of ID_TYRREX_pupaPeriodTime  data frame

pupaPeriodTime <-1.00
pupaPeriodTime  #@pupaPeriodTime =1.00

##Conservative estimation of 30 years according to Mallon and Hone (2024), the original curve and data was based on Cullen et al.(2020)
##
longevity = 34*371 #length(trex_M_A$Age_D)
t=1:longevity


# JUST IN CASE!! Lee et al. (2019) estimates an incubation time of 123+-30days for tyrannosaurus, so this could also be calculated as 
## obtain BL
# trex_L_A<-read.table("Trex_rev_FMNH PR 2081.txt",h=TRUE)
# trex_L_A$age<-trex_L_A$Age_D #Data is already in days in the .txt
# trex_L_A$mass_Kg=trex_L_A$DryMass_Kg #Data is already in dryMass in the .txt
# trex_L_A$length<-(trex_L_A$mass_Kg/coefficientForMassA)^(1/scaleForMassB)
# trex_L_A$age_years<-trex_L_A$age/371
# IncTime <- 123/371
# glm1<-glm(mass_Kg~log(age_years+IncTime),data=trex_L_A,gaussian(link="log"))
# exp(coef(glm1)[2])


ageVector <- seq(371, max(trex_M_A$Age_D), 371) #ageOfFirstMaturation is already in days

# THIS IS NO LONGER NEEDED HERE
# ##ageOfFirstMaturation is changed to closely match a day within the ageVector (exact year timing)
# ageOfFirstMaturation=ageVector[which((abs(ageOfFirstMaturation-ageVector))==min(abs(ageOfFirstMaturation-ageVector)))]


#@@David: cómo se pone en R y para Json?

##per produir el ageVector llegible per transformar al Json
paste0(ageVector,collapse=",")
# "371,742,1113,1484,1855,2226,2597,2968,3339,3710,4081,4452,4823,5194,5565,5936,6307,6678,7049,7420,7791,8162,8533,8904,9275,9646,10017,10388,10759,11130,11501,11872,12243"
# OR MORE SIMPLY
# paste0(trex_M_A$Age_D[-1], collapse = ",")
# "371,742,1113,1484,1855,2226,2597,2968,3339,3710,4081,4452,4823,5194,5565,5936,6307,6678,7049,7420,7791,8162,8533,8904,9275,9646,10017,10388,10759,11130,11501,11872,12243"

# Start reading ID_TYRREX_ageVector data frame
ID_TYRREX_ageVector<-data.frame(
			Variable = "ageVector",
			DOI1 = "12.1009_BF005517.fake", #@BVazquez
			Category1 = "Thesis dissertation",
			DataTreatment = "Taken from Table 2.2.1.b for commercial (total fecundity)"
)		
# End of ID_TYRREX_ageVector data frame

ageVector #@ageVector="371,742,1113,1484,1855,2226,2597,2968,3339,3710,4081,4452,4823,5194,5565,5936,6307,6678,7049,7420,7791,8162,8533,8904,9275,9646,10017,10388,10759,11130,11501,11872,12243"

# End of ID_TYRREX_instarFirstReproduction data frame

##@@simul (see also @@coDependency) ageOfFirstMaturation if changed other parameter in the optimizer.r must change
##(@BVazquez, @BVila justification to be written) and this value and see how the different
instarFirstReproduction<- 22 ## Carr et al. 2020 PeerJ young adult to adult  ##because ageVector starts at date of Instar II and xmid falls between ageVector indexes
instarFirstReproduction #@instarFirstReproduction=22

####@@coDependency - @@Mario - instarFirstReproduction when it changes, other parameters need to be recalculated such as
####instarFirstReproduction requires recalculation of: femaleMaxReproductionEvents, longevitySinceMaturation

# Start reading ID_TYRREX_femaleMaxReproductionEvents data frame
ID_TYRREX_femaleMaxReproductionEvents<-data.frame(
			Variable = "femaleMaxReproductionEvents", 
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "taken from Table 2.2.2.b for females (mean from commercial and wild)"
)
# End of ID_TYRREX_	 data frame

ageOfFirstMaturation=instarFirstReproduction*371
totalReproductiveTime=longevity-ageOfFirstMaturation #already in days
femaleMaxReproductionEvents<-round(totalReproductiveTime/371)
femaleMaxReproductionEvents #@femaleMaxReproductionEvents=12


### Modelfitting for the growth curve


# (coefficientForMassA*0.021^scaleForMassB)*conversionToWetMass
# (coefficientForMassA*0.035^scaleForMassB)*conversionToWetMass


age<-trex_M_A$Age_D     
length<-trex_M_A$FC_Km 
 
 
## age=c(0,ageVector)
temp<-rep(tempFromLab, length(age))
tyrannosaurus_data_frame<- data.frame(age=age, length=length)
tyrannosaurus_data <- list()

tyrannosaurus_data[[as.character(tempFromLab)]] = tyrannosaurus_data_frame

fitted <- gcmGAT(
			tyrannosaurus_data, 
			x="age", 
			y="length", 
			use_app = TRUE)

names("21.4")



best_model <- fitted[[as.character(tempFromLab)]]$best_model

# Start reading ID_TYRREX_curveType data frame
ID_TYRREX_curveType<-data.frame(
			Variable = "curveType",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "we performed a model autofit adjustment using AIC criterion"
)			
# End of ID_TYRREX_curveType data frame

 curveType<-fitted$'21.4'$best_model_name
 curveType #@curveType="Logistic4P"


# Start reading ID_TYRREX_femaleWetMass data frame
ID_TYRREX_femaleWetMass<-data.frame(
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
# End of ID_TYRREX_femaleWetMass data frame

femaleMaxLength=predict(fitted$'21.4'$best_model,data=ageVector[length(ageVector)])[length(ageVector)]
femaleDryMass<-coefficientForMassA*femaleMaxLength^scaleForMassB
femaleWetMass<-as.vector(femaleDryMass*conversionToWetMass)
femaleWetMass  #@femaleWetMass=8313.572


# Start reading ID_TYRREX_lengthAtBirth data frame
ID_TYRREX_lengthAtBirth<-data.frame(
			Variable = "lengthAtBirth", 
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "We performed a growth curve with the data of lenghts choosing the model by AIC criterion. Then, we took the intercept as value"
)
# End of ID_TYRREX_lengthAtBirth data frame

#Neonatal mass is calculated based on Cullen et al. (2020) (supplementary information, following Grady et al., 2014)
#egg mass = 0.05 x (adult mass)^0.46, then neonatal mass = egg mass * 0.7? Is this assuming that 0.3 is eggshell?

eggDryMass_Cullen= 0.05*femaleDryMass^0.46 
massAtBirth = eggDryMass_Cullen * 0.7
eggDryMass_Cullen
massAtBirth
# neonatal femur circumferences were estimated using the  “developmental mass extrapolation” approach of Erickson and Tumanova (2000)
# neonatal_femur_circum <- adult_femur_circum * (neonatal_mass / adult_mass)^(1/3)
neonatal_FC <- femaleMaxLength*(massAtBirth / femaleDryMass)^(1/3)
neonatal_FC

#From this, to obtain a BL for the newborn 
# BM to FemurLength (mm) based on Christian and Fariña (2004), following Mallon and Hone (2024)
neonatal_FL<-10^((log10(massAtBirth) + 6.288) / 3.222)
# FL to BL (mm) based on Currie (2000), following Mallon and Hone (2024)
lengthAtBirth=10^(1.0276*(log10(neonatal_FL))+0.8437)
lengthAtBirth=lengthAtBirth*0.000001
lengthAtBirth
# 0.0007641174km=76.4cm (BL whole hatchling); 76.4cm/2=38.2cm=eggSize (25-55cm in titano)

# Previously, based on our model, it checks if our results have similar values to allometric equations
lengthatbirth_original <- predict(fitted$'21.4'$best_model,data=ageVector[1])[1]
lengthAtBirth=as.vector(lengthatbirth_original)
lengthAtBirth#@lengthAtBirth=3.515999e-05 ##3.5 cm
massAtBirth=as.vector(coefficientForMassA*lengthAtBirth^scaleForMassB)
massAtBirth 
# BM to FemurLength (mm) based on Christian and Fariña (2004), following Mallon and Hone (2024)
neonatal_FL<-10^((log10(massAtBirth) + 6.288) / 3.222)
# FL to BL (mm) based on Currie (2000), following Mallon and Hone (2024)
fullLengthAtBirth=10^(1.0276*(log10(neonatal_FL))+0.8437)
fullLengthAtBirth=fullLengthAtBirth*0.000001
fullLengthAtBirth ###change name
0.0007339976


# Start reading ID_TYRREX_eggDryMass data frame
ID_TYRREX_eggDryMass<-data.frame(
			Variable = "eggDryMass",
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "12.1009_BF005517.fake",
			Category1 = "Article",
			Category2 = "Article", 
			Category3 = "Thesis dissertation", 
			DataTreatment = "we used the M-L equation with a and b factor calculated in R2C10 and R2C11 and L (lengthAtBirth) from R2C8. Then we used conversionToWetMass from R2C9"
)
# End of ID_TYRREX_eggDryMass data frame

eggDryMass<-as.vector(coefficientForMassA*lengthAtBirth^scaleForMassB)
eggDryMass #@eggDryMass= 4856.21


# Start reading ID_TYRREX_lMin data frame
ID_TYRREX_lMin<-data.frame(
			Variable = "lMin",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "body length of the adult for lMin and lMax because we don't want variability"
)
# End of ID_TYRREX_lMin data frame

lMin<-femaleMaxLength
lMin #@lMin=0.000576262

# Start reading ID_TYRREX_lMax data frame
ID_TYRREX_lMax<-data.frame(
			Variable = "lMax",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "body length of the adult for lMin and lMax because we don't want variability"
)
# End of ID_TYRREX_lMax data frame

lMax<-femaleMaxLength
lMax #@lMax=0.000576262

# Start reading ID_TYRREX_growthK data frame
ID_TYRREX_growthK<-data.frame(
			Variable = "growth",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_TYRREX_growthK data frame

k <-1/coef(best_model)["scal"]
k=as.vector(k)
k
growthK<-k
growthK #@growthK= 0.0007657056


# Start reading ID_TYRREX_longevitySinceMaturation data frame
ID_TYRREX_longevitySinceMaturation<-data.frame(
			Variable = "longevitySinceMaturation",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "taken from Table 2.2.1.b (mean from commercial and wild)"
)
# End of ID_TYRREX_longevitySinceMaturation data frame

ageOfFirstMaturation
8162
lifeTimeLayingEggs=longevity-ageOfFirstMaturation # Values of oviposition time from the table (mean of commercial and wild)

longevitySinceMaturation <- as.numeric((ageOfFirstMaturation + lifeTimeLayingEggs)/ageOfFirstMaturation)
longevitySinceMaturation<-round(longevitySinceMaturation,4) 
longevitySinceMaturation #@longevitySinceMaturation=1.5455


# Start reading ID_TYRREX_growthKmin data frame
ID_TYRREX_growthKmin<-data.frame(
			Variable = "growthKmin",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_TYRREX_growthKmin data frame

growthKmin<-as.vector(growthK)
growthKmin #@growthKmin=0.0007657056



# Start reading ID_TYRREX_growthKmax data frame
ID_TYRREX_growthKmax<-data.frame(
			Variable = "growthKmax",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_TYRREX_growthKmax data frame

growthKmax<-as.vector(growthK)
growthKmax #@growthKmax= 0.0007657056


# SECTION 2: In this section there are included all the parameters for WEAVER. 
# Some of them are directly inputed from the parametric values obtained in the previous section.
# The rest of the parameters are taken from the literature or there are set up by 
# programming criteria or by the user (this preferences must be understood before changing).

# =====================
# GENERAL PARAMETERS
# =====================

json.activatedHandling = FALSE  # If TRUE, predator handling time is considered for the simulation
json.activityUnderPredationRisk = 1.0  # Fraction of normal activity maintained under predation risk
json.poreUsePerInstar = c(4.0, 4.0, 4.0, 4.0)  # Pore use for each developmental instar
json.coefficientForHandlingTimeA = 28.407  # Coefficient 'A' for handling time equation
json.coefficientForHandlingTimeB = -0.664  # Coefficient 'B' for handling time equation
json.coefficientForHandlingTimeC = -6.365  # Coefficient 'C' for handling time equation
json.conversionToWetMass = c(
	conversionToWetMass, conversionToWetMass,
	conversionToWetMass, conversionToWetMass
)  # Factor to convert from dry mass (or another measure) to wet mass
json.daysWithoutFoodForMetabolicDownregulation = 999999999.0  # Days without food before metabolic downregulation
json.debug.surviveWithoutFood = FALSE  # Debug flag to allow survival without food if TRUE
json.debug.forcePresenceAllResourcesInvolved = FALSE  # If TRUE, forces all resource types to be present in a simulation
json.defaultHuntingMode = "sit_and_wait"  # Default hunting mode if no other is specified
json.femaleMaxReproductionEvents = femaleMaxReproductionEvents  # Maximum number of reproduction events for a female
json.forDensitiesA = 74.8  # Parameter 'A' for population density functions
json.forDensitiesB = -0.75  # Parameter 'B' for population density functions
json.forDensitiesEggSize = (lengthAtBirth*0.3)+lengthAtBirth  # Egg size used in some density-related calculations
json.forDensitiesGrowth = growthK  # Growth rate factor in density-related calculations

# =====================
# GENETICS PARAMETERS
# =====================

json.genetics.modules.correlationCoefficientRhoPerModule = list(0.1, 0.1)  # Correlation coefficients between traits in each genetic module
json.genetics.modules.traitsPerModule = 3  # Number of traits per genetic module
json.genetics.numberOfAllelesPerLocus = 10  # How many alleles exist at each locus
json.genetics.numberOfChiasmasPerChromosome = 2  # Number of chiasmas (crossover points) in meiosis per chromosome
json.genetics.numberOfLociPerTrait = 20  # Number of loci affecting each trait

# These parameters alter allele frequencies or values in the population over time.

json.genetics.modifyAlleles = list(
	list(
		affectedLocus = list(
			0.3,
			0.6,
			0.9
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
			0.3,
			0.6,
			0.9
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
json.traits.base.actE_met.ranges.max = 0.0  # Upper range boundary for sampling initial trait values
json.traits.base.actE_met.ranges.min = 0.0  # Lower range boundary for sampling initial trait values
json.traits.base.actE_met.restrictValue = 0.01  # Precision or incremental step for this trait

json.traits.base.actE_met.temperature.dependent = FALSE
json.traits.base.actE_met.temperature.activationEnergy = 0.0
json.traits.base.actE_met.temperature.energyDecay = 0.0
json.traits.base.actE_met.temperature.temperatureOptimal = 0.0
json.traits.base.actE_met.temperature.temperatureRef = 0.0

## base - assim // as used in Joan project
json.traits.base.assim.limits.max = 0.0  # Upper limit on assimilation rate
json.traits.base.assim.limits.min = 0.0  # Lower limit on assimilation rate
json.traits.base.assim.ranges.max = 0.9  # Upper range boundary for sampling assimilation
json.traits.base.assim.ranges.min = 0.8  # Lower range boundary for sampling assimilation
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
json.traits.base.devTime.ranges.max = ageVector[length(ageVector)]  # If max and min = max(ageVector), no genetic variation
json.traits.base.devTime.ranges.min = ageVector[length(ageVector)]  # If max and min = max(ageVector), no genetic variation
json.traits.base.devTime.restrictValue = 0.01  # If NULL, No increment restriction will be applied

json.traits.base.devTime.temperature.dependent = FALSE
json.traits.base.devTime.temperature.activationEnergy = 0.0
json.traits.base.devTime.temperature.energyDecay = 0.0
json.traits.base.devTime.temperature.temperatureOptimal = 0.0
json.traits.base.devTime.temperature.temperatureRef = 0.0

## base - eggDevTime #TODO
json.traits.base.eggDevTime.limits.max = 0.0  # Max for individual egg development time
json.traits.base.eggDevTime.limits.min = 0.0  # Min for individual egg development time
json.traits.base.eggDevTime.ranges.max = 153.0  # Upper boundary for sampling
json.traits.base.eggDevTime.ranges.min = 93.0  # Lower boundary for sampling
json.traits.base.eggDevTime.restrictValue = 0.01  # Increment step

json.traits.base.eggDevTime.temperature.dependent = FALSE
json.traits.base.eggDevTime.temperature.activationEnergy = 0.0
json.traits.base.eggDevTime.temperature.energyDecay = 0.0
json.traits.base.eggDevTime.temperature.temperatureOptimal = 0.0
json.traits.base.eggDevTime.temperature.temperatureRef = 0.0

## base - energy_tank - Hoffman et al. 2000 J. of Science of Food and Agriculture. @@BVazquez, @@BVila - canviar a tots dinos
json.traits.base.energy_tank.limits.max = 0.0  # Maximum capacity for the "energy tank"
json.traits.base.energy_tank.limits.min = 0.0  # Minimum capacity
json.traits.base.energy_tank.ranges.max = 0.1388  # Upper boundary for sampling
json.traits.base.energy_tank.ranges.min = 0.1052  # Lower boundary for sampling
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
json.traits.base.growth.limits.max = 0.0  # Maximum growth rate limit (0 means no constraint or zero?)
json.traits.base.growth.limits.min = 0.0  # Minimum growth rate limit
json.traits.base.growth.ranges.max = as.vector(growthK)  # Upper boundary for sampling growth rate
json.traits.base.growth.ranges.min = as.vector(growthK)  # Lower boundary for sampling growth rate
json.traits.base.growth.restrictValue = 0.01  # Increment step

json.traits.base.growth.temperature.dependent = FALSE
json.traits.base.growth.temperature.activationEnergy = 0.0
json.traits.base.growth.temperature.energyDecay = 0.0
json.traits.base.growth.temperature.temperatureOptimal = 0.0
json.traits.base.growth.temperature.temperatureRef = 0.0

## base - coeffMassForScopeRadius
json.traits.base.coeffMassForScopeRadius.limits.max = 0.0  
json.traits.base.coeffMassForScopeRadius.limits.min = 0.0 
json.traits.base.coeffMassForScopeRadius.ranges.max = 0.2 
json.traits.base.coeffMassForScopeRadius.ranges.min = 0.2
json.traits.base.coeffMassForScopeRadius.restrictValue = 0.01  

json.traits.base.coeffMassForScopeRadius.temperature.dependent = FALSE
json.traits.base.coeffMassForScopeRadius.temperature.activationEnergy = 0.0
json.traits.base.coeffMassForScopeRadius.temperature.energyDecay = 0.0
json.traits.base.coeffMassForScopeRadius.temperature.temperatureOptimal = 0.0
json.traits.base.coeffMassForScopeRadius.temperature.temperatureRef = 0.0

## base - scaleMassForScopeRadius
json.traits.base.scaleMassForScopeRadius.limits.max = 0.0  
json.traits.base.scaleMassForScopeRadius.limits.min = 0.0 
json.traits.base.scaleMassForScopeRadius.ranges.max = 0.0 
json.traits.base.scaleMassForScopeRadius.ranges.min = 0.0
json.traits.base.scaleMassForScopeRadius.restrictValue = 0.0  

json.traits.base.scaleMassForScopeRadius.temperature.dependent = FALSE
json.traits.base.scaleMassForScopeRadius.temperature.activationEnergy = 0.0
json.traits.base.scaleMassForScopeRadius.temperature.energyDecay = 0.0
json.traits.base.scaleMassForScopeRadius.temperature.temperatureOptimal = 0.0
json.traits.base.scaleMassForScopeRadius.temperature.temperatureRef = 0.0

## base - coeffMassForInteractionRadius
json.traits.base.coeffMassForInteractionRadius.limits.max = 0.0  
json.traits.base.coeffMassForInteractionRadius.limits.min = 0.0 
json.traits.base.coeffMassForInteractionRadius.ranges.max = 0.2 
json.traits.base.coeffMassForInteractionRadius.ranges.min = 0.2
json.traits.base.coeffMassForInteractionRadius.restrictValue = 0.01  

json.traits.base.coeffMassForInteractionRadius.temperature.dependent = FALSE
json.traits.base.coeffMassForInteractionRadius.temperature.activationEnergy = 0.0
json.traits.base.coeffMassForInteractionRadius.temperature.energyDecay = 0.0
json.traits.base.coeffMassForInteractionRadius.temperature.temperatureOptimal = 0.0
json.traits.base.coeffMassForInteractionRadius.temperature.temperatureRef = 0.0

## base - scaleMassForInteractionRadius
json.traits.base.scaleMassForInteractionRadius.limits.max = 0.0  
json.traits.base.scaleMassForInteractionRadius.limits.min = 0.0 
json.traits.base.scaleMassForInteractionRadius.ranges.max = 0.0 
json.traits.base.scaleMassForInteractionRadius.ranges.min = 0.0
json.traits.base.scaleMassForInteractionRadius.restrictValue = 0.0  

json.traits.base.scaleMassForInteractionRadius.temperature.dependent = FALSE
json.traits.base.scaleMassForInteractionRadius.temperature.activationEnergy = 0.0
json.traits.base.scaleMassForInteractionRadius.temperature.energyDecay = 0.0
json.traits.base.scaleMassForInteractionRadius.temperature.temperatureOptimal = 0.0
json.traits.base.scaleMassForInteractionRadius.temperature.temperatureRef = 0.0

## base - coeffMassForSearchRadius
json.traits.base.coeffMassForSearchRadius.limits.max = 0.0  
json.traits.base.coeffMassForSearchRadius.limits.min = 0.0 
json.traits.base.coeffMassForSearchRadius.ranges.max = 0.05
json.traits.base.coeffMassForSearchRadius.ranges.min = 0.05
json.traits.base.coeffMassForSearchRadius.restrictValue = 0.01  

json.traits.base.coeffMassForSearchRadius.temperature.dependent = FALSE
json.traits.base.coeffMassForSearchRadius.temperature.activationEnergy = 0.0
json.traits.base.coeffMassForSearchRadius.temperature.energyDecay = 0.0
json.traits.base.coeffMassForSearchRadius.temperature.temperatureOptimal = 0.0
json.traits.base.coeffMassForSearchRadius.temperature.temperatureRef = 0.0

## base - scaleMassForSearchRadius
json.traits.base.scaleMassForSearchRadius.limits.max = 0.0  
json.traits.base.scaleMassForSearchRadius.limits.min = 0.0 
json.traits.base.scaleMassForSearchRadius.ranges.max = 0.0 
json.traits.base.scaleMassForSearchRadius.ranges.min = 0.0
json.traits.base.scaleMassForSearchRadius.restrictValue = 0.0  

json.traits.base.scaleMassForSearchRadius.temperature.dependent = FALSE
json.traits.base.scaleMassForSearchRadius.temperature.activationEnergy = 0.0
json.traits.base.scaleMassForSearchRadius.temperature.energyDecay = 0.0
json.traits.base.scaleMassForSearchRadius.temperature.temperatureOptimal = 0.0
json.traits.base.scaleMassForSearchRadius.temperature.temperatureRef = 0.0

## base - coeffMassForSpeed
json.traits.base.coeffMassForSpeed.limits.max = 0.0  
json.traits.base.coeffMassForSpeed.limits.min = 0.0 
json.traits.base.coeffMassForSpeed.ranges.max = 26.5 
json.traits.base.coeffMassForSpeed.ranges.min = 24.5
json.traits.base.coeffMassForSpeed.restrictValue = 0.0  

json.traits.base.coeffMassForSpeed.temperature.dependent = FALSE
json.traits.base.coeffMassForSpeed.temperature.activationEnergy = 0.0
json.traits.base.coeffMassForSpeed.temperature.energyDecay = 0.0
json.traits.base.coeffMassForSpeed.temperature.temperatureOptimal = 0.0
json.traits.base.coeffMassForSpeed.temperature.temperatureRef = 0.0

## base - scaleMassForSpeed
json.traits.base.scaleMassForSpeed.limits.max = 0.0  
json.traits.base.scaleMassForSpeed.limits.min = 0.0 
json.traits.base.scaleMassForSpeed.ranges.max = 0.26 
json.traits.base.scaleMassForSpeed.ranges.min = 0.26
json.traits.base.scaleMassForSpeed.restrictValue = 0.0  

json.traits.base.scaleMassForSpeed.temperature.dependent = FALSE
json.traits.base.scaleMassForSpeed.temperature.activationEnergy = 0.0
json.traits.base.scaleMassForSpeed.temperature.energyDecay = 0.0
json.traits.base.scaleMassForSpeed.temperature.temperatureOptimal = 0.0
json.traits.base.scaleMassForSpeed.temperature.temperatureRef = 0.0

## base - lengthAtMaturation
json.traits.base.lengthAtMaturation.limits.max = 0.0  # Maximum length limit (0 could mean no constraint)
json.traits.base.lengthAtMaturation.limits.min = 0.0  # Minimum length limit
json.traits.base.lengthAtMaturation.ranges.max = femaleMaxLength  # Upper boundary for sampling length
json.traits.base.lengthAtMaturation.ranges.min = femaleMaxLength  # Lower boundary for sampling length
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
json.traits.base.memoryDepth.ranges.max = 6.0 
json.traits.base.memoryDepth.ranges.min = 5.0
json.traits.base.memoryDepth.restrictValue = 0.01

json.traits.base.memoryDepth.temperature.dependent = FALSE
json.traits.base.memoryDepth.temperature.activationEnergy = 0.0
json.traits.base.memoryDepth.temperature.energyDecay = 0.0
json.traits.base.memoryDepth.temperature.temperatureOptimal = 0.0
json.traits.base.memoryDepth.temperature.temperatureRef = 0.0

## base - met_rate
json.traits.base.met_rate.limits.max = 0.0 
json.traits.base.met_rate.limits.min = 0.0
json.traits.base.met_rate.ranges.max = 0.82
json.traits.base.met_rate.ranges.min = 0.82
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
json.traits.base.shock_resistance.limits.max = 0.0  # Maximum limit for shock resistance
json.traits.base.shock_resistance.limits.min = 0.0   # Minimum limit for shock resistance
json.traits.base.shock_resistance.ranges.max = 410.0 # Upper boundary for sampling shock resistance
json.traits.base.shock_resistance.ranges.min = 410.0 # Lower boundary for sampling shock resistance
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
json.traits.probabilityDensityFunction.muForPDF.ranges.max = 8.65 # aquatic:1.44
json.traits.probabilityDensityFunction.muForPDF.ranges.min = 8.65 # aquatic:1.44
json.traits.probabilityDensityFunction.muForPDF.restrictValue = 0.0

json.traits.probabilityDensityFunction.muForPDF.temperature.dependent = FALSE
json.traits.probabilityDensityFunction.muForPDF.temperature.activationEnergy = 0.0
json.traits.probabilityDensityFunction.muForPDF.temperature.energyDecay = 0.0
json.traits.probabilityDensityFunction.muForPDF.temperature.temperatureOptimal = 0.0
json.traits.probabilityDensityFunction.muForPDF.temperature.temperatureRef = 0.0

## probabilityDensityFunction - sigmaForPDF
json.traits.probabilityDensityFunction.sigmaForPDF.limits.max = 0.0 
json.traits.probabilityDensityFunction.sigmaForPDF.limits.min = 0.0 
json.traits.probabilityDensityFunction.sigmaForPDF.ranges.max = 2.54 #aquatic: 2.34
json.traits.probabilityDensityFunction.sigmaForPDF.ranges.min = 2.54 #aquatic: 2.34
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
json.traits.cellValue.cellEvaluationBiomass.restrictValue = 0.01

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
json.traits.cellValue.cellEvaluationProConspecific.restrictValue = 0.01

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
json.traits.cellValue.conspecificWeight.restrictValue = 0.01

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
		trait = "base$eggDevTime",
		element = "value"
	),
	list(
		trait = "base$energy_tank",
		element = "value"
	),
	list(
		trait = "base$dummy",
		element = "value"
	),
	list(
		trait = "base$assim",
		element = "value"
	),
	list(
		trait = "base$coeffMassForSpeed",
		element = "value"
	),
	list(
		trait = "base$memoryDepth",
		element = "value"
	)
)

# =====================
# GROWTH MODULE PARAMETERS
# =====================

json.growthModule.assignedForMolt = 0.99  # Fraction of resources allocated to molting
json.growthModule.betaScaleTank = 1.0  # Scaling factor for the energy tank in the growth module
json.growthModule.capitalBreeding.enabled = FALSE  # If TRUE, uses stored resources for reproduction
json.growthModule.capitalBreeding.capitalBreedingParams.numberOfCapitalBreeds = 0
json.growthModule.capitalBreeding.capitalBreedingParams.timeOfReproEventDuringCapitalBreeding = 0
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

# Marshall, C. R., et al. (2021). “Absolute abundance and preservation rate of Tyrannosaurus rex.” Science 372(6539): 284–287. DOI: 10.1126/science.abc8300. PubMed
# Science article

#postjuvenile refers to 15 years and older = 5565 in days

# Griebeler, E. M. (2023). “Vital statistics, absolute abundance and preservation rate of Tyrannosaurus rex.” Palaeontology 66. DOI: 10.1111/pala.12648. This is particularly useful because Table 2 gives all of the alternative density estimates above. Wiley Online Library
# Palaeontology article
# Meiri, S. (2022). “Population sizes of Tyrannosaurus rex cannot be precisely estimated.” Frontiers of Biogeography 14(2), e53781. DOI: 10.21425/F5FBG53781.

fieldDensity = 0.009 ##indivs/km2 - postjuveniles
totalNumberOfIndividuals = 1000

##IMPORTANT RELEVANT NOTE: HERE WE USE THE POSTJUVENILE AGE OF 15 YEARS older
##TO ESTIMATE ABUNDANCES. HOWEVER, OUR AGE AT MATURATION IS MUCH LATER because
##WE ARE FINALLY ASSUMING "MILD" INDETERMINATE GROWTH FOR DINOSAURS, SUCH AS THAT OF HUMANS

result <- calculateIndividualsPerInstar(
    totalNumberOfIndividuals = 1000,
    field_density = fieldDensity,
     ageVector = ageVector,
    coefficientForMassA = coefficientForMassA,
    coefficientForMassAforMature = coefficientForMassAforMature,
    scaleForMassB = scaleForMassB,
    scaleForMassBforMature = scaleForMassBforMature,
    conversionToWetMass = conversionToWetMass,
    growthModel = fitted[[as.character(tempFromLab)]]$best_model,
	ageDensityDatum = "postjuvenile",
    postjuvenile_age = 15
)

json.individualsPerInstar <- result$individualsPerInstar
# json.individualsPerInstar
# 387 228 135  81  50  31  20  13   9   6   5   4   3   2   2   2   2   2   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1   1

# result$densitiesPerInstar                 # individuals / km^2 by age class

# totalDensity <- result$totalDensity       # individuals / km^2
# totalArea    <- result$totalArea          # km^2

# totalDensity
# totalArea

# nCellsSideEcosystem=round(sqrt(totalArea))
# nCellsSideEcosystem  ##1 cell = 1km2
# 53

json.requiredRepresentAllPossibleAlleles = FALSE

json.maleMaxReproductionEvents = 8   # Maximum number of reproduction events for a male
json.maleMobility = 1  # Mobility factor for males (compared to females or baseline)
json.maleReproductionFactor = 0.25  # Fraction of how many mates or eggs a male can fertilize compared to baseline
json.minRelativeHumidityThreshold = rep(10, times = length(json.individualsPerInstar))  # Minimum humidity needed for survival or normal behavior

# =====================
# FINAL PARAMETERS
# =====================

json.name = "Tyrannosaurus_rex"  # Name of the species or simulation run
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
json.probabilityDeathFromBackground = rep(0.0, times = length(json.individualsPerInstar))	  # Baseline mortality rate from non-predation causes
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

json.sexualType = "diploid"  # Reproduction type (e.g., haplodiploid, diploid, etc.)
json.statisticsIndividualsPerInstar = 1000  # Number of individuals used in statistics per instar
json.tempFromLab = tempFromLab  # Reference temperature from lab conditions
