
#include "IBM/World/Map/Points/PointMap.h"

#include "IBM/World/Map/Points/PointSpatialTree.h"
#include "IBM/World/Map/MapInterface.h"

using namespace std;



unique_ptr<PointMap> PointMap::createInstance(const MapInterface& mapInterface)
{
	switch(mapInterface.getMapType()) {
		case MapInterface::Type::SpatialTree: {
			return make_unique<PointSpatialTree>();
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

template <class Archive>
void PointMap::serialize(Archive &ar, const unsigned int version) {
    ar & axisValues;
}

// Specialisation
template void PointMap::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void PointMap::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void PointMap::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void PointMap::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, PointMap* &pointMapPtr, const unsigned int version, const MapInterface& mapInterface) {
            // For loading
            if(Archive::is_loading::value) 
            {
                pointMapPtr = PointMap::createInstance(mapInterface).release();
            }

            switch(mapInterface.getMapType()) {
                case MapInterface::Type::SpatialTree: {
                    static_cast<PointSpatialTree*>(pointMapPtr)->serialize(ar, version);
                    break;
                }
                default: {
                    throwLineInfoException("Default case");
                    break;
                }
            }
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, PointMap*&, const unsigned int, const MapInterface&);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, PointMap*&, const unsigned int, const MapInterface&);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, PointMap*&, const unsigned int, const MapInterface&);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, PointMap*&, const unsigned int, const MapInterface&);
    }
}
