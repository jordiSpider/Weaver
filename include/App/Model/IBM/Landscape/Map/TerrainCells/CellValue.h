/**
 * @file CellValue.h
 * @brief Defines the CellValue struct representing the evaluation of a terrain cell by an animal.
 */

#ifndef CELL_VALUE_H_
#define CELL_VALUE_H_

#include <utility>
#include <memory>
#include <fstream>
#include <ostream>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalNonStatistical.h"
#include "App/Model/IBM/Landscape/Map/SpatialTree/Points/PointSpatialTree.h"
#include "App/Model/IBM/Landscape/LivingBeings/Edible.h"
#include "App/Model/IBM/Landscape/Map/Geometry/Geometry.h"
#include "App/Model/IBM/Landscape/Map/Geometry/Coverage.h"


/**
 * @struct CellValue
 * @brief Stores the evaluation of a terrain cell from the perspective of an animal.
 *
 * This struct is used to compare terrain cells based on perceived value, coverage,
 * number of females, predatory risk, habitat domain, and other criteria relevant
 * for animal decision-making in the simulation.
 */
struct CellValue {
    /// The most suitable edible in the cell according to the evaluation
    const Edible* bestEdibility;

    /// Whether the cell is fully covered by the edible area
    bool fullCoverage;

    /// Effective area of the cell used for coverage calculations
    const RingModel* cellEffectiveArea;

    /// Position of the cell in the map
    const PointMap* const cellPosition;

    /// Perceived value of the cell by the evaluating animal
    PreciseDouble valuePerceived;

    /// Number of mature females in the cell
    unsigned int numberOfFemales;

    /// Whether the cell belongs to the habitat domain of the evaluating animal
    bool habitatDomain;

    /**
     * @brief Constructor that evaluates a cell for a given animal.
     * 
     * @param animalWhoIsEvaluating The animal performing the evaluation
     * @param bestEdibility Pointer to the most suitable edible in the cell
     * @param fullCoverage True if the edible fully covers the cell
     * @param cellEffectiveArea Effective area of the cell
     * @param cellPosition Pointer to the position of the cell
     * @param cellCenter Center coordinates of the cell
     * @param edibilityValue Perceived edibility value of the cell
     * @param predatoryRiskEdibilityValue Value modifier due to predatory risk
     * @param conspecificBiomass Biomass of conspecifics in the cell
     * @param numberOfFemales Number of mature females present in the cell
     * @param habitatDomain Whether the cell belongs to the habitat domain
     */
    CellValue(const AnimalNonStatistical& animalWhoIsEvaluating, const Edible* bestEdibility, bool fullCoverage, const RingModel* cellEffectiveArea, const PointMap* const cellPosition, const PointContinuous& cellCenter, const PreciseDouble &edibilityValue, const PreciseDouble &predatoryRiskEdibilityValue, const PreciseDouble &conspecificBiomass, const unsigned int numberOfFemales, const bool habitatDomain);
    
    /**
     * @brief Destructor.
     */
    ~CellValue();

    /**
     * @brief Copy constructor.
     * 
     * @param other Another CellValue object to copy from
     */
    CellValue(const CellValue& other);
    
    /**
     * @brief Copy assignment operator is deleted.
     */
    CellValue& operator=(const CellValue&) = delete;

    /**
     * @brief Greater-than operator based on perceived value and other criteria.
     * 
     * @param other Another CellValue to compare with
     * @return True if this cell is evaluated as greater than the other
     */
	bool operator>(const CellValue& other) const;
    
    /**
     * @brief Greater-than-or-equal operator based on perceived value and other criteria.
     * 
     * @param other Another CellValue to compare with
     * @return True if this cell is evaluated as greater than or equal to the other
     */
    bool operator>=(const CellValue& other) const;
};

#endif // CELL_VALUE_H_