#ifndef ONTOGENETIC_LINK_H_
#define ONTOGENETIC_LINK_H_


#include <nlohmann/json.hpp>


class OntogeneticLink {
private:
    bool edible;
    double preference;
    double profitability;

public:
    OntogeneticLink();
    ~OntogeneticLink();

    void setInfo(const nlohmann::json& linkInfo);

    const bool getEdible() const;
    const double& getPreference() const;
    const double& getProfitability() const;
};


#endif /* ONTOGENETIC_LINK_H_ */
