
#ifndef ANIMAL_SIGNATURE_H_
#define ANIMAL_SIGNATURE_H_


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Gender.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/AnimalSpeciesID.h"
#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/Instar.h"


class AnimalSignature
{
private:
    static size_t BITS_GENDER;
    static size_t BITS_INSTAR;
    static size_t BITS_ANIMAL_SP_ID;
    static size_t BITS_LIFE_STAGE;

    static size_t SHIFT_GENDER;
    static size_t SHIFT_INSTAR;
    static size_t SHIFT_ANIMAL_SP_ID;
    static size_t SHIFT_LIFE_STAGE;

    static size_t TOTAL_SIGNATURE_BITS;


    uint32_t value;


    static size_t bitsNeeded(size_t count) noexcept;

public:
    static size_t BITMASK_DYNAMIC_SIZE;


    static constexpr uint32_t packSignature(const LifeStage& ls, const AnimalSpeciesID& as, const Instar& in, const Gender& g) noexcept
    {
        return (static_cast<uint32_t>(ls) << SHIFT_LIFE_STAGE) |
            (static_cast<uint32_t>(as) << SHIFT_ANIMAL_SP_ID) |
            (static_cast<uint32_t>(in.getValue()) << SHIFT_INSTAR) |
            (static_cast<uint32_t>(g) << SHIFT_GENDER);
    }

    static void configureBits(size_t maxNumberOfInstar, size_t numberOfAnimalSpecies);


	AnimalSignature() noexcept : value(0) {}

	AnimalSignature(const LifeStage& ls, const AnimalSpeciesID& as, const Instar& in, const Gender& g) noexcept
		: value(packSignature(ls, as, in, g)) {
	}


    inline uint32_t getValue() const noexcept {
        return value;
    }


    template <class Archive>
    void serialize(Archive& ar, const unsigned int version);
};

#endif /* ANIMAL_SIGNATURE_H_ */
