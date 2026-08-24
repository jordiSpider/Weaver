#include "App/Model/IBM/Landscape/Map/Patches/Shape/RandomGaussianPatch.h"

using namespace std;
using json = nlohmann::json;


RandomGaussianPatch::RandomGaussianPatch(const json &)
    : PatchShape(RingModel(), "")
{
    
}

RandomGaussianPatch::~RandomGaussianPatch()
{

}

PatchShape::Type RandomGaussianPatch::getType() const
{
    return Type::RandomGaussian;
}
