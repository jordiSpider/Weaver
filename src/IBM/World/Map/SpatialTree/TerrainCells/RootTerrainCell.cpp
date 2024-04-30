

#include "IBM/World/Map/SpatialTree/TerrainCells/RootTerrainCell.h"

using namespace std;
using json = nlohmann::json;



RootTerrainCell::RootTerrainCell(SpatialTreeInterface* const mapInterface)
    : BranchTerrainCell(nullptr, mapInterface)
{
    
}

RootTerrainCell::RootTerrainCell(PointSpatialTree* const position, SpatialTreeInterface* const mapInterface)
    : BranchTerrainCell(nullptr, position, mapInterface)
{
    
}


RootTerrainCell::~RootTerrainCell()
{

}


const BranchTerrainCell* const RootTerrainCell::getParent() const
{
    return nullptr;
}

BranchTerrainCell* const RootTerrainCell::getMutableParent() const
{
    return nullptr;
}

unique_ptr<FullCoverageAnimals> RootTerrainCell::getMutableAnimalsUp(const AnimalSearchParams &animalSearchParams)
{
    return make_unique<FullCoverageAnimals>();
}

unique_ptr<PartialCoverageAnimals> RootTerrainCell::getMutableAnimalsUp(
        function<bool(AnimalInterface&)> upChecker, const AnimalSearchParams &animalSearchParams)
{
    return make_unique<PartialCoverageAnimals>();
}

SearchableEdibles RootTerrainCell::getEdiblesOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const Ring &radiusArea, const EdibleSearchParams &edibleSearchParams)
{   
    auto intersection = Geometry::calculateIntersection(radiusArea, getEffectiveArea());

    if(Geometry::checkCoverage(intersection.get(), getEffectiveArea()) == Coverage::Full)
    {
        return getMutableEdiblesOnAllCell(TerrainCellInterface::Full, sourcePosition, radius, radiusArea, edibleSearchParams);
    }
    else
    {
        return getMutableEdiblesOnAllCell(TerrainCellInterface::Partial, sourcePosition, radius, radiusArea, edibleSearchParams);
    }
}

unique_ptr<vector<pair<TerrainCellInterface*, pair<TerrainCellInterface::TerrainCellCoverage, unique_ptr<Ring>>>>> RootTerrainCell::getNeighboursCellsOnRadius(
        const Ring &radiusArea, const unsigned int searchDepth)
{
    return getRadiusTerrainCells(radiusArea, searchDepth);
}

void RootTerrainCell::obtainWorldResourceBiomassAndAnimalsPopulation(vector<double> &worldResourceBiomass, CustomIndexedVector<AnimalSpecies::AnimalID, CustomIndexedVector<LifeStage, unsigned int>> &worldAnimalsPopulation)
{
    for(const auto &resource : getResources())
    {
        if(resource != nullptr)
        {
            worldResourceBiomass[resource->getSpecies()->getResourceSpeciesId()] += resource->calculateWetMass();
        }
    }

    obtainWorldAnimalsPopulation(worldAnimalsPopulation);
}

void RootTerrainCell::substractBiomassUp(const unsigned int resourceSpeciesId, double dryMassToBeSubstracted)
{
    // Not to implement
}

template <class Archive>
void RootTerrainCell::serialize(Archive &ar, const unsigned int version, vector<ExtendedMoisture*>& appliedMoisture) 
{
    boost::serialization::base_object<BranchTerrainCell>(*this).serialize(ar, version, appliedMoisture);
}


// Specialisation
template void RootTerrainCell::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int, vector<ExtendedMoisture*>&);
template void RootTerrainCell::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int, vector<ExtendedMoisture*>&);

template void RootTerrainCell::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int, vector<ExtendedMoisture*>&);
template void RootTerrainCell::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int, vector<ExtendedMoisture*>&);


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, RootTerrainCell* &rootTerrainCellPtr, const unsigned int version, SpatialTreeInterface* const newMapInterface, vector<ExtendedMoisture*>& appliedMoisture) {
            // For loading
            if(Archive::is_loading::value) 
            {
                rootTerrainCellPtr = new RootTerrainCell(newMapInterface);
            }

            rootTerrainCellPtr->serialize(ar, version, appliedMoisture);
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, RootTerrainCell*&, const unsigned int, SpatialTreeInterface* const, vector<ExtendedMoisture*>&);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, RootTerrainCell*&, const unsigned int, SpatialTreeInterface* const, vector<ExtendedMoisture*>&);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, RootTerrainCell*&, const unsigned int, SpatialTreeInterface* const, vector<ExtendedMoisture*>&);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, RootTerrainCell*&, const unsigned int, SpatialTreeInterface* const, vector<ExtendedMoisture*>&);
    }
}
