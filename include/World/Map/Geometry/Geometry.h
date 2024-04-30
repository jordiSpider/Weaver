#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <memory>
#include <vector>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/ring.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/arithmetic/arithmetic.hpp>
#include <boost/geometry/strategies/cartesian/buffer_point_circle.hpp>
#include <boost/geometry/strategies/agnostic/buffer_distance_symmetric.hpp>
#include <boost/geometry/strategies/cartesian/buffer_join_round.hpp>
#include <boost/geometry/strategies/cartesian/buffer_end_round.hpp>
#include <boost/geometry/strategies/cartesian/buffer_side_straight.hpp>
#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/algorithms/convert.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>
#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/within.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/closest_points.hpp>

#include "World/Map/Points/PointContinuous.h"
#include "World/Map/Geometry/Coverage.h"
#include "World/Map/Points/PointMap.h"
#include "Maths/Random.h"



using Box = boost::geometry::model::box<PointContinuous>;
using Ring = boost::geometry::model::ring<PointContinuous>;
using Polygon = boost::geometry::model::polygon<PointContinuous>;
using LineString = boost::geometry::model::linestring<PointContinuous>;
using Segment = boost::geometry::model::segment<PointContinuous>;


class Geometry
{
public:
    static constexpr const unsigned int POINTS_PER_CIRCLE = 45;
    static constexpr const double DOUBLE_EPSILON = 1e-16;

    static std::unique_ptr<Ring> calculateIntersection(const Ring& objA, const Ring& objB);
    static const double calculateArea(const Ring& obj);
    static const double calculateCoveragePercent(const Ring* const objA, const Ring& objB);
    static const Coverage checkCoverage(const Ring* const objA, const Ring& objB);
    static const Coverage checkCoverage(const double& percent);
    static const std::unique_ptr<Box> makeBoxEffectiveArea(const PointMap &position, const double &size);
    static const std::unique_ptr<Box> makeBox(const PointMap &position, const double &size, const bool applyEpsilon=false);
    static std::unique_ptr<Ring> makeSphere(const PointContinuous &center, const double &radius);
    static PointContinuous generateRandomPointOnCircumference(const PointContinuous &center, const double &radius);
    static PointContinuous generateRandomPointOnBox(const Ring &box);
    static PointContinuous generateRandomPointOnPolygon(const Ring &area);
    static double calculateDistanceBetweenPoints(const PointContinuous& pointA, const PointContinuous& pointB);
    static double calculateDistanceBetweenPointAndPolygonNodes(const PointContinuous& point, const Ring& polygon);
    static double calculateMaximumDistanceBetweenPointAndPolygonNodes(const PointContinuous& point, const Ring& polygon);
    static double calculateMinimumDistanceBetweenPointAndPolygonNodes(const PointContinuous& point, const Ring& polygon);
    static PointContinuous calculateClosestPoint(const Ring &polygon, const LineString &line);
    static bool withinPolygon(const PointContinuous &point, const Ring &polygon);
};

#endif /* GEOMETRY_H_ */