#include "App/Model/IBM/Landscape/Map/Patches/Source/MoistureSource.h"


using namespace std;
using json = nlohmann::json;


MoistureSource::MoistureSource()
    : PathSource()
{

}

MoistureSource::MoistureSource(const json &config, const PreciseDouble& timeStepsPerDay)
    : PathSource(),
      inEnemyFreeSpace(config["inEnemyFreeSpace"].get<bool>()), 
      inCompetitorFreeSpace(config["inCompetitorFreeSpace"].get<bool>()),
      maximumResourceCapacityDensity(config["maximumResourceCapacityDensity"].get<double>()),
      temperatureDynamics(config["temperatureDynamics"]),
      currentTemperature(temperatureDynamics.getInitialValue()),
      relativeHumidityDynamics(NonMassBasedDynamics::createInstance(config["relativeHumidityDynamics"], timeStepsPerDay)),
      currentRelativeHumidity(relativeHumidityDynamics->getInitialValue())
{

}

MoistureSource::~MoistureSource()
{
    delete relativeHumidityDynamics;
}


string MoistureSource::showInfo() const
{
    ostringstream info;

    info << " - Moisture parameters:\n";

    info << "   - Maximum resource capacity density = " << maximumResourceCapacityDensity << "\n";

    info << "   - Temperature dynamics:\n";

    info << temperatureDynamics.showInfo("     ") << "\n";

    info << "   - Relative humidity dynamics:\n";

    info << relativeHumidityDynamics->showInfo("     ") << "\n";

    return info.str();
}

PathSource::Type MoistureSource::getType() const
{
	return Type::Moisture;
}

const WetMass& MoistureSource::getMaximumResourceCapacityDensity() const
{
    return maximumResourceCapacityDensity;
}

const Temperature& MoistureSource::getTemperature() const
{
    return currentTemperature;
}

const PreciseDouble& MoistureSource::getMoisture() const
{
    return currentRelativeHumidity;
}

bool MoistureSource::isInEnemyFreeSpace() const
{
    return inEnemyFreeSpace;
}

bool MoistureSource::isInCompetitorFreeSpace() const
{
    return inCompetitorFreeSpace;
}

void MoistureSource::update()
{
    temperatureDynamics.update();
    currentTemperature = Temperature(temperatureDynamics.getValue(getTemperature().getTemperatureCelsius()));

    relativeHumidityDynamics->update();
    currentRelativeHumidity = relativeHumidityDynamics->getValue(getMoisture());
}

const CycleDynamics& MoistureSource::getTemperatureDynamics() const
{
    return temperatureDynamics;
}


BOOST_CLASS_EXPORT(MoistureSource)

template<class Archive>
void MoistureSource::serialize(Archive & ar, const unsigned int) {
    ar & boost::serialization::base_object<PathSource>(*this);

    ar & inEnemyFreeSpace;
    ar & inCompetitorFreeSpace;
    
    ar & maximumResourceCapacityDensity;

    ar & temperatureDynamics;
    ar & currentTemperature;

    ar & relativeHumidityDynamics;
    ar & currentRelativeHumidity;
}

// Specialisation
template void MoistureSource::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void MoistureSource::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void MoistureSource::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void MoistureSource::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
