#include "IBM/World/LivingBeings/Animals/AnimalInterface.h"

#include "IBM/World/Map/MapInterface.h"
#include "IBM/World/Map/TerrainCells/TerrainCellInterface.h"
#include "IBM/World/LivingBeings/Animals/SpatialTreeAnimal.h"

using namespace std;



AnimalInterface::AnimalInterface(TerrainCellInterface* const terrainCellInterface)
    : Edible(terrainCellInterface)
{

}

AnimalInterface::AnimalInterface(Species* const mySpecies, const Instar &instar, const bool temporary) 
    : Edible(mySpecies, instar, temporary)
{
	
}

AnimalInterface::AnimalInterface(Species* const mySpecies, const Instar &instar, const bool temporary, TerrainCellInterface* parentTerrainCellInterface) 
    : Edible(mySpecies, parentTerrainCellInterface, instar, temporary)
{
	
}

AnimalInterface::~AnimalInterface()
{

}

AnimalSpecies* const AnimalInterface::getSpecies() const 
{ 
	return static_cast<AnimalSpecies* const>(Edible::getSpecies()); 
}

template <class Archive>
void AnimalInterface::serialize(Archive &ar, const unsigned int version) 
{
    ar & boost::serialization::base_object<Edible>(*this);
}

// Specialisation
template void AnimalInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, AnimalInterface* &animalInterfacePtr, const unsigned int version, 
                const MapInterface& mapInterface, TerrainCellInterface* const terrainCellInterface) {
            
            switch(mapInterface.getMapType()) {
				case MapInterface::Type::SpatialTree: {
					if(Archive::is_loading::value) 
                    {
                        animalInterfacePtr = SpatialTreeAnimal::createInstance(terrainCellInterface).release();
                    }
                    
                    static_cast<SpatialTreeAnimal*>(animalInterfacePtr)->serialize(ar, version);
					break;
				}
				default: {
					throwLineInfoException("Default case");
					break;
				}
			}
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, AnimalInterface*&, const unsigned int, const MapInterface&, TerrainCellInterface* const);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, AnimalInterface*&, const unsigned int, const MapInterface&, TerrainCellInterface* const);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, AnimalInterface*&, const unsigned int, const MapInterface&, TerrainCellInterface* const);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, AnimalInterface*&, const unsigned int, const MapInterface&, TerrainCellInterface* const);
    }
}
