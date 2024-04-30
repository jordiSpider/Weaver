#ifndef WEAVER_FSTREAM_H
#define WEAVER_FSTREAM_H

#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace fs = boost::filesystem;

/**
 * @brief A class that extends std::ofstream and adds a member for boost::filesystem::path.
 *
 * This class allows writing to a file and keeps track of the associated file path.
 */
class WeaverOFStream : public std::ofstream {
public:
    /**
     * @brief Default constructor.
     */
    WeaverOFStream();

    /**
     * @brief Constructs a WeaverOFStream object with the given file path.
     *
     * @param filePath The file path to associate with the ofstream.
     */
    explicit WeaverOFStream(const fs::path& filePath);

    /**
     * @brief Get the associated file path.
     *
     * @return The file path associated with the ofstream.
     */
    const fs::path& getFilePath() const;

    /**
     * @brief Serialize the WeaverOFStream object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    friend class boost::serialization::access;

    fs::path filePath; ///< The associated file path.
};

/**
 * @brief A class that extends std::ifstream and adds a member for boost::filesystem::path.
 *
 * This class allows reading from a file and keeps track of the associated file path.
 */
class WeaverIFStream : public std::ifstream {
public:
    /**
     * @brief Default constructor.
     */
    WeaverIFStream();

    /**
     * @brief Constructs a WeaverIFStream object with the given file path.
     *
     * @param filePath The file path to associate with the ifstream.
     */
    explicit WeaverIFStream(const fs::path& filePath);

    /**
     * @brief Get the associated file path.
     *
     * @return The file path associated with the ifstream.
     */
    const fs::path& getFilePath() const;

    /**
     * @brief Serialize the WeaverIFStream object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    friend class boost::serialization::access;

    fs::path filePath; ///< The associated file path.
};

#endif // WEAVER_FSTREAM_H
