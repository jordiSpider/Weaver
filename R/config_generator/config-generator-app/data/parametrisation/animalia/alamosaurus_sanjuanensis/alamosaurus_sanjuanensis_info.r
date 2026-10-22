# @@@@ species: alamosaurus_sanjuanensis

if (!"app" %in% ls()) {
	rm(list=ls(all=TRUE))

	script_path <- dirname(sys.frame(1)$ofile)
}

parametrisation_path <- sub("(.*/parametrisation)/.*", "\\1", script_path)


library(ggplot2)
library(dplyr)

source(file.path(parametrisation_path, "utilities", "codes", "functions", "growth_curve_modelfitting", "gcmGAT.r"))


set.seed(123)

# Option to avoid plotting 
# options(allow_plotting = FALSE)

# Start reading ID_ALASAN_tempFromLab data frame

#@@BVazquez - documentar biblio sobre temperatura Cretaci
ID_ALASAN_tempFromLab<-data.frame(
			Variable = "tempFromLab",
			DOI1 = "10.5194/cp-12-1181-2016",
			Category1 = "Article",
			DataTreatment = "taken from supplementary simulations and based on references in Riera et al., 2013; Amiot et al., 2004"
)
# End of ID_ALASAN_tempFromLab data frame
tempFromLab<-21.4 ##data for calculations here
tempFromLab #@tempFromLab=21.4  ##data for Json for simulation

# Start reading ID_ALASAN_eggClutchFromEquation data frame	
ID_ALASAN_eggClutchFromEquation<-data.frame(
			Variable = "eggClutchFromEquation", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Not available"  ##@David - is not common knowledge is equation not available
)
# End reading ID_ALASAN_eggClutchFromEquation data frame

eggClutchFromEquation<-FALSE
eggClutchFromEquation #@eggClutchFromEquation=FALSE

# Start reading ID_ALASAN_eggMassFromEquation data frame	
ID_ALASAN_eggMassFromEquation<-data.frame(
			Variable = "eggMassFromEquation", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Not available"
)
# End reading ID_ALASAN_eggMassFromEquation data frame

eggMassFromEquation<-"false"
eggMassFromEquation #@eggMassFromEquation="false"

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
			DOI1 = "10.1007/s12542-009-0017-5", #@@BVazquez 
			Category1 = "Article",
			DataTreatment = "We performed a ratio between wetMass and dryMass based on the BM calculations for Hungarosaurus"
)
 # End of ID_ALASAN_conversionToWetMass data frame

conversionToWetMass<-3.3333
conversionToWetMass #@conversionToWetMass=3.3333

# Start reading ID_ALASAN_eggsPerBatch data frame
ID_ALASAN_eggsPerBatch<-data.frame(
			Variable = "eggsPerBatch",
			DOI1 = "10.1371/journal.pone.0072862",
			DOI2 = "10.1666/13028",
			ISSN = NA,#@@BVazquez
			Category1 = "Article",
			Category2 = "Article",
			DataTreatment = "We employ alometric equations in both studies, based on the egg mass derived from Werner & Griebeler 2013 and the EggClutchMass from Ruxton et al. 2014"
)
# End of ID_ALASAN_eggsPerBatch data frame

eggsPerBatch<-22
eggsPerBatch #@eggsPerBatch=22

# Start reading ID_ALASAN_coefficientForMassA data frame
ID_ALASAN_coefficientForMassA<-data.frame(
			Variable = "coefficientForMassA", 
			DOI1 = "10.1186/1741-7007-10-60",
			Category1 = "Article",
			DataTreatment = "Data and glm are based on BL and dryMass estimations from Campione and Evans 2012, and since the data is already derived from titanosaur BL and Drymass, conversion to wetMass is not needed"
)
# End of ID_ALASAN_coefficientForMassA data frame

ML_titano<-read.table(file.path(script_path, "Titano_M_L.txt"), h=T)

ML_titano$dryMass=ML_titano$Wet_M_kg*(1/conversionToWetMass) #we need dryMass for the model in order to obtain coefficientForMassA and scaleForMassB

##Fit the model that correlates titanosaur BL (used in Weaver) with dryMass (obtained from literature) and print the resulting coefficients as in dryMass=ABody_Length^b
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
			DOI1 = "10.1186/1741-7007-10-60",
			Category1 = "Article",
			DataTreatment = "Data and glm are based on BL and dryMass estimations from Campione and Evans 2012, and since the data is already derived from titanosaur BL and Drymass, conversion to wetMass is not needed"
)
# End of ID_ALASAN_scaleForMassB data frame

scaleForMassB <- as.vector(coef(glm.ab)[2])

scaleForMassB #@scaleForMassB=2.063347

# Start reading ID_ALASAN_coefficientForMassAforMature data frame
ID_ALASAN_coefficientForMassAforMature<-data.frame(
			Variable = "coefficientForMassAforMature", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "No metamorphosis. Mature individuals take the value of juveniles"
)
# End of ID_ALASAN_coefficientForMassAforMature data frame

coefficientForMassAforMature <- coefficientForMassA
coefficientForMassAforMature #@coefficientForMassAforMature=30816900

# Start reading ID_ALASAN_scaleForMassBforMature data frame
ID_ALASAN_scaleForMassBforMature<-data.frame(
			Variable = "scaleForMassBforMature", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "No metamorphosis. Mature individuals take the value of juveniles"
)
# End of ID_ALASAN_scaleForMassBforMature data frame

scaleForMassBforMature = scaleForMassB
scaleForMassBforMature #@scaleForMassBforMature=2.063347

# Start reading ID_ALASAN_pupaPeriodTime  data frame
ID_ALASAN_pupaPeriodTime <-data.frame(
			Variable = "pupaPeriodTime ",
			DOI1 = NA,
			Category1 = NA, 
			DataTreatment = "Not present in dinosaurs, therefore it is assumed as 1day."
)
# End of ID_ALASAN_pupaPeriodTime  data frame

# Start implementing ID_ALASAN_ageVector data frame											   
pupaPeriodTime <-1.00
pupaPeriodTime  #@pupaPeriodTime =1.00

##200 years lifespan according to Lehman and Woodward, but estimates differ according to authors
##as low as 45 years - thus, @@BVazquez, @@Jordi - simulations with different lifespan assumptions
##Now, dinosaurs have to be simulated as growing indeterminally!
longevity = 200*371   ###This is special because we have only a piece of the growth curve
longevity

##Here we discard Lehman and Woodward's "von Bertalanffy" model for mass and we build a conventional
##von Bertalanffy model from scratch, directly based on length not on mass
##predict mass for an individual with the Lehman and Woodard model. 
# M=32663*(1-exp(-b*t))^3 ##the minus "-" before the "b" sign was not included in the publication - mass in Kg
# M[1]  ##this provides a mass for the newlyborn of zero, see solution below

length_out=100
t=seq(0,longevity,length.out=length_out)

##orginal data from real masses estimated in Lehman and Woodward 2008 obtained with Webplotdigitizer
early_stages<-read.table(file.path(script_path, "alamo observed v2.txt"),h=TRUE)

early_stages$age<-early_stages$age*371 # In days
early_stages$mass_Kg=early_stages$mass_Kg*(1/conversionToWetMass) #Convert wetMass from Lehman and Woodward into dryMass as in Campione and Evans
early_stages$length<-(early_stages$mass_Kg/coefficientForMassA)^(1/scaleForMassB) # through the coefficients derived from the glm based on Campione and Evans, convert BM to BL 

# early_stages$age_years<-early_stages$age/371 # Back to years
# glm1<-glm(mass_Kg~log(age_years),data=early_stages,gaussian(link="log")) #Model based on lehman and woodward that correlates dryMass and Age (years)
# exp(coef(glm1)[2])
# plot(early_stages$age_years,exp(predict(glm1)))

##The intercept will be the length of the newborn
lm1<-lm(length~age,data=early_stages)
lm1
# Coefficients:
# (Intercept)          age  
  # 3.198e-04    1.989e-06  

summary(lm1)$r.squared
predict(lm1,data.frame(age=0))

##range of egg diameter for Titanosaurids 15-25 cm. ref. Klein et al. 2011. ##Vila et al. 2010 PlosONe range: 16-18 cm.
##assuming a 2-fold for animal total length gives 30-50 cm. Thus we are in a lower end 32 cm.
##@@BVazquez @@Jordi: run simulations assuming eggs and lengths in this range 30-50cm for lengthAtBirth.
##Note: Werner & Griebeler 2013 equation  0.09*femaleWetMass^0.311 gives 32 Kg for egg mass, way off
##Note: Erickson & Tumanova 2000 - 11% of female body length - way off 2.2 m for the newborn
lengthAtBirthProvisional<-as.vector(predict(lm1,data.frame(age=0)))
lengthAtBirthProvisional #=32cm
eggDryMassProvisional=coefficientForMassA*lengthAtBirthProvisional^scaleForMassB #=1.9kg; eggWetMassProvisional=(eggDryMassProvisional*conversionToWetMass)=6.3kg

ageVector=seq(371,t[length(t)],371) #ageOfFirstMaturation is already in days

####FITTING A VB GROWTH CURVE TO BODY LENGTH
###Now we estimate t0 for VB by using inverse regression in a linear model 
plot(length~age,data=early_stages,xlim=c(-190,2),ylim=c(0,0.001))
abline(lm(length~age,data=early_stages),lwd=3,lty=1)
abline(h=3.198e-04,lwd=3,col="red",lty=2)

##Rearranging the linear equation we obtain the wanted value of t0 given a length of 0
a=as.vector(coef(lm1)[1]) #intercept
b=as.vector(coef(lm1)[2]) #slope ~ growth rate ~ k
b# 1.989456e-06
y=0 #at t0, BL=0
x=(y-a)/b #rearranged linear equation
x
-160.7533
t0=x
##This equality (t0=x) is not true when going from a linear model to VB, but it is a pragmatic approximation.

# k=b #this b is from a linear model, not applicable to VB ##we assume this to be maximum growth rate given the real data at exponential growth phase in mass
# b=0.070 ##span in publication 0.065-0.75. So it can be used for simulations, it is used in the paper for their VB equation: W=32663(1-e^bt)^3. this is a VB k determined by the authors based on their results and their incorrectly constructed curve.
Linf = ((32663*(1/conversionToWetMass))/coefficientForMassA)^(1/scaleForMassB) #Linf (asymptotic body length) calculation: From the asymptotic body wetMass, it is converted to dryMass, then dryMass->bodyLength using the allometric coefficients (coefficientForMassA,scaleForMassB)
# L = Linf * (1-exp(-k*(t-t0)))
# plot(t,L) 


## wetMass HAS NOT BEEN CALCULATED UP UNTIL NOW, THEREFORE THIS SECTION HAS BEEN INVALIDATED AS WE ALREADY HAVE dryMass:
# wetMassVector=M[1:length(ageVector)] #M has been determined before, it represents the wetMass of the individual
# dryM=(1/conversionToWetMass)*wetMassVector
# dryMass=ML_titano$dryMass
# dryMassVector=dryMass[1:length(ageVector)]
# length=(dryMassVector/coefficientForMassA)^(1/scaleForMassB)
### k optimisation - genetic algorithms to find the VB model that best fits the observed data
###Here we run different models with different k values from a VB as to find the k value
###that minimizes the distance between observed and fitted data for the known data but
##Thus, we use trial an error AKA, optimization algorithm, to calculate k (here we do it visually to speed it up, but needs to be done)
# k=0.000124 #This initial value was calulated by eye

	# Install and load required package
	if (!require(GA)) {
	  install.packages("GA")
	  library(GA)
	}

	## Define the fitness function: uses genetic algorithms for k optimisation, obtaining the value of k (growth rate) that better adjusts a VB growth curve to the real data of observed bodyLength (in early_stages). In this manner we end up with k, Linf, and t0 for our growth curve. 
	fitness_function <- function(ks_value) {
	  # Extract the single ks value from the vector
	  ks <- ks_value[1]
	  
	  # tryCatch(
	  # {
		# VB equation
		L <- Linf * (1 - exp(-ks * (t - t0)))
		
		# Prepare length data
		length_data <- c(lengthAtBirthProvisional, L)
		# age_data <- c(0, seq(1, ageVector[length(ageVector)], length.out = length_out))
		# length_truncated <- length_data[1:(length(age_data) - 1)]
		# length_new <- c(lengthAtBirthProvisional, length_truncated)
		# length_final <- length_new
		
		# # Create data frame
		# temp_vec <- rep(tempFromLab, length(age_data))
		# data_long <- data.frame(age = age_data, length = length_final, temp = temp_vec)
		
		# alamosaurus_data_frame <- data.frame(age = data_long$age, length = data_long$length)
		# alamosaurus_data <- list()
		# alamosaurus_data[[as.character(tempFromLab)]] <- alamosaurus_data_frame
		
		# # Fit model
		# fitted <- gcmGAT(
		  # alamosaurus_data, 
		  # x = "age",
		  # y = "length", 
		  # lengthAtBirth = lengthAtBirthProvisional
		# )
		
		# # Get predictions
		# prediction <- predict(
		  # fitted[[as.character(tempFromLab)]]$best_model,
		  # newdata = data.frame(age = early_stages$age)
		# )
		
		prediction=L <- Linf * (1 - exp(-ks * (early_stages$age - t0)))
		
		# Calculate MSE (we return negative MSE because ga maximizes by default)
		mse <- mean((prediction - early_stages$length)^2, na.rm = TRUE)
		
		# Return negative MSE since we want to minimize MSE
		return(-mse)
		
	  # }#, error = function(e) {
		# # Return a very poor fitness if there's an error
		# return(-1e10)
	  # }
	  # )
	}

	## Define bounds for ks
	# lower_bound <- k - 0.50 * k
	# upper_bound <- k + 0.50 * k
	
	lower_bound <- 0.00001
	upper_bound <- 0.001


	## Run genetic algorithm (apply the ga())
	ga_results <- ga(
	  type = "real-valued",
	  fitness = fitness_function,
	  pmutation = 0.5,
	  lower = lower_bound,
	  upper = upper_bound,
	  popSize = 1000,        # Population size
	  maxiter = 400,        # Maximum number of generations
	  run = 300,            # Stop if fitness doesn't improve for 25 generations
	  parallel = FALSE,    # Set to TRUE if you want parallel processing
	  monitor = TRUE,      # Show progress
	  seed = 123           # For reproducibility
	)

	# Get the best solution
	best_ks <- ga_results@solution[1]
	best_mse <- -ga_results@fitnessValue  # Convert back to positive MSE

	# Print results
	cat("Genetic Algorithm Results:\n")
	cat("Best ks value:", best_ks, "\n")
	cat("Best MSE:", best_mse, "\n")
	cat("Fitness function evaluations:", ga_results@iter, "\n")

	# Plot convergence
	plot(ga_results)

	# Summary of results
	summary(ga_results)

	L = Linf * (1-exp(-best_ks*(t-t0))) #VB equation
	plot(t,L)
	points(early_stages$age,early_stages$length,col="red",pch=16) #datapoints from lehman and woodward plotted

##And this is the final value
	best_ks
0.0001215718
# 0.0001215729
#0.0001215718
k=best_ks

###Determine ageOfFirstMaturation
##Specially long series for calculating ageOfFirstMaturation, i.e. when growth starts to plateau
t_series=seq(0,longevity,length.out=longevity)#daily time-series
L = Linf * (1-exp(-k*(t_series-t0))) #VB equation
plot(t_series,L)
points(early_stages$age,early_stages$length,col="red",pch=16) #datapoints from lehman and woodward plotted

##Since growth is actually determinate (but some growth occurs after maturation - need to simulate as indeterminate = "true"), we need to decide the ageOfFirstMaturation, when growth rate (in BL) slows down as the individual becomes sexually mature and begins to invest more on reproduction
howManyDecimalPlaces = 6 ##this value largely determines ageOfFirstMaturation, when rounding to this value of decimals is equal between L[i] and (L[i]-howFarBackInDays).
howFarBackInDays = 5 ##5 means that we compare one day with five days before
i=howFarBackInDays+1
while(round(L[i],howManyDecimalPlaces) != round(L[i-howFarBackInDays],howManyDecimalPlaces)){
	i=i+1
}
#Record length and age when the previous equality is met:
maxLength=L[i+1-howFarBackInDays]
ageOfFirstMaturation = t_series[i+1-howFarBackInDays] + pupaPeriodTime
maxLength
0.0185367
ageOfFirstMaturation	
20477.28					

##if round at 8 digits, ageOfFirstMaturation is:
# 58404.79		
##if round at 7 digits, ageOfFirstMaturation is:
# 4791.65359065359
#maxLength*1000=BL at ageOfFirstMaturation=12.2305m or 0.0122305km

abline(v=ageOfFirstMaturation,lwd=3,col="red",lty=2)  ##not matching ageVector
# abline(v=48044,lwd=3,col="red")

## ageOfFirstMaturation and longevity is now expressed in days already, so this section is no longer useful:
# ageOfFirstMaturation = ageOfFirstMaturation*371 ##in days
# longevity = longevity*371 ##in days

##ageOfFirstMaturation is changed to closely match a day within the ageVector (exact year timing)
ageOfFirstMaturation=ageVector[which((abs(ageOfFirstMaturation-ageVector))==min(abs(ageOfFirstMaturation-ageVector)))]
ageOfFirstMaturation
20405	 

abline(v=ageOfFirstMaturation,lwd=3,col="red") ##matching ageVector


#@@David: cómo se pone en R y para Json?

##Print ageVector so it is readable when generating the .json:
paste0(ageVector,collapse=",")
# "371,742,1113,1484,1855,2226,2597,2968,3339,3710,4081,4452,4823,5194,5565,5936,6307,6678,7049,7420,7791,8162,8533,8904,9275,9646,10017,10388,10759,11130,11501,11872,12243,12614,12985,13356,13727,14098,14469,14840,15211,15582,15953,16324,16695,17066,17437,17808,18179,18550,18921,19292,19663,20034,20405,20776,21147,21518,21889,22260,22631,23002,23373,23744,24115,24486,24857,25228,25599,25970,26341,26712,27083,27454,27825,28196,28567,28938,29309,29680,30051,30422,30793,31164,31535,31906,32277,32648,33019,33390,33761,34132,34503,34874,35245,35616,35987,36358,36729,37100,37471,37842,38213,38584,38955,39326,39697,40068,40439,40810,41181,41552,41923,42294,42665,43036,43407,43778,44149,44520,44891,45262,45633,46004,46375,46746,47117,47488,47859,48230,48601,48972,49343,49714,50085,50456,50827,51198,51569,51940,52311,52682,53053,53424,53795,54166,54537,54908,55279,55650,56021,56392,56763,57134,57505,57876,58247,58618,58989,59360,59731,60102,60473,60844,61215,61586,61957,62328,62699,63070,63441,63812,64183,64554,64925,65296,65667,66038,66409,66780,67151,67522,67893,68264,68635,69006,69377,69748,70119,70490,70861,71232,71603,71974,72345,72716,73087,73458,73829,74200"
 

# Start reading ID_ALASAN_ageVector data frame
ID_ALASAN_ageVector<-data.frame(
			Variable = "ageVector",
			DOI1 = "10.1666/0094-8373(2008)034[0264:MGRFSD]2.0.CO;2", #@BVazquez
			Category1 = "Article",
			DataTreatment = "Derived from the growth curves for Alamosaurus and the calculated longevity presented in lehman and Woodward."
)		
# End of ID_ALASAN_ageVector data frame

ageVector #@ageVector="371,742,1113,1484,1855,2226,2597,2968,3339,3710,4081,4452,4823,5194,5565,5936,6307,6678,7049,7420,7791,8162,8533,8904,9275,9646,10017,10388,10759,11130,11501,11872,12243,12614,12985,13356,13727,14098,14469,14840,15211,15582,15953,16324,16695,17066,17437,17808,18179,18550,18921,19292,19663,20034,20405,20776,21147,21518,21889,22260,22631,23002,23373,23744,24115,24486,24857,25228,25599,25970,26341,26712,27083,27454,27825,28196,28567,28938,29309,29680,30051,30422,30793,31164,31535,31906,32277,32648,33019,33390,33761,34132,34503,34874,35245,35616,35987,36358,36729,37100,37471,37842,38213,38584,38955,39326,39697,40068,40439,40810,41181,41552,41923,42294,42665,43036,43407,43778,44149,44520,44891,45262,45633,46004,46375,46746,47117,47488,47859,48230,48601,48972,49343,49714,50085,50456,50827,51198,51569,51940,52311,52682,53053,53424,53795,54166,54537,54908,55279,55650,56021,56392,56763,57134,57505,57876,58247,58618,58989,59360,59731,60102,60473,60844,61215,61586,61957,62328,62699,63070,63441,63812,64183,64554,64925,65296,65667,66038,66409,66780,67151,67522,67893,68264,68635,69006,69377,69748,70119,70490,70861,71232,71603,71974,72345,72716,73087,73458,73829,74200"



L = Linf * (1-exp(-best_ks*(t-t0))) #VB equation

age=t
length=L

age=c(0,t)
temp<-rep(tempFromLab, length(age))
alamosaurus_data_frame<- data.frame(age=age, length=c(length[1],length))
alamosaurus_data <- list()

alamosaurus_data[[as.character(tempFromLab)]] = alamosaurus_data_frame

fitted <- gcmGAT(
			alamosaurus_data, 
			x="age", 
			y="length", 
			use_app = TRUE)


best_model <- fitted[[as.character(tempFromLab)]]$best_model







# Start reading ID_ALASAN_instarFirstReproduction data frame
ID_ALASAN_instarFirstReproduction<-data.frame(
			Variable = "instarFirstReproduction",
			DOI1 = "10.1666/0094-8373(2008)034[0264:MGRFSD]2.0.CO;2",
			Category1 = "Article",
			DataTreatment = "Derived from the model constructed from scratch in this script, together with the growth curves for Alamosaurus and the calculated longevity presented in the same study."
)
# End of ID_ALASAN_instarFirstReproduction data frame

##@@simul (see also @@coDependency) ageOfFirstMaturation if changed other parameters in the optimizer.r must change
##(@BVazquez, @BVila justification to be written) and this value and see how the different
instarFirstReproduction<-which(ageVector == ageOfFirstMaturation)  ##because ageVector starts at date of Instar II
instarFirstReproduction #@instarFirstReproduction=55
55  

####@@coDependency - @@Mario - instarFirstReproduction when it changes, other parameters need to be recalculated such as
####instarFirstReproduction requires recalculation of: femaleMaxReproductionEvents, longevitySinceMaturation

# Start reading ID_ALASAN_femaleMaxReproductionEvents data frame
ID_ALASAN_femaleMaxReproductionEvents<-data.frame(
			Variable = "femaleMaxReproductionEvents", 
			DOI1 = NA,
			Category1 = NA, 
			DataTreatment = "We assume 1 ReproductionEvent/Year since ageOfFirstMaturation"
)
# End of ID_ALASAN_	 data frame

totalReproductiveTime=longevity-ageOfFirstMaturation #already in days
femaleMaxReproductionEvents<-round(totalReproductiveTime/371)
femaleMaxReproductionEvents #@femaleMaxReproductionEvents=145
145


# SINCE WE KEPT USING BL, dryMass WAS NEVER CONVERTED INTO wetMass AT THE BEGINNING OF THE SCRIPT, therefore section below is invalidated:
##Important: convert the Mass to dryMass before using the ab coefficients
# dryMass=M*(1/conversionToWetMass)
# L=(dryMass/coefficientForMassA)^(1/scaleForMassB)


# plot(early_stages$age,early_stages$length,xlim=c(0,max(early_stages$age)),
# ylim=c(0,max(early_stages$length)))

# abline(lm(length~age,data=early_stages))
# abline(v=0,lwd=3)


# Start reading ID_ALASAN_curveType data frame
ID_ALASAN_curveType<-data.frame(
			Variable = "curveType",
			DOI1 = "10.1666/0094-8373(2008)034[0264:MGRFSD]2.0.CO;2",
			Category1 = "Article", 
			DataTreatment = "In lehman and Woodward, growth curves are presented as a VB growth type for Alamosaurus even though for the original study the curve is logarithmic. In any case, we performed a model autofit adjustment using AIC criterion"
)
# End of ID_ALASAN_curveType data frame

curveType<-fitted[[as.character(tempFromLab)]]$best_model_name
curveType #@curveType="VonBertalanffy"


# Start reading ID_ALASAN_femaleWetMass data frame
ID_ALASAN_femaleWetMass<-data.frame(
			Variable = "femaleWetMass",
			DOI1 = "10.1666/0094-8373(2008)034[0264:MGRFSD]2.0.CO;2",
			DOI2 = "10.1007/s12542-009-0017-5",
			DOI3 = "10.1666/0022-3360(2002)0762.0.CO;2",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = c("We took the maximum value of the growth curve for length and then we transformed this value,
							to wet weight using conversionToWetMass. We also add a reference for the largest estimated specimen of Alamosaurus included in Lehman and Woodward")
)
# End of ID_ALASAN_femaleWetMass data frame

femaleMaxLength=Linf * (1-exp(-k*(ageOfFirstMaturation-t0)))#Obtain maxBL at t=length of ageVector
femaleDryMass<-coefficientForMassA*femaleMaxLength^scaleForMassB #using the glm model coefficients from Campione and Evans, convert BL to dryMass
femaleWetMass<-as.vector(femaleDryMass*conversionToWetMass) #convert dryMass to wetMass
femaleWetMass  #@femaleWetMass=27372.97


# Start reading ID_ALASAN_lengthAtBirth data frame
ID_ALASAN_lengthAtBirth<-data.frame(
			Variable = "lengthAtBirth", 
			DOI1 = "10.1666/0094-8373(2008)034[0264:MGRFSD]2.0.CO;2",
			Category1 = "Article", 
			DataTreatment = "We performed a growth curve with the data of lenghts choosing the model by AIC criterion. Then, we took the intercept as value"
)
# End of ID_ALASAN_lengthAtBirth data frame

lengthAtBirth <- Linf * (1-exp(-k*(0-t0)))
lengthAtBirth #@lengthAtBirth=0.0003905196 ##within the 25-50 cm as expected ;; """after adjusting with gcmGAT= 0.0003198117"""

# Start reading ID_ALASAN_eggDryMass data frame
ID_ALASAN_eggDryMass<-data.frame(
			Variable = "eggDryMass",
			DOI1 = "10.1371/journal.pone.0072862",
			ISBN = "978-0-916984-80-9",
			Category1 = "Article",
			Category2 = "Book",
			DataTreatment = "It has been calculated based on the VB curve model. However there is also data based on alometric equation in the paper. Try with an alternative calculation based on eggdensity (1.06, from book reference) and known diameter (15-25cm(18))"
)
# End of ID_ALASAN_eggDryMass data frame

eggDryMass<-as.vector(coefficientForMassA*lengthAtBirth^scaleForMassB)
eggDryMass #@eggDryMass=2.858674

# eggWetMass<-eggDryMass*conversionToWetMass
# eggWetMass= 9.528818 #this is too heavy, but then again it is derived from an unrealistically long-lived individual.

# Start reading ID_ALASAN_lMin data frame
ID_ALASAN_lMin<-data.frame(
			Variable = "lMin",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "body length of the adult for lMin and lMax is the same because we don't want variability"
)
# End of ID_ALASAN_lMin data frame

lMin<-femaleMaxLength
lMin #@lMin=0.01852241


# Start reading ID_ALASAN_lMax data frame
ID_ALASAN_lMax<-data.frame(
			Variable = "lMax",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "body length of the adult for lMin and lMax is the same because we don't want variability"
)
# End of ID_ALASAN_lMax data frame

lMax<-femaleMaxLength
lMax #@lMax=0.01852241


# Start reading ID_ALASAN_growthK data frame
ID_ALASAN_growthK<-data.frame(
			Variable = "growth",
			DOI1 = "10.1371/journal.pone.0067012",
			Category1 = "Article",
			DataTreatment = "Based on the methodology shown in the paper, we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_ALASAN_growthK data frame

growthK<-k
print(paste("growthK: ", growthK)) 
growthK #@growthK=0.0001215718

# Start reading ID_ALASAN_longevitySinceMaturation data frame
ID_ALASAN_longevitySinceMaturation<-data.frame(
			Variable = "longevitySinceMaturation",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Calculated based on longevity and ageOfFirstMaturation"
)
# End of ID_ALASAN_longevitySinceMaturation data frame

ageOfFirstMaturation
20405


lifeTimeLayingEggs=longevity-ageOfFirstMaturation # Values of oviposition time from the table (mean of commercial and wild)

longevitySinceMaturation <- as.numeric((ageOfFirstMaturation + lifeTimeLayingEggs + pupaPeriodTime)/ageOfFirstMaturation)
longevitySinceMaturation<-round(longevitySinceMaturation,4) 
longevitySinceMaturation #@longevitySinceMaturation=3.6364
3.6364  

# Start reading ID_ALASAN_growthKmin data frame
ID_ALASAN_growthKmin<-data.frame(
			Variable = "growthKmin",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_ALASAN_growthKmin data frame

growthKmin<-as.vector(growthK)
growthKmin #@growthKmin=0.0001215718



# Start reading ID_ALASAN_growthKmax data frame
ID_ALASAN_growthKmax<-data.frame(
			Variable = "growthKmax",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_ALASAN_growthKmax data frame

growthKmax<-as.vector(growthK)
growthKmax #@growthKmax= 0.0001215718


# SECTION 2: In this section there are included all the parameters for WEAVER. 
# Some of them are directly inputed from the parametric values obtained in the previous section.
# The rest of the parameters are taken from the literature or there are set up by 
# programming criteria or by the user (this preferences must be understood before changing).

# =====================
# GENERAL PARAMETERS
# =====================

json.activatedHandling = FALSE  # If TRUE, predator handling time is considered for the simulation
json.activityUnderPredationRisk = 1.0  # Fraction of normal activity maintained under predation risk
json.poreUsePerInstar = rep(4.0,length(ageVector)+1)  # Pore use for each developmental instar
json.coefficientForHandlingTimeA = 28.407  # Coefficient 'A' for handling time equation
json.coefficientForHandlingTimeB = -0.664  # Coefficient 'B' for handling time equation
json.coefficientForHandlingTimeC = -6.365  # Coefficient 'C' for handling time equation
json.conversionToWetMass = rep(conversionToWetMass,length(ageVector+1)) # Factor to convert from dry mass (or another measure) to wet mass
json.daysWithoutFoodForMetabolicDownregulation = 9999.0  # Days without food before metabolic downregulation
json.debug.surviveWithoutFood = FALSE  # Debug flag to allow survival without food if TRUE
json.debug.forcePresenceAllResourcesInvolved = FALSE  # If TRUE, forces all resource types to be present in a simulation
json.defaultHuntingMode = "does_not_hunt"  # Default hunting mode if no other is specified
json.femaleMaxReproductionEvents = femaleMaxReproductionEvents  # Maximum number of reproduction events for a female
json.forDensitiesA = 74.8  # Parameter 'A' for population density functions
json.forDensitiesB = -0.75  # Parameter 'B' for population density functions
json.forDensitiesEggSize = (lengthAtBirth*0.3)+lengthAtBirth  # Egg size used in some density-related calculations
json.forDensitiesGrowth = growthK  # Growth rate factor in density-related calculations

# =====================
# GENETICS PARAMETERS
# =====================

json.genetics.modules.correlationCoefficientRhoPerModule = c(0.01, 0.01)  # Correlation coefficients between traits in each genetic module
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

## base - assim // From Joan TFM
json.traits.base.assim.limits.max = 0.0  # Upper limit on assimilation rate
json.traits.base.assim.limits.min = 0.0  # Lower limit on assimilation rate
json.traits.base.assim.ranges.max = 0.7  # Upper range boundary for sampling assimilation
json.traits.base.assim.ranges.min = 0.6  # Lower range boundary for sampling assimilation
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

## base - eggDevTime - MOREDINO
json.traits.base.eggDevTime.limits.max = 0.0  # Max for individual egg development time
json.traits.base.eggDevTime.limits.min = 0.0  # Min for individual egg development time
json.traits.base.eggDevTime.ranges.max = 181.0  # Upper boundary for sampling
json.traits.base.eggDevTime.ranges.min = 111.0  # Lower boundary for sampling
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
json.traits.base.coeffMassForScopeRadius.ranges.max = 0.5 
json.traits.base.coeffMassForScopeRadius.ranges.min = 0.5
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
json.traits.base.coeffMassForInteractionRadius.ranges.max = 0.05 
json.traits.base.coeffMassForInteractionRadius.ranges.min = 0.05
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
json.traits.base.coeffMassForSearchRadius.ranges.max = 2.0 
json.traits.base.coeffMassForSearchRadius.ranges.min = 2.0
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
json.traits.base.coeffMassForSpeed.ranges.max = 24.5 
json.traits.base.coeffMassForSpeed.ranges.min = 26.5
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
json.traits.probabilityDensityFunction.muForPDF.ranges.max = 8.65
json.traits.probabilityDensityFunction.muForPDF.ranges.min = 8.65
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

ages <- c(0, ageVector)

    # Predict body lengths
    L <- predict(
        best_model,
        newdata = data.frame(age = ages)
    )

# Compute masses
massesVector <- (coefficientForMassA * L^scaleForMassB) * conversionToWetMass

massesVector <- as.numeric(massesVector)
massesVector

# write.table(massesVector,"massesVectorAlamosaurus.txt",row.names=F,col.names=F)

source(
	file.path(parametrisation_path, "utilities", "codes", "functions", "Alamo_Edmonto_densities_from_Hatton_2015.r"),
	local = TRUE
)

individualsPerInstar <- res$species$AlamosaurusMass$individualsPerInstar

json.individualsPerInstar=individualsPerInstar

# Number of individuals simulated per instar

json.requiredRepresentAllPossibleAlleles = FALSE

json.maleMaxReproductionEvents = 8   # Maximum number of reproduction events for a male
json.maleMobility = 1  # Mobility factor for males (compared to females or baseline)
json.maleReproductionFactor = 0.25  # Fraction of how many mates or eggs a male can fertilize compared to baseline
json.minRelativeHumidityThreshold = rep(10, times = length(json.individualsPerInstar))  # Minimum humidity needed for survival or normal behavior

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


################################################################
################################################################
################################################################


source(file.path(parametrisation_path, "utilities", "codes", "functions", "calculateMaximumResourceCapacityDensityAlternative.r"))


##NOTE: firstInstarToCalculate is because ontogenetic niche shifts


maxCapacity<-calculateMaximumResourceCapacityDensityAlternative(
	simulationType = "Dinosaurs", individualsPerInstar = individualsPerInstar,
	firstInstarToCalculate=1, landscapeArea=53^2, ageVector, 
	coefficientForMassA, coefficientForMassAforMature, 
	scaleForMassB, scaleForMassBforMature, conversionToWetMass, 
	growthModel = fitted[[as.character(tempFromLab)]]$best_model
)

print("maximumResourceCapacityDensityAlternative:")
print(maxCapacity)

# [1] 6701.766 ##this is the total mass in a cell. We initializate at this value too.


individualsPerInstar
sum(individualsPerInstar[1:length(individualsPerInstar)])
# 245260


