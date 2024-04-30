/**
 * @file GaussianDistribution.cpp
 * @brief Implementation file for Gaussian distribution classes.
 * Created on: Jul 31, 2012
 * Author: jrbcast
 */

#include "IBM/Maths/GaussianDistribution.h"

Gaussian1D::Gaussian1D()
{
	amplitude = 1.0;
	setSigma(1.0);
	center = 0.0;
	precompute = false;
}

Gaussian1D::Gaussian1D(double newAmplitude, double newSigma, double newCenter)
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

			double distance = 0;
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

double Gaussian1D::getSigma()
{
	return sigma;
}

void Gaussian1D::setSigma(double newValue)
{

	if (newValue <= 0)
	{
		std::cerr << "Sigma values cannot be non-positive numbers, exiting" << std::endl;
		exit(-1);
	}

	sigma = newValue;
	inverseSigma = 1.0 / sigma;
}

double Gaussian1D::getAmplitude()
{
	return amplitude;
}

void Gaussian1D::setAmplitude(double newValue)
{
	amplitude = newValue;
}

double Gaussian1D::getValueAtDistance(double distance)
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

double Gaussian1D::getValueAtPoint(double point)
{
	return getValueAtDistance(point - center);
}

template<class Archive>
void Gaussian1D::serialize(Archive & ar, const unsigned int version) {
	ar & sigma;
	ar & amplitude;
	ar & center;
	ar & precompute;
	ar & precomputeBins;
	ar & precomputeDim;
	ar & precomputed;
	ar & precomputeBinWidth;
	ar & inverseSigma;
}

/****** Isotropic 3D *******/

IsotropicGaussian3D::IsotropicGaussian3D() : amplitude(1), center(0.0, 0.0, 0.0)
{
	setSigma(1);
}

IsotropicGaussian3D::IsotropicGaussian3D(double newAmplitude, double newSigma, fPoint3D newCenter) : amplitude(newAmplitude), center(newCenter)
{
	setSigma(newSigma);
}

IsotropicGaussian3D::IsotropicGaussian3D(double newAmplitude, double newSigma, double newCenterX, double newCenterY,
		double newCenterZ) : amplitude(newAmplitude), center(newCenterX, newCenterY, newCenterZ)
{
	setSigma(newSigma);
}

IsotropicGaussian3D::~IsotropicGaussian3D()
{
}

void IsotropicGaussian3D::setSigma(double newValue)
{
	if (newValue <= 0)
	{
		std::cerr << "Sigma values cannot be non-positive numbers, exiting" << std::endl;
		exit(-1);
	}

	sigma = newValue;
	inverseSquaredSigma = 1.0 / sigma;
	inverseSquaredSigma = inverseSquaredSigma * inverseSquaredSigma;
}

void IsotropicGaussian3D::setAmplitude(double newValue)
{
	amplitude = newValue;
}

double IsotropicGaussian3D::getValueAtDistance(double xDistance, double yDistance, double zDistance)
{
	// Calculate norm of x,y,z vector
	double norm = (double) (xDistance * xDistance + yDistance * yDistance + zDistance * zDistance);
	return (amplitude * exp(-pow(norm * inverseSquaredSigma, 2)));
}

double IsotropicGaussian3D::getValueAtDistance(fVector3D distance)
{
	return getValueAtDistance(distance.getX(), distance.getY(), distance.getZ());
}

double IsotropicGaussian3D::getValueAtPoint(double pointX, double pointY, double pointZ)
{
	return getValueAtDistance(pointX - center.getX(), pointY - center.getY(), pointZ - center.getZ());
}

double IsotropicGaussian3D::getValueAtPoint(fPoint3D point)
{
	return getValueAtDistance(point.getX() - center.getX(), point.getY() - center.getY(), point.getZ() - center.getZ());
}

template<class Archive>
void IsotropicGaussian3D::serialize(Archive & ar, const unsigned int version) {
	ar & sigma;
	ar & amplitude;
	ar & center;
	ar & inverseSquaredSigma;
}

/****** Anisotropic 3D *******/

AnisotropicGaussian3D::AnisotropicGaussian3D() : amplitude(1), center(0.0, 0.0, 0.0)
{
	setSigmas(1, 1, 1);
}

AnisotropicGaussian3D::AnisotropicGaussian3D(double newAmplitude, double newSigmaWidth, double newSigmaLength,
		double newSigmadepth, fPoint3D newCenter) : amplitude(newAmplitude), center(newCenter)
{
	setSigmas(newSigmaWidth, newSigmaLength, newSigmadepth);
}

AnisotropicGaussian3D::AnisotropicGaussian3D(double newAmplitude, double newSigmaWidth, double newSigmaLength,
		double newSigmadepth, double newCenterX, double newCenterY, double newCenterZ)
	: amplitude(newAmplitude), center(newCenterX, newCenterY, newCenterZ)
{
	setSigmas(newSigmaWidth, newSigmaLength, newSigmadepth);
}

AnisotropicGaussian3D::~AnisotropicGaussian3D()
{
}

double AnisotropicGaussian3D::getSigmaWidth()
{
	return sigmaWidth;
}

double AnisotropicGaussian3D::getSigmaLength()
{
	return sigmaLength;
}

double AnisotropicGaussian3D::getSigmaHeight()
{
	return sigmaDepth;
}

void AnisotropicGaussian3D::setSigmas(double newSigmaWidth, double newSigmaLength, double newSigmadepth)
{
	if (newSigmaWidth <= 0 || newSigmaLength <= 0 || newSigmadepth <= 0)
	{
		std::cerr << "Sigma values cannot be non-positive numbers, exiting" << std::endl;
		exit(-1);
	}

	sigmaWidth = newSigmaWidth;
	sigmaLength = newSigmaLength;
	sigmaDepth = newSigmadepth;
	inverseSquaredSigmaWidth = 1.0 / sigmaWidth;
	inverseSquaredSigmaWidth = inverseSquaredSigmaWidth * inverseSquaredSigmaWidth;
	inverseSquaredSigmaLength = 1.0 / sigmaLength;
	inverseSquaredSigmaLength = inverseSquaredSigmaLength * inverseSquaredSigmaLength;
	inverseSquaredSigmaDepth = 1.0 / sigmaDepth;
	inverseSquaredSigmaDepth = inverseSquaredSigmaDepth * inverseSquaredSigmaDepth;
}

fPoint3D AnisotropicGaussian3D::getCenter()
{
	return center;
}

double AnisotropicGaussian3D::getAmplitude()
{
	return amplitude;
}

void AnisotropicGaussian3D::setAmplitude(double newValue)
{
	amplitude = newValue;
}

double AnisotropicGaussian3D::getValueAtDistance(double xDistance, double yDistance, double zDistance)
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

double AnisotropicGaussian3D::getValueAtPoint(double pointX, double pointY, double pointZ)
{
	return getValueAtDistance(pointX - center.getX(), pointY - center.getY(), pointZ - center.getZ());
}

double AnisotropicGaussian3D::getValueAtPoint(fPoint3D point)
{
	return getValueAtDistance(point.getX() - center.getX(), point.getY() - center.getY(), point.getZ() - center.getZ());
}

template<class Archive>
void AnisotropicGaussian3D::serialize(Archive & ar, const unsigned int version) {
	ar & sigmaWidth;
	ar & sigmaLength;
	ar & sigmaDepth;
	ar & amplitude;
	ar & center;
	ar & inverseSquaredSigmaWidth;
	ar & inverseSquaredSigmaLength;
	ar & inverseSquaredSigmaDepth;
}
