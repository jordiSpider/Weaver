
#include "IBM/World/LivingBeings/Animals/GrowthModule/IndividualGrowthModel.h"

#include "IBM/World/LivingBeings/Animals/AnimalGrowth.h"
#include "IBM/World/LivingBeings/Animals/Animal.h"
#include "IBM/World/LivingBeings/Animals/GrowthModule/VonBertalanffyIndividualGrowthModel.h"
#include "IBM/World/LivingBeings/Animals/GrowthModule/LogisticIndividualGrowthModel.h"
#include "IBM/World/LivingBeings/Animals/GrowthModule/ExponentialIndividualGrowthModel.h"

using namespace std;


unique_ptr<IndividualGrowthModel> IndividualGrowthModel::createInstance(const Type& type)
{
    switch(type) {
        case Type::VonBertalanffy: {
            return make_unique<VonBertalanffyIndividualGrowthModel>();
            break;
        }
        case Type::Logistic: {
            return make_unique<LogisticIndividualGrowthModel>();
            break;
        }
        case Type::Exponential: {
            return make_unique<ExponentialIndividualGrowthModel>();
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}


IndividualGrowthModel::IndividualGrowthModel()
{
    
}

IndividualGrowthModel::~IndividualGrowthModel() 
{

}
