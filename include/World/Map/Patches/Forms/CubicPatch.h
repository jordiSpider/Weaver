#ifndef CUBIC_PATCH_H_
#define CUBIC_PATCH_H_

#include <nlohmann/json.hpp>
#include <boost/geometry/algorithms/convert.hpp>

#include "World/Map/Patches/Forms/FormPatch.h"
#include "World/Map/Points/PointContinuous.h"
#include "World/Map/Geometry/Geometry.h"
#include "Misc/Macros.h"



class CubicPatch : public FormPatch {
protected:
    const std::string makeFormInfo(const std::vector<double> &bottomLeftCorner, const std::vector<double> &topRightCorner) const;
    const std::string getFormType() const;
    const Ring* const makeCube(const std::vector<double> &bottomLeftCorner, const std::vector<double> &topRightCorner);

public:
    CubicPatch(const nlohmann::json &formInfo, const std::string &filename);
    virtual ~CubicPatch();
};

#endif /* CUBIC_PATCH_H_ */