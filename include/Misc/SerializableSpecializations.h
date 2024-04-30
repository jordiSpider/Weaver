/**
 * @file SerializableSpecializations.h
 *
 * @brief Specializations for Boost.Serialization.
 *
 * This header provides specializations for Boost.Serialization to handle serialization
 * and deserialization of specific types, such as boost::filesystem::path and std::vector<T>.
 * These specializations allow seamless integration of these types into the Boost.Serialization framework.
 */
#ifndef SERIALIZABLE_SPECIALIZATIONS_H_
#define SERIALIZABLE_SPECIALIZATIONS_H_

#include <boost/serialization/access.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/serialization/string.hpp>

#include "IBM/World/Map/Map.h"
#include "IBM/World/Map/TerrainCells/Moisture/ExtendedMoisture.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"

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
        void serialize(Archive &ar, boost::filesystem::path &serializablePath, const unsigned int version) {
            // Deserialization
            if (Archive::is_loading::value) {
                // Deserialize the string and construct a path
                std::string pathString;
                ar & pathString;
                serializablePath = boost::filesystem::path(pathString);
            } // Serialization
            else {
                // Convert the path to a string and serialize it
                std::string pathString = serializablePath.string();
                ar & pathString;
            }
        }


        /**
         * @brief Serialize or deserialize a std::vector<T>.
         *
         * This function is a Boost.Serialization specialization for serializing or deserializing
         * a std::vector<T> object. It handles the serialization or deserialization of the vector's elements.
         *
         * @tparam Archive The type of the Boost.Serialization archive (e.g., boost::archive::text_oarchive).
         * @tparam T The type of elements stored in the std::vector.
         * @param ar The Boost.Serialization archive to perform serialization or deserialization.
         * @param serializableVector The std::vector<T> to be serialized or deserialized.
         * @param version An optional parameter representing the version of the serialization format.
         *
         * @note For serialization, the elements of the vector are serialized individually.
         *       For deserialization, the vector is resized to match the serialized size,
         *       and elements are deserialized into the vector.
         */
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

        /**
         * @brief Serialize a std::unique_ptr<Map>.
         *
         * This function handles the serialization and deserialization of a std::unique_ptr<Map>.
         *
         * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
         * @param ar The archive to use.
         * @param serializableUniquePtr The std::unique_ptr<Map> to serialize or deserialize.
         * @param version The version of the serialization format.
         */
        template <class Archive>
        void serialize(Archive &ar, std::unique_ptr<Map> &serializableUniquePtr, const unsigned int version) {
            // For loading
            if (Archive::is_loading::value) {
                // Assuming Map has a default constructor
                serializableUniquePtr = std::make_unique<Map>();
                ar & *serializableUniquePtr;  // Deserialize the content of the Map
            } else { // For saving
                ar & *serializableUniquePtr; // Serialize the content of the Map
            }
        }

        /**
         * @brief Serialize a std::unique_ptr<ExtendedMoisture>.
         *
         * This function handles the serialization and deserialization of a std::unique_ptr<ExtendedMoisture>.
         *
         * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
         * @param ar The archive to use.
         * @param serializableUniquePtr The std::unique_ptr<ExtendedMoisture> to serialize or deserialize.
         * @param version The version of the serialization format.
         */
        template <class Archive>
        void serialize(Archive &ar, std::unique_ptr<ExtendedMoisture> &serializableUniquePtr, const unsigned int version) {
            // For loading
            if (Archive::is_loading::value) {
                // Assuming Map has a default constructor
                serializableUniquePtr = std::make_unique<ExtendedMoisture>();
                ar & *serializableUniquePtr;  // Deserialize the content of the Map
            } else { // For saving
                ar & *serializableUniquePtr; // Serialize the content of the Map
            }
        }

        /**
         * @brief Serialize a std::unique_ptr<ResourceSpecies>.
         *
         * This function handles the serialization and deserialization of a std::unique_ptr<ResourceSpecies>.
         *
         * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
         * @param ar The archive to use.
         * @param serializableUniquePtr The std::unique_ptr<ResourceSpecies> to serialize or deserialize.
         * @param version The version of the serialization format.
         */
        template <class Archive>
        void serialize(Archive &ar, std::unique_ptr<ResourceSpecies> &serializableUniquePtr, const unsigned int version) {
            // For loading
            if (Archive::is_loading::value) {
                // Assuming Map has a default constructor
                serializableUniquePtr = std::make_unique<ResourceSpecies>();
                ar & *serializableUniquePtr;  // Deserialize the content of the Map
            } else { // For saving
                ar & *serializableUniquePtr; // Serialize the content of the Map
            }
        }

        /**
         * @brief Serialize a std::unique_ptr<AnimalSpecies>.
         *
         * This function handles the serialization and deserialization of a std::unique_ptr<AnimalSpecies>.
         *
         * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
         * @param ar The archive to use.
         * @param serializableUniquePtr The std::unique_ptr<AnimalSpecies> to serialize or deserialize.
         * @param version The version of the serialization format.
         */
        template <class Archive>
        void serialize(Archive &ar, std::unique_ptr<AnimalSpecies> &serializableUniquePtr, const unsigned int version) {
            // For loading
            if (Archive::is_loading::value) {
                // Assuming Map has a default constructor
                serializableUniquePtr = std::make_unique<AnimalSpecies>();
                ar & *serializableUniquePtr;  // Deserialize the content of the Map
            } else { // For saving
                ar & *serializableUniquePtr; // Serialize the content of the Map
            }
        }

    } // namespace serializati
} // namespace boost

#endif //SERIALIZABLE_SPECIALIZATIONS_H_