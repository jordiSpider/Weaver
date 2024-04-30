
#ifndef SERIALIZABLE_VECTOR_H_
#define SERIALIZABLE_VECTOR_H_

#include <boost/serialization/access.hpp>
#include <vector>


namespace boost {
    namespace serialization {
        template<class Archive, typename T>
        void serialize(Archive &ar, std::vector<T> &serializableVector, const unsigned int version) {
            size_t size;

            // Deserialization
            if (Archive::is_loading::value) {
                ar & size;

                serializableVector.resize(size);

                for(size_t i = 0; i < size; ++i) {
                    ar & serializableVector[i];
                }
            } // Serialization
            else {
                size = serializableVector.size();
                ar & size;

                for (auto& elem : serializableVector) {
                    ar & elem;
                }
            }
        }
    }
}

#endif /* SERIALIZABLE_VECTOR_H_ */
