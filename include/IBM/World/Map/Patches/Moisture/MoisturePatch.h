#ifndef MOISTURE_PATCH_H_
#define MOISTURE_PATCH_H_

#include <nlohmann/json.hpp>
#include <memory>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/Patches/Patch.h"
#include "IBM/World/Map/TerrainCells/Moisture/ExtendedMoisture.h"
#include "IBM/World/Map/TerrainCells/Moisture/Factory/MoistureFactory.h"
#include "Exceptions/LineInfoException.h"



class MoisturePatch : public Patch {
private:
    friend class boost::serialization::access;

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

    /**
     * @brief Serialize the MoisturePatch object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* MOISTURE_PATCH_H_ */