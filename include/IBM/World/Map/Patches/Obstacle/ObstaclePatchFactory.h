#ifndef OBSTACLE_PATCH_FACTORY_H_
#define OBSTACLE_PATCH_FACTORY_H_


#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Exceptions/LineInfoException.h"
#include "IBM/World/Map/Patches/PatchFactory.h"
#include "IBM/World/Map/Patches/Obstacle/ObstaclePatch.h"
#include "IBM/World/Map/Patches/Obstacle/CubicObstaclePatch.h"



class ObstaclePatchFactory : public PatchFactory
{
private:
	friend class boost::serialization::access;

public:
	static std::unique_ptr<const ObstaclePatch> createInstance(const nlohmann::json &obstaclePatchInfo, const std::string &filename);

	/**
     * @brief Serialize the ObstaclePatchFactory object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif // OBSTACLE_PATCH_FACTORY_H_
