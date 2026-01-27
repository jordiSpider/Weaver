/**
 * @file TerrainCell.h
 * @brief Defines the TerrainCell class representing a cell in the landscape.
 *
 * TerrainCell manages animals and resources within a discrete spatial cell of the landscape.
 * It provides methods for inserting animals, applying functions to animals and resources,
 * managing patches, and querying neighboring cells.
 */

#ifndef TERRAINCELL_H_
#define TERRAINCELL_H_


#include <fstream>
#include <vector>
#include <memory>
#include <functional>
#include <utility>
#include <boost/geometry/arithmetic/arithmetic.hpp>


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
#include <map>

#include "App/Model/IBM/Landscape/Map/Points/PointMap.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Animal.h"
#include "Misc/Types.h"
#include "App/Model/IBM/Maths/MathFunctions.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpecies.h"

#include "App/Model/IBM/Landscape/Map/Geometry/Geometry.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/IndividualLevelTrait.h"
#include "Misc/Utilities.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/AnimalSearchParams.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/ResourceSearchParams.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Gender.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"
#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"
#include "App/Model/IBM/Maths/Random.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/PatchApplicator.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/CellValue.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResource.h"
#include "App/Model/IBM/Maths/PreciseDouble.h"



/**
 * @typedef ConstAnimalVector
 * @brief Vector of constant pointers to Animal.
 */
typedef std::vector<const Animal*> ConstAnimalVector;

/**
 * @typedef AnimalVector
 * @brief Vector of pointers to Animal.
 */
typedef std::vector<Animal*> AnimalVector;

/**
 * @typedef GenderVector
 * @brief Indexed vector of animals grouped by gender.
 */
typedef CustomIndexedVector<Gender, AnimalVector> GenderVector;

/**
 * @typedef InstarVector
 * @brief Indexed vector of animals grouped by instar and gender.
 */
typedef CustomIndexedVector<Instar, GenderVector> InstarVector;

/**
 * @typedef AnimalSpeciesVector
 * @brief Indexed vector of animals grouped by species, instar, and gender.
 */
typedef CustomIndexedVector<AnimalSpeciesID, InstarVector> AnimalSpeciesVector;

/**
 * @typedef LifeStageVector
 * @brief Indexed vector of animals grouped by life stage, species, instar, and gender.
 */
typedef CustomIndexedVector<LifeStage, AnimalSpeciesVector> LifeStageVector;

/**
 * @typedef PartialCoverageAnimals
 * @brief Partial coverage structure for animal search.
 */
typedef std::vector<std::unique_ptr<AnimalVector>> PartialCoverageAnimals;

/**
 * @typedef FullCoverageAnimals
 * @brief Full coverage structure for animal search.
 */
typedef std::vector<std::unique_ptr<std::vector<AnimalVector*>>> FullCoverageAnimals;

/**
 * @typedef SearchableAnimals
 * @brief Pair of partial and full coverage animal vectors.
 */
typedef std::pair<std::unique_ptr<PartialCoverageAnimals>, std::unique_ptr<FullCoverageAnimals>> SearchableAnimals;

/**
 * @typedef ResourcesOnRadius
 * @brief Vector storing resource quantities within a search radius.
 */
typedef std::vector<DryMass> ResourcesOnRadius;

/**
 * @typedef SearchableResources
 * @brief Map of resources with their biomass values for searching.
 */
typedef std::unordered_map<CellResourceInterface*, DryMass> SearchableResources;

/**
 * @typedef EdiblesOnRadius
 * @brief Pair of resources and animals within a radius.
 */
typedef std::pair<std::unique_ptr<ResourcesOnRadius>, SearchableAnimals> EdiblesOnRadius;

/**
 * @typedef SearchableEdibles
 * @brief Pair of searchable resources and searchable animals.
 */
typedef std::pair<std::unique_ptr<SearchableResources>, SearchableAnimals> SearchableEdibles;

/**
 * @typedef PreviousAnimalFunctions
 * @brief Functions to apply to animals before main processing.
 */
using PreviousAnimalFunctions = std::vector<std::function<void(AnimalVector&)>>;

/**
 * @typedef IndividualFunctions
 * @brief Functions to apply to individual animals.
 */
using IndividualFunctions = std::vector<std::function<void(Animal&)>>;

/**
 * @typedef PostAnimalFunctions
 * @brief Functions to apply to animals after main processing.
 */
using PostAnimalFunctions = std::vector<std::function<void(AnimalVector&)>>;

/**
 * @typedef AnimalFunctions
 * @brief Tuple of previous, individual, and post animal functions.
 */
using AnimalFunctions = std::tuple<PreviousAnimalFunctions, IndividualFunctions, PostAnimalFunctions>;

/**
 * @typedef ResourceFunctions
 * @brief Vector of functions to apply to resources.
 */
using ResourceFunctions = std::vector<std::function<void(CellResourceInterface&, bool, const PointContinuous* const, const PreciseDouble&, const RingModel* const)>>;


/**
 * @class TerrainCell
 * @brief Represents a single cell in the landscape.
 *
 * TerrainCell stores animals and resources within a discrete spatial cell.
 * It provides methods to insert, remove, or update animals and resources,
 * apply functions to them, and manage patches and search operations.
 */
class TerrainCell
{
protected:
    PatchApplicator* patchApplicator;  ///< Pointer to the patch applicator.
    PointMap* position;                ///< Position of the cell in the map.
    PreciseDouble size;                ///< Size of the cell.
    PreciseDouble hyperVolume;         ///< Hypervolume of the cell.
    PointContinuous* center;           ///< Continuous center position.
    const RingModel* effectiveArea;    ///< Effective area for calculations.

    #ifdef DEBUG
        TimeStep updateLastTimeStep;   ///< Last update time step (debug).
        void testUpdate(const TimeStep actualTimeStep); ///< Debug helper.
    #endif

    /**
     * @brief Registers animals present in this cell to the landscape.
     * @param landscapeAnimals Vector of pointers to animals in the landscape.
     */
    void registerAnimals(std::vector<AnimalNonStatistical*>& landscapeAnimals);

    /**
     * @brief Creates a new animal instance in this terrain cell.
     * @param landscape Pointer to the landscape.
     * @param instar Instar of the animal.
     * @param animalSpecies Pointer to the species.
     * @param saveGenetics Whether to save genetics information.
     * @param saveMassInfo Whether to save mass information.
     * @param actualTimeStep Current simulation time step.
     * @param timeStepsPerDay Number of simulation steps per day.
     * @return Created animal.
     */
    virtual AnimalNonStatistical* createAnimal(Landscape* const landscape, const Instar &instar, AnimalSpecies* animalSpecies, const Genome* const genome, const bool saveGenetics, const bool saveMassInfo, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)=0;
    
    /**
     * @brief Generates the continuous center point of the cell.
     * @return Pointer to the center point.
     */
    PointContinuous* makeCenter() const;

    /**
     * @brief Sets the patch applicator for this cell.
     * @param newPatchApplicator Pointer to the new patch applicator.
     */
    void setPatchApplicator(PatchApplicator* newPatchApplicator);

    LifeStageVector* animals; ///< Stores all the animals in this cell.

public:
    /**
     * @brief Default constructor.
     */
    TerrainCell();

    /**
     * @brief Constructor with position and size.
     * @param position Position in the spatial map
     * @param size Cell size
     */
    TerrainCell(PointMap* const &position, const PreciseDouble &size);

    /**
     * @brief Destructor.
     */
    virtual ~TerrainCell();

    /**
     * @brief Returns the position of the cell.
     * @return Reference to the cell's position
     */
    virtual const PointMap& getPosition() const;

    /**
     * @brief Returns a mutable reference to the cell's position.
     * @return Mutable reference to position
     */
    virtual PointMap& getMutablePosition() const;

    /**
     * @brief Returns the size of the cell.
     * @return Cell size
     */
    const PreciseDouble& getSize() const;

    /**
     * @brief Returns the hypervolume of the cell.
     * @return Hypervolume
     */
    const PreciseDouble& getHyperVolume() const;

    /**
     * @brief Returns all animals in the cell.
     * @return Reference to the animals vector
     */
    const LifeStageVector& getAnimals() const;

    /**
     * @brief Returns a mutable reference to all animals in the cell.
     * @return Mutable reference to the animals vector
     */
    LifeStageVector& getMutableAnimals() const;
    
    /**
     * @brief Returns the effective area polygon of the cell.
     * @return Reference to effective area
     */
    const RingModel& getEffectiveArea() const;

    /**
     * @brief Generates the effective area polygon for the cell.
     * @return Pointer to effective area polygon
     */
    const RingModel* makeEffectiveArea() const;

    /**
     * @brief Changes an animal to senesced status.
     * @param landscape Pointer to the landscape
     * @param targetAnimal Animal to change
     * @param numberOfTimeSteps Number of time steps for aging
     */
    void changeAnimalToSenesced(Landscape* const landscape, Animal* targetAnimal, const TimeStep numberOfTimeSteps);

    /**
     * @brief Calculates Manhattan distance from this cell to a given point.
     * @param point Target point
     * @return Distance in cells
     */
    size_t calculateManhattanDistanceToPoint(const PointMap &point) const;

    /**
     * @brief Adds a specific animal species to the terrain cell.
     * 
     * Initializes internal structures for storing animals of this species,
     * considering life stages and search parameters.
     * 
     * @param animalSpecies The species to add
     * @param allAnimalsSearchParams Search parameters for which life stages, instars, and genders are relevant
     * @param timeStepsPerDay Simulation time steps per day
     */
    virtual void addAnimalSpecies(const AnimalSpecies& animalSpecies, const AnimalSearchParams& allAnimalsSearchParams, const PreciseDouble& timeStepsPerDay);
    
    /**
     * @brief Adds a resource species to the terrain cell.
     * 
     * Initializes the resources and associates them with this cell.
     * 
     * @param landscape Pointer to the landscape
     * @param landscapeResources 3D vector containing all landscape resources
     * @param resourceSpecies Resource species to add
     * @param resourceBaseSource Pointer to the source of the resource
     * @param allAnimalsSearchParams Search parameters for animals (needed for interactions)
     * @param timeStepsPerDay Simulation time steps per day
     */
    virtual void addResourceSpecies(Landscape* const landscape, std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource, const AnimalSearchParams& allAnimalsSearchParams, const PreciseDouble& timeStepsPerDay);

    /**
     * @brief Deserializes and restores sources applied to this terrain cell.
     * 
     * @param appliedMoisture Vector of moisture sources applied
     * @param appliedResource Vector of resource sources applied
     */
    virtual void deserializeSources(
        std::vector<std::pair<size_t, MoistureSource*>>& appliedMoisture,
        std::vector<std::vector<std::pair<size_t, ResourceSource*>>>& appliedResource
    );

    /**
     * @brief Deserializes and restores species previously applied to the cell.
     * 
     * @param existingResourceSpecies Vector of all existing resource species
     * @param existingAnimalSpecies Vector of all existing animal species
     */
    virtual void deserializeSpecies(std::vector<ResourceSpecies*>& existingResourceSpecies, std::vector<AnimalSpecies*>& existingAnimalSpecies);

    /**
     * @brief Registers edible resources in the terrain cell.
     * 
     * Must be implemented by derived classes depending on coverage type.
     * 
     * @param landscapeResources 3D vector of landscape resources
     * @param landscapeAnimals Vector of non-statistical animals
     */
    virtual void registerEdibles(std::vector<std::vector<std::vector<CellResource*>>>& landscapeResources, std::vector<AnimalNonStatistical*>& landscapeAnimals)=0;

    /**
     * @brief Migrates an animal from this cell to a new terrain cell.
     * 
     * @param landscape Pointer to the landscape
     * @param animalToMigrate Pointer to the animal to migrate
     * @param newTerrainCell Pointer to the destination terrain cell
     * @param newPosition Continuous position of the animal in the new cell
     */
    virtual void migrateAnimalTo(Landscape* const landscape, Animal* animalToMigrate, TerrainCell* newTerrainCell, const PointContinuous &newPosition)=0;

    /**
     * @brief Returns the continuous center point of the terrain cell.
     * 
     * @return Reference to the center point
     */
    const PointContinuous& getCenter() const;

    /**
     * @brief Applies a series of functions to all resources in the cell.
     * 
     * @param resourceFunctions Vector of pairs of resource search parameters and functions to apply
     */
    void applyFunctionToResources(const std::vector<std::pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions);

    /**
     * @brief Applies a series of functions to resources within a specific radius.
     * 
     * @param sourcePosition Center of the radius
     * @param radius Radius of the area to apply the functions
     * @param radiusArea Optional precomputed polygon representing the area
     * @param resourceFunctions Vector of resource search parameters and functions to apply
     */
    void applyFunctionToResources(
        const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const std::vector<std::pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    );

    /**
     * @brief Applies functions to animals in the cell matching search parameters.
     * 
     * @param animalFunctions Vector of pairs of animal search parameters and functions to apply
     */
    virtual void applyFunctionToAnimals(const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions);

    /**
     * @brief Applies functions to animals that satisfy a custom checker.
     * 
     * @param checker Function to determine which animals to process
     * @param animalFunctions Vector of animal search parameters and functions to apply
     */
    void applyFunctionToAnimals(
        std::function<bool(Animal&)> checker, 
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    );

    /**
     * @brief Randomly applies functions to animals in the cell matching search parameters.
     * 
     * @param animalFunctions Vector of pairs of animal search parameters and functions to apply
     */
    virtual void randomApplyFunctionToAnimals(const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions);

    /**
     * @brief Randomly applies functions to animals that satisfy a custom checker.
     * 
     * @param checker Function to determine which animals to process
     * @param animalFunctions Vector of animal search parameters and functions to apply
     */
    void randomApplyFunctionToAnimals(
        std::function<bool(Animal&)> checker, 
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    );

    /**
     * @brief Applies functions to both animals and edible resources in the cell.
     * 
     * @param animalFunctions Vector of pairs of animal search parameters and functions
     * @param resourceFunctions Vector of pairs of resource search parameters and functions
     */
    virtual void applyFunctionToEdibles(
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const std::vector<std::pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    );

    /**
     * @brief Applies functions to animals satisfying a checker and resources within a specific area.
     * 
     * @param checker Function to filter animals
     * @param sourcePosition Center of the area
     * @param radius Radius of the area
     * @param radiusArea Optional precomputed polygon
     * @param animalFunctions Vector of animal search parameters and functions
     * @param resourceFunctions Vector of resource search parameters and functions
     */
    virtual void applyFunctionToEdibles(
        std::function<bool(Animal&)> checker, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const std::vector<std::pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    );

    /**
     * @brief Applies functions to animals and resources within this cell.
     * 
     * @param fullCoverage Whether the cell is fully covered
     * @param sourcePosition Center of the area
     * @param radius Radius to consider
     * @param radiusArea Optional precomputed polygon
     * @param animalFunctions Vector of animal search parameters and functions
     * @param resourceFunctions Vector of resource search parameters and functions
     */
    void applyFunctionToEdiblesInCell(bool fullCoverage, 
        const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const std::vector<std::pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    );

    /**
     * @brief Applies functions to animals and resources within a circular radius.
     * 
     * @param sourcePosition Center point
     * @param radius Radius to consider
     * @param animalFunctions Vector of animal search parameters and functions
     * @param resourceFunctions Vector of resource search parameters and functions
     */
    void applyFunctionToEdiblesInRadius(
        const PointContinuous &sourcePosition, const PreciseDouble &radius, 
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const std::vector<std::pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    );

    /**
     * @brief Applies functions to animals and resources within a circular radius with optional polygon area.
     * 
     * @param sourcePosition Center point
     * @param radius Radius to consider
     * @param radiusArea Optional precomputed polygon
     * @param animalFunctions Vector of animal search parameters and functions
     * @param resourceFunctions Vector of resource search parameters and functions
     */
    virtual void applyFunctionToEdiblesInRadius(
        const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const std::vector<std::pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const std::vector<std::pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    )=0;

    /**
     * @brief Finds a cell and position in a given bearing direction from this cell.
     * 
     * @param map Pointer to the map
     * @param targetNeighborToTravelTo Target cell and position to reach
     * @param animalPosition Current position of the animal
     * @param remainingSearchArea Remaining search radius
     * @return Pair indicating if a cell was found and the resulting cell and position
     */
    virtual std::pair<bool, std::pair<TerrainCell*, PointContinuous>> getCellByBearing(
        Map* const map, const std::pair<PointMap, PointContinuous> &targetNeighborToTravelTo, const PointContinuous &animalPosition, const PreciseDouble& remainingSearchArea
    )=0;

    /**
     * @brief Returns a pointer to a cell at the specified map position.
     * 
     * @param dstPos Destination cell position
     * @return Pointer to the TerrainCell at that position
     */
    virtual TerrainCell* getCell(const PointMap &dstPos)=0;

    /**
     * @brief Inserts an animal into this cell.
     * 
     * @param landscape Pointer to the landscape
     * @param newAnimal Pointer to the new animal
     */
    virtual void insertAnimal(Landscape* const landscape, Animal* const newAnimal);
    
    /**
     * @brief Inserts a new animal randomly into the cell.
     * 
     * @param landscape Pointer to the landscape
     * @param instar Instar of the animal
     * @param animalSpecies Species of the animal
     * @param isStatistical Whether the animal is statistical
     * @param saveGenetics Whether to save genetics
     * @param saveMassInfo Whether to save mass information
     * @param actualTimeStep Current time step
     * @param timeStepsPerDay Simulation time steps per day
     * @return Tuple containing success, old cell, new cell, pointer to animal, and index
     */
    virtual std::tuple<bool, TerrainCell*, TerrainCell*, Animal*> randomInsertAnimal(Landscape* const landscape, const Instar &instar, AnimalSpecies* animalSpecies, const bool isStatistical, const Genome* const genome, const bool saveGenetics, const bool saveMassInfo, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay);
    
    /**
     * @brief Adds an animal to internal storage structures.
     * 
     * @param lifeStage Life stage of the animal
     * @param animalSpeciesId Species ID
     * @param instar Instar of the animal
     * @param gender Gender of the animal
     * @param newAnimal Pointer to the animal
     */
    void addAnimal(const LifeStage &lifeStage, const size_t animalSpeciesId, const Instar &instar, const Gender &gender, Animal* const newAnimal);
    
    /**
     * @brief Removes a specific animal from the cell.
     * 
     * @param animalToRemove Pointer to the animal to remove
     */
    void eraseAnimal(Animal* const animalToRemove);
    
    /**
     * @brief Removes a specific animal from the cell with full specification.
     * 
     * @param lifeStage Life stage of the animal
     * @param animalSpeciesId Species ID
     * @param instar Instar
     * @param gender Gender
     * @param animalToRemove Pointer to the animal
     */
    void eraseAnimal(const LifeStage &lifeStage, const size_t animalSpeciesId, const Instar &instar, const Gender &gender, Animal* const animalToRemove);
    
    /**
     * @brief Removes all animals from the cell.
     */
    void eraseAllAnimals();

    /**
     * @brief Populates the cell with all inhabitable neighboring terrain cells.
     * 
     * @param map Pointer to the map
     */
    virtual void obtainInhabitableTerrainCells(Map* const map);

    /**
     * @brief Updates the state of the terrain cell for a given number of time steps.
     * 
     * @param numberOfTimeSteps Number of time steps to update
     */
    virtual void update(const TimeStep& numberOfTimeSteps);

    /**
     * @brief Gets the neighboring cells within a radius for a specific animal evaluation.
     * 
     * @param bestEvaluations Output vector with best evaluated cells
     * @param sourcePosition Center of search
     * @param radius Radius to consider
     * @param searchDepth Maximum search depth
     * @param searchNeighborsWithFemales Whether to prioritize neighbors with females
     * @param animalWhoIsEvaluating Pointer to the evaluating animal
     * @param animalsHasTriedToPredate List of animals that have already attempted predation
     */
    virtual void getNeighboursCellsOnRadius(
        std::vector<CellValue>& bestEvaluations, const PointContinuous &sourcePosition, const PreciseDouble &radius, const size_t searchDepth, const bool searchNeighborsWithFemales, AnimalNonStatistical* animalWhoIsEvaluating, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate
    )=0;

    /**
     * @brief Returns a mutable reference to the patch applicator of the cell.
     * 
     * @return Reference to PatchApplicator
     */
    virtual PatchApplicator& getMutablePatchApplicator();

    /**
     * @brief Returns a const reference to the patch applicator of the cell.
     * 
     * @return Const reference to PatchApplicator
     */
    virtual const PatchApplicator& getPatchApplicator() const;

    /**
     * @brief Saves a snapshot of resource species to a file.
     * 
     * @param file Output file stream
     * @param species Pointer to the resource species
     */
    virtual void saveResourceSpeciesSnapshot(std::ofstream &file, const ResourceSpecies* const &species) const;
    
    /**
     * @brief Saves a snapshot of water (moisture) to a file.
     * 
     * @param file Output file stream
     */
    virtual void saveWaterSnapshot(std::ofstream &file) const;
    
    /**
     * @brief Returns the number of mature females of a given species in the cell.
     * 
     * @param animalSpecies Pointer to the animal species
     * @return Number of mature females
     */
    unsigned int getNumberOfMatureFemales(AnimalSpecies *const animalSpecies);
    
    /**
     * @brief Performs a detailed evaluation of the cell for a given animal.
     * 
     * @param bestEvaluations Vector to store evaluated values
     * @param animalWhoIsEvaluating Pointer to the animal performing the evaluation
     * @param sourcePosition Center of evaluation
     * @param radius Radius of evaluation
     * @param radiusArea Optional precomputed area
     * @param searchNeighborsWithFemales Whether to prioritize neighbors with females
     * @param parentFullCoverage Whether parent area has full coverage
     * @param animalsHasTriedToPredate List of animals that already attempted predation
     */
    void getCellEvaluation(std::vector<CellValue>& bestEvaluations, AnimalNonStatistical* animalWhoIsEvaluating, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const bool searchNeighborsWithFemales, const bool parentFullCoverage, const std::list<const AnimalNonStatistical*> &animalsHasTriedToPredate);

    /**
     * @brief Serialization function for TerrainCell.
     * 
     * @tparam Archive Type of archive
     * @param ar Archive object
     * @param version Serialization version
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* TERRAINCELL_H_ */
