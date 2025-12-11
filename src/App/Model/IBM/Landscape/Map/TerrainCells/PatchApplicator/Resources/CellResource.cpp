#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResource.h"

#include "App/Model/IBM/Landscape/Landscape.h"

using namespace std;



CellResource::CellResource()
	: CellResourceInterface()
{
	
}

CellResource::CellResource(Landscape* const landscape, TerrainCell* terrainCell, const size_t newResourcePatchPriority, ResourceSpecies& resourceSpecies, ResourceSource* const newResourceSource)
	: CellResourceInterface(landscape->generateEdibleId(), terrainCell, newResourcePatchPriority, resourceSpecies),
	  growthBuildingBlock(this, &resourceSpecies.getMutableGrowthBuildingBlock(), newResourceSource)
{
	landscape->generateResourceId();
}

CellResource::~CellResource()
{

}

const CellResourceGrowth& CellResource::getGrowthBuildingBlock() const
{
	return growthBuildingBlock;
}

CellResourceGrowth& CellResource::getMutableGrowthBuildingBlock()
{
	return growthBuildingBlock;
}

void CellResource::update()
{
	
}

void CellResource::applySource(Landscape* const landscape, const ResourceSource* const source, const size_t resourcePatchPriority)
{
	setPatchPriority(resourcePatchPriority);


	getMutableGrowthBuildingBlock().applySource(landscape->getCompetitionAmongResourceSpecies(), source);
}

void CellResource::growth(Landscape* const landscape)
{
	const WetMass excess = getMutableGrowthBuildingBlock().growth(
		landscape, getTerrainCell()->getPatchApplicator().getCellMoisture().getMoisture(),
		getTerrainCell()->getPatchApplicator().getCellMoisture().getTemperature()
	);


	if(getGrowthBuildingBlock().getSource().getPatchSpread() && excess > 0.0)
	{
		spread(landscape, excess);
	}
}

void CellResource::spread(Landscape* const landscape, const WetMass& massToSpread)
{
	#ifdef DEBUG
	if constexpr(DIMENSIONS == 3u)
	{
		throwLineInfoException("No 3D implementation");
	}
    #endif


	vector<TerrainCell*> neighbours;

	for(unsigned char i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();

		PointSpatialTree point = static_cast<const SpatialTreeTerrainCell*>(getTerrainCell())->getPosition();

        if(getTerrainCell()->getPosition().get(axis) > 0u)
		{
			point.set(axis, getTerrainCell()->getPosition().get(axis) - 1u);

			neighbours.push_back(getMutableTerrainCell()->getCell(point));
		}

		if(getTerrainCell()->getPosition().get(axis) < static_cast<const SpatialTree*>(landscape->getMap())->getAxisSize(static_cast<const SpatialTreeTerrainCell*>(getTerrainCell())->getPosition().getDepth())-1)
		{
			point.set(axis, getTerrainCell()->getPosition().get(axis) + 1u);

			neighbours.push_back(getMutableTerrainCell()->getCell(point));
		}
    }


	const WetMass massPerNeighbour(massToSpread.getValue() / static_cast<double>(DIMENSIONS * 2));


	for(TerrainCell* neighbour : neighbours)
    {
		CellResource* resource = &static_cast<CellResource&>(neighbour->getMutablePatchApplicator().getMutableCellResource(getSpecies()->getResourceSpeciesId()));

		if(resource->getGrowthBuildingBlock().getSource().getPatchSpread())
		{
			resource->addBiomass(landscape, massPerNeighbour, landscape->getCompetitionAmongResourceSpecies());
		}
	}
}

void CellResource::addBiomass(Landscape* const landscape, const WetMass& addedMass, const bool competitionAmongResourceSpecies) 
{ 
	const WetMass excess = getMutableGrowthBuildingBlock().addBiomass(addedMass, competitionAmongResourceSpecies);

	if(excess > 0.0)
	{
		spread(landscape, excess);
	}
};

void CellResource::deserializeCellResource(std::vector<std::pair<size_t, ResourceSource*>>& appliedResource)
{
	bool found = false;

    for(size_t i = 0; i < appliedResource.size() && !found; i++)
    {
        if(getPatchPriority() == appliedResource[i].first)
        {
			getMutableGrowthBuildingBlock().setResourceSource(appliedResource[i].second);
            found = true;
        }
    }
}



DryMass CellResource::calculateDryMassAvailable(const bool fullCoverage, const PointContinuous* const, const PreciseDouble &, const RingModel* const radiusArea) const
{
	if(getGrowthBuildingBlock().getCurrentTotalWetMass() > 0.0)
	{
		PreciseDouble coveragePercent;
				
		if(fullCoverage) {
			coveragePercent = 1.0;
		}
		else {
			coveragePercent = Geometry::calculateFirstCoveragePercentBySecond(&getTerrainCell()->getEffectiveArea(), radiusArea, true);
		}

		DryMass dryMassAvailable(WetMass((getGrowthBuildingBlock().getCurrentTotalWetMass() - getGrowthBuildingBlock().getMinimumEdibleBiomass()).getValue() * coveragePercent), getSpecies()->getGrowthBuildingBlock().getConversionToWetMass(getGrowthBuildingBlock().getInstar()));
		dryMassAvailable = DryMass(fmax(0.0, dryMassAvailable.getValue()));

		return dryMassAvailable;
	}
	else
	{
		return DryMass(0.0);
	}
}


void CellResource::setSpeciesGrowth(SpeciesGrowth* newSpeciesGrowth)
{
	growthBuildingBlock.setSpeciesGrowth(newSpeciesGrowth);
}


void CellResource::substractBiomass(const DryMass& dryMassToBeSubstracted, const PointContinuous &, const PreciseDouble &, const bool competitionAmongResourceSpecies)
{
	getMutableGrowthBuildingBlock().substractBiomass(dryMassToBeSubstracted, competitionAmongResourceSpecies);

	if(static_cast<const SpatialTreeTerrainCell*>(getTerrainCell())->getParent() != nullptr)
    {
		static_cast<BranchCellResource&>(static_cast<SpatialTreeTerrainCell*>(getMutableTerrainCell())->getMutableParent()->getMutablePatchApplicator().getMutableCellResource(getSpecies()->getResourceSpeciesId())).substractBiomassUp(dryMassToBeSubstracted);
	}
}

void CellResource::substractBiomass(const DryMass& dryMassToBeSubstracted, const bool, const PointContinuous &, const PreciseDouble &, const RingModel* const, const bool competitionAmongResourceSpecies)
{
	getMutableGrowthBuildingBlock().substractBiomass(dryMassToBeSubstracted, competitionAmongResourceSpecies);
}




BOOST_CLASS_EXPORT(CellResource)

template <class Archive>
void CellResource::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<CellResourceInterface>(*this);

	ar & growthBuildingBlock;

	if (Archive::is_loading::value)
	{
        growthBuildingBlock.setOwner(this);
    }
}

// Specialisation
template void CellResource::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellResource::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellResource::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellResource::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
