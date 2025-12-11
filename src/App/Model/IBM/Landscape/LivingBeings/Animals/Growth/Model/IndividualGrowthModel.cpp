
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/IndividualGrowthModel.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/LinearIndividualGrowthModel.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/VonBertalanffyIndividualGrowthModel.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/LogisticIndividualGrowthModel.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/Logistic4PIndividualGrowthModel.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/ExponentialIndividualGrowthModel.h"

using namespace std;



IndividualGrowthModel* IndividualGrowthModel::createInstance(const Type& type, const PreciseDouble& A)
{
    switch(type) {
        case Type::Linear: {
            return new LinearIndividualGrowthModel();
            break;
        }
        case Type::VonBertalanffy: {
            return new VonBertalanffyIndividualGrowthModel();
            break;
        }
        case Type::Logistic: {
            return new LogisticIndividualGrowthModel();
            break;
        }
        case Type::Logistic4P: {
            return new Logistic4PIndividualGrowthModel(A);
            break;
        }
        case Type::Exponential: {
            return new ExponentialIndividualGrowthModel();
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



BOOST_SERIALIZATION_ASSUME_ABSTRACT(IndividualGrowthModel)

template <class Archive>
void IndividualGrowthModel::serialize(Archive &, const unsigned int) 
{
    
}

// // Specialisation
template void IndividualGrowthModel::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void IndividualGrowthModel::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void IndividualGrowthModel::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void IndividualGrowthModel::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
