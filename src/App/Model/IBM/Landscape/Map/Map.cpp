
#include "App/Model/IBM/Landscape/Map/Map.h"

#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/AnimalSearchParams.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree.h"

using namespace std;
using json = nlohmann::json;





Map* Map::createInstance(const json &mapInfo, MoistureSource* const moistureBaseSource)
{
	switch(EnumClass<Type>::stringToEnumValue(mapInfo["mapType"].get<string>())) {
		case Type::SpatialTree: {
			return new SpatialTree(mapInfo["landscapeWideParams"], mapInfo["spatialTreeParams"], moistureBaseSource);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}



Map::Map()
{

}

bool isPowerOf2(const unsigned int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

unsigned int nextPowerOf2(const unsigned int n) {
    if (n == 0) return 1u;
    return 1u << (size_t)ceil(log2(n));
}

Map::Map(const json &mapConfig)
	: minCellSize(mapConfig["minCellSize"].get<double>()), minHyperVolume(pow(minCellSize, DIMENSIONS)),
      lifeStageSearchParams(EnumClass<LifeStage>::size())
{
    unsigned int numberOfCellsAxisX = mapConfig["numberOfCellsAxisX"].get<unsigned int>();
    unsigned int numberOfCellsAxisY = mapConfig["numberOfCellsAxisY"].get<unsigned int>();

    unsigned int maxNumberOfCellsAxis = (numberOfCellsAxisX > numberOfCellsAxisY) ? numberOfCellsAxisX : numberOfCellsAxisY;

    if(!isPowerOf2(maxNumberOfCellsAxis))
    {
        numberOfCellsPerAxis = nextPowerOf2(maxNumberOfCellsAxis);
    }
    else
    {
        numberOfCellsPerAxis = maxNumberOfCellsAxis;
    }
}


Map::~Map()
{

}

void Map::addInhabitableTerrainCell(TerrainCell* newInhabitableTerrainCell)
{
    inhabitableTerrainCells.push_back(newInhabitableTerrainCell);
}


const PreciseDouble& Map::getMinCellSize() const
{
	return minCellSize;
}

const PreciseDouble& Map::getMinHyperVolume() const
{
    return minHyperVolume;
}

unsigned int Map::getNumberOfCellsPerAxis() const
{
    return numberOfCellsPerAxis;
}

const vector<TerrainCell*>& Map::getInhabitableTerrainCells() const
{
    return inhabitableTerrainCells;
}

vector<TerrainCell*>& Map::getMutableInhabitableTerrainCells()
{
    return inhabitableTerrainCells;
}

TerrainCell*& Map::getMutableInhabitableTerrainCell(const unsigned int index)
{
    return inhabitableTerrainCells[index];
}

const AnimalSearchParams& Map::getLifeStageSearchParams(const LifeStage &lifeStage) const
{
    return lifeStageSearchParams[lifeStage];
}

const AnimalSearchParams& Map::getAllAnimalsSearchParams() const
{
    return allAnimalsSearchParams;
}

bool Map::isSpeciesInhabitableTerrainCell(const AnimalSpecies &animalSpecies, const TerrainCell* const potentialInhabitableTerrainCell, const std::vector<ResourceSpecies::ResourceID>& involvedResourceSpecies)
{
    bool containsResources, meetsInEnemyFreeSpace, meetsInCompetitorFreeSpace;

    meetsInEnemyFreeSpace = true;
    meetsInCompetitorFreeSpace = true;

    if(potentialInhabitableTerrainCell->getPatchApplicator().getCellMoisture().isInEnemyFreeSpace() && animalSpecies.getDefaultHuntingMode() != HuntingMode::does_not_hunt) 
    {
        meetsInEnemyFreeSpace = false;
    }

    if(potentialInhabitableTerrainCell->getPatchApplicator().getCellMoisture().isInCompetitorFreeSpace() && animalSpecies.getDefaultHuntingMode() == HuntingMode::does_not_hunt) 
    {
        meetsInCompetitorFreeSpace = false;
    }

    if(meetsInEnemyFreeSpace && meetsInCompetitorFreeSpace) {
        if(animalSpecies.getForcePresenceAllResourcesInvolved())
        {
            containsResources = true;
        }
        else
        {
            containsResources = false;
        }

        for(const ResourceSpecies::ResourceID &resourceSpecies : involvedResourceSpecies)
        {
            const bool resourceIsPresent = potentialInhabitableTerrainCell->getPatchApplicator().getCellResource(resourceSpecies).getGrowthBuildingBlock().getCurrentTotalWetMass() > 0.0;

            if(animalSpecies.getForcePresenceAllResourcesInvolved())
            {
                containsResources = containsResources && resourceIsPresent;
            }
            else
            {
                containsResources = containsResources || resourceIsPresent;
            }
        }

        if(containsResources) 
        {
            return true;
        }
    }
    
    return false;
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(Map)

template <class Archive>
void Map::serialize(Archive &ar, const unsigned int) {
    ar & minCellSize;
    ar & minHyperVolume;
    ar & numberOfCellsPerAxis;
    ar & lifeStageSearchParams;
    ar & allAnimalsSearchParams;
}

// // Specialisation
template void Map::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Map::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Map::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Map::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


