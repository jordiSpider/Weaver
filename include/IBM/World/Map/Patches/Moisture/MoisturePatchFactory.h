#ifndef MOISTURE_PATCH_FACTORY_H_
#define MOISTURE_PATCH_FACTORY_H_


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
#include "IBM/World/Map/Patches/Moisture/MoisturePatch.h"
#include "IBM/World/Map/Patches/Moisture/CubicMoisturePatch.h"
#include "IBM/World/Map/Patches/Moisture/SphericalMoisturePatch.h"
#include "IBM/World/Map/Patches/Moisture/GaussianMoisturePatch.h"
#include "IBM/World/Map/Patches/Moisture/HomogeneousMoisturePatch.h"
#include "IBM/World/Map/Patches/Moisture/RandomGaussianMoisturePatch.h"
#include "IBM/World/Map/Patches/Moisture/MoistureBasePatch.h"



class MoisturePatchFactory : public PatchFactory
{
private:
	friend class boost::serialization::access;

public:
	static std::unique_ptr<MoisturePatch> createInstance(const nlohmann::json &moisturePatchInfo, const std::string &filename);
	static std::unique_ptr<MoisturePatch> createInstance(const nlohmann::json &moisturePatchInfo);

	/**
     * @brief Serialize the MoisturePatchFactroy object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif // MOISTURE_PATCH_FACTORY_H_
