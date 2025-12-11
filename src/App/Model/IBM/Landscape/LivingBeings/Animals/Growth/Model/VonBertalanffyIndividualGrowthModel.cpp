
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/VonBertalanffyIndividualGrowthModel.h"


using namespace std;



VonBertalanffyIndividualGrowthModel::VonBertalanffyIndividualGrowthModel() 
    : IndividualGrowthModel() 
{

}

VonBertalanffyIndividualGrowthModel::~VonBertalanffyIndividualGrowthModel() 
{

}

const PreciseDouble& VonBertalanffyIndividualGrowthModel::getLinf() const
{
    return Linf;
}

const PreciseDouble& VonBertalanffyIndividualGrowthModel::getT0() const
{
    return t0;
}

void VonBertalanffyIndividualGrowthModel::adjust(const PreciseDouble& growthCoefficient, const Length& Lmax, const Day& Tmax, const Length& lengthAtBirth)
{
    Linf = (Lmax.getValue()*exp(growthCoefficient*Tmax.getValue()) - lengthAtBirth.getValue()) / (exp(growthCoefficient*Tmax.getValue()) - 1);

    t0 = log(1-(lengthAtBirth.getValue()/Linf))/growthCoefficient;
}

Length VonBertalanffyIndividualGrowthModel::calculateLength(const Day& age, const PreciseDouble& growthCoefficient) const
{
    return Length(VonBertalanffy(growthCoefficient, age));
}

IndividualGrowthModel::Type VonBertalanffyIndividualGrowthModel::getType() const
{
    return Type::VonBertalanffy;
}

PreciseDouble VonBertalanffyIndividualGrowthModel::VonBertalanffy(const PreciseDouble& k, const Day& t) const
{
    return getLinf()*(1-exp(-k*(t.getValue() - getT0())));
}




BOOST_CLASS_EXPORT(VonBertalanffyIndividualGrowthModel)

template <class Archive>
void VonBertalanffyIndividualGrowthModel::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<IndividualGrowthModel>(*this);

    ar & Linf;
    ar & t0;
}

// Specialisation
template void VonBertalanffyIndividualGrowthModel::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void VonBertalanffyIndividualGrowthModel::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void VonBertalanffyIndividualGrowthModel::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void VonBertalanffyIndividualGrowthModel::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
