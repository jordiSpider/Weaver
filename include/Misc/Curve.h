#ifndef CURVE_H_
#define CURVE_H_

#include <nlohmann/json.hpp>
#include <string>
#include <cmath>
#include <unordered_map>
#include <magic_enum.hpp>
#include <memory>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"
#include "Misc/EnumClass.h"



class CurveParams;

class Curve {
public:
    enum class Type {
        VonBertalanffy,
        Logistic,
        Logistic3P,
        Logistic4P,
        Gompertz,
        Exponential
    };

    static std::unique_ptr<Curve> createInstance(const nlohmann::json &info);
    static std::unique_ptr<Curve> createInstance(const Type &curveType);

    Curve() {};
    virtual ~Curve() {};

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

    virtual double getValue(const CurveParams &params) const=0;
    virtual Curve::Type getType() const=0;

protected:
    
};

namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, Curve* &curvePtr, const unsigned int version);
    }
}



class CurveParams {
public:
    static std::unique_ptr<CurveParams> createInstance(const Curve::Type &curveType);

    CurveParams() {};
    virtual ~CurveParams() {}

    const double& getGrowthCoefficient() const;
    const double& getTime() const;

    void setGrowthCoefficient(const double &growthCoefficient);
    void setTime(const double &time);

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

protected:
    double growthCoefficient;
    double time;
};


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, CurveParams* &curveParamsPtr, const unsigned int version, const Curve::Type& curveType);
    }
}



class VonBertalanffyCurveParams: public CurveParams {
public:
    VonBertalanffyCurveParams() : CurveParams() {}
    virtual ~VonBertalanffyCurveParams() {}

    const double& getAsymptoticSize() const;

    void setAsymptoticSize(const double &vonBertAsymptoticSize);

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

protected:
    double vonBertAsymptoticSize;
};

class VonBertalanffyCurve: public Curve {
private:
    double vonBertTime0;
    double vonBertLdistanceMin;
	double vonBertLdistanceMax;

public:
    VonBertalanffyCurve();
    VonBertalanffyCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    virtual ~VonBertalanffyCurve() {}

    const double& getVonBertTime0() const;
    const double& getLdistanceMin() const;
    const double& getLdistanceMax() const;
    Curve::Type getType() const;

    double getValue(const CurveParams &params) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


class LogisticCurveParams: public CurveParams {
public:
    LogisticCurveParams() : CurveParams() {}
    virtual ~LogisticCurveParams() {}

    const double& getAsymptoticSize() const;
    const double& getMidpointValue() const;

    void setAsymptoticSize(const double &logisticAsymptoticSize);
    void setMidpointValue(const double &logisticMidpointValue);

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

protected:
    double logisticAsymptoticSize;
    double logisticMidpointValue;
};

class LogisticCurve: public Curve {
private:
    double logisticLdistanceMin;
	double logisticLdistanceMax;

public:
    LogisticCurve();
    LogisticCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    virtual ~LogisticCurve() {}

    const double& getLdistanceMin() const;
    const double& getLdistanceMax() const;
    Curve::Type getType() const;

    double getValue(const CurveParams &params) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


class Logistic3PCurveParams: public CurveParams {
public:
    Logistic3PCurveParams() : CurveParams() {}
    virtual ~Logistic3PCurveParams() {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

protected:
    
};

class Logistic3PCurve: public Curve {
private:
    double logistic3pAsymptoticSize;
    double logistic3pMidpointValue;

public:
    Logistic3PCurve();
    Logistic3PCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    virtual ~Logistic3PCurve() {}

    const double& getAsymptoticSize() const;
    const double& getMidpointValue() const;
    Curve::Type getType() const;

    double getValue(const CurveParams &params) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


class Logistic4PCurveParams: public CurveParams {
public:
    Logistic4PCurveParams() : CurveParams() {}
    virtual ~Logistic4PCurveParams() {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

protected:
    
};

class Logistic4PCurve: public Curve {
private:
    double logistic4pLeftAsymptote;
    double logistic4pRightAsymptote;
    double logistic4pAsymptoticSize;
    double logistic4pMidpointValue;

public:
    Logistic4PCurve();
    Logistic4PCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    virtual ~Logistic4PCurve() {}

    const double& getLeftAsymptote() const;
    const double& getRightAsymptote() const;
    const double& getAsymptoticSize() const;
    const double& getMidpointValue() const;
    Curve::Type getType() const;

    double getValue(const CurveParams &params) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


class GompertzCurveParams: public CurveParams {
public:
    GompertzCurveParams() : CurveParams() {}
    virtual ~GompertzCurveParams() {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

protected:
    
};

class GompertzCurve: public Curve {
private:
    double gompertzAsymptoticSize;
    double gompertzValueTime0;

public:
    GompertzCurve();
    GompertzCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    virtual ~GompertzCurve() {}

    const double& getAsymptoticSize() const;
    const double& getValueTime0() const;
    Curve::Type getType() const;

    double getValue(const CurveParams &params) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


class ExponentialCurveParams: public CurveParams {
public:
    ExponentialCurveParams() : CurveParams() {}
    virtual ~ExponentialCurveParams() {}

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

protected:

};

class ExponentialCurve: public Curve {
private:
    double exponentialA;

public:
    ExponentialCurve();
    ExponentialCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    virtual ~ExponentialCurve() {}

    Curve::Type getType() const;

    const double& getExponentialA() const;
    double getValue(const CurveParams &params) const;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};


#endif // CURVE_H_
