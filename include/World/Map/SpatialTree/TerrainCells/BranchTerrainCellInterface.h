#ifndef BRANCH_TERRAINCELL_INTERFACE_H_
#define BRANCH_TERRAINCELL_INTERFACE_H_


#include "World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"
#include "World/Map/SpatialTree/Points/PointSpatialTree.h"




class BranchTerrainCellInterface : public SpatialTreeTerrainCell
{
public:
    BranchTerrainCellInterface(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, const Ring *const effectiveArea, const double &size, 
        SpatialTreeInterface* const mapInterface, LifeStageVector* const animals, 
        const bool obstacle, const bool fullObstacle, 
        const int obstaclePatchPriority, MoistureInterface* const moistureInfo, const bool moistureSource, 
        const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity);
    virtual ~BranchTerrainCellInterface();

    virtual TerrainCellInterface* randomInsertAnimalOnChild(AnimalInterface* const newAnimal, TerrainCellInterface* child)=0;

    virtual void substractBiomassUp(const unsigned int resourceSpeciesId, double dryMassToBeSubstracted)=0;

    virtual const std::vector<const SpatialTreeTerrainCellInterface*>& getChildrenTerrainCells() const=0;
    virtual std::vector<SpatialTreeTerrainCellInterface*>& getMutableChildrenTerrainCells()=0;
    virtual const SpatialTreeTerrainCellInterface* const getChildTerrainCell(const unsigned int childIndex) const=0;
    virtual SpatialTreeTerrainCellInterface* getMutableChildTerrainCell(const unsigned int childIndex)=0;
    virtual std::unique_ptr<std::vector<unsigned int>> calculateChildrenPositions(const PointMap &sourcePosition) const=0;
};

#endif /* BRANCH_TERRAINCELL_INTERFACE_H_ */
