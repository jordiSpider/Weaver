/**
 * @file SensoryModel.h
 * @brief Defines the SensoryModel class, representing a probabilistic sensory detection model.
 */

#ifndef SENSORY_MODEL_H_
#define SENSORY_MODEL_H_


#include <nlohmann/json.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <cmath>

#include "App/Model/IBM/Maths/PreciseDouble.h"


/**
 * @class SensoryModel
 * @brief Represents a sensory detection model used to calculate the probability
 * of detecting an object based on distance.
 *
 * This model uses a decay function to represent the decrease in detection
 * probability with increasing distance, controlled by a decay parameter beta.
 */
class SensoryModel {
public:
    /**
     * @brief Default constructor.
     */
    SensoryModel();

    /**
     * @brief Constructor from JSON configuration.
     * 
     * @param config JSON object containing the parameters of the sensory model.
     */
    SensoryModel(const nlohmann::json& config);
    
    /**
     * @brief Virtual destructor.
     */
    virtual ~SensoryModel();

    // Delete copy constructor
    SensoryModel(const SensoryModel&) = delete;

    // Delete assignment operator
	SensoryModel& operator=(const SensoryModel&) = delete;

    /**
     * @brief Calculates the probability of detection given a distance.
     *
     * The probability decreases with distance according to the sensory model.
     *
     * @param distance Distance to the object.
     * @param maximumDistance Maximum detection distance.
     * @return Detection probability as PreciseDouble.
     */
    PreciseDouble calculateProbability(const PreciseDouble& distance, const PreciseDouble& maximumDistance) const;

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
    /// Small epsilon value for numerical stability.
    static inline const double EPSILON = 1e-5;

    /// Decay parameter controlling the decrease in detection probability with distance.
    PreciseDouble beta;

    /**
     * @brief Calculates the decay rate for the detection probability.
     *
     * This internal function determines how fast the probability decreases
     * as distance increases.
     *
     * @param maximumDistance Maximum detection distance.
     * @return Decay rate as PreciseDouble.
     */
    PreciseDouble calculateDecayRate(const PreciseDouble& maximumDistance) const;
};

#endif /* SENSORY_MODEL_H_ */
