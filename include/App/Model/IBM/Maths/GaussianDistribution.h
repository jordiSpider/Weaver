/**
 * @file GaussianDistribution.h
 * @brief Classes for handling 1D and 3D Gaussian distributions (isotropic and anisotropic) with optional precomputation.
 *
 * Provides classes for Gaussian distributions:
 * - Gaussian1D: 1-dimensional Gaussian with optional precomputed values.
 * - IsotropicGaussian3D: 3D Gaussian with the same sigma in all directions.
 * - AnisotropicGaussian3D: 3D Gaussian with different sigma values along X, Y, Z axes.
 */

#ifndef GAUSSIAN_DISTRIBUTION_H_
#define GAUSSIAN_DISTRIBUTION_H_

#include <math.h>
#include <vector>
#include <iostream>

#include "App/Model/IBM/Maths/Constants.h"
#include "Misc/Types.h"
#include "App/Model/IBM/Maths/PreciseDouble.h"

/**
 * @class Gaussian1D
 * @brief Class for handling 1-dimensional Gaussian distribution.
 *
 * Supports optional precomputation for faster repeated evaluations.
 */
class Gaussian1D {
protected:
	PreciseDouble sigma;           /**< Sigma parameter of the Gaussian function. */
    PreciseDouble amplitude;       /**< Amplitude of the Gaussian function. */
    PreciseDouble center;          /**< Center of the Gaussian function. */
    bool precompute;               /**< Flag indicating if precomputation is enabled. */
    unsigned int precomputeBins;   /**< Number of bins for precomputed values. */
    unsigned int precomputeDim;    /**< Dimension (radius) of precomputation from center. */
    std::vector<PreciseDouble> precomputed; /**< Precomputed values of Gaussian function. */
    PreciseDouble precomputeBinWidth; /**< Size of each bin, stored inverted for speed. */
    PreciseDouble inverseSigma;    /**< Precomputed 1/sigma for speed. */

public:
	/** @brief Default constructor. Amplitude=1, Sigma=1, Center=0. */
	Gaussian1D();

	/** @brief Constructor with specified amplitude, sigma, and center. */
	Gaussian1D(const PreciseDouble& newAmplitude, const PreciseDouble& newSigma, const PreciseDouble& newCenter);
	
	/** @brief Destructor. */
	virtual ~Gaussian1D();

	/** @brief Get the sigma value. */
	const PreciseDouble& getSigma();

	/** @brief Set the sigma value. */
	void setSigma(const PreciseDouble& newValue);

	/** @brief Check if the Gaussian1D is precomputed. */
	bool isPrecomputed();

	/** @brief Enable or disable precomputation. */
	void setPrecomputed(bool doPrecompute);

	/** @brief Get the amplitude. */
	const PreciseDouble& getAmplitude();

	/** @brief Set the amplitude. */
	void setAmplitude(const PreciseDouble& newValue);

	/** @brief Get the center. */
	const PreciseDouble& getCenter();

	/** @brief Compute Gaussian value at a given distance from center. */
	PreciseDouble getValueAtDistance(const PreciseDouble& distance);

	/** @brief Compute Gaussian value at a specific point. */
	PreciseDouble getValueAtPoint(const PreciseDouble& point);
};


/**
 * @class IsotropicGaussian3D
 * @brief Class for handling isotropic 3-dimensional Gaussian distributions.
 *
 * The sigma is the same in all three dimensions. Provides evaluation at points or distances.
 */
class IsotropicGaussian3D {
protected:
	PreciseDouble sigma;           /**< Sigma for Gaussian function. */
    PreciseDouble amplitude;       /**< Amplitude for Gaussian function. */
    dPoint3D center;               /**< Center in 3D space. */
    PreciseDouble inverseSquaredSigma; /**< Precomputed 1/sigma² for speed. */

public:
	/** @brief Default constructor. */
	IsotropicGaussian3D();

	/** @brief Constructor with amplitude, sigma, and center. */
	IsotropicGaussian3D(const PreciseDouble& newAmplitude, const PreciseDouble& newSigma, dPoint3D newCenter);
	
	/** @brief Constructor with amplitude, sigma, and center coordinates. */
	IsotropicGaussian3D(const PreciseDouble& newAmplitude, const PreciseDouble& newSigma, const PreciseDouble& newCenterX, const PreciseDouble& newCenterY, const PreciseDouble& newCenterZ);
	
	/** @brief Destructor. */
	virtual ~IsotropicGaussian3D();

	/** @brief Get the sigma value. */
	const PreciseDouble& getSigma() const;

	/** @brief Set the sigma value. */
	void setSigma(const PreciseDouble& newValue);

	/** @brief Get the amplitude. */
	const PreciseDouble& getAmplitude() const;

	/** @brief Set the amplitude. */
	void setAmplitude(const PreciseDouble& newValue);

	/** @brief Get the 3D center point. */
	const dPoint3D& getCenter() const;

	/** @brief Get Gaussian value at distance from center (separate coordinates). */
	PreciseDouble getValueAtDistance(const PreciseDouble& xDistance, const PreciseDouble& yDistance, const PreciseDouble& zDistance);

    /** @brief Get Gaussian value at point (separate coordinates). */
	PreciseDouble getValueAtPoint(const PreciseDouble& xPoint, const PreciseDouble& yPoint, const PreciseDouble& zPoint);

    /** @brief Get Gaussian value at distance vector. */
	PreciseDouble getValueAtDistance(dVector3D distance);

    /** @brief Get Gaussian value at point vector. */
	PreciseDouble getValueAtPoint(dPoint3D point);
};


/**
 * @class AnisotropicGaussian3D
 * @brief Class for handling anisotropic 3D Gaussian distributions.
 *
 * Each dimension can have its own sigma. Precomputed inverse squared sigmas are used for speed.
 */
class AnisotropicGaussian3D {
protected:
	PreciseDouble sigmaWidth;      /**< Sigma in X direction. */
    PreciseDouble sigmaLength;     /**< Sigma in Y direction. */
    PreciseDouble sigmaDepth;      /**< Sigma in Z direction. */
    PreciseDouble amplitude;       /**< Maximum amplitude. */
    dPoint3D center;               /**< 3D center point. */
    PreciseDouble inverseSquaredSigmaWidth;  /**< Precomputed 1/sigma² in X. */
    PreciseDouble inverseSquaredSigmaLength; /**< Precomputed 1/sigma² in Y. */
    PreciseDouble inverseSquaredSigmaDepth;  /**< Precomputed 1/sigma² in Z. */

public:
	/** @brief Default constructor. Sigma=(1,1,1), amplitude=1, center=(0,0,0). */
	AnisotropicGaussian3D();

	/** @brief Constructor with amplitude, sigmas, and center. */
	AnisotropicGaussian3D(const PreciseDouble& newAmplitude, const PreciseDouble& newSigmaWidth, const PreciseDouble& newSigmaLength, const PreciseDouble& newSigmadepth, dPoint3D newCenter);
	
	/** @brief Constructor with amplitude, sigmas, and center coordinates. */
	AnisotropicGaussian3D(const PreciseDouble& newAmplitude, const PreciseDouble& newSigmaWidth, const PreciseDouble& newSigmaLength, const PreciseDouble& newSigmadepth, const PreciseDouble& newCenterX, const PreciseDouble& newCenterY, const PreciseDouble& newCenterZ);
	
	/** @brief Destructor. */
	virtual ~AnisotropicGaussian3D();

	/** @brief Get sigma in X direction. */
	const PreciseDouble& getSigmaWidth();

	/** @brief Get sigma in Y direction. */
	const PreciseDouble& getSigmaLength();

	/** @brief Get sigma in Z direction. */
	const PreciseDouble& getSigmaHeight();

	/** @brief Set sigma values in all directions. */
	void setSigmas(const PreciseDouble& newSigmaWidth, const PreciseDouble& newSigmaLength, const PreciseDouble& newSigmadepth );

	/** @brief Get amplitude. */
	const PreciseDouble& getAmplitude();

	/** @brief Set amplitude. */
	void setAmplitude(const PreciseDouble& newValue);

	/** @brief Get 3D center point. */
	dPoint3D getCenter();

	/** @brief Get Gaussian value at distance (X,Y,Z). */
	PreciseDouble getValueAtDistance(const PreciseDouble& xDistance, const PreciseDouble& yDistance, const PreciseDouble& zDistance);
	
	/** @brief Get Gaussian value at point (X,Y,Z). */
	PreciseDouble getValueAtPoint(const PreciseDouble& xPoint, const PreciseDouble& yPoint, const PreciseDouble& zPoint);

	/** @brief Get Gaussian value at distance vector. */
	PreciseDouble getValueAtDistance(dVector3D distance );
	
	/** @brief Get Gaussian value at point vector. */
	PreciseDouble getValueAtPoint(dPoint3D point);
};

#endif /* GAUSSIAN_DISTRIBUTION_H_ */
