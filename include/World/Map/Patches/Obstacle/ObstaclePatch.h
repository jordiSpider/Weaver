#ifndef OBSTACLE_PATCH_H_
#define OBSTACLE_PATCH_H_

#include <nlohmann/json.hpp>
#include <memory>

#include "World/Map/Patches/Patch.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"



class ObstaclePatch : public Patch {
protected:
    ObstaclePatch(const nlohmann::json &obstacleInfo, const FormPatch* const &form);

    // Getter
    const std::string getContentType() const;
    const std::string getContentInfo() const;

public:
    virtual ~ObstaclePatch();
};

#endif /* OBSTACLE_PATCH_H_ */