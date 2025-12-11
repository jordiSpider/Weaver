
#include "App/Model/IBM/Landscape/Map/Points/PointMap.h"

#include "App/Model/IBM/Landscape/Map/Map.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/Points/PointSpatialTree.h"

using namespace std;



PointMap* PointMap::createInstance(const Map& map)
{
	switch(map.getType()) {
		case Map::Type::SpatialTree: {
			return new PointSpatialTree();
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}


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


unsigned int PointMap::get(const Axis &axis) const
{
    try
    {
        return axisValues[axis];
    }
    catch(const std::exception&)
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
    catch(const std::exception&)
    {
        throwLineInfoException("Point " + to_string(DIMENSIONS) + "D has no " + string(magic_enum::enum_name(axis)) + "-axis");
    }
}

ostream& operator<<(ostream& os, const PointMap& point)
{
    os << point.get(magic_enum::enum_cast<Axis>(0).value());

    for(unsigned char i = 1; i < DIMENSIONS; i++)
    {
        os << "\t" << point.get(magic_enum::enum_cast<Axis>(i).value());
    }
    
    return os;
}

bool PointMap::operator==(const PointMap& other) const
{
    for(unsigned char i = 0; i < DIMENSIONS; i++)
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



BOOST_SERIALIZATION_ASSUME_ABSTRACT(PointMap)

template <class Archive>
void PointMap::serialize(Archive &ar, const unsigned int) {
    ar & axisValues;
}

// // Specialisation
template void PointMap::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void PointMap::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void PointMap::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void PointMap::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
