#' Calculate Dry Mass From Body Length
#'
#' Computes the organism’s dry mass based on a length–mass allometric
#' relationship of the form:
#' \deqn{M = a \cdot L^{b}}
#'
#' @param length Numeric.  
#'   Body length of the organism.
#' @param coefficient Numeric.  
#'   Allometric coefficient \(a\) controlling the scaling of mass.
#' @param scale Numeric.  
#'   Allometric exponent \(b\) that determines how mass scales with length.
#'
#' @return Numeric.  
#'   Estimated dry mass corresponding to the given length.
#'
#' @details
#' This function applies a simple power-law relationship between length and dry
#' mass. Species-specific or instar-specific coefficients must be provided.
#'
#' @examples
#' calculateDryMass(length = 5, coefficient = 0.002, scale = 3)
#'
calculateDryMass<-function(length, coefficient, scale)
{
	return(coefficient*length^scale)
} 


#' Calculate Body Length From Dry Mass
#'
#' Computes the organism's body length from a known dry mass using the inverse
#' of the length–mass allometric function:
#' \deqn{L = \left( \frac{M}{a} \right)^{1/b}}
#'
#' @param M Numeric.  
#'   Dry mass of the organism.
#' @param coefficient Numeric.  
#'   Allometric coefficient \(a\).
#' @param scale Numeric.  
#'   Allometric exponent \(b\).
#'
#' @return Numeric.  
#'   Estimated body length.
#'
#' @details
#' This function inverts the power-law length–mass relationship commonly
#' used in ecological and physiological modelling.
#'
#' @examples
#' calculateDryLength(M = 0.5, coefficient = 0.002, scale = 3)
#'
calculateDryLength<-function(M, coefficient, scale)
{
	return((M/coefficient)^(1/scale))
}


#' Convert Dry Mass to Wet Mass
#'
#' Converts dry mass to wet mass using a constant conversion factor.
#'
#' @param dryMass Numeric.  
#'   Dry mass of the organism.
#' @param conversionToWetMass Numeric.  
#'   Conversion factor applied to transform dry mass to wet mass.
#'
#' @return Numeric.  
#'   Estimated wet mass.
#'
#' @details
#' The conversion factor is typically species-specific and may come from
#' empirical measurements or literature values.
#'
#' @examples
#' calculateWetMass(dryMass = 0.1, conversionToWetMass = 4.5)
#'
calculateWetMass<-function(dryMass, conversionToWetMass)
{
    return(dryMass*conversionToWetMass)
}
