/**
 * @file Geometry.h
 * @brief Defines the Geometry class and type aliases for spatial operations.
 *
 * This file provides geometric operations for ecological simulations, including
 * creation of boxes, spheres, polygons, intersections, distance calculations,
 * coverage analysis, and random point generation within geometric shapes.
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <memory>
#include <vector>
#include <boost/exception/all.hpp>
#include <boost/geometry/geometry.hpp>
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
#include <boost/geometry/io/dsv/write.hpp>
#include <fstream>
#include <ostream>

#include "App/Model/IBM/Landscape/Map/Points/PointContinuous.h"
#include "App/Model/IBM/Landscape/Map/Geometry/Coverage.h"
#include "App/Model/IBM/Landscape/Map/Points/PointMap.h"
#include "App/Model/IBM/Maths/Random.h"
#include "Misc/Types.h"


/// @brief Box model using boost::geometry.
using BoxModel = boost::geometry::model::box<PointContinuous>;
/// @brief Ring model using boost::geometry.
using RingModel = boost::geometry::model::ring<PointContinuous>;
/// @brief Polygon model using boost::geometry.
using PolygonModel = boost::geometry::model::polygon<PointContinuous>;
/// @brief Line string model using boost::geometry.
using LineStringModel = boost::geometry::model::linestring<PointContinuous>;
/// @brief Segment model using boost::geometry.
using SegmentModel = boost::geometry::model::segment<PointContinuous>;


/**
 * @class Geometry
 * @brief Provides static geometric operations for spatial modeling.
 *
 * The Geometry class includes methods for creating geometric shapes,
 * calculating distances, intersections, areas, and coverage levels, and
 * generating random points within shapes. All methods are static.
 */
class Geometry
{
private:
    /**
     * @brief Creates a box given a position and size.
     * @param position Center position of the box.
     * @param size Size of the box along each axis.
     * @param applyEpsilon Whether to apply a small epsilon to avoid precision issues.
     * @return Pointer to the created BoxModel.
     */
    static BoxModel* makeBox(const PointMap &position, const PreciseDouble &size, const bool applyEpsilon=false);

    /**
     * @brief Calculates the area of a ring.
     * @param obj Pointer to the ring object.
     * @return Area of the ring.
     */
    static PreciseDouble calculateArea(const RingModel* const obj);

    /**
     * @brief Determines the coverage level based on a percentage.
     * @param percent Coverage percentage (0-100).
     * @return Coverage enum indicating coverage level.
     */
    static Coverage checkCoverageLevel(const PreciseDouble& percent);

public:
    /// @brief Number of points used to approximate a circle.
    static constexpr const unsigned int POINTS_PER_CIRCLE = 45;

    /**
     * @brief Creates a box representing an effective area.
     * @param position Center position of the box.
     * @param size Size of the box.
     * @return Pointer to the BoxModel representing the effective area.
     */
    static BoxModel* makeBoxEffectiveArea(const PointMap &position, const PreciseDouble &size);
    
    /**
     * @brief Creates a spherical ring.
     * @param center Center of the sphere.
     * @param radius Radius of the sphere.
     * @return Pointer to a RingModel approximating the sphere.
     */
    static RingModel* makeSphere(const PointContinuous &center, const PreciseDouble &radius);

    /**
     * @brief Calculates the intersection of two rings.
     * @param objA Pointer to the first ring.
     * @param objB Pointer to the second ring.
     * @return Pointer to a RingModel representing the intersection area.
     */
    static RingModel* calculateIntersection(const RingModel* const objA, const RingModel* const objB);

    /**
     * @brief Calculates the Euclidean distance between two points.
     * @param pointA First point.
     * @param pointB Second point.
     * @return Distance between the two points.
     */
    static PreciseDouble calculateDistanceBetweenPoints(const PointContinuous& pointA, const PointContinuous& pointB);
    
    /**
     * @brief Calculates the shortest distance from a point to a polygon.
     * @param point Point to measure from.
     * @param polygon Polygon to measure to.
     * @return Minimum distance between point and polygon.
     */
    static PreciseDouble calculateDistanceBetweenPointAndPolygon(const PointContinuous& point, const RingModel& polygon);
    
    /**
     * @brief Finds the closest point on a line to a polygon.
     * @param polygon Polygon to check.
     * @param line LineString to check.
     * @return Closest point on the line to the polygon.
     */
    static PointContinuous calculateClosestPoint(const RingModel& polygon, const LineStringModel& line);

    /**
     * @brief Calculates a point along a line segment at a given distance from the initial point.
     * @param initialPoint Starting point of the line.
     * @param finalPoint Ending point of the line.
     * @param distance Distance from the initial point along the line.
     * @return Point at the specified distance along the line.
     */
    static PointContinuous calculateLineStringPointAtDistance(const PointContinuous &initialPoint, const PointContinuous &finalPoint, const PreciseDouble& distance);

    /**
     * @brief Checks if a point is within a polygon.
     * @param point Point to check.
     * @param polygon Pointer to the polygon.
     * @return True if the point lies inside the polygon.
     */
    static bool withinPolygon(const PointContinuous &point, const RingModel* const polygon);

    /**
     * @brief Generates a random point within a box.
     * @param box Pointer to the box.
     * @return Random point inside the box.
     */
    static PointContinuous generateRandomPointOnBox(const RingModel* const box);
    
    /**
     * @brief Generates a random point within a polygon.
     * @param area Pointer to the polygon area.
     * @return Random point inside the polygon.
     */
    static PointContinuous generateRandomPointOnPolygon(const RingModel* const area);

    /**
     * @brief Determines coverage level of the first ring by the second.
     * @param first Pointer to the first ring.
     * @param second Pointer to the second ring.
     * @param applyIntersection Whether to consider only the intersection area.
     * @return Coverage enum representing the coverage level.
     */
    static Coverage checkFirstCoverageLevelBySecond(const RingModel* const first, const RingModel* const second, const bool applyIntersection);
    
    /**
     * @brief Checks if an area is fully covered by a sphere.
     * @param area Pointer to the area polygon.
     * @param center Center of the sphere.
     * @param radius Sphere radius.
     * @return True if fully covered.
     */
    static bool fullCoveredBySphere(const RingModel* const area, const PointContinuous &center, const PreciseDouble &radius);
    
    /**
     * @brief Determines coverage level of an area by a sphere.
     * @param area Pointer to the area polygon.
     * @param center Center of the sphere.
     * @param radius Sphere radius.
     * @return Coverage enum representing the coverage level.
     */
    static Coverage checkCoveredLevelBySphere(const RingModel* const area, const PointContinuous &center, const PreciseDouble &radius);
    
    /**
     * @brief Checks if a point is inside a sphere.
     * @param point Point to check.
     * @param center Sphere center.
     * @param radius Sphere radius.
     * @return True if the point lies inside the sphere.
     */
    static bool pointInsideSphere(const PointContinuous& point, const PointContinuous &center, const PreciseDouble &radius);

    /**
     * @brief Calculates the coverage percentage of the first ring by the second.
     * @param first Pointer to the first ring.
     * @param second Pointer to the second ring.
     * @param applyIntersection Whether to consider only the intersection area.
     * @return Coverage percentage (0-100).
     */
    static PreciseDouble calculateFirstCoveragePercentBySecond(const RingModel* const first, const RingModel* const second, const bool applyIntersection);
};

#endif /* GEOMETRY_H_ */