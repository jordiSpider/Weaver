#ifndef EVALUATION_FUNCTION_H_
#define EVALUATION_FUNCTION_H_


#include <nlohmann/json.hpp>

#include "Exceptions/LineInfoException.h"



using EvaluationFunctionType = double (*)(const nlohmann::json&);


double amplitudeFunction(const nlohmann::json& info);

class EvaluationFunction
{
public:
    static EvaluationFunctionType selectFunction(std::string evaluationFunctionSelected);
};




#endif /* EVALUATION_FUNCTION_H_ */