#ifndef MOISTURE_BASE_PATCH_H_
#define MOISTURE_BASE_PATCH_H_

#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/Patches/Moisture/HomogeneousMoisturePatch.h"



class MoistureBasePatch : public HomogeneousMoisturePatch
{
private:
    friend class boost::serialization::access;

public:
    MoistureBasePatch(const nlohmann::json &moisturePatchInfo);
    virtual ~MoistureBasePatch();

    /**
     * @brief Serialize the MoistureBasePatch object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* MOISTURE_BASE_PATCH_H_ */