#' Maximum Voracity Based on Yodzis & Innes (1992)
#'
#' Estimates the maximum amount of food consumed (wet mass) by an organism
#' based on its fresh body mass using the allometric relationship described
#' by Yodzis & Innes (1992).
#'
#' The formula used is:
#' \deqn{F = a \cdot M^b}
#'
#' @param M Numeric. Fresh body mass of the organism (in mg).
#' @param YodzisA Numeric. Allometric coefficient \eqn{a}.  
#'   Default is `0.1`.
#' @param YodzisB Numeric. Allometric exponent \eqn{b}.  
#'   Default is `0.75`.
#'
#' @return Numeric. Estimated wet food intake (in mg).
#'
#' @details
#' This function provides a simple allometric model for maximum food intake,
#' widely used in ecosystem and trophic modelling.
#'
#' @examples
#' yodzis(5)              # Default coefficients
#' yodzis(5, 0.12, 0.8)   # Custom parameters
yodzis<-function(M,YodzisA=0.1,YodzisB=0.75) 
{
	wetFood<-YodzisA*M^YodzisB 
	
	return(wetFood)
}


#' Maximum Voracity Based on Garland (1983)
#'
#' Estimates the maximum food intake (wet mass) using the allometric
#' relationship derived from Garland (1983).
#'
#' The formula used is:
#' \deqn{F = 0.152 \cdot M^{0.738}}
#'
#' @param M Numeric. Fresh body mass of the organism (in kg).
#'
#' @return Numeric. Estimated wet food intake (in kg).
#'
#' @details
#' The original coefficient in Garland (1983) was 152 with output in grams.
#' Here it has been rescaled to `0.152` so that both input and output are
#' expressed in kilograms.
#'
#' @examples
#' garland(0.002)   # For a 2 g organism (0.002 kg)
garland<-function(M) 
{
	wetFood=0.152*M^0.738

	return(wetFood)
}
