
#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/HabitatDomain/CellHabitatDomain.h"


using namespace std;




CellHabitatDomain::CellHabitatDomain()
    : CellHabitatDomainInterface() 
{

}

CellHabitatDomain::CellHabitatDomain(const size_t newHabitatDomainPatchPriority)
    : CellHabitatDomainInterface(newHabitatDomainPatchPriority) 
{

}

CellHabitatDomain::~CellHabitatDomain()
{
    
}



BOOST_CLASS_EXPORT(CellHabitatDomain)

template <class Archive>
void CellHabitatDomain::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<CellHabitatDomainInterface>(*this);
}

// Specialisation
template void CellHabitatDomain::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellHabitatDomain::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellHabitatDomain::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellHabitatDomain::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
