#ifndef UPDATER_H_
#define UPDATER_H_


#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

#include "Tools/Updater/Change.h"
#include "Tools/Config/VersionNumber.h"


class Updater
{
private:
    VersionNumber version;
    std::vector<Change*> changes;

protected:
    void addChanges(std::vector<Change*> &&newChanges);

public:
    Updater();
    Updater(std::string &&version);
    virtual ~Updater();

    const VersionNumber& getVersion() const;

    void update(Config &config);

    bool operator<(const Updater& other) const;
};



#endif /* UPDATER_H_ */