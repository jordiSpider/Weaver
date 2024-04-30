#include "TerrainCellInterface.h"
#include <limits>
#include <cmath>

using namespace std;


TerrainCellInterface::TerrainCellInterface(WorldInterface* worldInterface)
	: worldInterface(worldInterface)
{

}

TerrainCellInterface::~TerrainCellInterface()
{

}


TerrainCellEvaluation::TerrainCellEvaluation()
	: cell(nullptr), value(NEG_DBL_MAX)
{
	
}

TerrainCellEvaluation::TerrainCellEvaluation(AnimalSpecies* animalSpecies, TerrainCellInterface* cell, const double &edibilityValue, 
											 const double &predatoryRiskEdibilityValue, const double &conspecificBiomass)
	: cell(cell)
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
	return abs(value - other.value) < numeric_limits<double>::epsilon();
}

bool TerrainCellEvaluation::operator<(const TerrainCellEvaluation& other) const {
	return value < other.value;
}

bool TerrainCellEvaluation::operator>(const TerrainCellEvaluation& other) const {
	return value > other.value;
}
