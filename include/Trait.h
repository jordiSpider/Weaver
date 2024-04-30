#ifndef TRAIT_H_
#define TRAIT_H_

#include <string>
#include <unordered_map>
#include "LineInfoException.h"

#include <magic_enum.hpp>

#define FMT_HEADER_ONLY
#include <fmt/core.h>

class Trait {
public:
	enum TraitType {
		energy_tank,
		growth,
		pheno, 
		factorEggMass,
		assim,
		voracity,
		speed,
		search_area,
		met_rate,
		actE_vor,
		actE_speed,
		actE_search,
		shock_resistance,
		actE_met,
        memoryDepth
	};

	inline static const TraitType stringToEnumValue(const std::string &str) { 
        try
        {
            return stringToEnum.at(str);
        }
        catch(const std::out_of_range& e) 
        {
            throwLineInfoException(fmt::format("Unknown trait type '{}'. Valid values are {}", str, printAvailableValues()));
        }
    }
	inline static std::string_view printAvailableTraits() { return enumHeader; }
	inline static constexpr size_t size() { return magic_enum::enum_count<TraitType>(); }
    inline const TraitType& getType() const { return type; }
    virtual bool isFixedTrait() const=0;

    explicit Trait(const std::string& typeStr);
    ~Trait() {};

protected:
    const TraitType type;

private:
	static const std::unordered_map<std::string_view, const TraitType> stringToEnum;
	static const std::string enumHeader;
    static const std::string enumValues;

	inline static std::string_view to_string(const TraitType& trait) { return magic_enum::enum_name(trait); }
	static const std::unordered_map<std::string_view, const TraitType> generateMap();
	static const std::string generateHeader(); 
    inline static std::string_view printAvailableValues() { return enumValues; }
    static const std::string generateAvailableValues();   
};

typedef Trait::TraitType TraitType;

class FixedTrait: public Trait {
private:
    double value;

public:
    explicit FixedTrait(const std::string& typeStr, const double& value);
    ~FixedTrait() {};

    inline const double& getValue() const { return value; }
    inline void setValue(const double& value) { this->value = value; }
    virtual bool isFixedTrait() const { return true; }
};

class VariableTrait: public Trait {
private:
    unsigned int order;
    double minTraitRange;
    double maxTraitRange;
    double minRestrictedRange;
    double maxRestrictedRange;
    double minTraitLimit;
    double maxTraitLimit;
    double minObservedPseudoValue;
    double maxObservedPseudoValue;
    double restrictPerTrait;

public:
    explicit VariableTrait(const std::string& typeStr, const unsigned int& order, 
                  const double& minTraitRange, const double& maxTraitRange, 
                  const double& minTraitLimit, const double& maxTraitLimit,
                  const double& restrictPerTrait);
    ~VariableTrait() {};

    inline const unsigned int& getOrder() const { return order; }
    inline const double& getMinTraitRange() const { return minTraitRange; }
    inline const double& getMaxTraitRange() const { return maxTraitRange; }
    inline const double& getMinRestrictedRange() const { return minRestrictedRange; }
    inline const double& getMaxRestrictedRange() const { return maxRestrictedRange; }
    inline const double& getMinTraitLimit() const { return minTraitLimit; }
    inline const double& getMaxTraitLimit() const { return maxTraitLimit; }
    inline const double& getMinObservedPseudoValue() const { return minObservedPseudoValue; }
    inline const double& getMaxObservedPseudoValue() const { return maxObservedPseudoValue; }
    inline void setMinObservedPseudoValue(const double& minObservedPseudoValue) { this->minObservedPseudoValue = minObservedPseudoValue; }
    inline void setMaxObservedPseudoValue(const double& maxObservedPseudoValue) { this->maxObservedPseudoValue = maxObservedPseudoValue; }
    virtual bool isFixedTrait() const { return false; }
};

#endif // TRAIT_H_
