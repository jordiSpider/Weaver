#include "Misc/OntogeneticLink.h"


using namespace std;


OntogeneticLink::OntogeneticLink()
    : edible(false), preference(0.0), profitability(0.0)
{

}

OntogeneticLink::~OntogeneticLink()
{

}

void OntogeneticLink::setInfo(const PreciseDouble& newPreference, const PreciseDouble& newProfitability)
{
    edible = true;
    preference = newPreference;
    profitability = newProfitability;
}

bool OntogeneticLink::getEdible() const
{
    return edible;
}

const PreciseDouble& OntogeneticLink::getPreference() const
{
    return preference;
}

const PreciseDouble& OntogeneticLink::getProfitability() const
{
    return profitability;
}


template<class Archive>
void OntogeneticLink::serialize(Archive & ar, const unsigned int) {
    ar & edible;
    ar & preference;
    ar & profitability;
}

// Specialisation
template void OntogeneticLink::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void OntogeneticLink::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void OntogeneticLink::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void OntogeneticLink::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
