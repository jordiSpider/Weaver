/**
 * @file GaussianDistribution.h
 * @brief Header file for Gaussian distribution classes.
 * Created on: Jul 31, 2012
 * Author: jrbcast
 */

#ifndef GAUSSIAN3D_H_
#define GAUSSIAN3D_H_

#include <math.h>
#include <vector>
#include <iostream>

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "Constants.h"
#include "Misc/Types.h"

/**
 * @class Gaussian1D
 * @brief Class for handling 1-dimensional Gaussian distribution.
 */
class Gaussian1D {
private:
	// Allow this class to be serialized using boost serialization libraries
    friend class boost::serialization::access;

protected:
	/**
	 * @brief Represents the sigma value for the Gaussian function.
	 */
	double sigma;

	/**
	 * @brief Represents the amplitude of the Gaussian function.
	 */
	double amplitude;

	/**
	 * @brief Represents the center of the Gaussian function.
	 */
	double center;

	/* Speed-up is done by pre-computing Gaussian values just once */ 

	/**
	 * @brief Indicates whether precomputation is enabled.
	 */
	bool precompute;

	/**
	 * @brief Number of precomputed items for the Gaussian function.
	 */
	unsigned int precomputeBins;

	/**	
	 * @brief Dimension (radius) of the Gaussian to be precomputed from its center.
	 */
	unsigned int precomputeDim;

	/**
	 * @brief Vector to store precomputed values for the Gaussian function.
	 */
	std::vector<double> precomputed;

	/**
	 * @brief Size of each bin for precomputation, stored inverted to speed up memory accesses.
	 */
	double precomputeBinWidth;

	/**
	 * @brief Represents the inverse of the sigma value, calculated to speed up computations.
	 */
	double inverseSigma;
public:
	/**
	 * @brief Default constructor for Gaussian1D with default parameter values.
	 * 
	 * Amplitude is set to 1.0, Sigma is set to 1.0, and Center is set to 0.0 by default.
	 */
	Gaussian1D();

	/**
 	 * @brief Overloaded constructor for Gaussian1D.
 	 * @param newAmplitude The new amplitude value for the Gaussian.
 	 * @param newSigma The new sigma value for the Gaussian.
 	 * @param newCenter The new center value for the Gaussian.
 	 */
	Gaussian1D(double newAmplitude, double newSigma, double newCenter);
	
	/**
	 * @brief Destructor for Gaussian1D.
	 */
	virtual ~Gaussian1D();

	/**
	 * @brief Getter for Sigma parameter.
	 * @returns double representing Sigma. 
	 */
	double getSigma();

	/**
	 * @brief Setter for Sigma parameter.
	 */
	void setSigma(double newValue);

	/**
	 * @brief Check if the Gaussian1D is precomputed.
	 */
	bool isPrecomputed();

	/**
	 * @brief Calculates precomputed values for the Gaussian function.
	 */
	void setPrecomputed(bool doPrecompute);

	/**
	 * @brief Getter for Amplitude parameter.
	 */
	double getAmplitude();

	/**
	 * @brief Setter for Amplitude parameter.
	 */
	void setAmplitude(double newValue);

	/**
	 * @brief Getter for Amplitude parameter.
	 *
	 * @returns A double representing the center of the Gaussian function
	 */
	double getCenter();

	/**
	 * @brief Get value for Gaussian distribution at a given distance from center.
	 *
	 * @returns A double representing the value of the Gaussian function.
	 */
	double getValueAtDistance(double distance);

	/**
	 * @brief Get value for Gaussian distribution at a given point in space.
	 *
	 * @returns A double representing the value of the Gaussian function.
	 */
	double getValueAtPoint(double point);

	// Serialization functions
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
};


/**
 * @class IsotropicGaussian3D
 * @brief Class for handling isotropic 3-dimensional Gaussian distribution.
 */
class IsotropicGaussian3D {
private:
	// Allow this class to be serialized using boost serialization libraries
	friend class boost::serialization::access;

protected:
	/**
	 * @brief Represents the sigma value for the Gaussian function.
	 */
	double sigma;

	/**
	 * @brief Represents the amplitude of the Gaussian function.
	 */
	double amplitude;

	/**
	 * @brief Represents the center of the Gaussian function in 3D space.
	 */
	fPoint3D center;

	/**
	 * @brief Represents the inverse of the squared sigma value, calculated to speed up computations.
	 */
	double inverseSquaredSigma;
public:
	/**
	 * @brief Default constructor.
	 */
	IsotropicGaussian3D();

	/**
	 * @brief Overloaded constructor.
	 *
	 * @param newAmplitude amplitude for the Gaussian 3D distribution.
	 * @param newSigma sigma for the Gaussian 3D distribution.
	 * @param newCenter spatial center for the Gaussian 3D distribution.
	 */
	IsotropicGaussian3D(double newAmplitude, double newSigma, fPoint3D newCenter);
	
	/**
	 * @brief Overloaded constructor.
	 *
	 * @param newAmplitude amplitude for the Gaussian 3D distribution.
	 * @param newSigma sigma for the Gaussian 3D distribution.
	 * @param newCenterX X coordinate for spatial center for the Gaussian 3D distribution.
	 * @param newCenterY Y coordinate for spatial center for the Gaussian 3D distribution.
	 * @param newCenterZ Z coordinate for spatial center for the Gaussian 3D distribution.
	 */
	IsotropicGaussian3D(double newAmplitude, double newSigma, double newCenterX, double newCenterY, double newCenterZ);
	
	/**
	 * @brief Destructor for IsotropicGaussian3D.
	 */
	virtual ~IsotropicGaussian3D();

	/**
	 * @brief Getter for Gaussian 3D sigma value.
	 *
	 * @returns A double representing the sigma of the Gaussian function.
	 */
	double getSigma() { return sigma; };

	/**
	 * @brief Setter for Gaussian 3D sigma value.
	 *
	 * @param A double representing the sigma of the Gaussian function.
	 */
	void setSigma(double newValue);

	/**
	 * @brief Getter for Gaussian 3D amplitude value.
	 *
	 * @returns A double representing the amplitude of the Gaussian function.
	 */
	double getAmplitude() { return amplitude; };

	/**
	 * @brief Setter for Gaussian 3D amplitude value.
	 *
	 * @param A double representing the amplitude of the Gaussian function.
	 */
	void setAmplitude(double newValue);

	/**
	 * @brief Getter for Gaussian 3D center point value.
	 *
	 * @returns A fPoint3D representing a 3D location of the Gaussian 3D center point.
	 */
	fPoint3D getCenter() { return center; };

	/**
	 * @brief Get value for Gaussian 3D distribution at a given distance from its center.
	 *
	 * @param xDistance distance in X dimension from center.
	 * @param yDistance distance in Y dimension from center.
	 * @param zDistance distance in Z dimension from center.
	 *
	 * @returns A double representing the value of the Gaussian function.
	 */
	double getValueAtDistance(double xDistance, double yDistance, double zDistance);
	
	/**
	 * @brief Get value for Gaussian 3D distribution at a given distance from its center.
	 *
	 * @param xPoint point X coordinate in 3D space.
	 * @param yPoint point Y coordinate in 3D space.
	 * @param zPoint point Z coordinate in 3D space.
	 *
	 * @returns A double representing the value of the Gaussian function.
	 */
	double getValueAtPoint(double xPoint, double yPoint, double zPoint);

	/**
	 * @brief Get value for Gaussian 3D distribution at a given distance from its center.
	 *
	 * @param distance distance expressed as a 3D vector.
	 *
	 * @returns A double representing the value of the Gaussian function.
	 */
	double getValueAtDistance(fVector3D distance );

	/**
	 * @brief Get value for Gaussian 3D distribution at a given point in 3D space.
	 *
	 * @param point point in 3D space.
	 *
	 * @returns A double representing the value of the Gaussian function.
	 */
	double getValueAtPoint(fPoint3D point);

	// Serialization functions
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


/**
 * @class AnisotropicGaussian3D
 * @brief Class for handling anisotropic 3D Gaussian distribution.
 */
class AnisotropicGaussian3D {
private:
	// Allow this class to be serialized using boost serialization libraries
    friend class boost::serialization::access;

protected:
	/**
	 * @brief Represents the sigma value for the Gaussian function in X dimension.
	 */
	double sigmaWidth;

	/**
	 * @brief Represents the sigma value for the Gaussian function in Y dimension.
	 */
	double sigmaLength;

	/**
	 * @brief Represents the sigma value for the Gaussian function in Z dimension.
	 */
	double sigmaDepth;

	/**
	 * @brief Represents the maximum amplitude value for the 3D Gaussian function.
	 */
	double amplitude;

	/**
	 * @brief Represents the center of the Gaussian function in 3D space.
	 */
	fPoint3D center;

	// Just to speed-up things, precalculate this each time you
	// change sigma value.

	/**
	 * @brief Precomputed value to store (1/sigma)² in X dimension.
	 */
	double inverseSquaredSigmaWidth;

	/**
	 * @brief Precomputed value to store (1/sigma)² in Y dimension.
	 */
	double inverseSquaredSigmaLength;

	/**
	 * @brief Precomputed value to store (1/sigma)² in Z dimension.
	 */
	double inverseSquaredSigmaDepth;
public:
	/**
	 * @brief Default AnisotropicGaussian3D constructor.
	 *
	 * Defaults sigma to (1.0,1.0,1.0), amplitude to 1 and sets its center to (0.0, 0.0, 0.0)
	 *
	 */
	AnisotropicGaussian3D();

	/**
	 * @brief Overloaded constructor.
	 *
	 * @param newAmplitude amplitude for the Gaussian 3D distribution.
	 * @param newSigmaWidth sigma for the Gaussian 3D distribution in X dimension.
	 * @param newSigmaLength sigma for the Gaussian 3D distribution in Y dimension.
	 * @param newSigmadepth sigma for the Gaussian 3D distribution in Z dimension.
	 * @param newCenter spatial center for the Gaussian 3D distribution.
	 */
	AnisotropicGaussian3D(double newAmplitude, double newSigmaWidth, double newSigmaLength, double newSigmadepth, fPoint3D newCenter);
	
	/**
	 * @brief Overloaded constructor.
	 *
	 * @param newAmplitude amplitude for the Gaussian 3D distribution.
	 * @param newSigmaWidth sigma for the Gaussian 3D distribution in X dimension.
	 * @param newSigmaLength sigma for the Gaussian 3D distribution in Y dimension.
	 * @param newSigmaDepth sigma for the Gaussian 3D distribution in Z dimension.
	 * @param newCenterX spatial center for the Gaussian 3D distribution in X dimension.
	 * @param newCenterY spatial center for the Gaussian 3D distribution in Y dimension.
	 * @param newCenterZ spatial center for the Gaussian 3D distribution in Z dimension.
	 */
	AnisotropicGaussian3D(double newAmplitude, double newSigmaWidth, double newSigmaLength, double newSigmadepth, double newCenterX, double newCenterY, double newCenterZ);
	
	/**
	 * @brief Destructor for AnisotropicGaussian3D.
	 */
	virtual ~AnisotropicGaussian3D();

	/**
	 * @brief Getter for Gaussian 3D sigma value in X direction.
	 *
	 * @returns A double representing sigma in X direction.
	 */
	double getSigmaWidth();

	/**
	 * @brief Getter for Gaussian 3D sigma value in Y direction.
	 *
	 * @returns A double representing sigma in Y direction.
	 */
	double getSigmaLength();

	/**
	 * @brief Getter for Gaussian 3D sigma value in Z direction.
	 *
	 * @returns A double representing sigma in Z direction.
	 */
	double getSigmaHeight();

	/**
	 * @brief Setter for Gaussian 3D sigma values.
	 *
	 * @param newSigmaWidth A double representing sigma in X direction.
	 * @param newSigmaLength A double representing sigma in Y direction.
	 * @param newSigmadepth A double representing sigma in Z direction.
	 */
	void setSigmas(double newSigmaWidth, double newSigmaLength, double newSigmadepth );

	/**
	 * @brief Getter for Gaussian 3D amplitude value.
	 *
	 * @returns A double representing maximum amplitude for AnisotropicGaussian3D.
	 */
	double getAmplitude();

	/**
	 * @brief Setter for Gaussian 3D amplitude value.
	 *
	 * @param newValue A double representing maximum amplitude for AnisotropicGaussian3D.
	 */
	void setAmplitude(double newValue);

	/**
	 * @brief Getter for AnisotropicGaussian3D spatial center.
	 *
	 * @returns A fPoint3D representing spatial coordinates for AnisotropicGaussian3D.
	 */
	fPoint3D getCenter();

	/**
	 * @brief Get value for Gaussian 3D distribution at a given distance from its center.
	 *
	 * @param xDistance distance in the X coordinate.
	 * @param yDistance distance in the Y coordinate.
	 * @param zDistance distance in the Z coordinate.
	 *
	 * @returns A double representing the value of the Gaussian function.
	 */
	double getValueAtDistance(double xDistance, double yDistance, double zDistance);
	
	/**
	 * @brief Get value for Gaussian 3D distribution at a given point in space.
	 *
	 * @param xPoint coordinate in the X coordinate.
	 * @param xPoint coordinate in the Y coordinate.
	 * @param zPoint coordinate in the Z coordinate.
	 *
	 * @returns A double representing the value of the Gaussian function.
	 */
	double getValueAtPoint(double xPoint, double yPoint, double zPoint);

	/**
	 * @brief Get value for Gaussian 3D distribution at a given distance from its center.
	 *
	 * @param distance distance in 3D space.
	 *
	 * @returns A double representing the value of the Gaussian function.
	 */
	double getValueAtDistance(fVector3D distance );
	
	/**
	 * @brief Get value for Gaussian 3D distribution at a given point in space.
	 *
	 * @param point coordinates in 3D space.
	 *
	 * @returns A double representing the value of the Gaussian function.
	 */
	double getValueAtPoint(fPoint3D point);

	// Serialization functions
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

#endif /* GAUSSIAN3D_H_ */
