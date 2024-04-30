
#include "World/Map/Geometry/Geometry.h"

using namespace std;


unique_ptr<Ring> Geometry::calculateIntersection(const Ring& objA, const Ring& objB)
{
    vector<Polygon> intersection;
	boost::geometry::intersection(objA, objB, intersection);

    if(intersection.empty())
    {
        return nullptr;
    }
    else
    {
        return make_unique<Ring>(intersection[0].outer());
    }
}

const double Geometry::calculateArea(const Ring& obj)
{
    return boost::geometry::area(obj);
}

const double Geometry::calculateCoveragePercent(const Ring* const objA, const Ring& objB)
{
    return (objA != nullptr) ? calculateArea(*objA) / calculateArea(objB) : 0.0;
}

const Coverage Geometry::checkCoverage(const Ring* const objA, const Ring& objB)
{
    return checkCoverage(calculateCoveragePercent(objA, objB));
}

const Coverage Geometry::checkCoverage(const double& percent)
{
    if(fabs(1.0 - percent) <= numeric_limits<double>::epsilon())
	{
		return Coverage::Full;
	}
	else 
	{
		if(percent < 0.5)
		{
			if(percent <= numeric_limits<double>::epsilon())
			{
				return Coverage::Null;
			}
			else 
			{
				return Coverage::Partial;
			}
		}
		else
		{
			return Coverage::Over50Percent;
		}
	}
}

const std::unique_ptr<Box> Geometry::makeBoxEffectiveArea(const PointMap &position, const double &size)
{
    return makeBox(position, size, true);
}

const std::unique_ptr<Box> Geometry::makeBox(const PointMap &position, const double &size, const bool applyEpsilon)
{
    PointContinuous minCorner;
    for(unsigned int i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();
        setPositionAxisValue(minCorner, i, position.get(axis) * size);
    }

    PointContinuous maxCorner = minCorner;
    boost::geometry::add_value(maxCorner, (applyEpsilon) ? size-DOUBLE_EPSILON : size);

    return make_unique<Box>(minCorner, maxCorner);
}

unique_ptr<Ring> Geometry::makeSphere(const PointContinuous &center, const double &radius)
{
    boost::geometry::strategy::buffer::point_circle point_strategy(POINTS_PER_CIRCLE);
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(radius);
    boost::geometry::strategy::buffer::join_round join_strategy(POINTS_PER_CIRCLE);
    boost::geometry::strategy::buffer::end_round end_strategy(POINTS_PER_CIRCLE);
    boost::geometry::strategy::buffer::side_straight side_strategy;

    boost::geometry::model::multi_polygon<Polygon> result;
    boost::geometry::buffer(
        center, result, distance_strategy, side_strategy,
        join_strategy, end_strategy, point_strategy
    );

    auto sphere = make_unique<Ring>(result[0].outer());

    return sphere;
}

PointContinuous Geometry::generateRandomPointOnCircumference(const PointContinuous &center, const double &radius)
{
    #if DIMENSIONS == 3
    throwLineInfoException("Not implemented method for 3D");
    #endif

    // Generar un ángulo aleatorio en radianes dentro del rango [0, 2*pi]
    double angle = Random::randomUniform(0.0, 2.0 * M_PI);

    // Calcular las coordenadas x e y del punto en el contorno exterior
    double x = center.get<0>() + radius * cos(angle);
    double y = center.get<1>() + radius * sin(angle);

    return PointContinuous(x, y);
}

PointContinuous Geometry::generateRandomPointOnBox(const Ring &box)
{
    #if DIMENSIONS == 3
    throwLineInfoException("Not implemented method for 3D");
    #endif

    auto boundingBox = boost::geometry::return_envelope<Box>(box);

    double randomX = Random::randomUniform(boost::geometry::get<0>(boundingBox.min_corner()), (boost::geometry::get<0>(boundingBox.max_corner())));
    double randomY = Random::randomUniform(boost::geometry::get<1>(boundingBox.min_corner()), (boost::geometry::get<1>(boundingBox.max_corner())));
    
    return PointContinuous(randomX, randomY);
}

PointContinuous Geometry::generateRandomPointOnPolygon(const Ring &area)
{
    #if DIMENSIONS == 3
    throwLineInfoException("Not implemented method for 3D");
    #endif

    auto boundingBox = boost::geometry::return_envelope<Box>(area);

    PointContinuous randomPoint;
    do 
    {
        double randomX = Random::randomUniform(boost::geometry::get<0>(boundingBox.min_corner()), (boost::geometry::get<0>(boundingBox.max_corner())));
        double randomY = Random::randomUniform(boost::geometry::get<1>(boundingBox.min_corner()), (boost::geometry::get<1>(boundingBox.max_corner())));
        randomPoint = PointContinuous(randomX, randomY);
    } 
    while(!boost::geometry::within(randomPoint, area));

    return randomPoint;
}

double Geometry::calculateDistanceBetweenPoints(const PointContinuous& pointA, const PointContinuous& pointB)
{
    return boost::geometry::distance(pointA, pointB);
}

double Geometry::calculateDistanceBetweenPointAndPolygonNodes(const PointContinuous& point, const Ring& polygon)
{
    return boost::geometry::distance(point, polygon);
}

double Geometry::calculateMaximumDistanceBetweenPointAndPolygonNodes(const PointContinuous& point, const Ring& polygon)
{
    double maximumDistance = 0.0;

    for(const auto &polygonPoint : polygon)
    {
        double pointDistance = Geometry::calculateDistanceBetweenPoints(point, polygonPoint);

        if(maximumDistance < pointDistance)
        {
            maximumDistance = pointDistance;
        }
    }

    return maximumDistance;
}

double Geometry::calculateMinimumDistanceBetweenPointAndPolygonNodes(const PointContinuous& point, const Ring& polygon)
{
    double minimumDistance = 0.0;

    for(const auto &polygonPoint : polygon)
    {
        double pointDistance = Geometry::calculateDistanceBetweenPoints(point, polygonPoint);

        if(minimumDistance > pointDistance)
        {
            minimumDistance = pointDistance;
        }
    }

    return minimumDistance;
}

PointContinuous Geometry::calculateClosestPoint(const Ring &polygon, const LineString &line)
{
    Segment segment;
    boost::geometry::closest_points(polygon, line, segment);

    LineString closestLine;
    boost::geometry::convert(segment, closestLine);

    for(const auto &point : closestLine)
    {
        double pointDistance = boost::geometry::distance(point, polygon);
        if(pointDistance <= numeric_limits<double>::epsilon())
        {
            return point;
        }
    }

    throwLineInfoException("There is no point close to the polygon");
}

bool Geometry::withinPolygon(const PointContinuous &point, const Ring &polygon)
{
    return boost::geometry::within(point, polygon);
}
