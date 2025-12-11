/**
 * @file PointSpatialTree.h
 * @brief Defines the PointSpatialTree class for representing spatial tree coordinates.
 */

#ifndef POINT_SPATIAL_TREE_H_
#define POINT_SPATIAL_TREE_H_

#include <vector>
#include <iostream>


#include <boost/serialization/export.hpp>


#include "App/Model/IBM/Landscape/Map/Points/PointMap.h"
#include "App/Model/IBM/Landscape/Map/Points/Axis.h"


/**
 * @class PointSpatialTree
 * @brief Represents a point in a spatial tree with a specific depth.
 *
 * Extends PointMap by adding the depth of the point in a spatial tree structure.
 * This class is used to locate cells in hierarchical spatial trees for efficient
 * terrain and landscape management.
 */
class PointSpatialTree : public PointMap {
protected:
	/// Depth of the point in the spatial tree hierarchy.
	unsigned int depth;

public:
	/// Default constructor. Initializes a point with depth 0.
	PointSpatialTree();

	/**
     * @brief Constructor with axis values and depth.
     * @param axisValues Coordinates along each axis.
     * @param depth Depth of the point in the spatial tree.
     */
	PointSpatialTree(const std::vector<unsigned int> &axisValues, const unsigned int &depth);

	/**
     * @brief Copy constructor.
     * @param other The PointSpatialTree to copy from.
     */
	PointSpatialTree(const PointSpatialTree &other);

	/**
     * @brief Copy assignment operator.
     * @param other The PointSpatialTree to assign from.
     * @return Reference to this PointSpatialTree.
     */
	PointSpatialTree& operator=(const PointSpatialTree& other);

	/// Destructor.
	virtual ~PointSpatialTree();

	/**
     * @brief Get the depth of the point in the spatial tree.
     * @return Depth as an unsigned integer.
     */
	unsigned int getDepth() const;

	/**
     * @brief Equality comparison operator.
     * @param other Another PointSpatialTree to compare with.
     * @return True if both points have the same coordinates and depth.
     */
	bool operator==(const PointSpatialTree& other) const;

	/**
     * @brief Inequality comparison operator.
     * @param other Another PointSpatialTree to compare with.
     * @return True if points differ in coordinates or depth.
     */
	bool operator!=(const PointSpatialTree& other) const;

	/**
     * @brief Output stream operator for printing the point.
     * @param os Output stream.
     * @param point PointSpatialTree to print.
     * @return Reference to the output stream.
     */
	friend std::ostream& operator<<(std::ostream& os, const PointSpatialTree& point);

	/**
     * @brief Serialization function for saving/loading the object.
     * @tparam Archive Type of the archive.
     * @param ar Archive object.
     * @param version Serialization version.
     */
	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* POINT_SPATIAL_TREE_H_ */