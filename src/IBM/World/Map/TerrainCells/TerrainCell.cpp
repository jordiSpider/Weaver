
#include "IBM/World/Map/TerrainCells/TerrainCell.h"
#include "IBM/World/World.h"
#include "IBM/World/Map/TerrainCells/EdibleSearchParams.h"
#include "IBM/World/Map/Map.h"

using namespace std;
namespace fs = boost::filesystem;



TerrainCell::TerrainCell(PointMap* const &position, const Ring *const effectiveArea, const double &size, Map* const &map,
        LifeStageVector* const animals,
        const bool obstacle, const bool fullObstacle, const int obstaclePatchPriority, MoistureInterface* const moistureInfo, 
        const bool moistureSource, const bool inMoisturePatch, const int moisturePatchPriority, const double &totalMaximumResourceCapacity)
    : TerrainCellInterface(),
      position(position), center(makeCenter(position, size)), size(size), map(map), effectiveArea(effectiveArea), 
      copied(false), totalMaximumResourceCapacity(totalMaximumResourceCapacity),
      obstacle(obstacle), fullObstacle(fullObstacle), moistureInfo(moistureInfo), moistureSource(moistureSource), 
      inMoisturePatch(inMoisturePatch), resources(ResourceSpecies::getResourceSpeciesCounter()),
      resourcesPointers(ResourceSpecies::getResourceSpeciesCounter(), nullptr),
      resourcesConstPointers(ResourceSpecies::getResourceSpeciesCounter(), nullptr), animals(animals), 
      obstaclePatchPriority(obstaclePatchPriority), 
      moisturePatchPriority(moisturePatchPriority), resourcePatchPriority(ResourceSpecies::getResourceSpeciesCounter(), -1)
{
    #ifdef DEBUG
        updateLastTimeStep = UINT_MAX;
    #endif
}


TerrainCell::~TerrainCell()
{
    if(!isCopied())
    {
        delete position;

        eraseAllAnimals();
        delete animals;
        
        if(!isMoistureSource())
        {
            delete moistureInfo;
        }

        delete effectiveArea;
    }
}

LifeStageVector* const TerrainCell::makeDefaultAnimals(const Map* const &map)
{
    auto defaultAnimals = new LifeStageVector(LifeStage::size());

    for(auto &lifeStage : *defaultAnimals)
    {
        lifeStage.resize(AnimalSpecies::getAnimalSpeciesCounter());
        for(unsigned int animalSpecies = 0; animalSpecies < map->getWorld()->getExistingAnimalSpecies().size(); animalSpecies++)
        {
            lifeStage[animalSpecies].resize(map->getWorld()->getExistingAnimalSpecies().at(animalSpecies)->getNumberOfInstars());
            for(auto &instar : lifeStage[animalSpecies])
            {
                instar.resize(AnimalSpecies::Gender::size());
            }
        }
    }

    return defaultAnimals;
}


void TerrainCell::setFullObstacle(const ObstaclePatch &obstaclePatch)
{
    setFullObstacle(true, true, obstaclePatch.getPriority());
}


void TerrainCell::setFullObstacle(const bool newFullObstacleValue, const bool newObstacleValue, const int newObstaclePatchPriority)
{
    this->fullObstacle = newFullObstacleValue;
    setObstacle(newObstacleValue, newObstaclePatchPriority);
}

const bool TerrainCell::isCopied() const
{
    return copied;
}


void TerrainCell::setCopied(const bool newCopiedValue)
{
    copied = newCopiedValue;
}

bool TerrainCell::isFullObstacle() const
{
    return fullObstacle;
}


const PointMap& TerrainCell::getPosition() const
{
    return *position;
}

PointMap& TerrainCell::getMutablePosition() const
{
    return *position;
}

const Map& TerrainCell::getMap() const
{
    return *map;
}

Map& TerrainCell::getMutableMap() const
{
    return *map;
}


const World* const TerrainCell::getWorld() const
{
    return getMap().getWorld();
}

World* const TerrainCell::getMutableWorld() const
{
    return getMutableMap().getMutableWorld();
}

const double& TerrainCell::getSize() const
{
    return size;
}


const LifeStageVector& TerrainCell::getAnimals() const
{
    return *animals;
}

LifeStageVector& TerrainCell::getMutableAnimals() const
{
    return *animals;
}

const vector<int>& TerrainCell::getResourcePatchPriority() const
{
    return resourcePatchPriority;
}

const MoistureInterface* const& TerrainCell::getMoistureInfo() const
{
    return moistureInfo;
}

MoistureInterface* const& TerrainCell::getMutableMoistureInfo() const
{
    return moistureInfo;
}

const int TerrainCell::getMoisturePatchPriority() const
{
    return moisturePatchPriority;
}


const Ring& TerrainCell::getEffectiveArea() const
{
	return *effectiveArea;
}


void TerrainCell::setResourcePatchPriority(const unsigned int resourceSpeciesId, const int newResourcePatchPriority)
{
    resourcePatchPriority[resourceSpeciesId] = newResourcePatchPriority;
}


void TerrainCell::setObstacle(const bool newObstacleValue, const int newObstaclePatchPriority)
{
    this->obstacle = newObstacleValue;
    setObstaclePatchPriority(newObstaclePatchPriority);
}


void TerrainCell::setObstaclePatchPriority(const int newObstaclePatchPriority)
{
    obstaclePatchPriority = newObstaclePatchPriority;
}


bool TerrainCell::isObstacle() const {
    return obstacle;
}


void TerrainCell::insertAnimal(Animal* const newAnimal)
{
    addAnimal(newAnimal->getLifeStage(), newAnimal->getSpecies()->getAnimalSpeciesId(), newAnimal->getInstar(), newAnimal->getGender(), newAnimal);
}


tuple<bool, TerrainCellInterface*, TerrainCellInterface*, Animal*, unsigned int> TerrainCell::randomInsertAnimal(const Instar &instar, AnimalSpecies* animalSpecies, const bool isStatistical)
{
    Animal* newAnimal;
    unsigned int numberOfDiscardedIndividualsOutsideRestrictedRanges;
    
    if(isStatistical)
    {
        newAnimal = new AnimalStatistical(instar, animalSpecies, this);
    }
    else
    {
        tie(newAnimal, numberOfDiscardedIndividualsOutsideRestrictedRanges) = createAnimal(instar, animalSpecies);
    }
    

    vector<double> axisValues(DIMENSIONS);
    
    for(unsigned int i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();
        axisValues[i] = getPosition().get(axis) * getSize() + Random::randomUniform(0, getSize());
    }

    newAnimal->setPosition(axisValues);
    TerrainCell::insertAnimal(newAnimal);

    return make_tuple(false, nullptr, nullptr, newAnimal, numberOfDiscardedIndividualsOutsideRestrictedRanges);
}


const unique_ptr<PointContinuous>& TerrainCell::getCenter() const
{
    return center;
}


unique_ptr<PointContinuous> TerrainCell::makeCenter(const PointMap* const &position, const double &size) const
{
    auto center = make_unique<PointContinuous>();

    const double halfOfSize = size / 2.0;

    for(unsigned int i = 0; i < DIMENSIONS; i++)
    {
        setPositionAxisValue(*center, i, position->get(magic_enum::enum_cast<Axis>(i).value()) * size + halfOfSize);
    }

    return center;
}

void TerrainCell::addAnimal(const LifeStage &lifeStage, const unsigned int animalSpeciesId, const Instar &instar, const AnimalSpecies::Gender::GenderValue &gender, Animal* const newAnimal)
{
    animals->at(lifeStage.getValue())[animalSpeciesId][instar][gender].push_back(newAnimal);

    newAnimal->setTerrainCellInterface(this);
}

void TerrainCell::eraseAnimal(Animal* const animalToRemove)
{
    eraseAnimal(animalToRemove->getLifeStage(), animalToRemove->getSpecies()->getAnimalSpeciesId(), animalToRemove->getInstar(), animalToRemove->getGender(), animalToRemove);
}

void TerrainCell::eraseAnimal(const LifeStage &lifeStage, const unsigned int animalSpeciesId, const Instar &instar, const AnimalSpecies::Gender::GenderValue &gender, Animal* const animalToRemove)
{
    AnimalVector* vector = &animals->at(lifeStage.getValue())[animalSpeciesId][instar][gender];
    vector->erase(std::remove(vector->begin(), vector->end(), animalToRemove), vector->end());
}

void TerrainCell::changeAnimalToSenesced(Animal* targetAnimal, const unsigned int numberOfTimeSteps)
{
	#ifdef DEBUG
	//Output::coutFlush("{} predated {}\n", targetAnimal->getPredatedByID(), targetAnimal->getId());
	#endif

	targetAnimal->setNewLifeStage(LifeStage::SENESCED, numberOfTimeSteps);
}

void TerrainCell::eraseAllAnimals()
{
    for(auto &lifeStage : *animals)
    {
        for(auto &animalSpecies : lifeStage)
        {
            for(auto &instar : animalSpecies)
            {
                for(auto &gender : instar)
                {
                    for(auto &animal : gender)
                    {
                        delete animal;
                    }
                    gender.clear();
                }
            }
        }
    }
}


const Ring* const TerrainCell::makeEffectiveArea(PointMap* const position, const double size) const
{
    auto boxEffectiveArea = Geometry::makeBoxEffectiveArea(*position, size);

    Ring ringEffectiveArea;
	boost::geometry::convert(*boxEffectiveArea, ringEffectiveArea);
    
    return new Ring(ringEffectiveArea);
}

vector<ResourceInterface*>& TerrainCell::getMutableResources()
{
    return resourcesPointers;
}

const vector<const ResourceInterface*>& TerrainCell::getResources() const
{
    return resourcesConstPointers;
}

ResourceInterface* const TerrainCell::getMutableResource(const unsigned int resourceSpeciesId)
{
    return resourcesPointers[resourceSpeciesId];
}

const ResourceInterface* const TerrainCell::getResource(const unsigned int resourceSpeciesId) const
{
    return resourcesConstPointers[resourceSpeciesId];
}

const double TerrainCell::getBiomass(const unsigned int resourceSpeciesId) const
{
    if(getResource(resourceSpeciesId) == nullptr)
    {
        return 0.0;
    }
    else
    {
        return getResource(resourceSpeciesId)->getBiomass();
    }
}

void TerrainCell::updateMoisture()
{
    moistureInfo->update();
}

void TerrainCell::update(const unsigned int numberOfTimeSteps, ostream& tuneTraitsFile)
{
    #ifdef DEBUG
        testUpdate();
    #endif

    moistureInfo->update();
    for(auto &resource : getMutableResources())
    {
        if(resource != nullptr)
        {
            if(!resource->isFullCapacity())
            {
                resource->update();
            }
        }
    }
    activateAndResumeAnimals(numberOfTimeSteps);
    tuneAnimals(numberOfTimeSteps, tuneTraitsFile);
}


double TerrainCell::spreadResource(const double &massToSpread, const unsigned int resourceSpeciesId)
{
    auto neighbours = this->getMutableMap().getResourceNeighbours(this, resourceSpeciesId, 1);

    if(!neighbours->empty()) {
        double excess = 0.0;

        const double massPerNeighbor = massToSpread / neighbours->size();

        for(auto &neighbour : *neighbours)
        {
            excess += neighbour->addResourceBiomass(massPerNeighbor, resourceSpeciesId, this);
        }

        return excess;
    }
    else
    {
        return massToSpread;
    }
}

void TerrainCell::setTotalMaximumResourceCapacity(const double &newTotalMaximumResourceCapacity)
{
    totalMaximumResourceCapacity = newTotalMaximumResourceCapacity;
}

const double& TerrainCell::getTotalMaximumResourceCapacity() const
{
    return totalMaximumResourceCapacity;
}

void TerrainCell::updateTotalMaximumResourceCapacity()
{
    if(isInMoisturePatch())
    {
        setTotalMaximumResourceCapacity(pow(getSize(), DIMENSIONS) * getMoistureInfo()->getMaximumResourceCapacityDensity());
    }
}

bool TerrainCell::resourceIsPresent(const unsigned int resourceSpeciesId) const
{
    return getResource(resourceSpeciesId) != nullptr;
}

double TerrainCell::addResourceBiomass(const double &addedMass, const unsigned int resourceSpeciesId, const TerrainCellInterface* const source)
{
    return getMutableResource(resourceSpeciesId)->addBiomass(addedMass, source);
}

double TerrainCell::addResourceBiomass(const double &addedMass, const unsigned int resourceSpeciesId, const unique_ptr<vector<unsigned int>> &childrenPositions)
{
    return getMutableResource(resourceSpeciesId)->addBiomass(addedMass, childrenPositions);
}


double TerrainCell::getTotalResourceBiomass() const
{
	double mass = 0.0;

    for(unsigned int i = 0; i < getMap().getWorld()->getExistingResourceSpecies().size(); i++)
    {
        if(getResource(i) != nullptr)
        {
            mass += getResource(i)->calculateWetMass();
        }
    }

	return mass;
}


void TerrainCell::obtainInhabitableTerrainCells()
{
    if(!isObstacle())
    {
        getMutableMap().addInhabitableTerrainCell(this);
    }
}


const int TerrainCell::getResourcePatchPriority(const unsigned int resourceSpeciesId) const
{
    return resourcePatchPriority[resourceSpeciesId];
}


void TerrainCell::activateAndResumeAnimals(const unsigned int numberOfTimeSteps)
{
    // Unborn animals

    applyFunctionToAnimals(
        {
            [numberOfTimeSteps](AnimalNonStatistical& animal) { 
                animal.isReadyToBeBorn(numberOfTimeSteps);
            }
        },
        getMap().getLifeStageSearchParams(LifeStage::UNBORN)
    );

    // Diapause animals

    applyFunctionToAnimals(
        {
            [](AnimalNonStatistical& animal) { 
                animal.getMutableAnimalGrowth()->isReadyToResumeFromDiapauseOrIncreaseDiapauseTimeSteps(animal.getTerrainCellInterface()->getMoistureInfo()->getMoisture());
            }
        },
        getMap().getLifeStageSearchParams(LifeStage::DIAPAUSE)
    );

    // Pupa animals
    applyFunctionToAnimals(
        {
            [](AnimalNonStatistical& animal) { 
                animal.getMutableAnimalGrowth()->isReadyToResumeFromPupaOrDecreasePupaTimer();
            }
        },
        getMap().getLifeStageSearchParams(LifeStage::PUPA)
    );

    // Handling animals
    /*
    applyFunctionToAnimals(
        {},
        getMap().getLifeStageSearchParams(LifeStage::HANDLING)
    );
    */
}

void TerrainCell::applyFunctionToAnimals(const vector<function<void(AnimalNonStatistical&)>> &functions,
        const AnimalSearchParams &animalSearchParams)
{
    for(const LifeStage::LifeStageValue& lifeStage : animalSearchParams.getSearchableLifeStages())
    {
        for(const id_type& animalSpeciesId : animalSearchParams.getSearchableAnimalSpecies(lifeStage))
        {
            for(const Instar& instar : animalSearchParams.getSearchableInstars(lifeStage, animalSpeciesId))
            {
                for(const AnimalSpecies::Gender::GenderValue& gender : animalSearchParams.getSearchableGenders(lifeStage, animalSpeciesId, instar))
                {
                    for(auto &animal : animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender))
                    {
                        for(const auto &func : functions)
                        {
                            func(*static_cast<AnimalNonStatistical*>(animal));
                        }
                    }
                }
            }
        }
    }
}

#ifdef DEBUG
    void TerrainCell::testUpdate()
    {
        if(updateLastTimeStep == getMap().getWorld()->getActualTimeStep())
        {
            throwLineInfoException("Terrain cell updated twice in the same time step.");
        }

        updateLastTimeStep = getMap().getWorld()->getActualTimeStep();
    }
#endif

void TerrainCell::randomApplyFunctionToAnimals(const vector<function<void(AnimalNonStatistical&)>> &functions,
        const AnimalSearchParams &animalSearchParams)
{
    auto randomIndexLifeStageVector = Random::createIndicesVector(animalSearchParams.getSearchableLifeStages().size());
    for(const auto &randomIndexLifeStage : *randomIndexLifeStageVector)
    {
        auto lifeStageIt = animalSearchParams.getSearchableLifeStages().begin();
        advance(lifeStageIt, randomIndexLifeStage);

        const LifeStage::LifeStageValue lifeStage = *lifeStageIt;

        auto randomIndexAnimalSpeciesIdVector = Random::createIndicesVector(animalSearchParams.getSearchableAnimalSpecies(lifeStage).size());
        for(const auto &randomIndexAnimalSpeciesId : *randomIndexAnimalSpeciesIdVector)
        {
            auto animalSpeciesIdIt = animalSearchParams.getSearchableAnimalSpecies(lifeStage).begin();
            advance(animalSpeciesIdIt, randomIndexAnimalSpeciesId);

            const id_type animalSpeciesId = *animalSpeciesIdIt;

            auto randomIndexInstarVector = Random::createIndicesVector(animalSearchParams.getSearchableInstars(lifeStage, animalSpeciesId).size());
            for(const auto &randomIndexInstar : *randomIndexInstarVector)
            {
                auto instarIt = animalSearchParams.getSearchableInstars(lifeStage, animalSpeciesId).begin();
                advance(instarIt, randomIndexInstar);

                const Instar instar = *instarIt;

                auto randomIndexGenderVector = Random::createIndicesVector(animalSearchParams.getSearchableGenders(lifeStage, animalSpeciesId, instar).size());
                for(const auto &randomIndexGender : *randomIndexGenderVector)
                {
                    auto genderIt = animalSearchParams.getSearchableGenders(lifeStage, animalSpeciesId, instar).begin();
                    advance(genderIt, randomIndexGender);

                    const AnimalSpecies::Gender::GenderValue gender = *genderIt;

                    auto animalVector = &animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender);

                    // Este flag indica si el vector actual se ha modificado y por lo tanto se debe
                    // calcular de nuevo las posiciones aleatorias, las funciones a usar tienen que tener
                    // en cuenta que animales ya han realizado la acción, tal y como se ha con la función 
                    // de movimiento
                    bool modifiedVector;

                    do
                    {
                        modifiedVector = false;

                        auto randomIndexAnimalVector = Random::createIndicesVector(animalVector->size());

                        for(unsigned int i = 0; i < randomIndexAnimalVector->size() && !modifiedVector; i++)
                        {
                            for(const auto &func : functions)
                            {
                                func(*static_cast<AnimalNonStatistical*>(animalVector->at(randomIndexAnimalVector->at(i))));
                            }

                            if(animalVector->size() != randomIndexAnimalVector->size())
                            {
                                modifiedVector = true;
                            }
                        }
                    }
                    while(modifiedVector);
                }
            }
        }
    }
}

void TerrainCell::tuneAnimals(const unsigned int numberOfTimeSteps, ostream& tuneTraitsFile)
{
    for(auto &lifeStage : {LifeStage::ACTIVE, LifeStage::PUPA, LifeStage::SATIATED, LifeStage::HANDLING, LifeStage::DIAPAUSE}) {
        for(auto &animalSpeciesElem : animals->at(lifeStage))
        {
            for(auto &instarElem : animalSpeciesElem)
            {
                for(auto &genderElem : instarElem)
                {
                    for(auto &animalElem : genderElem)
                    {
                        if(static_cast<AnimalNonStatistical*>(animalElem)->getUpdateLastTimeStep() != numberOfTimeSteps)
                        {
                            static_cast<AnimalNonStatistical*>(animalElem)->setUpdateLastTimeStep(numberOfTimeSteps);
                            static_cast<AnimalNonStatistical*>(animalElem)->increaseAge(1);
                            static_cast<AnimalNonStatistical*>(animalElem)->tune(getMoistureInfo()->getTemperature(), numberOfTimeSteps, getMoistureInfo()->getMoisture(), tuneTraitsFile, true);
                        }
                    }
                }
            }
        }
    }
}


void TerrainCell::purgeDeadAnimals()
{
    for(const auto &lifeStage : {LifeStage::STARVED, LifeStage::PREDATED, LifeStage::BACKGROUND, LifeStage::SENESCED, LifeStage::SHOCKED})
    {
        for(auto &animalSpecies : animals->at(lifeStage))
        {
            for(auto &instar : animalSpecies)
            {
                for(auto &gender : instar)
                {
                    for(auto &animal : gender)
                    {
                        delete animal;
                    }

                    gender.clear();
                }
            }
        }
    }
}


void TerrainCell::moveAnimals(const unsigned int numberOfTimeSteps, ostream& encounterProbabilitiesFile, ostream& predationProbabilitiesFile, bool saveEdibilitiesFile, ostream& edibilitiesFile, float exitTimeThreshold, double pdfThreshold, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double maxSearchArea, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
	auto t0 = clock();

    randomApplyFunctionToAnimals(
        {
            [numberOfTimeSteps, &encounterProbabilitiesFile, &predationProbabilitiesFile, saveEdibilitiesFile, &edibilitiesFile, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH](AnimalNonStatistical& animal) { 
                animal.moveAnimal(numberOfTimeSteps, encounterProbabilitiesFile, predationProbabilitiesFile, saveEdibilitiesFile, edibilitiesFile, pdfThreshold, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, maxSearchArea, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
            }
        },
        getMap().getLifeStageSearchParams(LifeStage::ACTIVE)
    );

    auto t1 = clock();

    if((double(t1-t0)/CLOCKS_PER_SEC) > exitTimeThreshold)
    {
        throwLineInfoException("too many animals for too little food!!!");
	}

    getMutableMap().getMutableWorld()->increaseMovePrintBar();
}

tuple<double, double, double> TerrainCell::evaluateAnimal(const AnimalNonStatistical &animalWhoIsEvaluating, 
        const list<const EdibleInterface*> &ediblesHasTriedToPredate, const AnimalNonStatistical &animal,
        double preferenceThresholdForEvaluation, 
        double muForPDF, double sigmaForPDF, 
        double predationSpeedRatioAH, double predationHunterVoracityAH, 
        double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, 
        double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, 
        double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, 
        double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, 
        double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, 
        double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
    double predatoryRiskEdibilityValue = 0.0;
    double edibilityValue = 0.0;
    double conspecificBiomass = 0.0;

    if(animal.canEatEdible(&animalWhoIsEvaluating, ediblesHasTriedToPredate, animalWhoIsEvaluating.calculateDryMass()))
    {
        predatoryRiskEdibilityValue = animal.calculatePredatoryRiskEdibilityValue(animalWhoIsEvaluating, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
    }

    if(animalWhoIsEvaluating.canEatEdible(&animal, ediblesHasTriedToPredate, animal.calculateDryMass()))
    {
        if(animalWhoIsEvaluating.getSpecies()->getEdiblePreference(animal.getSpecies()->getId(), animalWhoIsEvaluating.getInstar(), animal.getInstar()) > preferenceThresholdForEvaluation){	
            edibilityValue = animalWhoIsEvaluating.calculateEdibilityValueWithMass(animal, animal.calculateDryMass(), muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
        }
    }

    if(animalWhoIsEvaluating.getSpecies()->getAnimalSpeciesId() == animal.getSpecies()->getAnimalSpeciesId())
    {
        conspecificBiomass = animal.getInterpolatedDryMass(0, animal.calculateDryMass());
    }

    return make_tuple(predatoryRiskEdibilityValue, edibilityValue, conspecificBiomass);
}

double TerrainCell::evaluateResource(const AnimalNonStatistical &animalWhoIsEvaluating, 
        const list<const EdibleInterface*> &ediblesHasTriedToPredate, const ResourceInterface* resource,
        const double dryMass, double preferenceThresholdForEvaluation, 
        double muForPDF, double sigmaForPDF, 
        double predationSpeedRatioAH, double predationHunterVoracityAH, 
        double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, 
        double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, 
        double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, 
        double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, 
        double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, 
        double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
    double edibilityValue = 0.0;

    if(animalWhoIsEvaluating.canEatEdible(resource, ediblesHasTriedToPredate, dryMass))
    {
        if(animalWhoIsEvaluating.getSpecies()->getEdiblePreference(resource->getSpecies()->getId(), animalWhoIsEvaluating.getInstar(), resource->getInstar()) > preferenceThresholdForEvaluation){	
            edibilityValue = animalWhoIsEvaluating.calculateEdibilityValueWithMass(*resource, dryMass, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
        }
    }

    return edibilityValue;
}

tuple<double, double, double, const EdibleInterface*> TerrainCell::getCellEvaluation(AnimalNonStatistical* animalWhoIsEvaluating, pair<TerrainCellInterface::TerrainCellCoverage, unique_ptr<Ring>> &evaluationArea, const list<const EdibleInterface*> &ediblesHasTriedToPredate, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
    //arthros and for dinos to force searching for preferred food items be animals or resource, prompts mobility 
	double preferenceThresholdForEvaluation = 0.0;//defaults to 0 but it is a parameter to include in run_params.json
	
	double totalPredatoryRiskEdibilityValue = 0.0;
	double totalEdibilityValue = 0.0;
	double totalConspecificBiomass = 0.0;

    const Animal* bestAnimal = nullptr;
    double bestAnimalEdibilityValue = 0.0;

    const ResourceInterface* bestResource = nullptr;
    double bestResourceEdibilityValue = 0.0;

    const std::vector<const AnimalSpecies*>* existingAnimalSpecies = &getMap().getWorld()->getExistingAnimalSpecies();

    vector<vector<double>> animalSpeciesPredatoryRiskEdibilityValue(existingAnimalSpecies->size());
    vector<vector<double>> animalSpeciesEdibilityValue(existingAnimalSpecies->size());
    vector<double> animalSpeciesConspecificBiomass;

    vector<double> resourceSpeciesEdibilityValue;

    for(const AnimalSpecies* animalSpecies : *existingAnimalSpecies)
    {
        animalSpeciesPredatoryRiskEdibilityValue[animalSpecies->getAnimalSpeciesId()].resize(animalSpecies->getNumberOfInstars(), 0.0);
        animalSpeciesEdibilityValue[animalSpecies->getAnimalSpeciesId()].resize(animalSpecies->getNumberOfInstars(), 0.0);
    }

    animalSpeciesConspecificBiomass.resize(animalWhoIsEvaluating->getSpecies()->getNumberOfInstars(), 0.0);

    resourceSpeciesEdibilityValue.resize(getMap().getWorld()->getExistingResourceSpecies().size(), 0.0);

    Instar instarCellEvaluation;

    if(animalWhoIsEvaluating->getLifeStage() == LifeStage::REPRODUCING)
    {
        instarCellEvaluation = Instar(1);
    }
    else
    {
        instarCellEvaluation = animalWhoIsEvaluating->getInstar();
    }

    SearchableEdibles searchableEdibles = getMutableEdiblesOnAllCell(evaluationArea.first, 
        animalWhoIsEvaluating->getPosition(), animalWhoIsEvaluating->getFinalTraitValue(Trait::Type::perception_area), 
        *evaluationArea.second, animalWhoIsEvaluating->getSpecies()->getCellEvaluationSearchParams(instarCellEvaluation)
    );

    auto searchableResources = move(searchableEdibles.first);
	auto searchableAnimals = move(searchableEdibles.second);

	auto partialCoverageAnimals = move(searchableAnimals.first);
	auto fullCoverageAnimals = move(searchableAnimals.second);

    for(auto &terrainCellAnimals : *fullCoverageAnimals)
	{
		for(auto &animalsVector : *terrainCellAnimals)
		{
			for(auto &animal : *animalsVector)
			{
                double predatoryRiskEdibilityValue, edibilityValue, conspecificBiomass;
                tie(predatoryRiskEdibilityValue, edibilityValue, conspecificBiomass) = evaluateAnimal(*animalWhoIsEvaluating, ediblesHasTriedToPredate, *static_cast<AnimalNonStatistical*>(animal), preferenceThresholdForEvaluation, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);

                animalSpeciesPredatoryRiskEdibilityValue[animal->getSpecies()->getAnimalSpeciesId()][animal->getInstar().getValue()] += predatoryRiskEdibilityValue;
                animalSpeciesEdibilityValue[animal->getSpecies()->getAnimalSpeciesId()][animal->getInstar().getValue()] += edibilityValue;
                if(animalWhoIsEvaluating->getSpecies() == animal->getSpecies())
                {
                    animalSpeciesConspecificBiomass[animal->getInstar().getValue()] += conspecificBiomass;
                }

                if(bestAnimalEdibilityValue < edibilityValue)
                {
                    bestAnimalEdibilityValue = edibilityValue;
                    bestAnimal = animal;
                }
            }
        }
    }

    for(auto &terrainCellAnimals : *partialCoverageAnimals)
	{
		for(auto &animal : *terrainCellAnimals)
		{
            double predatoryRiskEdibilityValue, edibilityValue, conspecificBiomass;
            tie(predatoryRiskEdibilityValue, edibilityValue, conspecificBiomass) = evaluateAnimal(*animalWhoIsEvaluating, ediblesHasTriedToPredate, *static_cast<AnimalNonStatistical*>(animal), preferenceThresholdForEvaluation, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);

            animalSpeciesPredatoryRiskEdibilityValue[animal->getSpecies()->getAnimalSpeciesId()][animal->getInstar().getValue()] += predatoryRiskEdibilityValue;
            animalSpeciesEdibilityValue[animal->getSpecies()->getAnimalSpeciesId()][animal->getInstar().getValue()] += edibilityValue;
            if(animalWhoIsEvaluating->getSpecies() == animal->getSpecies())
            {
                animalSpeciesConspecificBiomass[animal->getInstar().getValue()] += conspecificBiomass;
            }

            if(bestAnimalEdibilityValue < edibilityValue)
            {
                bestAnimalEdibilityValue = edibilityValue;
                bestAnimal = animal;
            }
        }
    }

    for(auto& [resource, dryMass] : *searchableResources)
	{
        double edibilityValue = evaluateResource(*animalWhoIsEvaluating, ediblesHasTriedToPredate, resource, dryMass, preferenceThresholdForEvaluation, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
    
        resourceSpeciesEdibilityValue[resource->getSpecies()->getResourceSpeciesId()] += edibilityValue;

        if(bestResourceEdibilityValue < edibilityValue)
        {
            bestResourceEdibilityValue = edibilityValue;
            bestResource = resource;
        }
    }

    for(const AnimalSpecies* animalSpecies : *existingAnimalSpecies)
    {
        for(unsigned int instarValue = 1; instarValue <= animalSpecies->getNumberOfInstars(); instarValue++)
        {
            Instar instar(instarValue);

            if(animalSpeciesPredatoryRiskEdibilityValue[animalSpecies->getAnimalSpeciesId()][instar.getValue()] > 0.0)
            {
                double maximumPredatoryRiskEdibilityValue = animalSpecies->getInstarK_Value(instar, getSize()) / (animalWhoIsEvaluating->calculateDryMass() * exp(1.0));

                totalPredatoryRiskEdibilityValue += MathFunctions::linearInterpolate01(animalSpeciesPredatoryRiskEdibilityValue[animalSpecies->getAnimalSpeciesId()][instar.getValue()], maximumPredatoryRiskEdibilityValue);
            }

            if(animalSpeciesEdibilityValue[animalSpecies->getAnimalSpeciesId()][instar.getValue()] > 0.0)
            {
                double maximumEdibilityValue = animalSpecies->getInstarK_Value(instar, getSize()) / animalSpecies->getMaximumDryMassObserved();

                totalEdibilityValue += MathFunctions::linearInterpolate01(animalSpeciesEdibilityValue[animalSpecies->getAnimalSpeciesId()][instar.getValue()], maximumEdibilityValue);
            }
        } 
    }

    for(const double &resourceEdibilityValue : resourceSpeciesEdibilityValue)
    {
        totalEdibilityValue += resourceEdibilityValue;
    }
    
    for(unsigned int instarValue = 1; instarValue <= animalWhoIsEvaluating->getSpecies()->getNumberOfInstars(); instarValue++)
    {
        Instar instar(instarValue);

        if(animalSpeciesConspecificBiomass[instar.getValue()] > 0.0)
        {
            double maximumConspecificBiomass = animalWhoIsEvaluating->getSpecies()->getInstarK_Value(instar, getSize()) / animalWhoIsEvaluating->getSpecies()->getMaximumDryMassObserved();
            totalConspecificBiomass += MathFunctions::linearInterpolate01(animalSpeciesConspecificBiomass[instar.getValue()], maximumConspecificBiomass);
        }
    }


    if(animalWhoIsEvaluating->getSpecies()->getNumberOfLinksAsPrey() > 0)
    {
        totalPredatoryRiskEdibilityValue /= animalWhoIsEvaluating->getSpecies()->getNumberOfLinksAsPrey();
    }

    if(animalWhoIsEvaluating->getSpecies()->getNumberOfLinksAsPredator() > 0)
    {
        totalEdibilityValue /= animalWhoIsEvaluating->getSpecies()->getNumberOfLinksAsPredator();
    }

    totalConspecificBiomass /= animalWhoIsEvaluating->getSpecies()->getNumberOfInstars();


    const EdibleInterface* bestEdible;

    // Priority is given to animals over resources
    if(bestAnimal != nullptr)
    {
        bestEdible = bestAnimal;
    }
    else
    {
        bestEdible = bestResource;
    }

	return make_tuple(totalEdibilityValue, totalPredatoryRiskEdibilityValue, totalConspecificBiomass, bestEdible);
}

unique_ptr<AnimalVector> TerrainCell::getMutableAnimalsBy(function<bool(Animal&)> areaChecker, 
        const AnimalSearchParams &animalSearchParams)
{
    auto searchableAnimals = make_unique<AnimalVector>();

    for(const LifeStage::LifeStageValue& lifeStage : animalSearchParams.getSearchableLifeStages())
    {
        for(const id_type& animalSpeciesId : animalSearchParams.getSearchableAnimalSpecies(lifeStage))
        {
            for(const Instar& instar : animalSearchParams.getSearchableInstars(lifeStage, animalSpeciesId))
            {
                for(const AnimalSpecies::Gender::GenderValue& gender : animalSearchParams.getSearchableGenders(lifeStage, animalSpeciesId, instar))
                {
                    for(auto &animal : animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender))
                    {
                        if(areaChecker(*animal))
                        {
                            searchableAnimals->push_back(animal);
                        }
                    }
                }
            }
        }
    }

    return searchableAnimals;
}

unique_ptr<vector<AnimalVector*>> TerrainCell::getMutableAnimalsBy(const AnimalSearchParams &animalSearchParams)
{
    auto searchableAnimals = make_unique<vector<AnimalVector*>>();

    for(const LifeStage::LifeStageValue& lifeStage : animalSearchParams.getSearchableLifeStages())
    {
        for(const id_type& animalSpeciesId : animalSearchParams.getSearchableAnimalSpecies(lifeStage))
        {
            for(const Instar& instar : animalSearchParams.getSearchableInstars(lifeStage, animalSpeciesId))
            {
                for(const AnimalSpecies::Gender::GenderValue& gender : animalSearchParams.getSearchableGenders(lifeStage, animalSpeciesId, instar))
                {
                    if(!animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender).empty())
                    {
                        searchableAnimals->push_back(&animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender));
                    }
                }
            }
        }
    }

    return searchableAnimals;
}

unique_ptr<const vector<AnimalVector*>> TerrainCell::getAnimalsBy(const AnimalSearchParams &animalSearchParams)
{
    unique_ptr<const vector<AnimalVector*>> searchableAnimals = getMutableAnimalsBy(animalSearchParams);

    return searchableAnimals;
}

unique_ptr<vector<double>> TerrainCell::getRadiusResourcesBy(
        const double &percent, const ResourceSearchParams &resourceSearchParams)
{
    auto searchableResources = make_unique<vector<double>>(ResourceSpecies::getResourceSpeciesCounter(), 0.0);

    for(const auto& resourceSpeciesId : resourceSearchParams.getSearchParams())
    {
        searchableResources->at(resourceSpeciesId) += getResource(resourceSpeciesId)->calculateDryMass() * percent;
    }

    return searchableResources;
}

const unsigned int TerrainCell::calculateManhattanDistanceToPoint(const PointMap &point) const
{
    unsigned int distance = 0;

    for(unsigned int i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();

        distance += abs(static_cast<int>(point.get(axis) - getPosition().get(axis)));
    }

    return distance;
}

void TerrainCell::dieFromBackground(const unsigned int numberOfTimeSteps)
{
    applyFunctionToAnimals(
        {
            [numberOfTimeSteps](AnimalNonStatistical& animal) { 
                animal.dieFromBackground(numberOfTimeSteps);
            }
        },
        getMap().getLifeStageSearchParams(LifeStage::ACTIVE)
    );
}

void TerrainCell::assimilateFoodMass()
{
    applyFunctionToAnimals(
        {
            [](AnimalNonStatistical& animal) { 
                animal.assimilateFoodMass();
            }
        },
        getMap().getLifeStageSearchParams(LifeStage::ACTIVE)
    );
}

void TerrainCell::metabolizeAnimals()
{
    applyFunctionToAnimals(
        {
            [](AnimalNonStatistical& animal) { 
                animal.metabolize();
            }
        },
        getMap().getLifeStageSearchParams(LifeStage::ACTIVE)
    );
}

void TerrainCell::growAnimals()
{
    applyFunctionToAnimals(
        {
            [](AnimalNonStatistical& animal) { 
                animal.getMutableAnimalGrowth()->grow();
            }
        },
        getMap().getLifeStageSearchParams(LifeStage::ACTIVE)
    );
}

ostream& TerrainCell::printAnimalsVoracities(ostream& os)
{
    applyFunctionToAnimals(
        {
            [&os](AnimalNonStatistical& animal) { 
                animal.printVoracities(os);
            }
        },
        getMap().getLifeStageSearchParams(LifeStage::ACTIVE)
    );
    
	return os;
}

void TerrainCell::printAnimals(ofstream &file) const
{
    if(!this->isObstacle())
    {
        for(const auto &lifeStage : *animals)
        {
            for(const auto &animalSpecies : lifeStage)
            {
                for(const auto &instar : animalSpecies)
                {
                    for(const auto &gender : instar)
                    {
                        for(const auto &animal : gender)
                        {
                            file << *animal << endl;
                        }
                    }
                }
            }
        }
    }
}

double TerrainCell::turnEdibleIntoDryMassToBeEaten(EdibleInterface &targetEdible, const double &targetDryMass, const Ring* const perceptionArea, const unsigned int numberOfTimeSteps, AnimalNonStatistical* predatorEdible, double leftovers)
{
	#ifdef DEBUG
	//Output::coutFlush("{} predated {}\n", targetEdible.getPredatedByID(), targetEdible.getId());
	#endif

	float targetEdibleProfitability = predatorEdible->getSpecies()->getEdibleProfitability(targetEdible.getSpecies()->getId(), predatorEdible->getInstar(), targetEdible.getInstar());

	//double dryMassToBeEaten = 0;

	double dryMassToBeEaten;
	

	if(targetEdible.getSpecies()->isMobile())
	{
        dryMassToBeEaten = targetDryMass;

		targetEdible.setNewLifeStage(LifeStage::PREDATED, numberOfTimeSteps, predatorEdible->getId());
	}
	else
	{
        dryMassToBeEaten = targetEdible.turnIntoDryMassToBeEaten(predatorEdible->getRemainingVoracity(), targetEdibleProfitability + predatorEdible->getFinalTraitValue(Trait::Type::assim), leftovers);
		
		//dryMassToBeEaten = targetEdible.turnIntoDryMassToBeEaten(predatorEdible->getRemainingVoracity(), targetEdibleProfitability + predatorEdible->getFinalTraitValue(Trait::Type::assim));
		//arthros and for dinos, very big bug in feeding on resource and updating ungi fixed
		/* cout << "BEFOREEEEEEEEEE AND AFTEEEEEEEERRRRRRRRRRRRRRRRRR" << endl;
		cout << targetEdible.calculateDryMass() << endl;
		cout << dryMassToBeEaten << endl; */
		targetEdible.substractBiomass(dryMassToBeEaten, perceptionArea);
		//cout << targetEdible.calculateDryMass() << endl;
	
	}

	if(dryMassToBeEaten < 0)
	{
		cout << "Edible id: " << predatorEdible->getId() << " (" << predatorEdible->getSpecies()->getScientificName() << ") - The dry mass to be eaten after considering profitability resulted in a negative value." << endl;
	}

	return dryMassToBeEaten;
}

SearchableAnimals TerrainCell::getAnimalsOnRadius(const PointContinuous &sourcePosition, const double &radius,
        const EdibleSearchParams &animalSearchParams)
{
    return getEdiblesOnRadius(sourcePosition, radius, animalSearchParams).second;
}

void TerrainCell::breedAnimals(const unsigned int numberOfTimeSteps, fs::path outputDirectory, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
    auto animalsToReproduce = getMutableAnimalsBy(getMap().getLifeStageSearchParams(LifeStage::REPRODUCING));

    for(auto &animalVector : *animalsToReproduce)
    {
        for(auto &female : *animalVector)
        {
            AnimalNonStatistical* femaleCast = static_cast<AnimalNonStatistical*>(female);

            if(femaleCast->isInBreedingZone())
            {
                auto offSprings = femaleCast->breed(numberOfTimeSteps, getMoistureInfo()->getTemperature(), saveAnimalConstitutiveTraits, constitutiveTraitsFile);

                ofstream geneticsFile;
                createOutputFile(geneticsFile, outputDirectory, "animal_genetics", "txt", ofstream::app);
                if(!geneticsFile.is_open()) {
                    cerr << "Error opening the file." << endl;
                } else {
                    for(auto &offSpring : *offSprings)
                    {
                        //offSpring->printGenetics(geneticsFile);
                    }
                    geneticsFile.close();
                }

                for(auto &offSpring : *offSprings)
                {
                    offSpring->setPosition(femaleCast->getPosition());
                    insertAnimal(offSpring);
                }

                femaleCast->setInBreedingZone(false);
            }
        }
    }
}

bool TerrainCell::canApplyObstaclePatch(const ObstaclePatch &obstaclePatch) const
{
    return obstaclePatch.getPriority() > this->getObstaclePatchPriority();
}


const int TerrainCell::getObstaclePatchPriority() const
{
    return obstaclePatchPriority;
}


bool TerrainCell::checkObstaclePatch(const ObstaclePatch &obstaclePatch)
{
    if(canApplyObstaclePatch(obstaclePatch))
    {
        return applyObstaclePatch(obstaclePatch);
    }

    return false;
}


bool TerrainCell::canApplyMoisturePatch(const MoisturePatch &moisturePatch) const
{
    return moisturePatch.getPriority() > getMoisturePatchPriority();
}


pair<bool, bool> TerrainCell::checkMoisturePatch(const MoisturePatch &moisturePatch)
{
    if(canApplyMoisturePatch(moisturePatch))
    {
        return applyMoisturePatch(moisturePatch);
    }

    return make_pair(false, false);
}


bool TerrainCell::canApplyResourcePatch(const ResourcePatch &resourcePatch) const
{
    return !this->isFullObstacle() && 
        resourcePatch.getPriority() > this->getResourcePatchPriority(resourcePatch.getSpecies()->getResourceSpeciesId());
}


bool TerrainCell::checkResourcePatch(const ResourcePatch &resourcePatch)
{
    if(canApplyResourcePatch(resourcePatch))
    {
        return applyResourcePatch(resourcePatch);
    }

    return false;
}


void TerrainCell::setResourceSourcePatch(const ResourcePatch &resourcePatch)
{
    auto resource = ResourceFactory::createInstance(
        resourcePatch.getSpecies(), this, resourcePatch.getInitialBiomassDensity() * pow(getSize(), DIMENSIONS), resourcePatch.getResourceMaximumCapacityDensity() * pow(getSize(), DIMENSIONS), 
        this->getMap().getWorld()->getMassRatio(), 
        resourcePatch.getPatchSpread()
    );

    setResourceSourcePatch(resource, resourcePatch.getPriority());
}


void TerrainCell::setResourceSourcePatch(unique_ptr<ResourceInterface> &newResource, const int newResourcePatchPriority)
{
    setResourcePatch(newResource->getSpecies()->getResourceSpeciesId(), newResource, newResourcePatchPriority);
}


void TerrainCell::setResourcePatch(const unsigned int resourceSpeciesId, unique_ptr<ResourceInterface> &newResource, const int newResourcePatchPriority)
{
    setResource(resourceSpeciesId, newResource);
    setResourcePatchPriority(resourceSpeciesId, newResourcePatchPriority);
}


void TerrainCell::setResource(const unsigned int resourceSpeciesId, unique_ptr<ResourceInterface> &newResource)
{
    resources[resourceSpeciesId] = move(newResource);
    resourcesPointers[resourceSpeciesId] = resources[resourceSpeciesId].get();
    resourcesConstPointers[resourceSpeciesId] = resources[resourceSpeciesId].get();
}


void TerrainCell::setMoistureSourcePatch(const MoisturePatch &moisturePatch)
{
    setMoistureSourcePatch(moisturePatch.getMoistureInfo().get(), moisturePatch.getPriority());
}


void TerrainCell::setMoistureSourcePatch(MoistureInterface* const &newMoistureInfo, const int newMoisturePatchPriority)
{
    setMoisturePatch(newMoistureInfo, newMoisturePatchPriority, true, true);
}


void TerrainCell::setMoisturePatch(MoistureInterface* const &newMoistureInfo, const int newMoisturePatchPriority, const bool newMoistureSourceValue, const bool newInMoisturePatchValue)
{
    setMoistureInfo(newMoistureInfo);
    setMoisturePatchPriority(newMoisturePatchPriority);
    setMoistureSource(newMoistureSourceValue);
    setInMoisturePatch(newInMoisturePatchValue);

    updateTotalMaximumResourceCapacity();
}


void TerrainCell::setMoistureInfo(MoistureInterface* const &newMoistureInfo)
{
    if(!isMoistureSource())
    {
        delete moistureInfo;
    }

    moistureInfo = newMoistureInfo;
}


void TerrainCell::setMoisturePatchPriority(const int newMoisturePatchPriority)
{
    moisturePatchPriority = newMoisturePatchPriority;
}


void TerrainCell::setMoistureSource(const bool newMoistureSourceValue)
{
    moistureSource = newMoistureSourceValue;
}


void TerrainCell::setInMoisturePatch(const bool newInMoisturePatchValue)
{
    inMoisturePatch = newInMoisturePatchValue;
}


bool TerrainCell::isMoistureSource() const
{
    return moistureSource;
}


bool TerrainCell::isInMoisturePatch() const
{
    return inMoisturePatch;
}


void TerrainCell::obtainWorldAnimalsPopulation(vector<vector<unsigned int>> &worldAnimalsPopulation)
{
    if(!isObstacle())
    {
        for(const auto &animalSpecies : getWorld()->getExistingAnimalSpecies())
        {
            auto population = getAnimalsBy(animalSpecies->getPopulationSearchParams());

            for(const auto &elem : *population)
            {
                worldAnimalsPopulation[animalSpecies->getAnimalSpeciesId()][elem->back()->getLifeStage()] += elem->size();
            }
        }
    }
}


void TerrainCell::obtainAnimalsPopulationAndGeneticsFrequencies(vector<vector<unsigned int>> &worldAnimalsPopulation, vector<vector<pair<vector<double>, vector<double>>>> &worldGeneticsFrequencies)
{
    if(!this->isObstacle())
    {
        for(const auto &animalSpecies : getWorld()->getExistingAnimalSpecies())
        {
            auto population = getAnimalsBy(animalSpecies->getPopulationSearchParams());

            for(const auto &elem : *population)
            {
                worldAnimalsPopulation[animalSpecies->getAnimalSpeciesId()][elem->back()->getLifeStage()] += elem->size();

                for(const auto &animal : *elem)
                {
                    Trait::Type traitType;
                    TraitDefinitionSection::Elements elementType;

                    for(unsigned int individualLevelTraitPos = 0; individualLevelTraitPos < animalSpecies->getNumberOfIndividualLevelTraits(); individualLevelTraitPos++)
                    {
                        tie(traitType, elementType) = animalSpecies->getIndividualLevelTraitElements().at(individualLevelTraitPos);

                        unsigned int traitOrder = static_cast<IndividualLevelTrait*>(animalSpecies->getTrait(traitType).getElements().at(elementType))->getOrder();
                        auto homologousCorrelosome = &animal->getGenome().getHomologousCorrelosomes().at(traitOrder);

                        for(unsigned int selectedLoci = 0; selectedLoci < animalSpecies->getNumberOfLociPerTrait(); ++selectedLoci)
                        {
                            worldGeneticsFrequencies[animalSpecies->getAnimalSpeciesId()][individualLevelTraitPos].first.push_back(homologousCorrelosome->first->getAllele(selectedLoci)->getValue());
                            worldGeneticsFrequencies[animalSpecies->getAnimalSpeciesId()][individualLevelTraitPos].second.push_back(homologousCorrelosome->second->getAllele(selectedLoci)->getValue());
                        }
                    }
                }
            }
        }
    }
}


void TerrainCell::saveAnimalSpeciesSnapshot(ofstream &file, const AnimalSpecies* const &species)
{
    unsigned int value = 0;

    if(!this->isObstacle())
    {
        auto population = getAnimalsBy(species->getLifeStageSearchParams(LifeStage::ACTIVE));

        for(const auto &elem : *population)
        {
            value += elem->size();
        }
    }

    file.write((char *) &value, sizeof(unsigned int));
}


void TerrainCell::saveResourceSpeciesSnapshot(ofstream &file, const ResourceSpecies* const &species) const
{
    double value = 0.0;

    if(!this->isObstacle())
    {
        value = getBiomass(species->getResourceSpeciesId());
    }

    file.write((char *) &value, sizeof(double));
}


void TerrainCell::saveWaterSnapshot(ofstream &file) const
{
    if(!this->isObstacle())
    {
        file.write((char *) &getMoistureInfo()->getMoisture(), sizeof(double));
    }
}

unsigned int TerrainCell::getNumberOfMatureFemales(AnimalSpecies *const animalSpecies)
{
    unsigned int counter = 0;

    auto fullCoverageAnimals = getAnimalsOnAllCell(animalSpecies->getMatureFemalesSearchParams());

    for(auto &terrainCellAnimals : *fullCoverageAnimals)
	{
		for(auto &animalsVector : *terrainCellAnimals)
		{
            counter += animalsVector->size();
        }
    }

    return counter;
}

pair<unique_ptr<ResourcesOnRadius>, unique_ptr<vector<AnimalVector*>>> TerrainCell::getMutableEdiblesBy(
        const EdibleSearchParams &edibleSearchParams)
{
    return make_pair(
        getResourcesBy(edibleSearchParams.getResourceSearchParams()), 
        getMutableAnimalsBy(edibleSearchParams.getAnimalSearchParams())
    );
}

pair<unique_ptr<ResourcesOnRadius>, unique_ptr<AnimalVector>> TerrainCell::getMutableEdiblesBy(
        function<bool(Animal&)> areaChecker, const Ring &effectiveArea, const EdibleSearchParams &edibleSearchParams)
{
    return make_pair(
        getResourcesBy(effectiveArea, edibleSearchParams.getResourceSearchParams()), 
        getMutableAnimalsBy(areaChecker, edibleSearchParams.getAnimalSearchParams())
    );
}

unique_ptr<ResourcesOnRadius> TerrainCell::getResourcesBy(const ResourceSearchParams &resourceSearchParams)
{
    auto resourcesOnCell = make_unique<ResourcesOnRadius>(resourceSearchParams.getSearchParams().size(), 0.0);

    auto it = resourceSearchParams.getSearchParams().begin();
    for(unsigned int i = 0; i < resourceSearchParams.getSearchParams().size(); i++, ++it)
    {
        if(resourceIsPresent(*it))
        {
            (*resourcesOnCell)[i] += getResource(*it)->calculateDryMass();
        }
    }

    return resourcesOnCell;
}

unique_ptr<ResourcesOnRadius> TerrainCell::getResourcesBy(const Ring &effectiveArea, const ResourceSearchParams &resourceSearchParams)
{
    auto resourcesOnCell = make_unique<ResourcesOnRadius>(resourceSearchParams.getSearchParams().size(), 0.0);

    auto it = resourceSearchParams.getSearchParams().begin();
    for(unsigned int i = 0; i < resourceSearchParams.getSearchParams().size(); i++, ++it)
    {
        if(resourceIsPresent(*it))
        {
            (*resourcesOnCell)[i] += getResource(*it)->calculateDryMass() * Geometry::calculateCoveragePercent(&effectiveArea, getEffectiveArea());
        }
    }

    return resourcesOnCell;
}

void TerrainCell::performAnimalsActions(const unsigned int numberOfTimeSteps, ostream& voracitiesFile, fs::path outputFolder, bool saveAnimalConstitutiveTraits, ofstream &constitutiveTraitsFile)
{
    printAnimalsVoracities(voracitiesFile);
    dieFromBackground(numberOfTimeSteps);
    assimilateFoodMass();
    metabolizeAnimals();
    growAnimals();
    breedAnimals(numberOfTimeSteps, outputFolder, saveAnimalConstitutiveTraits, constitutiveTraitsFile);
}

template <class Archive>
void TerrainCell::serialize(Archive &ar, const unsigned int version) {

    ar & position;
    ar & center;
    ar & size;
    ar & map;
    ar & effectiveArea;
    ar & copied;
    ar & totalMaximumResourceCapacity;
    ar & obstacle;
    ar & fullObstacle;
    ar & moistureInfo;
    ar & moistureSource;
    ar & inMoisturePatch;
    ar & resources;
    ar & resourcesPointers;
    ar & resourcesConstPointers;
    ar & animals; 
    ar & obstaclePatchPriority;
    ar & moisturePatchPriority;
    ar & resourcePatchPriority;
}
