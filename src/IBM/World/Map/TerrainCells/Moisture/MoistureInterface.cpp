
#include "IBM/World/Map/TerrainCells/Moisture/MoistureInterface.h"

#include "IBM/World/Map/MapInterface.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCellInterface.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCellInterface.h"
#include "IBM/World/Map/TerrainCells/Moisture/ExtendedMoisture.h"
#include "IBM/World/Map/TerrainCells/Moisture/SummaryMoisture.h"

using namespace std;


unique_ptr<MoistureInterface> MoistureInterface::createInstance(
    const MapInterface& mapInterface, TerrainCellInterface* const terrainCellInterface,
    const bool inMoisturePatch)
{
    switch(mapInterface.getMapType()) {
		case MapInterface::Type::SpatialTree: {
            switch(static_cast<SpatialTreeTerrainCellInterface* const>(terrainCellInterface)->getSpatialTreeTerrainCellType()) {
                case SpatialTreeTerrainCellInterface::Type::Branch: {
                    if(inMoisturePatch)
                    {
                        BranchTerrainCellInterface* const parent = static_cast<SpatialTreeTerrainCellInterface* const>(terrainCellInterface)->getMutableParent();

                        ExtendedMoisture* moistureSource;

                        if(parent->isMoistureSource())
                        {
                            moistureSource = static_cast<ExtendedMoisture* const>(parent->getMutableMoistureInfo());
                        }
                        else
                        {
                            moistureSource = static_cast<SubdivisionMoisture* const>(parent->getMutableMoistureInfo())->getMutableMoistureSource();
                        }

                        return make_unique<SubdivisionMoisture>(moistureSource);
                    }
                    else
                    {
                        return make_unique<SummaryMoisture>(static_cast<BranchTerrainCellInterface* const>(terrainCellInterface));
                    }
                    break;
                }
                case SpatialTreeTerrainCellInterface::Type::Leaf:
                case SpatialTreeTerrainCellInterface::Type::TemporalLeaf: {
                    BranchTerrainCellInterface* const parent = static_cast<SpatialTreeTerrainCellInterface* const>(terrainCellInterface)->getMutableParent();

                    ExtendedMoisture* moistureSource;

                    if(parent->isMoistureSource())
                    {
                        moistureSource = static_cast<ExtendedMoisture* const>(parent->getMutableMoistureInfo());
                    }
                    else
                    {
                        moistureSource = static_cast<SubdivisionMoisture* const>(parent->getMutableMoistureInfo())->getMutableMoistureSource();
                    }

                    return make_unique<SubdivisionMoisture>(moistureSource);
                    break;
                }
                default: {
                    throwLineInfoException("Default case");
                    break;
                }
            }
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}


MoistureInterface::MoistureInterface()
{

}

MoistureInterface::~MoistureInterface()
{

}

void MoistureInterface::update()
{
    updateTemperature();
    updateRelativeHumidity();
}

template <class Archive>
void MoistureInterface::serialize(Archive &ar, const unsigned int version) 
{

}

// Specialisation
template void MoistureInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void MoistureInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void MoistureInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void MoistureInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);



namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, MoistureInterface* &moistureInterfacePtr, const unsigned int version, 
                const MapInterface& mapInterface, TerrainCellInterface* const terrainCellInterface,
                const bool moistureSource, const bool inMoisturePatch, vector<ExtendedMoisture*>& appliedMoisture) {
            
            if(moistureSource)
            {
                ExtendedMoisture* moistureCastPtr = static_cast<ExtendedMoisture*>(moistureInterfacePtr);
                serialize(ar, moistureCastPtr, version, appliedMoisture);
            }
            else
            {
                // For loading
                if(Archive::is_loading::value) 
                {
                    moistureInterfacePtr = MoistureInterface::createInstance(mapInterface, terrainCellInterface, inMoisturePatch).release();
                }

                switch(mapInterface.getMapType()) {
                    case MapInterface::Type::SpatialTree: {
                        switch(static_cast<SpatialTreeTerrainCellInterface* const>(terrainCellInterface)->getSpatialTreeTerrainCellType()) {
                            case SpatialTreeTerrainCellInterface::Type::Branch: {
                                if(inMoisturePatch)
                                {
                                    static_cast<SubdivisionMoisture*>(moistureInterfacePtr)->serialize(ar, version);
                                }
                                else
                                {
                                    static_cast<SummaryMoisture*>(moistureInterfacePtr)->serialize(ar, version);
                                }
                                break;
                            }
                            case SpatialTreeTerrainCellInterface::Type::Leaf:
                            case SpatialTreeTerrainCellInterface::Type::TemporalLeaf: {
                                static_cast<SubdivisionMoisture*>(moistureInterfacePtr)->serialize(ar, version);
                                break;
                            }
                            default: {
                                throwLineInfoException("Default case");
                                break;
                            }
                        }
                        break;
                    }
                    default: {
                        throwLineInfoException("Default case");
                        break;
                    }
                }
            }
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, MoistureInterface*&, const unsigned int, const MapInterface&, TerrainCellInterface* const, const bool, const bool, vector<ExtendedMoisture*>&);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, MoistureInterface*&, const unsigned int, const MapInterface&, TerrainCellInterface* const, const bool, const bool, vector<ExtendedMoisture*>&);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, MoistureInterface*&, const unsigned int, const MapInterface&, TerrainCellInterface* const, const bool, const bool, vector<ExtendedMoisture*>&);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, MoistureInterface*&, const unsigned int, const MapInterface&, TerrainCellInterface* const, const bool, const bool, vector<ExtendedMoisture*>&);
    }
}
