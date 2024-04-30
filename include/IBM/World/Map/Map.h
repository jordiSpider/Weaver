#ifndef MAP_H_
#define MAP_H_

#include <nlohmann/json.hpp>
#include <boost/geometry/algorithms/convert.hpp>
#include <fstream>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Misc/Types.h"
#include "IBM/World/Map/Patches/Moisture/MoistureBasePatch.h"
#include "IBM/World/LivingBeings/LifeStage.h"
#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"
#include "Misc/EnumClass.h"
#include "IBM/World/LivingBeings/Animals/AnimalStatistical.h"


class World;
class AnimalSearchParams;


class Map {
private:
    friend class boost::serialization::access;

protected:
    const double minCellSize;
    const unsigned int numberOfCellsPerAxis;
    World* const world;
    std::vector<TerrainCellInterface*> inhabitableTerrainCells;
    std::vector<AnimalSearchParams> lifeStageSearchParams;

    bool isSpeciesInhabitableTerrainCell(
        const AnimalSpecies &animalSpecies, const TerrainCellInterface* const potentialInhabitableTerrainCell, 
        const Instar &instar
    );
    
    virtual void obtainMapCellsInfo(std::vector<std::pair<std::vector<double>, std::vector<unsigned int>>> &mapCellsInfo)=0;

public:
    enum class Type : unsigned int
    {
        SpatialTree
    };


    static std::unique_ptr<Map> createInstance(const nlohmann::json &mapInfo, World* const world);


    Map(const nlohmann::json &mapConfig, World* const world);
    virtual ~Map();

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

    void addInhabitableTerrainCell(TerrainCellInterface* newInhabitableTerrainCell);
    const double& getMinCellSize() const;
    const unsigned int& getNumberOfCellsPerAxis() const;
    const World* const getWorld() const;
    void printCellAlongCells(std::ofstream &file);
    World* const getMutableWorld() const;
    const std::vector<TerrainCellInterface*>& getInhabitableTerrainCells() const;
    std::vector<TerrainCellInterface*>& getMutableInhabitableTerrainCells();
    TerrainCellInterface*& getMutableInhabitableTerrainCell(const unsigned int index);
    void obtainLifeStageSearchParams();
    const AnimalSearchParams& getLifeStageSearchParams(const LifeStage::LifeStageValue &lifeStage) const;
    virtual void insertAnimal(Animal* const newAnimal)=0;
    virtual TerrainCellInterface* const getCell(const PointMap &cellPos) const=0;
    virtual void updateMoisture()=0;
    virtual void update(const unsigned int numberOfTimeSteps, std::ofstream &tuneTraitsFile)=0;

    virtual PointMap obtainPointMap(const PointContinuous &point, const unsigned int depth) const=0;

    virtual std::unique_ptr<std::vector<TerrainCellInterface*>> getResourceNeighbours(TerrainCellInterface* const &source, const unsigned int resourceSpeciesId, const unsigned int distance)=0;
    virtual std::unique_ptr<std::vector<TerrainCellInterface*>> getManhattanNeighbours(TerrainCellInterface* const &source, const unsigned int distance)=0;
    virtual void obtainInhabitableTerrainCells()=0;
    virtual std::unique_ptr<std::vector<InstarVector<std::vector<std::vector<TerrainCellInterface*>::iterator>>>> obtainSpeciesInhabitableTerrainCells()=0;
    virtual std::pair<std::unique_ptr<std::vector<InstarVector<std::vector<AnimalStatistical*>>>>, unsigned int> generateStatisticsPopulation(std::vector<InstarVector<std::vector<std::vector<TerrainCellInterface*>::iterator>>> &mapSpeciesInhabitableTerrainCells)=0;
    virtual void eraseStatisticsPopulation()=0;
    virtual void purgeDeadAnimals()=0;
    virtual const unsigned int generatePopulation(AnimalSpecies* currentAnimalSpecies, const InstarVector<std::vector<std::vector<TerrainCellInterface*>::iterator>> &speciesInhabitableTerrainCells)=0;
    virtual SearchableEdibles getEdiblesOnRadius(
        TerrainCellInterface* const sourceTerrainCell, const PointContinuous &sourcePosition, const double &radius,
        const EdibleSearchParams &edibleSearchParams
    )=0;
    virtual void printAnimalsAlongCells(std::ofstream &file) const=0;
    virtual std::string getMapPositionHeader() const=0;
    virtual void obtainWorldResourceBiomassAndAnimalsPopulation(std::vector<double> &worldResourceBiomass, std::vector<std::vector<unsigned int>> &worldAnimalsPopulation) const=0;
    virtual void obtainAnimalsPopulationAndGeneticsFrequencies(std::vector<std::vector<unsigned int>> &worldAnimalsPopulation, std::vector<std::vector<std::pair<std::vector<double>, std::vector<double>>>> &worldGeneticsFrequencies) const=0;
    virtual void saveAnimalSpeciesSnapshot(std::ofstream &file, const AnimalSpecies* const &species) const=0;
    virtual void saveResourceSpeciesSnapshot(std::ofstream &file, const ResourceSpecies* const &species) const=0;
    virtual void saveWaterSnapshot(std::ofstream &file) const=0;
    virtual void moveAnimals(const unsigned int numberOfTimeSteps, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, std::ostream& edibilitiesFile, float exitTimeThreshold, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)=0;
    virtual void performAnimalsActions(const unsigned int numberOfTimeSteps, std::ostream& voracitiesFile, boost::filesystem::path outputFolder, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile)=0;
    virtual const unsigned int& getTotalNumberOfTerrainCells() const=0;

    /**
     * @brief Serialize the Map object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* MAP_H_ */