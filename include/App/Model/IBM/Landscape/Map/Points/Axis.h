/**
 * @file Axis.h
 * @brief Defines the Axis enum for 3D spatial directions.
 *
 * This enum represents the three principal axes in a 3D Cartesian coordinate system.
 * It is used throughout the simulation for specifying directions or indexing coordinates.
 */

#ifndef AXIS_H_
#define AXIS_H_

/**
 * @enum Axis
 * @brief Enumeration of 3D Cartesian axes.
 */
enum Axis : unsigned char
{
    /// @brief X-axis (horizontal direction).
    X = 0, 
    /// @brief Y-axis (vertical direction).
    Y = 1, 
    /// @brief Z-axis (depth direction).
    Z = 2
};

#endif /* AXIS_H_ */