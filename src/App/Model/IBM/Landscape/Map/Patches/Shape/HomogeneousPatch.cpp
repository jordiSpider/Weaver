#include "App/Model/IBM/Landscape/Map/Patches/Shape/HomogeneousPatch.h"

using namespace std;
using json = nlohmann::json;




HomogeneousPatch::HomogeneousPatch()
    : PatchShape(new RingModel(), "")
{
    
}


HomogeneousPatch::~HomogeneousPatch()
{

}


Coverage HomogeneousPatch::checkCoverage(const RingModel* const) const
{
    return Coverage::Full;
}

PatchShape::Type HomogeneousPatch::getType() const
{
    return Type::Homogeneous;
}
