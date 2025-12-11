/**
 * @file CubicPatch.h
 * @brief Defines the CubicPatch class representing cuboidal patch shapes.
 *
 * This file contains the CubicPatch class, a concrete implementation of PatchShape
 * representing cuboidal (axis-aligned box) patches in the landscape. It provides
 * methods to generate a description and influence zone of the patch.
 */

#ifndef CUBIC_PATCH_H_
#define CUBIC_PATCH_H_

#include <boost/geometry/algorithms/convert.hpp>

#include "App/Model/IBM/Landscape/Map/Patches/Shape/PatchShape.h"


/**
 * @class CubicPatch
 * @brief Represents a cuboidal patch shape.
 *
 * The CubicPatch class extends PatchShape and provides functionality to define
 * a patch using two points (bottom-left and top-right corners). It can generate
 * an influence zone as a RingModel and a textual description of the patch.
 */
class CubicPatch : public PatchShape {
protected:
    /**
     * @brief Generates a textual description of the cuboidal patch.
     *
     * @param bottomLeftCorner Vector of coordinates for the bottom-left corner.
     * @param topRightCorner Vector of coordinates for the top-right corner.
     * @return Description string of the patch.
     */
    const std::string generateDescription(const std::vector<double> &bottomLeftCorner, const std::vector<double> &topRightCorner) const;
    
    /**
     * @brief Generates the influence zone of the cuboidal patch as a RingModel.
     *
     * @param bottomLeftCorner Vector of coordinates for the bottom-left corner.
     * @param topRightCorner Vector of coordinates for the top-right corner.
     * @return Pointer to a RingModel representing the patch influence zone.
     */
    const RingModel* generateInfluenceZone(const std::vector<double> &bottomLeftCorner, const std::vector<double> &topRightCorner);

public:
    /**
     * @brief Constructs a CubicPatch from a JSON configuration.
     * @param shapeConfig JSON configuration specifying the cuboid corners.
     */
    CubicPatch(const nlohmann::json &shapeConfig);
    
    /**
     * @brief Constructs a CubicPatch from two corner points.
     * @param bottomLeftCorner Vector of coordinates for the bottom-left corner.
     * @param topRightCorner Vector of coordinates for the top-right corner.
     */
    CubicPatch(const std::vector<double>& bottomLeftCorner, const std::vector<double>& topRightCorner);
    
    /**
     * @brief Destructor.
     */
    virtual ~CubicPatch();

    /**
     * @brief Deleted copy constructor (CubicPatch is non-copyable).
     */
    CubicPatch(const CubicPatch&) = delete;
    
    /**
     * @brief Deleted copy assignment operator (CubicPatch is non-copyable).
     */
    CubicPatch& operator=(const CubicPatch&) = delete;

    /**
     * @brief Returns the type of the patch shape.
     * @return Type Enum value indicating the patch shape type (Cubic).
     */
    Type getType() const;
};

#endif /* CUBIC_PATCH_H_ */