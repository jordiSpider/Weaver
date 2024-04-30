#ifndef FORM_PATCH_H_
#define FORM_PATCH_H_

#include <vector>
#include <nlohmann/json.hpp>
#include <string>
#include <magic_enum.hpp>

#include "World/Map/Geometry/Geometry.h"
#include "Exceptions/LineInfoException.h"



class FormPatch {
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
};

#endif /* FORM_PATCH_H_ */