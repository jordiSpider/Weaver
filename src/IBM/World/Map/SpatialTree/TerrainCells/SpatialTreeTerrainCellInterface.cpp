

#include "IBM/World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCellInterface.h"
#include "IBM/World/Map/SpatialTreeInterface.h"

#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/LeafTerrainCell.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/TemporalLeafTerrainCell.h"

using namespace std;




unique_ptr<SpatialTreeTerrainCellInterface> SpatialTreeTerrainCellInterface::createInstance(const SpatialTreeTerrainCellInterface::Type &spatialTreeTerrainCellType, BranchTerrainCellInterface* const parentTerrainCell, SpatialTreeInterface* const mapInterface)
{
	switch(spatialTreeTerrainCellType) {
        case Type::Branch: {
			return make_unique<BranchTerrainCell>(parentTerrainCell, mapInterface);
			break;
		}
        case Type::Leaf: {
			return make_unique<LeafTerrainCell>(parentTerrainCell, mapInterface);
			break;
		}
        case Type::TemporalLeaf: {
			return make_unique<TemporalLeafTerrainCell>(parentTerrainCell, mapInterface);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}


SpatialTreeTerrainCellInterface::SpatialTreeTerrainCellInterface(SpatialTreeInterface* const mapInterface)
    : TerrainCell(mapInterface)
{

}

SpatialTreeTerrainCellInterface::SpatialTreeTerrainCellInterface(PointSpatialTree* const &position, const Ring *const effectiveArea, const double &size, SpatialTreeInterface* const &mapInterface,
        LifeStageVector* const animals, const bool obstacle, const bool fullObstacle, 
        const int obstaclePatchPriority, MoistureInterface* const moistureInfo, const bool moistureSource, 
        const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity)
    : TerrainCell(position, effectiveArea, size, mapInterface, animals,
        obstacle, fullObstacle, obstaclePatchPriority, moistureInfo, moistureSource, inMoisturePatch, moisturePatchPriority,
        totalMaximumResourceCapacity)
{
    
}


SpatialTreeTerrainCellInterface::~SpatialTreeTerrainCellInterface()
{
    
}

const PointSpatialTree& SpatialTreeTerrainCellInterface::getPosition() const
{
    return static_cast<const PointSpatialTree&>(TerrainCell::getPosition());
}

PointSpatialTree& SpatialTreeTerrainCellInterface::getMutablePosition() const
{
    return static_cast<PointSpatialTree&>(TerrainCell::getMutablePosition());
}

template <class Archive>
void SpatialTreeTerrainCellInterface::serialize(Archive &ar, const unsigned int version, vector<ExtendedMoisture*>& appliedMoisture) 
{
    boost::serialization::base_object<TerrainCell>(*this).serialize(ar, version, appliedMoisture);
}

// Specialisation
template void SpatialTreeTerrainCellInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int, vector<ExtendedMoisture*>&);
template void SpatialTreeTerrainCellInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int, vector<ExtendedMoisture*>&);

template void SpatialTreeTerrainCellInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int, vector<ExtendedMoisture*>&);
template void SpatialTreeTerrainCellInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int, vector<ExtendedMoisture*>&);


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, SpatialTreeTerrainCellInterface* &spatialTreeTerrainCellInterfacePtr, const unsigned int version, BranchTerrainCellInterface* const parentTerrainCell, SpatialTreeInterface* const mapInterface, vector<ExtendedMoisture*>& appliedMoisture) {
            SpatialTreeTerrainCellInterface::Type spatialTreeTerrainCellType;
            
            // For loading
            if(Archive::is_loading::value) 
            {
                ar & spatialTreeTerrainCellType;
                spatialTreeTerrainCellInterfacePtr = SpatialTreeTerrainCellInterface::createInstance(spatialTreeTerrainCellType, parentTerrainCell, mapInterface).release();
            }
            else
            {
                spatialTreeTerrainCellType = spatialTreeTerrainCellInterfacePtr->getSpatialTreeTerrainCellType();
                ar & spatialTreeTerrainCellType;
            }

            switch(spatialTreeTerrainCellType) {
                case SpatialTreeTerrainCellInterface::Type::Branch: {
                    static_cast<BranchTerrainCell*>(spatialTreeTerrainCellInterfacePtr)->serialize(ar, version, appliedMoisture);
                    break;
                }
                case SpatialTreeTerrainCellInterface::Type::Leaf: {
                    static_cast<LeafTerrainCell*>(spatialTreeTerrainCellInterfacePtr)->serialize(ar, version, appliedMoisture);
                    break;
                }
                case SpatialTreeTerrainCellInterface::Type::TemporalLeaf: {
                    static_cast<TemporalLeafTerrainCell*>(spatialTreeTerrainCellInterfacePtr)->serialize(ar, version, appliedMoisture);
                    break;
                }
                default: {
                    throwLineInfoException("Default case");
                    break;
                }
            }
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, SpatialTreeTerrainCellInterface*&, const unsigned int, BranchTerrainCellInterface* const, SpatialTreeInterface* const, vector<ExtendedMoisture*>&);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, SpatialTreeTerrainCellInterface*&, const unsigned int, BranchTerrainCellInterface* const, SpatialTreeInterface* const, vector<ExtendedMoisture*>&);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, SpatialTreeTerrainCellInterface*&, const unsigned int, BranchTerrainCellInterface* const, SpatialTreeInterface* const, vector<ExtendedMoisture*>&);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, SpatialTreeTerrainCellInterface*&, const unsigned int, BranchTerrainCellInterface* const, SpatialTreeInterface* const, vector<ExtendedMoisture*>&);
    }
}
