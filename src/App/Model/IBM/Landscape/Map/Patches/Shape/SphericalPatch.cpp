#include "App/Model/IBM/Landscape/Map/Patches/Shape/SphericalPatch.h"

using namespace std;
using json = nlohmann::json;


SphericalPatch::SphericalPatch(const json &shapeConfig)
    : PatchShape(
        generateInfluenceZone(
            shapeConfig["sphericalParams"]["center"].get<std::vector<double>>(),
            shapeConfig["sphericalParams"]["radius"].get<double>()
        ),
        generateDescription(
            shapeConfig["sphericalParams"]["center"].get<std::vector<double>>(),
            shapeConfig["sphericalParams"]["radius"].get<double>()
        )
      )
{
    
}

SphericalPatch::~SphericalPatch()
{

}

const string SphericalPatch::generateDescription(const vector<double> &center, const double &radius) const
{
    std::ostringstream newDescription;

    newDescription << " - Center = (";

    newDescription << center[0];

    for(unsigned int i = 1; i < center.size(); i++)
    {
        newDescription << ", " << center[i];
    }

    newDescription << ")\n";
    
    newDescription << " - Radius = " << radius;

    return newDescription.str();
}

const RingModel* SphericalPatch::generateInfluenceZone(const vector<double> &center, const double &radius)
{
    PointContinuous centerPoint;
    for(unsigned char axis = 0; axis < DIMENSIONS; axis++)
    {
        setPositionAxisValue(centerPoint, axis, center[axis]);
    }

    auto sphere = Geometry::makeSphere(centerPoint, radius);

    return new RingModel(*sphere);
}

PatchShape::Type SphericalPatch::getType() const
{
    return Type::Spherical;
}
