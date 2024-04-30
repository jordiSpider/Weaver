#ifndef CUBIC_OBSTACLE_PATCH_H_
#define CUBIC_OBSTACLE_PATCH_H_

#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/Patches/Forms/CubicPatch.h"
#include "IBM/World/Map/Patches/Obstacle/ObstaclePatch.h"



class CubicObstaclePatch : public ObstaclePatch
{
private:
    friend class boost::serialization::access;

public:
    CubicObstaclePatch(const nlohmann::json &obstaclePatchInfo, const std::string &filename);
    virtual ~CubicObstaclePatch();

    /**
     * @brief Serialize the CubicObstaclePatch object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* CUBIC_OBSTACLE_PATCH_H_ */