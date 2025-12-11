#include "App/Model/IBM/Landscape/LivingBeings/Edible.h"


using namespace std;




Edible::Edible()
	: mySpecies(nullptr)
{

}

Edible::Edible(const EdibleID id, Species* const mySpecies, TerrainCell* terrainCell) 
	: id(id), mySpecies(mySpecies), terrainCell(terrainCell)
{
	
}


Edible::~Edible()
{
	
}



const EdibleID& Edible::getId() const
{
	return id;
}


const Species* Edible::getSpecies() const 
{ 
	return mySpecies; 
}

Species* Edible::getMutableSpecies() 
{ 
	return mySpecies; 
}

const TerrainCell* Edible::getTerrainCell() const
{ 
	return terrainCell; 
}


TerrainCell* Edible::getMutableTerrainCell() const
{
	return terrainCell;
}


void Edible::setTerrainCell(TerrainCell* newTerrainCell)
{ 
	terrainCell = newTerrainCell; 
}

void Edible::setSpecies(Species* newSpecies)
{
	mySpecies = newSpecies;

	setSpeciesGrowth(&newSpecies->getMutableGrowthBuildingBlock());
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(Edible)

template <class Archive>
void Edible::serialize(Archive &ar, const unsigned int) {
	ar & id;
}

// // Specialisation
template void Edible::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Edible::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Edible::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Edible::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
