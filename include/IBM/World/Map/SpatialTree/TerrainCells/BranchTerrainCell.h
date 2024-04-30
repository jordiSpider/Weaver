#ifndef BRANCH_TERRAINCELL_H_
#define BRANCH_TERRAINCELL_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <cmath>

#include "IBM/World/Map/SpatialTree/TerrainCells/BranchTerrainCellInterface.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/TemporalLeafTerrainCell.h"
#include "IBM/World/Map/SpatialTree/Points/PointSpatialTree.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/Moisture/SummaryMoisture.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/Resource/SummaryResource.h"
#include "IBM/World/Map/TerrainCells/Moisture/ExtendedMoisture.h"
#include "IBM/World/Map/TerrainCells/Moisture/UpdateMethodType/CycleMoisture.h"
#include "IBM/World/Map/Points/Axis.h"
#include "IBM/Maths/OptimisedOperators.h"
#include "IBM/World/Map/Geometry/Coverage.h"
#include "Exceptions/LineInfoException.h"



class BranchTerrainCell : public BranchTerrainCellInterface
{
private:
    /**
     * @name Setters
     * @{
     */
    void setChildTerrainCell(const unsigned int childIndex, std::unique_ptr<BranchTerrainCell> &newChild);
    /** @} */

    /**
     * @name Generate leaf
     * @{
     */
    std::unique_ptr<BranchTerrainCell> convertTemporalLeaf2Branch(const unsigned int childIndex);
    /** @} */

    /**
     * @name Obstacle
     * @{
     */
    void updateObstacleValue(); //?
    /** @} */

    /**
     * @name Resource
     * @{
     */
    
    /** @} */

    /**
     * @name Animal
     * @{
     */
    void refreshAnimalPosition();
    /** @} */

protected:
    std::vector<std::unique_ptr<SpatialTreeTerrainCellInterface>> childrenTerrainCells;
    std::vector<SpatialTreeTerrainCellInterface*> childrenTerrainCellPointers;
    std::vector<const SpatialTreeTerrainCellInterface*> childrenTerrainCellConstPointers;

    void generateChildren(const std::vector<const ResourceInterface*>* const resources = nullptr, const std::vector<int>* const resourcePatchPriority = nullptr);

    const unsigned int calculateChildPositionOnVector(const PointContinuous &childPos) const;
    const unsigned int calculateChildPositionOnVector(const PointSpatialTree &childPos) const;

    /**
     * @name Patches
     * @{
     */

    /**
     * @name Obstacle patches
     * @{
     */
    bool applyObstaclePatch(const ObstaclePatch &obstaclePatch); //?
    bool applyFullCoverageObstaclePatch(const ObstaclePatch &obstaclePatch); //?
    bool applyPartialCoverageObstaclePatch(const ObstaclePatch &obstaclePatch); //?
    /** @} */

    /**
     * @name Moisture patches
     * @{
     */
    int maximumMoisturePatchPriority;

    std::pair<bool, bool> applyMoisturePatch(const MoisturePatch &moisturePatch); //?
    std::pair<bool, bool> applyPartialCoverageMoisturePatch(const MoisturePatch &moisturePatch); //?
    const int getMaximumMoisturePatchPriority() const; //?
    void setSummaryMoisturePatch(); //?
    void setMaximumMoisturePatchPriority(const int newMaximumMoisturePatchPriority); //?
    void setMoistureSourcePatch(const MoisturePatch &moisturePatch) override;
    void setMoistureSourcePatch(MoistureInterface* const &newMoistureInfo, const int newMoisturePatchPriority);
    void setMoisturePatch(MoistureInterface* const &newMoistureInfo, const int newMoisturePatchPriority, const bool newMoistureSourceValue, const bool newInMoisturePatchValue) override; //?
    void setMoisturePatch(MoistureInterface* const &newMoistureInfo, const int newMoisturePatchPriority, const int newMaximumMoisturePatchPriority, const bool newMoistureSourceValue, const bool newInMoisturePatchValue); //?
    /** @} */

    /**
     * @name Resource patches
     * @{
     */
    std::vector<int> maximumResourcePatchPriority;

    bool applyResourcePatch(const ResourcePatch &resourcePatch); //?
    bool applyFullCoverageResourcePatch(const ResourcePatch &resourcePatch); //?
    bool applyPartialCoverageResourcePatch(const ResourcePatch &resourcePatch); //?
    void setSummaryResourcePatch(const unsigned int resourceSpeciesId); //?
    const int getMaximumResourcePatchPriority(const unsigned int resourceSpeciesId) const; //?
    void setMaximumResourcePatchPriority(const unsigned int resourceSpeciesId, const int newMaximumResourcePatchPriority); //?
    void setResourceSourcePatch(std::unique_ptr<ResourceInterface> &newResource, const int newResourcePatchPriority);
    void setResourcePatch(const unsigned int resourceSpeciesId, std::unique_ptr<ResourceInterface> &newResource, const int newResourcePatchPriority) override; //?
    void setResourcePatch(const unsigned int resourceSpeciesId, std::unique_ptr<ResourceInterface> &newResource, const int newResourcePatchPriority, const int newMaximumResourcePatchPriority); //?
    /** @} */

    /** @} */

    void updateTotalMaximumResourceCapacity() override;

public:
    // Constructor with default moisture info
    BranchTerrainCell(BranchTerrainCellInterface* const parentTerrainCell, PointSpatialTree* const position, SpatialTreeInterface* const mapInterface);
    // Constructor from a temporal leaf
    BranchTerrainCell(const TemporalLeafTerrainCell &leaf);
    virtual ~BranchTerrainCell();

    // Getter
    const std::vector<const SpatialTreeTerrainCellInterface*>& getChildrenTerrainCells() const;
    std::vector<SpatialTreeTerrainCellInterface*>& getMutableChildrenTerrainCells();
    const SpatialTreeTerrainCellInterface* const getChildTerrainCell(const unsigned int childIndex) const;
    SpatialTreeTerrainCellInterface* getMutableChildTerrainCell(const unsigned int childIndex);
    const SpatialTreeTerrainCellInterface* const getChildTerrainCell(const PointSpatialTree &cellPos) const;
    SpatialTreeTerrainCellInterface* getMutableChildTerrainCell(const PointSpatialTree &cellPos);

    void insertAnimal(AnimalInterface* const newAnimal) override;
    TerrainCellInterface* randomInsertAnimalOnChild(AnimalInterface* const newAnimal, TerrainCellInterface* child);
    std::tuple<bool, TerrainCellInterface*, TerrainCellInterface*> randomInsertAnimal(AnimalInterface* const newAnimal) override;

    void eraseAllAnimals();
    void purgeDeadAnimals();

    bool isChild(const PointContinuous &childPos) const;
    bool isChild(const PointSpatialTree &childPos) const;

    std::unique_ptr<std::vector<std::pair<TerrainCellInterface*, std::pair<TerrainCellInterface::TerrainCellCoverage, std::unique_ptr<Ring>>>>> getRadiusTerrainCells(
        const Ring &radiusArea, const unsigned int searchDepth
    );

    virtual void substractBiomassUp(const unsigned int resourceSpeciesId, double dryMassToBeSubstracted);

    const bool isLeaf() const;

    void updateMoisture() override;
    void updateChildrenMoisture();
    void update(const unsigned int timeStep, std::ostream& tuneTraitsFile) override;
    void updateChildren(const unsigned int timeStep, std::ostream& tuneTraitsFile);

    std::unique_ptr<std::vector<unsigned int>> calculateChildrenPositions(const PointMap &sourcePosition) const;
    void calculateChildrenPositionsRecursively(std::unique_ptr<std::vector<unsigned int>> &childrenPositions, 
                                   const unsigned int contactAxis, const bool bottomContactSide, const unsigned int currentCombination, 
                                   const unsigned int indexVector) const;

    void obtainInhabitableTerrainCells() override;

    SpatialTreeTerrainCellInterface* const getCell(const PointSpatialTree &cellPos);

    void printAnimals(std::ofstream &file) const;
    void printCell(std::vector<std::pair<std::vector<double>, std::vector<unsigned int>>> &mapCellsInfo);

    void obtainWorldAnimalsPopulation(std::vector<std::vector<unsigned int>> &worldAnimalsPopulation) override;
    void obtainAnimalsPopulationAndGeneticsFrequencies(std::vector<std::vector<unsigned int>> &worldAnimalsPopulation, std::vector<std::vector<std::pair<std::vector<double>, std::vector<double>>>> &worldGeneticsFrequencies) override;
    void saveAnimalSpeciesSnapshot(std::ofstream &file, const AnimalSpecies* const &species) override;
    void saveResourceSpeciesSnapshot(std::ofstream &file, const ResourceSpecies* const &species) const override;
    void saveWaterSnapshot(std::ofstream &file) const override;
    void moveAnimals(int day, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, std::ostream& edibilitiesFile, float exitTimeThreshold, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH) override;
    void performAnimalsActions(int timeStep, std::ostream& voracitiesFile, fs::path outputFolder, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);

    /**
     * @name Getters
     * @{
     */

    /**
     * @name Edibles
     * @{
     */

    /**
     * @name On area
     * @{
     */
    EdiblesOnRadius getMutableEdiblesDown(
        std::function<bool(AnimalInterface&)> downChecker, const Ring &effectiveArea,
        const EdibleSearchParams &edibleSearchParams
    );
    /** @} */

    /** @} */

    /**
     * @name Animals
     * @{
     */

    /**
     * @name Non-conditional
     * @{
     */
    std::unique_ptr<FullCoverageAnimals> getMutableAnimalsDown(const AnimalSearchParams &animalSearchParams);
    /** @} */

    /** @} */

    /** @} */

    /**
     * @name Patches
     * @{
     */

    /**
     * @name Moisture patches
     * @{
     */
    void setSubdivisionMoisturePatch(const MoisturePatch &moisturePatch) override; //?
    /** @} */

    /** @} */
};

#endif /* BRANCH_TERRAINCELL_H_ */
