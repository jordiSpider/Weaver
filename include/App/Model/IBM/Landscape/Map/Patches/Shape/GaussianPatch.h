/**
 * @file GaussianPatch.h
 * @brief Defines the GaussianPatch class, a type of PatchShape with Gaussian distribution.
 *
 * This file contains the declaration of the GaussianPatch class, which represents
 * a patch in the landscape whose influence follows a Gaussian (bell-shaped) distribution.
 */

#ifndef GAUSSIAN_PATCH_H_
#define GAUSSIAN_PATCH_H_

#include "App/Model/IBM/Landscape/Map/Patches/Shape/PatchShape.h"

/**
 * @class GaussianPatch
 * @brief Represents a patch with Gaussian-shaped influence.
 *
 * The GaussianPatch class is a concrete implementation of PatchShape.
 * The influence zone follows a Gaussian distribution centered on the patch,
 * allowing partial coverage of landscape cells depending on distance from the center.
 */
class GaussianPatch : public PatchShape {
public:
    /**
     * @brief Constructs a GaussianPatch from a JSON configuration.
     * @param shapeConfig JSON object specifying the parameters of the Gaussian patch
     *                    (e.g., center, radius, variance).
     */
    GaussianPatch(const nlohmann::json &shapeConfig);
    
    /**
     * @brief Destructor.
     */
    virtual ~GaussianPatch();

    /**
     * @brief Deleted copy constructor (GaussianPatch is non-copyable).
     */
    GaussianPatch(const GaussianPatch&) = delete;
    
    /**
     * @brief Deleted copy assignment operator (GaussianPatch is non-copyable).
     */
    GaussianPatch& operator=(const GaussianPatch&) = delete;

    /**
     * @brief Returns the type of this patch shape.
     * @return Type Enum value PatchShape::Gaussian.
     */
    Type getType() const;
};

#endif /* GAUSSIAN_PATCH_H_ */