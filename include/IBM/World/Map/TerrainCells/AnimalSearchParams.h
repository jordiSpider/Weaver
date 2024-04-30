#ifndef ANIMAL_SEARCH_PARAMS_H_
#define ANIMAL_SEARCH_PARAMS_H_


#include <vector>
#include <unordered_set>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <fstream>
#include <ostream>

#include "Misc/Types.h"
#include "IBM/World/LivingBeings/Edible.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "IBM/World/LivingBeings/LifeStage.h"
#include "Misc/CustomIndexedVector.h"
#include "Misc/EnumClass.h"


class WorldInterface;


class AnimalSearchParams
{
private:
    friend class boost::serialization::access;

protected:
    std::unordered_set<LifeStage> searchableLifeStages;
    CustomIndexedVector<LifeStage, std::unordered_set<AnimalSpecies::AnimalID>> searchableAnimalSpecies;
    CustomIndexedVector<LifeStage, std::vector<std::unordered_set<Instar>>> searchableInstars;
    CustomIndexedVector<LifeStage, std::vector<CustomIndexedVector<Instar, std::unordered_set<AnimalSpecies::Gender>>>> searchableGenders;

    void initializedParams(const WorldInterface* const worldInterface);

public:
    AnimalSearchParams();
    AnimalSearchParams(
        const WorldInterface* const worldInterface,
        const std::vector<LifeStage> &newSearchableLifeStages,
        const std::vector<AnimalSpecies::AnimalID> &newSearchableAnimalSpecies = {},
        const std::vector<Instar> &newSearchableInstars = {},
        const std::vector<AnimalSpecies::Gender> &newSearchableGenders = EnumClass<AnimalSpecies::Gender>::getEnumValues()
    );
    virtual ~AnimalSearchParams();

    void addSearchParams(
        const WorldInterface* const worldInterface,
        const std::vector<LifeStage> &newSearchableLifeStages = EnumClass<LifeStage>::getEnumValues(),
        const std::vector<AnimalSpecies::AnimalID> &newSearchableAnimalSpecies = {},
        const std::vector<Instar> &newSearchableInstars = {},
        const std::vector<AnimalSpecies::Gender> &newSearchableGenders = EnumClass<AnimalSpecies::Gender>::getEnumValues()
    );

    const std::unordered_set<LifeStage>& getSearchableLifeStages() const;
    const std::unordered_set<AnimalSpecies::AnimalID>& getSearchableAnimalSpecies(
        const LifeStage &lifeStage
    ) const;
    const std::unordered_set<Instar>& getSearchableInstars(
        const LifeStage &lifeStage,
        const AnimalSpecies::AnimalID &animalSpeciesId
    ) const;
    const std::unordered_set<AnimalSpecies::Gender>& getSearchableGenders(
        const LifeStage &lifeStage,
        const AnimalSpecies::AnimalID &animalSpeciesId, const Instar &instar
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

namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, AnimalSearchParams* &animalSearchParamsPtr, const unsigned int version);
    }
}

#endif /* ANIMAL_SEARCH_PARAMS_H_ */
