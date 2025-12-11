#include "App/Model/IBM/Landscape/Map/Patches/Shape/PatchShape.h"

#include "App/Model/IBM/Landscape/Map/Patches/Shape/CubicPatch.h"
#include "App/Model/IBM/Landscape/Map/Patches/Shape/GaussianPatch.h"
#include "App/Model/IBM/Landscape/Map/Patches/Shape/HomogeneousPatch.h"
#include "App/Model/IBM/Landscape/Map/Patches/Shape/RandomGaussianPatch.h"
#include "App/Model/IBM/Landscape/Map/Patches/Shape/SphericalPatch.h"


using namespace std;
using json = nlohmann::json;


PatchShape* PatchShape::createInstance(const json &shapeConfig)
{
	switch(EnumClass<Type>::stringToEnumValue(shapeConfig["type"].get<string>())) {
		case Type::Homogeneous: {
			return new HomogeneousPatch();
			break;
		}
		case Type::Gaussian: {
			return new GaussianPatch(shapeConfig);
			break;
		}
		case Type::Spherical: {
			return new SphericalPatch(shapeConfig);
			break;
		}
		case Type::Cubic: {
			return new CubicPatch(shapeConfig);
			break;
		}
		case Type::RandomGaussian: {
			return new RandomGaussianPatch(shapeConfig);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}


PatchShape::PatchShape(const RingModel* const influenceZone, const string description)
    : influenceZone(influenceZone),
	  description(description)
{
    
}

PatchShape::~PatchShape()
{
	delete influenceZone;
}

const string_view PatchShape::getDescription() const
{
	return description;
}

const RingModel* PatchShape::getInfluenceZone() const
{
	return influenceZone;
}

Coverage PatchShape::checkCoverage(const RingModel* const cellBounds) const
{
	return Geometry::checkFirstCoverageLevelBySecond(cellBounds, getInfluenceZone(), true);
}
