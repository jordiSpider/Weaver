
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
    : position(position), size(size), hyperVolume(pow(size, DIMENSIONS)), center(makeCenter()), effectiveArea(makeEffectiveArea()),
      animals(new LifeStageVector(EnumClass<LifeStage>::size()))
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
    delete animals;

    delete effectiveArea;

    delete center;
}

void TerrainCell::addAnimalSpecies(const AnimalSpecies& animalSpecies, const AnimalSearchParams& allAnimalsSearchParams, const PreciseDouble& timeStepsPerDay)
{
    vector<pair<const AnimalSearchParams&, AnimalFunctions>> animalFunctions;

    animalFunctions.emplace_back(
        allAnimalsSearchParams,
        AnimalFunctions{
            PreviousAnimalFunctions{},
            IndividualFunctions{
                [&animalSpecies, &timeStepsPerDay](Animal& animal) { 
                    animal.addAnimalSpecies(animalSpecies.getGrowthBuildingBlock().getNumberOfInstars(), timeStepsPerDay);
                }
            },
            PostAnimalFunctions{}
        }
    );


    applyFunctionToAnimals(animalFunctions);


    for(auto &lifeStage : *animals)
    {
        lifeStage.push_back(InstarVector(animalSpecies.getGrowthBuildingBlock().getNumberOfInstars()));

        for(auto &instar : lifeStage.back())
        {
            instar.resize(EnumClass<Gender>::size());
        }
    }


    getMutablePatchApplicator().addAnimalSpecies(animalSpecies);
}

void TerrainCell::addResourceSpecies(Landscape* const landscape, vector<vector<vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource, const AnimalSearchParams& allAnimalsSearchParams, const PreciseDouble& timeStepsPerDay)
{
    vector<pair<const AnimalSearchParams&, AnimalFunctions>> animalFunctions;

    animalFunctions.emplace_back(
        allAnimalsSearchParams,
        AnimalFunctions{
            PreviousAnimalFunctions{},
            IndividualFunctions{
                [&resourceSpecies, &timeStepsPerDay](Animal& animal) { 
                    animal.addResourceSpecies(resourceSpecies.getGrowthBuildingBlock().getNumberOfInstars(), timeStepsPerDay);
                }
            },
            PostAnimalFunctions{}
        }
    );


    applyFunctionToAnimals(animalFunctions);
    

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

const LifeStageVector& TerrainCell::getAnimals() const
{
    return *animals;
}

LifeStageVector& TerrainCell::getMutableAnimals() const
{
    return *animals;
}




const RingModel& TerrainCell::getEffectiveArea() const
{
	return *effectiveArea;
}














void TerrainCell::insertAnimal(Landscape* const, Animal* const newAnimal)
{
    Animal* const animalCast = static_cast<Animal* const>(newAnimal);

    addAnimal(animalCast->getLifeStage(), animalCast->getSpecies()->getAnimalSpeciesId(), animalCast->getGrowthBuildingBlock().getInstar(), animalCast->getGender(), newAnimal);
}


tuple<bool, TerrainCell*, TerrainCell*, Animal*, size_t> TerrainCell::randomInsertAnimal(Landscape* const landscape, const Instar &instar, AnimalSpecies* animalSpecies, const bool isStatistical, const bool saveGenetics, const bool saveMassInfo, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
    Animal* newAnimal;
    size_t numberOfDiscardedIndividualsOutsideRestrictedRanges;
    
    if(isStatistical)
    {
        newAnimal = new AnimalStatistical(landscape, instar, animalSpecies, this, timeStepsPerDay);
    }
    else
    {
        tie(newAnimal, numberOfDiscardedIndividualsOutsideRestrictedRanges) = createAnimal(landscape, instar, animalSpecies, saveGenetics, saveMassInfo, actualTimeStep, timeStepsPerDay);
    }
    

    vector<PreciseDouble> axisValues(DIMENSIONS);
    
    for(unsigned char i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();
        axisValues[i] = static_cast<double>(getPosition().get(axis)) * getSize() + Random::randomUniform(0.0, getSize());
    }

    newAnimal->setPosition(axisValues);
    TerrainCell::insertAnimal(landscape, newAnimal);

    return make_tuple(false, nullptr, nullptr, newAnimal, numberOfDiscardedIndividualsOutsideRestrictedRanges);
}


const PointContinuous& TerrainCell::getCenter() const
{
    return *center;
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

void TerrainCell::addAnimal(const LifeStage &lifeStage, const size_t animalSpeciesId, const Instar &instar, const Gender &gender, Animal* const newAnimal)
{
    animals->at(lifeStage)[animalSpeciesId][instar][gender].push_back(newAnimal);

    newAnimal->setTerrainCell(this);
}

void TerrainCell::eraseAnimal(Animal* const animalToRemove)
{
    Animal* const animalToRemoveCast = static_cast<Animal* const>(animalToRemove);
    eraseAnimal(animalToRemoveCast->getLifeStage(), animalToRemoveCast->getSpecies()->getAnimalSpeciesId(), animalToRemoveCast->getGrowthBuildingBlock().getInstar(), animalToRemoveCast->getGender(), animalToRemove);
}

void TerrainCell::eraseAnimal(const LifeStage &lifeStage, const size_t animalSpeciesId, const Instar &instar, const Gender &gender, Animal* const animalToRemove)
{
    AnimalVector* vector = &animals->at(lifeStage)[animalSpeciesId][instar][gender];
    vector->erase(std::remove(vector->begin(), vector->end(), animalToRemove), vector->end());
}

void TerrainCell::changeAnimalToSenesced(Landscape* const landscape, Animal* targetAnimal, const TimeStep numberOfTimeSteps)
{
	Animal* targetAnimalCast = static_cast<Animal*>(targetAnimal);

	targetAnimalCast->setNewLifeStage(landscape, LifeStage::SENESCED, numberOfTimeSteps, landscape->getTimeStepsPerDay());
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


const RingModel* TerrainCell::makeEffectiveArea() const
{
    BoxModel* boxEffectiveArea = Geometry::makeBoxEffectiveArea(getPosition(), getSize());

    RingModel ringEffectiveArea;
	boost::geometry::convert(*boxEffectiveArea, ringEffectiveArea);

    delete boxEffectiveArea;
    
    return new RingModel(ringEffectiveArea);
}

void TerrainCell::update(const TimeStep& numberOfTimeSteps)
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

void TerrainCell::applyFunctionToResources(const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions)
{
    if(!resourceFunctions.empty())
    {
        if(!getPatchApplicator().getCellObstacle().isObstacle())
        {
            for(const auto& [resourceSearchParams, functions] : resourceFunctions)
            {
                for(const auto& resourceSpeciesId : resourceSearchParams.getSearchParams())
                {
                    for(const auto &func : functions)
                    {
                        func(getMutablePatchApplicator().getMutableCellResource(resourceSpeciesId), true, nullptr, 0.0, nullptr);
                    }
                }
            }
        }
    }
}

void TerrainCell::applyFunctionToResources(
        const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    )
{
    if(!resourceFunctions.empty())
    {
        if(!getPatchApplicator().getCellObstacle().isObstacle())
        {
            for(const auto& [resourceSearchParams, functions] : resourceFunctions)
            {
                for(const auto& resourceSpeciesId : resourceSearchParams.getSearchParams())
                {
                    for(const auto &func : functions)
                    {
                        func(getMutablePatchApplicator().getMutableCellResource(resourceSpeciesId), false, &sourcePosition, radius, radiusArea);
                    }
                }
            }
        }
    }
}


/****************/
/*    Animal    */
/****************/

void TerrainCell::applyFunctionToAnimals(const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions)
{
    if(!animalFunctions.empty())
    {
        if(!getPatchApplicator().getCellObstacle().isObstacle())
        {
            for(const auto& [animalSearchParams, functions] : animalFunctions)
            {
                const auto& [prevFunctions, individualFunctions, postFunctions] = functions;


                for(const LifeStage& lifeStage : animalSearchParams.getSearchableLifeStages())
                {
                    for(const AnimalSpeciesID& animalSpeciesId : animalSearchParams.getSearchableAnimalSpecies(lifeStage))
                    {
                        for(const Instar& instar : animalSearchParams.getSearchableInstars(lifeStage, animalSpeciesId))
                        {
                            for(const Gender& gender : animalSearchParams.getSearchableGenders(lifeStage, animalSpeciesId, instar))
                            {
                                for(const auto& func : prevFunctions)
                                {
                                    func(animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender));
                                }

                                for(auto& animal : animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender))
                                {
                                    for(const auto& func : individualFunctions)
                                    {
                                        func(*animal);
                                    }
                                }

                                for(const auto& func : postFunctions)
                                {
                                    func(animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender));
                                }
                            }
                        }
                    }
                }
            }
        }
    } 
}

void TerrainCell::applyFunctionToAnimals(
        function<bool(Animal&)> checker, 
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    )
{
    if(!animalFunctions.empty())
    {
        if(!getPatchApplicator().getCellObstacle().isObstacle())
        {
            for(const auto& [animalSearchParams, functions] : animalFunctions)
            {
                const auto& [prevFunctions, individualFunctions, postFunctions] = functions;


                for(const LifeStage& lifeStage : animalSearchParams.getSearchableLifeStages())
                {
                    for(const AnimalSpeciesID& animalSpeciesId : animalSearchParams.getSearchableAnimalSpecies(lifeStage))
                    {
                        for(const Instar& instar : animalSearchParams.getSearchableInstars(lifeStage, animalSpeciesId))
                        {
                            for(const Gender& gender : animalSearchParams.getSearchableGenders(lifeStage, animalSpeciesId, instar))
                            {
                                for(const auto& func : prevFunctions)
                                {
                                    func(animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender));
                                }

                                for(auto& animal : animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender))
                                {
                                    if(checker(*animal))
                                    {
                                        for(const auto& func : individualFunctions)
                                        {
                                            func(*animal);
                                        }
                                    }
                                }

                                for(const auto& func : postFunctions)
                                {
                                    func(animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


void TerrainCell::randomApplyFunctionToAnimals(const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions)
{
    if(!animalFunctions.empty())
    {
        if(!getPatchApplicator().getCellObstacle().isObstacle())
        {
            for(const auto& [animalSearchParams, functions] : animalFunctions)
            {
                const auto& [prevFunctions, individualFunctions, postFunctions] = functions;


                vector<LifeStage> shuffleSearchableLifeStages(animalSearchParams.getSearchableLifeStages());
                Random::shuffleVector(shuffleSearchableLifeStages);

                for(const LifeStage& lifeStage : shuffleSearchableLifeStages)
                {
                    vector<AnimalSpeciesID> shuffleSearchableAnimalSpecies(animalSearchParams.getSearchableAnimalSpecies(lifeStage));
                    Random::shuffleVector(shuffleSearchableAnimalSpecies);

                    for(const AnimalSpeciesID& animalSpeciesId : shuffleSearchableAnimalSpecies)
                    {
                        vector<Instar> shuffleSearchableInstars(animalSearchParams.getSearchableInstars(lifeStage, animalSpeciesId));
                        Random::shuffleVector(shuffleSearchableInstars);

                        for(const Instar& instar : shuffleSearchableInstars)
                        {
                            vector<Gender> shuffleSearchableGenders(animalSearchParams.getSearchableGenders(lifeStage, animalSpeciesId, instar));
                            Random::shuffleVector(shuffleSearchableGenders);

                            for(const Gender& gender : shuffleSearchableGenders)
                            {
                                for(const auto &func : prevFunctions)
                                {
                                    func(animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender));
                                }

                                vector<Animal*> shuffleAnimals(static_cast<const AnimalVector &>(animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender)));
                                Random::shuffleVector(shuffleAnimals);

                                for(auto& animal : shuffleAnimals)
                                {
                                    for(const auto &func : individualFunctions)
                                    {
                                        func(*animal);
                                    }
                                }

                                for(const auto &func : postFunctions)
                                {
                                    func(animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void TerrainCell::randomApplyFunctionToAnimals(
        function<bool(Animal&)> checker, 
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions
    )
{
    if(!animalFunctions.empty())
    {
        if(!getPatchApplicator().getCellObstacle().isObstacle())
        {
            for(const auto& [animalSearchParams, functions] : animalFunctions)
            {
                const auto& [prevFunctions, individualFunctions, postFunctions] = functions;


                vector<LifeStage> shuffleSearchableLifeStages(animalSearchParams.getSearchableLifeStages());
                Random::shuffleVector(shuffleSearchableLifeStages);

                for(const LifeStage& lifeStage : shuffleSearchableLifeStages)
                {
                    vector<AnimalSpeciesID> shuffleSearchableAnimalSpecies(animalSearchParams.getSearchableAnimalSpecies(lifeStage));
                    Random::shuffleVector(shuffleSearchableAnimalSpecies);

                    for(const AnimalSpeciesID& animalSpeciesId : shuffleSearchableAnimalSpecies)
                    {
                        vector<Instar> shuffleSearchableInstars(animalSearchParams.getSearchableInstars(lifeStage, animalSpeciesId));
                        Random::shuffleVector(shuffleSearchableInstars);

                        for(const Instar& instar : shuffleSearchableInstars)
                        {
                            vector<Gender> shuffleSearchableGenders(animalSearchParams.getSearchableGenders(lifeStage, animalSpeciesId, instar));
                            Random::shuffleVector(shuffleSearchableGenders);

                            for(const Gender& gender : shuffleSearchableGenders)
                            {
                                for(const auto &func : prevFunctions)
                                {
                                    func(animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender));
                                }

                                vector<Animal*> shuffleAnimals(static_cast<const AnimalVector &>(animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender)));
                                Random::shuffleVector(shuffleAnimals);

                                for(auto& animal : shuffleAnimals)
                                {
                                    if(checker(*static_cast<Animal*>(animal)))
                                    {
                                        for(const auto &func : individualFunctions)
                                        {
                                            func(*animal);
                                        }
                                    }
                                }

                                for(const auto &func : postFunctions)
                                {
                                    func(animals->at(lifeStage).at(animalSpeciesId).at(instar).at(gender));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


/****************/
/*    Edible    */
/****************/

void TerrainCell::applyFunctionToEdibles(
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    )
{
    applyFunctionToAnimals(animalFunctions);
    applyFunctionToResources(resourceFunctions);
}

void TerrainCell::applyFunctionToEdibles(
        function<bool(Animal&)> checker, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    )
{
    applyFunctionToAnimals(checker, animalFunctions);
    applyFunctionToResources(sourcePosition, radius, radiusArea, resourceFunctions);
}


/************************/
/*    Edible in Cell    */
/************************/

void TerrainCell::applyFunctionToEdiblesInCell(bool fullCoverage, 
    const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, 
    const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
    const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions)
{   
    if(fullCoverage)
    {
        applyFunctionToEdibles(animalFunctions, resourceFunctions);
    }
    else
    {
        auto radiusChecker = [&sourcePosition, &radius](Animal& animal) {
            return Geometry::pointInsideSphere(animal.getPosition(), sourcePosition, radius);
        };

        applyFunctionToEdibles(radiusChecker, sourcePosition, radius, radiusArea, animalFunctions, resourceFunctions);
    }
}


/**************************/
/*    Edible in Radius    */
/**************************/

void TerrainCell::applyFunctionToEdiblesInRadius(
        const PointContinuous &sourcePosition, const PreciseDouble &radius, 
        const vector<pair<const AnimalSearchParams&, AnimalFunctions>>& animalFunctions,
        const vector<pair<const ResourceSearchParams&, ResourceFunctions>>& resourceFunctions
    )
{   
    if(radius > 0.0)
    {
        RingModel* radiusArea = Geometry::makeSphere(sourcePosition, radius);

        applyFunctionToEdiblesInRadius(sourcePosition, radius, radiusArea, animalFunctions, resourceFunctions);
    
        delete radiusArea;
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


void TerrainCell::getCellEvaluation(vector<CellValue>& bestEvaluations, AnimalNonStatistical* animalWhoIsEvaluating, const PointContinuous &sourcePosition, const PreciseDouble &radius, const RingModel* const radiusArea, const bool searchNeighborsWithFemales, const bool parentFullCoverage, const list<const AnimalNonStatistical*> &animalsHasTriedToPredate)
{
    AnimalNonStatistical* animalWhoIsEvaluatingCast = static_cast<AnimalNonStatistical*>(animalWhoIsEvaluating);



    bool fullCoverage = parentFullCoverage;
    bool nullCoverage;

    if(fullCoverage)
    {
        nullCoverage = false;
    }
    else
    {
        switch(Geometry::checkCoveredLevelBySphere(&getEffectiveArea(), sourcePosition, radius)) {
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
            animalWhoIsEvaluatingCast->getSpecies()->getConspecificSearchParams(),
            AnimalFunctions{
                PreviousAnimalFunctions{},
                IndividualFunctions{
                    [&animalWhoIsEvaluatingCast, &totalConspecificBiomass](Animal& animal) {
                        totalConspecificBiomass += animalWhoIsEvaluatingCast->calculateConspecificBiomass(static_cast<AnimalNonStatistical&>(animal)).getValue();
                    }
                },
                PostAnimalFunctions{}
            }
        );

        animalFunctions.emplace_back(
            animalWhoIsEvaluatingCast->getSpecies()->getPreySearchParams(animalWhoIsEvaluatingCast->getInstarToEvaluateCells()).getAnimalSearchParams(),
            AnimalFunctions{
                PreviousAnimalFunctions{},
                IndividualFunctions{
                    [&animalWhoIsEvaluatingCast, &animalsHasTriedToPredate, &totalEdibilityValue, &bestAnimalEdibilityValue, &bestAnimal](Animal& animal) {
                        AnimalNonStatistical* animalCast = static_cast<AnimalNonStatistical*>(&animal);

                        const PreciseDouble edibilityValue = animalWhoIsEvaluatingCast->calculateCellQuality(*animalCast, animalsHasTriedToPredate);

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
            animalWhoIsEvaluatingCast->getSpecies()->getPredatorSearchParams(animalWhoIsEvaluatingCast->getInstarToEvaluateCells()),
            AnimalFunctions{
                PreviousAnimalFunctions{},
                IndividualFunctions{
                    [&animalWhoIsEvaluatingCast, &totalPredatoryRiskEdibilityValue](Animal& animal) {
                        totalPredatoryRiskEdibilityValue += animalWhoIsEvaluatingCast->calculatePredationRisk(static_cast<AnimalNonStatistical&>(animal));
                    }
                },
                PostAnimalFunctions{}
            }
        );


        vector<pair<const ResourceSearchParams&, ResourceFunctions>> resourceFunctions;

        resourceFunctions.emplace_back(
            animalWhoIsEvaluatingCast->getSpecies()->getPreySearchParams(animalWhoIsEvaluatingCast->getInstarToEvaluateCells()).getResourceSearchParams(),
            ResourceFunctions{
                [&animalWhoIsEvaluatingCast, &bestResourceEdibilityValue, &bestResource, &totalEdibilityValue](CellResourceInterface& resource, bool fullCoverage, const PointContinuous* const sourcePosition, const PreciseDouble &radius, const RingModel * const radiusArea) {
                    const DryMass dryMassAvailable = resource.calculateDryMassAvailable(fullCoverage, sourcePosition, radius, radiusArea);
                    
                    const PreciseDouble edibilityValue = animalWhoIsEvaluatingCast->calculateCellQuality(resource, dryMassAvailable);

                    totalEdibilityValue += edibilityValue;

                    if(bestResourceEdibilityValue < edibilityValue)
                    {
                        bestResourceEdibilityValue = edibilityValue;
                        bestResource = &resource;
                    }
                }
            }
        );


        applyFunctionToEdiblesInCell(fullCoverage, sourcePosition, radius, radiusArea, animalFunctions, resourceFunctions);


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

        if(searchNeighborsWithFemales)
        {
            numberOfFemales = getNumberOfMatureFemales(animalWhoIsEvaluatingCast->getMutableSpecies());
        }


        animalWhoIsEvaluatingCast->setMaximumCellEvaluation(totalEdibilityValue, totalPredatoryRiskEdibilityValue, totalConspecificBiomass);

        const PreciseDouble maximumPatchEdibilityValue = animalWhoIsEvaluatingCast->getMaximumPatchEdibilityValue();
        const PreciseDouble maximumPatchPredationRisk = animalWhoIsEvaluatingCast->getMaximumPatchPredationRisk();
        const PreciseDouble maximumPatchConspecificBiomass = animalWhoIsEvaluatingCast->getMaximumPatchConspecificBiomass();

        const PreciseDouble edibilityValueProbability = (maximumPatchEdibilityValue > 0.0) ? totalEdibilityValue / maximumPatchEdibilityValue : 0.0;
        const PreciseDouble predationRiskProbability = (maximumPatchPredationRisk > 0.0) ? totalPredatoryRiskEdibilityValue / maximumPatchPredationRisk : 0.0;
        const PreciseDouble conspecificBiomass = (maximumPatchConspecificBiomass > 0.0) ? totalConspecificBiomass / maximumPatchConspecificBiomass : 0.0;


        CellValue cellEvaluation(*animalWhoIsEvaluatingCast, bestEdible, 
            fullCoverage, &getEffectiveArea(), &getPosition(), getCenter(), 
            edibilityValueProbability, predationRiskProbability, conspecificBiomass, numberOfFemales, 
            getPatchApplicator().getCellHabitatDomain().isHabitatDomain(animalWhoIsEvaluatingCast->getSpecies()->getAnimalSpeciesId(), animalWhoIsEvaluatingCast->getInstarToEvaluateCells())
        );


        if(bestEvaluations.empty())
        {
            bestEvaluations.emplace_back(cellEvaluation);
        }
        else
        {
            if(searchNeighborsWithFemales)
            {
                if(cellEvaluation.numberOfFemales >= bestEvaluations.front().numberOfFemales)
                {
                    if(cellEvaluation.numberOfFemales > bestEvaluations.front().numberOfFemales)
                    {
                        bestEvaluations.clear();
                    }

                    bestEvaluations.emplace_back(cellEvaluation);
                }
            }
            else
            {
                if(cellEvaluation >= bestEvaluations.front())
                {
                    if(cellEvaluation > bestEvaluations.front())
                    {
                        bestEvaluations.clear();
                    }

                    bestEvaluations.emplace_back(cellEvaluation);
                }  
            }
        }
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

    for(AnimalSpeciesVector& animalSpeciesVector : *animals)
    {
        for(size_t i = 0; i < existingAnimalSpecies.size(); i++)
        {
            for(GenderVector& genderVector : animalSpeciesVector[AnimalSpeciesID(i)])
            {
                for(AnimalVector& animalVector : genderVector)
                {
                    for(Animal*& animal : animalVector)
                    {
                        animal->setSpecies(existingAnimalSpecies[i]);
                    }
                }
            }
        }
    }
}


void TerrainCell::registerAnimals(vector<AnimalNonStatistical*>& landscapeAnimals)
{
    for(AnimalSpeciesVector& animalSpeciesVector : *animals)
    {
        for(InstarVector& instarVector : animalSpeciesVector)
        {
            for(GenderVector& genderVector : instarVector)
            {
                for(AnimalVector& animalVector : genderVector)
                {
                    for(Animal*& animal : animalVector)
                    {
                        landscapeAnimals.push_back(static_cast<AnimalNonStatistical*>(animal));
                    }
                }
            }
        }
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

unsigned int TerrainCell::getNumberOfMatureFemales(AnimalSpecies *const animalSpecies)
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

    applyFunctionToAnimals(animalFunctions);

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
        for(AnimalSpeciesVector& animalSpeciesVector : *animals)
        {
            for(InstarVector& instarVector : animalSpeciesVector)
            {
                for(GenderVector& genderVector : instarVector)
                {
                    for(AnimalVector& animalVector : genderVector)
                    {
                        for(Animal*& animal : animalVector)
                        {
                            animal->setTerrainCell(this);
                        }
                    }
                }
            }
        }
    }
}

// // Specialisation
template void TerrainCell::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void TerrainCell::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void TerrainCell::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void TerrainCell::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
