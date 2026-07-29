
# Define the function to transform data
transform_data <- function(data, asld) {
  filter_data <- function(data) {
    data$stage <- tolower(data$stage)
    data$age[data$stage == "adult"] <- data$age[data$stage == "larva"] +
      data$age[data$stage == "deutonymph"] +
      data$age[data$stage == "protonymph"]
    data$age[data$stage == "deutonymph"] <- data$age[data$stage == "protonymph"] +
      data$age[data$stage == "larva"]
    data$age[data$stage == "protonymph"] <- data$age[data$stage == "larva"]
    data$age[data$stage == "larva"] <- 0
    stages_to_keep <- c("larva", "protonymph", "deutonymph", "adult")
    data_filtered <- data[data$stage %in% stages_to_keep, ]
    return(data_filtered)
  }
  
  # Filter and adjust data based on stages
  data_age_f <- filter_data(data)
  
  # Print to debug
  # print("Filtered data:")
  # print(data_age_f)
  
  # Initialize age column in data_long dataframe
  data_long <- data.frame(identity = character(), stage = character(), lengths = numeric(), age = numeric(), stringsAsFactors = FALSE)
  
  # Mapping between abbreviated stage names and full names
  stage_mapping <- list("Larva" = "larva", "Proto" = "protonymph", "Deuto" = "deutonymph", "Adult" = "adult")
  
  # Convert data to long format
  # print("Column names of asld:")
  # print(colnames(asld))  # Print column names of asld for debugging
  
  for (i in 1:nrow(asld)) {
    for (stage in c("Larva_Length", "Proto_Length", "Deuto_Length", "Adult_Length")) {
      # print(paste("Processing row", i, "and stage", stage))  # Debug print
      if (!is.na(asld[i, stage])) {
        short_stage <- gsub("_Length", "", stage)
        full_stage <- stage_mapping[[short_stage]]
        data_long <- rbind(data_long, data.frame(identity = asld[i, "Identity"], stage = full_stage, lengths = asld[i, stage]))
      }
    }
  }
  
  # Print to debug
  # print("Long format data:")
  # print(data_long)
  
  # Match stages and calculate age
  for (i in 1:nrow(data_long)) {
    for (j in 1:nrow(data_age_f)) {
      if (data_long[i, "stage"] == data_age_f[j, "stage"]) {
        data_long[i, "age"] <- data_age_f[j, "age"]
        break
      }
    }
  }
  
  return(data_long)
}
