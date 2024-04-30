#ifndef OBBSTACLE_H_
#define OBSTAACLE_H_

#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

/**
 * @class Obstacle
 * @brief Represents an obstacle.
 *
 * The Obstacle class provides a basic representation of an obstacle in our simulation worlds.
 * It influences how animals move.
 */
class Obstacle
{
private:
    friend class boost::serialization::access;

public:
    // Serialization functions
    /**
     * @brief Serialize the Obstacle object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif /* OBSTACLE_H_ */