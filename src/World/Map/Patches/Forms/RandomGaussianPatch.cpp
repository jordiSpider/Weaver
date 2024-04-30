#include "World/Map/Patches/Forms/RandomGaussianPatch.h"

using namespace std;
using json = nlohmann::json;



RandomGaussianPatch::RandomGaussianPatch(const json &formInfo, const string &filename)
    : FormPatch(new Ring(), "", filename)
{
    
}


RandomGaussianPatch::~RandomGaussianPatch()
{

}


const string RandomGaussianPatch::getFormType() const
{
    return "random gaussian";
}
