/**
 * @file RandomGaussianPatch.h
 * @brief Defines the RandomGaussianPatch class, a type of PatchShape with stochastic Gaussian influence.
 *
 * This file contains the declaration of the RandomGaussianPatch class, which represents
 * a patch whose influence is distributed according to a Gaussian function with random variation.
 */

#ifndef RANDOM_GAUSSIAN_PATCH_H_
#define RANDOM_GAUSSIAN_PATCH_H_

#include "App/Model/IBM/Landscape/Map/Patches/Shape/PatchShape.h"

/**
 * @class RandomGaussianPatch
 * @brief Represents a patch with stochastic Gaussian influence.
 *
 * The RandomGaussianPatch class is a concrete implementation of PatchShape.
 * The patch influence follows a Gaussian distribution but includes random variability,
 * making it useful for modeling heterogeneous environmental effects.
 */
class RandomGaussianPatch : public PatchShape {
public:
    /**
     * @brief Constructs a RandomGaussianPatch from a JSON configuration.
     * @param shapeConfig JSON object containing the patch parameters.
     */
    RandomGaussianPatch(const nlohmann::json &shapeConfig);
    
    /**
     * @brief Destructor.
     */
    virtual ~RandomGaussianPatch();

    /**
     * @brief Deleted copy constructor (RandomGaussianPatch is non-copyable).
     */
    RandomGaussianPatch(const RandomGaussianPatch&) = delete;
    
    /**
     * @brief Deleted copy assignment operator (RandomGaussianPatch is non-copyable).
     */
    RandomGaussianPatch& operator=(const RandomGaussianPatch&) = delete;

    /**
     * @brief Returns the type of this patch shape.
     * @return Type Enum value PatchShape::RandomGaussian.
     */
    Type getType() const;
};

#endif /* RANDOM_GAUSSIAN_PATCH_H_ */