/**
 * @file VonBertalanffyIndividualGrowthModel.h
 * @brief Defines the VonBertalanffyIndividualGrowthModel class, which implements the von Bertalanffy growth model.
 */

#ifndef VON_BERTALANFFY_INDIVIDUAL_GROWTH_MODEL_H_
#define VON_BERTALANFFY_INDIVIDUAL_GROWTH_MODEL_H_

#include <cmath>

#include <boost/serialization/export.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/IndividualGrowthModel.h"


/**
 * @class VonBertalanffyIndividualGrowthModel
 * @brief Implements an individual growth model using the von Bertalanffy equation.
 * 
 * This class models the growth of an individual using the von Bertalanffy growth function.
 * It inherits from IndividualGrowthModel and provides methods to adjust parameters, calculate
 * length at a given age, and access key model parameters such as Linf and T0.
 */
class VonBertalanffyIndividualGrowthModel: public IndividualGrowthModel {
public:
    /**
     * @brief Default constructor.
     */
    VonBertalanffyIndividualGrowthModel();

    /**
     * @brief Destructor.
     */
    virtual ~VonBertalanffyIndividualGrowthModel();

    /**
     * @brief Get the Linf parameter of the von Bertalanffy model.
     * @return The asymptotic maximum length (Linf).
     */
    const PreciseDouble& getLinf() const;

    /**
     * @brief Get the T0 parameter of the von Bertalanffy model.
     * @return The theoretical age at which length is zero (T0).
     */
    const PreciseDouble& getT0() const;

    /**
     * @brief Calculates the length of an individual at a given age using the von Bertalanffy function.
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
     * @return IndividualGrowthModel::Type::VonBertalanffy
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
    PreciseDouble t0;   /**< Theoretical age at which length is zero. */

    /**
     * @brief Computes the von Bertalanffy growth function for a given rate k and time t.
     * @param k Growth rate parameter.
     * @param t Time (age).
     * @return Result of the von Bertalanffy growth function.
     */
    PreciseDouble VonBertalanffy(const PreciseDouble& k, const Day& t) const;
};

#endif /* VON_BERTALANFFY_INDIVIDUAL_GROWTH_MODEL_H_ */
