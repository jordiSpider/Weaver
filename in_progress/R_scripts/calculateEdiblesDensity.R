

###########
## World ##
###########


simulationType = "Arthropods"



####################
## Animal Species ##
####################

lengthAtMaturationMax = 0.396
lengthAtMaturationMin = 0.322

coefficientForMassA = 0.154231343
coefficientForMassAforMature = 0.154231343

scaleForMassB = 2.8981
scaleForMassBforMature = 2.8981

animalConversionToWetMass = 3.45

newAAdult = 0.15
newB = 0.75

numberOfAdults = 50

worldArea = 1e+06


######################
## Resource Species ##
######################


edibleFractionOfMaxCarryingCapacity = 0.9

resourceConversionToWetMass = 5



########################################################
########################################################
########################################################


adultsDensity = numberOfAdults / worldArea



lengthAtMaturation = mean(c(lengthAtMaturationMax, lengthAtMaturationMin))

dryMassAtMaturation = coefficientForMassAforMature * lengthAtMaturation^scaleForMassBforMature

wetMassAtMaturation = dryMassAtMaturation * animalConversionToWetMass



if(simulationType == "Dinosaurs"){
    maxVoracityAdultPerDay = (152*wetMassAtMaturation^0.738) / 1000
} else {
    maxVoracityAdultPerDay = newAAdult*wetMassAtMaturation^newB
}

maxVoracityAdultPerDay = maxVoracityAdultPerDay / animalConversionToWetMass


maxVoracityAdultDensity = maxVoracityAdultPerDay * adultsDensity






initialBiomassDensity = (maxVoracityAdultDensity / edibleFractionOfMaxCarryingCapacity) * resourceConversionToWetMass



print(initialBiomassDensity)

