
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/Growth/CellResourceInterfaceGrowth.h"


using namespace std;



CellResourceInterfaceGrowth::CellResourceInterfaceGrowth()
	: EdibleGrowth()
{
	
}

CellResourceInterfaceGrowth::CellResourceInterfaceGrowth(CellResourceInterface* owner, ResourceSpeciesGrowth* const newSpeciesGrowth)
	: EdibleGrowth(newSpeciesGrowth, Instar(1)), owner(owner)
{
	setCurrentTotalWetMass(WetMass(0.0));
}

CellResourceInterfaceGrowth::~CellResourceInterfaceGrowth()
{
	
}

const ResourceSpeciesGrowth* CellResourceInterfaceGrowth::getSpeciesGrowth() const
{
	return static_cast<const ResourceSpeciesGrowth*>(EdibleGrowth::getSpeciesGrowth());
}

ResourceSpeciesGrowth* CellResourceInterfaceGrowth::getMutableSpeciesGrowth()
{
	return static_cast<ResourceSpeciesGrowth*>(EdibleGrowth::getMutableSpeciesGrowth());
}

void CellResourceInterfaceGrowth::setOwner(CellResourceInterface* newOwner)
{
	owner = newOwner;
}

const CellResourceInterface* CellResourceInterfaceGrowth::getOwner() const
{
	return owner;
}

CellResourceInterface* CellResourceInterfaceGrowth::getMutableOwner()
{
	return owner;
}

void CellResourceInterfaceGrowth::setCurrentTotalDryMass(const DryMass&)
{
	throwLineInfoException("Error: Do not use this method with the class 'CellResourceInterfaceGrowth'.");
}

void CellResourceInterfaceGrowth::substractBiomass(const DryMass& dryMassToBeSubstracted) 
{ 
	setCurrentTotalWetMass(getCurrentTotalWetMass() - WetMass(dryMassToBeSubstracted, getSpeciesGrowth()->getConversionToWetMass(getInstar())));
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(CellResourceInterfaceGrowth)

template <class Archive>
void CellResourceInterfaceGrowth::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<EdibleGrowth>(*this);
}

// // Specialisation
template void CellResourceInterfaceGrowth::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellResourceInterfaceGrowth::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellResourceInterfaceGrowth::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellResourceInterfaceGrowth::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
