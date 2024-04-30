#ifndef FORM_PATCH_H_
#define FORM_PATCH_H_

#include <vector>
#include <nlohmann/json.hpp>
#include <string>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/Geometry/Geometry.h"
#include "Exceptions/LineInfoException.h"



class FormPatch {
private:
    friend class boost::serialization::access;

protected:
    const Ring* const effectiveArea;
    std::string filename;
    std::string formInfo;

    FormPatch(const Ring* const effectiveArea, const std::string formInfo, const std::string &filename);

    // Getter
    const Ring* const getEffectiveArea() const;    

public:
    virtual ~FormPatch();

    virtual const double calculateCoveragePercent(const Ring &cellEffectiveArea) const;
    const std::string_view getPatchFilename() const;
    const std::string_view getFormInfo() const;

    virtual const std::string getFormType() const=0;

    /**
     * @brief Serialize the FormPatch object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* FORM_PATCH_H_ */