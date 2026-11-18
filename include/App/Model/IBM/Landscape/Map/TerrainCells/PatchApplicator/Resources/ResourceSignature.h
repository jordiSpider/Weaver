
#ifndef RESOURCE_SIGNATURE_H_
#define RESOURCE_SIGNATURE_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/ResourceSpecies.h"


class ResourceSignature
{
private:
    static size_t BITS_RESOURCE_SP_ID;

    static size_t TOTAL_SIGNATURE_BITS;


    uint8_t value;


    static size_t bitsNeeded(size_t count) noexcept;

public:
    static size_t BITMASK_DYNAMIC_SIZE;


    static constexpr uint8_t packSignature(const ResourceSpecies::ResourceID& rs) noexcept
    {
        return static_cast<uint8_t>(rs);
    }

    static void configureBits(size_t numberOfResourceSpecies);


	ResourceSignature() noexcept : value(0) {}

    ResourceSignature(const ResourceSpecies::ResourceID& rs) noexcept
		: value(packSignature(rs)) {
	}


    inline uint8_t getValue() const noexcept {
        return value;
    }


    template <class Archive>
    void serialize(Archive& ar, const unsigned int version);
};

#endif /* RESOURCE_SIGNATURE_H_ */
