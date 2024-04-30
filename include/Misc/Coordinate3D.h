/*
 * point3D.h
 *
 *  Created on: Aug 2, 2012
 *      Author: jrbcast
 */

#ifndef COORDINATE3D_H_
#define COORDINATE3D_H_


#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>


/**
 * @class Coordinate3D
 * @brief A class representing a 3D coordinate with x, y, and z values.
 * @tparam T The type of the coordinate values (e.g., int, float, double).
 */
template<class T>
class Coordinate3D {
private:
	T x; /**<  x coordinate */
    T y; /**<  y coordinate */
    T z; /**<  z coordinate */

	friend class boost::serialization::access;

public:
	/**
    * @brief Constructor for Coordinate3D.
    * @param x The x-coordinate.
    * @param y The y-coordinate.
    * @param z The z-coordinate.
    */
	Coordinate3D(const T &x, const T &y, const T &z) : x(x), y(y), z(z) {}

	/**
    * @brief Get the x-coordinate.
    * @return The x-coordinate.
    */
	const T getX() const { return x; }

	/**
    * @brief Get the y-coordinate.
    * @return The y-coordinate.
    */
	const T getY() const { return y; }

	/**
    * @brief Get the z-coordinate.
    * @return The z-coordinate.
    */
	const T getZ() const { return z; }

	/**
     * @brief Set the x-coordinate.
     * @param newX The new x-coordinate.
     */
    void setX(const T &newX) { x = newX; }

    /**
     * @brief Set the y-coordinate.
     * @param newY The new y-coordinate.
     */
    void setY(const T &newY) { y = newY; }

    /**
     * @brief Set the z-coordinate.
     * @param newZ The new z-coordinate.
     */
    void setZ(const T &newZ) { z = newZ; }

	/**
     * @brief Serialize the object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & x;
        ar & y;
        ar & z;
    }
};

#endif /* COORDINATE3D_H_ */


