#ifndef ONTOGENETIC_LINK_H_
#define ONTOGENETIC_LINK_H_


#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <nlohmann/json.hpp>


/**
 * @class OntogeneticLink
 * @brief A class representing an ontogenetic link with edible, preference, and profitability attributes.
 */
class OntogeneticLink {
private:
    bool edible; // Indicates if the link is edible.
    double preference; // The preference value of the link.
    double profitability; // The profitability value of the link.

    friend class boost::serialization::access;

public:
    /**
     * @brief Default constructor for OntogeneticLink.
     */
    OntogeneticLink();

    /**
     * @brief Destructor for OntogeneticLink.
     */
    ~OntogeneticLink();

    /**
     * @brief Set information for the OntogeneticLink using JSON data.
     * @param linkInfo The JSON data containing link information.
     */
    void setInfo(const nlohmann::json& linkInfo);

    /**
     * @brief Get the edible status of the link.
     * @return True if the link is edible, false otherwise.
     */
    const bool getEdible() const;

    /**
     * @brief Get the preference value of the link.
     * @return The preference value.
     */
    const double& getPreference() const;

    /**
     * @brief Get the profitability value of the link.
     * @return The profitability value.
     */
    const double& getProfitability() const;

    /**
     * @brief Serialize the object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


#endif /* ONTOGENETIC_LINK_H_ */
