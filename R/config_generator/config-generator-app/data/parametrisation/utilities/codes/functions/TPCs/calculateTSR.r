#' Calculate Temperature–Size Rule (TSR)
#'
#' This function estimates the **Temperature–Size Rule (TSR)** across multiple temperatures
#' using fitted growth models. It predicts the final dry mass of mature individuals at the maximum
#' observed age for each temperature treatment.
#'
#' @param models A named list of model objects, where each element corresponds to a temperature.
#'   Each element must contain a component `best_model` representing the best-fitting growth model
#'   for that temperature.
#' @param data A named list of data frames corresponding to each temperature, each containing at least
#'   the column `age` used to determine the maximum age for prediction.
#' @param coefficientForMassAforMature Numeric. Coefficient \( a \) used in the allometric equation
#'   for converting predicted body length to dry mass for mature individuals.
#' @param scaleForMassBforMature Numeric. Exponent \( b \) in the allometric equation
#'   for converting predicted body length to dry mass for mature individuals.
#'
#' @return
#' A list of lists, where each element contains:
#' \describe{
#'   \item{temperature}{Numeric. Temperature corresponding to the model.}
#'   \item{dryMass}{Numeric. Predicted dry mass of mature individuals at that temperature.}
#' }
#'
#' @details
#' The function applies the allometric relationship:
#' \deqn{M = a \cdot L^b}
#' where:
#' \itemize{
#'   \item \( M \) is the predicted dry mass,
#'   \item \( L \) is the predicted body length at the maximum observed age,
#'   \item \( a \) and \( b \) are the species-specific coefficients provided as arguments.
#' }
#'
#' This approach allows comparing how adult body size varies with temperature, a key aspect of
#' the **Temperature–Size Rule (TSR)**, which typically predicts smaller body sizes at higher temperatures.
#'
#' @examples
#' # Example with two temperatures
#' data_list <- list(
#'   "10" = data.frame(age = c(1, 2, 3, 4)),
#'   "20" = data.frame(age = c(1, 2, 3, 4))
#' )
#'
#' models <- list(
#'   "10" = list(best_model = lm(length ~ age, data = data.frame(age = 1:4, length = c(2, 4, 6, 8)))),
#'   "20" = list(best_model = lm(length ~ age, data = data.frame(age = 1:4, length = c(1.5, 3, 4.5, 6))))
#' )
#'
#' calculateTSR(
#'   models = models,
#'   data = data_list,
#'   coefficientForMassAforMature = 0.003,
#'   scaleForMassBforMature = 2.7
#' )
#'
#' @export
calculateTSR <- function(models, data, coefficientForMassAforMature, scaleForMassBforMature) {
    
    # Apply over all temperature models
    TSR = lapply(names(models), function(temp) {
        
        # Predict the final (maximum age) body length using the best model for the given temperature
        predicted_length <- predict(models[[temp]]$best_model, newdata = 
                            data.frame(age = max(data[[temp]]$age)))

        # Convert predicted length to dry mass using the allometric equation
        predicted_dryMass<-coefficientForMassAforMature*predicted_length^scaleForMassBforMature

        # Return a structured list with temperature and corresponding predicted dry mass
        return(list(
            temperature=as.numeric(temp),
            dryMass=predicted_dryMass
        ))
    })

    # Return TSR as a list of temperature–dry mass pairs
    return(TSR)
}
