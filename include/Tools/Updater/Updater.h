#ifndef UPDATER_H_
#define UPDATER_H_


#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

#include "Tools/Config/Config.h"
#include "Tools/Config/VersionNumber.h"


class Updater
{
private:
    VersionNumber version;

public:
    Updater();
    Updater(std::string &&version);
    virtual ~Updater();

    const VersionNumber& getVersion() const;

    virtual void update(Config &config);

    bool operator<(const Updater& other) const;
};



#endif /* UPDATER_H_ */