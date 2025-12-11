

#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/RootTerrainCell.h"

using namespace std;
using json = nlohmann::json;



RootTerrainCell::RootTerrainCell()
    : BranchTerrainCell()
{
    
}

RootTerrainCell::RootTerrainCell(const std::vector<PreciseDouble>& cellSizes, const unsigned int& mapDepth, PointSpatialTree *position, MoistureSource* const moistureBaseSource)
    : BranchTerrainCell(cellSizes, mapDepth, position, moistureBaseSource)
{
    
}


RootTerrainCell::~RootTerrainCell()
{

}


const BranchTerrainCell* RootTerrainCell::getParent() const
{
    return nullptr;
}

BranchTerrainCell* RootTerrainCell::getMutableParent() const
{
    return nullptr;
}

void RootTerrainCell::getNeighboursCellsOnRadius(
    vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const size_t searchDepth, const bool searchNeighborsWithFemales, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate)
{
    getRadiusTerrainCells(bestEvaluations, sourcePosition, radius, radiusArea, searchDepth, searchNeighborsWithFemales, false, animalWhoIsEvaluating, animalsHasTriedToPredate);
}

SpatialTreeTerrainCell* RootTerrainCell::getCell(const PointSpatialTree &cellPos)
{
    if(cellPos == getPosition())
    {
        return this;
    }
    else
    {
        return getMutableChildTerrainCell(cellPos)->getCell(cellPos);
    }
}



/****************/
/*    Animal    */
/****************/

    /************/
    /*    Up    */
    /************/

void RootTerrainCell::applyUpFunctionToAnimals(
        function<bool(Animal&)>,
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>&
    )
{
    
}

//////////////////

void RootTerrainCell::randomApplyUpFunctionToAnimals(
        function<bool(Animal&)>,
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>&
    )
{
    
}


/**************************/
/*    Edible in Radius    */
/**************************/

void RootTerrainCell::applyFunctionToEdiblesInRadius(
        const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    )
{   
    if(Geometry::fullCoveredBySphere(&getEffectiveArea(), sourcePosition, radius))
    {
        applyFunctionToEdiblesInCell(true, sourcePosition, radius, radiusArea, animalFunctions, resourceFunctions);
    }
    else
    {
        applyFunctionToEdiblesInCell(false, sourcePosition, radius, radiusArea, animalFunctions, resourceFunctions);
    }
}



BOOST_CLASS_EXPORT(RootTerrainCell)

template <class Archive>
void RootTerrainCell::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<BranchTerrainCell>(*this);
}

// Specialisation
template void RootTerrainCell::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void RootTerrainCell::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void RootTerrainCell::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void RootTerrainCell::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
