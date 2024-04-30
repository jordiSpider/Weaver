
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

template<class Archive>
void OntogeneticLink::serialize(Archive & ar, const unsigned int version) {
    ar & edible;
    ar & preference;
    ar & profitability;
}

// Specialisation
template void OntogeneticLink::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void OntogeneticLink::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void OntogeneticLink::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void OntogeneticLink::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
