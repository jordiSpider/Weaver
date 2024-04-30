#ifndef RANDOM_GAUSSIAN_MOISTURE_PATCH_H_
#define RANDOM_GAUSSIAN_MOISTURE_PATCH_H_

#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/Patches/Forms/RandomGaussianPatch.h"
#include "IBM/World/Map/Patches/Moisture/MoisturePatch.h"



class RandomGaussianMoisturePatch : public MoisturePatch
{
private:
    friend class boost::serialization::access;

public:
    RandomGaussianMoisturePatch(const nlohmann::json &moisturePatchInfo, const std::string &filename);
    virtual ~RandomGaussianMoisturePatch();

    /**
     * @brief Serialize the RansomGaussianMoisturePatch object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* RANDOM_GAUSSIAN_MOISTURE_PATCH_H_ */