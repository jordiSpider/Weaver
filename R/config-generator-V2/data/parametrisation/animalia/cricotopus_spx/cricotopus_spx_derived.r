# @@@@ species: cricotopus_spx

if (!"app" %in% ls()) {
	script_path <- dirname(sys.frame(1)$ofile)

	source(file.path(script_path, "cricotopus_spx_baseline.r"))
}

parametrisation_path <- sub("(.*/parametrisation)/.*", "\\1", script_path)


source(file.path(parametrisation_path, "utilities", "codes", "functions", "growth_curve_modelfitting", "gcmGAT.r"))


set.seed(123)



cricotopus_table<-read.table(file.path(script_path, "Chironomidae_Cricotopus.txt"), header=TRUE)
colnames(cricotopus_table)[which(names(cricotopus_table) == "days")] <- "age"
colnames(cricotopus_table)[which(names(cricotopus_table) == "mm")] <- "length"

head(cricotopus_table)

cricotopus_data <- split(cricotopus_table, cricotopus_table$temperature)

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


best_model<-fitted[[as.character(param.tempFromLab)]]$best_model


female_predicted_length <- predict(best_model, newdata = 
                           data.frame(age = max(cricotopus_data[[as.character(param.tempFromLab)]])))
femaledrymass<-param.growthModule.coefficientForMassA*female_predicted_length^param.growthModule.scaleForMassB
femaleWetMass<-femaledrymass*param.conversionToWetMass[[1]]
femaleWetMass  #@femaleWetMass=0.4802704

param.growthModule.femaleWetMass = femaleWetMass
