#ifndef OBSTACLE_PATCH_H_
#define OBSTACLE_PATCH_H_

#include <nlohmann/json.hpp>
#include <memory>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/Patches/Patch.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"



class ObstaclePatch : public Patch {
private:
    friend class boost::serialization::access;

protected:
    ObstaclePatch(const nlohmann::json &obstacleInfo, const FormPatch* const &form);

    // Getter
    const std::string getContentType() const;
    const std::string getContentInfo() const;

public:
    virtual ~ObstaclePatch();

    /**
     * @brief Serialize the ObsstaclePatch object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* OBSTACLE_PATCH_H_ */