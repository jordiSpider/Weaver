
#include "App/Model/IBM/Landscape/Map/SpatialTree/Points/PointSpatialTree.h"

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

PointSpatialTree& PointSpatialTree::operator=(const PointSpatialTree& other)
{
    if (this != &other) {
        axisValues = other.axisValues;
        depth = other.depth;
    }
    return *this;
}


PointSpatialTree::~PointSpatialTree()
{

}


unsigned int PointSpatialTree::getDepth() const
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



BOOST_CLASS_EXPORT(PointSpatialTree)

template <class Archive>
void PointSpatialTree::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<PointMap>(*this);

    ar & depth;
}

// Specialisation
template void PointSpatialTree::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void PointSpatialTree::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void PointSpatialTree::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void PointSpatialTree::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
