
#include "World/Map/Points/PointMap.h"

using namespace std;



PointMap::PointMap()
    : axisValues(DIMENSIONS, 0)
{
    if(axisValues.size() != DIMENSIONS)
    {
        throwLineInfoException("Point " + to_string(DIMENSIONS) + "D requires exactly " + to_string(DIMENSIONS) + " values");
    }
}

PointMap::PointMap(const vector<unsigned int> &axisValues)
    : axisValues(axisValues)
{
    if(axisValues.size() != DIMENSIONS)
    {
        throwLineInfoException("Point " + to_string(DIMENSIONS) + "D requires exactly " + to_string(DIMENSIONS) + " values");
    }
}


PointMap::PointMap(const PointMap &other)
    : axisValues(other.getAxisValues())
{

}


PointMap::~PointMap()
{

}


const std::vector<unsigned int>& PointMap::getAxisValues() const
{
    return axisValues;
}


const unsigned int PointMap::get(const Axis &axis) const
{
    try
    {
        return axisValues[axis];
    }
    catch(const std::exception& e)
    {
        throwLineInfoException("Point " + to_string(DIMENSIONS) + "D has no " + string(magic_enum::enum_name(axis)) + "-axis");
    }
}


void PointMap::set(const Axis & axis, const unsigned int newValue)
{
    try
    {
        axisValues[axis] = newValue;
    }
    catch(const std::exception& e)
    {
        throwLineInfoException("Point " + to_string(DIMENSIONS) + "D has no " + string(magic_enum::enum_name(axis)) + "-axis");
    }
}

ostream& operator<<(ostream& os, const PointMap& point)
{
    os << point.get(magic_enum::enum_cast<Axis>(0).value());

    for(unsigned int i = 1; i < DIMENSIONS; i++)
    {
        os << "\t" << point.get(magic_enum::enum_cast<Axis>(i).value());
    }
    
    return os;
}

bool PointMap::operator==(const PointMap& other) const
{
    for(unsigned int i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();

        if(this->get(axis) != other.get(axis))
        {
            return false;
        }
    }

    return true;
}

bool PointMap::operator!=(const PointMap& other) const
{
    return !(*this==other);
}
