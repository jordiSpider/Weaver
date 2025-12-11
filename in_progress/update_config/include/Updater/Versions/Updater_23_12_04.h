#ifndef UPDATER_23_12_04_H_
#define UPDATER_23_12_04_H_


#include <nlohmann/json.hpp>
#include <tuple>

#include "Tools/Updater/Updater.h"
#include "Tools/Config/Config.h"
#include "Exceptions/LineInfoException.h"


class Updater_23_12_04 : public Updater
{
public:
    Updater_23_12_04();
    virtual ~Updater_23_12_04();

    void update(Config &config);
};



#endif /* UPDATER_23_12_04_H_ */