#include "IBM/World/Map/Patches/Moisture/MoisturePatchFactory.h"


using namespace std;
using json = nlohmann::json;



unique_ptr<MoisturePatch> MoisturePatchFactory::createInstance(const json &moisturePatchInfo, const string &filename)
{
    switch(PatchFactory::Type::stringToEnumValue(moisturePatchInfo["shape"]["type"])) {
		case PatchFactory::Type::Homogeneous: {
			return make_unique<HomogeneousMoisturePatch>(moisturePatchInfo, filename);
			break;
		}
		case PatchFactory::Type::Gaussian: {
			return make_unique<GaussianMoisturePatch>(moisturePatchInfo, filename);
			break;
		}
		case PatchFactory::Type::Spherical: {
			return make_unique<SphericalMoisturePatch>(moisturePatchInfo, filename);
			break;
		}
		case PatchFactory::Type::Cubic: {
			return make_unique<CubicMoisturePatch>(moisturePatchInfo, filename);
			break;
		}
		case PatchFactory::Type::RandomGaussian: {
			return make_unique<RandomGaussianMoisturePatch>(moisturePatchInfo, filename);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}


unique_ptr<MoisturePatch> MoisturePatchFactory::createInstance(const json &moisturePatchInfo)
{
	return make_unique<MoistureBasePatch>(moisturePatchInfo);
}

template <class Archive>
void MoisturePatchFactory::serialize(Archive &ar, const unsigned int version) 
{
	
}
