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

// PREAMBLE 
#ifndef NORMALDIST_H_
#define NORMALDIST_H_


// FUNCTIONS DEFINITION
double dnorm(double x,double mu,double sigma);

// CLASS DEFINITION

class NormalDistributionPDF {

	public :
		NormalDistributionPDF();
		NormalDistributionPDF(double mu,double sigma);

		void setParametersFromRange(double min,double max);

		
		void set_sigma(double sigma);
		void set_mu(double mu);

		double get_sigma();
		double get_mu();

		void set_parameters(double mu,double sigma);
		double evaluate(double x);
		double evaluate(double x,double mu, double sigma);

	private : 
		float mu;
		float sigma;
		void check_sigma(double sigma);

};

#endif // NORMALDIST_H_
