
#include "App/Model/IBM/Landscape/Map/Points/PointContinuous.h"

using namespace std;



PreciseDouble getPositionAxisValue(const PointContinuous& position, const unsigned char axis)
{
    #if DIMENSIONS == 2
        switch(axis) {
            case 0: {
                return position.get<0>();
                break;
            }
            case 1: {
                return position.get<1>();
                break;
            }
            default: {
                throwLineInfoException("Default case");
                break;
            }
        }
    #elif DIMENSIONS == 3
        switch(axis) {
            case 0: {
                return position.get<0>();
                break;
            }
            case 1: {
                return position.get<1>();
                break;
            }
            case 2: {
                return position.get<2>();
                break;
            }
            default: {
                throwLineInfoException("Default case");
                break;
            }
        }
    #else
        throwLineInfoException("Invalid DIMENSIONS value");
    #endif
}

void setPositionAxisValue(PointContinuous& position, const unsigned char axis, const PreciseDouble& newValue)
{
    #if DIMENSIONS == 2
        switch(axis) {
            case 0: {
                return position.set<0>(newValue.getValue());
                break;
            }
            case 1: {
                return position.set<1>(newValue.getValue());
                break;
            }
            default: {
                throwLineInfoException("Default case");
                break;
            }
        }
    #elif DIMENSIONS == 3
        switch(axis) {
            case 0: {
                return position.set<0>(newValue.getValue());
                break;
            }
            case 1: {
                return position.set<1>(newValue.getValue());
                break;
            }
            case 2: {
                return position.set<2>(newValue.getValue());
                break;
            }
            default: {
                throwLineInfoException("Default case");
                break;
            }
        }
    #else
        throwLineInfoException("Invalid DIMENSIONS value");
    #endif
}


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, PointContinuous &pointContinuous, const unsigned int) {
            vector<PreciseDouble> values;
            values.reserve(DIMENSIONS);

            // For loading
            if(Archive::is_loading::value) 
            {
                ar & values;
                for(unsigned char axis = 0; axis < DIMENSIONS; axis++)
                {
                    setPositionAxisValue(pointContinuous, axis, values[axis]);
                }
            }
            else
            {
                for(unsigned char axis = 0; axis < DIMENSIONS; axis++)
                {
                    values.push_back(getPositionAxisValue(pointContinuous, axis));
                }
                ar & values;
            }
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, PointContinuous&, const unsigned int);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, PointContinuous&, const unsigned int);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, PointContinuous&, const unsigned int);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, PointContinuous&, const unsigned int);


        template<class Archive>
        void serialize(Archive &ar, PointContinuous* &pointContinuousPtr, const unsigned int version) {
            // For loading
            if(Archive::is_loading::value) 
            {
                pointContinuousPtr = new PointContinuous();
            }

            ar & *pointContinuousPtr;
        }
    }
}
