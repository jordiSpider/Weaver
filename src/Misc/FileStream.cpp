#include "Misc/FileStream.h" 

// WeaverOFStream Implementation
WeaverOFStream::WeaverOFStream(const fs::path& filePath)
    : std::ofstream(filePath.string()), filePath(filePath) {}

const fs::path& WeaverOFStream::getFilePath() const {
    return filePath;
}

template <class Archive>
void WeaverOFStream::serialize(Archive &ar, const unsigned int version) {

}




// WeaverIFStream Implementation
WeaverIFStream::WeaverIFStream(const fs::path& filePath)
    : std::ifstream(filePath.string()), filePath(filePath) {}

const fs::path& WeaverIFStream::getFilePath() const {
    return filePath;
}

template <class Archive>
void WeaverIFStream::serialize(Archive &ar, const unsigned int version) {

}
