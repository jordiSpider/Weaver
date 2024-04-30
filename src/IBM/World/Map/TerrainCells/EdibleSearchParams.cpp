
#include "IBM/World/Map/TerrainCells/EdibleSearchParams.h"

using namespace std;



EdibleSearchParams::EdibleSearchParams()
    : animalSearchParams(), resourceSearchParams()
{
    
}

EdibleSearchParams::~EdibleSearchParams()
{
    
}

void EdibleSearchParams::addAnimalSearchParams(
        const WorldInterface* const worldInterface,
        const vector<LifeStage> &searchableLifeStages,
        const vector<AnimalSpecies::AnimalID> &searchableAnimalSpecies,
        const vector<Instar> &searchableInstars,
        const vector<AnimalSpecies::Gender> &searchableGenders)
{
    animalSearchParams.addSearchParams(
        worldInterface, searchableLifeStages, searchableAnimalSpecies, 
        searchableInstars, searchableGenders
    );
}

void EdibleSearchParams::addResourceSearchParams(const WorldInterface* const worldInterface, const vector<ResourceSpecies::ResourceID> &searchableResourceSpecies)
{
    resourceSearchParams.addSearchParams(worldInterface, searchableResourceSpecies);
}

const AnimalSearchParams& EdibleSearchParams::getAnimalSearchParams() const
{
    return animalSearchParams;
}

const ResourceSearchParams& EdibleSearchParams::getResourceSearchParams() const
{
    return resourceSearchParams;
}

template <class Archive>
void EdibleSearchParams::serialize(Archive &ar, const unsigned int version) {
    ar & animalSearchParams;
    ar & resourceSearchParams;
}


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, EdibleSearchParams* &edibleSearchParamsPtr, const unsigned int version) {
            // Deserialization
            if (Archive::is_loading::value) {
				ar & *edibleSearchParamsPtr;
            } // Serialization
            else {
                ar & *edibleSearchParamsPtr;
            }
        }

		// Specialisation
		template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, EdibleSearchParams*&, const unsigned int);
		template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, EdibleSearchParams*&, const unsigned int);

		template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, EdibleSearchParams*&, const unsigned int);
		template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, EdibleSearchParams*&, const unsigned int);
    }
}
