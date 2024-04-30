#ifndef LIFE_STAGE_H_
#define LIFE_STAGE_H_


#include <ostream>
#include <vector>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>


enum class LifeStage : unsigned int
{
    UNBORN, 
    ACTIVE, 
    STARVED, 
    PREDATED, 
    REPRODUCING, 
    PUPA, 
    SATIATED, 
    HANDLING, 
    DIAPAUSE, 
    BACKGROUND, 
    SENESCED, 
    SHOCKED
};

std::ostream& operator<<(std::ostream& os, const LifeStage lifeStage);


#endif /* LIFE_STAGE_H_ */
