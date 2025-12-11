/**
 * @file GaussianDistribution.cpp
 * @brief Implementation file for Gaussian distribution classes.
 * Created on: Jul 31, 2012
 * Author: jrbcast
 */

#include "App/Model/IBM/Maths/GaussianDistribution.h"

Gaussian1D::Gaussian1D()
{
	amplitude = 1.0;
	setSigma(1.0);
	center = 0.0;
	precompute = false;
}

Gaussian1D::Gaussian1D(const PreciseDouble& newAmplitude, const PreciseDouble& newSigma, const PreciseDouble& newCenter)
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

			PreciseDouble distance = 0.0;
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

const PreciseDouble& Gaussian1D::getSigma()
{
	return sigma;
}

void Gaussian1D::setSigma(const PreciseDouble& newValue)
{

	if (newValue <= 0)
	{
		std::cerr << "Sigma values cannot be non-positive numbers, exiting" << std::endl;
		exit(-1);
	}

	sigma = newValue;
	inverseSigma = 1.0 / sigma;
}

const PreciseDouble& Gaussian1D::getAmplitude()
{
	return amplitude;
}

void Gaussian1D::setAmplitude(const PreciseDouble& newValue)
{
	amplitude = newValue;
}

PreciseDouble Gaussian1D::getValueAtDistance(const PreciseDouble& distance)
{
	if (precompute)
	{
		return precomputed[static_cast<unsigned int>(floor(distance * precomputeBinWidth).getValue())];
	}
	else
	{
		return (amplitude * exp(-pow(distance * inverseSigma, 2)));
	}
}

PreciseDouble Gaussian1D::getValueAtPoint(const PreciseDouble& point)
{
	return getValueAtDistance(point - center);
}

/****** Isotropic 3D *******/

IsotropicGaussian3D::IsotropicGaussian3D() : amplitude(1), center(0.0, 0.0, 0.0)
{
	setSigma(1);
}

IsotropicGaussian3D::IsotropicGaussian3D(const PreciseDouble& newAmplitude, const PreciseDouble& newSigma, dPoint3D newCenter) : amplitude(newAmplitude), center(newCenter)
{
	setSigma(newSigma);
}

IsotropicGaussian3D::IsotropicGaussian3D(const PreciseDouble& newAmplitude, const PreciseDouble& newSigma, const PreciseDouble& newCenterX, const PreciseDouble& newCenterY,
		const PreciseDouble& newCenterZ) : amplitude(newAmplitude), center(newCenterX, newCenterY, newCenterZ)
{
	setSigma(newSigma);
}

IsotropicGaussian3D::~IsotropicGaussian3D()
{

}

const PreciseDouble& IsotropicGaussian3D::getSigma() const 
{ 
	return sigma; 
}

void IsotropicGaussian3D::setSigma(const PreciseDouble& newValue)
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

const PreciseDouble& IsotropicGaussian3D::getAmplitude() const 
{ 
	return amplitude; 
}

void IsotropicGaussian3D::setAmplitude(const PreciseDouble& newValue)
{
	amplitude = newValue;
}

const dPoint3D& IsotropicGaussian3D::getCenter() const 
{ 
	return center; 
}

PreciseDouble IsotropicGaussian3D::getValueAtDistance(const PreciseDouble& xDistance, const PreciseDouble& yDistance, const PreciseDouble& zDistance)
{
	// Calculate norm of x,y,z vector
	PreciseDouble norm = xDistance * xDistance + yDistance * yDistance + zDistance * zDistance;
	return (amplitude * exp(-pow(norm * inverseSquaredSigma, 2)));
}

PreciseDouble IsotropicGaussian3D::getValueAtDistance(dVector3D distance)
{
	return getValueAtDistance(distance.getX(), distance.getY(), distance.getZ());
}

PreciseDouble IsotropicGaussian3D::getValueAtPoint(const PreciseDouble& pointX, const PreciseDouble& pointY, const PreciseDouble& pointZ)
{
	return getValueAtDistance(pointX - center.getX(), pointY - center.getY(), pointZ - center.getZ());
}

PreciseDouble IsotropicGaussian3D::getValueAtPoint(dPoint3D point)
{
	return getValueAtDistance(point.getX() - center.getX(), point.getY() - center.getY(), point.getZ() - center.getZ());
}

/****** Anisotropic 3D *******/

AnisotropicGaussian3D::AnisotropicGaussian3D() : amplitude(1), center(0.0, 0.0, 0.0)
{
	setSigmas(1, 1, 1);
}

AnisotropicGaussian3D::AnisotropicGaussian3D(const PreciseDouble& newAmplitude, const PreciseDouble& newSigmaWidth, const PreciseDouble& newSigmaLength,
		const PreciseDouble& newSigmadepth, dPoint3D newCenter) : amplitude(newAmplitude), center(newCenter)
{
	setSigmas(newSigmaWidth, newSigmaLength, newSigmadepth);
}

AnisotropicGaussian3D::AnisotropicGaussian3D(const PreciseDouble& newAmplitude, const PreciseDouble& newSigmaWidth, const PreciseDouble& newSigmaLength,
		const PreciseDouble& newSigmadepth, const PreciseDouble& newCenterX, const PreciseDouble& newCenterY, const PreciseDouble& newCenterZ)
	: amplitude(newAmplitude), center(newCenterX, newCenterY, newCenterZ)
{
	setSigmas(newSigmaWidth, newSigmaLength, newSigmadepth);
}

AnisotropicGaussian3D::~AnisotropicGaussian3D()
{
}

const PreciseDouble& AnisotropicGaussian3D::getSigmaWidth()
{
	return sigmaWidth;
}

const PreciseDouble& AnisotropicGaussian3D::getSigmaLength()
{
	return sigmaLength;
}

const PreciseDouble& AnisotropicGaussian3D::getSigmaHeight()
{
	return sigmaDepth;
}

void AnisotropicGaussian3D::setSigmas(const PreciseDouble& newSigmaWidth, const PreciseDouble& newSigmaLength, const PreciseDouble& newSigmadepth)
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

dPoint3D AnisotropicGaussian3D::getCenter()
{
	return center;
}

const PreciseDouble& AnisotropicGaussian3D::getAmplitude()
{
	return amplitude;
}

void AnisotropicGaussian3D::setAmplitude(const PreciseDouble& newValue)
{
	amplitude = newValue;
}

PreciseDouble AnisotropicGaussian3D::getValueAtDistance(const PreciseDouble& xDistance, const PreciseDouble& yDistance, const PreciseDouble& zDistance)
{
	// Calculate norm of x,y,z vector
	PreciseDouble normWidth = xDistance * xDistance;
	PreciseDouble normLength = yDistance * yDistance;
	PreciseDouble normDepth = zDistance * zDistance;

	return (amplitude * exp(-(normWidth * inverseSquaredSigmaWidth + normLength * inverseSquaredSigmaLength + normDepth
			* inverseSquaredSigmaDepth)));
}

PreciseDouble AnisotropicGaussian3D::getValueAtDistance(dVector3D distance)
{
	return getValueAtDistance(distance.getX(), distance.getY(), distance.getZ());
}

PreciseDouble AnisotropicGaussian3D::getValueAtPoint(const PreciseDouble& pointX, const PreciseDouble& pointY, const PreciseDouble& pointZ)
{
	return getValueAtDistance(pointX - center.getX(), pointY - center.getY(), pointZ - center.getZ());
}

PreciseDouble AnisotropicGaussian3D::getValueAtPoint(dPoint3D point)
{
	return getValueAtDistance(point.getX() - center.getX(), point.getY() - center.getY(), point.getZ() - center.getZ());
}
