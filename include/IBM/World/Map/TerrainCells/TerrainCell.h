#ifndef TERRAINCELL_H_
#define TERRAINCELL_H_


#include <fstream>
#include <vector>
#include <memory>
#include <functional>
#include <boost/geometry/arithmetic/arithmetic.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>
#include <boost/filesystem.hpp>

#include "IBM/World/Map/Points/PointMap.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"
#include "Misc/Types.h"
#include "IBM/Maths/MathFunctions.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/Map/TerrainCells/Moisture/MoistureInterface.h"
#include "IBM/World/Map/Patches/Moisture/MoisturePatch.h"
#include "IBM/World/LivingBeings/Resources/ResourceFactory.h"
#include "IBM/World/Map/TerrainCells/Moisture/NullMoisture.h"
#include "IBM/World/Map/Geometry/Geometry.h"
#include "Misc/Utilities.h"
#include "IBM/World/Map/TerrainCells/AnimalSearchParams.h"
#include "IBM/World/Map/TerrainCells/ResourceSearchParams.h"
#include "IBM/World/LivingBeings/LifeStage.h"
#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"




class TerrainCell : public TerrainCellInterface
{
private:
    friend class boost::serialization::access;

protected:
    PointMap* const position;
    const std::unique_ptr<PointContinuous> center;
    const double size;
    MapInterface* const mapInterface;
    const Ring* const effectiveArea;
    bool copied;

    // Total maximum capacity to all resources
    double totalMaximumResourceCapacity;

    const bool isCopied() const;

    void applyFunctionToAnimals(
        const std::vector<std::function<void(AnimalInterface&)>> &functions, const AnimalSearchParams &animalSearchParams
    );

    void randomApplyFunctionToAnimals(
        const std::vector<std::function<void(AnimalInterface&)>> &functions, const AnimalSearchParams &animalSearchParams
    );


    virtual void activateAndResumeAnimals(int timeStep);

    virtual void tuneTraits(const unsigned int timeStep, std::ostream& tuneTraitsFile);

    LifeStageVector* const makeDefaultAnimals(const MapInterface* const &mapInterface);

    void setTotalMaximumResourceCapacity(const double &newTotalMaximumResourceCapacity);

    
    

    const std::unique_ptr<PointContinuous>& getCenter() const;
    std::unique_ptr<PointContinuous> makeCenter(const PointMap* const &position, const double &size) const;
    
    /**
     * @name Obstacle
     * @{
     */
    bool obstacle;
    bool fullObstacle;

    void setObstacle(const bool newObstacleValue, const int newObstaclePatchPriority); //?
    void setFullObstacle(const bool newFullObstacleValue, const bool newObstacleValue, const int newObstaclePatchPriority); //?
    void setFullObstacle(const ObstaclePatch &obstaclePatch); //?
    /** @} */

    /**
     * @name Moisture
     * @{
     */
    MoistureInterface* moistureInfo;
    bool moistureSource;
    bool inMoisturePatch;

    void setMoistureInfo(MoistureInterface* const &newMoistureInfo); //?
    void setMoistureSource(const bool newMoistureSourceValue); //?
    void setInMoisturePatch(const bool newInMoisturePatchValue); //?
    /** @} */

    /**
     * @name Resource
     * @{
     */
    ResourceSpeciesVector resources;
    std::vector<ResourceInterface*> resourcesPointers;
    std::vector<const ResourceInterface*> resourcesConstPointers;

    void setResource(const unsigned int resourceSpeciesId, std::unique_ptr<ResourceInterface> &newResource); //?
    /** @} */

    /**
     * @name Animal
     * @{
     */
    LifeStageVector* const animals; // Stores all the animals living within this cell
    /** @} */

    /**
     * @name Patches
     * @{
     */

    /**
     * @name Obstacle patches
     * @{
     */
    int obstaclePatchPriority;

    void setObstaclePatchPriority(const int newObstaclePatchPriority); //?
    bool canApplyObstaclePatch(const ObstaclePatch &obstaclePatch) const; //?
    virtual bool applyObstaclePatch(const ObstaclePatch &obstaclePatch)=0; //?
    /** @} */

    /**
     * @name Moisture patches
     * @{
     */
    int moisturePatchPriority;

    void setMoisturePatchPriority(const int newMoisturePatchPriority); //?
    bool canApplyMoisturePatch(const MoisturePatch &moisturePatch) const; //?
    virtual std::pair<bool, bool> applyMoisturePatch(const MoisturePatch &moisturePatch)=0; //?
    virtual void setMoisturePatch(MoistureInterface* const &newMoistureInfo, const int newMoisturePatchPriority, const bool newMoistureSourceValue, const bool newInMoisturePatchValue); //?
    virtual void setMoistureSourcePatch(const MoisturePatch &moisturePatch); //?
    virtual void setMoistureSourcePatch(MoistureInterface* const &newMoistureInfo, const int newMoisturePatchPriority); //?
    /** @} */

    /**
     * @name Resource patches
     * @{
     */
    std::vector<int> resourcePatchPriority;

    void setResourcePatchPriority(const unsigned int resourceSpeciesId, const int newResourcePatchPriority); //?
    bool canApplyResourcePatch(const ResourcePatch &resourcePatch) const; //?
    virtual bool applyResourcePatch(const ResourcePatch &resourcePatch)=0; //?
    void setResourceSourcePatch(const ResourcePatch &resourcePatch); //?
    virtual void setResourceSourcePatch(std::unique_ptr<ResourceInterface> &newResource, const int newResourcePatchPriority); //?
    virtual void setResourcePatch(const unsigned int resourceSpeciesId, std::unique_ptr<ResourceInterface> &newResource, const int newResourcePatchPriority); //?
    /** @} */

    /** @} */

    std::tuple<double, double, double> evaluateAnimal(const AnimalInterface &animalWhoIsEvaluating, 
        const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, const AnimalInterface &animal,
        double preferenceThresholdForEvaluation, 
        double muForPDF, double sigmaForPDF, 
        double predationSpeedRatioAH, double predationHunterVoracityAH, 
        double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, 
        double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, 
        double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, 
        double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, 
        double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, 
        double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH
    );

    double evaluateResource(const AnimalInterface &animalWhoIsEvaluating, 
        const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, const ResourceInterface* resource,
        const double dryMass, double preferenceThresholdForEvaluation, 
        double muForPDF, double sigmaForPDF, 
        double predationSpeedRatioAH, double predationHunterVoracityAH, 
        double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, 
        double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, 
        double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, 
        double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, 
        double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, 
        double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH
    );

    virtual void updateTotalMaximumResourceCapacity();

public:
    TerrainCell(PointMap* const &position, const Ring *const effectiveArea, const double &size, MapInterface* const &mapInterface,
        LifeStageVector* const animals, const bool obstacle, const bool fullObstacle, 
        const int obstaclePatchPriority, MoistureInterface* const moistureInfo, const bool moistureSource, 
        const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity);
    virtual ~TerrainCell();

    // Getter
    const PointMap& getPosition() const;
    PointMap& getMutablePosition() const;
    virtual const MapInterface& getMapInterface() const;
    virtual MapInterface& getMutableMapInterface() const;
    const WorldInterface* const getWorldInterface() const;
    WorldInterface* const getMutableWorldInterface() const;
    const double& getSize() const;
    const LifeStageVector& getAnimals() const;
    LifeStageVector& getMutableAnimals() const;
    
    const Ring& getEffectiveArea() const;

    double getTotalResourceBiomass() const;

    double spreadResource(const double &massToSpread, const unsigned int resourceSpeciesId);
    
    double turnEdibleIntoDryMassToBeEaten(EdibleInterface &targetAnimal, const double &targetDryMass, const Ring* const perceptionArea, int day, AnimalInterface* predatorEdible, double leftovers);

    const Ring* const makeEffectiveArea(PointMap* const position, const double size) const;

    void changeAnimalToSenesced(AnimalInterface* targetAnimal, int day);

    const unsigned int calculateManhattanDistanceToPoint(const PointMap &point) const;

    virtual void insertAnimal(AnimalInterface* const newAnimal);
    virtual std::tuple<bool, TerrainCellInterface*, TerrainCellInterface*> randomInsertAnimal(AnimalInterface* const newAnimal);
    void addAnimal(const LifeStage &lifeStage, const unsigned int animalSpeciesId, const Instar &instar, const AnimalSpecies::Gender::GenderValue &gender, AnimalInterface* const newAnimal);
    void eraseAnimal(AnimalInterface* const animalToRemove);
    void eraseAnimal(const LifeStage &lifeStage, const unsigned int animalSpeciesId, const Instar &instar, const AnimalSpecies::Gender::GenderValue &gender, AnimalInterface* const animalToRemove);
    void eraseAllAnimals();
    void purgeDeadAnimals();

    const double& getTotalMaximumResourceCapacity() const;

    virtual void obtainInhabitableTerrainCells();

    virtual void moveAnimals(int day, std::ostream& encounterProbabilitiesFile, std::ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, std::ostream& edibilitiesFile, float exitTimeThreshold, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);

    virtual void updateMoisture();
    virtual void update(const unsigned int timeStep, std::ostream& tuneTraitsFile);

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
    std::pair<std::unique_ptr<ResourcesOnRadius>, std::unique_ptr<std::vector<AnimalVector*>>> getMutableEdiblesBy(
        const EdibleSearchParams &edibleSearchParams
    );
    /** @} */

    /**
     * @name On area
     * @{
     */
    std::pair<std::unique_ptr<ResourcesOnRadius>, std::unique_ptr<AnimalVector>> getMutableEdiblesBy(
        std::function<bool(AnimalInterface&)> areaChecker, const Ring &effectiveArea, const EdibleSearchParams &edibleSearchParams
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
    std::unique_ptr<std::vector<AnimalVector*>> getMutableAnimalsBy(const AnimalSearchParams &animalSearchParams);
    std::unique_ptr<const std::vector<AnimalVector*>> getAnimalsBy(const AnimalSearchParams &animalSearchParams);
    /** @} */

    /**
     * @name On radius
     * @{
     */
    SearchableAnimals getAnimalsOnRadius(
        const PointContinuous &sourcePosition, const double &radius, const EdibleSearchParams &animalSearchParams
    );
    /** @} */

    /**
     * @name On area
     * @{
     */

    /**
     * @name Partial area
     * @{
     */
    std::unique_ptr<AnimalVector> getMutableAnimalsBy(
        std::function<bool(AnimalInterface&)> areaChecker, const AnimalSearchParams &animalSearchParams
    );
    /** @} */

    /** @} */

    /** @} */

    /**
     * @name Resources
     * @{
     */

    /**
     * @name Non-conditional
     * @{
     */
    std::unique_ptr<ResourcesOnRadius> getResourcesBy(const ResourceSearchParams &resourceSearchParams);
    /** @} */

    /**
     * @name On radius
     * @{
     */
    std::unique_ptr<ResourcesOnRadius> getRadiusResourcesBy(
        const double &percent, const ResourceSearchParams &resourceSearchParams
    );
    /** @} */

    /**
     * @name On area
     * @{
     */
    std::unique_ptr<ResourcesOnRadius> getResourcesBy(const Ring &effectiveArea, const ResourceSearchParams &resourceSearchParams);
    /** @} */

    /** @} */

    /** @} */

    
    

    void printAnimals(std::ofstream &file) const;


    /**
     * @name Obstacle
     * @{
     */
    bool isObstacle() const; //?
    bool isFullObstacle() const; //?
    /** @} */

    /**
     * @name Moisture
     * @{
     */
    const MoistureInterface* const& getMoistureInfo() const; //?
    MoistureInterface* const& getMutableMoistureInfo() const;
    bool isMoistureSource() const; //?
    bool isInMoisturePatch() const; //?
    /** @} */

    /**
     * @name Resource
     * @{
     */
    std::vector<ResourceInterface*>& getMutableResources();
    const std::vector<const ResourceInterface*>& getResources() const;
    ResourceInterface* const getMutableResource(const unsigned int resourceSpeciesId);
    const ResourceInterface* const getResource(const unsigned int resourceSpeciesId) const;
    const double getBiomass(const unsigned int resourceSpeciesId) const;
    bool resourceIsPresent(const unsigned int resourceSpeciesId) const;
    double addResourceBiomass(const double &addedMass, const unsigned int resourceSpeciesId, const TerrainCellInterface* const source);
    double addResourceBiomass(const double &addedMass, const unsigned int resourceSpeciesId, const std::unique_ptr<std::vector<unsigned int>> &childrenPositions);
    const std::vector<int>& getResourcePatchPriority() const;
    /** @} */

    /**
     * @name Patches
     * @{
     */

    /**
     * @name Obstacle patches
     * @{
     */
    bool checkObstaclePatch(const ObstaclePatch &obstaclePatch); //?
    const int getObstaclePatchPriority() const; //?
    /** @} */

    /**
     * @name Moisture patches
     * @{
     */
    std::pair<bool, bool> checkMoisturePatch(const MoisturePatch &moisturePatch); //?
    const int getMoisturePatchPriority() const; //?
    /** @} */

    /**
     * @name Resource patches
     * @{
     */
    bool checkResourcePatch(const ResourcePatch &resourcePatch); //?
    const int getResourcePatchPriority(const unsigned int resourceSpeciesId) const; //?
    /** @} */

    /** @} */

    void performAnimalsActions(int timeStep, std::ostream& voracitiesFile, boost::filesystem::path outputFolder, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);
    virtual void obtainWorldAnimalsPopulation(std::vector<std::vector<unsigned int>> &worldAnimalsPopulation);
    virtual void obtainAnimalsPopulationAndGeneticsFrequencies(std::vector<std::vector<unsigned int>> &worldAnimalsPopulation, std::vector<std::vector<std::pair<std::vector<double>, std::vector<double>>>> &worldGeneticsFrequencies);
    virtual void saveAnimalSpeciesSnapshot(std::ofstream &file, const AnimalSpecies* const &species);
    virtual void saveResourceSpeciesSnapshot(std::ofstream &file, const ResourceSpecies* const &species) const;
    virtual void saveWaterSnapshot(std::ofstream &file) const;
    void breedAnimals(int timeStep, boost::filesystem::path outputDirectory, bool saveAnimalConstitutiveTraits, std::ofstream &constitutiveTraitsFile);
    void dieFromBackground(int day);
    void assimilateFoodMass(int timeStep);
    void metabolizeAnimals(int timeStep);
    void growAnimals(int timeStep);
    std::ostream& printAnimalsVoracities(int timeStep, std::ostream& os);
    void setCopied(const bool newCopiedValue);
    unsigned int getNumberOfMatureFemales(AnimalSpecies *const animalSpecies);
    std::tuple<double, double, double, const EdibleInterface*> getCellEvaluation(AnimalInterface* animalWhoIsEvaluating, std::pair<TerrainCellInterface::TerrainCellCoverage, std::unique_ptr<Ring>> &evaluationArea, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH);

    /**
     * @brief Serialize the TerrainCell object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* TERRAINCELL_H_ */
