#ifndef TERRAIN_CELL_INTERFACE_H_
#define TERRAIN_CELL_INTERFACE_H_


#include <vector>
#include <magic_enum.hpp>
#include <functional>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/Points/PointMap.h"
#include "Misc/Types.h"
#include "IBM/World/LivingBeings/Instar.h"
#include "IBM/World/Map/TerrainCells/Moisture/MoistureInterface.h"
#include "IBM/World/Map/Patches/Moisture/MoisturePatch.h"
#include "IBM/World/Map/Patches/Resource/ResourcePatch.h"
#include "IBM/World/Map/Patches/Obstacle/ObstaclePatch.h"
#include "IBM/World/Map/Geometry/Geometry.h"
#include "IBM/World/LivingBeings/EdibleInterface.h"
#include "IBM/World/LivingBeings/Edible.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"
#include "IBM/World/LivingBeings/LifeStage.h"



class MapInterface;



typedef std::vector<const AnimalInterface*> ConstAnimalVector;
typedef std::vector<AnimalInterface*> AnimalVector;
typedef std::vector<AnimalVector> GenderVector;
typedef std::vector<InstarVector<GenderVector>> AnimalSpeciesVector;
typedef std::vector<AnimalSpeciesVector> LifeStageVector;

typedef std::vector<std::unique_ptr<ResourceInterface>> ResourceSpeciesVector;



typedef std::vector<std::unique_ptr<AnimalVector>> PartialCoverageAnimals;

typedef std::vector<std::unique_ptr<std::vector<AnimalVector*>>> FullCoverageAnimals;

typedef std::pair<std::unique_ptr<PartialCoverageAnimals>, std::unique_ptr<FullCoverageAnimals>> SearchableAnimals;

typedef std::vector<double> ResourcesOnRadius;

typedef std::unordered_map<ResourceInterface*, double> SearchableResources;

typedef std::pair<std::unique_ptr<ResourcesOnRadius>, SearchableAnimals> EdiblesOnRadius;

typedef std::pair<std::unique_ptr<SearchableResources>, SearchableAnimals> SearchableEdibles;


class TerrainCellInterface
{
private:
    friend class boost::serialization::access;

protected:
    virtual void activateAndResumeAnimals(int timeStep)=0;
    virtual const std::unique_ptr<PointContinuous>& getCenter() const=0;
    virtual std::unique_ptr<PointContinuous> makeCenter(const PointMap* const &position, const double &size) const=0;

public:
    enum TerrainCellCoverage : unsigned int
    {
        Full, 
        Sub_Terrain_Cell, 
        Partial
    }; 

    TerrainCellInterface();
    virtual ~TerrainCellInterface();

    // Getter
    virtual const PointMap& getPosition() const=0;
    virtual const MapInterface& getMapInterface() const=0;
    virtual MapInterface& getMutableMapInterface() const=0;
    virtual const WorldInterface* const getWorldInterface() const=0;
    virtual WorldInterface* const getMutableWorldInterface() const=0;
    virtual const double& getSize() const=0;
    virtual const LifeStageVector& getAnimals() const=0;
    
    virtual const Ring&  getEffectiveArea() const=0;

    virtual const unsigned int calculateManhattanDistanceToPoint(const PointMap &point) const=0;

    virtual double getTotalResourceBiomass() const=0;

    virtual double spreadResource(const double &massToSpread, const unsigned int resourceSpeciesId)=0;

    virtual const Ring* const makeEffectiveArea(PointMap* const position, const double size) const=0;

    virtual const double& getTotalMaximumResourceCapacity() const=0;

    virtual void insertAnimal(AnimalInterface* const newAnimal)=0;
    virtual std::tuple<bool, TerrainCellInterface*, TerrainCellInterface*> randomInsertAnimal(AnimalInterface* const newAnimal)=0;
    virtual void addAnimal(const LifeStage &lifeStage, const unsigned int animalSpeciesId, const Instar &instar, const AnimalSpecies::Gender::GenderValue &gender, AnimalInterface* const newAnimal)=0;
    virtual void eraseAnimal(AnimalInterface* const animalToRemove)=0;
    virtual void eraseAnimal(const LifeStage &lifeStage, const unsigned int animalSpeciesId, const Instar &instar, const AnimalSpecies::Gender::GenderValue &gender, AnimalInterface* const animalToRemove)=0;
    virtual void eraseAllAnimals()=0;

    virtual void changeAnimalToSenesced(AnimalInterface* targetAnimal, int day)=0;

    virtual void updateMoisture()=0;
    virtual void update(const unsigned int timeStep, std::ostream& tuneTraitsFile)=0;

    virtual void setCopied(const bool newCopiedValue)=0;

    virtual void migrateAnimalTo(AnimalInterface* animalToMigrate, TerrainCellInterface* newTerrainCell, const PointContinuous &newPosition)=0;

    virtual std::tuple<double, double, double, const EdibleInterface*> getCellEvaluation(AnimalInterface* animalWhoIsEvaluating, std::pair<TerrainCellInterface::TerrainCellCoverage, std::unique_ptr<Ring>> &evaluationArea, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)=0;

    virtual double addResourceBiomass(const double &addedMass, const unsigned int resourceSpeciesId, const TerrainCellInterface* const source)=0;
    virtual double addResourceBiomass(const double &addedMass, const unsigned int resourceSpeciesId, const std::unique_ptr<std::vector<unsigned int>> &childrenPositions)=0;

    virtual void tuneTraits(const unsigned int timeStep, std::ostream& tuneTraitsFile)=0;

    virtual bool resourceIsPresent(const unsigned int resourceSpeciesId) const=0;

    virtual void dieFromBackground(int day)=0;
    virtual void assimilateFoodMass(int timeStep)=0;
    virtual void metabolizeAnimals(int timeStep)=0;
    virtual void growAnimals(int timeStep)=0;
    virtual std::ostream& printAnimalsVoracities(int timeStep, std::ostream& os)=0;

    /**
     * @name Obstacle
     * @{
     */
    virtual bool isObstacle() const=0; //?
    virtual bool isFullObstacle() const=0; //?
    /** @} */

    /**
     * @name Moisture
     * @{
     */
    virtual const MoistureInterface* const& getMoistureInfo() const=0; //?
    virtual bool isMoistureSource() const=0; //?
    virtual bool isInMoisturePatch() const=0; //?
    /** @} */

    /**
     * @name Resource
     * @{
     */
    virtual std::vector<ResourceInterface*>& getMutableResources()=0;
    virtual const std::vector<const ResourceInterface*>& getResources() const=0;
    virtual ResourceInterface* const getMutableResource(const unsigned int resourceSpeciesId)=0;
    virtual const ResourceInterface* const getResource(const unsigned int resourceSpeciesId) const=0; //?
    /** @} */

    virtual void performAnimalsActions(int timeStep, std::ostream& voracitiesFile, fs::path outputFolder, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile)=0;

    virtual void obtainInhabitableTerrainCells()=0;

    virtual void breedAnimals(int timeStep, fs::path outputDirectory, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile)=0;

    virtual double turnEdibleIntoDryMassToBeEaten(EdibleInterface &targetAnimal, const double &targetDryMass, const Ring* const perceptionArea, int day, AnimalInterface* predatorEdible, double leftovers)=0;

    virtual void moveAnimals(int day, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, std::ostream& edibilitiesFile, float exitTimeThreshold, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)=0;

    /**
     * @name Getters
     * @{
     */

    /**
     * @name Edibles
     * @{
     */

    /**
     * @name Non-conditional
     * @{
     */
    virtual SearchableEdibles getMutableEdiblesOnAllCell(const EdibleSearchParams &edibleSearchParams)=0;
    /** @} */

    /**
     * @name On radius
     * @{
     */
    virtual SearchableEdibles getEdiblesOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const EdibleSearchParams &edibleSearchParams
    )=0;
    /** @} */

    /**
     * @name On area
     * @{
     */
    virtual SearchableEdibles getMutableEdiblesOnAllCell(
        TerrainCellInterface::TerrainCellCoverage coverage, const PointContinuous &sourcePosition, const double &radius, 
        const Ring &effectiveArea, const EdibleSearchParams &edibleSearchParams
    )=0;
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
    virtual std::unique_ptr<std::vector<AnimalVector*>> getMutableAnimalsBy(const AnimalSearchParams &animalSearchParams)=0;
    virtual std::unique_ptr<const std::vector<AnimalVector*>> getAnimalsBy(const AnimalSearchParams &animalSearchParams)=0;
    virtual std::unique_ptr<FullCoverageAnimals> getMutableAnimalsOnAllCell(const EdibleSearchParams &edibleSearchParams)=0;
    virtual std::unique_ptr<const FullCoverageAnimals> getAnimalsOnAllCell(const EdibleSearchParams &edibleSearchParams)=0;
    /** @} */

    /**
     * @name On radius
     * @{
     */
    virtual SearchableAnimals getAnimalsOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const EdibleSearchParams &animalSearchParams
    )=0;
    /** @} */

    /**
     * @name On area
     * @{
     */

    /**
     * @name Partial area
     * @{
     */
    virtual std::unique_ptr<AnimalVector> getMutableAnimalsBy(
        std::function<bool(AnimalInterface&)> areaChecker, const AnimalSearchParams &animalSearchParams
    )=0;
    /** @} */

    /** @} */

    /** @} */

    /** @} */

    virtual std::pair<bool, std::pair<TerrainCellInterface*, PointContinuous>> getCellByBearing(
        const std::pair<PointMap, PointContinuous> &targetNeighborToTravelTo, const PointContinuous &animalPosition
    )=0;
    virtual void printAnimals(std::ofstream &file) const=0;
    virtual void printCell(std::vector<std::pair<std::vector<double>, std::vector<unsigned int>>> &mapCellsInfo)=0;
    virtual unsigned int getNumberOfMatureFemales(AnimalSpecies *const animalSpecies)=0;
    virtual std::unique_ptr<std::vector<std::pair<TerrainCellInterface*, std::pair<TerrainCellInterface::TerrainCellCoverage, std::unique_ptr<Ring>>>>> getNeighboursCellsOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const unsigned int searchDepth
    )=0;

    /**
     * @brief Serialize the TerrainCellInterface object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* TERRAIN_CELL_INTERFACE_H_ */
