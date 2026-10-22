# @@@@ species: edmontosaurus_anectens

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

# Start reading ID_EDMANN_tempFromLab data frame

#@@BVazquez - documentar biblio sobre temperatura Cretaci
ID_EDMANN_tempFromLab<-data.frame(
			Variable = "tempFromLab",
			DOI1 = "10.5194/cp-12-1181-2016",
			Category1 = "Article",
			DataTreatment = "taken from supplementary simulations"
)
# End of ID_EDMANN_tempFromLab data frame
tempFromLab<-21.4 ##data for calculations here
tempFromLab #@tempFromLab=21.4  ##data for Json for simulation

# Start reading ID_EDMANN_eggClutchFromEquation data frame	
ID_EDMANN_eggClutchFromEquation<-data.frame(
			Variable = "eggClutchFromEquation", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Not available"  ##@David - is not common knowledge is equation not available
)
# End reading ID_EDMANN_eggClutchFromEquation data frame

eggClutchFromEquation<-FALSE
eggClutchFromEquation #@eggClutchFromEquation=FALSE

# Start reading ID_EDMANN_eggMassFromEquation data frame	
ID_EDMANN_eggMassFromEquation<-data.frame(
			Variable = "eggMassFromEquation", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Not available"
)
# End reading ID_EDMANN_eggMassFromEquation data frame

eggMassFromEquation<-"false"
eggMassFromEquation #@eggMassFromEquation="false"

# Start reading ID_EDMANN_sexRatio data frame		  
ID_EDMANN_sexRatio<-data.frame(
			Variable = "sexRatio", 
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Not available. We assume fisherian 50:50 sex ratio"
)
# End of ID_EDMANN_sexRatio data frame

sexRatio<-0.5
sexRatio #@sexRatio=0.5

# Start reading ID_EDMANN_conversionToWetMass data frame
ID_EDMANN_conversionToWetMass<-data.frame(
			Variable = "conversionToWetMass", 
			DOI1 = "10.1007/s12542-009-0017-5", #@@BVazquez 
			Category1 = "Article",
			DataTreatment = "We performed a ratio between wetMass and dryMass based on the BM calculations for Hungarosaurus"
)
 # End of ID_EDMANN_conversionToWetMass data frame

conversionToWetMass<-3.3333
conversionToWetMass #@conversionToWetMass=3.3333

# Start reading ID_EDMANN_eggsPerBatch data frame
ID_EDMANN_eggsPerBatch<-data.frame(
			Variable = "eggsPerBatch",
			DOI1 = "10.1371/journal.pone.0072862",
			DOI2 = "10.1666/13028",
			ISSN = NA,#@@BVazquez
			Category1 = "Article",
			Category2 = "Article",
			DataTreatment = "We employ alometric equations in both studies, based on the egg mass derived from Werner & Griebeler 2013 and the EggClutchMass from Ruxton et al. 2014"
)
# End of ID_EDMANN_eggsPerBatch data frame

eggsPerBatch<-20
eggsPerBatch #@eggsPerBatch=20 ##It appears to be a mean value from the literature of hadrosaurs (16-22), however, in MOREDINO it is calculated in several ways, the one from griebeler and werner 2013 shows promise for eggDryMass: 0.09·DryMass^0.311, then from clutch mass(=0,384*(DryMass^0,496)): clutchMass/eggDryMass


# Start reading ID_EDMANN_coefficientForMassA data frame
ID_EDMANN_coefficientForMassA<-data.frame(
			Variable = "coefficientForMassA", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = "Campione and Evans... explain"
)
# End of ID_EDMANN_coefficientForMassA data frame

ML_edmonto<-read.table(file.path(script_path, "edmonto_ROM_7383_fixed_egg_mass.txt"), h=T)

ML_edmonto$dryMass=ML_edmonto$DryMass_Kg

##Fit the model that correlates edmontosaurus TCirc with dryMass (obtained from literature) and print the resulting coefficients as in dryMass=A(TCirc)^b
glm.ab<-glm(DryMass_Kg~log(TCirc_Km),data=ML_edmonto,family=gaussian(link="log"))

summary(glm.ab)
#              Estimate Std. Error t value Pr(>|t|)    
# (Intercept)    43.4293     1.9882   21.84 2.45e-13 ***
# log(TCirc_Km)   4.5229     0.2492   18.15 4.25e-12 ***
coef(glm.ab)
    # (Intercept) log(TCirc_Km) 
    #43.429311      4.522941 
		  
plot(ML_edmonto$TCirc_Km,ML_edmonto$dryMass,pch=16)
theSeq<-data.frame(TCirc_Km=seq(min(ML_edmonto$TCirc_Km),
max(ML_edmonto$TCirc_Km),length.out=1000))
lines(theSeq$TCirc_Km,exp(predict(glm.ab,theSeq)),lwd=3)

exp(predict(glm.ab,theSeq))[1]

exp(predict(glm.ab,data.frame(TCirc_Km=ML_edmonto$TCirc_Km)))[1]
ML_edmonto$dryMass[1]
predict(glm.ab,data.frame(TCirc_Km=ML_edmonto$TCirc_Km),type="response")

coefficientForMassA<-as.vector(exp(coef(glm.ab)[1]))
coefficientForMassA 


cat(as.character(coefficientForMassA), "\n")
as.numeric(coefficientForMassA)
coefficientForMassA #@coefficientForMassA =7262900970542266368
# PREVIOUSLY 6.627414e+18

# Start reading ID_EDMANN_scaleForMassB data frame
ID_EDMANN_scaleForMassB<-data.frame(
			Variable = "scaleForMassB", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = " "
)
# End of ID_EDMANN_scaleForMassB data frame

scaleForMassB <- as.vector(coef(glm.ab)[2])

scaleForMassB #@scaleForMassB=4.522941
# PREVIOUSLY 4.511537

# Start reading ID_EDMANN_coefficientForMassAforMature data frame
ID_EDMANN_coefficientForMassAforMature<-data.frame(
			Variable = "coefficientForMassAforMature", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = "No metamorphosis. Mature individuals take the value of juveniles"
)
# End of ID_EDMANN_coefficientForMassAforMature data frame

coefficientForMassAforMature <- coefficientForMassA
coefficientForMassAforMature #@coefficientForMassAforMature=7262900970542266368

# Start reading ID_EDMANN_scaleForMassBforMature data frame
ID_EDMANN_scaleForMassBforMature<-data.frame(
			Variable = "scaleForMassBforMature", 
			DOI1 = "10.1023_A_1006236310613",
			DOI2 = "10.1007_BF00346269",
			DOI3 = "10.1007_BF03356514",
			Category1 = "Article",
			Category2 = "Article",
			Category3 = "Article",
			DataTreatment = "No metamorphosis. Mature individuals take the value of juveniles"
)
# End of ID_EDMANN_scaleForMassBforMature data frame

scaleForMassBforMature = scaleForMassB
scaleForMassBforMature #@scaleForMassBforMature=4.522941


# Start reading ID_EDMANN_curveType data frame
ID_EDMANN_curveType<-data.frame(
			Variable = "curveType",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "we performed a model autofit adjustment using AIC criterion"
)			
# End of ID_EDMANN_curveType data frame


# Start reading ID_EDMANN_pupaPeriodTime  data frame
ID_EDMANN_pupaPeriodTime <-data.frame(
			Variable = "pupaPeriodTime ",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "mean of values for pre-oviposition time for commercial and wild taken from Table 2.2.2.b"
)
											 

# End of ID_EDMANN_pupaPeriodTime  data frame

pupaPeriodTime <-1.00
pupaPeriodTime  #@pupaPeriodTime =1.00

##200 years lifespan according to Lehman and Woodward, but estimates differ according to authors
##as low as 45 years - thus, @@BVazquez, @@Jordi - simulations with different lifespan assumptions
##
longevity = max(ML_edmonto$Age_D)

##Here we discard Lehman and Woodwar "von Bertalanffy" model for mass and we build a conventional
##von Bertalanffy model from scratch, directly based on length not on mass
##predict mass for an individual with the Lehman and Woodard model. 
t=1:longevity
# M=32663*(1-exp(-b*t))^3 ##the minus "-" before the "b" sign was not included in the publication - mass in Kg
# M[1]  ##this provides a mass for the newlyborn of zero, see solution below				

plot(ML_edmonto$Age_D,exp(predict(glm.ab)))
# glm2<-glm(length~log(Age_D),data=ML_edmonto,gaussian(link="log"))
# plot(ML_edmonto$Age_D,exp(predict(glm2)))
#CANT APPLY TO EDMONTOSAURUS RIGHT? NO EXTRAPOLATION NEEDED LIKE IN ALAMO
# ##The intercept will be the length of the newborn
# lm1<-lm(length~age,data=edmonto_L_A)
# lm1
# # Coefficients:
# # (Intercept)          age  
   # # 2.19e-04     2.79e-08   

# summary(lm1)$r.squared
# # 0.4499293
# predict(lm1,data.frame(age=0))
# # 0.0002189849 

# ###Now we estimate t0 for VB by using inverse regression in a linear model 
# plot(length~age,data=edmonto_L_A,xlim=c(-190,2),ylim=c(0,0.001))
# abline(lm(length~age,data=edmonto_L_A),lwd=3,lty=1)
# abline(h=3.198e-04,lwd=3,col="red",lty=2)
						 
ageOfFirstMaturation=6 #instar 7 after Wosik and Evans 2022, but we need it in days
ageOfFirstMaturation=6*371
ageOfFirstMaturation

# ##Rearranging the linear equation we obtain the wanted value of t0 given a length of 0
# a=as.vector(coef(lm1)[1]) #coefficientForMassA as vector
# b=as.vector(coef(lm1)[2]) #scaleForMassB as vector
# y=0 #at t0, BL=0
# x=(y-a)/b #rearranged linear equation
# x
# -160.7533
# t0=x

# t0= -142#based on incubation time Maiasaura
# Linf=3.460e-04 #based on previous scripts
# k=0.000124 #provisional k

# L = Linf * (1-exp(-k*(t-t0))) #VB equation
# plot(t,L)
# points(edmonto_L_A$age,edmonto_L_A$length,col="red",pch=16)

# ##Since growth is actually determinate, we need to decide the ageOfFirstMaturation, when growth rate (in BL) slows down as the individual becomes sexually mature and begins to invest more on reproduction
# for(i in 1:length(L)){
	# if(i>1){
		# if(round(L[i],6) == round(L[i-1],6)){
		  # maxLength=L[i-1]
		  # ageOfFirstMaturation = t[i-1] + pupaPeriodTime
		  # print("##warning: the error message is not true, just that the solution has been found")
		  # stop(paste("maxLength: ",maxLength,"ageOfFirstMaturation: ",ageOfFirstMaturation),sep="")
		# }
	# }
# }


## ageOfFirstMaturation and longevity is now expressed in days already, so this section is no longer useful:
# ageOfFirstMaturation = ageOfFirstMaturation*371 ##in days
# longevity = longevity*371 ##in days

ageVector=seq(371,t[length(t)],371) #ageOfFirstMaturation is already in days
ageVector

#@@David: cómo se pone en R y para Json?

##per produir el ageVector llegible per transformar al Json
paste0(ageVector,collapse=",")
# "371,742,1113,1484,1855,2226,2597,2968,3339,3710,4081,4452,4823,5194,5565,5936,6307,6678,7049,7420"

# Start reading ID_EDMANN_ageVector data frame
ID_EDMANN_ageVector<-data.frame(
			Variable = "ageVector",
			DOI1 = "12.1009_BF005517.fake", #@BVazquez
			Category1 = "Thesis dissertation",
			DataTreatment = "Taken from Table 2.2.1.b for commercial (total fecundity)"
)		
# End of ID_EDMANN_ageVector data frame
																																																																																																																																																																																																																																																																																																									   

ageVector #@ageVector="371,742,1113,1484,1855,2226,2597,2968,3339,3710,4081,4452,4823,5194,5565,5936,6307,6678,7049,7420"

# Start reading ID_EDMANN_instarFirstReproduction data frame
ID_EDMANN_instarFirstReproduction<-data.frame(
			Variable = "instarFirstReproduction",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "Mentioned in the text"
)
# End of ID_EDMANN_instarFirstReproduction data frame

##@@simul (see also @@coDependency) ageOfFirstMaturation if changed other parameter in the optimizer.r must change
##(@BVazquez, @BVila justification to be written) and this value and see how the different
instarFirstReproduction<-ageVector[which((abs(ageOfFirstMaturation-ageVector))==min(abs(ageOfFirstMaturation-ageVector)))]  ##because ageVector starts at date of Instar II
instarFirstReproduction<-instarFirstReproduction/371 
instarFirstReproduction #@instarFirstReproduction=6

####@@coDependency - @@Mario - instarFirstReproduction when it changes, other parameters need to be recalculated such as
####instarFirstReproduction requires recalculation of: femaleMaxReproductionEvents, longevitySinceMaturation

# Start reading ID_EDMANN_femaleMaxReproductionEvents data frame
ID_EDMANN_femaleMaxReproductionEvents<-data.frame(
			Variable = "femaleMaxReproductionEvents", 
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation", 
			DataTreatment = "taken from Table 2.2.2.b for females (mean from commercial and wild)"
)
# End of ID_EDMANN_	 data frame

totalReproductiveTime=longevity-ageOfFirstMaturation #already in days
femaleMaxReproductionEvents<-round(totalReproductiveTime/371)
femaleMaxReproductionEvents #@femaleMaxReproductionEvents=14
12



# SINCE WE KEPT USING BL, dryMass WAS NEVER CONVERTED INTO wetMass AT THE BEGINNING OF THE SCRIPT section invalidated below:
##Important: convert the Mass to dryMass before using the ab coefficients
# dryMass=M*(1/conversionToWetMass)
# L=(dryMass/coefficientForMassA)^(1/scaleForMassB)


# plot(early_stages$age,early_stages$length,xlim=c(0,max(early_stages$age)),
# ylim=c(0,max(early_stages$length)))

# abline(lm(length~age,data=early_stages))
# abline(v=0,lwd=3)


##range of egg diameter for Titanosaurids 15-25 cm. ref. Klein et al. 2011. ##Vila et al. 2010 PlosONe range: 16-18 cm.
##assuming a 2-fold for animal total length gives 30-50 cm. Thus we are in a lower end 32 cm.
##@@BVazquez @@Jordi: run simulations assuming eggs and lengths in this range 30-50cm for lengthAtBirth.
##Note: Werner & Griebeler 2013 equation  0.09*femaleWetMass^0.311 gives 32 Kg for egg mass, way off
##Note: Erickson & Tumanova 2000 - 11% of female body length - way off 2.2 m for the newborn

# lengthAtBirthProvisional<-as.vector(predict(lm1,data.frame(age=0)))
# lengthAtBirthProvisional #=0.0002189849=21.9cm
# eggDryMassProvisional=coefficientForMassA*lengthAtBirthProvisional^scaleForMassB #=1.9kg; eggWetMassProvisional=(eggDryMassProvisional*conversionToWetMass)=6.3kg

### Modelfitting for the growth curve


# (coefficientForMassA*0.021^scaleForMassB)*conversionToWetMass
# (coefficientForMassA*0.035^scaleForMassB)*conversionToWetMass


## wetMass HAS NOT BEEN CALCULATED UP UNTIL NOW, THEREFORE THIS SECTION HAS BEEN INVALIDATED AS WE ALREADY HAVE dryMass:
# wetMassVector=M[1:length(ageVector)] #M has been determined before, it represents the wetMass of the individual
# dryM=(1/conversionToWetMass)*wetMassVector

# dryMass=ML_titano$dryMass

# dryMassVector=dryMass[1:length(ageVector)]

# length=(dryMassVector/coefficientForMassA)^(1/scaleForMassB)

#NOT NEEDED FOR EDMONTO?
# Linf = ((32663*(1/conversionToWetMass))/coefficientForMassA)^(1/scaleForMassB) #Linf calculation, not for hadrosaurs
# L = Linf * (1-exp(-k*(t-t0)))#BVeq
#length=trait que estem relacionant amb la drymass i per tant la longitut de l'animal.
# length=c(lengthAtBirthProvisional, L)
# age=c(0,1:ageOfFirstMaturation)
# length_truncated<-length[1:(length(age)-1)] 
# length_new<-c(lengthAtBirthProvisional, length_truncated)
# temp<-rep(tempFromLab, length(age))
# data_long<-data.frame(age=age,length=length_new,temp=temp)


age=c(0,ML_edmonto$Age_D)
temp<-rep(tempFromLab, length(age))
edmontosaurus_data_frame<- data.frame(age=c(0,ML_edmonto$Age_D), length=c(ML_edmonto$TCirc_Km[1],ML_edmonto$TCirc_Km))
edmontosaurus_data <- list()

edmontosaurus_data[[as.character(tempFromLab)]] = edmontosaurus_data_frame

fitted <- gcmGAT(
			edmontosaurus_data, 
			x="age", 
			y="length", 
			use_app = TRUE)


best_model <- fitted[[as.character(tempFromLab)]]$best_model

plot(edmontosaurus_data$'21.4'$age,edmontosaurus_data$'21.4'$length,pch=16)

lines(edmontosaurus_data$'21.4'$age,predict(best_model,newdata=data.frame(age=edmontosaurus_data$'21.4'$age)),col="blue",lty=2,lwd=3)

best_model

# Nonlinear regression model
  # model: length ~ Linf * (1 - exp(-k * (age - t0)))
   # data: data
      # Linf          k         t0 
 # 3.483e-04  1.423e-03 -1.473e+02 
 # residual sum-of-squares: 8.313e-10

# Number of iterations to convergence: 1 
# Achieved convergence tolerance: 1.49e-08

# Start reading ID_EDMANN_curveType data frame
ID_EDMANN_curveType<-data.frame(
			Variable = "curveType",
			DOI1 = "10.1666/0094-8373(2008)034[0264:MGRFSD]2.0.CO;2",
			Category1 = "Article", 
			DataTreatment = "In lehman and Woodward, growth curves are presented as a VB growth type for Alamosaurus even though for the original study the curve is logarithmic. In any case, we performed a model autofit adjustment using AIC criterion"
)			
# End of ID_EDMANN_curveType data frame

curveType<-fitted$'21.4'$best_model_name
curveType #@curveType="VonBertalanffy"


# Start reading ID_EDMANN_femaleWetMass data frame
ID_EDMANN_femaleWetMass<-data.frame(
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
# End of ID_EDMANN_femaleWetMass data frame

femaleMaxLength=predict(best_model,newdata=age)[length(age)] #Obtain FC at t=length of ageVector
femaleDryMass<-coefficientForMassA*femaleMaxLength^scaleForMassB #using the glm model coefficients from Campione and Evans, convert BL to dryMass
femaleWetMass<-as.vector(femaleDryMass*conversionToWetMass) #convert dryMass to wetMass
femaleWetMass  #@femaleWetMass=5384.825
5534.076


# Start reading ID_EDMANN_lengthAtBirth data frame
ID_EDMANN_lengthAtBirth<-data.frame(
			Variable = "lengthAtBirth", 
			ISBN = "9780521567237",
			Category1 = "Book", 
			DataTreatment = "calculated using the density of an egg (MOREDINO), eggDryMass and then inverse regression to estimate tibia circunference, assuming eggs of 20 cm diameter. 
			                 , which matches perfectly data from Horner and Currie XXXX for Hypacrosaurus. Horner, JR., & Currie, P.J. (1994). 
							Embryonic and neonatal morphology and ontogeny 
							of a new species of Hypacrosaurus (Ornithischia, 
							Lambeosauridae) from Montana and Alberta. In: 
							Dinosaur Eggs and Babies (Eds. Carpenter, K, 
							Hirsch, K.F., Horner, J.R.). Cambridge University 
							Press. Chap. 21: pgs. 312-336."
			
)
# End of ID_EDMANN_lengthAtBirth data frame

lengthAtBirth <- as.vector(predict(best_model, data.frame(age=0)))
lengthAtBirth #@lengthAtBirth=0.0003198117 ##within the 25-50 cm as expected; """after adjusting with gcmGAT= 0.0003198117"""
6.587e-05

# Start reading ID_EDMANN_eggDryMass data frame
ID_EDMANN_eggDryMass<-data.frame(
			Variable = "eggDryMass",
			ISBN = "9780521567237",
			Category1 = "Book",
			DataTreatment = "calculated using the density of an egg (MOREDINO), assuming eggs of 20 cm diameter. 
			                 , as in Horner and Currie 1994 for Hypacrosaurus. Horner, JR., & Currie, P.J. (1994). 
								Embryonic and neonatal morphology and ontogeny 
								of a new species of Hypacrosaurus (Ornithischia, 
								Lambeosauridae) from Montana and Alberta. In: 
								Dinosaur Eggs and Babies (Eds. Carpenter, K, 
								Hirsch, K.F., Horner, J.R.). Cambridge University 
								Press. Chap. 21: pgs. 312-336."
)
# End of ID_EDMANN_eggDryMass data frame

eggDryMass<-as.vector(coefficientForMassA*lengthAtBirth^scaleForMassB)
eggDryMass #@eggDryMass=1.893095


# Start reading ID_EDMANN_lMin data frame
ID_EDMANN_lMin<-data.frame(
			Variable = "lMin",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "body length of the adult for lMin and lMax is the same because we don't want variability"
)
# End of ID_EDMANN_lMin data frame

lMin<-femaleMaxLength
lMin #@lMin=0.0003482875


# Start reading ID_EDMANN_lMax data frame
ID_EDMANN_lMax<-data.frame(
			Variable = "lMax",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "body length of the adult for lMin and lMax is the same because we don't want variability"
)
# End of ID_EDMANN_lMax data frame

lMax<-femaleMaxLength
lMax #@lMax=0.0003482875



# Start reading ID_EDMANN_growthK data frame
ID_EDMANN_growthK<-data.frame(
			Variable = "growth",
			DOI1 = "10.1371/journal.pone.0067012",
			Category1 = "Article",
			DataTreatment = "Based on the methodology shown in the paper, we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_EDMANN_growthK data frame

##Definitiu?
growth_model<- best_model
growth<-coef(growth_model)["k"]
growthK<-growth #as.vector(t(growth))...WHY?
print(paste("growthK: ", growthK)) 
growthK #@growthK= 0.001422678"
0.001422678 

# Start reading ID_EDMANN_longevitySinceMaturation data frame
ID_EDMANN_longevitySinceMaturation<-data.frame(
			Variable = "longevitySinceMaturation",
			DOI1 = NA,
			Category1 = NA,
			DataTreatment = "Calculated based on longevity and ageOfFirstMaturation"
)
# End of ID_EDMANN_longevitySinceMaturation data frame

ageOfFirstMaturation
2226

lifeTimeLayingEggs=longevity-ageOfFirstMaturation # Values of oviposition time from the table (mean of commercial and wild)
###Note: pupaPeriodTime harcoded as 1 here for simplicity
longevitySinceMaturation <- as.numeric((ageOfFirstMaturation + lifeTimeLayingEggs + pupaPeriodTime)/ageOfFirstMaturation)
longevitySinceMaturation<-round(longevitySinceMaturation,4) 
longevitySinceMaturation #@longevitySinceMaturation=3.0574
3.0574  ##TODO - check on pupaPeriodTime

# Start reading ID_EDMANN_growthKmin data frame
ID_EDMANN_growthKmin<-data.frame(
			Variable = "growthKmin",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_EDMANN_growthKmin data frame

growthKmin<-as.vector(growthK)
growthKmin #@growthKmin=0.0001423333



# Start reading ID_EDMANN_growthKmax data frame
ID_EDMANN_growthKmax<-data.frame(
			Variable = "growthKmax",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_EDMANN_growthKmax data frame

growthKmax<-as.vector(growthK)
growthKmax #@growthKmax= 0.001422678


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
json.daysWithoutFoodForMetabolicDownregulation = 2.0  # Days without food before metabolic downregulation
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

## base - assim // Literature in: Taken from Pergamasus longicornis in Bowman 2019
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

## base - eggDevTime - from MOREDINO
json.traits.base.eggDevTime.limits.max = 0.0  # Max for individual egg development time
json.traits.base.eggDevTime.limits.min = 0.0  # Min for individual egg development time
json.traits.base.eggDevTime.ranges.max = 177.0  # Upper boundary for sampling
json.traits.base.eggDevTime.ranges.min = 107.0  # Lower boundary for sampling
json.traits.base.eggDevTime.restrictValue = 0.01  # Increment step

json.traits.base.eggDevTime.temperature.dependent = FALSE
json.traits.base.eggDevTime.temperature.activationEnergy = 0.0
json.traits.base.eggDevTime.temperature.energyDecay = 0.0
json.traits.base.eggDevTime.temperature.temperatureOptimal = 0.0
json.traits.base.eggDevTime.temperature.temperatureRef = 0.0

## base - energy_tank
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

best_model
# Nonlinear regression model
  # model: length ~ Linf * (1 - exp(-k * (age - t0)))
   # data: data
      # Linf          k         t0 
 # 0.0003461  0.0017170 -8.4058653 
 # residual sum-of-squares: 1.073e-09

# Number of iterations to convergence: 1 
# Achieved convergence tolerance: 1.49e-08


# Predict individual lengths
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

# write.table(massesVector,"massesVectorEdmontosaurus.txt",row.names=F,col.names=F)

source(
	file.path(parametrisation_path, "utilities", "codes", "functions", "Alamo_Edmonto_densities_from_Hatton_2015.r"),
	local = TRUE
)

individualsPerInstar <- res$species$EdmontosaurusMass$individualsPerInstar



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

json.name = "Edmontosaurus_anectens"  # Name of the species or simulation run
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
    growthModel = best_model
)

print("maximumResourceCapacityDensityAlternative:")
print(maxCapacity)

# [1] 650.0495 ##this is the total mass in a cell. We initializate at this value too.


individualsPerInstar
sum(individualsPerInstar[1:length(individualsPerInstar)])

# [1] 335662
