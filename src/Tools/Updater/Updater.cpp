
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
    for(Change* elem : changes)
    {
        delete elem;
    }
}

void Updater::addChanges(vector<Change*> &&newChanges)
{
    changes.insert(changes.end(), newChanges.begin(), newChanges.end());
}

const VersionNumber& Updater::getVersion() const
{
    return version;
}

void Updater::update(Config &config)
{
    for(Change* &change : changes)
    {
        change->applyChange(config);
    }

    config.refresh_version(getVersion());
}

bool Updater::operator<(const Updater& other) const
{
    return this->getVersion() < other.getVersion();
}
