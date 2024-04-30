#include "IBM/World/Map/Patches/Resource/ResourcePatchFactory.h"


using namespace std;
using json = nlohmann::json;



unique_ptr<const ResourcePatch> ResourcePatchFactory::createInstance(const json &resourcePatchInfo, ResourceSpecies* const species, const string &filename)
{
    switch(PatchFactory::Type::stringToEnumValue(resourcePatchInfo["shape"]["type"])) {
		case PatchFactory::Type::Homogeneous: {
			return make_unique<const HomogeneousResourcePatch>(resourcePatchInfo, species, filename);
			break;
		}
		case PatchFactory::Type::Gaussian: {
			return make_unique<const GaussianResourcePatch>(resourcePatchInfo, species, filename);
			break;
		}
		case PatchFactory::Type::Spherical: {
			return make_unique<const SphericalResourcePatch>(resourcePatchInfo, species, filename);
			break;
		}
		case PatchFactory::Type::Cubic: {
			return make_unique<const CubicResourcePatch>(resourcePatchInfo, species, filename);
			break;
		}
		case PatchFactory::Type::RandomGaussian: {
			return make_unique<const RandomGaussianResourcePatch>(resourcePatchInfo, species, filename);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}
