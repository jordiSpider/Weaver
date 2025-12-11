/**
 * @file HomogeneousPatch.h
 * @brief Defines the HomogeneousPatch class, a type of PatchShape with uniform influence.
 *
 * This file contains the declaration of the HomogeneousPatch class, which represents
 * a patch in the landscape whose influence is uniform across its area.
 */

#ifndef HOMOGENEOUS_PATCH_H_
#define HOMOGENEOUS_PATCH_H_

#include "App/Model/IBM/Landscape/Map/Patches/Shape/PatchShape.h"

/**
 * @class HomogeneousPatch
 * @brief Represents a patch with homogeneous (uniform) influence.
 *
 * The HomogeneousPatch class is a concrete implementation of PatchShape.
 * All landscape cells within the patch are equally affected, with full coverage
 * inside the influence zone.
 */
class HomogeneousPatch : public PatchShape {
public:
    /**
     * @brief Constructs a HomogeneousPatch with default settings.
     */
    HomogeneousPatch();

    /**
     * @brief Destructor.
     */
    virtual ~HomogeneousPatch();

    /**
     * @brief Deleted copy constructor (HomogeneousPatch is non-copyable).
     */
    HomogeneousPatch(const HomogeneousPatch&) = delete;
    
    /**
     * @brief Deleted copy assignment operator (HomogeneousPatch is non-copyable).
     */
    HomogeneousPatch& operator=(const HomogeneousPatch&) = delete;

    /**
     * @brief Checks the coverage level of a landscape cell by this homogeneous patch.
     * @param cellBounds Bounds of the landscape cell to check.
     * @return Coverage Enum indicating Null, Partial, Over50Percent, or Full coverage.
     */
    Coverage checkCoverage(const RingModel* const cellBounds) const override;

    /**
     * @brief Returns the type of this patch shape.
     * @return Type Enum value PatchShape::Homogeneous.
     */
    Type getType() const;
};

#endif /* HOMOGENEOUS_PATCH_H_ */