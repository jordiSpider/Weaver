
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Growth/Model/LinearIndividualGrowthModel.h"

using namespace std;



LinearIndividualGrowthModel::LinearIndividualGrowthModel() 
    : IndividualGrowthModel() 
{

}

LinearIndividualGrowthModel::~LinearIndividualGrowthModel() 
{

}

const Length& LinearIndividualGrowthModel::getLengthAtBirth() const
{
    return lengthAtBirth;
}

void LinearIndividualGrowthModel::adjust(const PreciseDouble&, const Length&, const Day&, const Length& lengthAtBirthValue)
{
    lengthAtBirth = lengthAtBirthValue;
}

Length LinearIndividualGrowthModel::calculateLength(const Day& age, const PreciseDouble& growthCoefficient) const
{
    return Length(Linear(growthCoefficient, age));
}

IndividualGrowthModel::Type LinearIndividualGrowthModel::getType() const
{
    return Type::Linear;
}

PreciseDouble LinearIndividualGrowthModel::Linear(const PreciseDouble& k, const Day& t) const
{
    return getLengthAtBirth().getValue() + k*t.getValue();
}



BOOST_CLASS_EXPORT(LinearIndividualGrowthModel)

template <class Archive>
void LinearIndividualGrowthModel::serialize(Archive &ar, const unsigned int) 
{
    ar & boost::serialization::base_object<IndividualGrowthModel>(*this);

    ar & lengthAtBirth;
}

// Specialisation
template void LinearIndividualGrowthModel::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void LinearIndividualGrowthModel::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void LinearIndividualGrowthModel::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void LinearIndividualGrowthModel::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
