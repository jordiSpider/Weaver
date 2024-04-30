/*
 * gaussian_distribution.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: jrbcast
 */

#include "Maths/GaussianDistribution.h"

Gaussian1D::Gaussian1D()
{
	amplitude = 1.0;
	setSigma(1.0);
	center = 0.0;
	precompute = false;
}

Gaussian1D::Gaussian1D(float newAmplitude, float newSigma, float newCenter)
{
	amplitude = newAmplitude;
	setSigma(newSigma);
	center = newCenter;
}

Gaussian1D::~Gaussian1D()
{
	if (precompute)
	{
		precomputed.clear();
	}
}

bool Gaussian1D::isPrecomputed()
{
	return precompute;
}

void Gaussian1D::setPrecomputed(bool doPrecompute = true)
{
	if (precompute)
	{
		if (!doPrecompute)
		{
			precompute = false;
			precomputed.clear();
		}
	}
	else
	{
		if (doPrecompute)
		{
			precompute = true;

			precomputeBinWidth = precomputeDim / precomputeBins;
			precomputed.reserve(precomputeBins);

			float distance = 0;
			precomputed.push_back(amplitude); // First item is the amplitude of the gaussian

			for (unsigned int i = 1; i < precomputeBins; i++)
			{
				distance += precomputeBinWidth;
				precomputed.push_back(getValueAtDistance(distance));
			}

			// We do this to avoid further divisions. Use multiplications instead
			precomputeBinWidth = 1.0 / precomputeBinWidth;
		}
	}
}

float Gaussian1D::getSigma()
{
	return sigma;
}

void Gaussian1D::setSigma(float newValue)
{

	if (newValue <= 0)
	{
		std::cerr << "Sigma values cannot be non-positive numbers, exiting" << std::endl;
		exit(-1);
	}

	sigma = newValue;
	inverseSigma = 0.5 * 1.0 / sigma;
}

float Gaussian1D::getAmplitude()
{
	return amplitude;
}

void Gaussian1D::setAmplitude(float newValue)
{
	amplitude = newValue;
}

double Gaussian1D::getValueAtDistance(float distance)
{
	if (precompute)
	{
		return precomputed[floor(distance * precomputeBinWidth)];
	}
	else
	{
		return (amplitude * exp(-pow(distance * inverseSigma, 2)));
	}
}

double Gaussian1D::getValueAtPoint(float point)
{
	return getValueAtDistance(point - center);
}

/****** Isotropic 3D *******/

IsotropicGaussian3D::IsotropicGaussian3D() : amplitude(1), center(0.0, 0.0, 0.0)
{
	setSigma(1);
}

IsotropicGaussian3D::IsotropicGaussian3D(float newAmplitude, float newSigma, fPoint3D newCenter) : amplitude(newAmplitude), center(newCenter)
{
	setSigma(newSigma);
}

IsotropicGaussian3D::IsotropicGaussian3D(float newAmplitude, float newSigma, float newCenterX, float newCenterY,
		float newCenterZ) : amplitude(newAmplitude), center(newCenterX, newCenterY, newCenterZ)
{
	setSigma(newSigma);
}

IsotropicGaussian3D::~IsotropicGaussian3D()
{
}

void IsotropicGaussian3D::setSigma(float newValue)
{
	if (newValue <= 0)
	{
		std::cerr << "Sigma values cannot be non-positive numbers, exiting" << std::endl;
		exit(-1);
	}

	sigma = newValue;
	inverseSquaredSigma = 0.5 * 1.0 / sigma;
	inverseSquaredSigma = inverseSquaredSigma * inverseSquaredSigma;
}

void IsotropicGaussian3D::setAmplitude(float newValue)
{
	amplitude = newValue;
}

double IsotropicGaussian3D::getValueAtDistance(float xDistance, float yDistance, float zDistance)
{
	// Calculate norm of x,y,z vector
	double norm = (double) (xDistance * xDistance + yDistance * yDistance + zDistance * zDistance);
	return (amplitude * exp(-pow(norm * inverseSquaredSigma, 2)));
}

double IsotropicGaussian3D::getValueAtDistance(fVector3D distance)
{
	return getValueAtDistance(distance.getX(), distance.getY(), distance.getZ());
}

double IsotropicGaussian3D::getValueAtPoint(float pointX, float pointY, float pointZ)
{
	return getValueAtDistance(pointX - center.getX(), pointY - center.getY(), pointZ - center.getZ());
}

double IsotropicGaussian3D::getValueAtPoint(fPoint3D point)
{
	return getValueAtDistance(point.getX() - center.getX(), point.getY() - center.getY(), point.getZ() - center.getZ());
}

/****** Anisotropic 3D *******/

AnisotropicGaussian3D::AnisotropicGaussian3D() : amplitude(1), center(0.0, 0.0, 0.0)
{
	setSigmas(1, 1, 1);
}

AnisotropicGaussian3D::AnisotropicGaussian3D(float newAmplitude, float newSigmaWidth, float newSigmaLength,
		float newSigmadepth, fPoint3D newCenter) : amplitude(newAmplitude), center(newCenter)
{
	setSigmas(newSigmaWidth, newSigmaLength, newSigmadepth);
}

AnisotropicGaussian3D::AnisotropicGaussian3D(float newAmplitude, float newSigmaWidth, float newSigmaLength,
		float newSigmadepth, float newCenterX, float newCenterY, float newCenterZ)
	: amplitude(newAmplitude), center(newCenterX, newCenterY, newCenterZ)
{
	setSigmas(newSigmaWidth, newSigmaLength, newSigmadepth);
}

AnisotropicGaussian3D::~AnisotropicGaussian3D()
{
}

float AnisotropicGaussian3D::getSigmaWidth()
{
	return sigmaWidth;
}

float AnisotropicGaussian3D::getSigmaLength()
{
	return sigmaLength;
}

float AnisotropicGaussian3D::getSigmaHeight()
{
	return sigmaDepth;
}

void AnisotropicGaussian3D::setSigmas(float newSigmaWidth, float newSigmaLength, float newSigmadepth)
{
	if (newSigmaWidth <= 0 || newSigmaLength <= 0 || newSigmadepth <= 0)
	{
		std::cerr << "Sigma values cannot be non-positive numbers, exiting" << std::endl;
		exit(-1);
	}

	sigmaWidth = newSigmaWidth;
	sigmaLength = newSigmaLength;
	sigmaDepth = newSigmadepth;
	inverseSquaredSigmaWidth = 0.5 * 1.0 / sigmaWidth;
	inverseSquaredSigmaWidth = inverseSquaredSigmaWidth * inverseSquaredSigmaWidth;
	inverseSquaredSigmaLength = 0.5 * 1.0 / sigmaLength;
	inverseSquaredSigmaLength = inverseSquaredSigmaLength * inverseSquaredSigmaLength;
	inverseSquaredSigmaDepth = 0.5 * 1.0 / sigmaDepth;
	inverseSquaredSigmaDepth = inverseSquaredSigmaDepth * inverseSquaredSigmaDepth;
}

fPoint3D AnisotropicGaussian3D::getCenter()
{
	return center;
}

float AnisotropicGaussian3D::getAmplitude()
{
	return amplitude;
}

void AnisotropicGaussian3D::setAmplitude(float newValue)
{
	amplitude = newValue;
}

double AnisotropicGaussian3D::getValueAtDistance(float xDistance, float yDistance, float zDistance)
{
	// Calculate norm of x,y,z vector
	double normWidth = (double) (xDistance * xDistance);
	double normLength = (double) (yDistance * yDistance);
	double normDepth = (double) (zDistance * zDistance);

	return (amplitude * exp(-(normWidth * inverseSquaredSigmaWidth + normLength * inverseSquaredSigmaLength + normDepth
			* inverseSquaredSigmaDepth)));
}

double AnisotropicGaussian3D::getValueAtDistance(fVector3D distance)
{
	return getValueAtDistance(distance.getX(), distance.getY(), distance.getZ());
}

double AnisotropicGaussian3D::getValueAtPoint(float pointX, float pointY, float pointZ)
{
	return getValueAtDistance(pointX - center.getX(), pointY - center.getY(), pointZ - center.getZ());
}

double AnisotropicGaussian3D::getValueAtPoint(fPoint3D point)
{
	return getValueAtDistance(point.getX() - center.getX(), point.getY() - center.getY(), point.getZ() - center.getZ());
}
