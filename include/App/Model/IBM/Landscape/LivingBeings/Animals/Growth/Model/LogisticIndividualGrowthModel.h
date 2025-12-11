/**
 * @file LogisticIndividualGrowthModel.h
 * @brief Defines the LogisticIndividualGrowthModel class, which implements a logistic growth model.
 */

#ifndef LOGISTIC_INDIVIDUAL_GROWTH_MODEL_H_
#define LOGISTIC_INDIVIDUAL_GROWTH_MODEL_H_

#include <cmath>

#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/IndividualGrowthModel.h"


/**
 * @class LogisticIndividualGrowthModel
 * @brief Implements a logistic individual growth model.
 * 
 * This class models the growth of an individual using a logistic function.
 * It inherits from IndividualGrowthModel and provides methods to adjust parameters, calculate
 * length at a given age, and access key model parameters such as Linf and Xmid.
 */
class LogisticIndividualGrowthModel: public IndividualGrowthModel {
public:
    /**
     * @brief Default constructor.
     */
    LogisticIndividualGrowthModel();

    /**
     * @brief Destructor.
     */
    virtual ~LogisticIndividualGrowthModel();

    /**
     * @brief Get the Linf parameter of the logistic model.
     * @return The asymptotic maximum length (Linf).
     */
    const PreciseDouble& getLinf() const;

    /**
     * @brief Get the Xmid parameter of the logistic model.
     * @return The Xmid value (age at inflection point).
     */
    const PreciseDouble& getXmid() const;

    /**
     * @brief Calculates the length of an individual at a given age using the logistic function.
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
     * @return IndividualGrowthModel::Type::Logistic
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
    PreciseDouble Linf; /**< Asymptotic maximum length of the individual. */
    PreciseDouble xmid; /**< Age at the inflection point of the logistic curve. */

    /**
     * @brief Computes the logistic growth function for a given rate k and time t.
     * @param k Growth rate parameter.
     * @param t Time (age).
     * @return Result of the logistic growth function.
     */
    PreciseDouble Logistic(const PreciseDouble& k, const Day& t) const;
};

#endif /* LOGISTIC_INDIVIDUAL_GROWTH_MODEL_H_ */
