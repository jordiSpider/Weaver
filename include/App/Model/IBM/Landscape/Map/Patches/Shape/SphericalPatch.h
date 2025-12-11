/**
 * @file SphericalPatch.h
 * @brief Defines the SphericalPatch class, a concrete PatchShape with spherical influence.
 *
 * This file contains the declaration of the SphericalPatch class, which represents
 * a patch whose influence is distributed in a spherical area.
 */

#ifndef SPHERICAL_PATCH_H_
#define SPHERICAL_PATCH_H_

#include <vector>

#include "App/Model/IBM/Landscape/Map/Patches/Shape/PatchShape.h"

/**
 * @class SphericalPatch
 * @brief Represents a patch with spherical influence.
 *
 * The SphericalPatch class is a concrete implementation of PatchShape.
 * The patch influence is defined by a center point and a radius, forming
 * a spherical area in which the patch exerts its effects.
 */
class SphericalPatch : public PatchShape {
protected:
    /**
     * @brief Generates a textual description of the spherical patch.
     * @param center Coordinates of the patch center.
     * @param radius Radius of the sphere.
     * @return String describing the patch.
     */
    const std::string generateDescription(const std::vector<double> &center, const double &radius) const;
    
    /**
     * @brief Generates the influence zone of the spherical patch as a RingModel.
     * @param center Coordinates of the patch center.
     * @param radius Radius of the sphere.
     * @return Pointer to a RingModel representing the influence zone.
     */
    const RingModel* generateInfluenceZone(const std::vector<double> &center, const double &radius);

public:
    /**
     * @brief Constructs a SphericalPatch from a JSON configuration.
     * @param shapeConfig JSON object containing the patch parameters.
     */
    SphericalPatch(const nlohmann::json &shapeConfig);
    
    /**
     * @brief Destructor.
     */
    virtual ~SphericalPatch();

    /**
     * @brief Deleted copy constructor (SphericalPatch is non-copyable).
     */
    SphericalPatch(const SphericalPatch&) = delete;
    
    /**
     * @brief Deleted copy assignment operator (SphericalPatch is non-copyable).
     */
    SphericalPatch& operator=(const SphericalPatch&) = delete;

    /**
     * @brief Returns the type of this patch shape.
     * @return Type Enum value PatchShape::Spherical.
     */
    Type getType() const;
};

#endif /* SPHERICAL_PATCH_H_ */