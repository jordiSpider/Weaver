/**
 * @file CellValueTraitType.h
 * @brief Defines the CellValueTraitType enum, representing types of cell value traits.
 */

#ifndef CELL_VALUE_TRAIT_TYPE_H_
#define CELL_VALUE_TRAIT_TYPE_H_

/**
 * @enum CellValueTraitType
 * @brief Represents different types of traits related to cell values in the simulation.
 *
 * These traits are used to evaluate cell properties or interactions,
 * such as biomass, conspecific density, or weighting factors.
 */
enum class CellValueTraitType : unsigned char {
    /// Trait representing the evaluation of cell biomass.
    cellEvaluationBiomass,

    /// Trait representing the evaluation of the number of conspecifics in the cell.
    cellEvaluationProConspecific,

    /// Trait representing the weighting of conspecifics in calculations.
    conspecificWeight
};


#endif // CELL_VALUE_TRAIT_TYPE_H_
