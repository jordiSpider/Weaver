#ifndef ANIMAL_SEARCH_PARAMS_H_
#define ANIMAL_SEARCH_PARAMS_H_


#include <vector>
#include <unordered_set>

#include "Misc/Types.h"
#include "World/LivingBeings/Edible.h"
#include "World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "World/LivingBeings/LifeStage.h"


class WorldInterface;


class AnimalSearchParams
{
protected:
    std::unordered_set<LifeStage::LifeStageValue> searchableLifeStages;
    std::vector<std::unordered_set<id_type>> searchableAnimalSpecies;
    std::vector<std::vector<std::unordered_set<Instar>>> searchableInstars;
    std::vector<std::vector<InstarVector<std::unordered_set<AnimalSpecies::Gender::GenderValue>>>> searchableGenders;

    void initializedParams(const WorldInterface* const worldInterface);

public:
    AnimalSearchParams();
    AnimalSearchParams(
        const WorldInterface* const worldInterface,
        const std::vector<LifeStage::LifeStageValue> &newSearchableLifeStages,
        const std::vector<id_type> &newSearchableAnimalSpecies = {},
        const std::vector<Instar> &newSearchableInstars = {},
        const std::vector<AnimalSpecies::Gender::GenderValue> &newSearchableGenders = AnimalSpecies::Gender::getEnumValues()
    );
    virtual ~AnimalSearchParams();

    void addSearchParams(
        const WorldInterface* const worldInterface,
        const std::vector<LifeStage::LifeStageValue> &newSearchableLifeStages = LifeStage::getEnumValues(),
        const std::vector<id_type> &newSearchableAnimalSpecies = {},
        const std::vector<Instar> &newSearchableInstars = {},
        const std::vector<AnimalSpecies::Gender::GenderValue> &newSearchableGenders = AnimalSpecies::Gender::getEnumValues()
    );

    const std::unordered_set<LifeStage::LifeStageValue>& getSearchableLifeStages() const;
    const std::unordered_set<id_type>& getSearchableAnimalSpecies(
        const LifeStage::LifeStageValue &lifeStage
    ) const;
    const std::unordered_set<Instar>& getSearchableInstars(
        const LifeStage::LifeStageValue &lifeStage,
        const id_type &animalSpeciesId
    ) const;
    const std::unordered_set<AnimalSpecies::Gender::GenderValue>& getSearchableGenders(
        const LifeStage::LifeStageValue &lifeStage,
        const id_type &animalSpeciesId, const Instar &instar
    ) const;
};

#endif /* ANIMAL_SEARCH_PARAMS_H_ */
