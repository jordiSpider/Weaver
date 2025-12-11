/**
 * @file IndividualGrowthModel.h
 * @brief Defines the IndividualGrowthModel class, an abstract base class for modeling
 *        individual animal growth using different growth functions.
 */

#ifndef INDIVIDUAL_GROWTH_MODEL_H_
#define INDIVIDUAL_GROWTH_MODEL_H_


#include <memory>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "Exceptions/LineInfoException.h"
#include "App/Model/IBM/Landscape/LivingBeings/StructuralUnits.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"


/**
 * @class IndividualGrowthModel
 * @brief Abstract base class for representing an individual animal growth model.
 * 
 * This class provides an interface for various growth models (linear, von Bertalanffy, logistic, exponential, 4-parameter logistic)
 * that calculate the growth of an individual over time. It defines the essential methods for adjusting parameters and
 * calculating the length of an individual at a given age.
 */
class IndividualGrowthModel {
public:
    /**
     * @brief Enumeration of supported growth model types.
     */
    enum class Type : unsigned char {
        Linear,        /**< Linear growth model */
        VonBertalanffy,/**< Von Bertalanffy growth model */
        Logistic,      /**< Logistic growth model */
        Exponential,   /**< Exponential growth model */
        Logistic4P     /**< 4-parameter logistic growth model */
    };

    /**
     * @brief Factory method to create an instance of a specific growth model.
     * 
     * @param type The type of growth model to create.
     * @param A Initial growth parameter.
     * @return Pointer to the created IndividualGrowthModel instance.
     */
    static IndividualGrowthModel* createInstance(const Type& type, const PreciseDouble& A);

    /**
     * @brief Default constructor.
     */
    IndividualGrowthModel();

    /**
     * @brief Virtual destructor.
     */
    virtual ~IndividualGrowthModel();

    /**
     * @brief Adjusts the growth model parameters.
     * 
     * @param growthCoefficient Growth coefficient specific to the model.
     * @param Lmax Maximum length achievable by the individual.
     * @param Tmax Maximum age (or time) for growth.
     * @param lengthAtBirth Initial length at birth.
     */
    virtual void adjust(const PreciseDouble& growthCoefficient, const Length& Lmax, const Day& Tmax, const Length& lengthAtBirth)=0;

    /**
     * @brief Calculates the length of the individual at a given age.
     * 
     * @param age Age (or time) at which to calculate length.
     * @param growthCoefficient Growth coefficient specific to the model.
     * @return The calculated length.
     */
    virtual Length calculateLength(const Day& age, const PreciseDouble& growthCoefficient) const=0;

    /**
     * @brief Returns the type of the growth model.
     * 
     * @return The growth model type (Linear, VonBertalanffy, Logistic, Exponential, Logistic4P).
     */
    virtual IndividualGrowthModel::Type getType() const=0;

    /**
      * @brief Serializes the object for persistence.
      *
      * @tparam Archive Serialization archive type.
      * @param ar Archive instance.
      * @param version Serialization version.
      */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* INDIVIDUAL_GROWTH_MODEL_H_ */
