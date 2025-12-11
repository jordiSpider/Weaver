/**
 * @file Coordinate3D.h
 * @brief Defines a templated 3D coordinate class with Boost.Serialization support.
 *
 * This header provides the Coordinate3D class template, which represents a point or
 * vector in three-dimensional space using a generic numeric type (int, float, double, etc.).
 * It supports getting and setting individual coordinates and serializing the object
 * using Boost.Serialization.
 */

#ifndef COORDINATE3D_H_
#define COORDINATE3D_H_


#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>


/**
 * @class Coordinate3D
 * @brief Represents a 3D coordinate with x, y, and z values.
 *
 * @tparam T Type of the coordinate values (e.g., int, float, double, or custom numeric type).
 *
 * This class encapsulates three-dimensional coordinates and provides getters, setters,
 * and Boost.Serialization support. It can be used to represent points, vectors, or
 * positions in 3D space.
 */
template<class T>
class Coordinate3D {
private:
	T x; /**< The x-coordinate */
    T y; /**< The y-coordinate */
    T z; /**< The z-coordinate */

public:
	/**
     * @brief Constructs a Coordinate3D with given x, y, z values.
     * @param x The x-coordinate.
     * @param y The y-coordinate.
     * @param z The z-coordinate.
     */
	Coordinate3D(const T &x, const T &y, const T &z) : x(x), y(y), z(z) {}

	/**
     * @brief Returns the x-coordinate.
     * @return The x value.
     */
	T getX() const { return x; }

	/**
     * @brief Returns the y-coordinate.
     * @return The y value.
     */
	T getY() const { return y; }

	/**
     * @brief Returns the z-coordinate.
     * @return The z value.
     */
	T getZ() const { return z; }

	/**
     * @brief Sets the x-coordinate.
     * @param newX The new x-coordinate value.
     */
    void setX(const T &newX) { x = newX; }

    /**
     * @brief Sets the y-coordinate.
     * @param newY The new y-coordinate value.
     */
    void setY(const T &newY) { y = newY; }

    /**
     * @brief Sets the z-coordinate.
     * @param newZ The new z-coordinate value.
     */
    void setZ(const T &newZ) { z = newZ; }

	/**
     * @brief Serializes the object using Boost.Serialization.
     * 
     * The function serializes the x, y, and z coordinates to the provided archive.
     * It supports both saving (output archive) and loading (input archive).
     * 
     * @tparam Archive Type of the Boost archive (e.g., binary_oarchive, binary_iarchive).
     * @param ar The archive object to serialize to or deserialize from.
     * @param version Version number of the serialization format (unused).
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        (void)version; // suppress unused parameter warning
        ar & x;
        ar & y;
        ar & z;
    }
};

#endif /* COORDINATE3D_H_ */


