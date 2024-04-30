#ifndef POINT_CONTINUOUS_H_
#define POINT_CONTINUOUS_H_

#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/core/cs.hpp>

#include "Exceptions/LineInfoException.h"



using PointContinuous = boost::geometry::model::point<double, DIMENSIONS, boost::geometry::cs::cartesian>;

constexpr const double& getPositionAxisValue(const PointContinuous& position, const unsigned int axis)
{
    #if DIMENSIONS == 2
        switch(axis) {
            case 0: {
                return position.get<0>();
                break;
            }
            case 1: {
                return position.get<1>();
                break;
            }
            default: {
                throwLineInfoException("Default case");
                break;
            }
        }
    #elif DIMENSIONS == 3
        switch(axis) {
            case 0: {
                return position.get<0>();
                break;
            }
            case 1: {
                return position.get<1>();
                break;
            }
            case 2: {
                return position.get<2>();
                break;
            }
            default: {
                throwLineInfoException("Default case");
                break;
            }
        }
    #else
        throwLineInfoException("Invalid DIMENSIONS value");
    #endif
}

constexpr void setPositionAxisValue(PointContinuous& position, const unsigned int axis, const double& newValue)
{
    #if DIMENSIONS == 2
        switch(axis) {
            case 0: {
                return position.set<0>(newValue);
                break;
            }
            case 1: {
                return position.set<1>(newValue);
                break;
            }
            default: {
                throwLineInfoException("Default case");
                break;
            }
        }
    #elif DIMENSIONS == 3
        switch(axis) {
            case 0: {
                return position.set<0>(newValue);
                break;
            }
            case 1: {
                return position.set<1>(newValue);
                break;
            }
            case 2: {
                return position.set<2>(newValue);
                break;
            }
            default: {
                throwLineInfoException("Default case");
                break;
            }
        }
    #else
        throwLineInfoException("Invalid DIMENSIONS value");
    #endif
}

#endif /* POINT_CONTINUOUS_H_ */