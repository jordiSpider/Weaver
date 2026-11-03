
#include "App/Model/IBM/Landscape/Map/SpatialTree/LivingBeings/Animals/SpatialTreeAnimal.h"

#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"
#include "App/Model/IBM/Landscape/Map/Map.h"
#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree.h"
#include "App/Model/IBM/Landscape/Map/Navigation/DestinationSelectionPolicy.h"
#include "App/Model/IBM/Landscape/Map/Navigation/RouteCostPolicy.h"
#include "App/Model/IBM/Landscape/Map/Navigation/RoutePlanner.h"
#include "App/Model/IBM/Landscape/Map/Navigation/RouteFollowingPolicy.h"
#include "App/Model/IBM/Landscape/Map/Geometry/Geometry.h"

#include <unordered_set>
#include <utility>

using namespace std;




SpatialTreeAnimal::SpatialTreeAnimal()
	: AnimalNonStatistical()
{
    initializeNavigationPolicies();
}

SpatialTreeAnimal::SpatialTreeAnimal(const Instar &instar, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
	: SpatialTreeAnimal(instar, mySpecies, terrainCell, nullptr, actualTimeStep, timeStepsPerDay)
{
	
}

SpatialTreeAnimal::SpatialTreeAnimal(const Instar &instar, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, const Genome* const genome, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
	: AnimalNonStatistical(instar, mySpecies, terrainCell, genome, actualTimeStep, timeStepsPerDay)
{
    initializeNavigationPolicies();
}


SpatialTreeAnimal::SpatialTreeAnimal(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& factorEggMassFromMom, const Generation& g_numb_prt_female,
		const Generation& g_numb_prt_male, id_type ID_prt_female, id_type ID_prt_male, AnimalSpecies* const mySpecies, Gender genderValue, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
	: AnimalNonStatistical(firstParentGamete, secondParentGamete, parentTerrainCell, factorEggMassFromMom, g_numb_prt_female, g_numb_prt_male, ID_prt_female, ID_prt_male, mySpecies, genderValue, actualTimeStep, timeStepsPerDay)
{
    initializeNavigationPolicies();
}


SpatialTreeAnimal::~SpatialTreeAnimal()
{

}

void SpatialTreeAnimal::initializeNavigationPolicies()
{
    destinationSelectionPolicy = make_shared<RandomizedCandidateSelectionPolicy>();
    routeCostPolicy = make_shared<EuclideanRouteCostPolicy>();
    routeDestinationSelectionPolicy = make_shared<CandidatePriorityRouteSelectionPolicy>();
    routePlanner = make_shared<AStarRoutePlanner>();
    routeFollowingPolicy = make_shared<WaypointRouteFollowingPolicy>();
    navigationRoute.waypoints.clear();
    nextRouteCellIndex = 1u;
}

void SpatialTreeAnimal::setRouteDestinationSelectionPolicy(shared_ptr<const RouteDestinationSelectionPolicy> policy)
{
    if (policy)
    {
        routeDestinationSelectionPolicy = std::move(policy);
        navigationRoute.waypoints.clear();
        nextRouteCellIndex = 1u;
    }
}

void SpatialTreeAnimal::setDestinationSelectionPolicy(shared_ptr<const DestinationSelectionPolicy> policy)
{
    if (policy)
    {
        destinationSelectionPolicy = std::move(policy);
        navigationRoute.waypoints.clear();
        nextRouteCellIndex = 1u;
    }
}

void SpatialTreeAnimal::setRouteCostPolicy(shared_ptr<const RouteCostPolicy> policy)
{
    if (policy)
    {
        routeCostPolicy = std::move(policy);
        navigationRoute.waypoints.clear();
        nextRouteCellIndex = 1u;
    }
}

void SpatialTreeAnimal::setRoutePlanner(shared_ptr<const RoutePlanner> planner)
{
    if (planner)
    {
        routePlanner = std::move(planner);
        navigationRoute.waypoints.clear();
        nextRouteCellIndex = 1u;
    }
}

void SpatialTreeAnimal::setRouteFollowingPolicy(shared_ptr<const RouteFollowingPolicy> policy)
{
    if (policy)
    {
        routeFollowingPolicy = std::move(policy);
        navigationRoute.waypoints.clear();
        nextRouteCellIndex = 1u;
    }
}

AnimalNonStatistical* SpatialTreeAnimal::createOffspring(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& factorEggMassFromMom, const Generation& g_numb_prt_female,
			const Generation& g_numb_prt_male, id_type ID_prt_female, id_type ID_prt_male, AnimalSpecies* const parentSpecies, Gender genderValue, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
    return new SpatialTreeAnimal(firstParentGamete, secondParentGamete, parentTerrainCell, factorEggMassFromMom, g_numb_prt_female, g_numb_prt_male, ID_prt_female, ID_prt_male, parentSpecies, genderValue, actualTimeStep, timeStepsPerDay);
}

bool SpatialTreeAnimal::searchTargetToTravelTo(Landscape* const landscape, const PreciseDouble &scopeArea, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchEdibilityValueGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchPredationRiskGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchConspecificBiomassGlobal)
{
    bool withoutDestinations = false;
    PointContinuous lastDestination = getPosition();


    decisions.setNewDestination();

    size_t searchDepth = static_cast<const PointSpatialTree&>(getMutableTerrainCell()->getPosition()).getDepth();

    bool searchNeighborsWithFemales = (getGender() == Gender::MALE && getGrowthBuildingBlock().isMature());

    bool searchNeighborsWithMales = (getGender() == Gender::FEMALE && getGrowthBuildingBlock().isMature() && !isMated());

    vector<CellValue> bestEvaluations;

    if (scopeArea > 0.0) {
        getMutableTerrainCell()->getNeighboursCellsOnRadius(bestEvaluations, getPosition(), scopeArea, searchDepth, searchNeighborsWithFemales, searchNeighborsWithMales, this, maximumPatchEdibilityValueGlobal, maximumPatchPredationRiskGlobal, maximumPatchConspecificBiomassGlobal);
    
        if (bestEvaluations.empty())
        {
            throwLineInfoException("bestEvaluations is empty");
        }
    }

    if(bestEvaluations.empty())
    {
        navigationRoute.waypoints.clear();
        nextRouteCellIndex = 1u;
        setAtDestination(true);
        return true;
    }


    for (auto& elem : bestEvaluations) {
        elem.calculateValuePerceived(*this);
    }


    const vector<size_t> candidateOrder = destinationSelectionPolicy->rankCandidates(bestEvaluations);
    SpatialTree* const map = static_cast<SpatialTree*>(landscape->getMutableMap());
    SpatialTreeTerrainCell* const startCell = static_cast<SpatialTreeTerrainCell*>(getMutableTerrainCell());
    const PreciseDouble animalBodySection = getSpecies()->getPoreUsePerInstar(getInstar());
    vector<RouteGoal> routeGoals;
    unordered_set<const TerrainCell*> goalCells;
    constexpr size_t maxDestinationSamples = 8u;
    constexpr size_t maxPolygonSamplingAttempts = 16u;

    for (const size_t candidateIndex : candidateOrder)
    {
        if (candidateIndex >= bestEvaluations.size())
        {
            continue;
        }

        const CellValue& candidate = bestEvaluations[candidateIndex];
        RingModel targetArea;
        if (candidate.fullCoverage)
        {
            targetArea = *candidate.cellEffectiveArea;
        }
        else
        {
            targetArea = Geometry::calculateIntersection(
                Geometry::makeSphere(getPosition(), scopeArea),
                *candidate.cellEffectiveArea);
        }

        if (targetArea.empty())
        {
            continue;
        }
        const BoxModel targetAreaBounds = boost::geometry::return_envelope<BoxModel>(targetArea);

        bool foundGoalForCell = false;
        for (size_t sampleIndex = 0u; sampleIndex < maxDestinationSamples && !foundGoalForCell; ++sampleIndex)
        {
            PointContinuous targetPoint;
            if (candidate.fullCoverage)
            {
                targetPoint = Geometry::generateRandomPointOnBox(targetArea);
            }
            else
            {
                bool foundPointInArea = false;
                for (size_t attempt = 0u; attempt < maxPolygonSamplingAttempts; ++attempt)
                {
                    targetPoint = PointContinuous(
                        Random::randomUniform(boost::geometry::get<0>(targetAreaBounds.min_corner()), boost::geometry::get<0>(targetAreaBounds.max_corner())).getValue(),
                        Random::randomUniform(boost::geometry::get<1>(targetAreaBounds.min_corner()), boost::geometry::get<1>(targetAreaBounds.max_corner())).getValue());
                    if (Geometry::withinPolygon(targetPoint, targetArea))
                    {
                        foundPointInArea = true;
                        break;
                    }
                }
                if (!foundPointInArea)
                {
                    break;
                }
            }

            if (!map->isTraversable(targetPoint, animalBodySection))
            {
                continue;
            }

            const TerrainCell* const goalCell = map->getCell(targetPoint);
            if (goalCell == nullptr)
            {
                continue;
            }
            if (goalCells.contains(goalCell))
            {
                foundGoalForCell = true;
                break;
            }

            routeGoals.push_back({goalCell, targetPoint, routeGoals.size()});
            goalCells.insert(goalCell);
            foundGoalForCell = true;
        }
    }

    const RoutePlanResult routePlan = routePlanner->planRouteToBestGoal(
        *map,
        *startCell,
        getPosition(),
        routeGoals,
        *this,
        animalBodySection,
        *routeCostPolicy,
        *routeDestinationSelectionPolicy);
    if (routePlan.route.waypoints.empty() || routePlan.goalIndex >= routeGoals.size())
    {
        navigationRoute.waypoints.clear();
        nextRouteCellIndex = 1u;
        setAtDestination(true);
        return true;
    }

    const RouteGoal& selectedGoal = routeGoals[routePlan.goalIndex];
    setTargetNeighborToTravelTo(make_pair(PointMap(selectedGoal.cell->getPosition()), selectedGoal.position));
    navigationRoute = routePlan.route;
    nextRouteCellIndex = 1u;
    setAtDestination(false);


    if(isSated() && getGrowthBuildingBlock().isMature() && getGender() == Gender::MALE)
    {
        bool samePoint = true;

        for(unsigned char axis = 0; axis < DIMENSIONS; axis++)
        {
            samePoint = samePoint && (getPositionAxisValue(lastDestination, axis) == getPositionAxisValue(getTargetNeighborToTravelTo().second, axis));
        }

        if(samePoint)
        {
            withoutDestinations = true;
            navigationRoute.waypoints.clear();
            nextRouteCellIndex = 1u;
            setAtDestination(true);
        }
    }


    return withoutDestinations;
}

void SpatialTreeAnimal::move(Landscape* const landscape, const TimeStep numberOfTimeSteps, const PreciseDouble& timeStepsPerDay, 
        const bool saveMovements, fmt::memory_buffer& movementsText, const bool saveActivity, 
        fmt::memory_buffer& activitiesText)
{
    removeCurrentPrey();

    if(saveMovements)
    {
        fmt::format_to(fmt::appender(movementsText), "{}\t{}\t{}\t{}",
            numberOfTimeSteps.getValue(), getId(), 
            getPositionAxisValue(getPosition(), 0).getValue(), getPositionAxisValue(getPosition(), 1).getValue()
        );
    }

    Day initialDay = Day(numberOfTimeSteps, timeStepsPerDay) + Day((distanceTravelled / getSearchAreaRadius()) * timeStepsPerDay);

    // Increase the number of movement attempts
	stepsAttempted++;

    SpatialTree* const map = static_cast<SpatialTree*>(landscape->getMutableMap());
    const PreciseDouble sectionSize = getSpecies()->getPoreUsePerInstar(getInstar());
    const PreciseDouble remainingMovement = getSearchAreaRadius() - distanceTravelled;
    RouteFollowingResult movement = routeFollowingPolicy->advance(
        navigationRoute,
        nextRouteCellIndex,
        *map,
        getPosition(),
        getTargetNeighborToTravelTo().second,
        remainingMovement,
        sectionSize
    );

    if (movement.routeBlocked)
    {
        TerrainCell* const startCell = getMutableTerrainCell();
        TerrainCell* const goalCell = map->getCell(getTargetNeighborToTravelTo().second);
        if (goalCell != nullptr)
        {
            navigationRoute = routePlanner->planRoute(*map, *startCell, *goalCell, getPosition(), getTargetNeighborToTravelTo().second, *this, sectionSize, *routeCostPolicy);
            nextRouteCellIndex = 1u;
            movement = routeFollowingPolicy->advance(
                navigationRoute,
                nextRouteCellIndex,
                *map,
                getPosition(),
                getTargetNeighborToTravelTo().second,
                remainingMovement,
                sectionSize
            );
        }

        if (movement.routeBlocked)
        {
            navigationRoute.waypoints.clear();
            nextRouteCellIndex = 1u;
            movement = {getPosition(), getMutableTerrainCell(), nextRouteCellIndex, true, false};
        }
    }

    setAtDestination(movement.atDestination);
    nextRouteCellIndex = movement.nextCellIndex;

    PreciseDouble distanceToAdd = Geometry::calculateDistanceBetweenPoints(getPosition(), movement.position);

    increaseDistanceTravelled(distanceToAdd);

    if(getMutableTerrainCell() != movement.terrainCell)
    {
        getMutableTerrainCell()->migrateAnimalTo(landscape, this, movement.terrainCell, movement.position);
    }
    else
    {
        setPosition(movement.position);
    }

    if(saveMovements)
    {
        fmt::format_to(fmt::appender(movementsText), "\t{}\t{}\t{}\t{}\t{}\n",
            getPositionAxisValue(getPosition(), 0).getValue(), getPositionAxisValue(getPosition(), 1).getValue(),
			getDistanceTravelled(), getSearchAreaRadius(), isExhausted()
        );
    }

    Day finalDay = Day(numberOfTimeSteps, timeStepsPerDay) + Day((distanceTravelled / getSearchAreaRadius()) * timeStepsPerDay);

    if(saveActivity)
    {
        fmt::format_to(fmt::appender(activitiesText), "{}\t{}\t{}\t{}\t{}\t{}\n",
            getId(), getSpecies()->getScientificName(), EnumClass<ActivityType>::to_string(ActivityType::MOVING),
            initialDay.getValue().getValue(), finalDay.getValue().getValue(), (finalDay - initialDay).getValue().getValue()
        );
    }


    if(isAtDestination() && getLifeStage() == LifeStage::REPRODUCING)
    {
        setInBreedingZone(true);
    }


    // if(getLifeStage() != LifeStage::PREDATED)
    // {
    //     if(checkStepCellLeaving())
    //     {
    //         searchTargetToTravelTo(getScopeAreaRadius(), withoutDestinations);

    //         if(!withoutDestinations)
    //         {
    //             moveOneStep(landscape, saveActivity, activityContent, saveMovements, movementsContent, actualTimeStep, timeStepsPerDay, edibilitiesContent, saveAnimalsEachDayPredationProbabilities, predationProbabilitiesContent, competitionAmongResourceSpecies, withoutDestinations);
    //         }
    //     }
    // } 
}



BOOST_CLASS_EXPORT(SpatialTreeAnimal)

template <class Archive>
void SpatialTreeAnimal::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<AnimalNonStatistical>(*this);
}

// Specialisation
template void SpatialTreeAnimal::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SpatialTreeAnimal::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SpatialTreeAnimal::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SpatialTreeAnimal::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
