/**
 * @file PawarElement.h
 * @brief Defines the PawarElement enum, representing elements of the Pawar metabolic model.
 */

#ifndef PAWAR_ELEMENT_H_
#define PAWAR_ELEMENT_H_

/**
 * @enum PawarElement
 * @brief Represents the key parameters in the Pawar model used for metabolic rate calculations.
 *
 * This enum is used to identify which parameter of the Pawar metabolic model a trait corresponds to.
 */
enum class PawarElement : unsigned char
{
    /// Activation energy (E) for the metabolic process.
    activationEnergy,

    /// Energy decay rate or deactivation energy.
    energyDecay,

    /// Optimal temperature for the metabolic process.
    temperatureOptimal,

    /// Reference temperature used in metabolic calculations.
    temperatureRef
};

#endif // PAWAR_ELEMENT_H_
