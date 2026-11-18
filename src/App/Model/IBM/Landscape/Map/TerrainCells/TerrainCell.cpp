
#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"

#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/IBM/Landscape/Map/TerrainCells/EdibleSearchParams.h"
#include "App/Model/IBM/Landscape/Map/Map.h"

using namespace std;
namespace fs = std::filesystem;



TerrainCell::TerrainCell()
{

}

TerrainCell::TerrainCell(PointMap* const &position, const PreciseDouble &size)
    : position(position), size(size), hyperVolume(pow(size, DIMENSIONS)), center(makeCenter()), effectiveArea(makeEffectiveArea())
{
    #ifdef DEBUG
        updateLastTimeStep = TimeStep(UINT_MAX);
    #endif
}


TerrainCell::~TerrainCell()
{
    delete patchApplicator;

    delete position;

    eraseAllAnimals();

    delete center;
}

void TerrainCell::addAnimalSpecies(const AnimalSpecies& animalSpecies)
{
    getMutablePatchApplicator().addAnimalSpecies(animalSpecies);
}

void TerrainCell::addResourceSpecies(Landscape* const landscape, vector<vector<vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource)
{
    getMutablePatchApplicator().addResourceSpecies(landscape, landscapeResources, resourceSpecies, resourceBaseSource);
}


PatchApplicator& TerrainCell::getMutablePatchApplicator()
{
    return *patchApplicator;
}

const PatchApplicator& TerrainCell::getPatchApplicator() const
{
    return *patchApplicator;
}


void TerrainCell::setPatchApplicator(PatchApplicator* newPatchApplicator)
{
    patchApplicator = newPatchApplicator;
}



const PointMap& TerrainCell::getPosition() const
{
    return *position;
}

PointMap& TerrainCell::getMutablePosition() const
{
    return *position;
}


const PreciseDouble& TerrainCell::getSize() const
{
    return size;
}

const PreciseDouble& TerrainCell::getHyperVolume() const
{
    return hyperVolume;
}

const std::vector<Animal*>& TerrainCell::getAnimals() const
{
    return animals;
}

std::vector<Animal*>& TerrainCell::getMutableAnimals()
{
    return animals;
}

bool TerrainCell::isAnimalInside(const PointContinuous& animalPos) const
{
    return Geometry::pointInsideBox(animalPos, getEffectiveArea());
}




const RingModel& TerrainCell::getEffectiveArea() const
{
	return effectiveArea;
}














void TerrainCell::insertAnimal(Landscape* const, Animal* const newAnimal)
{
    addAnimal(newAnimal);
}


tuple<bool, TerrainCell*, TerrainCell*, Animal*> TerrainCell::randomInsertAnimal(Landscape* const landscape, const Instar &instar, AnimalSpecies* animalSpecies, const bool isStatistical, const Genome* const genome, const bool saveGenetics, std::vector<fmt::memory_buffer>& geneticsText, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
    const optional<PointContinuous> position = generateAnimalPosition(animalSpecies, instar, *landscape->getMap());
    if (!position)
    {
        return make_tuple(false, nullptr, nullptr, nullptr);
    }

    Animal* newAnimal;
    
    if(isStatistical)
    {
        newAnimal = new AnimalStatistical(landscape, instar, animalSpecies, this, timeStepsPerDay);
    }
    else
    {
        newAnimal = createAnimal(landscape, instar, animalSpecies, genome, saveGenetics, geneticsText, actualTimeStep, timeStepsPerDay);
    }
    

    newAnimal->setPosition(*position);
    TerrainCell::insertAnimal(landscape, newAnimal);

    return make_tuple(false, nullptr, nullptr, newAnimal);
}

optional<PointContinuous> TerrainCell::generateAnimalPosition(const AnimalSpecies* animalSpecies, const Instar& instar, const Map& map) const
{
    const PreciseDouble bodySection = animalSpecies->getPoreUsePerInstar(instar);
    const auto isValid = [&](const PointContinuous& position)
    {
        return map.isTraversable(position, bodySection);
    };

    const double cellSize = getSize().getValue();
    const auto makePosition = [&](const vector<double>& fractions)
    {
        PointContinuous position;
        for (unsigned char axis = 0; axis < DIMENSIONS; ++axis)
        {
            const Axis mapAxis = magic_enum::enum_cast<Axis>(axis).value();
            setPositionAxisValue(position, axis,
                static_cast<double>(getPosition().get(mapAxis)) * cellSize + fractions[axis] * cellSize);
        }
        return position;
    };

    constexpr size_t randomAttempts = 16u;
    for (size_t attempt = 0u; attempt < randomAttempts; ++attempt)
    {
        vector<double> fractions(DIMENSIONS);
        for (double& fraction : fractions)
        {
            fraction = Random::randomUniform(0.0, 1.0).getValue();
        }
        const PointContinuous position = makePosition(fractions);
        if (isValid(position))
        {
            return position;
        }
    }

    constexpr size_t gridResolution = 16u;
    size_t gridCandidateCount = 1u;
    for (unsigned char axis = 0; axis < DIMENSIONS; ++axis)
    {
        gridCandidateCount *= gridResolution;
    }
    vector<double> gridOffsets(DIMENSIONS);
    for (double& offset : gridOffsets)
    {
        offset = Random::randomUniform(0.0, 1.0).getValue();
    }
    const size_t gridStart = Random::randomIndex(gridCandidateCount);
    for (size_t offset = 0u; offset < gridCandidateCount; ++offset)
    {
        const size_t gridIndex = (gridStart + offset) % gridCandidateCount;
        size_t remainingIndex = gridIndex;
        vector<double> fractions(DIMENSIONS);
        for (unsigned char axis = 0; axis < DIMENSIONS; ++axis)
        {
            const size_t coordinate = remainingIndex % gridResolution;
            remainingIndex /= gridResolution;
            fractions[axis] = (static_cast<double>(coordinate) + gridOffsets[axis]) / gridResolution;
        }
        const PointContinuous position = makePosition(fractions);
        if (isValid(position))
        {
            return position;
        }
    }

    const PointContinuous& centerPosition = getCenter();
    if (isValid(centerPosition))
    {
        return centerPosition;
    }

    return nullopt;
}

const PointContinuous& TerrainCell::getCenter() const
{
    return *center;
}

void TerrainCell::registerCells(std::vector<TerrainCell*>& terrainCells, unsigned int numberOfCellsPerAxis)
{
    size_t index = 0;

    for(int i = DIMENSIONS - 1; i >= 0; i--)
    {
        index += static_cast<size_t>(pow(numberOfCellsPerAxis, i)) * getPosition().get(magic_enum::enum_cast<Axis>(static_cast<unsigned char>(i)).value());
    }

    terrainCells[index] = this;
}


PointContinuous* TerrainCell::makeCenter() const
{
    PointContinuous* newCenter = new PointContinuous();

    const PreciseDouble halfOfSize = getSize() / 2.0;

    for(unsigned char i = 0; i < DIMENSIONS; i++)
    {
        setPositionAxisValue(*newCenter, i, static_cast<double>(getPosition().get(magic_enum::enum_cast<Axis>(i).value())) * getSize() + halfOfSize);
    }

    return newCenter;
}

void TerrainCell::addAnimal(Animal* const newAnimal)
{
    std::lock_guard<std::mutex> lock(animalsMutex);

    animals.push_back(newAnimal);
    newAnimal->setTerrainCell(this);
}

void TerrainCell::eraseAnimal(Animal* const animalToRemove)
{
    std::lock_guard<std::mutex> lock(animalsMutex);

    animals.erase(std::remove(animals.begin(), animals.end(), animalToRemove), animals.end());
}

void TerrainCell::changeAnimalToSenesced(Landscape* const landscape, Animal* targetAnimal, const TimeStep numberOfTimeSteps)
{
	Animal* targetAnimalCast = static_cast<Animal*>(targetAnimal);

	targetAnimalCast->setNewLifeStage(landscape, LifeStage::SENESCED, numberOfTimeSteps, landscape->getTimeStepsPerDay());
}

void TerrainCell::eraseAllAnimals()
{
    for(Animal* animal : animals)
    {
        delete animal;
    }
    animals.clear();
}


RingModel TerrainCell::makeEffectiveArea() const
{
    RingModel ringEffectiveArea;
	boost::geometry::convert(Geometry::makeBoxEffectiveArea(getPosition(), getSize()), ringEffectiveArea);
    
    return ringEffectiveArea;
}

#ifdef DEBUG
void TerrainCell::update(const TimeStep& numberOfTimeSteps, unsigned int)
#else
void TerrainCell::update(const TimeStep&, unsigned int)
#endif
{
    #ifdef DEBUG
        testUpdate(numberOfTimeSteps);
    #endif

    getMutablePatchApplicator().update();
}



void TerrainCell::obtainInhabitableTerrainCells(Map* const map)
{
    if(!getPatchApplicator().getCellObstacle().isObstacle())
    {
        map->addInhabitableTerrainCell(this);
    }
}


/******************/
/*    Resource    */
/******************/

void TerrainCell::applyFunctionToResources(const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions, const PreciseDouble&)
{
    if(!resourceFunctions.empty())
    {
        if(!getPatchApplicator().getCellObstacle().isObstacle())
        {
            for(const auto& [resourceSearchParams, functions] : resourceFunctions)
            {
                for (size_t i = 0; i < getPatchApplicator().getNumberOfResources(); ++i) {
                    CellResourceInterface& resource = getMutablePatchApplicator().getMutableCellResource(i);

                    if (resourceSearchParams.matches(resource)) {
                        for (const auto& func : functions)
                        {
                            func(resource, true, nullptr, 0.0);
                        }
                    }
                }
            }
        }
    }
}

void TerrainCell::applyFunctionToResources(
        const PointContinuous &sourcePosition, const PreciseDouble &radius, 
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions, 
        const PreciseDouble&
    )
{
    if(!resourceFunctions.empty())
    {
        if(!getPatchApplicator().getCellObstacle().isObstacle())
        {
            for(const auto& [resourceSearchParams, functions] : resourceFunctions)
            {
                for (size_t i = 0; i < getPatchApplicator().getNumberOfResources(); ++i) {
                    CellResourceInterface& resource = getMutablePatchApplicator().getMutableCellResource(i);

                    if (resourceSearchParams.matches(resource)) {
                        for (const auto& func : functions)
                        {
                            func(resource, false, &sourcePosition, radius);
                        }
                    }
                }
            }
        }
    }
}


/****************/
/*    Animal    */
/****************/

void TerrainCell::applyFunctionToAnimals(const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions, const PreciseDouble&)
{
    if(animalFunctions.empty() || getPatchApplicator().getCellObstacle().isObstacle()) return;

    for(const auto& [animalSearchParams, functions] : animalFunctions)
    {
        const auto& [prevFunctions, individualFunctions, postFunctions] = functions;

        for (const auto& func : prevFunctions) func(animals);

        for(Animal* animal : animals)
        {
            if(animalSearchParams.matches(*animal)) 
            {
                for (const auto& func : individualFunctions) func(*animal);
            }
        }

        for (const auto& func : postFunctions) func(animals);
    }
}

void TerrainCell::applyFunctionToAnimals(
        function<bool(Animal&)> checker, const PointContinuous&, const PreciseDouble&,
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const PreciseDouble&
    )
{
    if(animalFunctions.empty() || getPatchApplicator().getCellObstacle().isObstacle()) return;

    for(const auto& [animalSearchParams, functions] : animalFunctions)
    {
        const auto& [prevFunctions, individualFunctions, postFunctions] = functions;

        for (const auto& func : prevFunctions) func(animals);

        for (Animal* animal : animals)
        {
            if (animalSearchParams.matches(*animal) && checker(*animal))
            {
                for (const auto& func : individualFunctions) func(*animal);
            }
        }

        for (const auto& func : postFunctions) func(animals);
    }
}


void TerrainCell::randomApplyFunctionToAnimals(const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions, const PreciseDouble&)
{
    if(animalFunctions.empty() || getPatchApplicator().getCellObstacle().isObstacle()) return;

    thread_local std::vector<size_t> animalIndexes;

    Random::createIndicesVector(animalIndexes, animals.size());

    for(const auto& [animalSearchParams, functions] : animalFunctions)
    {
        const auto& [prevFunctions, individualFunctions, postFunctions] = functions;

        for (const auto& func : prevFunctions) func(animals);

        for (size_t index : animalIndexes)
        {
            Animal& animal = *animals[index];

            if (animalSearchParams.matches(animal))
            {
                for (const auto& func : individualFunctions) func(animal);
            }
        }

        for (const auto& func : postFunctions) func(animals);
    }
}

void TerrainCell::randomApplyFunctionToAnimals(
        function<bool(Animal&)> checker, const PointContinuous&, const PreciseDouble&,
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const PreciseDouble&
    )
{
    if(animalFunctions.empty() || getPatchApplicator().getCellObstacle().isObstacle()) return;

    thread_local std::vector<size_t> animalIndexes;

    Random::createIndicesVector(animalIndexes, animals.size());

    for(const auto& [animalSearchParams, functions] : animalFunctions)
    {
        const auto& [prevFunctions, individualFunctions, postFunctions] = functions;

        for (const auto& func : prevFunctions) func(animals);

        for (size_t index : animalIndexes)
        {
            Animal& animal = *animals[index];

            if (animalSearchParams.matches(animal) && checker(animal))
            {
                for (const auto& func : individualFunctions) func(animal);
            }
        }

        for (const auto& func : postFunctions) func(animals);
    }
}


/****************/
/*    Edible    */
/****************/

void TerrainCell::applyFunctionToEdibles(
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions,
        const PreciseDouble& animalBodySection
    )
{
    applyFunctionToAnimals(animalFunctions, animalBodySection);
    applyFunctionToResources(resourceFunctions, animalBodySection);
}

void TerrainCell::applyFunctionToEdibles(
        function<bool(Animal&)> checker, const PointContinuous &sourcePosition, const PreciseDouble &radius, 
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions,
        const PreciseDouble& animalBodySection
    )
{
    applyFunctionToAnimals(checker, sourcePosition, radius, animalFunctions, animalBodySection);
    applyFunctionToResources(sourcePosition, radius, resourceFunctions, animalBodySection);
}


/************************/
/*    Edible in Cell    */
/************************/

void TerrainCell::applyFunctionToEdiblesInCell(bool fullCoverage, 
    const PointContinuous &sourcePosition, const PreciseDouble &radius, 
    const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
    const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions,
    const PreciseDouble& animalBodySection)
{   
    if(fullCoverage)
    {
        applyFunctionToEdibles(animalFunctions, resourceFunctions, animalBodySection);
    }
    else
    {
        auto radiusChecker = [&sourcePosition, &radius](Animal& animal) {
            return Geometry::pointInsideSphere(animal.getPosition(), sourcePosition, radius);
        };

        applyFunctionToEdibles(radiusChecker, sourcePosition, radius, animalFunctions, resourceFunctions, animalBodySection);
    }
}




#ifdef DEBUG
    void TerrainCell::testUpdate(const TimeStep actualTimeStep)
    {
        if(updateLastTimeStep == actualTimeStep)
        {
            throwLineInfoException("Terrain cell updated twice in the same time step.");
        }

        updateLastTimeStep = actualTimeStep;
    }
#endif


void TerrainCell::getCellEvaluation(vector<CellValue>& bestEvaluations, AnimalNonStatistical* animalWhoIsEvaluating, const PointContinuous &sourcePosition, const PreciseDouble &radius, bool searchNeighborsWithFemales, bool searchNeighborsWithMales, const bool parentFullCoverage, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchEdibilityValueGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchPredationRiskGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchConspecificBiomassGlobal)
{
    if (getPatchApplicator().getCellObstacle().isObstacle()) return;


    bool fullCoverage = parentFullCoverage;
    bool nullCoverage;

    if(fullCoverage)
    {
        nullCoverage = false;
    }
    else
    {
        switch(Geometry::checkCoveredLevelBySphere(getEffectiveArea(), sourcePosition, radius)) {
            case Coverage::Full: {
                fullCoverage = true;
                nullCoverage = false;
                break;
            }
            case Coverage::Over50Percent:
            case Coverage::Partial: {
                nullCoverage = false;
                break;
            }
            case Coverage::Null: {
                nullCoverage = true;
                break;
            }
            default: {
                throwLineInfoException("Default case");
                break;
            }
        }
    }
        


    if(!nullCoverage)
    {
        PreciseDouble totalPredatoryRiskEdibilityValue = 0.0;
        PreciseDouble totalEdibilityValue = 0.0;
        PreciseDouble totalConspecificBiomass = 0.0;

        const Animal* bestAnimal = nullptr;
        PreciseDouble bestAnimalEdibilityValue = 0.0;

        const CellResourceInterface* bestResource = nullptr;
        PreciseDouble bestResourceEdibilityValue = 0.0;


        vector<pair<const AnimalSearchParams&, AnimalFunctions>> animalFunctions;

        animalFunctions.emplace_back(
            animalWhoIsEvaluating->getSpecies()->getConspecificSearchParams(),
            AnimalFunctions{
                PreviousAnimalFunctions{},
                IndividualFunctions{
                    [&animalWhoIsEvaluating, &totalConspecificBiomass](Animal& animal) {
                        totalConspecificBiomass += animalWhoIsEvaluating->calculateConspecificBiomass(static_cast<AnimalNonStatistical&>(animal)).getValue();
                    }
                },
                PostAnimalFunctions{}
            }
        );

        animalFunctions.emplace_back(
            animalWhoIsEvaluating->getSpecies()->getPreySearchParams(animalWhoIsEvaluating->getInstarToEvaluateCells()).getAnimalSearchParams(),
            AnimalFunctions{
                PreviousAnimalFunctions{},
                IndividualFunctions{
                    [&animalWhoIsEvaluating, &totalEdibilityValue, &bestAnimalEdibilityValue, &bestAnimal](Animal& animal) {
                        AnimalNonStatistical* animalCast = static_cast<AnimalNonStatistical*>(&animal);

                        const PreciseDouble edibilityValue = animalWhoIsEvaluating->calculateCellQuality(*animalCast);

                        totalEdibilityValue += edibilityValue;

                        if(bestAnimalEdibilityValue < edibilityValue)
                        {
                            bestAnimalEdibilityValue = edibilityValue;
                            bestAnimal = animalCast;
                        }
                    }
                },
                PostAnimalFunctions{}
            }
        );

        animalFunctions.emplace_back(
            animalWhoIsEvaluating->getSpecies()->getPredatorSearchParams(animalWhoIsEvaluating->getInstarToEvaluateCells()),
            AnimalFunctions{
                PreviousAnimalFunctions{},
                IndividualFunctions{
                    [&animalWhoIsEvaluating, &totalPredatoryRiskEdibilityValue](Animal& animal) {
                        totalPredatoryRiskEdibilityValue += animalWhoIsEvaluating->calculatePredationRisk(static_cast<AnimalNonStatistical&>(animal));
                    }
                },
                PostAnimalFunctions{}
            }
        );


        vector<pair<const ResourceSearchParams&, ResourceFunctions>> resourceFunctions;

        resourceFunctions.emplace_back(
            animalWhoIsEvaluating->getSpecies()->getPreySearchParams(animalWhoIsEvaluating->getInstarToEvaluateCells()).getResourceSearchParams(),
            ResourceFunctions{
                [&animalWhoIsEvaluating, &bestResourceEdibilityValue, &bestResource, &totalEdibilityValue](CellResourceInterface& resource, bool fullCoverage, const PointContinuous* const sourcePosition, const PreciseDouble &radius) {
                    const DryMass dryMassAvailable = resource.calculateDryMassAvailable(fullCoverage, sourcePosition, radius);
                    
                    const PreciseDouble edibilityValue = animalWhoIsEvaluating->calculateCellQuality(resource, dryMassAvailable);

                    totalEdibilityValue += edibilityValue;

                    if(bestResourceEdibilityValue < edibilityValue)
                    {
                        bestResourceEdibilityValue = edibilityValue;
                        bestResource = &resource;
                    }
                }
            }
        );

        const PreciseDouble sectionSize = animalWhoIsEvaluating->getSpecies()->getPoreUsePerInstar(animalWhoIsEvaluating->getInstar());

        applyFunctionToEdiblesInCell(fullCoverage, sourcePosition, radius, animalFunctions, resourceFunctions, sectionSize);


        const Edible* bestEdible;

        // Priority is given to animals over resources
        if(bestAnimal != nullptr)
        {
            bestEdible = bestAnimal;
        }
        else
        {
            bestEdible = bestResource;
        }


        unsigned int numberOfFemales = 0;
		unsigned int numberOfMales = 0;

#ifdef DEBUG
        if (searchNeighborsWithFemales && searchNeighborsWithMales)
        {
			throwLineInfoException("Cannot search for both females and males at the same time");
        }
#endif

        if(searchNeighborsWithFemales)
        {
            numberOfFemales = getNumberOfMatureFemales(animalWhoIsEvaluating->getMutableSpecies(), sectionSize);
        }

		if (searchNeighborsWithMales)
		{
            numberOfMales = getNumberOfMatureMales(animalWhoIsEvaluating->getMutableSpecies(), sectionSize);
		}


        animalWhoIsEvaluating->setMaximumCellEvaluation(totalEdibilityValue, totalPredatoryRiskEdibilityValue, totalConspecificBiomass);


        PreciseDouble& instarMaximumPatchEdibilityValueGlobal = maximumPatchEdibilityValueGlobal[animalWhoIsEvaluating->getGrowthBuildingBlock().getInstar()];
        instarMaximumPatchEdibilityValueGlobal = fmax(instarMaximumPatchEdibilityValueGlobal, totalEdibilityValue);

        PreciseDouble& instarMaximumPatchPredationRiskGlobal = maximumPatchPredationRiskGlobal[animalWhoIsEvaluating->getGrowthBuildingBlock().getInstar()];
        instarMaximumPatchPredationRiskGlobal = fmax(instarMaximumPatchPredationRiskGlobal, totalPredatoryRiskEdibilityValue);

        PreciseDouble& instarMaximumPatchConspecificBiomassGlobal = maximumPatchConspecificBiomassGlobal[animalWhoIsEvaluating->getGrowthBuildingBlock().getInstar()];
        instarMaximumPatchConspecificBiomassGlobal = fmax(instarMaximumPatchConspecificBiomassGlobal, totalConspecificBiomass);


        CellValue cellEvaluation(bestEdible, 
            fullCoverage, &getEffectiveArea(), &getPosition(), getCenter(), 
            totalEdibilityValue, totalPredatoryRiskEdibilityValue, totalConspecificBiomass, numberOfFemales, numberOfMales,
            getPatchApplicator().getCellHabitatDomain().isHabitatDomain(animalWhoIsEvaluating->getSpecies()->getAnimalSpeciesId(), animalWhoIsEvaluating->getInstarToEvaluateCells())
        );


        bestEvaluations.emplace_back(cellEvaluation);
    }
}

size_t TerrainCell::calculateManhattanDistanceToPoint(const PointMap &point) const
{
    size_t distance = 0;

    for(unsigned char i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();

        if(point.get(axis) > getPosition().get(axis))
        {
            distance += point.get(axis) - getPosition().get(axis);
        }
        else
        {
            distance += getPosition().get(axis) - point.get(axis);
        }
    }

    return distance;
}














void TerrainCell::deserializeSources(
        vector<pair<size_t, MoistureSource*>>& appliedMoisture,
        std::vector<vector<pair<size_t, ResourceSource*>>>& appliedResource
    )
{
    getMutablePatchApplicator().deserializeSources(appliedMoisture, appliedResource);
}

void TerrainCell::deserializeSpecies(std::vector<ResourceSpecies*>& existingResourceSpecies, std::vector<AnimalSpecies*>& existingAnimalSpecies)
{
    getMutablePatchApplicator().deserializeSpecies(existingResourceSpecies);

    for(Animal*& animal : animals)
    {
        animal->setSpecies(existingAnimalSpecies[animal->getAnimalSpeciesId()]);
    }
}


void TerrainCell::registerAnimals(vector<AnimalNonStatistical*>& landscapeAnimals)
{
    for(Animal*& animal : animals)
    {
        landscapeAnimals.push_back(static_cast<AnimalNonStatistical*>(animal));
    }
}

void TerrainCell::saveResourceSpeciesSnapshot(ofstream &file, const ResourceSpecies* const &species) const
{
    WetMass value(0.0);

    if(!getPatchApplicator().getCellObstacle().isObstacle())
    {
        value = getPatchApplicator().getCellResource(species->getResourceSpeciesId()).getGrowthBuildingBlock().getCurrentTotalWetMass();
    }

    file.write((char *) &value.getValue(), sizeof(double));
}


void TerrainCell::saveWaterSnapshot(ofstream &file) const
{
    if(!getPatchApplicator().getCellObstacle().isObstacle())
    {
        file.write((char *) &getPatchApplicator().getCellMoisture().getMoisture(), sizeof(double));
    }
}

unsigned int TerrainCell::getNumberOfMatureFemales(AnimalSpecies *const animalSpecies, const PreciseDouble& animalBodySection)
{
    unsigned int counter = 0;

    vector<pair<const AnimalSearchParams&, AnimalFunctions>> animalFunctions;

    animalFunctions.emplace_back(
        animalSpecies->getMatureFemalesSearchParams(),
        AnimalFunctions{
            PreviousAnimalFunctions{},
            IndividualFunctions{
                [&counter](Animal&) { 
                    counter++;
                }
            },
            PostAnimalFunctions{}
        }
    );

    applyFunctionToAnimals(animalFunctions, animalBodySection);

    return counter;
}

unsigned int TerrainCell::getNumberOfMatureMales(AnimalSpecies* const animalSpecies, const PreciseDouble& animalBodySection)
{
    unsigned int counter = 0;

    vector<pair<const AnimalSearchParams&, AnimalFunctions>> animalFunctions;

    animalFunctions.emplace_back(
        animalSpecies->getMatureMalesSearchParams(),
        AnimalFunctions{
            PreviousAnimalFunctions{},
            IndividualFunctions{
                [&counter](Animal&) {
                    counter++;
                }
            },
            PostAnimalFunctions{}
        }
    );

    applyFunctionToAnimals(animalFunctions, animalBodySection);

    return counter;
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(TerrainCell)

template <class Archive>
void TerrainCell::serialize(Archive &ar, const unsigned int) {
    ar & patchApplicator;

    if (Archive::is_loading::value)
	{
        patchApplicator->setOwner(this);
    }

    ar & position;
    ar & size;
    ar & hyperVolume;
    ar & center;

    if (Archive::is_loading::value)
	{
		effectiveArea = makeEffectiveArea();
	}

    #ifdef DEBUG
        if (Archive::is_loading::value)
	    {
            updateLastTimeStep = TimeStep(UINT_MAX);
        }
    #endif

    ar & animals;

    if (Archive::is_loading::value)
	{
        for(Animal*& animal : animals)
        {
            animal->setTerrainCell(this);
        }
    }
}

// // Specialisation
template void TerrainCell::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void TerrainCell::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void TerrainCell::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void TerrainCell::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
