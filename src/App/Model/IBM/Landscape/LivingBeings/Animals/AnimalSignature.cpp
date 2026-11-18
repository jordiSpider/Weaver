#include "App/Model/IBM/Landscape/LivingBeings/Animals/AnimalSignature.h"

#include <climits>

#include "Misc/EnumClass.h"



using namespace std;


size_t AnimalSignature::BITS_GENDER = 0u;
size_t AnimalSignature::BITS_INSTAR = 0u;
size_t AnimalSignature::BITS_ANIMAL_SP_ID = 0u;
size_t AnimalSignature::BITS_LIFE_STAGE = 0u;

size_t AnimalSignature::SHIFT_GENDER = 0u;
size_t AnimalSignature::SHIFT_INSTAR = 0u;
size_t AnimalSignature::SHIFT_ANIMAL_SP_ID = 0u;
size_t AnimalSignature::SHIFT_LIFE_STAGE = 0u;

size_t AnimalSignature::TOTAL_SIGNATURE_BITS = 0u;

size_t AnimalSignature::BITMASK_DYNAMIC_SIZE = 0u;


size_t AnimalSignature::bitsNeeded(size_t count) noexcept {
    if (count <= 1) return 1;
    size_t bits = 0;
    size_t target = count - 1; // Si tenemos 4 elementos (0 a 3), necesitamos representar el 3 (2 bits)
    while (target > 0) {
        bits++;
        target >>= 1;
    }
    return bits;
}

void AnimalSignature::configureBits(size_t maxNumberOfInstar, size_t numberOfAnimalSpecies)
{
	BITS_GENDER = bitsNeeded(EnumClass<Gender>::size());
    BITS_INSTAR = bitsNeeded(maxNumberOfInstar);
	BITS_ANIMAL_SP_ID = bitsNeeded(numberOfAnimalSpecies);
	BITS_LIFE_STAGE = bitsNeeded(EnumClass<LifeStage>::size());

    TOTAL_SIGNATURE_BITS = BITS_GENDER + BITS_INSTAR + BITS_ANIMAL_SP_ID + BITS_LIFE_STAGE;

    if (TOTAL_SIGNATURE_BITS > sizeof(uint32_t) * CHAR_BIT) {
		throwLineInfoException("Total bits needed for AnimalSignature exceed 32 bits. Please check the configuration.");
    }

    SHIFT_GENDER = 0;
    SHIFT_INSTAR = SHIFT_GENDER + BITS_GENDER;
    SHIFT_ANIMAL_SP_ID = SHIFT_INSTAR + BITS_INSTAR;
    SHIFT_LIFE_STAGE = SHIFT_ANIMAL_SP_ID + BITS_ANIMAL_SP_ID;

    BITMASK_DYNAMIC_SIZE = 1ULL << TOTAL_SIGNATURE_BITS;
}



template <class Archive>
void AnimalSignature::serialize(Archive &ar, const unsigned int) {
	ar & value;
}

// Specialisation
template void AnimalSignature::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void AnimalSignature::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void AnimalSignature::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void AnimalSignature::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
