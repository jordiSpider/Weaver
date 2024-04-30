/*
######### Odín Moron Garcia 22/05/2023 ######
Implementation of a Normal distribution function as a class with two constructors, one providing directly the parameters mu and sigma and a function to evaluate at any x and another to generate the same object after providing a minimum and a máximum.

The rationale is that we will use this curve and others to estimate temperature and relative humidity related values.
At the moment, that functionality is calculated from a cuadratic equation with negative a.
*/

/*  Some references 
 *  PI and other numbers
 * https://www.quantstart.com/articles/Mathematical-Constants-in-C/ 
 *  */

#include "NormalDist.h"

// FUNCTION CODE
double dnorm(double x,double mu,double sigma){
	// INPUT : to calculate the probability density function of a normal distribution we need three parameters namely
	// two "parameters" or "statistics" named mu (the distribution mean) and sigma (the distribution standard distribution)
	// a value to calculate its probability (it could be design as an array but now it is just a value)
	// TESTS : standard deviation cannot be by definition smaller than 0 and given sigma divides on the normalization constant can´t be 0 neither
	// NOTES : So far this function works only with double - should it be polymorphic?
	//
	if( sigma <= 0 ) {
		throw "Normal Distribution PDF function - sigma has a 0 value! Returning a 0 probability value";
	}
	// assert(sigma !=0); may be another way if we don´t handle the exception
	//
	double normalizationConstant =(1/sigma) *  M_SQRT1_2 * std::sqrt(M_1_PI);
	double exponent = - 0.5 * pow((x - mu)/sigma,2);
	double distributionValue = std::pow(M_E,exponent);

	return (normalizationConstant * distributionValue);
}
// CLASSES 
//
/* NORMAL DISTRIBUTION PROBABILITY DENSITY FUNCTION */

NormalDistributionPDF::NormalDistributionPDF(){};
NormalDistributionPDF::NormalDistributionPDF(double mu,double sigma){
	try{set_parameters(mu,sigma);}catch(const char* msg ){std::cerr << msg << std::endl;exit(-1);};
}

void NormalDistributionPDF::setParametersFromRange(double min,double max){

	// this is an equation improvised (22/03/2023) to obtain a normal distribution using a range, min - max
	// supposing a normal distribution, hence symmetric, the mean should be easily calculated using the midpoint
	// yet, it is not very clear (to me) what rule use to extract the standard deviation.
	// assuming a Standard Normal Distribution, 1*sigma correspond to the ~64% of the values, 3*sigma to the 99.7% of the population
	// similarly, there´s rules that say that the IQR should be at 1.35 * sigma and 95% at 1.96*sigma.
	// at the moment I just assume that 3*sigma=1/2 *  Range - for the standard Normal, the range would be [-3,3] hence a distance of 6, corresponding to 3 sigma, and then sigma would be = 1
	const int    Nsigmas = 3;
	const double Range = max - min;
	const double midpoint = (min + max)*0.5;
	const double sd = (max - min)/(2*Nsigmas);
	set_parameters(midpoint,sd);
};


void NormalDistributionPDF::set_sigma(double sigma){
	try{check_sigma(sigma);}catch(const char* msg ){std::cerr << msg << std::endl;exit(-1);};
	this->sigma = sigma;	
}
void NormalDistributionPDF::set_mu(double mu){
	this->mu = mu;
}

double NormalDistributionPDF::get_mu(){return this->mu;}
double NormalDistributionPDF::get_sigma(){return this->sigma;}

void NormalDistributionPDF::set_parameters(double mu,double sigma){
	set_mu(mu);
	set_sigma(sigma);
}

double NormalDistributionPDF::evaluate(double x){

	double normalizationConstant =(1/sigma) *  M_SQRT1_2 * std::sqrt(M_1_PI);
	double exponent = - 0.5 * pow((x - mu)/sigma,2);
	double distributionValue = std::pow(M_E,exponent);

	return (normalizationConstant * distributionValue);
}


double NormalDistributionPDF::evaluate(double x,double mu, double sigma){
	check_sigma(sigma);

	double normalizationConstant =(1/sigma) *  M_SQRT1_2 * std::sqrt(M_1_PI);
	double exponent = - 0.5 * pow((x - mu)/sigma,2);
	double distributionValue = std::pow(M_E,exponent);

	return (normalizationConstant * distributionValue);
}

void NormalDistributionPDF::check_sigma(double sigma){
	if( sigma <= 0 ) {
		throw "Normal Distribution PDF function - sigma has a 0 value! Returning a 0 probability value";
	}
}

