#ifndef UPDATER_23_11_05_H_
#define UPDATER_23_11_05_H_


#include <nlohmann/json.hpp>

#include "Tools/Updater/Updater.h"
#include "Tools/Config/Config.h"


class Updater_23_11_05 : public Updater
{
public:
    Updater_23_11_05();
    virtual ~Updater_23_11_05();

    void update(Config &config);
};



#endif /* UPDATER_23_11_05_H_ */