#ifndef MAP_H_
#define MAP_H_

#include <nlohmann/json.hpp>
#include <boost/geometry/algorithms/convert.hpp>
#include <fstream>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Misc/Types.h"
#include "IBM/World/Map/MapInterface.h"
#include "IBM/World/Map/Patches/Moisture/MoistureBasePatch.h"
#include "IBM/World/LivingBeings/LifeStage.h"
#include "Misc/CustomIndexedVector.h"


class WorldInterface;
class AnimalSearchParams;


class Map : public MapInterface {
private:
    friend class boost::serialization::access;

protected:
    double minCellSize;
    unsigned int numberOfCellsPerAxis;
    WorldInterface* const worldInterface;
    CustomIndexedVector<LifeStage, AnimalSearchParams> lifeStageSearchParams;

    bool isSpeciesInhabitableTerrainCell(
        const AnimalSpecies &animalSpecies, const TerrainCellInterface* const potentialInhabitableTerrainCell, 
        const Instar &instar
    );
    virtual void obtainMapCellsInfo(std::vector<std::pair<std::vector<double>, std::vector<unsigned int>>> &mapCellsInfo)=0;

public:
    Map(const nlohmann::json &mapConfig, WorldInterface* const worldInterface);
    Map(WorldInterface* const worldInterface);
    virtual ~Map();

    const double& getMinCellSize() const;
    const unsigned int& getNumberOfCellsPerAxis() const;
    const WorldInterface* const getWorldInterface() const;
    void printCellAlongCells(std::ofstream &file);
    WorldInterface* const getMutableWorldInterface() const;
    void obtainLifeStageSearchParams();
    const AnimalSearchParams& getLifeStageSearchParams(const LifeStage &lifeStage) const;

    /**
     * @brief Serialize the Map object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};


#endif /* MAP_H_ */