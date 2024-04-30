#ifndef GAUSSIAN_PATCH_H_
#define GAUSSIAN_PATCH_H_

#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/Patches/Forms/FormPatch.h"
#include "IBM/World/Map/Points/PointContinuous.h"



class GaussianPatch : public FormPatch {
private:
    friend class boost::serialization::access;

protected:
    const std::string getFormType() const;

public:
    GaussianPatch(const nlohmann::json &formInfo, const std::string &filename);
    virtual ~GaussianPatch();

    /**
     * @brief Serialize the GaussianPatch object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* GAUSSIAN_PATCH_H_ */