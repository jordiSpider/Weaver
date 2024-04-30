#include "IBM/World/Map/Patches/Forms/GaussianPatch.h"

using namespace std;
using json = nlohmann::json;



GaussianPatch::GaussianPatch(const json &formInfo, const string &filename)
    : FormPatch(new Ring(), "", filename)
{
    
}


GaussianPatch::~GaussianPatch()
{

}


const string GaussianPatch::getFormType() const
{
    return "gaussian";
}

template <class Archive>
void GaussianPatch::serialize(Archive &ar, const unsigned int version) 
{

}
