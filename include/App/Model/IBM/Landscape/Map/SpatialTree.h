/**
 * @file SpatialTree.h
 * @brief Defines the SpatialTree class, a spatially subdivided map implementation.
 *
 * This file declares the SpatialTree class, which implements a hierarchical spatial
 * tree structure (quadtree/octree-like) for managing terrain cells, animal positions,
 * and resources in a Landscape. It supports efficient animal searches, patch application,
 * and population generation.
 */

#ifndef SPATIAL_TREE_H_
#define SPATIAL_TREE_H_


#include <vector>
#include <nlohmann/json.hpp>
#include <magic_enum/magic_enum.hpp>
#include <cmath>
#include <string>


#include <boost/serialization/export.hpp>



#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"
#include "App/Model/IBM/Landscape/Map/Points/PointMap.h"
#include "App/Model/IBM/Landscape/Map/Map.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/Points/PointSpatialTree.h"
#include "App/Model/IBM/Landscape/Map/Points/PointContinuous.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCell.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/RootTerrainCell.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/LivingBeings/Animals/SpatialTreeAnimal.h"
#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalStatistical.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"




/**
 * @class SpatialTree
 * @brief SpatialTree map implementation for hierarchical spatial organization.
 *
 * The SpatialTree class extends Map and provides a recursive spatial partitioning
 * of the landscape. It allows fast access to terrain cells, manages active animals,
 * and supports population initialization and resource registration.
 */
class SpatialTree : public Map
{
protected:
    unsigned int mapDepth; ///< Depth of the spatial tree
    std::vector<PreciseDouble> cellSizes; ///< Sizes of cells at each depth
    std::vector<unsigned int> axisSizes; ///< Number of cells along each axis
    unsigned int totalNumberOfActiveAnimals; ///< Total active animals in the tree
    RootTerrainCell* rootTerrainCell; ///< Pointer to the root cell of the tree

    /**
     * @brief Obtains iterators to leaf and branch inhabitable terrain cells.
     */
    void obtainLeafAndBranchInhabitableTerrainCells(std::vector<std::vector<std::vector<TerrainCell*>::iterator>>& leafInhabitableTerrainCells, std::vector<std::vector<std::vector<TerrainCell*>::iterator>>& branchInhabitableTerrainCells);
    
    /**
     * @brief Calculates the depth of the spatial tree.
     * @return Depth of the tree.
     */
    unsigned int calculateMapDepth() const;
    
    /**
     * @brief Calculates cell sizes for each depth level.
     * @return Vector of cell sizes.
     */
    std::vector<PreciseDouble> calculateCellSizes() const;
    
    /**
     * @brief Calculates the number of cells per axis for each depth.
     * @return Vector of axis sizes.
     */
    std::vector<unsigned int> calculateAxisSizes() const;
    
    /**
     * @brief Computes the logarithmic map of a value.
     * @param value Value to map.
     * @return Logarithmically scaled value.
     */
    PreciseDouble logMap(const PreciseDouble& value) const;
    
    /**
     * @brief Calculates the center position of a given spatial tree cell.
     * @param cell Cell coordinates.
     * @param size Size of the cell.
     * @return Center position as PointContinuous.
     */
    PointContinuous calculateCenterPosition(const PointSpatialTree &cell, const PreciseDouble& size) const;

public:
    static constexpr const size_t numbreOfSubdivisions = 2; ///< Number of subdivisions per axis

    /**
     * @brief Default constructor.
     */
    SpatialTree();

    /**
     * @brief Constructor using JSON map and spatial tree configuration.
     * @param mapConfig JSON configuration for the map.
     * @param spatialTreeConfig JSON configuration for the spatial tree.
     * @param moistureBaseSource Base moisture source for terrain cells.
     */
    SpatialTree(const nlohmann::json &mapConfig, const nlohmann::json &spatialTreeConfig, MoistureSource* const moistureBaseSource);
    
    /**
     * @brief Destructor.
     */
    virtual ~SpatialTree();

    SpatialTree(const SpatialTree&) = delete; ///< Deleted copy constructor
    SpatialTree& operator=(const SpatialTree&) = delete; ///< Deleted assignment operator

    /**
     * @brief Increments the total number of active animals in the tree.
     */
    void increaseTotalNumberOfActiveAnimals(); 

    /**
     * @brief Decrements the total number of active animals in the tree.
     */
    void decreaseTotalNumberOfActiveAnimals(); 

    /**
     * @brief Applies a patch to the landscape within the spatial tree.
     * @param landscape Pointer to the Landscape object.
     * @param patch Reference to the Patch object to apply.
     * @return True if the patch was applied successfully, false otherwise.
     */
    bool applyPatch(Landscape* const landscape, const Patch& patch);

    /**
     * @brief Returns the depth of the spatial tree.
     * @return Depth of the tree as unsigned int.
     */
    unsigned int getMapDepth() const;

    /**
     * @brief Returns the size of the cells at a specific depth.
     * @param depth Depth level.
     * @return Cell size at the given depth.
     */
    const PreciseDouble& getCellSize(const unsigned int depth) const;

    /**
     * @brief Returns the number of cells along each axis at a specific depth.
     * @param depth Depth level.
     * @return Axis size at the given depth.
     */
    const unsigned int& getAxisSize(const unsigned int depth) const;

    /**
     * @brief Inserts a new animal into the spatial tree.
     * @param landscape Pointer to the Landscape object.
     * @param newAnimal Pointer to the Animal object to insert.
     */
    void insertAnimal(Landscape* const landscape, Animal* const newAnimal);
    
    /**
     * @brief Updates all terrain cells and animals in the spatial tree.
     * @param numberOfTimeSteps Current timestep to update.
     */
    void update(const TimeStep& numberOfTimeSteps);

    /**
     * @brief Maps a continuous point to the spatial tree at a given depth.
     * @param point Continuous point coordinates.
     * @param depth Depth level for mapping.
     * @return PointMap representing the discrete cell coordinates.
     */
    PointMap obtainPointMap(const PointContinuous &point, const unsigned int depth) const;
    
    /**
     * @brief Returns the total number of active animals in the tree.
     * @return Total active animals as unsigned int.
     */
    const unsigned int& getTotalNumberOfActiveAnimals() const;

    /**
     * @brief Registers resources and non-statistical animals in the tree.
     * @param landscapeResources 3D vector of CellResource pointers for the landscape.
     * @param landscapeAnimals Vector of AnimalNonStatistical pointers.
     */
    void registerEdibles(std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, std::vector<AnimalNonStatistical*>& landscapeAnimals);

    /**
     * @brief Adds a new animal species to the tree.
     * @param animalSpecies Reference to the AnimalSpecies object.
     * @param timeStepsPerDay Number of timesteps per day for this species.
     */
    void addAnimalSpecies(const AnimalSpecies& animalSpecies, const PreciseDouble& timeStepsPerDay);
    
    /**
     * @brief Adds a new resource species to the tree.
     * @param landscape Pointer to the Landscape object.
     * @param landscapeResources 3D vector of CellResource pointers for the landscape.
     * @param resourceSpecies Reference to the ResourceSpecies object.
     * @param resourceBaseSource Pointer to the base resource source.
     * @param timeStepsPerDay Number of timesteps per day.
     */
    void addResourceSpecies(Landscape* const landscape, std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Deserializes moisture and resource sources.
     * @param appliedMoisture Vector of applied moisture sources.
     * @param appliedResource 2D vector of applied resource sources.
     */
    void deserializeSources(
        std::vector<std::pair<size_t, MoistureSource*>>& appliedMoisture,
        std::vector<std::vector<std::pair<size_t, ResourceSource*>>>& appliedResource
    );

    /**
     * @brief Deserializes existing animal and resource species.
     * @param existingResourceSpecies Vector of existing ResourceSpecies pointers.
     * @param existingAnimalSpecies Vector of existing AnimalSpecies pointers.
     */
    void deserializeSpecies(std::vector<ResourceSpecies*>& existingResourceSpecies, std::vector<AnimalSpecies*>& existingAnimalSpecies);

    /**
     * @brief Recursively generates spatial points for the tree (overload 1).
     */
    void generatePointsRecursively(std::vector<PointSpatialTree>& points, const unsigned int sideLength, std::vector<unsigned int> &currentCoords, unsigned char axis, const PointSpatialTree& centralCoords, const std::vector<int> &initialCoords) const;
    
    /**
     * @brief Recursively generates spatial points for the tree (overload 2).
     */
    void generatePointsRecursively(std::vector<PointSpatialTree>& points, const unsigned int sideLength, std::vector<unsigned int> &currentCoords, unsigned char axis, unsigned int depth, const std::vector<int> &initialCoords) const;
    
    /**
     * @brief Generates spatial points (overload 1).
     */
    void generatePoints(std::vector<PointSpatialTree>& points, const unsigned int sideLength, const PointSpatialTree& centralCoords, const std::vector<int> &initialCoords) const;
    
    /**
     * @brief Generates spatial points (overload 2).
     */
    void generatePoints(std::vector<PointSpatialTree>& points, const unsigned int sideLength, const std::vector<int> &initialCoords, unsigned int depth) const;
    
    /**
     * @brief Populates all inhabitable terrain cells.
     */
    void obtainInhabitableTerrainCells();
    
    /**
     * @brief Obtains mapping of species to inhabitable terrain cells.
     */
    void obtainSpeciesInhabitableTerrainCells(std::vector<CustomIndexedVector<Instar, std::vector<std::vector<TerrainCell*>::iterator>>>& mapSpeciesInhabitableTerrainCells, const std::vector<AnimalSpecies*>& existingAnimalSpecies, const CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, std::vector<ResourceSpecies::ResourceID>>>& involvedResourceSpecies);
    
    /**
     * @brief Generates statistics population for visualization or analysis.
     */
    unsigned int generateStatisticsPopulation(std::vector<CustomIndexedVector<Instar, std::vector<AnimalStatistical*>>>& animalsPopulation, View* view, Landscape* const landscape, std::vector<AnimalSpecies*>& existingAnimalSpecies, std::vector<CustomIndexedVector<Instar, std::vector<std::vector<TerrainCell*>::iterator>>> &mapSpeciesInhabitableTerrainCells, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);
    
    /**
     * @brief Generates actual animal population in the landscape.
     */
    size_t generatePopulation(View* view, Landscape* const landscape, AnimalSpecies* currentAnimalSpecies, const CustomIndexedVector<Instar, unsigned int>& population, const CustomIndexedVector<Instar, std::vector<std::vector<TerrainCell*>::iterator>> &speciesInhabitableTerrainCells, const bool saveAnimalConstitutiveTraits, std::ostringstream& animalConstitutiveTraitsFile, const bool saveGenetics, const bool saveMassInfo, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);
    
    /**
     * @brief Returns the header string for map position outputs.
     * @return Map position header string.
     */
    std::string getMapPositionHeader() const;

    /**
     * @brief Obtains the total resource biomass for the landscape.
     */
    void obtainResourceBiomass(std::vector<WetMass> &landscapeResourceBiomass) const;
    
    /**
     * @brief Saves a snapshot of a resource species to file.
     */
    void saveResourceSpeciesSnapshot(std::ofstream &file, const ResourceSpecies* const &species) const;
    
    /**
     * @brief Saves water snapshot to file.
     */
    void saveWaterSnapshot(std::ofstream &file) const;

    /**
     * @brief Computes the global temperature range across all terrain cells.
     * @param globalTemperatureRange Set to store temperatures.
     */
    void obtainGlobalTemperatureRange(std::unordered_set<Temperature>& globalTemperatureRange) const;

    /**
     * @brief Returns the map type.
     * @return Map::Type
     */
    Type getType() const;

    /**
     * @brief Serializes the SpatialTree object.
     * @tparam Archive Archive type.
     * @param ar Serialization archive.
     * @param version Serialization version.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* SPATIAL_TREE_H_ */