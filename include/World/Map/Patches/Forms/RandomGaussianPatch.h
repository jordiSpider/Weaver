#ifndef RANDOM_GAUSSIAN_PATCH_H_
#define RANDOM_GAUSSIAN_PATCH_H_

#include <nlohmann/json.hpp>

#include "World/Map/Patches/Forms/FormPatch.h"
#include "World/Map/Points/PointContinuous.h"



class RandomGaussianPatch : public FormPatch {
protected:
    const std::string getFormType() const;

public:
    RandomGaussianPatch(const nlohmann::json &formInfo, const std::string &filename);
    virtual ~RandomGaussianPatch();
};

#endif /* RANDOM_GAUSSIAN_PATCH_H_ */