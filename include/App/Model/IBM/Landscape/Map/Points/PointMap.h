/**
 * @file PointMap.h
 * @brief Defines the PointMap class for discrete points in a Map.
 *
 * This file contains the PointMap class, which represents a point in the
 * discrete coordinate space of a Map. It provides access to axis values,
 * comparison operators, and serialization support.
 */

#ifndef POINT_MAP_H_
#define POINT_MAP_H_

#include <string>
#include <magic_enum/magic_enum.hpp>
#include <vector>
#include <iostream>
#include <memory>


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "App/Model/IBM/Landscape/Map/Points/Axis.h"
#include "Exceptions/LineInfoException.h"


class Map;


/**
 * @class PointMap
 * @brief Represents a point in a discrete map coordinate system.
 *
 * The PointMap class stores the position along each axis as unsigned integers.
 * It provides methods to get and set values, compare points, and serialize
 * them for persistence or communication.
 */
class PointMap {
protected:
	/// @brief Stores the axis values for each dimension.
	std::vector<unsigned int> axisValues;

public:
	/**
     * @brief Creates a new PointMap instance based on a Map.
     * @param map The Map instance used to determine point properties.
     * @return Pointer to a new PointMap instance.
     */
	static PointMap* createInstance(const Map& map);

	/**
     * @brief Default constructor, initializes an empty PointMap.
     */
	PointMap();

	/**
     * @brief Constructs a PointMap with given axis values.
     * @param axisValues Vector of axis values for each dimension.
     */
	PointMap(const std::vector<unsigned int> &axisValues);
	
	/**
     * @brief Copy constructor.
     * @param other PointMap to copy from.
     */
	PointMap(const PointMap &other);

	/**
     * @brief Destructor.
     */
	virtual ~PointMap();

	/**
     * @brief Gets the value of a specific axis.
     * @param axis Axis to query.
     * @return Value along the requested axis.
     */
	unsigned int get(const Axis &axis) const;
	
	/**
     * @brief Sets the value of a specific axis.
     * @param axis Axis to modify.
     * @param newValue New value for the axis.
     */
	void set(const Axis & axis, const unsigned int newValue);
	
	/**
     * @brief Returns all axis values.
     * @return Vector of unsigned integers representing axis values.
     */
	const std::vector<unsigned int>& getAxisValues() const;

	/**
     * @brief Equality operator.
     * @param other Another PointMap to compare with.
     * @return True if all axis values are equal, false otherwise.
     */
	bool operator==(const PointMap& other) const;
	
	/**
     * @brief Inequality operator.
     * @param other Another PointMap to compare with.
     * @return True if any axis value differs, false otherwise.
     */
	bool operator!=(const PointMap& other) const;
	
	/**
     * @brief Outputs the point to an output stream.
     * @param os Output stream.
     * @param point PointMap instance to print.
     * @return Reference to the output stream.
     */
	friend std::ostream& operator<<(std::ostream& os, const PointMap& point);

	/**
     * @brief Serializes the PointMap object.
     * @tparam Archive Archive type (e.g., binary_oarchive, binary_iarchive).
     * @param ar Archive instance.
     * @param version Serialization version.
     */
	template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* POINT_MAP_H_ */