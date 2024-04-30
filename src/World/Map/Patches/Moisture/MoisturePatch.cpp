#include "World/Map/Patches/Moisture/MoisturePatch.h"

using namespace std;
using json = nlohmann::json;



MoisturePatch::MoisturePatch(const json &moistureInfo, const FormPatch* const &form)
    : Patch(moistureInfo["priority"], form),
	  moistureInfo(MoistureFactory::createInstance(moistureInfo))
{
    
}

MoisturePatch::MoisturePatch(const unsigned int priority, const json &moistureInfo, const FormPatch* const &form)
    : Patch(priority, form),
	  moistureInfo(MoistureFactory::createInstance(moistureInfo))
{
    
}

MoisturePatch::~MoisturePatch()
{
	
}


const unique_ptr<ExtendedMoisture>& MoisturePatch::getMoistureInfo() const
{
	return moistureInfo;
}

unique_ptr<ExtendedMoisture>& MoisturePatch::getMutableMoistureInfo()
{
	return moistureInfo;
}

const string MoisturePatch::getContentType() const
{
	return string("moisture");
}

const string MoisturePatch::getContentInfo() const
{
	return getMoistureInfo()->showMoistureInfo();
}
