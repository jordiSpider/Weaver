
#include "IBM/World/Map/Points/PointContinuous.h"

using namespace std;


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, PointContinuous &pointContinuous, const unsigned int version) {
            vector<double> values;
            values.reserve(DIMENSIONS);
            
            // For loading
            if(Archive::is_loading::value) 
            {
                ar & values;
                for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
                {
                    setPositionAxisValue(pointContinuous, axis, values[axis]);
                }
            }
            else
            {
                for(unsigned int axis = 0; axis < DIMENSIONS; axis++)
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

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, PointContinuous*&, const unsigned int);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, PointContinuous*&, const unsigned int);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, PointContinuous*&, const unsigned int);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, PointContinuous*&, const unsigned int);
    }
}