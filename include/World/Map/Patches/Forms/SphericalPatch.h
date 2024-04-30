#ifndef SPHERICAL_PATCH_H_
#define SPHERICAL_PATCH_H_

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "World/Map/Patches/Forms/FormPatch.h"
#include "World/Map/Points/PointContinuous.h"
#include "World/Map/Points/Axis.h"
#include "World/Map/Geometry/Geometry.h"
#include "Misc/Macros.h"



class SphericalPatch : public FormPatch {
protected:
    const Ring* const makeSphere(const std::vector<double> &center, const double &radius);
    const std::string makeFormInfo(const std::vector<double> &center, const double &radius) const;
    const std::string getFormType() const;

public:
    SphericalPatch(const nlohmann::json &formInfo, const std::string &filename);
    virtual ~SphericalPatch();
};

#endif /* SPHERICAL_PATCH_H_ */