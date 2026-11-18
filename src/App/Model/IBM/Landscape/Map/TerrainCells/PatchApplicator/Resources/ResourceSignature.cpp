#include "App/Model/IBM/Landscape/Map/TerrainCells/PatchApplicator/Resources/ResourceSignature.h"

#include <climits>

#include "Misc/EnumClass.h"



using namespace std;


size_t ResourceSignature::BITS_RESOURCE_SP_ID = 0u;

size_t ResourceSignature::TOTAL_SIGNATURE_BITS = 0u;

size_t ResourceSignature::BITMASK_DYNAMIC_SIZE = 0u;


size_t ResourceSignature::bitsNeeded(size_t count) noexcept {
    if (count <= 1) return 1;
    size_t bits = 0;
    size_t target = count - 1; // Si tenemos 4 elementos (0 a 3), necesitamos representar el 3 (2 bits)
    while (target > 0) {
        bits++;
        target >>= 1;
    }
    return bits;
}

void ResourceSignature::configureBits(size_t numberOfResourceSpecies)
{
	BITS_RESOURCE_SP_ID = bitsNeeded(numberOfResourceSpecies);

    TOTAL_SIGNATURE_BITS = BITS_RESOURCE_SP_ID;

    if (TOTAL_SIGNATURE_BITS > sizeof(uint8_t) * CHAR_BIT) {
		throwLineInfoException("Total bits needed for AnimalSignature exceed 8 bits. Please check the configuration.");
    }

    BITMASK_DYNAMIC_SIZE = 1ULL << TOTAL_SIGNATURE_BITS;
}



template <class Archive>
void ResourceSignature::serialize(Archive &ar, const unsigned int) {
	ar & value;
}

// Specialisation
template void ResourceSignature::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ResourceSignature::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ResourceSignature::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ResourceSignature::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
