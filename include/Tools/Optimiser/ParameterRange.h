#ifndef PARAMETER_RANGE_H_
#define PARAMETER_RANGE_H_


#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include <unordered_set>
#include <algorithm>

#include "IBM/Maths/Random.h"
#include "Exceptions/LineInfoException.h"


class ParameterRange
{
public:
    ParameterRange();
    virtual ~ParameterRange();

    virtual std::unique_ptr<std::vector<nlohmann::json>> generateRandomValue(unsigned int numberOfValues, const std::vector<nlohmann::json>&& valuesToAvoid) const=0;

    virtual ParameterRange* clone() const=0;
};


class ParameterIntegerRange : public ParameterRange
{
private:
    const std::vector<nlohmann::json> range;

    std::vector<nlohmann::json> createRange(int lowerValue, int upperValue);

public:
    ParameterIntegerRange(const std::vector<nlohmann::json>& values);
    virtual ~ParameterIntegerRange();

    std::unique_ptr<std::vector<nlohmann::json>> generateRandomValue(unsigned int numberOfValues, const std::vector<nlohmann::json>&& valuesToAvoid) const;

    ParameterRange* clone() const;
};


class ParameterDoubleRange : public ParameterRange
{
private:
    const double lowerValue;
    const double upperValue;

public:
    ParameterDoubleRange(const std::vector<nlohmann::json>& values);
    virtual ~ParameterDoubleRange();

    std::unique_ptr<std::vector<nlohmann::json>> generateRandomValue(unsigned int numberOfValues, const std::vector<nlohmann::json>&& valuesToAvoid) const;

    ParameterRange* clone() const;
};


class ParameterBooleanRange : public ParameterRange
{
private:
    const std::vector<nlohmann::json> enumValues;

public:
    ParameterBooleanRange(const std::vector<nlohmann::json>& values);
    virtual ~ParameterBooleanRange();

    std::unique_ptr<std::vector<nlohmann::json>> generateRandomValue(unsigned int numberOfValues, const std::vector<nlohmann::json>&& valuesToAvoid) const;

    ParameterRange* clone() const;
};


class ParameterCharacterRange : public ParameterRange
{
private:
    const std::vector<nlohmann::json> enumValues;

public:
    ParameterCharacterRange(const std::vector<nlohmann::json>& values);
    virtual ~ParameterCharacterRange();

    std::unique_ptr<std::vector<nlohmann::json>> generateRandomValue(unsigned int numberOfValues, const std::vector<nlohmann::json>&& valuesToAvoid) const;

    ParameterRange* clone() const;
};

#endif /* PARAMETER_RANGE_H_ */