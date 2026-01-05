
#include "App/Model/IBM/Maths/MathFunctions.h"


using namespace std;



PreciseDouble MathFunctions::Pawar_2018(const Temperature& temperature, const PreciseDouble& traitAtTopt, const Temperature& Topt, const PreciseDouble& ED, const PreciseDouble& E, const Temperature& Tref)
{
	PreciseDouble traitAtTempRef = traitAtTopt / (exp(-E * ((1/(BOLTZMANN*(Topt.getTemperatureKelvin()))) - (1/(BOLTZMANN*(Tref.getTemperatureKelvin()))))) /
		(1 + (E / (ED - E)) * exp((ED /BOLTZMANN)* ((1/(Topt.getTemperatureKelvin())) - (1/(Topt.getTemperatureKelvin()))))));

	PreciseDouble trait = traitAtTempRef * exp(-E * ((1/(BOLTZMANN*(temperature.getTemperatureKelvin()))) - (1/(BOLTZMANN*(Tref.getTemperatureKelvin()))))) /
		(1 + (E / (ED - E)) * exp((ED /BOLTZMANN)* ((1/(Topt.getTemperatureKelvin())) - (1/temperature.getTemperatureKelvin()))));
	
	return trait;
}

PreciseDouble MathFunctions::preprocessTraitForPawar(const PreciseDouble& traitAtTopt, const bool inverse, const bool strictlyPositive)
{
	PreciseDouble processedTraitAtTopt = traitAtTopt;

	if(!strictlyPositive){
		if(processedTraitAtTopt < 0.0){
			processedTraitAtTopt = fabs(processedTraitAtTopt);
		}
	}

	if(inverse){
		processedTraitAtTopt = 1.0/processedTraitAtTopt;
	}

	return processedTraitAtTopt;
}

PreciseDouble MathFunctions::postprocessTraitForPawar(const PreciseDouble& traitAtTemp, const PreciseDouble& traitAtTopt, const bool inverse, const bool strictlyPositive)
{
	PreciseDouble trait = traitAtTemp;

	if(inverse){
		trait = 1.0/trait;
	}

	if(!strictlyPositive){
		if(traitAtTopt < 0.0){
			trait = trait - 2*fabs(traitAtTopt);
		}
	}

	return trait;
}

PreciseDouble MathFunctions::use_Pawar_2018(const Temperature& temperature, const PreciseDouble& traitAtTopt, const Temperature& Topt, const PreciseDouble& ED, const PreciseDouble& E, const Temperature& Tref, const bool inverse, const bool strictlyPositive, const PreciseDouble& propOftraitAtTopt)
{
	PreciseDouble processedTraitAtTopt = preprocessTraitForPawar(traitAtTopt, inverse, strictlyPositive);

	PreciseDouble nextToZeroTrait = propOftraitAtTopt*processedTraitAtTopt;

	PreciseDouble trait = MathFunctions::Pawar_2018(temperature, processedTraitAtTopt, Topt, ED, E, Tref);
	
	if(trait < nextToZeroTrait){
		trait = nextToZeroTrait;
	}

	return postprocessTraitForPawar(trait, traitAtTopt, inverse, strictlyPositive);
}

Temperature MathFunctions::bisection(const std::pair<Temperature, Temperature>& searchRange, const PreciseDouble& traitAtTopt, const Temperature& Topt, const PreciseDouble& ED, const PreciseDouble& E, const Temperature& Tref, const PreciseDouble& nextToZeroTrait, PreciseDouble tol, unsigned int max_iter) {
	
	Temperature a = searchRange.first;
	Temperature b = searchRange.second;
	
	PreciseDouble fa = MathFunctions::Pawar_2018(a, traitAtTopt, Topt, ED, E, Tref) - nextToZeroTrait;
	PreciseDouble fb = MathFunctions::Pawar_2018(b, traitAtTopt, Topt, ED, E, Tref) - nextToZeroTrait;

	if(fa * fb > 0) {
		throwLineInfoException("Target out of range.");
	}

	Temperature c;

	for (unsigned int i = 0; i < max_iter; ++i) {
		c = (a + b) * 0.5;

		PreciseDouble fc = MathFunctions::Pawar_2018(c, traitAtTopt, Topt, ED, E, Tref) - nextToZeroTrait;

		if (fabs(fc) < tol || (b.getTemperatureKelvin() - a.getTemperatureKelvin()) < tol)
			return c;

		if (fa * fc < 0) {
			b = c; fb = fc;
		} else {
			a = c; fa = fc;
		}
	}

	return c;
}

std::pair<Temperature,Temperature> MathFunctions::TemperatureRangePawar_2018(const PreciseDouble& traitAtTopt, const Temperature& Topt, const PreciseDouble& ED, const PreciseDouble& E, const Temperature& Tref, const PreciseDouble& nextToZeroTrait)
{
	// Se fijan los rango de búsqueda iniciales
	pair<Temperature, Temperature> Tmin_range;
	pair<Temperature, Temperature> Tmax_range;

	Tmin_range.first = Topt - Temperature(10.0);
	Tmin_range.second = Topt;

	Tmax_range.first = Topt;
	Tmax_range.second = Topt + Temperature(10.0);


	Temperature Tmin;

	bool Tmin_found = false;

	while(!Tmin_found)
	{
		try
		{
			Tmin = MathFunctions::bisection(Tmin_range, traitAtTopt, Topt, ED, E, Tref, nextToZeroTrait);
			Tmin_found = true;
		}
		catch(const LineInfoException&)
		{
			// Se amplía el rango de búsqueda al doble
			Tmin_range.first = Tmin_range.first - (Tmin_range.second - Tmin_range.first);
		}
	}


	Temperature Tmax;

	bool Tmax_found = false;

	while(!Tmax_found)
	{
		try
		{
			Tmax = MathFunctions::bisection(Tmax_range, traitAtTopt, Topt, ED, E, Tref, nextToZeroTrait);
			Tmax_found = true;
		}
		catch(const LineInfoException&)
		{
			// Se amplía el rango de búsqueda al doble
			Tmax_range.second = Tmax_range.second + (Tmax_range.second - Tmax_range.first);
		}
	}


	return make_pair(Tmin, Tmax); 
}

std::pair<Temperature,Temperature> MathFunctions::calculateTemperatureRangePawar(const PreciseDouble& traitAtTopt, const Temperature& Topt, const PreciseDouble& ED, const PreciseDouble& E, const Temperature& Tref, const bool inverse, const bool strictlyPositive, const PreciseDouble& propOftraitAtTopt)
{
	PreciseDouble processedTraitAtTopt = preprocessTraitForPawar(traitAtTopt, inverse, strictlyPositive);

	PreciseDouble nextToZeroTrait = propOftraitAtTopt*processedTraitAtTopt;

	return MathFunctions::TemperatureRangePawar_2018(processedTraitAtTopt, Topt, ED, E, Tref, nextToZeroTrait);
}

Length MathFunctions::TSR(const Temperature& temperature, const std::map<Temperature, DryMass>& tempSizeRuleVector, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature)
{
	auto firstPoint = tempSizeRuleVector.cbegin();

	auto secondPoint = firstPoint;
	++secondPoint;

	auto nextPoint = secondPoint;
	++nextPoint;

	while(nextPoint != tempSizeRuleVector.cend())
	{
		if(temperature <= secondPoint->first)
		{
			DryMass postTSRMass(
				MathFunctions::linearInterpolate(
					temperature.getTemperatureCelsius(), 
					firstPoint->first.getTemperatureCelsius(), secondPoint->first.getTemperatureCelsius(), 
					firstPoint->second.getValue(), secondPoint->second.getValue()
				)
			);

			return Length(postTSRMass, coefficientForMassAforMature, scaleForMassBforMature);
		}
		else
		{
			firstPoint = secondPoint;
			secondPoint = nextPoint;
			++nextPoint;
		}
	}

	DryMass postTSRMass(
		MathFunctions::linearInterpolate(
			temperature.getTemperatureCelsius(), 
			firstPoint->first.getTemperatureCelsius(), secondPoint->first.getTemperatureCelsius(), 
			firstPoint->second.getValue(), secondPoint->second.getValue()
		)
	);

	return Length(postTSRMass, coefficientForMassAforMature, scaleForMassBforMature);
}

Length MathFunctions::use_TSR(const Temperature& temperature, const std::map<Temperature, DryMass>& tempSizeRuleVector, const PreciseDouble &coefficientForMassAforMature, const PreciseDouble &scaleForMassBforMature, const Length& traitAtTempRef, const Temperature& Tref)
{
	Length traitAtTempRefWithoutShift = MathFunctions::TSR(Tref, tempSizeRuleVector, coefficientForMassAforMature, scaleForMassBforMature);

	PreciseDouble geneticShift = traitAtTempRef.getValue() / traitAtTempRefWithoutShift.getValue();

	std::map<Temperature, DryMass> tempSizeRuleVectorWithShift;

	for (const auto& [temp, dryMass] : tempSizeRuleVector) {
		tempSizeRuleVectorWithShift[temp] = DryMass(dryMass.getValue() * geneticShift);
	}

	return MathFunctions::TSR(temperature, tempSizeRuleVectorWithShift, coefficientForMassAforMature, scaleForMassBforMature);
}

PreciseDouble MathFunctions::linearInterpolate(const PreciseDouble& x, const PreciseDouble& min_x, const PreciseDouble& max_x, const PreciseDouble& min_y, const PreciseDouble& max_y)
{
	PreciseDouble result = 0.0;

	if(max_x != min_x) {
		result = min_y + (x - min_x) * (max_y - min_y) / (max_x - min_x);
	}

	return result;
}


/**
 * Cholesky decomposition of matrix A
 */
vector<vector<PreciseDouble> > MathFunctions::cholesky(const vector<PreciseDouble>& sigmaValues)
{
	vector<vector<PreciseDouble>> preCholMat(sigmaValues.size(), vector<PreciseDouble>(sigmaValues.size(), 0.0));
	
	for(unsigned int i = 0; i < sigmaValues.size(); i++)
	{
		preCholMat[i][i] = 1.0;

		for(unsigned int j = i+1; j < sigmaValues.size(); j++)
		{
			preCholMat[i][j] = sigmaValues[j+i-1];
			preCholMat[j][i] = sigmaValues[j+i-1];
		}
	}
	
	
	size_t n = preCholMat.size();
	PreciseDouble sum1 = 0.0;
	PreciseDouble sum2 = 0.0;
	PreciseDouble sum3 = 0.0;
	vector<vector<PreciseDouble> > l(n, vector<PreciseDouble> (n));
	l[0][0] = sqrt(preCholMat[0][0]);
	for (unsigned int j = 1; j <= n-1; j++) {
		l[j][0] = preCholMat[j][0]/l[0][0];
	}

	for (unsigned int i = 1; i <= (n-2); i++)
	{
		sum1 = 0.0;
		for (unsigned int k = 0; k <= (i-1); k++) {
			sum1 += pow(l[i][k], 2);
		}
		l[i][i]= sqrt(preCholMat[i][i]-sum1);

		for (unsigned int j = (i+1); j <= (n-1); j++)
		{
			sum2 = 0.0;
			for (unsigned int k = 0; k <= (i-1); k++) {
				sum2 += l[j][k]*l[i][k];
			}
			l[j][i]= (preCholMat[j][i]-sum2)/l[i][i];
		}
	}

	for (unsigned int k = 0; k <= (n-2); k++) {
		sum3 += pow(l[n-1][k], 2);
	}
	l[n-1][n-1] = sqrt(preCholMat[n-1][n-1]-sum3);

	return l;
}
