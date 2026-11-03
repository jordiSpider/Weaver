
#include "App/Model/IBM/Landscape/Map/SpatialTree.h"
#include "App/Model/IBM/Landscape/Landscape.h"
#include "App/Manager/LogManager.h"
#include "App/Model/IBM/Landscape/Map/Patches/Patch.h"
#include "App/Model/IBM/Landscape/Map/Patches/ObstaclePorosityMaskGenerator.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/Obstacle/BranchCellObstacle.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/TerrainCells/PatchApplicator/BranchPatchApplicator.h"

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>

using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;

namespace {
double pointSegmentDistanceSquared(const double x, const double y, const double startX, const double startY, const double endX, const double endY)
{
    const double deltaX = endX - startX;
    const double deltaY = endY - startY;
    const double lengthSquared = deltaX * deltaX + deltaY * deltaY;
    if (lengthSquared == 0.0)
    {
        const double pointDX = x - startX;
        const double pointDY = y - startY;
        return pointDX * pointDX + pointDY * pointDY;
    }

    const double projection = clamp(((x - startX) * deltaX + (y - startY) * deltaY) / lengthSquared, 0.0, 1.0);
    const double nearestX = startX + projection * deltaX;
    const double nearestY = startY + projection * deltaY;
    const double pointDX = x - nearestX;
    const double pointDY = y - nearestY;
    return pointDX * pointDX + pointDY * pointDY;
}

double segmentHorizontalDistanceSquared(
    const double startX,
    const double startY,
    const double endX,
    const double endY,
    const double edgeStartX,
    const double edgeY,
    const double edgeEndX)
{
    const double deltaX = endX - startX;
    const double deltaY = endY - startY;
    const double coordinateScale = max({1.0, abs(startX), abs(startY), abs(endX), abs(endY), abs(edgeStartX), abs(edgeY), abs(edgeEndX)});
    const double tolerance = numeric_limits<double>::epsilon() * coordinateScale * 16.0;
    if (deltaY != 0.0)
    {
        const double fraction = (edgeY - startY) / deltaY;
        if (fraction >= -numeric_limits<double>::epsilon() * 16.0 && fraction <= 1.0 + numeric_limits<double>::epsilon() * 16.0)
        {
            const double intersectionX = startX + clamp(fraction, 0.0, 1.0) * deltaX;
            if (intersectionX >= edgeStartX - tolerance && intersectionX <= edgeEndX + tolerance)
            {
                return 0.0;
            }
        }
    }
    else if (startY == edgeY
        && max(min(startX, endX), edgeStartX) <= min(max(startX, endX), edgeEndX) + tolerance)
    {
        return 0.0;
    }

    return min({
        pointSegmentDistanceSquared(startX, startY, edgeStartX, edgeY, edgeEndX, edgeY),
        pointSegmentDistanceSquared(endX, endY, edgeStartX, edgeY, edgeEndX, edgeY),
        pointSegmentDistanceSquared(edgeStartX, edgeY, startX, startY, endX, endY),
        pointSegmentDistanceSquared(edgeEndX, edgeY, startX, startY, endX, endY)
    });
}

bool clearanceIsInsufficient(const double distanceSquared, const double radiusSquared)
{
    return distanceSquared <= radiusSquared;
}

struct EdgeQueryScratch
{
    vector<uint32_t> visited;
    uint32_t generation = 0u;

    void begin(const size_t edgeCount)
    {
        if (visited.size() != edgeCount)
        {
            visited.assign(edgeCount, 0u);
            generation = 0u;
        }
        if (++generation == 0u)
        {
            fill(visited.begin(), visited.end(), 0u);
            generation = 1u;
        }
    }
};

size_t minimumCandidateCell(const double coordinate, const double cellSize)
{
    const size_t cell = static_cast<size_t>(floor(coordinate / cellSize));
    return cell == 0u ? 0u : cell - 1u;
}

size_t maximumCandidateCell(const double coordinate, const double cellSize, const size_t count)
{
    const size_t cell = static_cast<size_t>(floor(coordinate / cellSize));
    return min(count - 1u, cell + 1u);
}

void collectLeafCells(SpatialTreeTerrainCell* const cell, vector<LeafTerrainCell*>& leafCells, const size_t gridWidth)
{
    if(cell->isLeaf())
    {
        auto* leaf = static_cast<LeafTerrainCell*>(cell);
        const size_t x = leaf->getPosition().get(Axis::X);
        const size_t y = leaf->getPosition().get(Axis::Y);
        leafCells[y * gridWidth + x] = leaf;
        return;
    }

    auto* branch = static_cast<BranchTerrainCell*>(cell);
    for(SpatialTreeTerrainCell* const child : branch->getMutableChildrenTerrainCells())
    {
        collectLeafCells(child, leafCells, gridWidth);
    }
}

void updateObstacleSummaries(SpatialTreeTerrainCell* const cell)
{
    if(cell->isLeaf())
    {
        return;
    }

    auto* branch = static_cast<BranchTerrainCell*>(cell);
    for(SpatialTreeTerrainCell* const child : branch->getMutableChildrenTerrainCells())
    {
        updateObstacleSummaries(child);
    }

    static_cast<BranchPatchApplicator&>(branch->getMutablePatchApplicator()).getMutableCellObstacle().applyBranch(branch);
}

bool applyPorousObstaclePatch(
    RootTerrainCell* const root,
    const Patch& patch,
    const ObstacleSource& source,
    const size_t gridWidth,
    const double cellSize,
    unsigned int seed)
{
    if(patch.getShapeType() != PatchShape::Type::Cubic)
    {
        throw invalid_argument("Porosity is currently supported only for cubic patches.");
    }

    const size_t numberOfCells = gridWidth * gridWidth;
    vector<LeafTerrainCell*> leafCells(numberOfCells, nullptr);
    collectLeafCells(root, leafCells, gridWidth);

    vector<std::uint8_t> eligibleCells(numberOfCells, 0u);
    for(size_t index = 0u; index < numberOfCells; ++index)
    {
        LeafTerrainCell* const leaf = leafCells[index];
        if(patch.getPriority() <= leaf->getPatchApplicator().getCellObstacle().getPatchPriority())
        {
            continue;
        }

        eligibleCells[index] = 1u;
    }

    const ObstacleSource::PorosityParameters& sourceParameters = source.getPorosityParameters();
    const ObstaclePorosityMaskGenerator::Parameters parameters{
        sourceParameters.fraction,
        sourceParameters.minimumPoreWidth,
        sourceParameters.featureSize,
        sourceParameters.octaves
    };
    auto patchBounds = boost::geometry::return_envelope<BoxModel>(patch.getInfluenceZone());
    const vector<int> patchCells = ObstaclePorosityMaskGenerator::generatePatch(
        eligibleCells,
        gridWidth,
        gridWidth,
        cellSize,
        cellSize,
        boost::geometry::get<0>(patchBounds.min_corner()),
        boost::geometry::get<1>(patchBounds.min_corner()),
        boost::geometry::get<0>(patchBounds.max_corner()),
        boost::geometry::get<1>(patchBounds.max_corner()),
        parameters,
        seed);

    bool applied = false;
    for(size_t index = 0u; index < numberOfCells; ++index)
    {
        if(patchCells[index] >= 0)
        {
            leafCells[index]->getMutablePatchApplicator().getMutableCellObstacle().applySource(
                patch.getPriority(), patchCells[index] == 0);
            applied = true;
        }
    }

    if(!applied) return false;
    updateObstacleSummaries(root);
    return true;
}
}



SpatialTree::SpatialTree()
    : Map()
{

}

SpatialTree::SpatialTree(const json &mapConfig, const json &, MoistureSource* const moistureBaseSource)
    : Map(mapConfig), mapDepth(calculateMapDepth()), MAX_PARALLEL_DEPTH(std::min(4u, mapDepth - 2u)),
      cellSizes(calculateCellSizes()),
      axisSizes(calculateAxisSizes())
{
    rootTerrainCell = new RootTerrainCell(cellSizes, mapDepth, new PointSpatialTree(), moistureBaseSource);
}

SpatialTree::~SpatialTree()
{
    delete rootTerrainCell;
}


unsigned int SpatialTree::calculateMapDepth() const
{
    return static_cast<unsigned int>(logMap(static_cast<double>(getNumberOfCellsPerAxis())).getValue()) + 1u;
}


PreciseDouble SpatialTree::logMap(const PreciseDouble &value) const
{
    return log(value) / log(SpatialTree::numbreOfSubdivisions);
}


vector<PreciseDouble> SpatialTree::calculateCellSizes() const
{
    vector<PreciseDouble> newCellSizes;

    for(unsigned int cellsPerAxis = getNumberOfCellsPerAxis(); cellsPerAxis > 1u; cellsPerAxis /= SpatialTree::numbreOfSubdivisions)
    {
        newCellSizes.push_back(getMinCellSize() * static_cast<double>(cellsPerAxis));
    }

    newCellSizes.push_back(getMinCellSize());

    return newCellSizes;
}

PointMap SpatialTree::obtainPointMap(const PointContinuous &point, const unsigned int depth) const
{
    vector<unsigned int> axisValues(DIMENSIONS);

    for(unsigned char i = 0; i < DIMENSIONS; i++)
    {
        axisValues[i] = static_cast<unsigned int>((getPositionAxisValue(point, i) / getCellSize(depth)).getValue());
    }

    return PointMap(axisValues);
}

vector<unsigned int> SpatialTree::calculateAxisSizes() const
{
    vector<unsigned int> newAxisSizes;

    for(unsigned int cellsPerAxis = 1u; cellsPerAxis <= getNumberOfCellsPerAxis(); cellsPerAxis *= SpatialTree::numbreOfSubdivisions)
    {
        newAxisSizes.push_back(cellsPerAxis);
    }

    return newAxisSizes;
}


const PreciseDouble& SpatialTree::getCellSize(const unsigned int depth) const
{
    return cellSizes[depth];
}

const unsigned int& SpatialTree::getAxisSize(const unsigned int depth) const
{
    return axisSizes[depth];
}

TerrainCell* SpatialTree::getCell(const PointContinuous& position) const
{
    return getCell(obtainPointMap(position, mapDepth - 1u));
}

TerrainCell* SpatialTree::getCell(const PointMap& position) const
{
    return rootTerrainCell->getCell(PointSpatialTree(position.getAxisValues(), mapDepth - 1u));
}

void SpatialTree::update(const TimeStep& numberOfTimeSteps)
{
    rootTerrainCell->update(numberOfTimeSteps, MAX_PARALLEL_DEPTH);
}


void SpatialTree::addAnimalSpecies(const AnimalSpecies& animalSpecies)
{
    rootTerrainCell->addAnimalSpecies(animalSpecies);
}

void SpatialTree::addResourceSpecies(Landscape* const landscape, vector<vector<vector<CellResource*>>>& landscapeResources, ResourceSpecies& resourceSpecies, ResourceSource* const resourceBaseSource)
{
    rootTerrainCell->addResourceSpecies(landscape, landscapeResources, resourceSpecies, resourceBaseSource);
}

void SpatialTree::prepareObstacleGeometry()
{
    ensureObstacleGeometry();
}

std::vector<PointContinuous> SpatialTree::generateRouteCandidatePositions(const PreciseDouble& animalBodySection) const
{
    ensureObstacleGeometry();
    const double cellSize = getMinCellSize().getValue();
    const double radius = animalBodySection.getValue() / 2.0;
    if (!isfinite(cellSize) || cellSize <= 0.0 || !isfinite(radius) || radius < 0.0)
    {
        return {};
    }

    constexpr size_t arcSegmentsPerQuarter = 8u;
    constexpr double pi = 3.14159265358979323846;
    const size_t mapWidth = getNumberOfCellsByAxis(0u);
    const size_t mapHeight = getNumberOfCellsByAxis(1u);
    const double extentX = mapWidth * cellSize;
    const double extentY = mapHeight * cellSize;
    const double boundaryEpsilon = max(cellSize * 1.0e-8, numeric_limits<double>::epsilon() * max(extentX, extentY) * 8.0);
    const double angularHalfStep = pi / (4.0 * arcSegmentsPerQuarter);
    const double candidateRadius = radius / cos(angularHalfStep) + boundaryEpsilon;
    vector<PointContinuous> candidates;
    candidates.reserve(obstacleBoundarySegments.size() * (4u * arcSegmentsPerQuarter + 2u));

    const auto addCandidate = [&](const double x, const double y)
    {
        if (x < 0.0 || y < 0.0 || x >= extentX || y >= extentY)
        {
            return;
        }
        const PointContinuous candidate(x, y);
        if (isTraversable(candidate, animalBodySection))
        {
            candidates.push_back(candidate);
        }
    };

    for (const ObstacleBoundarySegment& edge : obstacleBoundarySegments)
    {
        const double fixed = edge.fixedCoordinate * cellSize;
        const double edgeStart = edge.startCoordinate * cellSize;
        const double edgeEnd = edge.endCoordinate * cellSize;

        if (edge.horizontal)
        {
            const double freeNormalAngle = edge.obstacleOnPositiveSide ? -pi / 2.0 : pi / 2.0;
            for (size_t arcIndex = 0u; arcIndex <= 2u * arcSegmentsPerQuarter; ++arcIndex)
            {
                const double angle = freeNormalAngle - pi / 2.0 + pi * arcIndex / (2u * arcSegmentsPerQuarter);
                addCandidate(edgeEnd + candidateRadius * cos(angle), fixed + candidateRadius * sin(angle));
                addCandidate(edgeStart + candidateRadius * cos(angle), fixed + candidateRadius * sin(angle));
            }
        }
        else
        {
            const double freeNormalAngle = edge.obstacleOnPositiveSide ? pi : 0.0;
            for (size_t arcIndex = 0u; arcIndex <= 2u * arcSegmentsPerQuarter; ++arcIndex)
            {
                const double angle = freeNormalAngle - pi / 2.0 + pi * arcIndex / (2u * arcSegmentsPerQuarter);
                addCandidate(fixed + candidateRadius * cos(angle), edgeEnd + candidateRadius * sin(angle));
                addCandidate(fixed + candidateRadius * cos(angle), edgeStart + candidateRadius * sin(angle));
            }
        }
    }

    sort(candidates.begin(), candidates.end(), [](const PointContinuous& left, const PointContinuous& right)
    {
        return left.get<0>() < right.get<0>() || (left.get<0>() == right.get<0>() && left.get<1>() < right.get<1>());
    });
    candidates.erase(unique(candidates.begin(), candidates.end(), [](const PointContinuous& left, const PointContinuous& right)
    {
        return left.get<0>() == right.get<0>() && left.get<1>() == right.get<1>();
    }), candidates.end());
    return candidates;
}

void SpatialTree::ensureObstacleGeometry() const
{
    if (obstacleGeometryValid)
    {
        return;
    }

    const size_t width = getNumberOfCellsByAxis(0u);
    const size_t height = getNumberOfCellsByAxis(1u);
    const size_t cellCount = width * height;
    obstacleCellMask.assign(cellCount, 0u);
    obstacleBoundarySegments.clear();
    obstacleEdgeOffsets.clear();
    obstacleEdgeIndices.clear();

    if (width == 0u || height == 0u)
    {
        obstacleEdgeOffsets.push_back(0u);
        obstacleGeometryValid = true;
        return;
    }

    for (size_t y = 0u; y < height; ++y)
    {
        for (size_t x = 0u; x < width; ++x)
        {
            const TerrainCell* const cell = getCell(PointMap({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}));
            obstacleCellMask[y * width + x] = cell != nullptr && cell->getPatchApplicator().getCellObstacle().isObstacle();
        }
    }

    for (size_t y = 1u; y < height; ++y)
    {
        size_t x = 0u;
        while (x < width)
        {
            const bool obstacleBelow = obstacleCellMask[(y - 1u) * width + x] != 0u;
            const bool obstacleAbove = obstacleCellMask[y * width + x] != 0u;
            if (obstacleBelow == obstacleAbove)
            {
                ++x;
                continue;
            }

            const size_t start = x++;
            while (x < width)
            {
                const bool nextObstacleBelow = obstacleCellMask[(y - 1u) * width + x] != 0u;
                const bool nextObstacleAbove = obstacleCellMask[y * width + x] != 0u;
                if (nextObstacleBelow == nextObstacleAbove || nextObstacleAbove != obstacleAbove)
                {
                    break;
                }
                ++x;
            }
            obstacleBoundarySegments.push_back({y, start, x, true, obstacleAbove});
        }
    }

    for (size_t x = 1u; x < width; ++x)
    {
        size_t y = 0u;
        while (y < height)
        {
            const bool obstacleLeft = obstacleCellMask[y * width + x - 1u] != 0u;
            const bool obstacleRight = obstacleCellMask[y * width + x] != 0u;
            if (obstacleLeft == obstacleRight)
            {
                ++y;
                continue;
            }

            const size_t start = y++;
            while (y < height
                && (obstacleCellMask[y * width + x - 1u] != 0u) != (obstacleCellMask[y * width + x] != 0u)
                && (obstacleCellMask[y * width + x] != 0u) == obstacleRight)
            {
                ++y;
            }
            obstacleBoundarySegments.push_back({x, start, y, false, obstacleRight});
        }
    }

    vector<size_t> edgeCounts(cellCount, 0u);
    const auto visitEdgeCells = [&](const ObstacleBoundarySegment& edge, const auto& action)
    {
        if (edge.horizontal)
        {
            const size_t firstRow = edge.fixedCoordinate - 1u;
            const size_t lastRow = edge.fixedCoordinate;
            const size_t firstColumn = edge.startCoordinate == 0u ? 0u : edge.startCoordinate - 1u;
            const size_t lastColumn = min(width - 1u, edge.endCoordinate);
            for (size_t row = firstRow; row <= lastRow; ++row)
            {
                for (size_t column = firstColumn; column <= lastColumn; ++column)
                {
                    action(row * width + column);
                }
            }
            return;
        }

        const size_t firstColumn = edge.fixedCoordinate - 1u;
        const size_t lastColumn = edge.fixedCoordinate;
        const size_t firstRow = edge.startCoordinate == 0u ? 0u : edge.startCoordinate - 1u;
        const size_t lastRow = min(height - 1u, edge.endCoordinate);
        for (size_t row = firstRow; row <= lastRow; ++row)
        {
            for (size_t column = firstColumn; column <= lastColumn; ++column)
            {
                action(row * width + column);
            }
        }
    };

    for (const ObstacleBoundarySegment& edge : obstacleBoundarySegments)
    {
        visitEdgeCells(edge, [&](const size_t cellIndex) { ++edgeCounts[cellIndex]; });
    }

    obstacleEdgeOffsets.resize(cellCount + 1u, 0u);
    for (size_t cellIndex = 0u; cellIndex < cellCount; ++cellIndex)
    {
        obstacleEdgeOffsets[cellIndex + 1u] = obstacleEdgeOffsets[cellIndex] + edgeCounts[cellIndex];
    }
    obstacleEdgeIndices.resize(obstacleEdgeOffsets.back());
    vector<size_t> nextEdgeIndex(obstacleEdgeOffsets.begin(), obstacleEdgeOffsets.end() - 1u);
    for (size_t edgeIndex = 0u; edgeIndex < obstacleBoundarySegments.size(); ++edgeIndex)
    {
        visitEdgeCells(obstacleBoundarySegments[edgeIndex], [&](const size_t cellIndex)
        {
            obstacleEdgeIndices[nextEdgeIndex[cellIndex]++] = edgeIndex;
        });
    }

    obstacleGeometryValid = true;
}

void SpatialTree::invalidateObstacleGeometry()
{
    obstacleGeometryValid = false;
}

bool SpatialTree::isTraversable(const PointContinuous& position, const PreciseDouble& animalBodySection) const
{
    ensureObstacleGeometry();
    const double cellSize = getMinCellSize().getValue();
    const double radius = animalBodySection.getValue() / 2.0;
    const size_t mapWidth = getNumberOfCellsByAxis(0u);
    const size_t mapHeight = getNumberOfCellsByAxis(1u);
    const double x = position.get<0>();
    const double y = position.get<1>();
    if (mapWidth == 0u || mapHeight == 0u || cellSize <= 0.0 || !isfinite(cellSize)
        || !isfinite(radius) || radius < 0.0 || !isfinite(x) || !isfinite(y))
    {
        return false;
    }

    const double mapExtentX = static_cast<double>(mapWidth) * cellSize;
    const double mapExtentY = static_cast<double>(mapHeight) * cellSize;
    if (x < 0.0 || y < 0.0 || x > mapExtentX || y > mapExtentY)
    {
        return false;
    }

    const double radiusSquared = radius * radius;
    const double boundaryClearance = min({x, y, mapExtentX - x, mapExtentY - y});
    if (clearanceIsInsufficient(boundaryClearance * boundaryClearance, radiusSquared))
    {
        return false;
    }

    const size_t cellX = static_cast<size_t>(floor(x / cellSize));
    const size_t cellY = static_cast<size_t>(floor(y / cellSize));
    if (obstacleCellMask[cellY * mapWidth + cellX] != 0u)
    {
        return false;
    }

    const size_t minX = minimumCandidateCell(max(0.0, x - radius), cellSize);
    const size_t minY = minimumCandidateCell(max(0.0, y - radius), cellSize);
    const size_t maxX = maximumCandidateCell(min(mapExtentX, x + radius), cellSize, mapWidth);
    const size_t maxY = maximumCandidateCell(min(mapExtentY, y + radius), cellSize, mapHeight);
    thread_local EdgeQueryScratch scratch;
    scratch.begin(obstacleBoundarySegments.size());

    for (size_t obstacleY = minY; obstacleY <= maxY; ++obstacleY)
    {
        for (size_t obstacleX = minX; obstacleX <= maxX; ++obstacleX)
        {
            const size_t cellIndex = obstacleY * mapWidth + obstacleX;
            for (size_t edgeOffset = obstacleEdgeOffsets[cellIndex]; edgeOffset < obstacleEdgeOffsets[cellIndex + 1u]; ++edgeOffset)
            {
                const size_t edgeIndex = obstacleEdgeIndices[edgeOffset];
                if (scratch.visited[edgeIndex] == scratch.generation)
                {
                    continue;
                }
                scratch.visited[edgeIndex] = scratch.generation;

                const ObstacleBoundarySegment& edge = obstacleBoundarySegments[edgeIndex];
                const double fixed = edge.fixedCoordinate * cellSize;
                const double edgeStart = edge.startCoordinate * cellSize;
                const double edgeEnd = edge.endCoordinate * cellSize;
                const double distanceSquared = edge.horizontal
                    ? pointSegmentDistanceSquared(x, y, edgeStart, fixed, edgeEnd, fixed)
                    : pointSegmentDistanceSquared(x, y, fixed, edgeStart, fixed, edgeEnd);
                if (clearanceIsInsufficient(distanceSquared, radiusSquared))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

bool SpatialTree::isTraversable(const PointContinuous& start, const PointContinuous& end, const PreciseDouble& animalBodySection) const
{
    if (!isTraversable(start, animalBodySection) || !isTraversable(end, animalBodySection))
    {
        return false;
    }

    const double radius = animalBodySection.getValue() / 2.0;
    const double radiusSquared = radius * radius;
    const double cellSize = getMinCellSize().getValue();
    const size_t mapWidth = getNumberOfCellsByAxis(0u);
    const size_t mapHeight = getNumberOfCellsByAxis(1u);
    const double startX = start.get<0>();
    const double startY = start.get<1>();
    const double endX = end.get<0>();
    const double endY = end.get<1>();
    const double deltaX = endX - startX;
    const double deltaY = endY - startY;
    size_t cellX = static_cast<size_t>(floor(startX / cellSize));
    size_t cellY = static_cast<size_t>(floor(startY / cellSize));
    const size_t endCellX = static_cast<size_t>(floor(endX / cellSize));
    const size_t endCellY = static_cast<size_t>(floor(endY / cellSize));
    const size_t neighborRadius = static_cast<size_t>(ceil(radius / cellSize)) + 1u;
    const double infinity = numeric_limits<double>::infinity();
    int stepX = 0;
    int stepY = 0;
    double nextBoundaryX = infinity;
    double nextBoundaryY = infinity;
    double boundaryStepX = infinity;
    double boundaryStepY = infinity;

    if (deltaX > 0.0)
    {
        stepX = 1;
        nextBoundaryX = ((cellX + 1u) * cellSize - startX) / deltaX;
        boundaryStepX = cellSize / deltaX;
    }
    else if (deltaX < 0.0)
    {
        stepX = -1;
        nextBoundaryX = (startX - cellX * cellSize) / -deltaX;
        boundaryStepX = cellSize / -deltaX;
    }

    if (deltaY > 0.0)
    {
        stepY = 1;
        nextBoundaryY = ((cellY + 1u) * cellSize - startY) / deltaY;
        boundaryStepY = cellSize / deltaY;
    }
    else if (deltaY < 0.0)
    {
        stepY = -1;
        nextBoundaryY = (startY - cellY * cellSize) / -deltaY;
        boundaryStepY = cellSize / -deltaY;
    }

    thread_local EdgeQueryScratch scratch;
    scratch.begin(obstacleBoundarySegments.size());
    const auto hasClearanceAroundCell = [&](const size_t centerX, const size_t centerY)
    {
        const size_t firstX = neighborRadius > centerX ? 0u : centerX - neighborRadius;
        const size_t firstY = neighborRadius > centerY ? 0u : centerY - neighborRadius;
        const size_t lastX = centerX + min(neighborRadius, mapWidth - 1u - centerX);
        const size_t lastY = centerY + min(neighborRadius, mapHeight - 1u - centerY);
        for (size_t candidateY = firstY; candidateY <= lastY; ++candidateY)
        {
            for (size_t candidateX = firstX; candidateX <= lastX; ++candidateX)
            {
                const size_t cellIndex = candidateY * mapWidth + candidateX;
                for (size_t edgeOffset = obstacleEdgeOffsets[cellIndex]; edgeOffset < obstacleEdgeOffsets[cellIndex + 1u]; ++edgeOffset)
                {
                    const size_t edgeIndex = obstacleEdgeIndices[edgeOffset];
                    if (scratch.visited[edgeIndex] == scratch.generation)
                    {
                        continue;
                    }
                    scratch.visited[edgeIndex] = scratch.generation;

                    const ObstacleBoundarySegment& edge = obstacleBoundarySegments[edgeIndex];
                    const double fixed = edge.fixedCoordinate * cellSize;
                    const double edgeStart = edge.startCoordinate * cellSize;
                    const double edgeEnd = edge.endCoordinate * cellSize;
                    const double distanceSquared = edge.horizontal
                        ? segmentHorizontalDistanceSquared(startX, startY, endX, endY, edgeStart, fixed, edgeEnd)
                        : segmentHorizontalDistanceSquared(startY, startX, endY, endX, edgeStart, fixed, edgeEnd);
                    if (clearanceIsInsufficient(distanceSquared, radiusSquared))
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    };

    while (true)
    {
        if (!hasClearanceAroundCell(cellX, cellY))
        {
            return false;
        }
        if (cellX == endCellX && cellY == endCellY)
        {
            break;
        }

        if (nextBoundaryX < nextBoundaryY)
        {
            cellX = stepX > 0 ? cellX + 1u : cellX - 1u;
            nextBoundaryX += boundaryStepX;
        }
        else if (nextBoundaryY < nextBoundaryX)
        {
            cellY = stepY > 0 ? cellY + 1u : cellY - 1u;
            nextBoundaryY += boundaryStepY;
        }
        else
        {
            cellX = stepX > 0 ? cellX + 1u : cellX - 1u;
            cellY = stepY > 0 ? cellY + 1u : cellY - 1u;
            nextBoundaryX += boundaryStepX;
            nextBoundaryY += boundaryStepY;
        }
    }

    return true;
}

bool SpatialTree::applyPatch(Landscape* const landscape, const Patch& patch, unsigned int seed)
{
    bool applied;
    if(patch.getSource().getType() == PathSource::Type::Obstacle)
    {
        const auto& obstacleSource = static_cast<const ObstacleSource&>(patch.getSource());
        if(obstacleSource.hasPorosity())
        {
            applied = applyPorousObstaclePatch(rootTerrainCell, patch, obstacleSource, getNumberOfCellsPerAxis(), getMinCellSize().getValue(), seed);
            if (applied)
            {
                invalidateObstacleGeometry();
            }
            return applied;
        }
    }

    const bool modifiesObstacles = patch.getSource().getType() == PathSource::Type::Obstacle;
    applied = rootTerrainCell->getMutablePatchApplicator().applyPatch(landscape, patch).second;
    if (applied && modifiesObstacles)
    {
        invalidateObstacleGeometry();
    }
    return applied;
}



void SpatialTree::obtainGlobalTemperatureRange(unordered_set<Temperature>& globalTemperatureRange) const
{
    vector<Temperature> temperatureCycle;
    rootTerrainCell->getPatchApplicator().getCellMoisture().addTemperatureRange(globalTemperatureRange, temperatureCycle);
}


void SpatialTree::generatePointsRecursively(vector<PointSpatialTree>& points, const unsigned int sideLength, vector<unsigned int> &currentCoords, unsigned char axis, const PointSpatialTree& centralCoords, const vector<int> &initialCoords) const
{
    // Base case
    if(axis == DIMENSIONS) {
        if(currentCoords != centralCoords.getAxisValues())
        {
            points.emplace_back(currentCoords, centralCoords.getDepth());
        }
    }
    else
    {
        for(unsigned int i = 0; i < sideLength; ++i)
        {
            int axisValue = initialCoords[axis] + static_cast<int>(i);

            if(0 <= axisValue && axisValue < static_cast<const int&>(getAxisSize(centralCoords.getDepth())))
            {
                currentCoords[axis] = static_cast<unsigned int>(axisValue);
            
                vector<PointSpatialTree> subPoints;
                generatePointsRecursively(subPoints, sideLength, currentCoords, axis + 1u, centralCoords, initialCoords);
                for(auto &elem : subPoints)
                {
                    points.push_back(elem);
                }
            }
        }
    }
}

void SpatialTree::generatePointsRecursively(vector<PointSpatialTree>& points, const unsigned int sideLength, vector<unsigned int> &currentCoords, unsigned char axis, unsigned int depth, const vector<int> &initialCoords) const
{
    // Base case
    if(axis == DIMENSIONS) {
        points.emplace_back(currentCoords, depth);
    }
    else
    {
        for(unsigned int i = 0; i < sideLength; ++i)
        {
            int axisValue = initialCoords[axis] + static_cast<int>(i);

            if(0 <= axisValue && axisValue < static_cast<const int&>(getAxisSize(depth)))
            {
                currentCoords[axis] = static_cast<unsigned int>(axisValue);
            
                vector<PointSpatialTree> subPoints;
                generatePointsRecursively(subPoints, sideLength, currentCoords, axis + 1u, depth, initialCoords);
                for(auto &elem : subPoints)
                {
                    points.push_back(elem);
                }
            }
        }
    }
}


void SpatialTree::generatePoints(vector<PointSpatialTree>& points, const unsigned int sideLength, const PointSpatialTree& centralCoords, const vector<int> &initialCoords) const
{
    vector<unsigned int> currentCoords(DIMENSIONS, 0);
    generatePointsRecursively(points, sideLength, currentCoords, 0, centralCoords, initialCoords);
}

void SpatialTree::generatePoints(vector<PointSpatialTree>& points, const unsigned int sideLength, const vector<int> &initialCoords, unsigned int depth) const
{
    vector<unsigned int> currentCoords(DIMENSIONS, 0);
    generatePointsRecursively(points, sideLength, currentCoords, 0, depth, initialCoords);
}

void SpatialTree::obtainInhabitableTerrainCells()
{
    rootTerrainCell->obtainInhabitableTerrainCells(this);
}



void SpatialTree::obtainSpeciesInhabitableTerrainCells(vector<CustomIndexedVector<Instar, vector<vector<TerrainCell*>::iterator>>>& mapSpeciesInhabitableTerrainCells, const vector<AnimalSpecies*>& existingAnimalSpecies, const CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, std::vector<ResourceSpecies::ResourceID>>>& involvedResourceSpecies)
{
    mapSpeciesInhabitableTerrainCells.resize(existingAnimalSpecies.size());

    for(const AnimalSpecies* const &animalSpecies : existingAnimalSpecies)
    {
        mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()].resize(animalSpecies->getGrowthBuildingBlock().getNumberOfInstars());

        for(const Instar &instar : animalSpecies->getGrowthBuildingBlock().getInstarsRange())
        {
            const PreciseDouble sectionSize = animalSpecies->getPoreUsePerInstar(instar);

            for (auto inhabitableTerrainCell = getMutableInhabitableTerrainCells().begin(); inhabitableTerrainCell != getMutableInhabitableTerrainCells().end(); ++inhabitableTerrainCell)
            {
                if (isSpeciesInhabitableTerrainCell(*animalSpecies, *inhabitableTerrainCell, involvedResourceSpecies[animalSpecies->getAnimalSpeciesId()][instar]))
                {
                    mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar].push_back(inhabitableTerrainCell);
                }
            }
        }
    }
}


unsigned int SpatialTree::generateStatisticsPopulation(vector<CustomIndexedVector<Instar, vector<AnimalStatistical*>>>& animalsPopulation, Landscape* const landscape, vector<AnimalSpecies*>& existingAnimalSpecies, vector<CustomIndexedVector<Instar, vector<vector<TerrainCell*>::iterator>>> &mapSpeciesInhabitableTerrainCells, const TimeStep actualTimeStep, const PreciseDouble& timeStepsPerDay)
{
    unsigned int populationSize = 0;
    animalsPopulation.resize(existingAnimalSpecies.size());

	for(AnimalSpecies* &animalSpecies : existingAnimalSpecies)
    {
        LogManager::emit(fmt::format(" - Animal species {}\n", animalSpecies->getScientificName()));

        ProgressBar progressBar(animalSpecies->getGrowthBuildingBlock().getInstarsRange().size() * animalSpecies->getStatisticsIndividualsPerInstar());

        animalsPopulation[animalSpecies->getAnimalSpeciesId()].resize(animalSpecies->getGrowthBuildingBlock().getNumberOfInstars());

		for(const Instar &instar : animalSpecies->getGrowthBuildingBlock().getInstarsRange())
		{
            if (mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar].empty()) {
                LogManager::emit(fmt::format("Generation omitted: No inhabitable terrain cells available for {} ({} instar).\n", animalSpecies->getScientificName(), instar));

                progressBar.update(animalSpecies->getStatisticsIndividualsPerInstar());
            }
            else {
                for (unsigned long individual = 0; individual < animalSpecies->getStatisticsIndividualsPerInstar(); individual++)
                {
                    AnimalStatistical* newAnimal = nullptr;
                    auto candidateCells = mapSpeciesInhabitableTerrainCells[animalSpecies->getAnimalSpeciesId()][instar];
                    while (!candidateCells.empty() && newAnimal == nullptr)
                    {
                        const size_t randomCellIndex = Random::randomIndex(candidateCells.size());
                        std::vector<fmt::memory_buffer> geneticsBuffer;
                        auto newTerrainCell = (*candidateCells[randomCellIndex])->randomInsertAnimal(landscape, instar, animalSpecies, true, nullptr, false, geneticsBuffer, actualTimeStep, timeStepsPerDay);
                        newAnimal = static_cast<AnimalStatistical*>(get<3>(newTerrainCell));

                        if (newAnimal == nullptr)
                        {
                            candidateCells.erase(candidateCells.begin() + randomCellIndex);
                        }
                    }

                    if (newAnimal != nullptr)
                    {
                        animalsPopulation[animalSpecies->getAnimalSpeciesId()][instar].push_back(newAnimal);
                        populationSize++;
                        progressBar.update();
                    }
                    else
                    {
                        LogManager::emit(fmt::format("Generation omitted: No valid initialization position for {} ({} instar).\n", animalSpecies->getScientificName(), instar));
                        progressBar.update();
                    }
                }
            }
		}
	}

    return populationSize;
}

PointContinuous SpatialTree::calculateCenterPosition(const PointSpatialTree &cell, const PreciseDouble &size) const
{
    PointContinuous center;
    boost::geometry::assign_zero(center);

    PreciseDouble halfOfSize = size / 2.0;

    for(unsigned char i = 0; i < DIMENSIONS; i++)
    {
        Axis axis = magic_enum::enum_cast<Axis>(i).value();
        setPositionAxisValue(center, i, static_cast<double>(cell.get(axis)) * size + halfOfSize);
    }

    return center;
}

unsigned int SpatialTree::getMapDepth() const
{
    return mapDepth;
}

void SpatialTree::deserializeSources(
        vector<pair<size_t, MoistureSource*>>& appliedMoisture, 
        std::vector<vector<pair<size_t, ResourceSource*>>>& appliedResource
    )
{
    rootTerrainCell->deserializeSources(appliedMoisture, appliedResource);
}

void SpatialTree::deserializeSpecies(std::vector<ResourceSpecies*>& existingResourceSpecies, std::vector<AnimalSpecies*>& existingAnimalSpecies)
{
    rootTerrainCell->deserializeSpecies(existingResourceSpecies, existingAnimalSpecies);
}

string SpatialTree::getMapPositionHeader() const
{
    string header = static_cast<string>(magic_enum::enum_names<Axis>()[0]);

    for(unsigned int axis = 1; axis < DIMENSIONS; axis++)
    {
        header += "\t" + static_cast<string>(magic_enum::enum_names<Axis>()[axis]);
    }

    return header;
}


void SpatialTree::obtainResourcesBiomass(vector<double> &resourcesBiomass) const
{
    for(size_t i = 0; i < rootTerrainCell->getPatchApplicator().getNumberOfResources(); i++)
    {
        resourcesBiomass[i] = rootTerrainCell->getPatchApplicator().getCellResource(i).getGrowthBuildingBlock().getCurrentTotalWetMass().getValue().getValue();
    }
}

void SpatialTree::registerEdibles(vector<vector<vector<CellResource*>>>& landscapeResources, vector<AnimalNonStatistical*>& landscapeAnimals)
{
    rootTerrainCell->registerEdibles(landscapeResources, landscapeAnimals);
}


void SpatialTree::saveResourceSpeciesSnapshot(ofstream &file, const ResourceSpecies* const &species) const
{
    rootTerrainCell->saveResourceSpeciesSnapshot(file, species);
}


void SpatialTree::saveWaterSnapshot(ofstream &file) const
{
    rootTerrainCell->saveWaterSnapshot(file);
}

SpatialTree::Type SpatialTree::getType() const
{
    return Type::SpatialTree;
}

void SpatialTree::registerCells(std::vector<TerrainCell*>& terrainCells)
{
    rootTerrainCell->registerCells(terrainCells, getNumberOfCellsPerAxis());
}




BOOST_CLASS_EXPORT(SpatialTree)

template <class Archive>
void SpatialTree::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<Map>(*this);
    
    ar & mapDepth;
    ar & MAX_PARALLEL_DEPTH;
    ar & cellSizes;
    ar & axisSizes;

    ar & rootTerrainCell;
}

// Specialisation
template void SpatialTree::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SpatialTree::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SpatialTree::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SpatialTree::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
