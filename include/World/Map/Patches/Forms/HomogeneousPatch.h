#ifndef HOMOGENEOUS_PATCH_H_
#define HOMOGENEOUS_PATCH_H_

#include <nlohmann/json.hpp>
#include <boost/geometry/algorithms/area.hpp>

#include "World/Map/Patches/Forms/FormPatch.h"
#include "World/Map/Points/PointContinuous.h"
#include "World/Map/Geometry/Coverage.h"



class HomogeneousPatch : public FormPatch {
protected:
    const std::string getFormType() const;

public:
    HomogeneousPatch(const std::string &filename);
    virtual ~HomogeneousPatch();

    const double calculateCoveragePercent(const Ring &cellRing) const override;
};

#endif /* HOMOGENEOUS_PATCH_H_ */