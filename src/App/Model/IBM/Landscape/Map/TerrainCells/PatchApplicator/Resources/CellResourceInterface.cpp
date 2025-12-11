
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/CellResourceInterface.h"

#include "App/Model/IBM/Landscape/Map/SpatialTree.h"

using namespace std;



CellResourceInterface::CellResourceInterface()
    : Edible(), CellElement()
{
	
}


CellResourceInterface::CellResourceInterface(const EdibleID id, TerrainCell* terrainCell, const size_t newResourcePatchPriority, ResourceSpecies& resourceSpecies)
    : Edible(id, &resourceSpecies, terrainCell), CellElement(newResourcePatchPriority)
{
	
}


CellResourceInterface::~CellResourceInterface()
{

}


bool CellResourceInterface::isHunting() const 
{ 
	return false; 
}

const ResourceSpecies* CellResourceInterface::getSpecies() const 
{ 
	return static_cast<const ResourceSpecies*>(Edible::getSpecies()); 
}

ResourceSpecies* CellResourceInterface::getMutableSpecies() 
{ 
	return static_cast<ResourceSpecies*>(Edible::getMutableSpecies()); 
}

PreciseDouble CellResourceInterface::getVoracity() const 
{ 
	return 0.0; 
}

void CellResourceInterface::increasePredationEncounters() 
{
	
}

DryMass CellResourceInterface::turnIntoDryMass(const DryMass &targetDryMass, const PreciseDouble& predatorRemainingVoracity) const
{
	return DryMass(fmin(predatorRemainingVoracity, targetDryMass.getValue()));
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(CellResourceInterface)

template <class Archive>
void CellResourceInterface::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<Edible>(*this);
	ar & boost::serialization::base_object<CellElement>(*this);
}

// // Specialisation
template void CellResourceInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellResourceInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellResourceInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellResourceInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
