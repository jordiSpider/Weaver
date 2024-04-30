#include "IBM/World/Map/Patches/Forms/SphericalPatch.h"

using namespace std;
using json = nlohmann::json;



SphericalPatch::SphericalPatch(const json &formInfo, const string &filename)
    : FormPatch(
        makeSphere(
            formInfo["sphericalParams"]["center"], 
            formInfo["sphericalParams"]["radius"]
        ),
        makeFormInfo(
            formInfo["sphericalParams"]["center"],
            formInfo["sphericalParams"]["radius"]
        ),
        filename
      )
{
    
}


SphericalPatch::~SphericalPatch()
{

}


const Ring* const SphericalPatch::makeSphere(const vector<double> &center, const double &radius)
{
    PointContinuous centerPoint;
    for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
    {
        setPositionAxisValue(centerPoint, axis, center[axis]);
    }

    auto sphere = Geometry::makeSphere(centerPoint, radius);

    return new Ring(*sphere);
}

const string SphericalPatch::makeFormInfo(const vector<double> &center, const double &radius) const
{
    string centerInfo = fmt::format(
        "{}", center[0]
    );

    for(unsigned int i = 1; i < center.size(); i++)
    {
        centerInfo += fmt::format(
            ", {}", center[i]
        );
    }

    return fmt::format(
        " - Center = ({})\n - Radius = {}", 
        centerInfo, radius
    );
}

const string SphericalPatch::getFormType() const
{
    return "spherical";
}

template <class Archive>
void SphericalPatch::serialize(Archive &ar, const unsigned int version) 
{

}
