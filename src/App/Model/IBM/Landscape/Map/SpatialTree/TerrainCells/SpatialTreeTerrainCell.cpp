

#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"

#include "App/Model/IBM/Landscape/Map/SpatialTree.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/BranchTerrainCell.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/EdibleSearchParams.h"
#include "App/Model/IBM/Landscape/Landscape.h"


using namespace std;



SpatialTreeTerrainCell::SpatialTreeTerrainCell()
    : TerrainCell()
{

}

SpatialTreeTerrainCell::SpatialTreeTerrainCell(BranchTerrainCell* const parentTerrainCell, PointSpatialTree* const position, const PreciseDouble &size)
    : TerrainCell(position, size),
      parentTerrainCell(parentTerrainCell)
{

}

SpatialTreeTerrainCell::~SpatialTreeTerrainCell()
{
    
}


void SpatialTreeTerrainCell::setParent(BranchTerrainCell* newParentTerrainCell)
{
    parentTerrainCell = newParentTerrainCell;
}

const BranchTerrainCell* SpatialTreeTerrainCell::getParent() const
{
    return parentTerrainCell;
}

BranchTerrainCell* SpatialTreeTerrainCell::getMutableParent() const
{
    return parentTerrainCell;
}




TerrainCell* SpatialTreeTerrainCell::getCell(const PointMap &dstPos)
{
    return getCell(static_cast<const PointSpatialTree&>(dstPos));
}




AnimalNonStatistical* SpatialTreeTerrainCell::createAnimal(Landscape* const landscape, const Instar &instar, AnimalSpecies* animalSpecies, const Genome* const genome, const bool saveGenetics, std::vector<fmt::memory_buffer>& geneticsText, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
    AnimalNonStatistical* newAnimal = new SpatialTreeAnimal(instar, animalSpecies, this, genome, actualTimeStep, timeStepsPerDay);

    // Indicate that the animal created is the final animal, and therefore assign a final ID to it.
    newAnimal->doDefinitive(landscape, saveGenetics, geneticsText);

    return newAnimal;
}

void SpatialTreeTerrainCell::migrateAnimalTo(Landscape* const landscape, Animal* animalToMigrate, TerrainCell* newTerrainCell, const PointContinuous &newPosition)
{
    AnimalNonStatistical* animalToMigrateCast = static_cast<AnimalNonStatistical*>(animalToMigrate);

    eraseAnimal(animalToMigrate);
    animalToMigrateCast->setPosition(newPosition);

    newTerrainCell->insertAnimal(landscape, animalToMigrate);

    IndividualGrowthModel::Type previousGrowthModel = animalToMigrateCast->getMutableGrowthBuildingBlock().getIndividualGrowthModel()->getType();

    animalToMigrateCast->calculateGrowthModel(landscape->getTimeStepsPerDay());

    if(previousGrowthModel != animalToMigrateCast->getMutableGrowthBuildingBlock().getIndividualGrowthModel()->getType())
    {
        animalToMigrateCast->calculateGrowthCurves(landscape->getTimeStepsPerDay());
    }
}

/**************************/
/*    Edible in Radius    */
/**************************/

void SpatialTreeTerrainCell::applyFunctionToEdiblesInRadius(
        const PointContinuous &sourcePosition, const PreciseDouble &radius, 
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions,
        const PreciseDouble& animalBodySection
    )
{   
    if(Geometry::fullCoveredBySphere(getEffectiveArea(), sourcePosition, radius))
    {
        getMutableParent()->applyFunctionToEdiblesInRadius(sourcePosition, radius, animalFunctions, resourceFunctions, animalBodySection);
    }
    else
    {
        applyFunctionToEdiblesInCell(false, sourcePosition, radius, animalFunctions, resourceFunctions, animalBodySection);
    }
}






void SpatialTreeTerrainCell::getNeighboursCellsOnRadius(
    vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const size_t searchDepth, bool searchNeighborsWithFemales, bool searchNeighborsWithMales, AnimalNonStatistical* animalWhoIsEvaluating, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchEdibilityValueGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchPredationRiskGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchConspecificBiomassGlobal)
{
    if (Geometry::fullCoveredBySphere(getEffectiveArea(), sourcePosition, radius))
    {
        getMutableParent()->getNeighboursCellsOnRadius(bestEvaluations, sourcePosition, radius, searchDepth, searchNeighborsWithFemales, searchNeighborsWithMales, animalWhoIsEvaluating, maximumPatchEdibilityValueGlobal, maximumPatchPredationRiskGlobal, maximumPatchConspecificBiomassGlobal);
    }
    else
    {
        getRadiusTerrainCells(bestEvaluations, sourcePosition, radius, searchDepth, searchNeighborsWithFemales, searchNeighborsWithMales, false, animalWhoIsEvaluating, maximumPatchEdibilityValueGlobal, maximumPatchPredationRiskGlobal, maximumPatchConspecificBiomassGlobal);
    }
}




const PointSpatialTree& SpatialTreeTerrainCell::getPosition() const
{
    return static_cast<const PointSpatialTree&>(TerrainCell::getPosition());
}

PointSpatialTree& SpatialTreeTerrainCell::getMutablePosition() const
{
    return static_cast<PointSpatialTree&>(TerrainCell::getMutablePosition());
}


SpatialTreePatchApplicator& SpatialTreeTerrainCell::getMutablePatchApplicator()
{
    return static_cast<SpatialTreePatchApplicator&>(TerrainCell::getMutablePatchApplicator());
}

const SpatialTreePatchApplicator& SpatialTreeTerrainCell::getPatchApplicator() const
{
    return static_cast<const SpatialTreePatchApplicator&>(TerrainCell::getPatchApplicator());
}





BOOST_SERIALIZATION_ASSUME_ABSTRACT(SpatialTreeTerrainCell)

template <class Archive>
void SpatialTreeTerrainCell::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<TerrainCell>(*this);
}

// // Specialisation
template void SpatialTreeTerrainCell::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SpatialTreeTerrainCell::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SpatialTreeTerrainCell::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SpatialTreeTerrainCell::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
