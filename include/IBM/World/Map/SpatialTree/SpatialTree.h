#ifndef SPATIAL_TREE_H_
#define SPATIAL_TREE_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <magic_enum.hpp>
#include <cmath>
#include <boost/filesystem.hpp>

#include "IBM/World/Map/TerrainCells/TerrainCell.h"
#include "IBM/World/Map/Points/PointMap.h"
#include "IBM/World/Map/SpatialTree/SpatialTreeInterface.h"
#include "IBM/World/Map/SpatialTree/Points/PointSpatialTree.h"
#include "IBM/World/Map/Points/PointContinuous.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCellInterface.h"
#include "IBM/World/Map/SpatialTree/TerrainCells/RootTerrainCell.h"
#include "IBM/World/Map/Patches/Moisture/MoisturePatch.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/Map/SpatialTree/LivingBeings/SpatialTreeAnimalFactory.h"
#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "Exceptions/LineInfoException.h"





class SpatialTree : public SpatialTreeInterface
{
protected:
    const unsigned int mapDepth;
    const std::vector<double> cellSizes;
    const std::vector<unsigned int> axisSizes;
    unsigned int totalNumberOfTerrainCells;
    const std::unique_ptr<RootTerrainCell> rootTerrainCell;

    std::pair<std::unique_ptr<std::vector<std::vector<std::vector<TerrainCellInterface *>::iterator>>>, std::unique_ptr<std::vector<std::vector<std::vector<TerrainCellInterface *>::iterator>>>> obtainLeafAndBranchInhabitableTerrainCells();
    const unsigned int calculateMapDepth() const;
    std::vector<double> calculateCellSizes() const;
    std::vector<unsigned int> calculateAxisSizes() const;
    unsigned int calculateTotalNumberOfTerrainCells() const;
    double logMap(const double &value) const;
    const unsigned int calculateAnimalDepth(const AnimalInterface* const newAnimal);
    PointContinuous calculateCenterPosition(const PointSpatialTree &cell, const double &size) const;
    void obtainMapCellsInfo(std::vector<std::pair<std::vector<double>, std::vector<unsigned int>>> &mapCellsInfo);

public:
    SpatialTree(const nlohmann::json &mapConfig, const nlohmann::json &spatialTreeConfig, WorldInterface* const worldInterface);
    virtual ~SpatialTree();

    /**
     * @name Patches
     * @{
     */

    /**
     * @name Obstacle patches
     * @{
     */
    void applyObstaclePatch(const std::unique_ptr<const ObstaclePatch> &obstaclePatch);
    /** @} */

    /**
     * @name Moisture patches
     * @{
     */
    bool applyMoisturePatch(const MoisturePatch &moisturePatch);
    /** @} */
    
    /**
     * @name Resource patches
     * @{
     */
    bool applyResourcePatch(const ResourcePatch &resourcePatch);
    /** @} */

    /** @} */

    const unsigned int getMapDepth() const;
    const double& getCellSize(const unsigned int depth) const;
    const unsigned int& getAxisSize(const unsigned int depth) const;
	SpatialTreeTerrainCellInterface* const getCell(const PointMap &cellPos) const;
    SpatialTreeTerrainCellInterface* const getCell(const PointSpatialTree &dstPos, SpatialTreeTerrainCellInterface* const &srcCell) const;
    void insertAnimal(Animal* const newAnimal);
    void updateMoisture();
    void update(const unsigned int timeStep, std::ofstream &tuneTraitsFile);
    PointMap obtainPointMap(const PointContinuous &point, const unsigned int depth) const;
    const unsigned int& getTotalNumberOfTerrainCells() const;
    
    std::unique_ptr<std::vector<TerrainCellInterface*>> getResourceNeighbours(TerrainCellInterface* const &source, const unsigned int resourceSpeciesId, const unsigned int distance);
    std::unique_ptr<std::vector<TerrainCellInterface*>> getManhattanNeighbours(TerrainCellInterface* const &source, const unsigned int distance);
    std::unique_ptr<std::vector<PointSpatialTree>> generatePointsRecursively(const unsigned int sideLength, std::vector<unsigned int> &currentCoords, unsigned int axis, const PointSpatialTree& centralCoords, const std::vector<int> &initialCoords) const;
    std::unique_ptr<std::vector<PointSpatialTree>> generatePointsRecursively(const unsigned int sideLength, std::vector<unsigned int> &currentCoords, unsigned int axis, unsigned int depth, const std::vector<int> &initialCoords) const;
    std::unique_ptr<std::vector<PointSpatialTree>> generatePoints(const unsigned int sideLength, const PointSpatialTree& centralCoords, const std::vector<int> &initialCoords) const;
    std::unique_ptr<std::vector<PointSpatialTree>> generatePoints(const unsigned int sideLength, const std::vector<int> &initialCoords, unsigned int depth) const;
    void obtainInhabitableTerrainCells();
    std::unique_ptr<std::vector<InstarVector<std::vector<std::vector<TerrainCellInterface*>::iterator>>>> obtainSpeciesInhabitableTerrainCells();
    std::pair<std::unique_ptr<std::vector<InstarVector<std::vector<AnimalInterface*>>>>, unsigned int> generateStatisticsPopulation(std::vector<InstarVector<std::vector<std::vector<TerrainCellInterface*>::iterator>>> &mapSpeciesInhabitableTerrainCells);
    const int generatePopulation(AnimalSpecies* currentAnimalSpecies, const InstarVector<std::vector<std::vector<TerrainCellInterface*>::iterator>> &speciesInhabitableTerrainCells);
    void eraseStatisticsPopulation();
    void purgeDeadAnimals();
    void printAnimalsAlongCells(std::ofstream &file) const;
    std::string getMapPositionHeader() const;
    void obtainWorldResourceBiomassAndAnimalsPopulation(std::vector<double> &worldResourceBiomass, std::vector<std::vector<unsigned int>> &worldAnimalsPopulation) const;
    void obtainAnimalsPopulationAndGeneticsFrequencies(std::vector<std::vector<unsigned int>> &worldAnimalsPopulation, std::vector<std::vector<std::pair<std::vector<double>, std::vector<double>>>> &worldGeneticsFrequencies) const;
    void saveAnimalSpeciesSnapshot(std::ofstream &file, const AnimalSpecies* const &species) const;
    void saveResourceSpeciesSnapshot(std::ofstream &file, const ResourceSpecies* const &species) const;
    void saveWaterSnapshot(std::ofstream &file) const;
    SearchableEdibles getEdiblesOnRadius(
        TerrainCellInterface* const sourceTerrainCell, const PointContinuous &sourcePosition, const double &radius,
        const EdibleSearchParams &edibleSearchParams
    );
    void moveAnimals(int day, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, std::ostream& edibilitiesFile, float exitTimeThreshold, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);
    void performAnimalsActions(int timeStep, std::ostream& voracitiesFile, boost::filesystem::path outputFolder, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);
};

#endif /* SPATIAL_TREE_H_ */