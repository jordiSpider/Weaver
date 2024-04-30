#include "World/Map/Patches/Forms/FormPatch.h"

using namespace std;
using json = nlohmann::json;




FormPatch::FormPatch(const Ring* const effectiveArea, const string formInfo, const string &filename)
    : effectiveArea(effectiveArea),
	  filename(filename),
	  formInfo(formInfo)
{
    
}

FormPatch::~FormPatch()
{
	delete effectiveArea;
}


const string_view FormPatch::getPatchFilename() const
{
	return filename;
}

const string_view FormPatch::getFormInfo() const
{
	return formInfo;
}


const Ring* const FormPatch::getEffectiveArea() const
{
	return effectiveArea;
}


const double FormPatch::calculateCoveragePercent(const Ring &cellEffectiveArea) const
{
	auto intersection = Geometry::calculateIntersection(*getEffectiveArea(), cellEffectiveArea);
	return Geometry::calculateCoveragePercent(intersection.get(), cellEffectiveArea);
}
