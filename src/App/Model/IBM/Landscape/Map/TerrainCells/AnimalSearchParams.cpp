
#include "App/Model/IBM/Landscape/Map/TerrainCells/AnimalSearchParams.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Animal.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalSignature.h"
#include "Exceptions/LineInfoException.h"


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
    validSignaturesBitmask = other.validSignaturesBitmask;
}

AnimalSearchParams& AnimalSearchParams::operator=(const AnimalSearchParams& other)
{
    if (this != &other) {
        validSignaturesBitmask = other.validSignaturesBitmask;
    }
    return *this;
}

void AnimalSearchParams::init()
{
    if (AnimalSignature::BITMASK_DYNAMIC_SIZE == 0) {
        throwLineInfoException("AnimalSignature::BITMASK_DYNAMIC_SIZE is not configured. Please call AnimalSignature::configureBits() before initialize AnimalSearchParams.");
    }

    if (validSignaturesBitmask.size() < AnimalSignature::BITMASK_DYNAMIC_SIZE) {
        validSignaturesBitmask.resize(AnimalSignature::BITMASK_DYNAMIC_SIZE, false);
    }
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
                    
                    uint32_t signature = AnimalSignature::packSignature(lifeStage, animalSpeciesId, instar, gender);
                    validSignaturesBitmask[signature] = true;
                    
                }
            }
        }
    }
}

bool AnimalSearchParams::matches(const Animal& animal) const noexcept
{
    return validSignaturesBitmask[animal.getSignature().getValue()];
}
