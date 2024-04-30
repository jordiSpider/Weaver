
#include "IBM/World/Map/Map.h"

#include "IBM/World/World.h"
#include "IBM/World/Map/TerrainCells/AnimalSearchParams.h"
#include "IBM/World/Map/SpatialTree.h"

using namespace std;
using json = nlohmann::json;




unique_ptr<Map> Map::createInstance(const json &mapInfo, World* const world)
{
	switch(EnumClass<Type>::stringToEnumValue(mapInfo["mapType"])) {
		case Type::SpatialTree: {
			return make_unique<SpatialTree>(mapInfo["worldWideParams"], mapInfo["spatialTreeParams"], world);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}



Map::Map(const json &mapConfig, World* const world)
	: minCellSize(mapConfig["minCellSize"]), numberOfCellsPerAxis(mapConfig["numberOfCellsPerAxis"]),
      world(world), lifeStageSearchParams(LifeStage::size())
{
    
}


Map::~Map()
{

}

void Map::addInhabitableTerrainCell(TerrainCellInterface* newInhabitableTerrainCell)
{
    inhabitableTerrainCells.push_back(newInhabitableTerrainCell);
}


const double& Map::getMinCellSize() const
{
	return minCellSize;
}

const unsigned int& Map::getNumberOfCellsPerAxis() const
{
    return numberOfCellsPerAxis;
}

const World* const Map::getWorld() const
{
    return world;
}

World* const Map::getMutableWorld() const
{
    return world;
}

const vector<TerrainCellInterface*>& Map::getInhabitableTerrainCells() const
{
    return inhabitableTerrainCells;
}

vector<TerrainCellInterface*>& Map::getMutableInhabitableTerrainCells()
{
    return inhabitableTerrainCells;
}

TerrainCellInterface*& Map::getMutableInhabitableTerrainCell(const unsigned int index)
{
    return inhabitableTerrainCells[index];
}

void Map::obtainLifeStageSearchParams()
{
    for(const auto &lifeStage : LifeStage::getEnumValues())
    {
        lifeStageSearchParams[lifeStage].addSearchParams(getWorld(), {lifeStage});
    }
}

const AnimalSearchParams& Map::getLifeStageSearchParams(const LifeStage::LifeStageValue &lifeStage) const
{
    return lifeStageSearchParams[lifeStage];
}

void Map::printCellAlongCells(ofstream &file)
{
    vector<pair<vector<double>, vector<unsigned int>>> mapCellsInfo(
        pow(getNumberOfCellsPerAxis(), DIMENSIONS),
        make_pair(
            vector<double>(getWorld()->getExistingResourceSpecies().size(), 0.0),
            vector<unsigned int>(getWorld()->getExistingAnimalSpecies().size(), 0)
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
                containsResources = containsResources && potentialInhabitableTerrainCell->resourceIsPresent(involvedResourceSpecies->getResourceSpeciesId());
            }
            else
            {
                containsResources = containsResources || potentialInhabitableTerrainCell->resourceIsPresent(involvedResourceSpecies->getResourceSpeciesId());
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
    ar & minCellSize;
    ar & numberOfCellsPerAxis;
    ar & world;
    ar & inhabitableTerrainCells;
    ar &  lifeStageSearchParams;
}
