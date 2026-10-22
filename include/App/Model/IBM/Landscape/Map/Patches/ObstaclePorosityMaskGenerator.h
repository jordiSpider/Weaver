#ifndef OBSTACLE_POROSITY_MASK_GENERATOR_H
#define OBSTACLE_POROSITY_MASK_GENERATOR_H

#include <cstddef>
#include <cstdint>
#include <vector>

class ObstaclePorosityMaskGenerator {
public:
	struct Parameters {
		double fraction = 0.0;
		double minimumPoreWidth = 0.0;
		double featureSize = 0.0;
		std::size_t octaves = 0u;
	};

	static std::vector<std::uint8_t> generate(
		const std::vector<std::uint8_t>& coveredCells,
		std::size_t gridWidth,
		std::size_t gridHeight,
		double originX,
		double originY,
		double cellWidth,
		double cellHeight,
		const Parameters& parameters,
		unsigned int seed);

	static std::vector<int> generatePatch(
		const std::vector<std::uint8_t>& eligibleCells,
		std::size_t mapWidth,
		std::size_t mapHeight,
		double cellWidth,
		double cellHeight,
		double bottomLeftX,
		double bottomLeftY,
		double topRightX,
		double topRightY,
		const Parameters& parameters,
		unsigned int seed);
};

#if defined(_WIN32) && defined(WEAVER_OBSTACLE_ENGINE_BUILD)
#define WEAVER_OBSTACLE_ENGINE_EXPORT __declspec(dllexport)
#elif !defined(_WIN32) && defined(WEAVER_OBSTACLE_ENGINE_BUILD)
#define WEAVER_OBSTACLE_ENGINE_EXPORT __attribute__((visibility("default")))
#else
#define WEAVER_OBSTACLE_ENGINE_EXPORT
#endif

extern "C" WEAVER_OBSTACLE_ENGINE_EXPORT void weaver_generate_obstacle_porosity(
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
	int* status);

#endif
