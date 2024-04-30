
#include "Tools/Optimiser/ParameterRange.h"


using namespace std;
using json = nlohmann::json;


ParameterRange::ParameterRange()
{


}
ParameterRange::~ParameterRange()
{

}


ParameterIntegerRange::ParameterIntegerRange(const vector<json>& values)
    : ParameterRange(), range(createRange(values[0], values[1]))
{
    if(values.size() != 2)
    {
        throwLineInfoException("Error: A range of integers must be a vector in the format {lowerValue, upperValue}.");
    }
}

ParameterIntegerRange::~ParameterIntegerRange()
{

}

vector<json> ParameterIntegerRange::createRange(int lowerValue, int upperValue)
{
    vector<int> range(upperValue - lowerValue + 1);
    iota(range.begin(), range.end(), lowerValue);

    vector<json> castRange;

    std::transform(range.begin(), range.end(), std::back_inserter(castRange),
                   [](int valor) { return nlohmann::json(valor); });

    return castRange;
}

unique_ptr<vector<json>> ParameterIntegerRange::generateRandomValue(unsigned int numberOfValues, const vector<json>&& valuesToAvoid) const
{
    vector<json> values(range);

    unordered_set<json> uniqueSet(valuesToAvoid.begin(), valuesToAvoid.end());

    vector<json> uniqueValuesToAvoid(uniqueSet.begin(), uniqueSet.end());

    for(json& value : uniqueValuesToAvoid)
    {
        values.erase(find(values.begin(), values.end(), value));
    }

    return Random::createUniqueVector<json>(values, numberOfValues);
}

ParameterRange* ParameterIntegerRange::clone() const
{
    return new ParameterIntegerRange(*this);
}


ParameterDoubleRange::ParameterDoubleRange(const vector<json>& values)
    : ParameterRange(), lowerValue(values[0]), upperValue(values[1])
{
    if(values.size() != 2)
    {
        throwLineInfoException("Error: A range of doubles must be a vector in the format {lowerValue, upperValue}.");
    }
}

ParameterDoubleRange::~ParameterDoubleRange()
{

}

unique_ptr<vector<json>> ParameterDoubleRange::generateRandomValue(unsigned int numberOfValues, const vector<json>&& valuesToAvoid) const
{
    unique_ptr<vector<json>> values = make_unique<vector<json>>();
    values->reserve(numberOfValues);

    while(values->size() < numberOfValues)
    {
        double newValue = Random::randomDoubleInRange(lowerValue, upperValue);

        if(find(valuesToAvoid.begin(), valuesToAvoid.end(), newValue) == valuesToAvoid.end())
        {
            if(find(values->begin(), values->end(), newValue) == values->end())
            {
                values->push_back(newValue);
            }
        }
    }

    return values;
}

ParameterRange* ParameterDoubleRange::clone() const
{
    return new ParameterDoubleRange(*this);
}


ParameterBooleanRange::ParameterBooleanRange(const vector<json>& values)
    : ParameterRange(), enumValues(values)
{
    if(values.empty())
    {
        throwLineInfoException("Error: Range value cannot be empty.");
    }
}

ParameterBooleanRange::~ParameterBooleanRange()
{

}

unique_ptr<vector<json>> ParameterBooleanRange::generateRandomValue(unsigned int numberOfValues, const vector<json>&& valuesToAvoid) const
{
    vector<json> values(enumValues);

    unordered_set<json> uniqueSet(valuesToAvoid.begin(), valuesToAvoid.end());
    vector<json> uniqueValuesToAvoid(uniqueSet.begin(), uniqueSet.end());

    for(json& value : uniqueValuesToAvoid)
    {
        values.erase(find(values.begin(), values.end(), value));
    }

    return Random::createUniqueVector<json>(values, numberOfValues);
}

ParameterRange* ParameterBooleanRange::clone() const
{
    return new ParameterBooleanRange(*this);
}


ParameterCharacterRange::ParameterCharacterRange(const vector<json>& values)
    : ParameterRange(), enumValues(values)
{
    if(values.empty())
    {
        throwLineInfoException("Error: Range value cannot be empty.");
    }
}

ParameterCharacterRange::~ParameterCharacterRange()
{

}

unique_ptr<vector<json>> ParameterCharacterRange::generateRandomValue(unsigned int numberOfValues, const vector<json>&& valuesToAvoid) const
{
    vector<json> values(enumValues);

    unordered_set<json> uniqueSet(valuesToAvoid.begin(), valuesToAvoid.end());
    vector<json> uniqueValuesToAvoid(uniqueSet.begin(), uniqueSet.end());

    for(json& value : uniqueValuesToAvoid)
    {
        values.erase(find(values.begin(), values.end(), value));
    }

    return Random::createUniqueVector<json>(values, numberOfValues);
}

ParameterRange* ParameterCharacterRange::clone() const
{
    return new ParameterCharacterRange(*this);
}
