
#include "Tools/Optimiser/ParameterRangeFactory.h"


using namespace std;
using json = nlohmann::json;


unique_ptr<ParameterRange> ParameterRangeFactory::createInstance(const vector<json>& rangeInfo)
{
    switch(rangeInfo.back().type()) 
    {
        case json::value_t::boolean:
            return make_unique<ParameterBooleanRange>(rangeInfo);
            break;
        case json::value_t::number_integer:
        case json::value_t::number_unsigned:
            return make_unique<ParameterIntegerRange>(rangeInfo);
            break;
        case json::value_t::number_float:
            return make_unique<ParameterDoubleRange>(rangeInfo);
            break;
        case json::value_t::string:
            return make_unique<ParameterCharacterRange>(rangeInfo);
            break;
        default:
            throwLineInfoException("Error: Unrecognised type.");
            break;
    }
}
