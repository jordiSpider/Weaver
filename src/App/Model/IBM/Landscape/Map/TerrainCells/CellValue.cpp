#include "App/Model/IBM/Landscape/Map/TerrainCells/CellValue.h"

using namespace std;



CellValue::CellValue(const AnimalNonStatistical& animalWhoIsEvaluating, const Edible* bestEdibility, bool fullCoverage, const RingModel* cellEffectiveArea, const PointMap* const cellPosition, const PointContinuous& cellCenter, const PreciseDouble &edibilityValue, 
											 const PreciseDouble &predatoryRiskEdibilityValue, const PreciseDouble &conspecificBiomass, const unsigned int numberOfFemales, const bool habitatDomain)
	: bestEdibility(bestEdibility), fullCoverage(fullCoverage), cellEffectiveArea(cellEffectiveArea), cellPosition(cellPosition), numberOfFemales(numberOfFemales), habitatDomain(habitatDomain)
{
	const PreciseDouble cellEvaluationBiomass = animalWhoIsEvaluating.getGenetics().getCellValueIndividualTraits(CellValueTraitType::cellEvaluationBiomass).getPhenotypicValue();
	const PreciseDouble cellEvaluationRisk = 1.0 - cellEvaluationBiomass;

	const PreciseDouble cellEvaluationProConspecific = animalWhoIsEvaluating.getGenetics().getCellValueIndividualTraits(CellValueTraitType::cellEvaluationProConspecific).getPhenotypicValue();
	const PreciseDouble cellEvaluationAntiConspecific = 1.0 - cellEvaluationProConspecific;


	const PreciseDouble cellEdibilityAndRiskEvaluation = cellEvaluationBiomass * edibilityValue - cellEvaluationRisk * predatoryRiskEdibilityValue;
	const PreciseDouble cellConspecificAndAntiEvaluation = cellEvaluationProConspecific * conspecificBiomass - cellEvaluationAntiConspecific * pow(conspecificBiomass,2);

	const PreciseDouble conspecificWeight = animalWhoIsEvaluating.getGenetics().getCellValueIndividualTraits(CellValueTraitType::conspecificWeight).getPhenotypicValue();


	const PreciseDouble valueReal = cellEdibilityAndRiskEvaluation * (1.0 - conspecificWeight) + cellConspecificAndAntiEvaluation * conspecificWeight;

	const PreciseDouble Pscope = animalWhoIsEvaluating.getSpecies()->getDecisionsBuildingBlock()->getSensoryModel().calculateProbability(
		Geometry::calculateDistanceBetweenPoints(animalWhoIsEvaluating.getPosition(), cellCenter), animalWhoIsEvaluating.getScopeAreaRadius()
	);

	const PreciseDouble error = Random::randomDoubleInRange(-(1.0 - Pscope), (1.0 - Pscope));

	valuePerceived = valueReal + valueReal * error;
}

CellValue::~CellValue()
{
	
}

CellValue::CellValue(const CellValue& other)
	: bestEdibility(other.bestEdibility), fullCoverage(other.fullCoverage), cellEffectiveArea(other.cellEffectiveArea),
	  cellPosition(other.cellPosition), valuePerceived(other.valuePerceived), numberOfFemales(other.numberOfFemales),
	  habitatDomain(other.habitatDomain)
{

}


bool CellValue::operator>(const CellValue& other) const {
	if(habitatDomain)
	{
		if(!other.habitatDomain)
		{
			return true;
		}
	}
	else
	{
		if(other.habitatDomain)
		{
			return false;
		}
	}

	return valuePerceived > other.valuePerceived;
}

bool CellValue::operator>=(const CellValue& other) const {
	if(habitatDomain)
	{
		if(!other.habitatDomain)
		{
			return true;
		}
	}
	else
	{
		if(other.habitatDomain)
		{
			return false;
		}
	}

	return valuePerceived >= other.valuePerceived;
}
