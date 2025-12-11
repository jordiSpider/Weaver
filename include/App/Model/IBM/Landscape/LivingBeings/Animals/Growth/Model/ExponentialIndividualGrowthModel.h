/**
 * @file ExponentialIndividualGrowthModel.h
 * @brief Defines the ExponentialIndividualGrowthModel class, which implements an exponential growth model.
 */

#ifndef EXPONENTIAL_INDIVIDUAL_GROWTH_MODEL_H_
#define EXPONENTIAL_INDIVIDUAL_GROWTH_MODEL_H_

#include <cmath>

#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/IndividualGrowthModel.h"


/**
 * @class ExponentialIndividualGrowthModel
 * @brief Implements an exponential individual growth model.
 * 
 * This class models the growth of an individual using an exponential function. It inherits from
 * IndividualGrowthModel and provides methods to adjust parameters, calculate length at a given age,
 * and retrieve model-specific parameters.
 */
class ExponentialIndividualGrowthModel: public IndividualGrowthModel {
public:
    /**
     * @brief Default constructor.
     */
    ExponentialIndividualGrowthModel();

    /**
     * @brief Destructor.
     */
    virtual ~ExponentialIndividualGrowthModel();

    /**
     * @brief Get the growth parameter 'a'.
     * @return The value of parameter 'a'.
     */
    const PreciseDouble& getA() const;

    /**
     * @brief Get the growth parameter 'b'.
     * @return The value of parameter 'b'.
     */
    const PreciseDouble& getB() const;

    /**
     * @brief Calculates the length of an individual at a given age using the exponential growth formula.
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
     * @return IndividualGrowthModel::Type::Exponential
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
    PreciseDouble a; /**< Parameter 'a' of the exponential growth model */
    PreciseDouble b; /**< Parameter 'b' of the exponential growth model */

    /**
     * @brief Computes the exponential growth function for a given rate k and time t.
     * @param k Growth rate parameter.
     * @param t Time (age).
     * @return Result of the exponential function.
     */
    PreciseDouble Exponential(const PreciseDouble& k, const Day& t) const;
};

#endif /* EXPONENTIAL_INDIVIDUAL_GROWTH_MODEL_H_ */
