
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/LogisticIndividualGrowthModel.h"


using namespace std;



LogisticIndividualGrowthModel::LogisticIndividualGrowthModel() 
    : IndividualGrowthModel() 
{

}

LogisticIndividualGrowthModel::~LogisticIndividualGrowthModel() 
{

}

const PreciseDouble& LogisticIndividualGrowthModel::getLinf() const
{
    return Linf;
}

const PreciseDouble& LogisticIndividualGrowthModel::getXmid() const
{
    return xmid;
}

void LogisticIndividualGrowthModel::adjust(const PreciseDouble& growthCoefficient, const Length& Lmax, const Day& Tmax, const Length& lengthAtBirth)
{
    Linf = (Lmax.getValue()*lengthAtBirth.getValue()*(exp(growthCoefficient*Tmax.getValue())-1)) / (lengthAtBirth.getValue()*exp(growthCoefficient*Tmax.getValue())-Lmax.getValue());

    if(Linf <= 0.0)
    {
        throwLineInfoException("The input values to the growth model are out of range.");
    }

    xmid = log((Linf-lengthAtBirth.getValue()) / lengthAtBirth.getValue()) / growthCoefficient;
}

Length LogisticIndividualGrowthModel::calculateLength(const Day& age, const PreciseDouble& growthCoefficient) const
{
    return Length(Logistic(growthCoefficient, age));
}

IndividualGrowthModel::Type LogisticIndividualGrowthModel::getType() const
{
    return Type::Logistic;
}

PreciseDouble LogisticIndividualGrowthModel::Logistic(const PreciseDouble& k, const Day& t) const
{
    return getLinf()/(1+exp(k*(getXmid()-t.getValue())));
}




BOOST_CLASS_EXPORT(LogisticIndividualGrowthModel)

template <class Archive>
void LogisticIndividualGrowthModel::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<IndividualGrowthModel>(*this);

    ar & Linf;
    ar & xmid;
}

// Specialisation
template void LogisticIndividualGrowthModel::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void LogisticIndividualGrowthModel::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void LogisticIndividualGrowthModel::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void LogisticIndividualGrowthModel::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
