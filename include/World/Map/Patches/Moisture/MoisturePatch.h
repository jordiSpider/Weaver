#ifndef MOISTURE_PATCH_H_
#define MOISTURE_PATCH_H_

#include <nlohmann/json.hpp>
#include <memory>

#include "World/Map/Patches/Patch.h"
#include "World/Map/TerrainCells/Moisture/ExtendedMoisture.h"
#include "World/Map/TerrainCells/Moisture/Factory/MoistureFactory.h"
#include "Exceptions/LineInfoException.h"



class MoisturePatch : public Patch {
protected:
    std::unique_ptr<ExtendedMoisture> moistureInfo;

    MoisturePatch(const nlohmann::json &moistureInfo, const FormPatch* const &form);
    MoisturePatch(const unsigned int priority, const nlohmann::json &moistureInfo, const FormPatch* const &form);

    // Getter
    const std::string getContentType() const;
    const std::string getContentInfo() const;

public:
    virtual ~MoisturePatch();
    
    const std::unique_ptr<ExtendedMoisture>& getMoistureInfo() const;
    std::unique_ptr<ExtendedMoisture>& getMutableMoistureInfo();
};

#endif /* MOISTURE_PATCH_H_ */