#include "World/Map/Patches/Forms/CubicPatch.h"

using namespace std;
using json = nlohmann::json;



CubicPatch::CubicPatch(const json &formInfo, const string &filename)
    : FormPatch(
        makeCube(
            formInfo["cubicParams"]["bottomLeftCorner"], 
            formInfo["cubicParams"]["topRightCorner"]
        ),
        makeFormInfo(formInfo["cubicParams"]["bottomLeftCorner"], formInfo["cubicParams"]["topRightCorner"]),
        filename
      )
{
    
}


CubicPatch::~CubicPatch()
{

}

const string CubicPatch::makeFormInfo(const vector<double> &bottomLeftCorner, const vector<double> &topRightCorner) const
{
    string bottomLeftCornerInfo = fmt::format(
        "{}", bottomLeftCorner[0]
    );

    for(unsigned int i = 1; i < bottomLeftCorner.size(); i++)
    {
        bottomLeftCornerInfo += fmt::format(
            ", {}", bottomLeftCorner[i]
        );
    }

    string topRightCornerInfo = fmt::format(
        "{}", topRightCorner[0]
    );

    for(unsigned int i = 1; i < topRightCorner.size(); i++)
    {
        topRightCornerInfo += fmt::format(
            ", {}", topRightCorner[i]
        );
    }

    return fmt::format(
        " - Bottom left corner = ({})\n - Top right corner = ({})", 
        bottomLeftCornerInfo, topRightCornerInfo
    );
}

const string CubicPatch::getFormType() const
{
    return "cubic";
}

const Ring* const CubicPatch::makeCube(const vector<double> &bottomLeftCorner, const vector<double> &topRightCorner)
{
    PointContinuous minCorner, maxCorner;

    for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
    {
        setPositionAxisValue(minCorner, axis, bottomLeftCorner[axis]);
        setPositionAxisValue(maxCorner, axis, topRightCorner[axis]);
    }

    Box boxEffectiveArea(minCorner, maxCorner);

    Ring ringEffectiveArea;
	boost::geometry::convert(boxEffectiveArea, ringEffectiveArea);
    
    return new Ring(ringEffectiveArea);
}
