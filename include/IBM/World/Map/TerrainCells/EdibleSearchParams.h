#ifndef EDIBLE_SEARCH_PARAMS_H_
#define EDIBLE_SEARCH_PARAMS_H_


#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/World/Map/TerrainCells/AnimalSearchParams.h"
#include "IBM/World/Map/TerrainCells/ResourceSearchParams.h"
#include "IBM/World/LivingBeings/LifeStage.h"
#include "Misc/EnumClass.h"


class WorldInterface;


class EdibleSearchParams
{
private:
    friend class boost::serialization::access;

protected:
    AnimalSearchParams animalSearchParams;
    ResourceSearchParams resourceSearchParams;

public:
    EdibleSearchParams();
    virtual ~EdibleSearchParams();

    void addAnimalSearchParams(
        const WorldInterface* const worldInterface,
        const std::vector<LifeStage> &searchableLifeStages = EnumClass<LifeStage>::getEnumValues(),
        const std::vector<AnimalSpecies::AnimalID> &searchableAnimalSpecies = {},
        const std::vector<Instar> &searchableInstars = {},
        const std::vector<AnimalSpecies::Gender> &searchableGenders = EnumClass<AnimalSpecies::Gender>::getEnumValues()
    );
    void addResourceSearchParams(const WorldInterface* const worldInterface, const std::vector<ResourceSpecies::ResourceID> &searchableResourceSpecies = {});

    const AnimalSearchParams& getAnimalSearchParams() const;
    const ResourceSearchParams& getResourceSearchParams() const;

    /**
     * @brief Serialize the EdibleSearchParams object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, EdibleSearchParams* &edibleSearchParamsPtr, const unsigned int version);
    }
}


#endif /* EDIBLE_SEARCH_PARAMS_H_ */
