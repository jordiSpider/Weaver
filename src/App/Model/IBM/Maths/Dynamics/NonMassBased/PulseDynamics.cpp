#include "App/Model/IBM/Maths/Dynamics/NonMassBased/PulseDynamics.h"


using namespace std;
using json = nlohmann::json;



PulseDynamics::PulseDynamics()
    : NonMassBasedDynamics()
{

}

PulseDynamics::PulseDynamics(const nlohmann::json &config, const PreciseDouble& timeStepsPerDay)
    : NonMassBasedDynamics(),
      distanceBetweenPulsesRange(
        make_pair(
            TimeStep(Day(config["daysBetweenPulsesRange"]["min"].get<double>()), timeStepsPerDay),
            TimeStep(Day(config["daysBetweenPulsesRange"]["max"].get<double>()), timeStepsPerDay)
        )
      ),
      fixedDistanceBetweenPulses(distanceBetweenPulsesRange.first == distanceBetweenPulsesRange.second),
      pulsePeakValueRange(
        make_pair(
            config["pulsePeakValueRange"]["min"].get<double>(),
            config["pulsePeakValueRange"]["max"].get<double>()
        )
      ),
      fixedPulsePeakValue(pulsePeakValueRange.first == pulsePeakValueRange.second),
      decay(Decay::createInstance(config["decay"]))
{
    nextPulseEventCountdown = TimeStep(0);
}

PulseDynamics::PulseDynamics(const nlohmann::json &config, const PreciseDouble& timeStepsPerDay, const PreciseDouble& hyperVolume)
    : NonMassBasedDynamics(),
      distanceBetweenPulsesRange(
        make_pair(
            TimeStep(Day(config["daysBetweenPulsesRange"]["min"].get<double>()), timeStepsPerDay),
            TimeStep(Day(config["daysBetweenPulsesRange"]["max"].get<double>()), timeStepsPerDay)
        )
      ),
      fixedDistanceBetweenPulses(distanceBetweenPulsesRange.first == distanceBetweenPulsesRange.second),
      pulsePeakValueRange(
        make_pair(
            config["pulsePeakValueRange"]["min"].get<double>() * hyperVolume,
            config["pulsePeakValueRange"]["max"].get<double>() * hyperVolume
        )
      ),
      fixedPulsePeakValue(pulsePeakValueRange.first == pulsePeakValueRange.second),
      decay(Decay::createInstance(config["decay"], hyperVolume))
{
    nextPulseEventCountdown = TimeStep(0);
}

PulseDynamics::~PulseDynamics()
{
    delete decay;
}

PreciseDouble PulseDynamics::getInitialValue() const
{
    return getPulsePeakValue();
}

PreciseDouble PulseDynamics::getValue(const PreciseDouble& baseValue) const
{
    if(isOnPulseEvent())
	{
        return getPulsePeakValue();
    }
    else
    {
        return decay->apply(baseValue);
    }
}

void PulseDynamics::update()
{
    if(isOnPulseEvent())
	{
        nextPulseEventCountdown = getDistanceBetweenPulses();
    }
    else
    {
        nextPulseEventCountdown = nextPulseEventCountdown - TimeStep(1);
    }
}

std::string PulseDynamics::showInfo(const std::string& indentation) const
{
    std::ostringstream info;

    info << indentation << "- Type = Pulse\n";

    info << indentation << "- Pulse peak value = " << showPulsePeakValueInfo() << "\n";

    info << indentation << "- Distance between pulses = " << showDistanceBetweenPulsesInfo() << "\n";

    info << indentation << "- Decay parameters = \n";

    info << decay->showInfo(indentation + "  ");

    return info.str();
}

const std::string PulseDynamics::showPulsePeakValueInfo() const
{
    std::ostringstream info;

    if(fixedPulsePeakValue)
    {
        info << pulsePeakValueRange.first;
    }
    else
    {
        info << "[" << pulsePeakValueRange.first << ", " << pulsePeakValueRange.second << "]";
    }

    return info.str();
}

const std::string PulseDynamics::showDistanceBetweenPulsesInfo() const
{
    std::ostringstream info;

    if(fixedDistanceBetweenPulses)
    {
        info << distanceBetweenPulsesRange.first;
    }
    else
    {
        info << "[" << distanceBetweenPulsesRange.first << ", " << distanceBetweenPulsesRange.second << "]";
    }

    return info.str();
}

bool PulseDynamics::isOnPulseEvent() const
{
    return nextPulseEventCountdown == TimeStep(0);
}

PreciseDouble PulseDynamics::getPulsePeakValue() const
{
    if(fixedPulsePeakValue)
    {
        return pulsePeakValueRange.first;
    }
    else
    {
        return Random::randomDoubleInRange(pulsePeakValueRange.first, pulsePeakValueRange.second);
    }
}

TimeStep PulseDynamics::getDistanceBetweenPulses() const
{
    if(fixedDistanceBetweenPulses)
    {
        return distanceBetweenPulsesRange.first;
    }
    else
    {
        return Random::randomTimeStepInRange(distanceBetweenPulsesRange.first, distanceBetweenPulsesRange.second);
    }
}


BOOST_CLASS_EXPORT(PulseDynamics)

template<class Archive>
void PulseDynamics::serialize(Archive & ar, const unsigned int) {
    ar & boost::serialization::base_object<NonMassBasedDynamics>(*this);

    ar & distanceBetweenPulsesRange;
    ar & fixedDistanceBetweenPulses;

    ar & nextPulseEventCountdown;

    ar & pulsePeakValueRange;
    ar & fixedPulsePeakValue;

    ar & decay;
}

// Specialisation
template void PulseDynamics::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void PulseDynamics::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void PulseDynamics::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void PulseDynamics::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
