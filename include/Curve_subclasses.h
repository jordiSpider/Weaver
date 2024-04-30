#ifndef CURVE_H_
#define CURVE_H_

#include "nlohmann/json.h"
#include <string>
#include "LineInfoException.h"
#include <unordered_map>

#include "magic_enum/magic_enum.h"

#define FMT_HEADER_ONLY
#include "fmt/core.h"


// ODIN : HE INTENTADO (UN POCO AL TUNTUN) QUE LA ANTIGUA CLASE CURVEPARAMS Y CURVETYPE SE LLAMEN AHORA GROWTHCURVEPARAM Y GROWTHCURVETYPE, Y AÑADIR LAS DISTRIBUTIONCURVEPARAM Y DISTRIBUTIONCURVETYPE. HE INTENTADO QUE AMBAS HEREDEN DE UNA CLASE CURVEPARAMS Y CURVETYPE - Y PARA ESO HE TENIDO QUE MOVER LOS CONSTRUCTORES Y OTROS METODOS (ESTO ES LO QUE NO SE SI HICE BIEN)

// Abstract base class - CurveParams designed to contain most 1D functions that we may need for several calculations in companion with a sister class named Curve
// one contain all functions to use the curve and the other is just a parameter container - there must be a match among subclasses


// Curve Parameters 
class CurveParams{
public :
	// CONSTRUCTOR AND DESTRUCTOR 
	CurveParams(){};
	~CurveParams(){};
	
};

class DistributionCurveParams : public CurveParams {
public:
    DistributionCurveParams() {};
    ~DistributionCurveParams() {}
	// specific getter and setter for the Abscissa, this could be implemented in
	//  the abstract class
	
    const double& getX() const; // commment : why this const at the end of the line?
    void setX(const double &X);

protected :
    double X;
};


class GrowthCurveParams : public CurveParams {
public:
    GrowthCurveParams() {};
    ~GrowthCurveParams() {}

    const double& getGrowthCoefficient() const;
    void setGrowthCoefficient(const double &growthCoefficient);

    const double& getTime() const;
    void setTime(const double &time);

protected:
    double time;
    double growthCoefficient;
};


// Abstract class curve - with two types of classes that match the curve parameter classes
// Rather than inheritance, they have been designed as nested class but I can´t see the reason

class Curve {
public:
    class CurveType{
    public:
	enum CurveTypeValue: int ;

	inline static const CurveTypeValue stringToEnumValue(const std::string &str) { 
	   try{   return stringToEnum.at(str);}catch(const std::out_of_range& e) {
		throwLineInfoException(fmt::format("Unknown curve type '{}'. Valid values are {}", str, printAvailableValues()));}
	}

        inline static constexpr size_t size() { return magic_enum::enum_count<CurveTypeValue>(); }

        virtual CurveType::CurveTypeValue getType() const=0; 

   private:
	static const std::unordered_map<std::string_view, const CurveTypeValue> stringToEnum;
	static const std::string enumValues;

	inline static std::string_view to_string(const CurveTypeValue& trait) { return magic_enum::enum_name(trait); }
	static const std::unordered_map<std::string_view, const CurveTypeValue> generateMap();
	inline static std::string_view printAvailableValues() { return enumValues; }
	static const std::string generateAvailableValues();   
    };


    class DistributionCurveType : public CurveType{
    public:
        enum CurveTypeValue {
	    NormalDistribution
	};

    };
    class GrowthCurveType : public CurveType{
    public:
        enum CurveTypeValue {
            VonBertalanffy,
            Logistic,
            Logistic3P,
            Logistic4P,
            Gompertz,
            Exponential
        };

    };


   Curve() {};
   ~Curve() {}

   virtual double getValue(const CurveParams &params) const=0;
//   virtual CurveType::CurveTypeValue getType() const=0;


protected:
    
};

typedef Curve::CurveType CurveType; 
typedef Curve::GrowthCurveType       GrowthCurveType; 
typedef Curve::DistributionCurveType DistribCurveType;


class VonBertalanffyCurveParams: public GrowthCurveParams {
public:
    VonBertalanffyCurveParams() : GrowthCurveParams() {} // porque aquí no necesita el ; ??
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
    GrowthCurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


class LogisticCurveParams: public GrowthCurveParams {
public:
    LogisticCurveParams() : GrowthCurveParams() {}
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
    GrowthCurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


class Logistic3PCurveParams: public GrowthCurveParams {
public:
    Logistic3PCurveParams() : GrowthCurveParams() {}
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
    GrowthCurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


class Logistic4PCurveParams: public GrowthCurveParams {
public:
    Logistic4PCurveParams() : GrowthCurveParams() {}
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
    GrowthCurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


class GompertzCurveParams: public GrowthCurveParams {
public:
    GompertzCurveParams() : GrowthCurveParams() {}
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
    GrowthCurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


class ExponentialCurveParams: public GrowthCurveParams {
public:
    ExponentialCurveParams() : GrowthCurveParams() {}
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

    GrowthCurveType::CurveTypeValue getType() const;

    double getValue(const CurveParams &params) const;
};


#endif // CURVE_H_
