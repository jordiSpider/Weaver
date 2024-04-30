#ifndef SPHERICAL_PATCH_H_
#define SPHERICAL_PATCH_H_

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/Patches/Forms/FormPatch.h"
#include "IBM/World/Map/Points/PointContinuous.h"
#include "IBM/World/Map/Points/Axis.h"
#include "IBM/World/Map/Geometry/Geometry.h"
#include "Misc/Macros.h"



class SphericalPatch : public FormPatch {
private:
    friend class boost::serialization::access;

protected:
    const Ring* const makeSphere(const std::vector<double> &center, const double &radius);
    const std::string makeFormInfo(const std::vector<double> &center, const double &radius) const;
    const std::string getFormType() const;

public:
    SphericalPatch(const nlohmann::json &formInfo, const std::string &filename);
    virtual ~SphericalPatch();

    /**
     * @brief Serialize the SphericalPatch object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* SPHERICAL_PATCH_H_ */