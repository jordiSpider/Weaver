#ifndef CUBIC_PATCH_H_
#define CUBIC_PATCH_H_

#include <nlohmann/json.hpp>
#include <boost/geometry/algorithms/convert.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/Patches/Forms/FormPatch.h"
#include "IBM/World/Map/Points/PointContinuous.h"
#include "IBM/World/Map/Geometry/Geometry.h"
#include "Misc/Macros.h"



class CubicPatch : public FormPatch {
private:
    friend class boost::serialization::access;

protected:
    const std::string makeFormInfo(const std::vector<double> &bottomLeftCorner, const std::vector<double> &topRightCorner) const;
    const std::string getFormType() const;
    const Ring* const makeCube(const std::vector<double> &bottomLeftCorner, const std::vector<double> &topRightCorner);

public:
    CubicPatch(const nlohmann::json &formInfo, const std::string &filename);
    virtual ~CubicPatch();

    /**
     * @brief Serialize the CubicPatch object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* CUBIC_PATCH_H_ */