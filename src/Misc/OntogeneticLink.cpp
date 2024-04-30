
#include "Misc/OntogeneticLink.h"


using json = nlohmann::json;

using namespace std;


OntogeneticLink::OntogeneticLink()
    : edible(false), preference(0.0), profitability(0.0)
{

}

OntogeneticLink::~OntogeneticLink()
{

}

void OntogeneticLink::setInfo(const json& linkInfo)
{
    edible = true;
    preference = linkInfo["preference"];
    profitability = linkInfo["profitability"];
}

const bool OntogeneticLink::getEdible() const
{
    return edible;
}

const double& OntogeneticLink::getPreference() const
{
    return preference;
}

const double& OntogeneticLink::getProfitability() const
{
    return profitability;
}
