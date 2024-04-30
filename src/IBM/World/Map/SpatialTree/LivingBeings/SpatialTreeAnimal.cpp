
#include "IBM/World/Map/SpatialTree/LivingBeings/SpatialTreeAnimal.h"

#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"

using namespace std;




SpatialTreeAnimal::SpatialTreeAnimal(double timeStep, const Instar &instar, int g_numb_prt1,
		int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, TerrainCellInterface* terrainCellInterface) 
	: AnimalNonStatistical(timeStep, instar, g_numb_prt1, g_numb_prt2, ID_prt1, ID_prt2, mySpecies, terrainCellInterface)
{
	
}


SpatialTreeAnimal::SpatialTreeAnimal(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCellInterface* parentTerrainCellInterface, double factorEggMassFromMom, double timeStep, int g_numb_prt1,
		int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender) 
	: AnimalNonStatistical(firstParentGamete, secondParentGamete, parentTerrainCellInterface, factorEggMassFromMom, timeStep, g_numb_prt1, g_numb_prt2, ID_prt1, ID_prt2, mySpecies, gender)
{
	
}


SpatialTreeAnimal::~SpatialTreeAnimal()
{

}

AnimalNonStatistical* SpatialTreeAnimal::createOffspring(Gamete* const firstParentGamete, Gamete* const secondParentGamete, TerrainCellInterface* parentTerrainCellInterface, double factorEggMassFromMom, double actualTime, int g_numb_prt1,
			int g_numb_prt2, int ID_prt1, int ID_prt2, AnimalSpecies* const mySpecies, AnimalSpecies::Gender::GenderValue gender)
{
    return new SpatialTreeAnimal(firstParentGamete, secondParentGamete, parentTerrainCellInterface, factorEggMassFromMom, actualTime, g_numb_prt1, g_numb_prt2, ID_prt1, ID_prt2, mySpecies, gender);
}

void SpatialTreeAnimal::searchTargetToTravelTo(const double &perceptionArea, const std::list<const EdibleInterface*> &ediblesHasTriedToPredate, double muForPDF, double sigmaForPDF, double predationSpeedRatioAH, double predationHunterVoracityAH, double predationProbabilityDensityFunctionAH, double predationSpeedRatioSAW, double predationHunterVoracitySAW, double predationProbabilityDensityFunctionSAW, double encounterHuntedVoracitySAW, double encounterHunterVoracitySAW, double encounterVoracitiesProductSAW, double encounterHunterSizeSAW, double encounterHuntedSizeSAW, double encounterProbabilityDensityFunctionSAW, double encounterHuntedVoracityAH, double encounterHunterVoracityAH, double encounterVoracitiesProductAH, double encounterHunterSizeAH, double encounterHuntedSizeAH, double encounterProbabilityDensityFunctionAH)
{
    unsigned int searchDepth = static_cast<const PointSpatialTree&>(getMutableTerrainCellInterface()->getPosition()).getDepth();
    auto potencialNeighbors = getMutableTerrainCellInterface()->getNeighboursCellsOnRadius(getPosition(), perceptionArea, searchDepth);
        
    //TODO what will happen if they have tracked all already?

    PointContinuous targetPoint;
    pair<TerrainCellInterface::TerrainCellCoverage, unique_ptr<Ring>> evaluationArea;
    const EdibleInterface* bestEdibility = nullptr;
    PointMap cellPosition;

    vector<unique_ptr<TerrainCellEvaluation>> bestEvaluations;

    for(auto &neighbor : *potencialNeighbors)
    {
        bool availableCurrentTargetNeighbor = true;

        if(isMature() && gender == AnimalSpecies::Gender::MALE)
        {
            if(neighbor.first->getNumberOfMatureFemales(getMutableSpecies()) == 0)
            {
                availableCurrentTargetNeighbor = false;
            }
        }

        if(availableCurrentTargetNeighbor)
        {
            double edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass;
            const EdibleInterface* neighborBestEdibility;
            tie(edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass, neighborBestEdibility) = neighbor.first->getCellEvaluation(this, neighbor.second, ediblesHasTriedToPredate, muForPDF, sigmaForPDF, predationSpeedRatioAH, predationHunterVoracityAH, predationProbabilityDensityFunctionAH, predationSpeedRatioSAW, predationHunterVoracitySAW, predationProbabilityDensityFunctionSAW, encounterHuntedVoracitySAW, encounterHunterVoracitySAW, encounterVoracitiesProductSAW, encounterHunterSizeSAW, encounterHuntedSizeSAW, encounterProbabilityDensityFunctionSAW, encounterHuntedVoracityAH, encounterHunterVoracityAH, encounterVoracitiesProductAH, encounterHunterSizeAH, encounterHuntedSizeAH, encounterProbabilityDensityFunctionAH);
            
            unique_ptr<TerrainCellEvaluation> currentEvaluation = make_unique<TerrainCellEvaluation>(getMutableSpecies(), neighborBestEdibility, 
                make_pair(static_cast<unsigned int>(neighbor.second.first), move(neighbor.second.second)), neighbor.first->getPosition(), 
                edibilityValue, predatoryRiskEdibilityValue, conspecificBiomass
            );

            if(bestEvaluations.empty())
            {
                bestEvaluations.push_back(move(currentEvaluation));
            }
            else
            {
                if(*bestEvaluations.front() == *currentEvaluation)
                {
                    bestEvaluations.push_back(move(currentEvaluation));
                }
                else if(*bestEvaluations.front() < *currentEvaluation)
                {
                    bestEvaluations.clear();
                    bestEvaluations.push_back(move(currentEvaluation));
                }
            }
        }
    }


    unsigned int randomIndex;

    if(bestEvaluations.size() > 1)
    {
        randomIndex = Random::randomIndex(bestEvaluations.size());
    }
    else
    {
        randomIndex = 0;
    }

    bestEdibility = bestEvaluations[randomIndex]->bestEdibility;
    evaluationArea = make_pair(
        static_cast<TerrainCellInterface::TerrainCellCoverage>(bestEvaluations[randomIndex]->evaluationArea.first),
        move(bestEvaluations[randomIndex]->evaluationArea.second)
    );
    cellPosition = bestEvaluations[randomIndex]->cellPosition;


    if(bestEdibility != nullptr)
    {
        if(bestEdibility->getSpecies()->isMobile())
        {
            targetPoint = static_cast<const AnimalNonStatistical*>(bestEdibility)->getPosition();
        }
        else
        {
            if(evaluationArea.first == TerrainCellInterface::TerrainCellCoverage::Full)
            {
                targetPoint = Geometry::generateRandomPointOnBox(*evaluationArea.second);
            }
            else
            {
                targetPoint = Geometry::generateRandomPointOnPolygon(*evaluationArea.second);
            }
        }
    }
    else
    {
        if(evaluationArea.first == TerrainCellInterface::TerrainCellCoverage::Full)
        {
            targetPoint = Geometry::generateRandomPointOnBox(*evaluationArea.second);
        }
        else
        {
            targetPoint = Geometry::generateRandomPointOnPolygon(*evaluationArea.second);
        }
    }
    
    setTargetNeighborToTravelTo(make_pair(cellPosition, targetPoint));
}

void SpatialTreeAnimal::moveOneStep()
{
	// Increase the number of movement attempts
	stepsAttempted++;

    bool atDestination;
    pair<TerrainCellInterface *, PointContinuous> cellToMoveTo;
	tie(atDestination, cellToMoveTo) = getMutableTerrainCellInterface()->getCellByBearing(getTargetNeighborToTravelTo(), getPosition());

    setAtDestination(atDestination);

	double distanceToAdd = Geometry::calculateDistanceBetweenPoints(getPosition(), cellToMoveTo.second);

	steps += distanceToAdd;

	if(timeStepMaximumSearchArea > 0.0)
	{
		timeStepMaximumHandlingTimer -= distanceToAdd / timeStepMaximumSearchArea;
	} 
	else
	{
		timeStepMaximumHandlingTimer = 0.0;
	}
	timeStepMaximumHandlingTimer = max(timeStepMaximumHandlingTimer, 0.0);

    if(getMutableTerrainCellInterface() != cellToMoveTo.first)
    {
        getMutableTerrainCellInterface()->migrateAnimalTo(this, cellToMoveTo.first, cellToMoveTo.second);
    }
}
