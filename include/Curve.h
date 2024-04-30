#ifndef CURVE_H_
#define CURVE_H_

#include <nlohmann/json.hpp>
#include <string>
#include "LineInfoException.h"
#include <unordered_map>

#include <magic_enum.hpp>

#define FMT_HEADER_ONLY
#include <fmt/core.h>


class CurveParams {
public:
    CurveParams() {};
    ~CurveParams() {}

    const double& getGrowthCoefficient() const;
    const double& getTime() const;

    void setGrowthCoefficient(const double &growthCoefficient);
    void setTime(const double &time);

protected:
    double growthCoefficient;
    double time;
};

class Curve {
public:
    class CurveType {
    public:
        enum CurveTypeValue {
            VonBertalanffy,
            Logistic,
            Logistic3P,
            Logistic4P,
            Gompertz,
            Exponential
        };

        inline static const CurveTypeValue stringToEnumValue(const std::string &str) { 
            try
            {
                return stringToEnum.at(str);
            }
            catch(const std::out_of_range& e) 
            {
                throwLineInfoException(fmt::format("Unknown curve type '{}'. Valid values are {}", str, printAvailableValues()));
            }
        }
        inline static constexpr size_t size() { return magic_enum::enum_count<CurveTypeValue>(); }

    private:
        static const std::unordered_map<std::string_view, const CurveTypeValue> stringToEnum;
        static const std::string enumValues;

        inline static std::string_view to_string(const CurveTypeValue& trait) { return magic_enum::enum_name(trait); }
        static const std::unordered_map<std::string_view, const CurveTypeValue> generateMap();
        inline static std::string_view printAvailableValues() { return enumValues; }
        static const std::string generateAvailableValues();   
    };

    Curve() {};
    ~Curve() {}

    virtual double getValue(const CurveParams &params) const=0;
    virtual CurveType::CurveTypeValue getType() const=0;

protected:
    
};

typedef Curve::CurveType CurveType;


class VonBertalanffyCurveParams: public CurveParams {
public:
    VonBertalanffyCurveParams() : CurveParams() {}
    ~VonBertalanffyCurveParams() {}

    const double& getAsymptoticSize() const;

    void setAsymptoticSize(const double &vonBertAsymptoticSize);

protected:
    double vonBertAsymptoticSize;
};

class VonBertalanffyCurve: public Curve {
private:
    const double vonBertTime0;
    const double vonBertLdistanceMin;
	const double vonBertLdistanceMax;

public:
    VonBertalanffyCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    ~VonBertalanffyCurve() {}

    const double& getVonBertTime0() const;
    const double& getLdistanceMin() const;
    const double& getLdistanceMax() const;
    CurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


class LogisticCurveParams: public CurveParams {
public:
    LogisticCurveParams() : CurveParams() {}
    ~LogisticCurveParams() {}

    const double& getAsymptoticSize() const;
    const double& getMidpointValue() const;

    void setAsymptoticSize(const double &logisticAsymptoticSize);
    void setMidpointValue(const double &logisticMidpointValue);

protected:
    double logisticAsymptoticSize;
    double logisticMidpointValue;
};

class LogisticCurve: public Curve {
private:
    const double logisticLdistanceMin;
	const double logisticLdistanceMax;

public:
    LogisticCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    ~LogisticCurve() {}

    const double& getLdistanceMin() const;
    const double& getLdistanceMax() const;
    CurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


class Logistic3PCurveParams: public CurveParams {
public:
    Logistic3PCurveParams() : CurveParams() {}
    ~Logistic3PCurveParams() {}

protected:
    
};

class Logistic3PCurve: public Curve {
private:
    const double logistic3pAsymptoticSize;
    const double logistic3pMidpointValue;

public:
    Logistic3PCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    ~Logistic3PCurve() {}

    const double& getAsymptoticSize() const;
    const double& getMidpointValue() const;
    CurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


class Logistic4PCurveParams: public CurveParams {
public:
    Logistic4PCurveParams() : CurveParams() {}
    ~Logistic4PCurveParams() {}

protected:
    
};

class Logistic4PCurve: public Curve {
private:
    const double logistic4pLeftAsymptote;
    const double logistic4pRightAsymptote;
    const double logistic4pAsymptoticSize;
    const double logistic4pMidpointValue;

public:
    Logistic4PCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    ~Logistic4PCurve() {}

    const double& getLeftAsymptote() const;
    const double& getRightAsymptote() const;
    const double& getAsymptoticSize() const;
    const double& getMidpointValue() const;
    CurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


class GompertzCurveParams: public CurveParams {
public:
    GompertzCurveParams() : CurveParams() {}
    ~GompertzCurveParams() {}

protected:
    
};

class GompertzCurve: public Curve {
private:
    const double gompertzAsymptoticSize;
    const double gompertzValueTime0;

public:
    GompertzCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    ~GompertzCurve() {}

    const double& getAsymptoticSize() const;
    const double& getValueTime0() const;
    CurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


class ExponentialCurveParams: public CurveParams {
public:
    ExponentialCurveParams() : CurveParams() {}
    ~ExponentialCurveParams() {}

    const double& getValueTime0() const;

    void setValueTime0(const double &exponentialValueTime0);

protected:
    double exponentialValueTime0;
};

class ExponentialCurve: public Curve {
private:
    

public:
    ExponentialCurve(const std::unordered_map<std::string,nlohmann::json>& growthCurve);
    ~ExponentialCurve() {}

    CurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


#endif // CURVE_H_
