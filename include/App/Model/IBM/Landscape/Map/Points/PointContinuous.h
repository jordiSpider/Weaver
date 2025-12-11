/**
 * @file PointContinuous.h
 * @brief Defines the PointContinuous type and utility functions for 3D continuous points.
 *
 * This file provides a typedef for 3D continuous points using Boost.Geometry, as well
 * as functions to access and modify individual axis values. Serialization support is
 * also provided via Boost.Serialization.
 */

#ifndef POINT_CONTINUOUS_H_
#define POINT_CONTINUOUS_H_

#include <vector>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/core/cs.hpp>


#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Maths/PreciseDouble.h"


/**
 * @typedef PointContinuous
 * @brief Represents a continuous point in DIMENSIONS-dimensional Cartesian space.
 *
 * This is a Boost.Geometry point type with double precision coordinates.
 */
using PointContinuous = boost::geometry::model::point<double, DIMENSIONS, boost::geometry::cs::cartesian>;

/**
 * @brief Gets the value of a specific axis from a PointContinuous.
 *
 * @param position The point from which to retrieve the value.
 * @param axis The axis index (0 = X, 1 = Y, 2 = Z).
 * @return PreciseDouble Value of the requested axis.
 */
PreciseDouble getPositionAxisValue(const PointContinuous& position, const unsigned char axis);

/**
 * @brief Sets the value of a specific axis in a PointContinuous.
 *
 * @param position The point to modify.
 * @param axis The axis index (0 = X, 1 = Y, 2 = Z).
 * @param newValue The new value to assign to the axis.
 */
void setPositionAxisValue(PointContinuous& position, const unsigned char axis, const PreciseDouble& newValue);

namespace boost {
    namespace serialization {
        /**
         * @brief Serializes a PointContinuous object.
         *
         * @tparam Archive The archive type (input/output archive).
         * @param ar Archive instance.
         * @param pointContinuous The point to serialize.
         * @param version Serialization version.
         */
        template<class Archive>
        void serialize(Archive &ar, PointContinuous &pointContinuous, const unsigned int version);

        /**
         * @brief Serializes a pointer to a PointContinuous object.
         *
         * @tparam Archive The archive type (input/output archive).
         * @param ar Archive instance.
         * @param pointContinuousPtr Pointer to the point to serialize.
         * @param version Serialization version.
         */
        template<class Archive>
        void serialize(Archive &ar, PointContinuous* &pointContinuousPtr, const unsigned int version);
    }
}

#endif /* POINT_CONTINUOUS_H_ */