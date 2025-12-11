#include "App/Model/IBM/Landscape/LivingBeings/Growth/EdibleGrowth.h"


using namespace std;




EdibleGrowth::EdibleGrowth()
    : speciesGrowth(nullptr)
{

}

EdibleGrowth::EdibleGrowth(SpeciesGrowth* const speciesGrowth, const Instar &instar)
	: instar(instar), speciesGrowth(speciesGrowth)
{
	
}

EdibleGrowth::~EdibleGrowth()
{
	
}

const SpeciesGrowth* EdibleGrowth::getSpeciesGrowth() const
{
    return speciesGrowth;
}

SpeciesGrowth* EdibleGrowth::getMutableSpeciesGrowth()
{
    return speciesGrowth;
}

const Instar& EdibleGrowth::getInstar() const 
{ 
	return instar; 
}

void EdibleGrowth::setInstar(const Instar& newInstar)
{
	instar = newInstar;
}

void EdibleGrowth::setSpeciesGrowth(SpeciesGrowth* newSpeciesGrowth)
{
    speciesGrowth = newSpeciesGrowth;
}

const DryMass& EdibleGrowth::getCurrentTotalDryMass() const
{
    return currentTotalDryMass;
}

const WetMass& EdibleGrowth::getCurrentTotalWetMass() const
{
    return currentTotalWetMass;
}

WetMass EdibleGrowth::calculateWetMass(const DryMass &dryMass) const
{
    return WetMass(dryMass, getSpeciesGrowth()->getConversionToWetMass(getInstar()));
}

void EdibleGrowth::setCurrentTotalDryMass(const DryMass& newCurrentTotalDryMass)
{
    currentTotalDryMass = newCurrentTotalDryMass;
    currentTotalWetMass = WetMass(getCurrentTotalDryMass(), getSpeciesGrowth()->getConversionToWetMass(getInstar()));
}

void EdibleGrowth::setCurrentTotalWetMass(const WetMass& newCurrentTotalWetMass)
{
    currentTotalWetMass = newCurrentTotalWetMass;
    currentTotalDryMass = DryMass(getCurrentTotalWetMass(), getSpeciesGrowth()->getConversionToWetMass(getInstar()));
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(EdibleGrowth)

template <class Archive>
void EdibleGrowth::serialize(Archive &ar, const unsigned int) {
    ar & instar;

    ar & currentTotalDryMass;
    ar & currentTotalWetMass;
}

// Specialisation
template void EdibleGrowth::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void EdibleGrowth::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void EdibleGrowth::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void EdibleGrowth::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
