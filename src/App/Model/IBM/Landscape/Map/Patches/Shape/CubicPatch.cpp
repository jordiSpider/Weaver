#include "App/Model/IBM/Landscape/Map/Patches/Shape/CubicPatch.h"

using namespace std;
using json = nlohmann::json;


CubicPatch::CubicPatch(const json &shapeConfig)
    : PatchShape(
        generateInfluenceZone(
            shapeConfig["cubicParams"]["bottomLeftCorner"].get<std::vector<double>>(),
            shapeConfig["cubicParams"]["topRightCorner"].get<std::vector<double>>()
        ),
        generateDescription(
            shapeConfig["cubicParams"]["bottomLeftCorner"].get<std::vector<double>>(),
            shapeConfig["cubicParams"]["topRightCorner"].get<std::vector<double>>()
        )
      )
{
    
}

CubicPatch::CubicPatch(const vector<double>& bottomLeftCorner, const vector<double>& topRightCorner)
    : PatchShape(
        generateInfluenceZone(bottomLeftCorner, topRightCorner),
        "No description"
      )
{
    
}

CubicPatch::~CubicPatch()
{

}

const string CubicPatch::generateDescription(const vector<double> &bottomLeftCorner, const vector<double> &topRightCorner) const
{
    std::ostringstream newDescription;

    newDescription << " - Bottom left corner = (";

    newDescription << bottomLeftCorner[0];

    for(size_t i = 1; i < bottomLeftCorner.size(); i++)
    {
        newDescription << ", " << bottomLeftCorner[i];
    }

    newDescription << ")\n";

    newDescription << " - Top right corner = (";

    newDescription << topRightCorner[0];

    for(size_t i = 1; i < topRightCorner.size(); i++)
    {
        newDescription << ", " << topRightCorner[i];
    }

    newDescription << ")";

    return newDescription.str();
}

const RingModel* CubicPatch::generateInfluenceZone(const vector<double> &bottomLeftCorner, const vector<double> &topRightCorner)
{
    PointContinuous minCorner, maxCorner;

    for(unsigned char axis = 0; axis < DIMENSIONS; axis++)
    {
        setPositionAxisValue(minCorner, axis, bottomLeftCorner[axis]);
        setPositionAxisValue(maxCorner, axis, topRightCorner[axis]);
    }

    BoxModel boxInfluenceZone(minCorner, maxCorner);

    RingModel ringInfluenceZone;
	boost::geometry::convert(boxInfluenceZone, ringInfluenceZone);
    
    return new RingModel(ringInfluenceZone);
}

PatchShape::Type CubicPatch::getType() const
{
    return Type::Cubic;
}
