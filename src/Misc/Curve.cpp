#include "Misc/Curve.h"


using namespace std;
using json = nlohmann::json;



unique_ptr<Curve> Curve::createInstance(const nlohmann::json &info)
{
	switch(EnumClass<Type>::stringToEnumValue(info["growthCurve"].at("curveType")))
	{
		case Type::VonBertalanffy:
			return make_unique<VonBertalanffyCurve>(info["growthCurve"]["vonBertalanffyParams"]);
			break;
		case Type::Logistic:
			return make_unique<LogisticCurve>(info["growthCurve"]["logisticParams"]);
			break;
		case Type::Logistic3P:
			return make_unique<Logistic3PCurve>(info["growthCurve"]["logistic3P_Params"]);
			break;
		case Type::Logistic4P:
			return make_unique<Logistic4PCurve>(info["growthCurve"]["logistic4P_Params"]);
			break;
		case Type::Gompertz:
			return make_unique<GompertzCurve>(info["growthCurve"]["gompertzParams"]);
			break;
		case Type::Exponential:
			return make_unique<ExponentialCurve>(info["growthCurve"]["exponentialParams"]);
			break;
		default:
			throwLineInfoException("Default case");
			break;
	}
}

unique_ptr<Curve> Curve::createInstance(const Curve::Type &curveType)
{
	switch(curveType)
	{
		case Type::VonBertalanffy:
			return make_unique<VonBertalanffyCurve>();
			break;
		case Type::Logistic:
			return make_unique<LogisticCurve>();
			break;
		case Type::Logistic3P:
			return make_unique<Logistic3PCurve>();
			break;
		case Type::Logistic4P:
			return make_unique<Logistic4PCurve>();
			break;
		case Type::Gompertz:
			return make_unique<GompertzCurve>();
			break;
		case Type::Exponential:
			return make_unique<ExponentialCurve>();
			break;
		default:
			throwLineInfoException("Default case");
			break;
	}
}


template<class Archive>
void Curve::serialize(Archive & ar, const unsigned int version)
{
	
}

// Specialisation
template void Curve::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Curve::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Curve::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Curve::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);



namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, Curve* &curvePtr, const unsigned int version) {
			Curve::Type curveType;
            
            // For loading
            if(Archive::is_loading::value) 
            {
                ar & curveType;
                curvePtr = Curve::createInstance(curveType).release();
            }
            else
            {
                curveType = curvePtr->getType();
                ar & curveType;
            }

			switch(curveType)
			{
				case Curve::Type::VonBertalanffy:
					static_cast<VonBertalanffyCurve*>(curvePtr)->serialize(ar, version);
					break;
				case Curve::Type::Logistic:
					static_cast<LogisticCurve*>(curvePtr)->serialize(ar, version);
					break;
				case Curve::Type::Logistic3P:
					static_cast<Logistic3PCurve*>(curvePtr)->serialize(ar, version);
					break;
				case Curve::Type::Logistic4P:
					static_cast<Logistic4PCurve*>(curvePtr)->serialize(ar, version);
					break;
				case Curve::Type::Gompertz:
					static_cast<GompertzCurve*>(curvePtr)->serialize(ar, version);
					break;
				case Curve::Type::Exponential:
					static_cast<ExponentialCurve*>(curvePtr)->serialize(ar, version);
					break;
				default:
					throwLineInfoException("Default case");
					break;
			}
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, Curve*&, const unsigned int);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, Curve*&, const unsigned int);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, Curve*&, const unsigned int);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, Curve*&, const unsigned int);
    }
}




unique_ptr<CurveParams> CurveParams::createInstance(const Curve::Type &curveType)
{
	switch(curveType)
	{
		case Curve::Type::VonBertalanffy:
			return make_unique<VonBertalanffyCurveParams>();
			break;
		case Curve::Type::Logistic:
			return make_unique<LogisticCurveParams>();
			break;
		case Curve::Type::Logistic3P:
			return make_unique<Logistic3PCurveParams>();
			break;
		case Curve::Type::Logistic4P:
			return make_unique<Logistic4PCurveParams>();
			break;
		case Curve::Type::Gompertz:
			return make_unique<GompertzCurveParams>();
			break;
		case Curve::Type::Exponential:
			return make_unique<ExponentialCurveParams>();
			break;
		default:
			throwLineInfoException("Default case");
			break;
	}
}


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

template<class Archive>
void CurveParams::serialize(Archive & ar, const unsigned int version)
{
	ar & growthCoefficient;
    ar & time;
}

// Specialisation
template void CurveParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CurveParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CurveParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CurveParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);



namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, CurveParams* &curveParamsPtr, const unsigned int version, const Curve::Type& curveType) {
			if(Archive::is_loading::value) 
            {
                curveParamsPtr = CurveParams::createInstance(curveType).release();
            }

			switch(curveType)
			{
				case Curve::Type::VonBertalanffy:
					static_cast<VonBertalanffyCurveParams*>(curveParamsPtr)->serialize(ar, version);
					break;
				case Curve::Type::Logistic:
					static_cast<LogisticCurveParams*>(curveParamsPtr)->serialize(ar, version);
					break;
				case Curve::Type::Logistic3P:
					static_cast<Logistic3PCurveParams*>(curveParamsPtr)->serialize(ar, version);
					break;
				case Curve::Type::Logistic4P:
					static_cast<Logistic4PCurveParams*>(curveParamsPtr)->serialize(ar, version);
					break;
				case Curve::Type::Gompertz:
					static_cast<GompertzCurveParams*>(curveParamsPtr)->serialize(ar, version);
					break;
				case Curve::Type::Exponential:
					static_cast<ExponentialCurveParams*>(curveParamsPtr)->serialize(ar, version);
					break;
				default:
					throwLineInfoException("Default case");
					break;
			}
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, CurveParams*&, const unsigned int, const Curve::Type&);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, CurveParams*&, const unsigned int, const Curve::Type&);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, CurveParams*&, const unsigned int, const Curve::Type&);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, CurveParams*&, const unsigned int, const Curve::Type&);
    }
}



const double& VonBertalanffyCurveParams::getAsymptoticSize() const
{
	return vonBertAsymptoticSize;
}

void VonBertalanffyCurveParams::setAsymptoticSize(const double &vonBertAsymptoticSize)
{
	this->vonBertAsymptoticSize = vonBertAsymptoticSize;
}

template<class Archive>
void VonBertalanffyCurveParams::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<CurveParams>(*this);

	ar & vonBertAsymptoticSize;
}

// Specialisation
template void VonBertalanffyCurveParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void VonBertalanffyCurveParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void VonBertalanffyCurveParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void VonBertalanffyCurveParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


VonBertalanffyCurve::VonBertalanffyCurve()
	: Curve()
{

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

Curve::Type VonBertalanffyCurve::getType() const
{
	return Curve::Type::VonBertalanffy;
}

double VonBertalanffyCurve::getValue(const CurveParams &params) const
{
	const VonBertalanffyCurveParams& castParams = static_cast<const VonBertalanffyCurveParams&>(params);
	return castParams.getAsymptoticSize()*(1-exp(-castParams.getGrowthCoefficient()*(castParams.getTime()-vonBertTime0)));
}

template<class Archive>
void VonBertalanffyCurve::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<Curve>(*this);

	ar & vonBertTime0;
    ar & vonBertLdistanceMin;
	ar & vonBertLdistanceMax;
}

// Specialisation
template void VonBertalanffyCurve::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void VonBertalanffyCurve::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void VonBertalanffyCurve::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void VonBertalanffyCurve::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


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

template<class Archive>
void LogisticCurveParams::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<CurveParams>(*this);

	ar & logisticAsymptoticSize;
    ar & logisticMidpointValue;
}

// Specialisation
template void LogisticCurveParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void LogisticCurveParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void LogisticCurveParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void LogisticCurveParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);



LogisticCurve::LogisticCurve()
	: Curve()
{

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

Curve::Type LogisticCurve::getType() const
{
	return Curve::Type::Logistic;
}

double LogisticCurve::getValue(const CurveParams &params) const
{
	const LogisticCurveParams& castParams = static_cast<const LogisticCurveParams&>(params);
	return castParams.getAsymptoticSize()/(1+exp((castParams.getMidpointValue()-castParams.getTime())*castParams.getGrowthCoefficient()));
}

template<class Archive>
void LogisticCurve::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<Curve>(*this);

	ar & logisticLdistanceMin;
	ar & logisticLdistanceMax;
}

// Specialisation
template void LogisticCurve::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void LogisticCurve::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void LogisticCurve::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void LogisticCurve::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);



template<class Archive>
void Logistic3PCurveParams::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<CurveParams>(*this);
}

// Specialisation
template void Logistic3PCurveParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Logistic3PCurveParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Logistic3PCurveParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Logistic3PCurveParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


Logistic3PCurve::Logistic3PCurve()
	: Curve()
{

}

Logistic3PCurve::Logistic3PCurve(const unordered_map<string,json>& growthCurve) 
	: Curve(), logistic3pAsymptoticSize(growthCurve.at("logistic3pAsymptoticSize")), logistic3pMidpointValue(logistic3pAsymptoticSize/2)
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

Curve::Type Logistic3PCurve::getType() const
{
	return Curve::Type::Logistic3P;
}

double Logistic3PCurve::getValue(const CurveParams &params) const
{
	const Logistic3PCurveParams& castParams = static_cast<const Logistic3PCurveParams&>(params);
	return getAsymptoticSize()/(1+exp((getMidpointValue()-castParams.getTime())/castParams.getGrowthCoefficient()));
}

template<class Archive>
void Logistic3PCurve::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<Curve>(*this);

	ar & logistic3pAsymptoticSize;
    ar & logistic3pMidpointValue;
}

// Specialisation
template void Logistic3PCurve::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Logistic3PCurve::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Logistic3PCurve::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Logistic3PCurve::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);



template<class Archive>
void Logistic4PCurveParams::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<CurveParams>(*this);
}

// Specialisation
template void Logistic4PCurveParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Logistic4PCurveParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Logistic4PCurveParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Logistic4PCurveParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


Logistic4PCurve::Logistic4PCurve()
	: Curve()
{

}

Logistic4PCurve::Logistic4PCurve(const unordered_map<string,json>& growthCurve) 
	: Curve(), logistic4pLeftAsymptote(growthCurve.at("logistic4pLeftAsymptote")), logistic4pRightAsymptote(growthCurve.at("logistic4pRightAsymptote")),
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

Curve::Type Logistic4PCurve::getType() const
{
	return Curve::Type::Logistic4P;
}

double Logistic4PCurve::getValue(const CurveParams &params) const
{
	const Logistic4PCurveParams& castParams = static_cast<const Logistic4PCurveParams&>(params);
	return getAsymptoticSize()/(1+exp((getMidpointValue()-castParams.getTime())/castParams.getGrowthCoefficient()));
}

template<class Archive>
void Logistic4PCurve::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<Curve>(*this);

	ar & logistic4pLeftAsymptote;
    ar & logistic4pRightAsymptote;
    ar & logistic4pAsymptoticSize;
    ar & logistic4pMidpointValue;
}

// Specialisation
template void Logistic4PCurve::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Logistic4PCurve::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Logistic4PCurve::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Logistic4PCurve::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


template<class Archive>
void GompertzCurveParams::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<CurveParams>(*this);
}

// Specialisation
template void GompertzCurveParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void GompertzCurveParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void GompertzCurveParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void GompertzCurveParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


GompertzCurve::GompertzCurve()
	: Curve()
{

}

GompertzCurve::GompertzCurve(const unordered_map<string,json>& growthCurve) 
	: Curve(), gompertzAsymptoticSize(growthCurve.at("gompertzAsymptoticSize")), gompertzValueTime0(growthCurve.at("gompertzValueTime0"))
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

Curve::Type GompertzCurve::getType() const
{
	return Curve::Type::Gompertz;
}

double GompertzCurve::getValue(const CurveParams &params) const
{
	const GompertzCurveParams& castParams = static_cast<const GompertzCurveParams&>(params);
	return getAsymptoticSize()*exp(-getValueTime0()*pow(castParams.getGrowthCoefficient(),castParams.getTime()));
}

template<class Archive>
void GompertzCurve::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<Curve>(*this);

	ar & gompertzAsymptoticSize;
    ar & gompertzValueTime0;
}

// Specialisation
template void GompertzCurve::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void GompertzCurve::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void GompertzCurve::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void GompertzCurve::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);



template<class Archive>
void ExponentialCurveParams::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<CurveParams>(*this);
}

// Specialisation
template void ExponentialCurveParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ExponentialCurveParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ExponentialCurveParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ExponentialCurveParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


ExponentialCurve::ExponentialCurve()
	: Curve()
{

}

ExponentialCurve::ExponentialCurve(const unordered_map<string,json>& growthCurve) 
	: Curve(), exponentialA(growthCurve.at("exponentialA"))
{
	
}

const double& ExponentialCurve::getExponentialA() const
{
	return exponentialA;
}

Curve::Type ExponentialCurve::getType() const
{
	return Curve::Type::Exponential;
}

double ExponentialCurve::getValue(const CurveParams &params) const
{
	const ExponentialCurveParams& castParams = static_cast<const ExponentialCurveParams&>(params);
	return getExponentialA() * exp(castParams.getGrowthCoefficient() * castParams.getTime());
}

template<class Archive>
void ExponentialCurve::serialize(Archive & ar, const unsigned int version)
{
	ar & boost::serialization::base_object<Curve>(*this);

	ar & exponentialA;
}

// Specialisation
template void ExponentialCurve::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ExponentialCurve::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ExponentialCurve::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ExponentialCurve::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
