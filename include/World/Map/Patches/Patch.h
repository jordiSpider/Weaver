#ifndef PATCH_H_
#define PATCH_H_

#include <utility>
#include <string>
#include <memory>
#include <boost/geometry/algorithms/area.hpp>

#include "World/Map/Geometry/Geometry.h"
#include "World/Map/Geometry/Coverage.h"
#include "World/Map/Patches/Forms/FormPatch.h"
#include "Misc/Macros.h"



class Patch {
protected:
    const int priority;
    const FormPatch* const form;


    Patch(const int priority, const FormPatch* const &form);

    // Getter
    virtual const std::string getContentType() const=0;
    virtual const std::string getContentInfo() const=0;

public:
    virtual ~Patch();

    virtual const Coverage checkCoverage(const Ring &cellEffectiveArea) const;

    const std::string showPatchInfo() const;

    const int getPriority() const;

    virtual bool operator<(const Patch& other) const;
};

#endif /* PATCH_H_ */