/*
 * ResourceSpore.h
 *
 *  Created on: Oct 04, 2023
 *      Author: jrbcast
 */

#ifndef RESOURCE_SPORE_H_
#define RESOURCE_SPORE_H_


#include "IBM/World/Map/TerrainCells/TerrainCell.h"
#include "Exceptions/LineInfoException.h"


/**
 * @file ResourceSpore.h
 * @brief Header file for the ResourceSpore class.
 */

/**
 * @class ResourceSpore
 * @brief Represents a fungus spore.
 *
 * This class provides a representation of a fungus spore, including its properties and behavior.
 */
class ResourceSpore {

private:
    double weight; // The weight of the spore in milligrams.
    double size;   // The size of the spore in micrometers.
    TerrainCell * terrainCell; // The world the spore lives in. 
public:
    /**
     * @brief Construct a new ResourceSpore object with the given weight and size.
     * @param weight The weight of the spore in milligrams.
     * @param size The size of the spore in micrometers.
     * @param myTerrainCell The cell the spore is in. 
     */
    ResourceSpore(double weight, double size, TerrainCell * myTerrainCell);

    /**
     * @brief Destroy the ResourceSpore object.
     */
    ~ResourceSpore();

    /**
     * @brief Get the size of the spore.
     * @return The size of the spore in micrometers.
     */
    double getSize() const;

    /**
     * @brief Set the size of the spore.
     * @param size The size of the spore in micrometers.
     */
    void setSize(double size);

    /**
     * @brief Get the weight of the spore.
     * @return The weight of the spore in milligrams.
     */
    double getWeight() const;

    /**
     * @brief Set the weight of the spore.
     * @param weight The weight of the spore in milligrams.
     */
    void setWeight(double weight);

   /**
     * @brief Get the position of the spore.
     * @return The position of the spore as a TerrainCell.
     */
    TerrainCell* getTerrainCell();

    /**
     * @brief Set the position of the spore.
     * @param cell The position of the spore as a TerrainCell.
     */
    void setTerrainCell(TerrainCell * cell);

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
    bool release(const double xw, const double yw, const double ws);

    /**
     * @brief Perform spore germination.
     *
     * This function simulates the germination of the spore, leading to fungal growth.
     */
    void germinate();

};

#endif /* RESOURCE_SPORE_H_ */