#include "App/Model/IBM/Maths/Dynamics/MassBased/GrowthRateDynamics.h"


using namespace std;
using json = nlohmann::json;



GrowthRateDynamics::GrowthRateDynamics()
    : MassBasedDynamics()
{

}

GrowthRateDynamics::GrowthRateDynamics(const nlohmann::json &config, const PreciseDouble& scaleMass, const WetMass& cellMass, const PreciseDouble& hyperVolume, const WetMass& resourceMaximumCapacity)
    : MassBasedDynamics(), initialValue(config["initialValue"].get<double>() * hyperVolume), 
	  resourceMaximumCapacity(resourceMaximumCapacity), M(cellMass.getValue() * scaleMass), 
      variableIntrinsicRateOfIncrease(config["variableIntrinsicRateOfIncrease"].get<bool>()),
      rMaxScale(config["rMaxScale"].get<double>())
{
    if(variableIntrinsicRateOfIncrease)
	{
		temperatureOptimal = Temperature(config["temperature"]["temperatureOptimal"].get<double>());
		energyDecay = config["temperature"]["energyDecay"].get<double>();
		activationEnergy = config["temperature"]["activationEnergy"].get<double>();
		temperatureRef = Temperature(config["temperature"]["temperatureRef"].get<double>());
	}
}

GrowthRateDynamics::~GrowthRateDynamics()
{

}


PreciseDouble GrowthRateDynamics::getInitialValue() const
{
    return initialValue;
}

PreciseDouble GrowthRateDynamics::getValue(const PreciseDouble& baseValue, bool competitionAmongResourceSpecies, const WetMass& totalMaximumResourceCapacity, const PreciseDouble& timeStepsPerDay, const Temperature& temperature, const PreciseDouble& moisture, const PreciseDouble& minRH, const PreciseDouble& maxRH) const
{
	const PreciseDouble rateOfIncrease = getRateOfIncrease(temperature, moisture, minRH, maxRH);

	PreciseDouble rateOfIncreasePerTimeStep = rateOfIncrease * timeStepsPerDay;

	if(rateOfIncrease > 1.7)
	{
		return (1+rateOfIncreasePerTimeStep) * baseValue;
	}
	else
	{
		if(competitionAmongResourceSpecies) {
			return baseValue + rateOfIncreasePerTimeStep * baseValue * (1-(baseValue/totalMaximumResourceCapacity.getValue()));
		}
		else {
			return baseValue + rateOfIncreasePerTimeStep * baseValue * (1-(baseValue/resourceMaximumCapacity.getValue()));
		}
	}
}

void GrowthRateDynamics::update()
{
	
}

std::string GrowthRateDynamics::showInfo(const std::string& indentation) const
{
    std::ostringstream info;

    info << indentation << "- Type = Growth rate\n";

    info << indentation << "- Initial value = " << initialValue << "\n";

	info << indentation << "- R max scale = " << rMaxScale << "\n";
	
    info << indentation << "- Variable intrinsic rate of increase = " << (variableIntrinsicRateOfIncrease) ? "TRUE\n" : "FALSE";

	if(variableIntrinsicRateOfIncrease)
	{
		info << indentation << "- Temperature parameters = \n";

		info << indentation << "  - Temperature optimal = " << temperatureOptimal << "\n";

		info << indentation << "  - Energy decay = " << energyDecay << "\n";

		info << indentation << "  - Activation energy = " << activationEnergy << "\n";

		info << indentation << "  - Reference temperature = " << temperatureRef;
	}

    return info.str();
}

PreciseDouble GrowthRateDynamics::getRateOfIncrease(const Temperature& temperature, const PreciseDouble& moisture, const PreciseDouble& minRH, const PreciseDouble& maxRH) const
{
	if(variableIntrinsicRateOfIncrease)
	{
		const PreciseDouble rMaxAtTopt = rMaxScale * exp(-0.23*(log(M))+23.82)/exp(E/(BOLTZMANN*temperature.getTemperatureKelvin()));

		const PreciseDouble rMax = MathFunctions::use_Pawar_2018(temperature, rMaxAtTopt, temperatureOptimal, energyDecay, activationEnergy, temperatureRef, false, true);

		const Parabola growingParabola(minRH, maxRH, rMax);

		PreciseDouble newR = growingParabola.getValue(moisture);

		// Prevent unrealistic negative numbers when outside RH ranges
		if(newR < 0)
		{
		  	newR = 0.0;
		}

		return newR;
	}
	else
	{
		return rMaxScale;
	}
}



BOOST_CLASS_EXPORT(GrowthRateDynamics)

template<class Archive>
void GrowthRateDynamics::serialize(Archive & ar, const unsigned int) {
    ar & boost::serialization::base_object<MassBasedDynamics>(*this);

    ar & initialValue;

    ar & M;

    ar & variableIntrinsicRateOfIncrease;

    ar & temperatureOptimal;
	ar & energyDecay;
	ar & activationEnergy;
	ar & temperatureRef;

    ar & rMaxScale;
}

// Specialisation
template void GrowthRateDynamics::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void GrowthRateDynamics::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void GrowthRateDynamics::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void GrowthRateDynamics::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
