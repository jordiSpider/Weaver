
#include "IBM/World/Map/MapInterface.h"

#include "IBM/World/Map/SpatialTree.h"

using namespace std;


unique_ptr<MapInterface> MapInterface::createInstance(const json &mapInfo, WorldInterface* const worldInterface)
{
	switch(EnumClass<Type>::stringToEnumValue(mapInfo["mapType"])) {
		case Type::SpatialTree: {
			return make_unique<SpatialTree>(mapInfo["worldWideParams"], mapInfo["spatialTreeParams"], worldInterface);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}

unique_ptr<MapInterface> MapInterface::createInstance(const MapInterface::Type &mapType, WorldInterface* const worldInterface)
{
	switch(mapType) {
		case Type::SpatialTree: {
			return make_unique<SpatialTree>(worldInterface);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}



MapInterface::MapInterface()
{
    
}


MapInterface::~MapInterface()
{

}

template <class Archive>
void MapInterface::serialize(Archive &ar, const unsigned int version) {
    
}


// Specialisation
template void MapInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void MapInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void MapInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void MapInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, MapInterface* &mapInterfacePtr, const unsigned int version, WorldInterface* const newWorldInterface, vector<ExtendedMoisture*>& appliedMoisture) {
            MapInterface::Type mapType;
            
            // For loading
            if(Archive::is_loading::value) 
            {
                ar & mapType;
                mapInterfacePtr = MapInterface::createInstance(mapType, newWorldInterface).release();
            }
            else
            {
                mapType = mapInterfacePtr->getMapType();
                ar & mapType;
            }

            switch(mapInterfacePtr->getMapType()) {
                case MapInterface::Type::SpatialTree: {
                    static_cast<SpatialTree*>(mapInterfacePtr)->serialize(ar, version, appliedMoisture);
                    break;
                }
                default: {
                    throwLineInfoException("Default case");
                    break;
                }
            }
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, MapInterface*&, const unsigned int, WorldInterface* const, vector<ExtendedMoisture*>&);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, MapInterface*&, const unsigned int, WorldInterface* const, vector<ExtendedMoisture*>&);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, MapInterface*&, const unsigned int, WorldInterface* const, vector<ExtendedMoisture*>&);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, MapInterface*&, const unsigned int, WorldInterface* const, vector<ExtendedMoisture*>&);
    }
}
