/*
 * gaussian_distribution.h
 *
 *  Created on: Jul 31, 2012
 *      Author: jrbcast
 */

#ifndef GAUSSIAN3D_H_
#define GAUSSIAN3D_H_

#include <math.h>
#include <vector>
#include <iostream>
#include "Constants.h"
#include "Types.h"

class Gaussian1D {
protected:
	float sigma;
	float amplitude;
	float center;

	// Speed-up
	bool precompute;
	unsigned int precomputeBins;		// Number of precomputed items
	unsigned int precomputeDim; 		// Dimension (radius) of the gaussian to be precomputed (from its center)
	std::vector<double> precomputed;	// Precomputed values are stored here
	double precomputeBinWidth;			// Size of each bin. Note this is stored inverted to speedup memory accesses

	// Just to speed-up things, precalculate this each time you
	// change sigma value.
	double inverseSigma;
public:
	Gaussian1D();
	Gaussian1D(float newAmplitude, float newSigma, float newCenter);
	virtual ~Gaussian1D();

	float getSigma();
	void setSigma(float newValue);

	bool isPrecomputed();
	void setPrecomputed(bool doPrecompute);

	float getAmplitude();
	void setAmplitude(float newValue);

	float getCenter();

	double getValueAtDistance(float distance);
	double getValueAtPoint(float point);
};

class IsotropicGaussian3D {
protected:
	double sigma;
	double amplitude;
	fPoint3D center;

	// Just to speed-up things, precalculate this each time you
	// change sigma value.
	double inverseSquaredSigma;
public:
	IsotropicGaussian3D();
	IsotropicGaussian3D(float newAmplitude, float newSigma, fPoint3D newCenter);
	IsotropicGaussian3D(float newAmplitude, float newSigma, float newCenterX, float newCenterY, float newCenterZ);
	virtual ~IsotropicGaussian3D();

	double getSigma() { return sigma; };
	void setSigma(float newValue);

	double getAmplitude() { return amplitude; };
	void setAmplitude(float newValue);

	fPoint3D getCenter() { return center; };

	double getValueAtDistance(float xDistance, float yDistance, float zDistance);
	double getValueAtPoint(float xPoint, float yPoint, float zPoint);

	double getValueAtDistance(fVector3D distance );
	double getValueAtPoint(fPoint3D point);

};

class AnisotropicGaussian3D {
protected:
	float sigmaWidth;
	float sigmaLength;
	float sigmaDepth;
	float amplitude;
	fPoint3D center;

	// Just to speed-up things, precalculate this each time you
	// change sigma value.
	double inverseSquaredSigmaWidth;
	double inverseSquaredSigmaLength;
	double inverseSquaredSigmaDepth;
public:
	AnisotropicGaussian3D();
	AnisotropicGaussian3D(float newAmplitude, float newSigmaWidth, float newSigmaLength, float newSigmadepth, fPoint3D newCenter);
	AnisotropicGaussian3D(float newAmplitude, float newSigmaWidth, float newSigmaLength, float newSigmadepth, float newCenterX, float newCenterY, float newCenterZ);
	virtual ~AnisotropicGaussian3D();

	float getSigmaWidth();
	float getSigmaLength();
	float getSigmaHeight();

	void setSigmas(float newSigmaWidth, float newSigmaLength, float newSigmadepth );

	float getAmplitude();
	void setAmplitude(float newValue);

	fPoint3D getCenter();

	double getValueAtDistance(float xDistance, float yDistance, float zDistance);
	double getValueAtPoint(float xPoint, float yPoint, float zPoint);

	double getValueAtDistance(fVector3D distance );
	double getValueAtPoint(fPoint3D point);

};


#endif /* GAUSSIAN3D_H_ */
