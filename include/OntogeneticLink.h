#ifndef WEAVER_ONTOGENETIC_LINK_H_
#define WEAVER_ONTOGENETIC_LINK_H_


#include <nlohmann/json.hpp>


struct OntogeneticLink {
    bool edible;
    double preference;
    double profitability;

    OntogeneticLink();
    OntogeneticLink(const nlohmann::json& linkInfo);
    ~OntogeneticLink();

    OntogeneticLink& operator=(const OntogeneticLink& other);
};


#endif /* WEAVER_ONTOGENETIC_LINK_H_ */
