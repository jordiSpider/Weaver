#include "IBM/World/LivingBeings/Animals/Genetics/Traits/TemperatureSection/TempSizeRuleTraitTemperatureSection.h"

#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"


using namespace std;
using json = nlohmann::json;




TempSizeRuleTraitTemperatureSection::TempSizeRuleTraitTemperatureSection(const json& info, const Temperature& tempFromLab)
	: TraitTemperatureSection(), tempFromLab(&tempFromLab)
{
	for(const json& tempSizeRulePoint : info["tempSizeRuleVector"])
	{
		Temperature pointTemperature(tempSizeRulePoint["temperature"]);

		if(pointTemperature < getTempFromLab())
		{
			lowerTempSizeRuleVector[pointTemperature] = tempSizeRulePoint["dryMass"];
		}
		else
		{
			upperTempSizeRuleVector[pointTemperature] = tempSizeRulePoint["dryMass"];
		}
	}
}

TempSizeRuleTraitTemperatureSection::~TempSizeRuleTraitTemperatureSection() 
{

}

const Temperature& TempSizeRuleTraitTemperatureSection::getTempFromLab() const
{
	return *tempFromLab;
}

unique_ptr<tuple<map<Temperature, pair<double, double>>, double, map<Temperature, pair<double, double>>, double>> TempSizeRuleTraitTemperatureSection::calculateTempSizeRuleConstantAccumulationVector(const double& lengthAtTempFromLab, const AnimalSpecies* const animalSpecies) const
{
	auto result = make_unique<tuple<map<Temperature, pair<double, double>>, double, map<Temperature, pair<double, double>>, double>>();

	double dryMassAtTempFromLab = animalSpecies->calculateDryMass(lengthAtTempFromLab, true);


	auto lowerFirstPoint = lowerTempSizeRuleVector.crbegin();

	double degreesDifference = getTempFromLab().getTemperatureCelsius() - lowerFirstPoint->first.getTemperatureCelsius();
	double tempSizeRuleConstant = lowerFirstPoint->second / (dryMassAtTempFromLab * degreesDifference);

	get<0>(*result).at(lowerFirstPoint->first) = {
		tempSizeRuleConstant,
		degreesDifference
	};

	get<1>(*result) = tempSizeRuleConstant;

	double lastTempSizeRuleConstantAccumulation = get<0>(*result).at(lowerFirstPoint->first).first;
	double lastDegreesDifferenceAccumulation = get<0>(*result).at(lowerFirstPoint->first).second;


	auto lowerSecondPoint = lowerTempSizeRuleVector.crbegin();
	++lowerFirstPoint;

	while(lowerFirstPoint != lowerTempSizeRuleVector.crend())
	{
		degreesDifference = lowerSecondPoint->first.getTemperatureCelsius() - lowerFirstPoint->first.getTemperatureCelsius();
		tempSizeRuleConstant = lowerFirstPoint->second / (lowerSecondPoint->second * degreesDifference);

		get<0>(*result).at(lowerFirstPoint->first) = {
			lastTempSizeRuleConstantAccumulation * tempSizeRuleConstant,
			lastDegreesDifferenceAccumulation * degreesDifference
		};

		get<1>(*result) = tempSizeRuleConstant;

		lastTempSizeRuleConstantAccumulation = get<0>(*result).at(lowerFirstPoint->first).first;
		lastDegreesDifferenceAccumulation = get<0>(*result).at(lowerFirstPoint->first).second;

		++lowerFirstPoint;
		++lowerSecondPoint;
	}



	auto upperSecondPoint = upperTempSizeRuleVector.cbegin();

	degreesDifference = upperSecondPoint->first.getTemperatureCelsius()-getTempFromLab().getTemperatureCelsius();
	tempSizeRuleConstant = dryMassAtTempFromLab / (upperSecondPoint->second * degreesDifference);

	get<2>(*result).at(upperSecondPoint->first) = {
		tempSizeRuleConstant,
		degreesDifference
	};

	get<3>(*result) = tempSizeRuleConstant;

	lastTempSizeRuleConstantAccumulation = get<2>(*result).at(upperSecondPoint->first).first;
	lastDegreesDifferenceAccumulation = get<2>(*result).at(upperSecondPoint->first).second;


	auto upperFirstPoint = upperTempSizeRuleVector.cbegin();
	++upperSecondPoint;

	while(upperSecondPoint != upperTempSizeRuleVector.cend())
	{
		degreesDifference = upperSecondPoint->first.getTemperatureCelsius()-upperFirstPoint->first.getTemperatureCelsius();
		tempSizeRuleConstant = upperFirstPoint->second / (upperSecondPoint->second * degreesDifference);

		get<2>(*result).at(upperSecondPoint->first) = {
			lastTempSizeRuleConstantAccumulation * tempSizeRuleConstant,
			lastDegreesDifferenceAccumulation * degreesDifference
		};

		get<3>(*result) = tempSizeRuleConstant;

		lastTempSizeRuleConstantAccumulation = get<2>(*result).at(upperSecondPoint->first).first;
		lastDegreesDifferenceAccumulation = get<2>(*result).at(upperSecondPoint->first).second;

		++upperFirstPoint;
		++upperSecondPoint;
	}

	return result;
}

double TempSizeRuleTraitTemperatureSection::applyTemperatureDependency(const Temperature& temperature, const CustomIndexedVector<TraitDefinitionSection::Elements, double>& traitElements,
        const map<Temperature, pair<double, double>>& lowerTempSizeRuleConstantAccumulationVector, const double& lastLowerTempSizeRuleConstant, 
        const map<Temperature, pair<double, double>>& upperTempSizeRuleConstantAccumulationVector, const double& lastUpperTempSizeRuleConstant,
		const AnimalSpecies* const animalSpecies
    ) const
{
	double initialTraitValue = animalSpecies->calculateDryMass(traitElements[TraitDefinitionSection::Elements::TraitValue], true);

	double tempSizeRuleConstantAccumulation, degreesDifferenceAccumulation;
	double finalTraitValue;

	if(temperature < getTempFromLab())
	{
		auto firstPoint = lowerTempSizeRuleConstantAccumulationVector.crbegin();

		if(firstPoint->first <= temperature)
		{
			tempSizeRuleConstantAccumulation = firstPoint->second.first;
			degreesDifferenceAccumulation = getTempFromLab().getTemperatureCelsius() - temperature.getTemperatureCelsius();
		}
		else
		{
			++firstPoint;
			auto secondPoint = lowerTempSizeRuleConstantAccumulationVector.crbegin();
			while(firstPoint != lowerTempSizeRuleConstantAccumulationVector.crend() && firstPoint->first > temperature)
			{
				++firstPoint;
				++secondPoint;
			}

			if(firstPoint != lowerTempSizeRuleConstantAccumulationVector.crend())
			{
				tempSizeRuleConstantAccumulation = firstPoint->second.first;
				
			}
			else
			{
				tempSizeRuleConstantAccumulation = secondPoint->second.first * lastLowerTempSizeRuleConstant;
			}

			degreesDifferenceAccumulation = secondPoint->second.second * (secondPoint->first.getTemperatureCelsius() - temperature.getTemperatureCelsius());
		}

		finalTraitValue = initialTraitValue * tempSizeRuleConstantAccumulation * degreesDifferenceAccumulation;
	}
	else if(temperature > getTempFromLab())
	{
		auto secondPoint = upperTempSizeRuleConstantAccumulationVector.cbegin();

		if(secondPoint->first >= temperature)
		{
			tempSizeRuleConstantAccumulation = secondPoint->second.first;
			degreesDifferenceAccumulation = temperature.getTemperatureCelsius() - getTempFromLab().getTemperatureCelsius();
		}
		else
		{
			++secondPoint;
			auto firstPoint = upperTempSizeRuleConstantAccumulationVector.cbegin();
			while(secondPoint != upperTempSizeRuleConstantAccumulationVector.cend() && secondPoint->first < temperature)
			{
				++firstPoint;
				++secondPoint;
			}

			if(secondPoint != upperTempSizeRuleConstantAccumulationVector.cend())
			{
				tempSizeRuleConstantAccumulation = secondPoint->second.first;
				
			}
			else
			{
				tempSizeRuleConstantAccumulation = firstPoint->second.first * lastUpperTempSizeRuleConstant;
			}

			degreesDifferenceAccumulation = firstPoint->second.second * (temperature.getTemperatureCelsius() - firstPoint->first.getTemperatureCelsius());
		}

		finalTraitValue = initialTraitValue / (tempSizeRuleConstantAccumulation * degreesDifferenceAccumulation);
	}
	else
	{
		finalTraitValue = initialTraitValue;
	}


	return animalSpecies->calculateDryLength(finalTraitValue, true);
}
