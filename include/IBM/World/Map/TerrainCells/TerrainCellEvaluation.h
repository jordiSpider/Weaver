#ifndef TERRAIN_CELL_EVALUATION_H_
#define TERRAIN_CELL_EVALUATION_H_

#include <utility>
#include <memory>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/Map/Points/PointSpatialTree.h"
#include "IBM/World/LivingBeings/EdibleInterface.h"
#include "IBM/World/Map/Geometry/Geometry.h"



struct TerrainCellEvaluation {
    const EdibleInterface* bestEdibility;
    std::pair<unsigned int, std::unique_ptr<Ring>> evaluationArea;
    PointMap cellPosition;
    double value;

    friend class boost::serialization::access;

    TerrainCellEvaluation(AnimalSpecies* animalSpecies, const EdibleInterface* bestEdibility, std::pair<unsigned int, std::unique_ptr<Ring>> evaluationArea, PointMap cellPosition, const double &edibilityValue, const double &predatoryRiskEdibilityValue, const double &conspecificBiomass);

	bool operator==(const TerrainCellEvaluation& other) const;
    bool operator<(const TerrainCellEvaluation& other) const;
    bool operator>(const TerrainCellEvaluation& other) const;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif // TERRAIN_CELL_EVALUATION_H_