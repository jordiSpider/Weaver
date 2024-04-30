
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
	double result;
	double newActivationEnergy = activationEnergy; //Serán las energias de activación de search, vor, speed
	double c = 1;
	double k = BOLTZMANN;

	//TODO 270 por qué coge ese valor a veces?

	result = c*exp(-newActivationEnergy/(k*temperature.getTemperatureKelvin()))/(1+exp((-1/(k*temperature.getTemperatureKelvin()))*(ED-((ED/Topt.getTemperatureKelvin())+(k*log(newActivationEnergy/(ED-newActivationEnergy))))*temperature.getTemperatureKelvin())));

	if(::isnan(newActivationEnergy))
	{
		cout << "ACTIVATION ENERGY NAN - CHECK IF ED>E" << endl;
	}

	if(::isnan(result))
	{
		cout << "ACTIVATION ENERGY NAN - CHECK IF ED>E" << endl;
	}
	
	result=result*1000000000000;

	return result;
}

double MathFunctions::useJohnsonLewis(Temperature newT, double activationEnergy, Temperature lowerThreshold, Temperature upperThreshold, Temperature Topt, double Ed, double trait) 
{
	double dellsNewT = johnsonLewis(newT, Topt, Ed, activationEnergy);

    double dellsMaxT = johnsonLewis(Topt, Topt, Ed, activationEnergy);

	double postDell;

    if(newT > Topt)
	{
	    double dellsMinT = johnsonLewis(upperThreshold, Topt, Ed, activationEnergy);
		postDell = linearInterpolate(dellsNewT, dellsMinT, dellsMaxT, 0, trait);
	}
	else
	{
		double dellsMinT = johnsonLewis(lowerThreshold, Topt, Ed, activationEnergy);
		postDell = linearInterpolate(dellsNewT, dellsMinT, dellsMaxT, 0, trait);
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

/**
 * Cholesky decomposition of matrix A
 */
vector<vector<double> > MathFunctions::cholesky(double LinfKcorr)
{
	vector<vector<double>> preCholMat(2, vector<double>(2));
	preCholMat[0][0] = 1;
	preCholMat[0][1] = LinfKcorr;
	preCholMat[1][0] = LinfKcorr;
	preCholMat[1][1] = 1;

	//cout << "preChol = " << preCholMat[0][0] << " - " << preCholMat[0][1] << " - " << preCholMat[1][0] << " - " << preCholMat[1][1] << endl;

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
