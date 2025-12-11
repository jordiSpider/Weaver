/**
* @file Map.h
* @brief Defines the Map class for spatial organization of landscapes.
*
* This file declares the Map class, which provides the interface and common
* functionalities for managing spatial cells, terrain features, and animal
* placement in landscape simulations. It includes methods for animal registration,
* resource management, patch application, and population generation.
*/

#ifndef MAP_H_
#define MAP_H_

#include <nlohmann/json.hpp>
#include <boost/geometry/algorithms/convert.hpp>
#include <fstream>


#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


#include <fstream>
#include <ostream>
#include <unordered_set>

#include "Misc/Types.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"
#include "Misc/EnumClass.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalStatistical.h"
#include "Misc/CustomIndexedVector.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"
#include "App/View/View.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/HuntingMode.h"
#include "App/Model/IBM/Maths/PreciseDouble.h"


class Landscape;
class AnimalSearchParams;


/**
 * @class Map
 * @brief Abstract base class for spatial landscape representations.
 *
 * The Map interface defines methods for managing terrain cells, animals, and resources
 * in a spatially explicit landscape. Derived classes implement specific spatial
 * structures such as SpatialTree.
 */
class Map {
protected:
    PreciseDouble minCellSize; /**< Minimum cell size in the map */
    PreciseDouble minHyperVolume; /**< Minimum hypervolume of a cell */
    unsigned int numberOfCellsPerAxis; /**< Number of cells along each axis */
    std::vector<TerrainCell*> inhabitableTerrainCells; /**< Cells that can be inhabited by animals */
    CustomIndexedVector<LifeStage, AnimalSearchParams> lifeStageSearchParams; /**< Search parameters per life stage */
    AnimalSearchParams allAnimalsSearchParams; /**< Search parameters for all animals */

    /**
    * @brief Checks if a species can inhabit a given terrain cell.
    *
    * @param animalSpecies Species to check.
    * @param potentialInhabitableTerrainCell Terrain cell to check.
    * @param involvedResourceSpecies Resources relevant to the species.
    * @return True if the species can inhabit the cell, false otherwise.
    */
    bool isSpeciesInhabitableTerrainCell(
        const AnimalSpecies &animalSpecies, const TerrainCell* const potentialInhabitableTerrainCell, 
        const std::vector<ResourceSpecies::ResourceID>& involvedResourceSpecies
    );

public:
    /**
    * @brief Type of map implementation.
    */
    enum class Type : unsigned int
    {
        SpatialTree /**< Map organized as a spatial tree */
    };

    /**
    * @brief Factory method to create a Map instance based on JSON configuration.
    *
    * @param mapInfo JSON map configuration.
    * @param moistureBaseSource Base moisture source.
    * @return Pointer to a new Map instance.
    */
    static Map* createInstance(const nlohmann::json &mapInfo, MoistureSource* const moistureBaseSource);

    /**
    * @brief Default constructor.
    */
    Map();

    /**
    * @brief Constructor from JSON configuration.
    *
    * @param mapConfig JSON map configuration.
    */
    Map(const nlohmann::json &mapConfig);

    /**
    * @brief Virtual destructor.
    */
    virtual ~Map();

    /**
    * @brief Deleted copy constructor.
    */
    Map(const Map&) = delete;

    /**
    * @brief Deleted copy assignment operator.
    */
    Map& operator=(const Map&) = delete;

    /**
    * @brief Apply a patch to the map (pure virtual).
    * @param landscape Associated landscape.
    * @param patch Patch to apply.
    * @return True if applied successfully.
    */
    virtual bool applyPatch(Landscape* const landscape, const Patch& patch)=0;

    /**
    * @brief Adds a terrain cell that can be inhabited.
    * @param newInhabitableTerrainCell Pointer to terrain cell.
    */
    void addInhabitableTerrainCell(TerrainCell* newInhabitableTerrainCell);
    
    /**
     * @brief Returns the minimum cell size.
     * @return Reference to minCellSize.
     */
    const PreciseDouble& getMinCellSize() const;

    /**
     * @brief Returns the minimum hypervolume of a cell.
     * @return Reference to minHyperVolume.
     */
    const PreciseDouble& getMinHyperVolume() const;

    /**
     * @brief Returns the number of cells per axis.
     * @return Number of cells per axis.
     */
    unsigned int getNumberOfCellsPerAxis() const;

    /**
     * @brief Returns all inhabitable terrain cells.
     * @return Const reference to the vector of terrain cells.
     */
    const std::vector<TerrainCell*>& getInhabitableTerrainCells() const;

    /**
     * @brief Returns mutable access to the terrain cells vector.
     * @return Reference to the vector of terrain cells.
     */
    std::vector<TerrainCell*>& getMutableInhabitableTerrainCells();

    /**
     * @brief Returns mutable access to a specific terrain cell.
     * @param index Index of the terrain cell.
     * @return Reference to the terrain cell pointer.
     */
    TerrainCell*& getMutableInhabitableTerrainCell(const unsigned int index);
    
    /**
     * @brief Returns the search parameters for a given life stage.
     * @param lifeStage Life stage to retrieve parameters for.
     * @return Const reference to the AnimalSearchParams.
     */
    const AnimalSearchParams& getLifeStageSearchParams(const LifeStage &lifeStage) const;
    
    /**
     * @brief Returns the search parameters for all animals.
     * @return Const reference to the AnimalSearchParams.
     */
    const AnimalSearchParams& getAllAnimalsSearchParams() const;
    
    /**
     * @brief Inserts a new animal into the map.
     * @param landscape Pointer to the Landscape object.
     * @param newAnimal Pointer to the new Animal object.
     */
    virtual void insertAnimal(Landscape* const landscape, Animal* const newAnimal)=0;
    
    /**
     * @brief Updates the map and all its entities for a timestep.
     * @param numberOfTimeSteps Current timestep.
     */
    virtual void update(const TimeStep& numberOfTimeSteps)=0;

    /**
     * @brief Registers edible resources and non-statistical animals.
     */
    virtual void registerEdibles(std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, std::vector<AnimalNonStatistical*>& landscapeAnimals)=0;

    /**
     * @brief Adds a new animal species to the map.
     */
    virtual void addAnimalSpecies(const AnimalSpecies& animalSpecies, const PreciseDouble& timeStepsPerDay)=0;
    
    /**
     * @brief Adds a new resource species to the map.
     */
    virtual void addResourceSpecies(Landscape* const landscape, std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource, const PreciseDouble& timeStepsPerDay)=0;

    /**
     * @brief Deserializes moisture and resource sources.
     */
    virtual void deserializeSources(
        std::vector<std::pair<size_t, MoistureSource*>>& appliedMoisture,
        std::vector<std::vector<std::pair<size_t, ResourceSource*>>>& appliedResource
    )=0;

    /**
     * @brief Deserializes existing animal and resource species.
     */
    virtual void deserializeSpecies(std::vector<ResourceSpecies*>& existingResourceSpecies, std::vector<AnimalSpecies*>& existingAnimalSpecies)=0;

    /**
     * @brief Increments the total number of active animals.
     */
    virtual void increaseTotalNumberOfActiveAnimals()=0;

    /**
     * @brief Decrements the total number of active animals.
     */
    virtual void decreaseTotalNumberOfActiveAnimals()=0;

    /**
     * @brief Returns a discrete map point corresponding to a continuous point.
     * @param point Continuous coordinates.
     * @param depth Depth level.
     * @return PointMap object.
     */
    virtual PointMap obtainPointMap(const PointContinuous &point, const unsigned int depth) const=0;

    /**
     * @brief Obtains all inhabitable terrain cells.
     */
    virtual void obtainInhabitableTerrainCells()=0;

    /**
     * @brief Obtains terrain cells suitable for each species.
     */
    virtual void obtainSpeciesInhabitableTerrainCells(std::vector<CustomIndexedVector<Instar, std::vector<std::vector<TerrainCell*>::iterator>>>& mapSpeciesInhabitableTerrainCells, const std::vector<AnimalSpecies*>& existingAnimalSpecies, const CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, std::vector<ResourceSpecies::ResourceID>>>& involvedResourceSpecies)=0;
    
    /**
     * @brief Generates a statistical population for analysis.
     */
    virtual unsigned int generateStatisticsPopulation(std::vector<CustomIndexedVector<Instar, std::vector<AnimalStatistical*>>>& animalsPopulation, View* view, Landscape* const landscape, std::vector<AnimalSpecies*>& existingAnimalSpecies, std::vector<CustomIndexedVector<Instar, std::vector<std::vector<TerrainCell*>::iterator>>> &mapSpeciesInhabitableTerrainCells, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)=0;
    
    /**
     * @brief Generates the actual animal population in the landscape.
     */
    virtual size_t generatePopulation(View* view, Landscape* const landscape, AnimalSpecies* currentAnimalSpecies, const CustomIndexedVector<Instar, unsigned int>& population, const CustomIndexedVector<Instar, std::vector<std::vector<TerrainCell*>::iterator>> &speciesInhabitableTerrainCells, const bool saveAnimalConstitutiveTraits, std::ostringstream& animalConstitutiveTraitsFile, const bool saveGenetics, const bool saveMassInfo, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)=0;
    
    /**
     * @brief Returns the header string for map position outputs.
     * @return Map position header string.
     */
    virtual std::string getMapPositionHeader() const=0;

    /**
     * @brief Obtains total resource biomass.
     * @param landscapeResourceBiomass Vector to store wet mass per resource.
     */
    virtual void obtainResourceBiomass(std::vector<WetMass> &landscapeResourceBiomass) const=0;
    
    /**
     * @brief Saves a snapshot of a resource species to a file.
     */
    virtual void saveResourceSpeciesSnapshot(std::ofstream &file, const ResourceSpecies* const &species) const=0;
    
    /**
     * @brief Saves a snapshot of water to a file.
     */
    virtual void saveWaterSnapshot(std::ofstream &file) const=0;

    /**
     * @brief Returns total number of active animals.
     */
    virtual const unsigned int& getTotalNumberOfActiveAnimals() const=0;

    /**
     * @brief Obtains the global temperature range across all terrain cells.
     */
    virtual void obtainGlobalTemperatureRange(std::unordered_set<Temperature>& globalTemperatureRange) const=0;

    /**
     * @brief Returns the type of map.
     * @return Map::Type
     */
    virtual Type getType() const=0;

    /**
    * @brief Serializes the Map object.
    * @tparam Archive Archive type.
    * @param ar Archive instance.
    * @param version Serialization version.
    */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};



#endif /* MAP_H_ */