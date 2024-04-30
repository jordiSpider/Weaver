#include "IBM/World/Map/TerrainCells/TerrainCellEvaluation.h"

using namespace std;



TerrainCellEvaluation::TerrainCellEvaluation(AnimalSpecies* animalSpecies, const EdibleInterface* bestEdibility, pair<unsigned int, unique_ptr<Ring>> evaluationArea, PointMap cellPosition, const double &edibilityValue, 
											 const double &predatoryRiskEdibilityValue, const double &conspecificBiomass)
	: bestEdibility(bestEdibility), evaluationArea(move(evaluationArea)), cellPosition(cellPosition)
{
	double cellEdibilityAndRiskEvaluation = animalSpecies->getCellEvaluationBiomass() * edibilityValue
							- animalSpecies->getCellEvaluationRisk() * predatoryRiskEdibilityValue;

	double cellConspecificAndAntiEvaluation = animalSpecies->getCellEvaluationProConspecific() * conspecificBiomass
							- animalSpecies->getCellEvaluationAntiConspecific() * pow(conspecificBiomass,2);

	double weighedEdibility = cellEdibilityAndRiskEvaluation * (1.0 - animalSpecies->getConspecificWeighing());
	double weighedConspecific = cellConspecificAndAntiEvaluation * animalSpecies->getConspecificWeighing();

	value = weighedEdibility + weighedConspecific;
}


bool TerrainCellEvaluation::operator==(const TerrainCellEvaluation& other) const {
	return fabs(value - other.value) <= numeric_limits<double>::epsilon();
}


bool TerrainCellEvaluation::operator<(const TerrainCellEvaluation& other) const {
	return value < other.value;
}


bool TerrainCellEvaluation::operator>(const TerrainCellEvaluation& other) const {
	return value > other.value;
}

template <class Archive>
void TerrainCellEvaluation::serialize(Archive &ar, const unsigned int version) {
	// Serialize each member individually
	ar & bestEdibility;
	ar & evaluationArea;
	ar & cellPosition;
	ar & value;
}
