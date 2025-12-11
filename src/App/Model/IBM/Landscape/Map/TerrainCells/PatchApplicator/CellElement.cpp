#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/CellElement.h"


using namespace std;





CellElement::CellElement()
{

}

CellElement::CellElement(const size_t patchPriority)
    : patchPriority(patchPriority)
{

}

CellElement::~CellElement()
{

}

size_t CellElement::getPatchPriority() const
{
    return patchPriority;
}

void CellElement::setPatchPriority(const size_t newPatchPriority)
{
    patchPriority = newPatchPriority;
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(CellElement)

template <class Archive>
void CellElement::serialize(Archive& ar, const unsigned int) 
{
    ar & patchPriority;
}

// Specialisation
template void CellElement::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellElement::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellElement::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellElement::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
