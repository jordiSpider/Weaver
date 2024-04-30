#ifndef ANIMAL_SEARCH_PARAMS_H_
#define ANIMAL_SEARCH_PARAMS_H_


#include <vector>
#include <unordered_set>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Misc/Types.h"
#include "IBM/World/LivingBeings/Edible.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/LivingBeings/LifeStage.h"


class WorldInterface;


class AnimalSearchParams
{
private:
    friend class boost::serialization::access;

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

    /**
     * @brief Serialize the AnimalSearchParams object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(AnimalSearchParams)

#endif /* ANIMAL_SEARCH_PARAMS_H_ */
