#include "Misc/FileStream.h" 


using namespace std;
namespace fs = boost::filesystem;



OutputFileStream::OutputFileStream()
    : ofstream()
{

}

// OutputFileStream Implementation
OutputFileStream::OutputFileStream(const fs::path& filePath)
    : ofstream(filePath.string()), filePath(filePath) 
{

}

const fs::path& OutputFileStream::getFilePath() const {
    return filePath;
}

template <class Archive>
void OutputFileStream::serialize(Archive &ar, const unsigned int version) {
    ar & filePath;
    
    // For loading
    if (Archive::is_loading::value) {
        open(filePath);
    }
}

namespace boost {
    namespace serialization {
        template <class Archive>
        void serialize(Archive &ar, OutputFileStream* &outputFileStreamPtr, const unsigned int version) {
            // For loading
            if (Archive::is_loading::value) {
                outputFileStreamPtr = new OutputFileStream();
            }

            outputFileStreamPtr->serialize(ar, version);
        }

        // Specialisation
		template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, OutputFileStream*&, const unsigned int);
		template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, OutputFileStream*&, const unsigned int);

		template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, OutputFileStream*&, const unsigned int);
		template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, OutputFileStream*&, const unsigned int);
    }
}



InputFileStream::InputFileStream()
    : ifstream()
{

}

// InputFileStream Implementation
InputFileStream::InputFileStream(const fs::path& filePath)
    : ifstream(filePath.string()), filePath(filePath) 
{

}

const fs::path& InputFileStream::getFilePath() const {
    return filePath;
}

template <class Archive>
void InputFileStream::serialize(Archive &ar, const unsigned int version) {
    // For loading
    if (Archive::is_loading::value) {
        fs::path newfilePath;
        ar & newfilePath;
        *this = InputFileStream(newfilePath);
    } else { // For saving
        ar & filePath;
    }
}