#' Linear Interpolation Functions
#'
#' Provides simple linear interpolation utilities for both vectors and scalar
#' values. These functions are useful for rescaling values from one range to another,
#' commonly used in normalization and parameter transformation tasks.
#'
#' @details
#' - `interpol()` performs linear interpolation for a numeric vector `x`,
#'   automatically using `min(x)` and `max(x)` as the source range.
#' - `interpol2()` performs linear interpolation for a single numeric value `x`,
#'   using explicit source and target bounds.
#'
#' Both functions apply the standard linear transformation:
#' \deqn{y = y_{min} + (x - x_{min}) \times \frac{(y_{max} - y_{min})}{(x_{max} - x_{min})}}
#'
#' @name math_functions
NULL


#' Interpolation for Numeric Vectors
#'
#' Rescales a numeric vector linearly to a new range \code{[miny, maxy]}.  
#' The input range is inferred automatically from \code{min(x)} and \code{max(x)}.
#'
#' @param x Numeric vector to interpolate.
#' @param miny Numeric. Minimum value of the target range.
#' @param maxy Numeric. Maximum value of the target range.
#'
#' @return A numeric vector of the same length as \code{x}, scaled to the new range.
#'
#' @examples
#' interpol(c(1, 2, 3, 4), miny = 0, maxy = 1)
#'
#' @export
interpol<-function(x,miny,maxy)
{
	y=miny+((x-min(x))*((maxy-miny)/(max(x)-min(x))))
	return(y)
}


#' Interpolation for Single Values
#'
#' Rescales a scalar value from a known range \code{[minx, maxx]} to a new range
#' \code{[miny, maxy]} using linear interpolation.
#'
#' @param x Numeric value to interpolate.
#' @param minx Numeric. Minimum of the input range.
#' @param maxx Numeric. Maximum of the input range.
#' @param miny Numeric. Minimum of the target range.
#' @param maxy Numeric. Maximum of the target range.
#'
#' @return A single numeric value scaled to the target range.
#'
#' @examples
#' interpol2(5, minx = 0, maxx = 10, miny = 100, maxy = 200)
#'
#' @export
interpol2<-function(x,minx,maxx,miny,maxy)
{
	y=miny+((x-minx)*((maxy-miny)/(maxx-minx)))
	return(y)
}
