#ifndef EDIBLE_SEARCH_PARAMS_H_
#define EDIBLE_SEARCH_PARAMS_H_


#include "World/Map/TerrainCells/AnimalSearchParams.h"
#include "World/Map/TerrainCells/ResourceSearchParams.h"
#include "World/LivingBeings/LifeStage.h"


class WorldInterface;


class EdibleSearchParams
{
protected:
    AnimalSearchParams animalSearchParams;
    ResourceSearchParams resourceSearchParams;

public:
    EdibleSearchParams();
    virtual ~EdibleSearchParams();

    void addAnimalSearchParams(
        const WorldInterface* const worldInterface,
        const std::vector<LifeStage::LifeStageValue> &searchableLifeStages = LifeStage::getEnumValues(),
        const std::vector<id_type> &searchableAnimalSpecies = {},
        const std::vector<Instar> &searchableInstars = {},
        const std::vector<AnimalSpecies::Gender::GenderValue> &searchableGenders = AnimalSpecies::Gender::getEnumValues()
    );
    void addResourceSearchParams(const std::vector<id_type> &searchableResourceSpecies = {});

    const AnimalSearchParams& getAnimalSearchParams() const;
    const ResourceSearchParams& getResourceSearchParams() const;
};

#endif /* EDIBLE_SEARCH_PARAMS_H_ */
