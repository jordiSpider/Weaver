#include "Curve.h"
#include <cmath>


using namespace std;
using json = nlohmann::json;


const unordered_map<string_view, const CurveType::CurveTypeValue> CurveType::generateMap() 
{
	unordered_map<string_view, const CurveTypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const CurveTypeValue curveType = static_cast<const CurveTypeValue>(i);
		enumMap.insert({to_string(curveType), curveType});
	}

	return enumMap;
}

const unordered_map<string_view, const CurveType::CurveTypeValue> CurveType::stringToEnum = CurveType::generateMap();

const string CurveType::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<CurveTypeValue>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string CurveType::enumValues = CurveType::generateAvailableValues();


const double& CurveParams::getGrowthCoefficient() const
{
	return growthCoefficient;
}

const double& CurveParams::getTime() const
{
	return time;
}

void CurveParams::setGrowthCoefficient(const double &growthCoefficient)
{
	this->growthCoefficient = growthCoefficient;
}

void CurveParams::setTime(const double &time)
{
	this->time = time;
}


const double& VonBertalanffyCurveParams::getAsymptoticSize() const
{
	return vonBertAsymptoticSize;
}

void VonBertalanffyCurveParams::setAsymptoticSize(const double &vonBertAsymptoticSize)
{
	this->vonBertAsymptoticSize = vonBertAsymptoticSize;
}

VonBertalanffyCurve::VonBertalanffyCurve(const unordered_map<string,json>& growthCurve) 
	: Curve(), vonBertTime0(growthCurve.at("vonBertTime0")), vonBertLdistanceMin(growthCurve.at("vonBertLdistanceMin")), vonBertLdistanceMax(growthCurve.at("vonBertLdistanceMax"))
{
	
}

const double& VonBertalanffyCurve::getVonBertTime0() const
{
	return vonBertTime0;
}

const double& VonBertalanffyCurve::getLdistanceMin() const 
{ 
	return vonBertLdistanceMin; 
}

const double& VonBertalanffyCurve::getLdistanceMax() const 
{ 
	return vonBertLdistanceMax; 
}

CurveType::CurveTypeValue VonBertalanffyCurve::getType() const
{
	return CurveType::VonBertalanffy;
}

double VonBertalanffyCurve::getValue(const CurveParams &params) const
{
	const VonBertalanffyCurveParams& castParams = static_cast<const VonBertalanffyCurveParams&>(params);
	return castParams.getAsymptoticSize()*(1-exp(-castParams.getGrowthCoefficient()*(castParams.getTime()-vonBertTime0)));
}


const double& LogisticCurveParams::getAsymptoticSize() const
{
	return logisticAsymptoticSize;
}

const double& LogisticCurveParams::getMidpointValue() const
{
	return logisticMidpointValue;
}

void LogisticCurveParams::setAsymptoticSize(const double &logisticAsymptoticSize)
{
	this->logisticAsymptoticSize = logisticAsymptoticSize;
}

void LogisticCurveParams::setMidpointValue(const double &logisticMidpointValue)
{
	this->logisticMidpointValue = logisticMidpointValue;
}

LogisticCurve::LogisticCurve(const unordered_map<string,json>& growthCurve) 
	: Curve(), logisticLdistanceMin(growthCurve.at("logisticLdistanceMin")), logisticLdistanceMax(growthCurve.at("logisticLdistanceMax"))
{
	
}

const double& LogisticCurve::getLdistanceMin() const 
{ 
	return logisticLdistanceMin; 
}

const double& LogisticCurve::getLdistanceMax() const 
{ 
	return logisticLdistanceMax; 
}

CurveType::CurveTypeValue LogisticCurve::getType() const
{
	return CurveType::Logistic;
}

double LogisticCurve::getValue(const CurveParams &params) const
{
	const LogisticCurveParams& castParams = static_cast<const LogisticCurveParams&>(params);
	return castParams.getAsymptoticSize()/(1+exp((castParams.getMidpointValue()-castParams.getTime())*castParams.getGrowthCoefficient()));
}


Logistic3PCurve::Logistic3PCurve(const unordered_map<string,json>& growthCurve) 
	: logistic3pAsymptoticSize(growthCurve.at("logistic3pAsymptoticSize")), logistic3pMidpointValue(logistic3pAsymptoticSize/2)
{
	
}

const double& Logistic3PCurve::getAsymptoticSize() const
{
	return logistic3pAsymptoticSize;
}

const double& Logistic3PCurve::getMidpointValue() const
{
	return logistic3pMidpointValue;
}

CurveType::CurveTypeValue Logistic3PCurve::getType() const
{
	return CurveType::Logistic3P;
}

double Logistic3PCurve::getValue(const CurveParams &params) const
{
	const Logistic3PCurveParams& castParams = static_cast<const Logistic3PCurveParams&>(params);
	return getAsymptoticSize()/(1+exp((getMidpointValue()-castParams.getTime())/castParams.getGrowthCoefficient()));
}


Logistic4PCurve::Logistic4PCurve(const unordered_map<string,json>& growthCurve) 
	: logistic4pLeftAsymptote(growthCurve.at("logistic4pLeftAsymptote")), logistic4pRightAsymptote(growthCurve.at("logistic4pRightAsymptote")),
	  logistic4pAsymptoticSize(logistic4pLeftAsymptote+(logistic4pRightAsymptote-logistic4pLeftAsymptote)), 
	  logistic4pMidpointValue(logistic4pLeftAsymptote+(logistic4pRightAsymptote-logistic4pLeftAsymptote)/2)
{
	
}

const double& Logistic4PCurve::getLeftAsymptote() const
{
	return logistic4pLeftAsymptote;
}

const double& Logistic4PCurve::getRightAsymptote() const
{
	return logistic4pRightAsymptote;
}

const double& Logistic4PCurve::getAsymptoticSize() const
{
	return logistic4pAsymptoticSize;
}

const double& Logistic4PCurve::getMidpointValue() const
{
	return logistic4pMidpointValue;
}

CurveType::CurveTypeValue Logistic4PCurve::getType() const
{
	return CurveType::Logistic4P;
}

double Logistic4PCurve::getValue(const CurveParams &params) const
{
	const Logistic4PCurveParams& castParams = static_cast<const Logistic4PCurveParams&>(params);
	return getAsymptoticSize()/(1+exp((getMidpointValue()-castParams.getTime())/castParams.getGrowthCoefficient()));
}


GompertzCurve::GompertzCurve(const unordered_map<string,json>& growthCurve) 
	: gompertzAsymptoticSize(growthCurve.at("gompertzAsymptoticSize")), gompertzValueTime0(growthCurve.at("gompertzValueTime0"))
{
	
}

const double& GompertzCurve::getAsymptoticSize() const
{
	return gompertzAsymptoticSize;
}

const double& GompertzCurve::getValueTime0() const
{
	return gompertzValueTime0;
}

CurveType::CurveTypeValue GompertzCurve::getType() const
{
	return CurveType::Gompertz;
}

double GompertzCurve::getValue(const CurveParams &params) const
{
	const GompertzCurveParams& castParams = static_cast<const GompertzCurveParams&>(params);
	return getAsymptoticSize()*exp(-getValueTime0()*pow(castParams.getGrowthCoefficient(),castParams.getTime()));
}


const double& ExponentialCurveParams::getValueTime0() const
{
	return exponentialValueTime0;
}

void ExponentialCurveParams::setValueTime0(const double &exponentialValueTime0)
{
	this->exponentialValueTime0 = exponentialValueTime0;
}

ExponentialCurve::ExponentialCurve(const unordered_map<string,json>& growthCurve) 
	: Curve()
{
	
}

CurveType::CurveTypeValue ExponentialCurve::getType() const
{
	return CurveType::Exponential;
}

double ExponentialCurve::getValue(const CurveParams &params) const
{
	const ExponentialCurveParams& castParams = static_cast<const ExponentialCurveParams&>(params);
	return castParams.getValueTime0() * exp(castParams.getGrowthCoefficient() * castParams.getTime());
}
