#include "IBM/World/Map/Patches/Moisture/MoisturePatch.h"

using namespace std;
using json = nlohmann::json;



MoisturePatch::MoisturePatch(const json &moistureInfo, const FormPatch* const &form)
    : Patch(moistureInfo["priority"], form),
	  moistureInfo(ExtendedMoisture::createInstance(moistureInfo).release())
{
    
}

MoisturePatch::MoisturePatch(const unsigned int priority, const json &moistureInfo, const FormPatch* const &form)
    : Patch(priority, form),
	  moistureInfo(ExtendedMoisture::createInstance(moistureInfo).release())
{
    
}

MoisturePatch::~MoisturePatch()
{
	if(moistureInfo != nullptr)
	{
		delete moistureInfo;
	}
}


const ExtendedMoisture* const& MoisturePatch::getMoistureInfo() const
{
	return moistureInfo;
}

ExtendedMoisture* const& MoisturePatch::getMutableMoistureInfo()
{
	return moistureInfo;
}

void MoisturePatch::saveMoistureInfo(std::vector<ExtendedMoisture*>& appliedMoisture)
{
	appliedMoisture.push_back(moistureInfo);
	moistureInfo = nullptr;
}

const string MoisturePatch::getContentType() const
{
	return string("moisture");
}

const string MoisturePatch::getContentInfo() const
{
	return getMoistureInfo()->showMoistureInfo();
}

template <class Archive>
void MoisturePatch::serialize(Archive &ar, const unsigned int version) {
	ar & moistureInfo;
}
