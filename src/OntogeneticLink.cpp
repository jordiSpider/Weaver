
#include "OntogeneticLink.h"


using json = nlohmann::json;

using namespace std;


OntogeneticLink::OntogeneticLink()
    : edible(false), preference(0.0), profitability(0.0)
{

}

OntogeneticLink::OntogeneticLink(const json& linkInfo)
    : edible(linkInfo["edible"]), preference(linkInfo["preference"]), profitability(linkInfo["profitability"])
{

}

OntogeneticLink& OntogeneticLink::operator=(const OntogeneticLink& other) {
    this->edible = other.edible;
    this->preference = other.preference;
    this->profitability = other.profitability;

    return *this;
}

OntogeneticLink::~OntogeneticLink()
{

}
