/**
 * @file PatchShape.h
 * @brief Defines the PatchShape base class for patches in the landscape.
 *
 * This file contains the PatchShape abstract base class, which represents
 * a generic patch shape in the landscape. Derived classes implement specific
 * geometries such as Cubic, Spherical, Gaussian, or Homogeneous patches.
 */

#ifndef PATCH_SHAPE_H_
#define PATCH_SHAPE_H_

#include <nlohmann/json.hpp>
#include <string>
#include <sstream>

#include "App/Model/IBM/Landscape/Map/Geometry/Geometry.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/EnumClass.h"


/**
 * @class PatchShape
 * @brief Abstract base class representing a patch shape.
 *
 * The PatchShape class provides a common interface for all patch shapes,
 * including the influence zone, description, and coverage checks.
 * Derived classes must implement the getType() method to specify the shape type.
 */
class PatchShape {
protected:
    /// Influence zone of the patch (geometry used for coverage calculations)
    const RingModel* const influenceZone;

    /// Description of the patch shape
    std::string description;

    /**
     * @brief Protected constructor for derived classes.
     * @param influenceZone Pointer to the geometry representing the patch's influence.
     * @param description Textual description of the patch shape.
     */
    PatchShape(const RingModel* const influenceZone, const std::string description);

    /**
     * @brief Deleted copy constructor (PatchShape is non-copyable).
     */
    PatchShape(const PatchShape&) = delete;
    
    /**
     * @brief Deleted copy assignment operator (PatchShape is non-copyable).
     */
    PatchShape& operator=(const PatchShape&) = delete;

    /**
     * @brief Returns the influence zone of the patch.
     * @return Pointer to the RingModel representing the patch influence zone.
     */
    const RingModel* getInfluenceZone() const;    

public:
    /**
     * @brief Types of patch shapes.
     */
    enum Type : unsigned char
    {
        Homogeneous,   /**< Uniform patch */
        Gaussian,      /**< Gaussian-shaped patch */
        Spherical,     /**< Spherical patch */
        Cubic,         /**< Cubic/box-shaped patch */
        RandomGaussian /**< Randomly generated Gaussian patch */
    };

    /**
     * @brief Factory method to create a PatchShape instance from JSON configuration.
     * @param shapeConfig JSON configuration specifying the shape type and parameters.
     * @return Pointer to a dynamically allocated PatchShape object.
     */
    static PatchShape* createInstance(const nlohmann::json &shapeConfig);

    /**
     * @brief Virtual destructor.
     */
    virtual ~PatchShape();

    /**
     * @brief Checks the coverage level of a given cell by this patch.
     * @param cellBounds Pointer to the cell's RingModel bounds.
     * @return Coverage enum value indicating how much of the cell is covered.
     */
    virtual Coverage checkCoverage(const RingModel* const cellBounds) const;
    
    /**
     * @brief Returns the textual description of the patch.
     * @return std::string_view containing the patch description.
     */
    const std::string_view getDescription() const;

    /**
     * @brief Returns the type of the patch shape.
     * @return Type Enum value indicating the patch shape type.
     */
    virtual Type getType() const=0;
};

#endif /* PATCH_SHAPE_H_ */