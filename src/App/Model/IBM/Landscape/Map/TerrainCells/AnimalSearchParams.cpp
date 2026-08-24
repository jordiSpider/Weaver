
#include "App/Model/IBM/Landscape/Map/TerrainCells/AnimalSearchParams.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Animal.h"


using namespace std;



AnimalSearchParams::AnimalSearchParams()
{
    
}

AnimalSearchParams::AnimalSearchParams(
    const vector<LifeStage> &newSearchableLifeStages,
    const vector<AnimalSpeciesID> &newSearchableAnimalSpecies,
    const vector<Instar> &newSearchableInstars,
    const vector<Gender> &newSearchableGenders)
    : AnimalSearchParams()
{
    addSearchParams(newSearchableLifeStages, newSearchableAnimalSpecies, newSearchableInstars, newSearchableGenders);
}

AnimalSearchParams::~AnimalSearchParams()
{
    
}

AnimalSearchParams::AnimalSearchParams(const AnimalSearchParams &other)
{
    validSignatures = other.validSignatures;
}

AnimalSearchParams& AnimalSearchParams::operator=(const AnimalSearchParams& other)
{
    if (this != &other) {
        validSignatures = other.validSignatures;
    }
    return *this;
}

void AnimalSearchParams::addSearchParams(
    const vector<LifeStage> &newSearchableLifeStages,
    const vector<AnimalSpeciesID> &newSearchableAnimalSpecies,
    const vector<Instar> &newSearchableInstars,
    const vector<Gender> &newSearchableGenders)
{
    for(const auto &lifeStage : newSearchableLifeStages) {
        for(const auto &animalSpeciesId : newSearchableAnimalSpecies) {
            for(const auto &instar : newSearchableInstars) {
                for(const auto &gender : newSearchableGenders) {
                    
                    uint32_t signature = packSignature(lifeStage, animalSpeciesId, instar, gender);
                    validSignatures.push_back(signature);
                    
                }
            }
        }
    }

    std::sort(validSignatures.begin(), validSignatures.end());
    validSignatures.erase(std::unique(validSignatures.begin(), validSignatures.end()), validSignatures.end());
}

bool AnimalSearchParams::matches(const Animal& animal) const noexcept
{
    uint32_t animalSignature = packSignature(
        animal.getLifeStage(),
        animal.getAnimalSpeciesId(),
        animal.getInstar(),
        animal.getGender()
    );

    return std::binary_search(validSignatures.begin(), validSignatures.end(), animalSignature);
}

void AnimalSearchParams::clear()
{
    validSignatures.clear();
}



BOOST_CLASS_EXPORT(AnimalSearchParams)

template <class Archive>
void AnimalSearchParams::serialize(Archive &ar, const unsigned int) {
    ar & validSignatures;
}

// Specialisation
template void AnimalSearchParams::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalSearchParams::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalSearchParams::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalSearchParams::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
