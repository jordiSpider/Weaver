
#include "Tools/Updater/Updater.h"


using namespace std;
using json = nlohmann::json;


Updater::Updater()
{

}

Updater::Updater(string &&version)
    : version(version)
{
    
}

Updater::~Updater()
{
    
}

const VersionNumber& Updater::getVersion() const
{
    return version;
}

void Updater::update(Config &config)
{
    config.refresh_version(getVersion());
}

bool Updater::operator<(const Updater& other) const
{
    return this->getVersion() < other.getVersion();
}
