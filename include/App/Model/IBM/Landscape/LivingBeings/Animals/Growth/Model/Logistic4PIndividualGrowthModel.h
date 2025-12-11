/**
 * @file Logistic4PIndividualGrowthModel.h
 * @brief Defines the Logistic4PIndividualGrowthModel class, which implements a 4-parameter logistic growth model.
 */

#ifndef LOGISTIC_4_P_INDIVIDUAL_GROWTH_MODEL_H_
#define LOGISTIC_4_P_INDIVIDUAL_GROWTH_MODEL_H_

#include <cmath>

#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/IndividualGrowthModel.h"


/**
 * @class Logistic4PIndividualGrowthModel
 * @brief Implements a 4-parameter logistic individual growth model.
 * 
 * This class models the growth of an individual using a four-parameter logistic function.
 * It inherits from IndividualGrowthModel and provides methods to adjust parameters, calculate
 * length at a given age, and access key model parameters such as A, B, and Xmid.
 */
class Logistic4PIndividualGrowthModel: public IndividualGrowthModel {
public:
    /**
     * @brief Default constructor.
     */
    Logistic4PIndividualGrowthModel();

    /**
     * @brief Constructor with initial maximum growth parameter A.
     * @param A Maximum asymptotic growth value.
     */
    Logistic4PIndividualGrowthModel(const PreciseDouble& A);

    /**
     * @brief Destructor.
     */
    virtual ~Logistic4PIndividualGrowthModel();

    /**
     * @brief Get the Xmid parameter of the logistic model.
     * @return The Xmid value (age at inflection point).
     */
    const PreciseDouble& getXmid() const;

    /**
     * @brief Get the A parameter of the logistic model.
     * @return The A value (upper asymptote of growth).
     */
    const PreciseDouble& getA() const;

    /**
     * @brief Get the B parameter of the logistic model.
     * @return The B value (shape parameter).
     */
    const PreciseDouble& getB() const;

    /**
     * @brief Calculates the length of an individual at a given age using the 4-parameter logistic function.
     * @param age Age of the individual.
     * @param growthCoefficient Growth coefficient for scaling the model.
     * @return The length of the individual at the specified age.
     */
    Length calculateLength(const Day& age, const PreciseDouble& growthCoefficient) const;

    /**
     * @brief Adjusts the model parameters based on maximum length, maximum age, and length at birth.
     * @param growthCoefficient Growth coefficient for scaling.
     * @param Lmax Maximum achievable length.
     * @param Tmax Maximum age for growth.
     * @param lengthAtBirth Initial length at birth.
     */
    void adjust(const PreciseDouble& growthCoefficient, const Length& Lmax, const Day& Tmax, const Length& lengthAtBirth);

    /**
     * @brief Returns the type of the growth model.
     * @return IndividualGrowthModel::Type::Logistic4P
     */
    IndividualGrowthModel::Type getType() const;

    /**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    PreciseDouble xmid; /**< Age at inflection point of the logistic curve. */
    PreciseDouble A;    /**< Upper asymptote (maximum growth) of the logistic curve. */
    PreciseDouble B;    /**< Shape parameter controlling the steepness of the curve. */

    /**
     * @brief Computes the 4-parameter logistic growth function for a given rate k and time t.
     * @param k Growth rate parameter.
     * @param t Time (age).
     * @return Result of the 4-parameter logistic function.
     */
    PreciseDouble Logistic4P(const PreciseDouble& k, const Day& t) const;
};

#endif /* LOGISTIC_4_P_INDIVIDUAL_GROWTH_MODEL_H_ */
