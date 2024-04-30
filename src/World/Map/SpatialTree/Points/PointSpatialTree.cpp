
#include "World/Map/SpatialTree/Points/PointSpatialTree.h"

using namespace std;



PointSpatialTree::PointSpatialTree()
    : PointMap(), depth(0)
{
    
}

PointSpatialTree::PointSpatialTree(const vector<unsigned int> &axisValues, const unsigned int &depth)
    : PointMap(axisValues), depth(depth)
{
    
}


PointSpatialTree::PointSpatialTree(const PointSpatialTree &other)
    : PointMap(other), depth(other.getDepth())
{

}


PointSpatialTree::~PointSpatialTree()
{

}


const unsigned int PointSpatialTree::getDepth() const
{
    return depth;
}

ostream& operator<<(ostream& os, const PointSpatialTree& point)
{
    os << static_cast<const PointMap&>(point) << "\t" << point.getDepth();
    
    return os;
}

bool PointSpatialTree::operator==(const PointSpatialTree& other) const
{
    if(this->getDepth() != other.getDepth())
    {
        return false;
    }

    return PointMap::operator==(other);
}

bool PointSpatialTree::operator!=(const PointSpatialTree& other) const
{
    return !(*this==other);
}
