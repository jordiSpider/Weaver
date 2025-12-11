
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Decisions/SensoryModel.h"


using namespace std;
using json = nlohmann::json;


SensoryModel::SensoryModel()
{

}

SensoryModel::SensoryModel(const json& config)
    : beta(config["beta"].get<double>())
{
    
}

SensoryModel::~SensoryModel()
{
    
}



PreciseDouble SensoryModel::calculateDecayRate(const PreciseDouble& maximumDistance) const
{
    return pow(-log(EPSILON), 1/beta) / (maximumDistance + EPSILON);
}

PreciseDouble SensoryModel::calculateProbability(const PreciseDouble& distance, const PreciseDouble& maximumDistance) const
{
    PreciseDouble distanceToUse = fmax(0.0, distance);

    if(distanceToUse >= maximumDistance)
    {
        return 0.0;
    }
    else
    {
        PreciseDouble decayRate = calculateDecayRate(maximumDistance);

        return exp(-pow(decayRate*distanceToUse, beta));
    }
}


template <class Archive>
void SensoryModel::serialize(Archive &ar, const unsigned int) {
    ar & beta;
}

// Specialisation
template void SensoryModel::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SensoryModel::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SensoryModel::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SensoryModel::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
