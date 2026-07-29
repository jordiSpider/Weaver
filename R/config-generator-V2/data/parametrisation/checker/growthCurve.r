#' Calculate Growth Curve of an Individual
#'
#' Computes the length of an individual over time using various growth models.
#'
#' @param Lmax Numeric. Maximum length the individual can reach.
#' @param k Numeric. Growth rate coefficient.
#' @param t Numeric. Time vector at which to calculate the length.
#' @param tmax Numeric. Maximum time for growth calculation.
#' @param eggDryMass Numeric. Dry mass of the egg at birth.
#' @param A Numeric. Lower asymptote for Logistic4P model.
#' @param type Character. Growth model type. Options:
#'   "Linear", "VonBertalanffy", "Logistic", "Logistic4P", "Exponential".
#' @param coefficientForMassA Numeric. Coefficient to convert length to dry mass.
#' @param scaleForMassB Numeric. Exponent to convert length to dry mass.
#'
#' @return Numeric vector of predicted lengths at times `t`.
#'
#' @details
#' This function calculates the growth curve of an individual given its birth size
#' and growth parameters. The dry length at birth is computed from the egg's dry mass.
#'
#' The available growth models are:
#' 
#' - **Linear**: `L = lengthAtBirth + k*t`
#' - **VonBertalanffy**: `L = Linf * (1 - exp(-k*(t-t0)))`, where `Linf` and `t0` are derived to fit `Lmax` at `tmax`.
#' - **Logistic**: `L = Linf / (1 + exp(k*(xmid - t)))`, with `xmid` and `Linf` derived to fit `lengthAtBirth` and `Lmax`.
#' - **Logistic4P**: Four-parameter logistic model: `L = A + (B - A) / (1 + exp(k*(xmid - t)))`.
#' - **Exponential**: `L = exp(k*t + aExpo) + bExpo`, fitted to match `lengthAtBirth` and `Lmax`.
#'
#' @examples
#' # Linear growth example
#' growthCurve(Lmax=10, k=0.2, t=0:10, tmax=10, eggDryMass=0.01, type="Linear",
#'             coefficientForMassA=0.1, scaleForMassB=0.75)
#'
#' # Von Bertalanffy growth example
#' growthCurve(Lmax=15, k=0.3, t=0:10, tmax=10, eggDryMass=0.01, type="VonBertalanffy",
#'             coefficientForMassA=0.1, scaleForMassB=0.75)
#'
#' @export
growthCurve<-function(Lmax = NULL,k = NULL,t = NULL, tmax = NULL, eggDryMass = NULL, 
	A = NULL, type = NULL, coefficientForMassA = NULL, scaleForMassB = NULL)
{
	# Calculate the dry length at birth from the egg dry mass
	lengthAtBirth = calculateDryLength(eggDryMass, coefficientForMassA, scaleForMassB)
	
	if(type == "Linear"){
		# Simple linear growth: length increases proportionally to time
		L = lengthAtBirth + k*t
	}

	if(type == "VonBertalanffy"){
		# Von Bertalanffy growth: asymptotic approach to maximum length
		Linf = (Lmax*exp(k*tmax)-lengthAtBirth)/(exp(k*tmax)-1)		       
		t0 = log(1-(lengthAtBirth/Linf))/k
		L = Linf*(1-exp(-k*(t-t0)))
	}

	if(type == "Logistic"){
		# Logistic growth: S-shaped curve
		Linf = (Lmax*lengthAtBirth*(exp(k*tmax)-1))/(lengthAtBirth*exp(k*tmax)-Lmax)
		xmid = log((Linf-lengthAtBirth)/lengthAtBirth)*(1/k)
		L = Linf/(1+exp((xmid-t)*k))
	}
	
	if(type == "Logistic4P"){
		# Four-parameter logistic growth model
		numerator = Lmax - lengthAtBirth
		denominator = (lengthAtBirth - A) - (Lmax - A) * exp(-tmax * k)
		xmid = (1.0 / k) * log(numerator / denominator)
		B = (lengthAtBirth - A)*(1+exp(xmid*k)) + A
		L = A + (B - A)/(1+exp((xmid - t)*k))
	}
	
	if(type == "Exponential"){
		# Exponential growth: length grows exponentially over time
		aExpo = log((Lmax-lengthAtBirth)/(exp(k*tmax)-1))
		bExpo = lengthAtBirth-exp(aExpo)
		L = exp(k*t + aExpo) + bExpo  
	}

	return(L)
}
