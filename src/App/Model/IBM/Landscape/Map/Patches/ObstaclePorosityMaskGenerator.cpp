#include "App/Model/IBM/Landscape/Map/Patches/ObstaclePorosityMaskGenerator.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <noise/module/perlin.h>

using namespace std;


vector<uint8_t> ObstaclePorosityMaskGenerator::generate(
	const vector<uint8_t>& coveredCells,
	const size_t gridWidth,
	const size_t gridHeight,
	const double originX,
	const double originY,
	const double cellWidth,
	const double cellHeight,
	const Parameters& parameters,
	unsigned int seed)
{
	if(gridHeight != 0u && gridWidth > numeric_limits<size_t>::max() / gridHeight)
	{
		throw invalid_argument("Porosity grid dimensions overflow.");
	}

	const size_t numberOfCells = gridWidth * gridHeight;
	if(coveredCells.size() != numberOfCells)
	{
		throw invalid_argument("Porosity coverage mask size does not match the grid dimensions.");
	}

	if(!isfinite(cellWidth) || !isfinite(cellHeight) || cellWidth <= 0.0 || cellHeight <= 0.0)
	{
		throw invalid_argument("Porosity grid cell dimensions must be greater than zero.");
	}

	if(!isfinite(parameters.fraction) || parameters.fraction < 0.0 || parameters.fraction > 1.0)
	{
		throw invalid_argument("Porosity fraction must be between zero and one.");
	}

	vector<uint8_t> porousCells(numberOfCells, 0u);
	if(parameters.fraction == 0.0 || numberOfCells == 0u)
	{
		return porousCells;
	}

	if(!isfinite(parameters.minimumPoreWidth) || parameters.minimumPoreWidth <= 0.0 ||
		!isfinite(parameters.featureSize) || parameters.featureSize <= 0.0 || parameters.octaves == 0u)
	{
		throw invalid_argument("Porosity requires a positive minimum pore width, feature size and octave count.");
	}

	const double poreWidthValue = ceil(parameters.minimumPoreWidth / cellWidth);
	const double poreHeightValue = ceil(parameters.minimumPoreWidth / cellHeight);
	if(!isfinite(poreWidthValue) || !isfinite(poreHeightValue) ||
		poreWidthValue > static_cast<double>(numeric_limits<size_t>::max()) ||
		poreHeightValue > static_cast<double>(numeric_limits<size_t>::max()))
	{
		throw invalid_argument("Minimum pore width exceeds the representable grid dimensions.");
	}
	const size_t poreWidth = max<size_t>(1u, static_cast<size_t>(poreWidthValue));
	const size_t poreHeight = max<size_t>(1u, static_cast<size_t>(poreHeightValue));
	if(poreWidth > gridWidth || poreHeight > gridHeight)
	{
		throw invalid_argument("Map is too small to contain a pore of minimumPoreWidth.");
	}
	if(poreHeight != 0u && poreWidth > numeric_limits<size_t>::max() / poreHeight)
	{
		throw invalid_argument("Minimum pore area overflows the supported grid dimensions.");
	}
	const size_t minimumPoreArea = poreWidth * poreHeight;

	noise::module::Perlin noiseField;
	noiseField.SetSeed(static_cast<int>(seed % static_cast<unsigned int>(numeric_limits<int>::max())));
	noiseField.SetFrequency(1.0 / parameters.featureSize);
	noiseField.SetOctaveCount(static_cast<int>(parameters.octaves));
	noiseField.SetPersistence(0.5);

	vector<double> noiseValues(numberOfCells, 0.0);
	size_t coveredCellCount = 0u;
	for(size_t index = 0u; index < numberOfCells; ++index)
	{
		if(coveredCells[index] == 0u)
		{
			continue;
		}
		const size_t x = index % gridWidth;
		const size_t y = index / gridWidth;
		const double worldX = originX + (static_cast<double>(x) + 0.5) * cellWidth;
		const double worldY = originY + (static_cast<double>(y) + 0.5) * cellHeight;
		noiseValues[index] = noiseField.GetValue(worldX, worldY, 0.0);
		++coveredCellCount;
	}
	if(coveredCellCount == 0u)
	{
		return porousCells;
	}

	const size_t targetPoreArea = static_cast<size_t>(llround(parameters.fraction * static_cast<double>(coveredCellCount)));
	if(targetPoreArea == 0u)
	{
		return porousCells;
	}

	const size_t candidateWidth = gridWidth - poreWidth + 1u;
	const size_t candidateHeight = gridHeight - poreHeight + 1u;
	if(candidateHeight != 0u && candidateWidth > numeric_limits<size_t>::max() / candidateHeight)
	{
		throw invalid_argument("Pore candidate grid dimensions overflow.");
	}
	const size_t candidateCount = candidateWidth * candidateHeight;
	vector<double> candidateScores(candidateCount, numeric_limits<double>::infinity());
	vector<uint8_t> candidateExists(candidateCount, 0u);
	for(size_t y = 0u; y < candidateHeight; ++y)
	{
		for(size_t x = 0u; x < candidateWidth; ++x)
		{
			double score = 0.0;
			bool fullyCovered = true;
			for(size_t dy = 0u; dy < poreHeight && fullyCovered; ++dy)
			{
				for(size_t dx = 0u; dx < poreWidth; ++dx)
				{
					const size_t cellIndex = (y + dy) * gridWidth + x + dx;
					if(coveredCells[cellIndex] == 0u)
					{
						fullyCovered = false;
						break;
					}
					score += noiseValues[cellIndex];
				}
			}
			if(fullyCovered)
			{
				const size_t candidateIndex = y * candidateWidth + x;
				candidateExists[candidateIndex] = 1u;
				candidateScores[candidateIndex] = score / static_cast<double>(minimumPoreArea);
			}
		}
	}

	vector<uint8_t> queued(candidateCount, 0u);
	vector<size_t> frontier;
	const auto addCandidate = [&](const size_t x, const size_t y) {
		if(x >= candidateWidth || y >= candidateHeight)
		{
			return;
		}
		const size_t index = y * candidateWidth + x;
		if(candidateExists[index] != 0u && queued[index] == 0u)
		{
			queued[index] = 1u;
			frontier.push_back(index);
		}
	};

	for(size_t y = 0u; y < candidateHeight; ++y)
	{
		for(size_t x = 0u; x < candidateWidth; ++x)
		{
			const size_t index = y * candidateWidth + x;
			if(candidateExists[index] == 0u)
			{
				continue;
			}
			bool touchesExterior = x == 0u || y == 0u || x + poreWidth == gridWidth || y + poreHeight == gridHeight;
			for(size_t dy = 0u; dy < poreHeight && !touchesExterior; ++dy)
			{
				touchesExterior = (x > 0u && coveredCells[(y + dy) * gridWidth + x - 1u] == 0u) ||
					(x + poreWidth < gridWidth && coveredCells[(y + dy) * gridWidth + x + poreWidth] == 0u);
			}
			for(size_t dx = 0u; dx < poreWidth && !touchesExterior; ++dx)
			{
				touchesExterior = (y > 0u && coveredCells[(y - 1u) * gridWidth + x + dx] == 0u) ||
					(y + poreHeight < gridHeight && coveredCells[(y + poreHeight) * gridWidth + x + dx] == 0u);
			}
			if(touchesExterior)
			{
				addCandidate(x, y);
			}
		}
	}

	size_t selectedArea = 0u;
	while(!frontier.empty() && selectedArea < max(minimumPoreArea, targetPoreArea))
	{
		const auto best = min_element(frontier.begin(), frontier.end(), [&candidateScores](const size_t lhs, const size_t rhs) {
			if(candidateScores[lhs] == candidateScores[rhs])
			{
				return lhs < rhs;
			}
			return candidateScores[lhs] < candidateScores[rhs];
		});
		const size_t candidateIndex = *best;
		frontier.erase(best);
		const size_t x = candidateIndex % candidateWidth;
		const size_t y = candidateIndex / candidateWidth;
		for(size_t dy = 0u; dy < poreHeight; ++dy)
		{
			for(size_t dx = 0u; dx < poreWidth; ++dx)
			{
				const size_t cellIndex = (y + dy) * gridWidth + x + dx;
				if(porousCells[cellIndex] == 0u)
				{
					porousCells[cellIndex] = 1u;
					++selectedArea;
				}
			}
		}
		for(size_t offset = 1u; offset < poreWidth; ++offset)
		{
			if(x >= offset) addCandidate(x - offset, y);
			if(offset < candidateWidth - x) addCandidate(x + offset, y);
		}
		for(size_t offset = 1u; offset < poreHeight; ++offset)
		{
			if(y >= offset) addCandidate(x, y - offset);
			if(offset < candidateHeight - y) addCandidate(x, y + offset);
		}
	}
	return porousCells;
}

vector<int> ObstaclePorosityMaskGenerator::generatePatch(
	const vector<uint8_t>& eligibleCells,
	const size_t mapWidth,
	const size_t mapHeight,
	const double cellWidth,
	const double cellHeight,
	const double bottomLeftX,
	const double bottomLeftY,
	const double topRightX,
	const double topRightY,
	const Parameters& parameters,
	const unsigned int seed)
{
	constexpr double effectiveAreaEpsilon = 1e-9;
	if(mapHeight != 0u && mapWidth > numeric_limits<size_t>::max() / mapHeight)
	{
		throw invalid_argument("Map dimensions overflow.");
	}
	const size_t mapCellCount = mapWidth * mapHeight;
	if(eligibleCells.size() != mapCellCount || mapWidth == 0u || mapHeight == 0u ||
		!isfinite(cellWidth) || !isfinite(cellHeight) || cellWidth <= 0.0 || cellHeight <= 0.0 ||
		!isfinite(bottomLeftX) || !isfinite(bottomLeftY) || !isfinite(topRightX) || !isfinite(topRightY) ||
		topRightX <= bottomLeftX || topRightY <= bottomLeftY)
	{
		throw invalid_argument("Map, cell size and patch corners must define a valid grid and patch.");
	}

	vector<int> patchCells(mapCellCount, -1);
	vector<uint8_t> coverage(mapCellCount, 0u);
	size_t minX = mapWidth;
	size_t minY = mapHeight;
	size_t maxX = 0u;
	size_t maxY = 0u;
	bool hasEligibleCell = false;
	for(size_t y = 0u; y < mapHeight; ++y)
	{
		for(size_t x = 0u; x < mapWidth; ++x)
		{
			const size_t index = y * mapWidth + x;
			if(eligibleCells[index] == 0u) continue;
			const double cellLeft = static_cast<double>(x) * cellWidth;
			const double cellBottom = static_cast<double>(y) * cellHeight;
			const double effectiveCellWidth = cellWidth - effectiveAreaEpsilon;
			const double effectiveCellHeight = cellHeight - effectiveAreaEpsilon;
			const double overlapX = max(0.0, min(cellLeft + effectiveCellWidth, topRightX) - max(cellLeft, bottomLeftX));
			const double overlapY = max(0.0, min(cellBottom + effectiveCellHeight, topRightY) - max(cellBottom, bottomLeftY));
			if(overlapX * overlapY < 0.5 * effectiveCellWidth * effectiveCellHeight) continue;
			coverage[index] = 1u;
			minX = min(minX, x);
			minY = min(minY, y);
			maxX = max(maxX, x);
			maxY = max(maxY, y);
			hasEligibleCell = true;
		}
	}
	if(!hasEligibleCell) return patchCells;

	for(size_t index = 0u; index < mapCellCount; ++index)
	{
		if(coverage[index] != 0u) patchCells[index] = 0;
	}
	const size_t patchWidth = maxX - minX + 1u;
	const size_t patchHeight = maxY - minY + 1u;
	vector<uint8_t> localCoverage(patchWidth * patchHeight, 0u);
	for(size_t y = minY; y <= maxY; ++y)
	{
		for(size_t x = minX; x <= maxX; ++x)
		{
			localCoverage[(y - minY) * patchWidth + x - minX] = coverage[y * mapWidth + x];
		}
	}
	const vector<uint8_t> localPorosity = generate(localCoverage, patchWidth, patchHeight, 0.0, 0.0,
		cellWidth, cellHeight, parameters, seed);
	for(size_t y = minY; y <= maxY; ++y)
	{
		for(size_t x = minX; x <= maxX; ++x)
		{
			const size_t mapIndex = y * mapWidth + x;
			if(coverage[mapIndex] != 0u && localPorosity[(y - minY) * patchWidth + x - minX] != 0u)
			{
				patchCells[mapIndex] = 1;
			}
		}
	}
	return patchCells;
}

void weaver_generate_obstacle_porosity(
	const int* eligibleCells,
	const int* mapWidth,
	const int* mapHeight,
	const double* cellWidth,
	const double* cellHeight,
	const double* bottomLeftX,
	const double* bottomLeftY,
	const double* topRightX,
	const double* topRightY,
	const double* fraction,
	const double* minimumPoreWidth,
	const double* featureSize,
	const int* octaves,
	const int* seed,
	int* patchCells,
	int* status)
{
	if(status == nullptr) return;
	*status = 1;
	try
	{
		if(mapWidth == nullptr || mapHeight == nullptr || cellWidth == nullptr || cellHeight == nullptr ||
			bottomLeftX == nullptr || bottomLeftY == nullptr || topRightX == nullptr || topRightY == nullptr ||
			fraction == nullptr || minimumPoreWidth == nullptr || featureSize == nullptr || octaves == nullptr ||
			seed == nullptr || eligibleCells == nullptr || patchCells == nullptr || *mapWidth <= 0 || *mapHeight <= 0 ||
			*octaves <= 0 || *seed < 0)
		{
			throw std::invalid_argument("Grid dimensions, octave count and buffers must be valid.");
		}
		const size_t width = static_cast<size_t>(*mapWidth);
		const size_t height = static_cast<size_t>(*mapHeight);
		if(width > std::numeric_limits<size_t>::max() / height)
		{
			throw std::invalid_argument("Porosity grid dimensions overflow.");
		}
		const size_t count = width * height;
		std::vector<std::uint8_t> eligible(count);
		for(size_t index = 0u; index < count; ++index)
		{
			eligible[index] = eligibleCells[index] != 0 ? 1u : 0u;
		}
		const ObstaclePorosityMaskGenerator::Parameters parameters{
			*fraction, *minimumPoreWidth, *featureSize, static_cast<size_t>(*octaves)
		};
		const std::vector<int> result = ObstaclePorosityMaskGenerator::generatePatch(
			eligible, width, height, *cellWidth, *cellHeight, *bottomLeftX, *bottomLeftY,
			*topRightX, *topRightY, parameters, static_cast<unsigned int>(*seed));
		for(size_t index = 0u; index < count; ++index)
		{
			patchCells[index] = result[index];
		}
		*status = 0;
	}
	catch(const std::invalid_argument&)
	{
		*status = 1;
	}
	catch(...)
	{
		*status = 2;
	}
}
