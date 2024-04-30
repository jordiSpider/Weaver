
#include "IBM/World/LivingBeings/Resources/Spores/ResourceSpore.h"


/**
 * @brief Construct a new ResourceSpore object with the given weight and size.
 * @param weight The weight of the spore in milligrams.
 * @param size The size of the spore in micrometers.
 */


ResourceSpore::ResourceSpore(double weight, double size, TerrainCell *myTerrainCell)
{
    // Additional constructor initialization if needed
    this->weight = weight;
    this->size = size;
    this->terrainCell = myTerrainCell;
}

/**
 * @brief Destroy the ResourceSpore object.
 */
ResourceSpore::~ResourceSpore()
{
    // Destructor code, if needed
    this->terrainCell = NULL;
}

/**
 * @brief Get the size of the spore.
 * @return The size of the spore in micrometers.
 */
double ResourceSpore::getSize() const
{
    return size;
}

/**
 * @brief Set the size of the spore.
 * @param size The size of the spore in micrometers.
 */
void ResourceSpore::setSize(double size)
{
    this->size = size;
}

/**
 * @brief Get the position of the spore.
 * @return The position of the spore as a TerrainCell.
 */
TerrainCell * ResourceSpore::getTerrainCell()
{
    return this->terrainCell;
}

/**
 * @brief Set the position of the spore.
 * @param cell The position of the spore as a TerrainCell.
 */
void ResourceSpore::setTerrainCell(TerrainCell *cell)
{
    this->terrainCell = cell;
}

/**
 * @brief Release the spore into the environment.
 *
 * This function simulates the release of the spore into the environment. It will then travel a certain distance depending
 * on weather conditions (wind speed and wind direction).
 *
 * @param xw The X-component of wind direction
 * @param yw The Y-component of wind direction
 * @param ws The Wind speed in meters per second
 *
 * @return A boolean representing wether the spore has landed within the world limits
 */
bool ResourceSpore::release(const double xw, const double yw, const double ws)
{
    // Normalize wind direction components
    double magnitude = std::sqrt(xw * xw + yw * yw);
    double normalized_xw = xw / magnitude;
    double normalized_yw = yw / magnitude;

    // Scattering parameters
    const double xScatterStd = 1.0; // Standard deviation for x-axis scattering
    const double yScatterStd = 1.0; // Standard deviation for y-axis scattering

    // Simulate spore travel
    // Initial position at (0, 0)
    double xi = terrainCell->getX();
    double yi = terrainCell->getY();

    // Initialize final positions
    double finalX = xi;
    double finalY = yi;

    // Calculate travel distance based on wind speed
    std::default_random_engine generator;
    std::normal_distribution<double> distanceDistribution(ws, ws / 2.0);
    double distance = distanceDistribution(generator);

    // Calculate horizontal movement
    double horizontalMovement = distance * normalized_xw;

    // Calculate vertical movement
    double verticalMovement = distance * normalized_yw;

    // Apply scattering in both x and y dimensions
    std::normal_distribution<double> xScatterDistribution(0.0, xScatterStd);
    std::normal_distribution<double> yScatterDistribution(0.0, yScatterStd);
    double xScatter = xScatterDistribution(generator);
    double yScatter = yScatterDistribution(generator);

    // Update final positions with scattering
    finalX += horizontalMovement + xScatter;
    finalY += verticalMovement + yScatter;

    // Ensure non-negative final positions
    finalX = std::max(finalX, 0.0);
    finalY = std::max(finalY, 0.0);

    // Returns true if the spore lands in the world and false otherwise. 
    //return this->terrainCell->world->isWithinWorldLimits(0, finalY, finalX);
    return true;
}

/**
 * @brief Perform spore germination.
 *
 * This function simulates the germination of the spore, leading to fungal growth.
 */
void ResourceSpore::germinate()
{
    throwLineInfoException("Implementation for spore germination");
}

/**
 * @brief Get the weight of the spore.
 * @return The weight of the spore in milligrams.
 */
double ResourceSpore::getWeight() const
{
    return weight;
}

/**
 * @brief Set the weight of the spore.
 * @param weight The weight of the spore in milligrams.
 */
void ResourceSpore::setWeight(double weight)
{
    this->weight = weight;
}