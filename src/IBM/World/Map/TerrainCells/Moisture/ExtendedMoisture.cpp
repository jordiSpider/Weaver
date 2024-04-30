
#include "IBM/World/Map/TerrainCells/Moisture/ExtendedMoisture.h"

using namespace std;
using json = nlohmann::json;


const ExtendedMoisture::Type::TypeValue ExtendedMoisture::Type::stringToEnumValue(const string &str) { 
	try
	{
		return stringToEnum.at(str);
	}
	catch(const out_of_range& e) 
	{
		throwLineInfoException(fmt::format("Unknown moisture type '{}'. Valid values are {}", str, printAvailableValues()));
	}
}

constexpr size_t ExtendedMoisture::Type::size() 
{ 
	return magic_enum::enum_count<TypeValue>(); 
}

std::string_view ExtendedMoisture::Type::to_string(const TypeValue& type) 
{ 
	return magic_enum::enum_name(type); 
}

const unordered_map<string_view, const ExtendedMoisture::Type::TypeValue> ExtendedMoisture::Type::generateMap() 
{
	unordered_map<string_view, const TypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const TypeValue type = static_cast<const TypeValue>(i);
		enumMap.insert({to_string(type), type});
	}

	return enumMap;
}

const unordered_map<string_view, const ExtendedMoisture::Type::TypeValue> ExtendedMoisture::Type::stringToEnum = ExtendedMoisture::Type::generateMap();

const string ExtendedMoisture::Type::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<TypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string ExtendedMoisture::Type::enumValues = ExtendedMoisture::Type::generateAvailableValues();

std::string_view ExtendedMoisture::Type::printAvailableValues() 
{ 
	return enumValues; 
}

template <class Archive>
void ExtendedMoisture::Type::serialize(Archive &ar, const unsigned int version) {
    ar & stringToEnum;
    ar & enumValues;
}


ExtendedMoisture::ExtendedMoisture(const json &moistureInfo)
    : MoistureInterface(),
	  inEnemyFreeSpace(moistureInfo["inEnemyFreeSpace"]), 
      inCompetitorFreeSpace(moistureInfo["inCompetitorFreeSpace"]),
      temperature(0.0), 
      moisture(0.0),
      maximumResourceCapacityDensity(moistureInfo["maximumResourceCapacityDensity"]),
      temperatureCycle(obtainTemperatureCycle(moistureInfo["temperatureCycle"]))
{

}

ExtendedMoisture::ExtendedMoisture(const ExtendedMoisture& other)
    : MoistureInterface(),
	  inEnemyFreeSpace(other.isInEnemyFreeSpace()), 
      inCompetitorFreeSpace(other.isInCompetitorFreeSpace()),
      temperature(other.getTemperature()), 
      moisture(other.getMoisture()),
      maximumResourceCapacityDensity(other.getMaximumResourceCapacityDensity()),
      temperatureCycle(other.getTemperatureCycle())
{

}

ExtendedMoisture::~ExtendedMoisture()
{

}

vector<Temperature> ExtendedMoisture::obtainTemperatureCycle(const json &temperatureCycleValues)
{
    vector<Temperature> temperatureCycle;
    transform(temperatureCycleValues.begin(), temperatureCycleValues.end(), back_inserter(temperatureCycle), [](double temperatureValue) {
        return Temperature(temperatureValue);
    });

    return temperatureCycle;
}

void ExtendedMoisture::setTemperature(const Temperature &newTemperature)
{
    temperature = newTemperature;
}

void ExtendedMoisture::setMoisture(const double &newMoisture)
{
    moisture = newMoisture;
}

void ExtendedMoisture::setInEnemyFreeSpace(const bool newInEnemyFreeSpace)
{
    inEnemyFreeSpace = newInEnemyFreeSpace;
}

void ExtendedMoisture::setInCompetitorFreeSpace(const bool newInCompetitorFreeSpace)
{
    inCompetitorFreeSpace = newInCompetitorFreeSpace;
}

const vector<Temperature>& ExtendedMoisture::getTemperatureCycle() const
{
    return temperatureCycle;
}

const double& ExtendedMoisture::getMaximumResourceCapacityDensity() const
{
    return maximumResourceCapacityDensity;
}

const Temperature& ExtendedMoisture::getTemperature() const
{
    return temperature;
}

const double& ExtendedMoisture::getMoisture() const
{
    return moisture;
}

const bool ExtendedMoisture::isInEnemyFreeSpace() const
{
    return inEnemyFreeSpace;
}

const bool ExtendedMoisture::isInCompetitorFreeSpace() const
{
    return inCompetitorFreeSpace;
}

void ExtendedMoisture::updateTemperature()
{
    // Not to implement
}

void ExtendedMoisture::updateRelativeHumidity()
{
    // Not to implement
}

void ExtendedMoisture::updateInfo()
{
    throwLineInfoException("Extended moisture is not updated");
}

const Temperature& ExtendedMoisture::getTemperatureOnTimeStep(const unsigned int numberOfTimeSteps) const
{
    return getTemperatureCycle().at(numberOfTimeSteps%getTemperatureCycle().size());
}

const string ExtendedMoisture::showMoistureInfo() const
{
	string temperatureCycleString = fmt::format(
        "[{}", getTemperatureCycle()[0].getTemperatureCelsius()
    );

    for(unsigned int i = 1; i < getTemperatureCycle().size(); i++)
    {
        temperatureCycleString += fmt::format(", {}", getTemperatureCycle()[i].getTemperatureCelsius());
    }

    temperatureCycleString += "]";

	return fmt::format(
		" - Moisture parameters:\n   - Temperature cycle = {}\n   - Maximum resource capacity density = {}", 
		temperatureCycleString, getMaximumResourceCapacityDensity()
	);
}

void ExtendedMoisture::refreshValue(const unsigned int numberOfTimeSteps)
{
    refreshTemperature(numberOfTimeSteps);
    refreshRelativeHumidity(numberOfTimeSteps);
}

void ExtendedMoisture::refreshTemperature(const unsigned int numberOfTimeSteps)
{
    setTemperature(getTemperatureOnTimeStep(numberOfTimeSteps));
}

const Temperature& ExtendedMoisture::getRandomTemperature() const
{
    return temperatureCycle.at(Random::randomIndex(getTemperatureCycle().size()));
}

template <class Archive>
void ExtendedMoisture::serialize(Archive &ar, const unsigned int version) {
    ar & inEnemyFreeSpace;
    ar & inCompetitorFreeSpace;
    ar & temperature;
    ar & moisture;
    ar & maximumResourceCapacityDensity;
    ar & temperatureCycle;
}
