
#ifndef SERIALIZABLE_PATH_H_
#define SERIALIZABLE_PATH_H_

#include <boost/serialization/access.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/serialization/string.hpp>


namespace boost {
    namespace serialization {
        /**
         * @brief Serialize or deserialize a boost::filesystem::path.
         *
         * This function is a Boost.Serialization specialization for serializing or deserializing
         * a boost::filesystem::path object. It converts the path to a string and performs the
         * serialization or deserialization accordingly.
         *
         * @tparam Archive The type of the Boost.Serialization archive (e.g., boost::archive::text_oarchive).
         * @param ar The Boost.Serialization archive to perform serialization or deserialization.
         * @param serializablePath The boost::filesystem::path to be serialized or deserialized.
         * @param version An optional parameter representing the version of the serialization format.
         *
         * @note For serialization, the path is converted to a string and serialized.
         *       For deserialization, the string is deserialized, and a boost::filesystem::path is constructed.
         */
        template<class Archive>
        void serialize(Archive &ar, boost::filesystem::path& serializablePath, const unsigned int version) {
            std::string pathString;
            
            // Deserialization
            if (Archive::is_loading::value) {
                // Deserialize the string and construct a path
                ar & pathString;
                serializablePath = boost::filesystem::path(pathString);
            } // Serialization
            else {
                // Convert the path to a string and serialize it
                pathString = serializablePath.string();
                ar & pathString;
            }
        }
    }
}

#endif /* SERIALIZABLE_PATH_H_ */
