/**
 * @file SerializablePath.h
 * @brief Provides Boost.Serialization support for std::filesystem::path.
 *
 * This file contains a specialization of Boost.Serialization for serializing
 * and deserializing std::filesystem::path objects. Paths are converted to strings
 * for serialization and reconstructed from strings during deserialization.
 */

#ifndef SERIALIZABLE_PATH_H_
#define SERIALIZABLE_PATH_H_

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>

#include <filesystem>


namespace boost {
    namespace serialization {

        /**
         * @brief Serialize or deserialize a std::filesystem::path.
         *
         * This function enables Boost.Serialization to handle std::filesystem::path objects
         * by converting the path to a string for serialization and reconstructing it
         * from the string during deserialization.
         *
         * @tparam Archive Type of the Boost archive (e.g., text_oarchive, binary_iarchive).
         * @param ar Boost archive used for serialization or deserialization.
         * @param serializablePath The path to serialize or deserialize.
         *
         * @note During serialization, the path is converted to a string.
         *       During deserialization, the string is read and converted back to a path.
         */
        template<class Archive>
        void serialize(Archive &ar, std::filesystem::path& serializablePath, const unsigned int) {
            std::string pathString;

            if (Archive::is_loading::value) {
                // Deserialization: read string and construct path
                ar & pathString;
                serializablePath = std::filesystem::path(pathString);
            }
            else {
                // Serialization: convert path to string and save
                pathString = serializablePath.string();
                ar & pathString;
            }
        }
    }
}

#endif /* SERIALIZABLE_PATH_H_ */