
#include "App/Model/IBM/Landscape/Map/Geometry/Geometry.h"

using namespace std;




BoxModel* Geometry::makeBox(const PointMap &position, const PreciseDouble &size, const bool applyEpsilon)
{
    PointContinuous minCorner;
    boost::geometry::assign_zero(minCorner);

    for(unsigned char i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();
        setPositionAxisValue(minCorner, i, static_cast<double>(position.get(axis)) * size);
    }

    PointContinuous maxCorner = minCorner;
    boost::geometry::add_value(maxCorner, ((applyEpsilon) ? size-PreciseDouble::EPS : size).getValue());

    return new BoxModel(minCorner, maxCorner);
}

PreciseDouble Geometry::calculateDistanceBetweenPoints(const PointContinuous& pointA, const PointContinuous& pointB)
{
    return boost::geometry::distance(pointA, pointB);
}

PreciseDouble Geometry::calculateDistanceBetweenPointAndPolygon(const PointContinuous& point, const RingModel& polygon)
{
    if(Geometry::withinPolygon(point, &polygon))
    {
        return 0.0;
    }
    else
    {
        return boost::geometry::distance(point, polygon);
    }
}



BoxModel* Geometry::makeBoxEffectiveArea(const PointMap &position, const PreciseDouble &size)
{
    return makeBox(position, size, true);
}

RingModel* Geometry::makeSphere(const PointContinuous &center, const PreciseDouble &radius)
{
    boost::geometry::strategy::buffer::point_circle point_strategy(POINTS_PER_CIRCLE);
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(radius.getValue());
    boost::geometry::strategy::buffer::join_round join_strategy(POINTS_PER_CIRCLE);
    boost::geometry::strategy::buffer::end_round end_strategy(POINTS_PER_CIRCLE);
    boost::geometry::strategy::buffer::side_straight side_strategy;

    boost::geometry::model::multi_polygon<PolygonModel> result;
    boost::geometry::buffer(
        center, result, distance_strategy, side_strategy,
        join_strategy, end_strategy, point_strategy
    );

    return new RingModel(result[0].outer());;
}

RingModel* Geometry::calculateIntersection(const RingModel* const objA, const RingModel* const objB)
{
    vector<PolygonModel> intersection;
	boost::geometry::intersection(*objA, *objB, intersection);

    if(intersection.empty())
    {
        return nullptr;
    }
    else
    {
        return new RingModel(intersection[0].outer());
    }
}

PointContinuous Geometry::calculateClosestPoint(const RingModel& polygon, const LineStringModel& line)
{
    SegmentModel segment;
    boost::geometry::closest_points(polygon, line, segment);

    LineStringModel closestLine;
    boost::geometry::convert(segment, closestLine);

    PointContinuous closestPoint;
    boost::geometry::assign_zero(closestPoint);
    PreciseDouble closestPointDistance = DBL_MAX;

    for(const auto &point : closestLine)
    {
        PreciseDouble pointDistance = boost::geometry::distance(point, polygon);

        if(pointDistance < closestPointDistance)
        {
            closestPoint = point;
            closestPointDistance = pointDistance;
        }
    }

    return closestPoint;
}

bool Geometry::withinPolygon(const PointContinuous &point, const RingModel* const polygon)
{
    return boost::geometry::within(point, *polygon);
}

PointContinuous Geometry::calculateLineStringPointAtDistance(const PointContinuous &initialPoint, const PointContinuous &finalPoint, const PreciseDouble& distance)
{
    // Calculate the direction vector from initialPoint to finalPoint
    vector<PreciseDouble> directionVector(DIMENSIONS);

    for(unsigned char i = 0; i < directionVector.size(); i++)
    {
        directionVector[i] = getPositionAxisValue(finalPoint, i) - getPositionAxisValue(initialPoint, i);
    }

    // Calculate the length of the line segment
    PreciseDouble radicand = 0.0;

    for(unsigned int i = 0; i < directionVector.size(); i++)
    {
        radicand += pow(directionVector[i], 2);
    }

    PreciseDouble length = sqrt(radicand);

    // Calculate the unit direction vector
    vector<PreciseDouble> unitDirectionVector(DIMENSIONS);

    for(unsigned int i = 0; i < unitDirectionVector.size(); i++)
    {
        unitDirectionVector[i] = directionVector[i] / length;
    }

    // Calculate the new point at the specified distance from initialPoint
    PointContinuous newPoint;
    boost::geometry::assign_zero(newPoint);

    for(unsigned char i = 0; i < DIMENSIONS; i++)
    {
        setPositionAxisValue(newPoint, i, getPositionAxisValue(initialPoint, i) + unitDirectionVector[i] * distance);
    }


    return newPoint;
}

PointContinuous Geometry::generateRandomPointOnBox(const RingModel* const box)
{
    #if DIMENSIONS == 3
    throwLineInfoException("Not implemented method for 3D");
    #endif

    auto boundingBox = boost::geometry::return_envelope<BoxModel>(*box);

    PreciseDouble randomX = Random::randomUniform(boost::geometry::get<0>(boundingBox.min_corner()), (boost::geometry::get<0>(boundingBox.max_corner())));
    PreciseDouble randomY = Random::randomUniform(boost::geometry::get<1>(boundingBox.min_corner()), (boost::geometry::get<1>(boundingBox.max_corner())));
    
    return PointContinuous(randomX.getValue(), randomY.getValue());
}

PointContinuous Geometry::generateRandomPointOnPolygon(const RingModel* const area)
{
    #if DIMENSIONS == 3
    throwLineInfoException("Not implemented method for 3D");
    #endif

    auto boundingBox = boost::geometry::return_envelope<BoxModel>(*area);

    PointContinuous randomPoint;
    do 
    {
        PreciseDouble randomX = Random::randomUniform(boost::geometry::get<0>(boundingBox.min_corner()), (boost::geometry::get<0>(boundingBox.max_corner())));
        PreciseDouble randomY = Random::randomUniform(boost::geometry::get<1>(boundingBox.min_corner()), (boost::geometry::get<1>(boundingBox.max_corner())));
        randomPoint = PointContinuous(randomX.getValue(), randomY.getValue());
    } 
    while(!boost::geometry::within(randomPoint, *area));

    return randomPoint;
}

Coverage Geometry::checkFirstCoverageLevelBySecond(const RingModel* const first, const RingModel* const second, const bool applyIntersection)
{
    return checkCoverageLevel(calculateFirstCoveragePercentBySecond(first, second, applyIntersection));
}

bool Geometry::fullCoveredBySphere(const RingModel* const area, const PointContinuous &center, const PreciseDouble &radius)
{
    for(const auto &corner : *area)
    {
        if(calculateDistanceBetweenPoints(center, corner) > radius)
        {
            return false;
        }
    }

    return true;
}

Coverage Geometry::checkCoveredLevelBySphere(const RingModel* const area, const PointContinuous &center, const PreciseDouble &radius)
{
    unsigned char pointsInsideSphere = 0u;

    for(const auto &corner : *area)
    {
        if(Geometry::pointInsideSphere(corner, center, radius))
        {
            pointsInsideSphere++;
        }
    }

    if(pointsInsideSphere == 0)
    {
        if(Geometry::withinPolygon(center, area))
        {
            return Coverage::Partial;
        }
        else
        {
            return Coverage::Null;
        }
    }
    else if(pointsInsideSphere == area->size())
    {
        return Coverage::Full;
    }
    else
    {
        return Coverage::Partial;
    }
}

bool Geometry::pointInsideSphere(const PointContinuous& point, const PointContinuous &center, const PreciseDouble &radius)
{
    return Geometry::calculateDistanceBetweenPoints(point, center) <= radius;
}

PreciseDouble Geometry::calculateArea(const RingModel* const obj)
{
    return boost::geometry::area(*obj);
}

PreciseDouble Geometry::calculateFirstCoveragePercentBySecond(const RingModel* const first, const RingModel* const second, const bool applyIntersection)
{
    if(first == nullptr || second == nullptr)
    {
        return 0.0;
    }

    if(applyIntersection)
    {
        RingModel* intersection = calculateIntersection(first, second);

        if(intersection == nullptr)
        {
            return 0.0;
        }
        else
        {
            PreciseDouble coveragePercent = calculateArea(intersection) / calculateArea(first);

            delete intersection;

            return coveragePercent;
        }
    }
    else
    {
        return calculateArea(second) / calculateArea(first);
    }
}

Coverage Geometry::checkCoverageLevel(const PreciseDouble& percent)
{
    if(percent >= 1.0)
	{
		return Coverage::Full;
	}
	else 
	{
		if(percent >= 0.5)
		{
			return Coverage::Over50Percent;
		}
		else
		{
            if(percent > 0.0)
			{
				return Coverage::Partial;
			}
			else 
			{
                return Coverage::Null;
			}
		}
	}
}
