
#include <iostream>
#include <cmath>
#include "IBM/Maths/MathFunctions.h"
#include "IBM/Maths/Constants.h"

using namespace std;

/**
 * Interpolation for values.
 * @param x
 * @param min_x
 * @param max_x
 * @param min_y
 * @param max_y
 * @return
 */

double MathFunctions::johnsonLewis(Temperature temperature, Temperature Topt, double ED, double activationEnergy) 
{
	double c = 1;

	//TODO 270 por qué coge ese valor a veces?

	double result = c*exp(-activationEnergy/(BOLTZMANN*temperature.getTemperatureKelvin()))/(1+exp((-1/(BOLTZMANN*temperature.getTemperatureKelvin()))*(ED-((ED/Topt.getTemperatureKelvin())+(BOLTZMANN*log(activationEnergy/(ED-activationEnergy))))*temperature.getTemperatureKelvin())));

	if(::isnan(activationEnergy))
	{
		cout << "ACTIVATION ENERGY NAN - CHECK IF ED>E" << endl;
	}

	if(::isnan(result))
	{
		cout << "ACTIVATION ENERGY NAN - CHECK IF ED>E" << endl;
	}

	return result;
}

double MathFunctions::useJohnsonLewis(const Temperature& newT, const double& activationEnergy, const Temperature& lowerThreshold, const Temperature& upperThreshold, const Temperature& Topt, const double& Ed, const double& traitAtTopt) 
{
	double dellsNewT = johnsonLewis(newT, Topt, Ed, activationEnergy);

    double dellsMaxT = johnsonLewis(Topt, Topt, Ed, activationEnergy);

	double postDell;

    if(newT > Topt)
	{
	    double dellsMinT = johnsonLewis(upperThreshold, Topt, Ed, activationEnergy);
		postDell = linearInterpolate(dellsNewT, dellsMinT, dellsMaxT, 0, traitAtTopt);
	}
	else
	{
		double dellsMinT = johnsonLewis(lowerThreshold, Topt, Ed, activationEnergy);
		postDell = linearInterpolate(dellsNewT, dellsMinT, dellsMaxT, 0, traitAtTopt);
	}

	return postDell;
}

double MathFunctions::linearInterpolate(double x, double min_x, double max_x, double min_y, double max_y)
{
	double result = 0.0;

	if(max_x != min_x) {
		result = min_y + ( (x - min_x) * ((max_y - min_y)/(max_x - min_x)) );
	}

	return result;
}

double MathFunctions::linearInterpolate01(double x, double min_x, double max_x)
{
	return linearInterpolate(x, min_x, max_x, 0, 1);
}

double MathFunctions::linearInterpolate01(double x, double max_x)
{
	return x / max_x;
}

double MathFunctions::linearExtrapolation(double x, double min_x, double max_x, double interpolationResult)
{
	double max_y = 0.0;

	if(max_x != min_x) {
		max_y = interpolationResult*(-max_x + min_x)/(min_x - x);
	}
	
	return max_y;
}


/**
 * Cholesky decomposition of matrix A
 */
vector<vector<double> > MathFunctions::cholesky(const vector<double>& sigmaValues)
{
	vector<vector<double>> preCholMat(sigmaValues.size(), vector<double>(sigmaValues.size(), 0.0));
	
	for(unsigned int i = 0; i < sigmaValues.size(); i++)
	{
		preCholMat[i][i] = 1.0;

		for(unsigned int j = i+1; j < sigmaValues.size(); j++)
		{
			preCholMat[i][j] = sigmaValues[j+i-1];
			preCholMat[j][i] = sigmaValues[j+i-1];
		}
	}
	
	
	int n = preCholMat.size();
	double sum1 = 0.0;
	double sum2 = 0.0;
	double sum3 = 0.0;
	vector<vector<double> > l(n, vector<double> (n));
	l[0][0] = sqrt(preCholMat[0][0]);
	for (int j = 1; j <= n-1; j++) {
		l[j][0] = preCholMat[j][0]/l[0][0];
	}

	for (int i = 1; i <= (n-2); i++)
	{
		sum1 = 0.0;
		for (int k = 0; k <= (i-1); k++) {
			sum1 += pow(l[i][k], 2);
		}
		l[i][i]= sqrt(preCholMat[i][i]-sum1);

		for (int j = (i+1); j <= (n-1); j++)
		{
			sum2 = 0.0;
			for (int k = 0; k <= (i-1); k++) {
				sum2 += l[j][k]*l[i][k];
			}
			l[j][i]= (preCholMat[j][i]-sum2)/l[i][i];
		}
	}

	for (int k = 0; k <= (n-2); k++) {
		sum3 += pow(l[n-1][k], 2);
	}
	l[n-1][n-1] = sqrt(preCholMat[n-1][n-1]-sum3);

	//cout << "resultChol = " << l[0][0] << " - " << l[0][1] << " - " << l[1][0] << " - " << l[1][1] << endl;

	return l;
}
