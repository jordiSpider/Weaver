
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/Logistic4PIndividualGrowthModel.h"


using namespace std;



Logistic4PIndividualGrowthModel::Logistic4PIndividualGrowthModel() 
    : IndividualGrowthModel()
{

}

Logistic4PIndividualGrowthModel::Logistic4PIndividualGrowthModel(const PreciseDouble& A) 
    : IndividualGrowthModel(), A(A)
{

}

Logistic4PIndividualGrowthModel::~Logistic4PIndividualGrowthModel() 
{

}

const PreciseDouble& Logistic4PIndividualGrowthModel::getXmid() const
{
    return xmid;
}

const PreciseDouble& Logistic4PIndividualGrowthModel::getA() const
{
    return A;
}

const PreciseDouble& Logistic4PIndividualGrowthModel::getB() const
{
    return B;
}

void Logistic4PIndividualGrowthModel::adjust(const PreciseDouble& growthCoefficient, const Length& Lmax, const Day& Tmax, const Length& lengthAtBirth)
{
    PreciseDouble numerator = Lmax.getValue() - lengthAtBirth.getValue();
    PreciseDouble denominator = (lengthAtBirth.getValue() - getA()) - (Lmax.getValue() - getA()) * exp(-Tmax.getValue() * growthCoefficient);

    xmid = (1.0 / growthCoefficient) * log(numerator / denominator);

    B = (lengthAtBirth.getValue() - getA())*(1+exp(getXmid()*growthCoefficient)) + getA();
}

Length Logistic4PIndividualGrowthModel::calculateLength(const Day& age, const PreciseDouble& growthCoefficient) const
{
    return Length(Logistic4P(growthCoefficient, age));
}

IndividualGrowthModel::Type Logistic4PIndividualGrowthModel::getType() const
{
    return Type::Logistic4P;
}

PreciseDouble Logistic4PIndividualGrowthModel::Logistic4P(const PreciseDouble& k, const Day& t) const
{
    return getA() + (getB() - getA())/(1+exp((getXmid() - t.getValue())*k));
}




BOOST_CLASS_EXPORT(Logistic4PIndividualGrowthModel)

template <class Archive>
void Logistic4PIndividualGrowthModel::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<IndividualGrowthModel>(*this);

    ar & xmid;
    ar & A;
    ar & B;
}

// Specialisation
template void Logistic4PIndividualGrowthModel::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Logistic4PIndividualGrowthModel::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Logistic4PIndividualGrowthModel::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Logistic4PIndividualGrowthModel::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
