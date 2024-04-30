#ifndef PARAMETER_RANGE_FACTORY_H_
#define PARAMETER_RANGE_FACTORY_H_



#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

#include "Tools/Optimiser/ParameterRange.h"
#include "Exceptions/LineInfoException.h"


class ParameterRangeFactory
{
public:
    static std::unique_ptr<ParameterRange> createInstance(const std::vector<nlohmann::json>& rangeInfo);
};

#endif /* PARAMETER_RANGE_FACTORY_H_ */