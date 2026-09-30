
#include "App/Model/IBM/Landscape/Map/SpatialTree/LivingBeings/Animals/SpatialTreeAnimal.h"

#include "App/Model/IBM/Landscape/Map/TerrainCells/TerrainCell.h"
#include "App/Model/IBM/Landscape/Map/Map.h"
#include "App/Model/IBM/Landscape/Landscape.h"

using namespace std;




SpatialTreeAnimal::SpatialTreeAnimal()
	: AnimalNonStatistical()
{

}

SpatialTreeAnimal::SpatialTreeAnimal(const Instar &instar, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
	: SpatialTreeAnimal(instar, mySpecies, terrainCell, nullptr, actualTimeStep, timeStepsPerDay)
{
	
}

SpatialTreeAnimal::SpatialTreeAnimal(const Instar &instar, AnimalSpecies* const mySpecies, TerrainCell* terrainCell, const Genome* const genome, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
	: AnimalNonStatistical(instar, mySpecies, terrainCell, genome, actualTimeStep, timeStepsPerDay)
{
	
}


SpatialTreeAnimal::SpatialTreeAnimal(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& factorEggMassFromMom, const Generation& g_numb_prt_female,
		const Generation& g_numb_prt_male, id_type ID_prt_female, id_type ID_prt_male, AnimalSpecies* const mySpecies, Gender genderValue, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
	: AnimalNonStatistical(firstParentGamete, secondParentGamete, parentTerrainCell, factorEggMassFromMom, g_numb_prt_female, g_numb_prt_male, ID_prt_female, ID_prt_male, mySpecies, genderValue, actualTimeStep, timeStepsPerDay)
{
	
}


SpatialTreeAnimal::~SpatialTreeAnimal()
{

}

AnimalNonStatistical* SpatialTreeAnimal::createOffspring(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCell* parentTerrainCell, const PreciseDouble& factorEggMassFromMom, const Generation& g_numb_prt_female,
			const Generation& g_numb_prt_male, id_type ID_prt_female, id_type ID_prt_male, AnimalSpecies* const parentSpecies, Gender genderValue, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
    return new SpatialTreeAnimal(firstParentGamete, secondParentGamete, parentTerrainCell, factorEggMassFromMom, g_numb_prt_female, g_numb_prt_male, ID_prt_female, ID_prt_male, parentSpecies, genderValue, actualTimeStep, timeStepsPerDay);
}

bool SpatialTreeAnimal::searchTargetToTravelTo(const PreciseDouble &scopeArea, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchEdibilityValueGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchPredationRiskGlobal, CustomIndexedVector<Instar, PreciseDouble>& maximumPatchConspecificBiomassGlobal)
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
        return false;
    }

    size_t randomIndex;

    if(bestEvaluations.size() > 1)
    {
        randomIndex = Random::randomIndex(bestEvaluations.size());
    }
    else
    {
        randomIndex = 0;
    }


    bool bestEdibleIsAnimal = false;

    if(bestEvaluations[randomIndex].bestEdibility != nullptr)
    {
        if(bestEvaluations[randomIndex].bestEdibility->getSpecies()->isMobile())
        {
            bestEdibleIsAnimal = true;
        }
    }

    PointContinuous targetPoint;

    if(bestEdibleIsAnimal)
    {
        targetPoint = static_cast<const AnimalNonStatistical*>(bestEvaluations[randomIndex].bestEdibility)->getPosition();   
    }
    else
    {
        if(bestEvaluations[randomIndex].fullCoverage)
        {
            targetPoint = Geometry::generateRandomPointOnBox(*bestEvaluations[randomIndex].cellEffectiveArea);
        }
        else
        {
            if(bestEvaluations[randomIndex].bestEdibility == nullptr) {
                targetPoint = Geometry::generateRandomPointOnBox(*bestEvaluations[randomIndex].cellEffectiveArea);
            }
            else {
                targetPoint = Geometry::generateRandomPointOnPolygon(
                    Geometry::calculateIntersection(
                        Geometry::makeSphere(getPosition(), scopeArea), 
                        *bestEvaluations[randomIndex].cellEffectiveArea
                    )
                );
            }
        }
    }

    setTargetNeighborToTravelTo(make_pair(*bestEvaluations[randomIndex].cellPosition, targetPoint));

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

    bool atDestinationTemp;
    pair<TerrainCell*, PointContinuous> cellToMoveTo;
	tie(atDestinationTemp, cellToMoveTo) = getMutableTerrainCell()->getCellByBearing(landscape->getMutableMap(), getTargetNeighborToTravelTo(), getPosition(), getSearchAreaRadius() - distanceTravelled);

    setAtDestination(atDestinationTemp);

	PreciseDouble distanceToAdd = Geometry::calculateDistanceBetweenPoints(getPosition(), cellToMoveTo.second);

    increaseDistanceTravelled(distanceToAdd);

	if(getMutableTerrainCell() != cellToMoveTo.first)
    {
        getMutableTerrainCell()->migrateAnimalTo(landscape, this, cellToMoveTo.first, cellToMoveTo.second);
    }
    else
    {
        setPosition(cellToMoveTo.second);
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
