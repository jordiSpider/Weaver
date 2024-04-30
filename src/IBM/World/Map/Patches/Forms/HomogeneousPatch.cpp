#include "IBM/World/Map/Patches/Forms/HomogeneousPatch.h"

using namespace std;
using json = nlohmann::json;




HomogeneousPatch::HomogeneousPatch(const string &filename)
    : FormPatch(new Ring(), "", filename)
{
    
}


HomogeneousPatch::~HomogeneousPatch()
{

}


const double HomogeneousPatch::calculateCoveragePercent(const Ring &cellRing) const
{
    return 1.0;
}

const string HomogeneousPatch::getFormType() const
{
    return "homogeneous";
}

template <class Archive>
void HomogeneousPatch::serialize(Archive &ar, const unsigned int version) 
{
    
}
