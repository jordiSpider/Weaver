
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/ExponentialIndividualGrowthModel.h"

using namespace std;



ExponentialIndividualGrowthModel::ExponentialIndividualGrowthModel() 
    : IndividualGrowthModel() 
{

}

ExponentialIndividualGrowthModel::~ExponentialIndividualGrowthModel() 
{

}

const PreciseDouble& ExponentialIndividualGrowthModel::getA() const
{
    return a;
}

const PreciseDouble& ExponentialIndividualGrowthModel::getB() const
{
    return b;
}

void ExponentialIndividualGrowthModel::adjust(const PreciseDouble& growthCoefficient, const Length& Lmax, const Day& Tmax, const Length& lengthAtBirth)
{
    a = log((Lmax - lengthAtBirth).getValue() / (exp(growthCoefficient*Tmax.getValue()) - 1));

    b = lengthAtBirth.getValue() - exp(a);
}

Length ExponentialIndividualGrowthModel::calculateLength(const Day& age, const PreciseDouble& growthCoefficient) const
{
    return Length(Exponential(growthCoefficient, age));
}

IndividualGrowthModel::Type ExponentialIndividualGrowthModel::getType() const
{
    return Type::Exponential;
}

PreciseDouble ExponentialIndividualGrowthModel::Exponential(const PreciseDouble& k, const Day& t) const
{
    return exp(k*t.getValue() + getA()) + getB();
}



BOOST_CLASS_EXPORT(ExponentialIndividualGrowthModel)

template <class Archive>
void ExponentialIndividualGrowthModel::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<IndividualGrowthModel>(*this);

    ar & a;
    ar & b;
}

// Specialisation
template void ExponentialIndividualGrowthModel::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ExponentialIndividualGrowthModel::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ExponentialIndividualGrowthModel::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ExponentialIndividualGrowthModel::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
