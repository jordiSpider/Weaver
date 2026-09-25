
#include "App/Model/IBM/Landscape/Map/Geometry/Geometry.h"

using namespace std;




BoxModel Geometry::makeBox(const PointMap &position, const PreciseDouble &size, const bool applyEpsilon)
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

    return BoxModel(minCorner, maxCorner);
}

PreciseDouble Geometry::calculateDistanceBetweenPoints(const PointContinuous& pointA, const PointContinuous& pointB)
{
    return boost::geometry::distance(pointA, pointB);
}

PreciseDouble Geometry::calculateDistanceBetweenPointAndPolygon(const PointContinuous& point, const RingModel& polygon)
{
    if(Geometry::withinPolygon(point, polygon))
    {
        return 0.0;
    }
    else
    {
        return boost::geometry::distance(point, polygon);
    }
}



BoxModel Geometry::makeBoxEffectiveArea(const PointMap &position, const PreciseDouble &size)
{
    return makeBox(position, size, true);
}

RingModel Geometry::makeSphere(const PointContinuous &center, const PreciseDouble &radius)
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

    return RingModel(result[0].outer());;
}

RingModel Geometry::calculateIntersection(const RingModel& objA, const RingModel& objB)
{
    vector<PolygonModel> intersection;
	boost::geometry::intersection(objA, objB, intersection);

    if(intersection.empty())
    {
        return RingModel();
    }
    else
    {
        return RingModel(intersection[0].outer());
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

bool Geometry::withinPolygon(const PointContinuous &point, const RingModel& polygon)
{
    return boost::geometry::within(point, polygon);
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

PointContinuous Geometry::generateRandomPointOnBox(const RingModel& box)
{
    #if DIMENSIONS == 3
    throwLineInfoException("Not implemented method for 3D");
    #endif

    auto boundingBox = boost::geometry::return_envelope<BoxModel>(box);

    PreciseDouble randomX = Random::randomUniform(boost::geometry::get<0>(boundingBox.min_corner()), (boost::geometry::get<0>(boundingBox.max_corner())));
    PreciseDouble randomY = Random::randomUniform(boost::geometry::get<1>(boundingBox.min_corner()), (boost::geometry::get<1>(boundingBox.max_corner())));
    
    return PointContinuous(randomX.getValue(), randomY.getValue());
}

PointContinuous Geometry::generateRandomPointOnPolygon(const RingModel& area)
{
    #if DIMENSIONS == 3
    throwLineInfoException("Not implemented method for 3D");
    #endif

    auto boundingBox = boost::geometry::return_envelope<BoxModel>(area);

    PointContinuous randomPoint;
    do 
    {
        PreciseDouble randomX = Random::randomUniform(boost::geometry::get<0>(boundingBox.min_corner()), (boost::geometry::get<0>(boundingBox.max_corner())));
        PreciseDouble randomY = Random::randomUniform(boost::geometry::get<1>(boundingBox.min_corner()), (boost::geometry::get<1>(boundingBox.max_corner())));
        randomPoint = PointContinuous(randomX.getValue(), randomY.getValue());
    } 
    while(!boost::geometry::within(randomPoint, area));

    return randomPoint;
}

Coverage Geometry::checkFirstCoverageLevelBySecond(const RingModel& first, const RingModel& second, const bool applyIntersection)
{
    return checkCoverageLevel(calculateFirstCoveragePercentBySecond(first, second, applyIntersection));
}

bool Geometry::fullCoveredBySphere(const RingModel& area, const PointContinuous &center, const PreciseDouble &radius)
{
    const double radiusSq = radius.getValue() * radius.getValue();

    for(const auto &corner : area)
    {
		if (!pointInCircle(center.get<0>(), center.get<1>(), radiusSq, corner.get<0>(), corner.get<1>()))
		{
			return false;
		}
    }

    return true;
}

Coverage Geometry::checkCoveredLevelBySphere(const RingModel& area, const PointContinuous &center, const PreciseDouble &radius)
{
    unsigned char pointsInsideSphere = 0u;

    for(const auto &corner : area)
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
    else if(pointsInsideSphere == area.size())
    {
        return Coverage::Full;
    }
    else
    {
        return Coverage::Partial;
    }
}

PreciseDouble Geometry::calculateCoveragePercentBySphere(const RingModel& area, const PointContinuous& center, const PreciseDouble& radius)
{
    if (area.empty())
    {
        return 0.0;
    }

    // 1. Obtener los límites del cuadrado (celda) de forma rápida
    auto boxFirst = boost::geometry::return_envelope<BoxModel>(area);
    const double minX = boxFirst.min_corner().get<0>();
    const double maxX = boxFirst.max_corner().get<0>();
    const double minY = boxFirst.min_corner().get<1>();
    const double maxY = boxFirst.max_corner().get<1>();

    const double cellWidth = maxX - minX;
    const double cellHeight = maxY - minY;

    // 2. Obtener el centro y el radio del círculo de interacción
    const double circleX = center.get<0>();
    const double circleY = center.get<1>();
    const double radiusSq = radius.getValue() * radius.getValue();

    // 3. Test de colisión rápido AABB vs Círculo (Fase ancha)
    // Encontrar el punto más cercano del cuadrado al centro del círculo
    const double closestX = std::max(minX, std::min(circleX, maxX));
    const double closestY = std::max(minY, std::min(circleY, maxY));

    const double distXSq = (circleX - closestX) * (circleX - closestX);
    const double distYSq = (circleY - closestY) * (circleY - closestY);

    // Si el punto más cercano está fuera del radio, la intersección es 0
    if ((distXSq + distYSq) > radiusSq)
    {
        return 0.0;
    }

    // Test de contención total (Si las 4 esquinas de la celda están dentro, cobertura 100%)
    if (pointInCircle(circleX, circleY, radiusSq, minX, minY) && pointInCircle(circleX, circleY, radiusSq, maxX, minY) &&
        pointInCircle(circleX, circleY, radiusSq, minX, maxY) && pointInCircle(circleX, circleY, radiusSq, maxX, maxY))
    {
        return 1.0;
    }

    // 4. Fase Estrecha: Muestreo por Rejilla (8x8 = 64 puntos distribuidos uniformemente)
    // Esto da una resolución de pasos del 1.5625% muy precisa y ultra veloz.
    constexpr int GRID_SIZE = 8;
    constexpr double INV_TOTAL_POINTS = 1.0 / (GRID_SIZE * GRID_SIZE);
    int pointsInside = 0;

    // Calculamos los saltos de la rejilla para que queden centrados en sus micro-cuadrantes
    const double stepX = cellWidth / GRID_SIZE;
    const double stepY = cellHeight / GRID_SIZE;
    const double startX = minX + stepX * 0.5;
    const double startY = minY + stepY * 0.5;

    for (int i = 0; i < GRID_SIZE; ++i)
    {
        const double px = startX + i * stepX;
        for (int j = 0; j < GRID_SIZE; ++j)
        {
            const double py = startY + j * stepY;
            if (((px - circleX) * (px - circleX) + (py - circleY) * (py - circleY)) <= radiusSq)
            {
                pointsInside++;
            }
        }
    }

    return static_cast<double>(pointsInside) * INV_TOTAL_POINTS;
}

bool Geometry::pointInsideBox(const PointContinuous& point, const RingModel& obj)
{
    auto box = boost::geometry::return_envelope<BoxModel>(obj);

    const double x = point.get<0>();
    const double y = point.get<1>();
    const double minX = box.min_corner().get<0>();
    const double maxX = box.max_corner().get<0>();
    const double minY = box.min_corner().get<1>();
    const double maxY = box.max_corner().get<1>();
    return (x >= minX && x <= maxX && y >= minY && y <= maxY);
}

bool Geometry::pointInsideSphere(const PointContinuous& point, const PointContinuous &center, const PreciseDouble &radius)
{
	double radiusSq = radius.getValue() * radius.getValue();
	return pointInCircle(center.get<0>(), center.get<1>(), radiusSq, point.get<0>(), point.get<1>());
}

PreciseDouble Geometry::calculateArea(const RingModel& obj)
{
    return boost::geometry::area(obj);
}

PreciseDouble Geometry::calculateFirstCoveragePercentBySecond(const RingModel& first, const RingModel& second, const bool applyIntersection)
{
    if(first.empty() || second.empty())
    {
        return 0.0;
    }

    if(applyIntersection)
    {
        RingModel intersection = calculateIntersection(first, second);

        if(intersection.empty())
        {
            return 0.0;
        }
        else
        {
            return calculateArea(intersection) / calculateArea(first);
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

bool Geometry::pointInCircle(double circleX, double circleY, double radiusSq, double px, double py) {
    return ((px - circleX) * (px - circleX) + (py - circleY) * (py - circleY)) <= radiusSq;
}
