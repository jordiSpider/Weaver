#ifndef MAP_INTERFACE_H_
#define MAP_INTERFACE_H_

#include <string>
#include <magic_enum.hpp>
#include <unordered_map>

#include "World/Map/Points/PointMap.h"
#include "World/LivingBeings/Animals/Animal.h"
#include "Exceptions/LineInfoException.h"
#include "World/Map/Patches/Moisture/MoisturePatch.h"
#include "World/Map/Geometry/Geometry.h"
#include "World/Map/TerrainCells/TerrainCellInterface.h"
#include "World/LivingBeings/LifeStage.h"



class WorldInterface;



class MapInterface {
public:
    MapInterface();
    virtual ~MapInterface();

    /**
     * @name Patches
     * @{
     */

    /**
     * @name Obstacle patches
     * @{
     */
    virtual void applyObstaclePatch(const std::unique_ptr<const ObstaclePatch> &obstaclePatch)=0;
    /** @} */

    /**
     * @name Moisture patches
     * @{
     */
    virtual bool applyMoisturePatch(const MoisturePatch &moisturePatch)=0;
    /** @} */

    /**
     * @name Resource patches
     * @{
     */
    virtual bool applyResourcePatch(const ResourcePatch &resourcePatch)=0;
    /** @} */

    /** @} */

    virtual const double& getMinCellSize() const=0;
    virtual const WorldInterface* const getWorldInterface() const=0;
    virtual WorldInterface* const getMutableWorldInterface() const=0;
    virtual void insertAnimal(Animal* const newAnimal)=0;
    virtual TerrainCellInterface* const getCell(const PointMap &cellPos) const=0;
    virtual void updateMoisture()=0;
    virtual void update(const unsigned int timeStep, std::ofstream &tuneTraitsFile)=0;

    virtual PointMap obtainPointMap(const PointContinuous &point, const unsigned int depth) const=0;

    virtual void obtainLifeStageSearchParams()=0;
    virtual const AnimalSearchParams& getLifeStageSearchParams(const LifeStage::LifeStageValue &lifeStage) const=0;

    virtual std::unique_ptr<std::vector<TerrainCellInterface*>> getResourceNeighbours(TerrainCellInterface* const &source, const unsigned int resourceSpeciesId, const unsigned int distance)=0;
    virtual std::unique_ptr<std::vector<TerrainCellInterface*>> getManhattanNeighbours(TerrainCellInterface* const &source, const unsigned int distance)=0;
    virtual const std::vector<TerrainCellInterface*>& getInhabitableTerrainCells() const=0;
    virtual TerrainCellInterface*& getMutableInhabitableTerrainCell(const unsigned int index)=0;
    virtual void addInhabitableTerrainCell(TerrainCellInterface* newInhabitableTerrainCell)=0;
    virtual void obtainInhabitableTerrainCells()=0;
    virtual std::unique_ptr<std::vector<InstarVector<std::vector<std::vector<TerrainCellInterface*>::iterator>>>> obtainSpeciesInhabitableTerrainCells()=0;
    virtual std::pair<std::unique_ptr<std::vector<InstarVector<std::vector<AnimalInterface*>>>>, unsigned int> generateStatisticsPopulation(std::vector<InstarVector<std::vector<std::vector<TerrainCellInterface*>::iterator>>> &mapSpeciesInhabitableTerrainCells)=0;
    virtual void eraseStatisticsPopulation()=0;
    virtual void purgeDeadAnimals()=0;
    virtual const int generatePopulation(AnimalSpecies* currentAnimalSpecies, const InstarVector<std::vector<std::vector<TerrainCellInterface*>::iterator>> &speciesInhabitableTerrainCells)=0;
    virtual SearchableEdibles getEdiblesOnRadius(
        TerrainCellInterface* const sourceTerrainCell, const PointContinuous &sourcePosition, const double &radius,
        const EdibleSearchParams &edibleSearchParams
    )=0;
    virtual void printAnimalsAlongCells(std::ofstream &file) const=0;
    virtual void printCellAlongCells(std::ofstream &file)=0;
    virtual const unsigned int& getNumberOfCellsPerAxis() const=0;
    virtual std::string getMapPositionHeader() const=0;
    virtual void obtainWorldResourceBiomassAndAnimalsPopulation(std::vector<double> &worldResourceBiomass, std::vector<std::vector<unsigned int>> &worldAnimalsPopulation) const=0;
    virtual void obtainAnimalsPopulationAndGeneticsFrequencies(std::vector<std::vector<unsigned int>> &worldAnimalsPopulation, std::vector<std::vector<std::pair<std::vector<double>, std::vector<double>>>> &worldGeneticsFrequencies) const=0;
    virtual void saveAnimalSpeciesSnapshot(std::ofstream &file, const AnimalSpecies* const &species) const=0;
    virtual void saveResourceSpeciesSnapshot(std::ofstream &file, const ResourceSpecies* const &species) const=0;
    virtual void saveWaterSnapshot(std::ofstream &file) const=0;
    virtual void moveAnimals(int day, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, std::ostream& edibilitiesFile, float exitTimeThreshold, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)=0;
    virtual void performAnimalsActions(int timeStep, std::ostream& voracitiesFile, fs::path outputFolder, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile)=0;
    virtual const unsigned int& getTotalNumberOfTerrainCells() const=0;
};

#endif /* MAP_INTERFACE_H_ */