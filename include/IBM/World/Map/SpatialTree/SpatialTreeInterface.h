#ifndef SPATIAL_TREE_INTERFACE_H_
#define SPATIAL_TREE_INTERFACE_H_

#include <string>
#include <magic_enum.hpp>
#include <unordered_map>

#include "IBM/World/Map/SpatialTree/TerrainCells/SpatialTreeTerrainCellInterface.h"
#include "IBM/World/Map/SpatialTree/Points/PointSpatialTree.h"
#include "Exceptions/LineInfoException.h"
#include "IBM/World/Map/Map.h"




class SpatialTreeInterface : public Map {
protected:
    SpatialTreeInterface(const nlohmann::json &mapConfig, WorldInterface* const worldInterface);
    virtual ~SpatialTreeInterface();

public:
    static constexpr const unsigned int numbreOfSubdivisions = 2;

    virtual const unsigned int getMapDepth() const=0;
    virtual const double& getCellSize(const unsigned int depth) const=0;
    virtual const unsigned int& getAxisSize(const unsigned int depth) const=0;
    virtual SpatialTreeTerrainCellInterface* const getCell(const PointSpatialTree &dstPos, SpatialTreeTerrainCellInterface* const &srcCell) const=0;
    virtual std::unique_ptr<std::vector<PointSpatialTree>> generatePoints(const unsigned int sideLength, const PointSpatialTree& centralCoords, const std::vector<int> &initialCoords) const=0;
    virtual std::unique_ptr<std::vector<PointSpatialTree>> generatePoints(const unsigned int sideLength, const std::vector<int> &initialCoords, unsigned int depth) const=0;
    virtual PointContinuous calculateCenterPosition(const PointSpatialTree &cell, const double &size) const=0;
};

#endif /* SPATIAL_TREE_INTERFACE_H_ */