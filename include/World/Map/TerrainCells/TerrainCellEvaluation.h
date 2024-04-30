#ifndef TERRAIN_CELL_EVALUATION_H_
#define TERRAIN_CELL_EVALUATION_H_

#include <utility>
#include <memory>

#include "World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "World/Map/SpatialTree/Points/PointSpatialTree.h"
#include "World/LivingBeings/EdibleInterface.h"
#include "World/Map/Geometry/Geometry.h"



struct TerrainCellEvaluation {
    const EdibleInterface* bestEdibility;
    std::pair<unsigned int, std::unique_ptr<Ring>> evaluationArea;
    PointMap cellPosition;
    double value;

    TerrainCellEvaluation(AnimalSpecies* animalSpecies, const EdibleInterface* bestEdibility, std::pair<unsigned int, std::unique_ptr<Ring>> evaluationArea, PointMap cellPosition, const double &edibilityValue, const double &predatoryRiskEdibilityValue, const double &conspecificBiomass);

	bool operator==(const TerrainCellEvaluation& other) const;
    bool operator<(const TerrainCellEvaluation& other) const;
    bool operator>(const TerrainCellEvaluation& other) const;
};

#endif // TERRAIN_CELL_EVALUATION_H_