#ifndef GAUSSIAN_PATCH_H_
#define GAUSSIAN_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/FormPatch.h"
#include "World/Map/Points/PointContinuous.h"



class GaussianPatch : public FormPatch {
protected:
    const std::string getFormType() const;

public:
    GaussianPatch(const nlohmann::json &formInfo, const std::string &filename);
    virtual ~GaussianPatch();
};

#endif /* GAUSSIAN_PATCH_H_ */