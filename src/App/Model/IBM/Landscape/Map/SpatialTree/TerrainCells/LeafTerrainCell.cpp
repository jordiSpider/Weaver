

#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/LeafTerrainCell.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/EdibleSearchParams.h"
#include "App/Model/IBM/Landscape/Landscape.h"


using namespace std;
using json = nlohmann::json;
namespace bg = boost::geometry;




LeafTerrainCell::LeafTerrainCell()
    : SpatialTreeTerrainCell()
{

}

LeafTerrainCell::LeafTerrainCell(const std::vector<PreciseDouble>& cellSizes, BranchTerrainCell* const parentTerrainCell, PointSpatialTree* const position, MoistureSource* const moistureBaseSource)
    : SpatialTreeTerrainCell(parentTerrainCell, position, cellSizes[position->getDepth()])
{
    setPatchApplicator(new LeafPatchApplicator(this, moistureBaseSource));
}


LeafTerrainCell::~LeafTerrainCell()
{
    
}


SpatialTreeTerrainCell* const& LeafTerrainCell::getChildTerrainCell(const PointSpatialTree &) const
{
    throwLineInfoException("Attempting to retrieve a nonexistent cell");
}

bool LeafTerrainCell::isLeaf() const
{
    return true;
}

void LeafTerrainCell::registerEdibles(vector<vector<vector<CellResource*>>>& landscapeResources, vector<AnimalNonStatistical*>& landscapeAnimals)
{
    getMutablePatchApplicator().registerResources(landscapeResources);
    registerAnimals(landscapeAnimals);
}

void LeafTerrainCell::getRadiusTerrainCells(
    vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const size_t, const bool searchNeighborsWithFemales, const bool parentFullCoverage, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate)
{
    if(!getPatchApplicator().getCellObstacle().isObstacle())
    {
        getCellEvaluation(bestEvaluations, animalWhoIsEvaluating, sourcePosition, radius, radiusArea, searchNeighborsWithFemales, parentFullCoverage, animalsHasTriedToPredate);
    }
}

SpatialTreeTerrainCell* LeafTerrainCell::getCell(const PointSpatialTree &cellPos)
{
    if(cellPos == getPosition())
    {
        return this;
    }
    else
    {
        return getMutableParent()->getCell(cellPos);
    }
}

bool LeafTerrainCell::isChild(const PointSpatialTree &) const
{
    return false;
}


LeafPatchApplicator& LeafTerrainCell::getMutablePatchApplicator()
{
    return static_cast<LeafPatchApplicator&>(SpatialTreeTerrainCell::getMutablePatchApplicator());
}

const LeafPatchApplicator& LeafTerrainCell::getPatchApplicator() const
{
    return static_cast<const LeafPatchApplicator&>(SpatialTreeTerrainCell::getPatchApplicator());
}



/****************/
/*    Animal    */
/****************/

    /**************/
    /*    Down    */
    /**************/

void LeafTerrainCell::applyDownFunctionToAnimals(const vector<pair<const AnimalSearchParams&, AnimalFunctions>>&)
{
    
}

void LeafTerrainCell::applyDownFunctionToAnimals(
        function<bool(Animal&)>, const PointContinuous &, const PreciseDouble &,
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>&
    )
{
    
}

////////////////

void LeafTerrainCell::randomApplyDownFunctionToAnimals(const vector<pair<const AnimalSearchParams&, AnimalFunctions>>&)
{
    
}







BOOST_CLASS_EXPORT(LeafTerrainCell)

template <class Archive>
void LeafTerrainCell::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<SpatialTreeTerrainCell>(*this);
}

// Specialisation
template void LeafTerrainCell::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void LeafTerrainCell::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void LeafTerrainCell::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void LeafTerrainCell::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
