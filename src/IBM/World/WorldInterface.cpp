#include "IBM/World/WorldInterface.h"

#include "IBM/World/ArthropodsWorld.h"
#include "IBM/World/DinosaursWorld.h"
#include "IBM/World/AquaticWorld.h"

using namespace std;

namespace fs = boost::filesystem;


unique_ptr<WorldInterface> WorldInterface::createInstance(json* jsonTree, json &worldConfig, fs::path outputFolder, fs::path configPath, int burnIn) {
    switch(EnumClass<Type>::stringToEnumValue(worldConfig["world"]["simulationType"])) {
        case Type::Arthropods: {
            return make_unique<ArthropodsWorld>(jsonTree, worldConfig, outputFolder, configPath, burnIn);
            break;
        }
        case Type::Dinosaurs: {
            return make_unique<DinosaursWorld>(jsonTree, worldConfig, outputFolder, configPath, burnIn);
            break;
        }
		case Type::Aquatic: {
            return make_unique<AquaticWorld>(jsonTree, worldConfig, outputFolder, configPath, burnIn);
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}

unique_ptr<WorldInterface> WorldInterface::createInstance(const WorldInterface::Type &worldType) {
    switch(worldType) {
        case Type::Arthropods: {
            return make_unique<ArthropodsWorld>();
            break;
        }
        case Type::Dinosaurs: {
            return make_unique<DinosaursWorld>();
            break;
        }
		case Type::Aquatic: {
            return make_unique<AquaticWorld>();
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}



WorldInterface::WorldInterface()
{

}


WorldInterface::~WorldInterface()
{

}

const bool WorldInterface::isDinosaurs() const
{
    return false;
}

template <class Archive>
void WorldInterface::serialize(Archive &ar, const unsigned int version) 
{
    
}


// Specialisation
template void WorldInterface::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void WorldInterface::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void WorldInterface::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void WorldInterface::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, WorldInterface* &worldInterfacePtr, const unsigned int version) {
            unsigned int checkpointVersion;
            WorldInterface::Type worldType;
            
            // For loading
            if(Archive::is_loading::value) 
            {
                ar & checkpointVersion;
                if(checkpointVersion != version)
                {
                    throwLineInfoException("Incorrect checkpoint version.");
                }

                ar & worldType;
                worldInterfacePtr = WorldInterface::createInstance(worldType).release();
            }
            else
            {
                checkpointVersion = version;
                ar & checkpointVersion;

                worldType = worldInterfacePtr->getWorldType();
                ar & worldType;
            }

            switch(worldType) {
                case WorldInterface::Type::Arthropods: {
                    static_cast<ArthropodsWorld*>(worldInterfacePtr)->serialize(ar, version);
                    break;
                }
                case WorldInterface::Type::Dinosaurs: {
                    static_cast<DinosaursWorld*>(worldInterfacePtr)->serialize(ar, version);
                    break;
                }
                case WorldInterface::Type::Aquatic: {
                    static_cast<AquaticWorld*>(worldInterfacePtr)->serialize(ar, version);
                    break;
                }
                default: {
                    throwLineInfoException("Default case");
                    break;
                }
            }
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, WorldInterface*&, const unsigned int);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, WorldInterface*&, const unsigned int);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, WorldInterface*&, const unsigned int);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, WorldInterface*&, const unsigned int);
    }
}