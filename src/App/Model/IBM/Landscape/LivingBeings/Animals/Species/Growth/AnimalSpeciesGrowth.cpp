
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Growth/AnimalSpeciesGrowth.h"


using namespace std;
using json = nlohmann::json;


AnimalSpeciesGrowth::AnimalSpeciesGrowth()
    : SpeciesGrowth()
{

}

AnimalSpeciesGrowth::AnimalSpeciesGrowth(const json& config, const unsigned int numberOfInstars, const vector<double>& conversionToWetMass, const Temperature& tempFromLab, const PreciseDouble& timeStepsPerDay)
    : SpeciesGrowth(numberOfInstars, conversionToWetMass)
{
    setFemaleWetMass(WetMass(config["femaleWetMass"].get<double>()));
    setEggDryMass(config["eggDryMass"]);
    setGrowthModel(config["growthModel"], tempFromLab);
    setInstarFirstReproduction(Instar(config["instarFirstReproduction"]));
    setInstarAgeVector(config["ageVector"].get<std::vector<double>>());

    setMaxPlasticityKVonBertalanffy(config["maxPlasticityKVonBertalanffy"].get<double>());
    setMinPlasticityKVonBertalanffy(config["minPlasticityKVonBertalanffy"].get<double>());

    setCoefficientForMassAforMature(config["coefficientForMassAforMature"].get<double>());

    setScaleForMassBforMature(config["scaleForMassBforMature"].get<double>());

    setCoefficientForMassA(config["coefficientForMassA"].get<double>());
	setScaleForMassB(config["scaleForMassB"].get<double>());

    setBetaScaleTank(config["betaScaleTank"].get<double>());
    setExcessInvestInSize(config["excessInvestInSize"].get<double>());

    setEggsPerBatch(config["eggsPerBatch"]);

    setCapitalBreeding(config["capitalBreeding"], timeStepsPerDay);

    setAssignedForMolt(config["assignedForMolt"].get<double>());

    setHabitatShift(config["habitatShift"], config["habitatShiftFactor"]);

    moltingAgeThreshold = config["moltingAgeThreshold"].get<double>();
}

AnimalSpeciesGrowth::~AnimalSpeciesGrowth()
{
    
}

const WetMass& AnimalSpeciesGrowth::getFemaleWetMass() const
{
    return femaleWetMass;
}

PreciseDouble AnimalSpeciesGrowth::calculateEggsPerBatch(const DryMass& individualDryMass) const
{
	if(getEggsPerBatchFromEquation())
	{
		return getInterceptForEggBatchFromEquation() + getSlopeForEggBatchFromEquation()*WetMass(individualDryMass, getConversionToWetMass(Instar(1))).getValue();
	}
	else
	{
		return getEggsPerBatch();
	}
}

const DryMass& AnimalSpeciesGrowth::getEggDryMass() const
{
    return eggDryMass;
}

std::pair<IndividualGrowthModel::Type, PreciseDouble> AnimalSpeciesGrowth::getActualGrowthModel(const Temperature& actualTemperature, const Temperature& tempFromLab) const
{
    if(growthModelTemperatureDependent)
    {
        if(actualTemperature < tempFromLab)
        {
            if(lowerGrowthModelThermalChanges.size() > 0)
            {
                auto firstPoint = lowerGrowthModelThermalChanges.cbegin();

                auto secondPoint = firstPoint;
                ++secondPoint;

                
                if(actualTemperature <= firstPoint->first)
                {
                    bool continueSearch = true;

                    while(continueSearch)
                    {
                        if(secondPoint == lowerGrowthModelThermalChanges.cend())
                        {
                            continueSearch = false;
                        }
                        else
                        {
                            if(secondPoint->first < actualTemperature)
                            {
                                continueSearch = false;
                            }
                        }

                        if(continueSearch)
                        {
                            ++firstPoint;
                            ++secondPoint;
                        }
                    }

                    return firstPoint->second;
                }
            }
        }
        else
        {
            if(upperGrowthModelThermalChanges.size() > 0)
            {
                auto firstPoint = upperGrowthModelThermalChanges.cbegin();

                auto secondPoint = firstPoint;
                ++secondPoint;

                
                if(actualTemperature >= firstPoint->first)
                {
                    bool continueSearch = true;

                    while(continueSearch)
                    {
                        if(secondPoint == upperGrowthModelThermalChanges.cend())
                        {
                            continueSearch = false;
                        }
                        else
                        {
                            if(secondPoint->first > actualTemperature)
                            {
                                continueSearch = false;
                            }
                        }

                        if(continueSearch)
                        {
                            ++firstPoint;
                            ++secondPoint;
                        }
                    }

                    return firstPoint->second;
                }
            }
        }
    }
    
    return defaultGrowthModel;
}

bool AnimalSpeciesGrowth::habitatShiftOccurs(const Instar &instar) const
{
	return habitatShiftPerInstar[instar.getValue()];
}

const PreciseDouble& AnimalSpeciesGrowth::getAssignedForMolt() const 
{ 
    return assignedForMolt; 
}

const PreciseDouble& AnimalSpeciesGrowth::getMoltingAgeThreshold() const
{
    return moltingAgeThreshold;
}

void AnimalSpeciesGrowth::setAssignedForMolt(const PreciseDouble& assignedForMoltValue)
{
	assignedForMolt = assignedForMoltValue;
}

bool AnimalSpeciesGrowth::hasIndeterminateGrowth() const 
{ 
    return indeterminateGrowth; 
}

const Instar& AnimalSpeciesGrowth::getInstarFirstReproduction() const 
{ 
    return instarFirstReproduction; 
}

const Instar& AnimalSpeciesGrowth::getInstarFirstMaturation() const
{
    if(hasIndeterminateGrowth())
    {
        return getInstarFirstReproduction();
    }
    else
    {
        return getLastInstar();
    }
}

PreciseDouble AnimalSpeciesGrowth::getMaxPlasticityKVonBertalanffy() const 
{ 
    return maxPlasticityKVonBertalanffy; 
}

void AnimalSpeciesGrowth::setMaxPlasticityKVonBertalanffy(const PreciseDouble& maxPlasticityKVonBertalanffyValue) 
{ 
    maxPlasticityKVonBertalanffy = maxPlasticityKVonBertalanffyValue; 
}

void AnimalSpeciesGrowth::setHabitatShift(const std::vector<unsigned int> &habitatShift, const json &newHabitatShiftFactor)
{
	habitatShiftPerInstar.resize(getNumberOfInstars(), false);
	habitatShiftFactor = 0.0;

	if(!habitatShift.empty())
	{
		for(const unsigned int instarValue : habitatShift)
		{
			Instar instar(instarValue);
			habitatShiftPerInstar[instar.getValue()] = true;
		}

		habitatShiftFactor = newHabitatShiftFactor.get<double>();
	}
}

PreciseDouble AnimalSpeciesGrowth::getMinPlasticityKVonBertalanffy() const 
{ 
    return minPlasticityKVonBertalanffy; 
}

void AnimalSpeciesGrowth::setMinPlasticityKVonBertalanffy(const PreciseDouble& minPlasticityKVonBertalanffyValue) 
{ 
    minPlasticityKVonBertalanffy = minPlasticityKVonBertalanffyValue; 
}

const CustomIndexedVector<Instar, Day>& AnimalSpeciesGrowth::getInstarAgeVector() const 
{ 
    return instarAgeVector; 
}

const Day& AnimalSpeciesGrowth::getInstarDevTime(const Instar& instar) const 
{ 
    return instarAgeVector[instar]; 
}

void AnimalSpeciesGrowth::setInstarAgeVector(const vector<double>& newInstarAgeVector)
{ 
    for (const double& value : newInstarAgeVector)
    {
        instarAgeVector.emplace_back(value);
    }
}

void AnimalSpeciesGrowth::setEggsPerBatch(const json& config)
{
	eggsPerBatchFromEquation = config["eggsPerBatchFromEquation"].get<bool>();

	if(eggsPerBatchFromEquation)
	{
		interceptForEggBatchFromEquation = config["equationParameters"]["interceptForEggBatchFromEquation"].get<double>();
		slopeForEggBatchFromEquation = config["equationParameters"]["slopeForEggBatchFromEquation"].get<double>();
	}
	else
	{
		eggsPerBatch = config["value"].get<double>();
	}
}

const PreciseDouble& AnimalSpeciesGrowth::getInterceptForEggBatchFromEquation() const
{
	return interceptForEggBatchFromEquation;
}

bool AnimalSpeciesGrowth::hasCapitalBreeding() const
{
	return capitalBreeding;
}

void AnimalSpeciesGrowth::setCapitalBreeding(const json &config, const PreciseDouble& timeStepsPerDay)
{
	capitalBreeding = config["enabled"].get<bool>();

	if(capitalBreeding)
	{
		timeOfReproEventDuringCapitalBreeding = TimeStep(Day(config["capitalBreedingParams"]["timeOfReproEventDuringCapitalBreeding"].get<double>()), timeStepsPerDay);
		numberOfCapitalBreeds = config["capitalBreedingParams"]["numberOfCapitalBreeds"].get<unsigned int>();
	}
    else
    {
        timeOfReproEventDuringCapitalBreeding = TimeStep(0);
        numberOfCapitalBreeds = 0;
    }
}

const TimeStep& AnimalSpeciesGrowth::getTimeOfReproEventDuringCapitalBreeding() const
{
	return timeOfReproEventDuringCapitalBreeding;
}

unsigned int AnimalSpeciesGrowth::getNumberOfCapitalBreeds() const
{
	return numberOfCapitalBreeds;
}

const PreciseDouble& AnimalSpeciesGrowth::getSlopeForEggBatchFromEquation() const
{
	return slopeForEggBatchFromEquation;
}

const PreciseDouble& AnimalSpeciesGrowth::getEggsPerBatch() const 
{ 
	return eggsPerBatch; 
}

bool AnimalSpeciesGrowth::getEggsPerBatchFromEquation() const
{
	return eggsPerBatchFromEquation;
}

const Length AnimalSpeciesGrowth::calculateDryLength(const DryMass& dryMass, const bool mature) const
{
	if(mature)
	{
		return Length(dryMass, getCoefficientForMassAforMature(), getScaleForMassBforMature());
	}
	else
	{
		return Length(dryMass, getCoefficientForMassA(), getScaleForMassB());
	}
}

PreciseDouble AnimalSpeciesGrowth::getCoefficientForMassAforMature() const 
{ 
    return coefficientForMassAforMature; 
}

void AnimalSpeciesGrowth::setCoefficientForMassAforMature(const PreciseDouble& coefficientForMassAforMatureValue) 
{ 
    coefficientForMassAforMature = coefficientForMassAforMatureValue; 
}

const PreciseDouble& AnimalSpeciesGrowth::getHabitatShiftFactor() const
{
	return habitatShiftFactor;
}

const DryMass AnimalSpeciesGrowth::calculateDryMass(const Length& length, const bool mature) const
{
	if(mature)
	{
		return DryMass(length, getCoefficientForMassAforMature(), getScaleForMassBforMature());
	}
	else
	{
		return DryMass(length, getCoefficientForMassA(), getScaleForMassB());
	}
}

pair<DryMass, DryMass> AnimalSpeciesGrowth::decomposeMassElements(const DryMass &dryMass, const PreciseDouble &energyTankTraitValue) const
{
	DryMass energyTank(energyTankTraitValue * pow(dryMass.getValue(), getBetaScaleTank()));
	DryMass bodySize = dryMass - energyTank;

	return make_pair<>(energyTank, bodySize);
}

PreciseDouble AnimalSpeciesGrowth::getExcessInvestInSize() const 
{ 
    return excessInvestInSize; 
}

void AnimalSpeciesGrowth::setExcessInvestInSize(const PreciseDouble& excessInvestInSizeValue) 
{ 
    excessInvestInSize = excessInvestInSizeValue; 
}

const PreciseDouble& AnimalSpeciesGrowth::getBetaScaleTank() const 
{ 
	return betaScaleTank; 
}

void AnimalSpeciesGrowth::setBetaScaleTank(const PreciseDouble& betaScaleTankValue) 
{ 
    betaScaleTank = betaScaleTankValue; 
}

PreciseDouble AnimalSpeciesGrowth::getScaleForMassBforMature() const 
{ 
    return scaleForMassBforMature; 
}

void AnimalSpeciesGrowth::setScaleForMassBforMature(const PreciseDouble& scaleForMassBforMatureValue) 
{ 
    scaleForMassBforMature = scaleForMassBforMatureValue; 
}

PreciseDouble AnimalSpeciesGrowth::getCoefficientForMassA() const 
{
    return coefficientForMassA;
}

void AnimalSpeciesGrowth::setCoefficientForMassA(const PreciseDouble& coefficientForMassAValue) 
{ 
    coefficientForMassA = coefficientForMassAValue; 
}

PreciseDouble AnimalSpeciesGrowth::getScaleForMassB() const 
{ 
    return scaleForMassB; 
}

void AnimalSpeciesGrowth::setScaleForMassB(const PreciseDouble& scaleForMassBValue) 
{ 
    scaleForMassB = scaleForMassBValue; 
}

void AnimalSpeciesGrowth::setFemaleWetMass(const WetMass& newFemaleWetMass)
{
    femaleWetMass = newFemaleWetMass;
}

void AnimalSpeciesGrowth::setEggDryMass(const json& config)
{
    if(config["eggDryMassFromEquation"].get<bool>())
    {
        PreciseDouble coefficient = config["equationParameters"]["coefficient"].get<double>();
        PreciseDouble scale = config["equationParameters"]["scale"].get<double>();
        
        eggDryMass = DryMass(WetMass(coefficient * pow(getFemaleWetMass().getValue(), scale)), getConversionToWetMass(Instar(1)));
    }
    else
    {
        eggDryMass = DryMass(config["value"].get<double>());
    }
}

void AnimalSpeciesGrowth::setGrowthModel(const json& growthModelInfo, const Temperature& tempFromLab)
{
    defaultGrowthModel = make_pair<>(
        EnumClass<IndividualGrowthModel::Type>::stringToEnumValue(growthModelInfo["defaultAtTempFromLab"]["model"].get<string>()),
        growthModelInfo["defaultAtTempFromLab"]["A"].get<double>()
    );

    growthModelTemperatureDependent = growthModelInfo["temperature"]["dependent"].get<bool>();

    if(growthModelTemperatureDependent)
    {
        for(const json& point : growthModelInfo["temperature"]["growthModelThermalChanges"])
        {
            Temperature pointTemperature(point["temperature"].get<double>());

            if(pointTemperature < tempFromLab)
            {
                lowerGrowthModelThermalChanges.push_back(make_pair<>(pointTemperature, make_pair<>(EnumClass<IndividualGrowthModel::Type>::stringToEnumValue(point["model"].get<string>()), point["A"].get<double>())));
            }
            else
            {
                upperGrowthModelThermalChanges.push_back(make_pair<>(pointTemperature, make_pair<>(EnumClass<IndividualGrowthModel::Type>::stringToEnumValue(point["model"].get<string>()), point["A"].get<double>())));
            }
        }

        std::sort(lowerGrowthModelThermalChanges.begin(), lowerGrowthModelThermalChanges.end(), [](pair<Temperature, pair<IndividualGrowthModel::Type, PreciseDouble>> a, pair<Temperature, pair<IndividualGrowthModel::Type, PreciseDouble>> b) {
            return a.first > b.first; 
        });

        std::sort(upperGrowthModelThermalChanges.begin(), upperGrowthModelThermalChanges.end(), [](pair<Temperature, pair<IndividualGrowthModel::Type, PreciseDouble>> a, pair<Temperature, pair<IndividualGrowthModel::Type, PreciseDouble>> b) {
            return a.first < b.first; 
        });
    }
}

void AnimalSpeciesGrowth::setInstarFirstReproduction(const Instar& newInstarFirstReproduction)
{
    instarFirstReproduction = newInstarFirstReproduction;

    indeterminateGrowth = getInstarFirstReproduction() < getLastInstar();
    

    for(Instar instar = getInstarFirstReproduction(); instar <= getLastInstar(); instar++)
	{
		matureInstarsRange.push_back(instar);
	}
}

const vector<Instar>& AnimalSpeciesGrowth::getMatureInstarsRange() const
{
    return matureInstarsRange;
}



BOOST_CLASS_EXPORT(AnimalSpeciesGrowth)

template <class Archive>
void AnimalSpeciesGrowth::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<SpeciesGrowth>(*this);
    
    ar & femaleWetMass;

    ar & eggDryMass;

    ar & defaultGrowthModel;

    ar & growthModelTemperatureDependent;
    ar & lowerGrowthModelThermalChanges;
    ar & upperGrowthModelThermalChanges;

    ar & indeterminateGrowth;
    ar & instarFirstReproduction;

    ar & matureInstarsRange;

    ar & instarAgeVector;

    ar & maxPlasticityKVonBertalanffy;
    ar & minPlasticityKVonBertalanffy;

    ar & coefficientForMassAforMature;

    ar & scaleForMassBforMature;

    ar & coefficientForMassA;
	ar & scaleForMassB;

    ar & betaScaleTank;
    ar & excessInvestInSize;

    ar & moltingAgeThreshold;

    ar & eggsPerBatchFromEquation;
    ar & eggsPerBatch;
    ar & interceptForEggBatchFromEquation;
	ar & slopeForEggBatchFromEquation;

    ar & capitalBreeding;
	ar & timeOfReproEventDuringCapitalBreeding;
	ar & numberOfCapitalBreeds;

    ar & assignedForMolt;

    ar & habitatShiftPerInstar;
	ar & habitatShiftFactor;
}

// Specialisation
template void AnimalSpeciesGrowth::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalSpeciesGrowth::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalSpeciesGrowth::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalSpeciesGrowth::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
