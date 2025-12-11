#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/HabitatDomain/CellHabitatDomainInterface.h"


using namespace std;





CellHabitatDomainInterface::CellHabitatDomainInterface()
    : CellElement()
{
    
}

CellHabitatDomainInterface::CellHabitatDomainInterface(const size_t newHabitatDomainPatchPriority)
    : CellElement(newHabitatDomainPatchPriority)
{

}

CellHabitatDomainInterface::~CellHabitatDomainInterface()
{

}


void CellHabitatDomainInterface::applySource(const HabitatDomainSource &source, const size_t habitatDomainPatchPriority)
{
    setPatchPriority(habitatDomainPatchPriority);

    for(size_t idIndex = 0; idIndex < habitatDomainStatus.size(); idIndex++)
    {
        AnimalSpeciesID id(idIndex);

        for(size_t instarIndex = 0; instarIndex < habitatDomainStatus[id].size(); instarIndex++)
        {
            Instar instar = Instar::convertToIndex(static_cast<unsigned int>(instarIndex));

            habitatDomainStatus[id][instar] = habitatDomainStatus[id][instar] || source.getHabitatDomainStatus().at(id).at(instar);
        }
    }
}


void CellHabitatDomainInterface::addAnimalSpecies(const AnimalSpecies& animalSpecies)
{
    habitatDomainStatus.emplace_back(animalSpecies.getGrowthBuildingBlock().getNumberOfInstars(), false);
}


const CustomIndexedVector<AnimalSpeciesID, CustomIndexedVector<Instar, bool>>& CellHabitatDomainInterface::getHabitatDomainStatus() const
{
    return habitatDomainStatus;
}

bool CellHabitatDomainInterface::isHabitatDomain(const AnimalSpeciesID& id, const Instar& instar) const
{
    return habitatDomainStatus[id][instar];
}




BOOST_SERIALIZATION_ASSUME_ABSTRACT(CellHabitatDomainInterface)

template <class Archive>
void CellHabitatDomainInterface::serialize(Archive& ar, const unsigned int) 
{
    ar & boost::serialization::base_object<CellElement>(*this);

    ar & habitatDomainStatus;
}

// Specialisation
template void CellHabitatDomainInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellHabitatDomainInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellHabitatDomainInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellHabitatDomainInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
