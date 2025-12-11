/**
 * @file OntogeneticLink.h
 * @brief Defines the OntogeneticLink class representing an ontogenetic relationship with edible, preference, and profitability attributes.
 *
 * This class is used in the IBM model to represent the link between organisms or resources.
 * It stores whether the link is edible, the preference value, and the profitability value.
 * It also supports Boost.Serialization for saving/loading.
 */

#ifndef ONTOGENETIC_LINK_H_
#define ONTOGENETIC_LINK_H_


#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>

#include "App/Model/IBM/Maths/PreciseDouble.h"


/**
 * @class OntogeneticLink
 * @brief Represents an ontogenetic link with attributes edible, preference, and profitability.
 *
 * Each link defines whether it can be consumed (edible), how much it is preferred
 * by the consumer (preference), and how profitable it is (profitability).
 */
class OntogeneticLink {
private:
    bool edible;                 /**< Indicates if the link is edible. */
    PreciseDouble preference;    /**< Preference value of the link. */
    PreciseDouble profitability; /**< Profitability value of the link. */

public:
    /**
     * @brief Default constructor.
     *
     * Initializes the OntogeneticLink with default values.
     */
    OntogeneticLink();

    /**
     * @brief Destructor.
     */
    ~OntogeneticLink();

    /**
     * @brief Sets the attributes of the link.
     *
     * @param newPreference The preference value.
     * @param newProfitability The profitability value.
     */
    void setInfo(const PreciseDouble& newPreference, const PreciseDouble& newProfitability);

    /**
     * @brief Returns whether the link is edible.
     * @return True if edible, false otherwise.
     */
    bool getEdible() const;

    /**
     * @brief Returns the preference value of the link.
     * @return Reference to the preference value.
     */
    const PreciseDouble& getPreference() const;

    /**
     * @brief Returns the profitability value of the link.
     * @return Reference to the profitability value.
     */
    const PreciseDouble& getProfitability() const;

    /**
     * @brief Serialize the object for Boost.Serialization.
     *
     * Converts the class members to/from the archive.
     *
     * @tparam Archive Type of the archive (binary_oarchive, text_oarchive, etc.).
     * @param ar Archive object.
     * @param version Version of serialization (unused).
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


#endif /* ONTOGENETIC_LINK_H_ */
