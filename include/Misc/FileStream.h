#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "Misc/SerializablePath.h"



/**
 * @brief A class that extends std::ofstream and adds a member for boost::filesystem::path.
 *
 * This class allows writing to a file and keeps track of the associated file path.
 */
class OutputFileStream : public std::ofstream {
public:
    /**
     * @brief Default constructor.
     */
    OutputFileStream();

    /**
     * @brief Constructs a OutputFileStream object with the given file path.
     *
     * @param filePath The file path to associate with the ofstream.
     */
    explicit OutputFileStream(const boost::filesystem::path& filePath);

    /**
     * @brief Get the associated file path.
     *
     * @return The file path associated with the ofstream.
     */
    const boost::filesystem::path& getFilePath() const;

    /**
     * @brief Serialize the OutputFileStream object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    friend class boost::serialization::access;

    boost::filesystem::path filePath; ///< The associated file path.
};

namespace boost {
    namespace serialization {
        /**
         * @brief Serialize a Map pointer.
         *
         * This function handles the serialization and deserialization of a Map pointer.
         *
         * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
         * @param ar The archive to use.
         * @param mapPtr The Map pointer to serialize or deserialize.
         * @param version The version of the serialization format.
         */
        template<class Archive>
        void serialize(Archive &ar, OutputFileStream* &outputFileStreamPtr, const unsigned int version);
    }
}

/**
 * @brief A class that extends std::ifstream and adds a member for boost::filesystem::path.
 *
 * This class allows reading from a file and keeps track of the associated file path.
 */
class InputFileStream : public std::ifstream {
public:
    /**
     * @brief Default constructor.
     */
    InputFileStream();

    /**
     * @brief Constructs a InputFileStream object with the given file path.
     *
     * @param filePath The file path to associate with the ifstream.
     */
    explicit InputFileStream(const boost::filesystem::path& filePath);

    /**
     * @brief Get the associated file path.
     *
     * @return The file path associated with the ifstream.
     */
    const boost::filesystem::path& getFilePath() const;

    /**
     * @brief Serialize the InputFileStream object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    friend class boost::serialization::access;

    boost::filesystem::path filePath; ///< The associated file path.
};

#endif // FILE_STREAM_H