#ifndef MOISTURE_FACTORY_H_
#define MOISTURE_FACTORY_H_


#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Exceptions/LineInfoException.h"
#include "IBM/World/Map/TerrainCells/Moisture/ExtendedMoisture.h"
#include "IBM/World/Map/TerrainCells/Moisture/UpdateMethodType/CycleMoisture.h"
#include "IBM/World/Map/TerrainCells/Moisture/UpdateMethodType/RainEventAndDecayOverTimeMoisture.h"



class MoistureFactory
{
private:
	friend class boost::serialization::access;

public:
	static std::unique_ptr<ExtendedMoisture> createInstance(const nlohmann::json &moistureInfo);

	/**
	 * @brief Serialize the MoistureFactory object.
	 * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
	 * @param ar The archive to use.
	 * @param version The version of the serialization format.
	 */
	template <class Archive>
	void serialize(Archive &ar, const unsigned int version);
};

#endif // MOISTURE_FACTORY_H_
