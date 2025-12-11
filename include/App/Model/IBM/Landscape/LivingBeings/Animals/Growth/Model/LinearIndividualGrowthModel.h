/**
 * @file LinearIndividualGrowthModel.h
 * @brief Defines the LinearIndividualGrowthModel class, which implements a linear growth model.
 */

#ifndef LINEAR_INDIVIDUAL_GROWTH_MODEL_H_
#define LINEAR_INDIVIDUAL_GROWTH_MODEL_H_

#include <cmath>

#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/IndividualGrowthModel.h"


/**
 * @class LinearIndividualGrowthModel
 * @brief Implements a linear individual growth model.
 * 
 * This class models the growth of an individual using a linear function. It inherits from 
 * IndividualGrowthModel and provides methods to adjust parameters, calculate length at a given age,
 * and retrieve the initial length at birth.
 */
class LinearIndividualGrowthModel: public IndividualGrowthModel {
public:
    /**
     * @brief Default constructor.
     */
    LinearIndividualGrowthModel();

    /**
     * @brief Destructor.
     */
    virtual ~LinearIndividualGrowthModel();

    /**
     * @brief Get the length at birth.
     * @return The initial length of the individual at birth.
     */
    const Length& getLengthAtBirth() const;

    /**
     * @brief Calculates the length of an individual at a given age using the linear growth formula.
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
     * @return IndividualGrowthModel::Type::Linear
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
    Length lengthAtBirth; /**< Initial length at birth of the individual. */

    /**
     * @brief Computes the linear growth function for a given rate k and time t.
     * @param k Growth rate parameter.
     * @param t Time (age).
     * @return Result of the linear growth function.
     */
    PreciseDouble Linear(const PreciseDouble& k, const Day& t) const;
};

#endif /* LINEAR_INDIVIDUAL_GROWTH_MODEL_H_ */
