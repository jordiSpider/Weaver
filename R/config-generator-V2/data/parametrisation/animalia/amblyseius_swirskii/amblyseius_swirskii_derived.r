# @@@@ species: amblyseius_swirskii

source(file.path(script_path, "transform_data.r"))


# Read the input data
amblyseius_swirskii_lengths_diego <- read.table(file.path(script_path, "tamaños_swirskii_DIEGO.txt"), head=TRUE)
asld <- amblyseius_swirskii_lengths_diego
age <- as.numeric(c(2.22, 0.72, 1.50, 1.53, 5.98))
stage <- c("egg", "larva", "protonymph", "deutonymph", "adult")
data <- cbind.data.frame(stage, age)

# Transform data
data_long_nonaggregate <- transform_data(data, asld)
data_long_nonaggregate<-data_long_nonaggregate[,-1]
colnames(data_long_nonaggregate)[2]<- "length"

data_long<- aggregate(length ~ stage+age, data = data_long_nonaggregate, mean)


# Start reading ID_AMBSWI_growthKmin data frame
ID_AMBSWI_growthKmin<-data.frame(
			Variable = "growthKmin",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_AMBSWI_growthKmin data frame

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

amblyseius_swirskii_data_min <- list()

amblyseius_swirskii_data_min[[as.character(param.tempFromLab)]] = data_for_min


force_model = list()
force_model[[as.character(param.tempFromLab)]] = param.growthModule.growthModel.defaultAtTempFromLab.model

fitted_min<-gcmGAT(
	amblyseius_swirskii_data_min, 
	x = "age", y = "length",
	force_model = force_model
)


growthKmin<-fitted_min[[as.character(param.tempFromLab)]]$growth
growthKmin #@growthKmin=0.07119579


# Start reading ID_AMBSWI_growthKmax data frame
ID_AMBSWI_growthKmax<-data.frame(
			Variable = "growthKmax",
			DOI1 = "12.1009_BF005517.fake",
			Category1 = "Thesis dissertation",
			DataTreatment = "we performed a model autofit adjustment using AIC criterion and then we extracted the value of k from the summary"
)
# End of ID_AMBSWI_growthKmax data frame


data_for_max<-aggregate(length ~ stage+age, data = data_long_nonaggregate, max)
data_for_max$length[1]<-data_long$length[1]

# data_for_max


amblyseius_swirskii_data_max <- list()

amblyseius_swirskii_data_max[[as.character(param.tempFromLab)]] = data_for_max


fitted_max<-gcmGAT(
	amblyseius_swirskii_data_max, 
	x = "age", y = "length",
	force_model = force_model
)



growthKmax<-fitted_max[[as.character(param.tempFromLab)]]$growth
growthKmax #@growthKmax= 0.4920556


if (isTRUE(getOption("allow_plotting"))) {

	predictions_min <- predict(fitted_min[[as.character(param.tempFromLab)]]$best_model, newdata = data_for_min)  
	predictions_max <- predict(fitted_max[[as.character(param.tempFromLab)]]$best_model, newdata = data_for_max)  

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
} else {
  	cat("Plotting is disabled.\n")
}


# =====================
# TRAITS PARAMETERS
# =====================

param.traits.base.growth.ranges.max = growthKmax  # Upper boundary for sampling growth rate
param.traits.base.growth.ranges.min = growthKmin  # Lower boundary for sampling growth rate

