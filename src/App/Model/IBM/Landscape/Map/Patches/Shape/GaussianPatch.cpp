#include "App/Model/IBM/Landscape/Map/Patches/Shape/GaussianPatch.h"

using namespace std;
using json = nlohmann::json;


GaussianPatch::GaussianPatch(const json &)
    : PatchShape(new RingModel(), "")
{
    
}

GaussianPatch::~GaussianPatch()
{

}

PatchShape::Type GaussianPatch::getType() const
{
    return Type::Gaussian;
}
