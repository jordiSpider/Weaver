
#include "IBM/World/Map/Map.h"
#include "IBM/World/WorldInterface.h"
#include "IBM/World/Map/TerrainCells/AnimalSearchParams.h"

using namespace std;
using json = nlohmann::json;



Map::Map(const json &mapConfig, WorldInterface* const worldInterface)
	: minCellSize(mapConfig["minCellSize"]), numberOfCellsPerAxis(mapConfig["numberOfCellsPerAxis"]),
      worldInterface(worldInterface), lifeStageSearchParams(EnumClass<LifeStage>::size())
{
    
}

Map::Map(WorldInterface* const worldInterface)
    : worldInterface(worldInterface)
{

}

Map::~Map()
{

}


const double& Map::getMinCellSize() const
{
	return minCellSize;
}

const unsigned int& Map::getNumberOfCellsPerAxis() const
{
    return numberOfCellsPerAxis;
}

const WorldInterface* const Map::getWorldInterface() const
{
    return worldInterface;
}

WorldInterface* const Map::getMutableWorldInterface() const
{
    return worldInterface;
}

void Map::obtainLifeStageSearchParams()
{
    for(const LifeStage &lifeStage : EnumClass<LifeStage>::getEnumValues())
    {
        lifeStageSearchParams[lifeStage].addSearchParams(getWorldInterface(), {lifeStage});
    }
}

const AnimalSearchParams& Map::getLifeStageSearchParams(const LifeStage &lifeStage) const
{
    return lifeStageSearchParams[lifeStage];
}

void Map::printCellAlongCells(ofstream &file)
{
    vector<pair<vector<double>, vector<unsigned int>>> mapCellsInfo(
        pow(getNumberOfCellsPerAxis(), DIMENSIONS),
        make_pair(
            vector<double>(getWorldInterface()->getExistingResourceSpecies().size(), 0.0),
            vector<unsigned int>(getWorldInterface()->getExistingAnimalSpecies().size(), 0)
        )
    );

    obtainMapCellsInfo(mapCellsInfo);

    for(unsigned int index = 0; index < mapCellsInfo.size(); index++)
    {
        for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
        {
            file << (index / static_cast<unsigned int>(pow(getNumberOfCellsPerAxis(), axis))) % getNumberOfCellsPerAxis() << "\t";
        }

        for(const double &resourceSpeciesValue : mapCellsInfo[index].first)
        {
            file << resourceSpeciesValue << "\t";
        }

        for(const unsigned int &animalSpeciesValue : mapCellsInfo[index].second)
        {
            file << animalSpeciesValue << "\t";
        }

        file << endl;
    }
}

bool Map::isSpeciesInhabitableTerrainCell(const AnimalSpecies &animalSpecies, const TerrainCellInterface* const potentialInhabitableTerrainCell, const Instar &instar)
{
    bool containsResources, meetsInEnemyFreeSpace, meetsInCompetitorFreeSpace;

    meetsInEnemyFreeSpace = true;
    meetsInCompetitorFreeSpace = true;

    if(potentialInhabitableTerrainCell->getMoistureInfo()->isInEnemyFreeSpace() && animalSpecies.getDefaultHuntingMode() != HuntingMode::does_not_hunt) 
    {
        meetsInEnemyFreeSpace = false;
    }

    if(potentialInhabitableTerrainCell->getMoistureInfo()->isInCompetitorFreeSpace() && animalSpecies.getDefaultHuntingMode() == HuntingMode::does_not_hunt) 
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

        for(const auto &involvedResourceSpecies : animalSpecies.getInstarInvolvedResourceSpecies(instar))
        {
            if(animalSpecies.getForcePresenceAllResourcesInvolved())
            {
                containsResources = containsResources && potentialInhabitableTerrainCell->resourceIsPresent(involvedResourceSpecies);
            }
            else
            {
                containsResources = containsResources || potentialInhabitableTerrainCell->resourceIsPresent(involvedResourceSpecies);
            }
        }

        if(containsResources) 
        {
            return true;
        }
    }
    
    return false;
}

template <class Archive>
void Map::serialize(Archive &ar, const unsigned int version) {
    ar & boost::serialization::base_object<MapInterface>(*this);

    ar & minCellSize;
    ar & numberOfCellsPerAxis;
    ar & lifeStageSearchParams;
}


// Specialisation
template void Map::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Map::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Map::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Map::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);