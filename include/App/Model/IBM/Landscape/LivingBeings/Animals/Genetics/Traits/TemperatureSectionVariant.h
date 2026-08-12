#ifndef TEMPERATURE_SECTION_VARIANT_H_
#define TEMPERATURE_SECTION_VARIANT_H_

#include <variant>

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/PawarIndividualTraitTemperatureSection.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Genetics/Traits/TempSizeRuleIndividualTraitTemperatureSection.h"
#include "Exceptions/LineInfoException.h"

using TemperatureSectionVariant = std::variant<
    std::monostate, // Para casos donde no hay dependencia de temperatura
    PawarIndividualTraitTemperatureSection,
    TempSizeRuleIndividualTraitTemperatureSection
>;

// Manual (de)serialization instead of <boost/serialization/std_variant.hpp>: that generic header
// declares boost::serialization::mp as a plain namespace, which clashes with the
// "namespace mp = boost::multiprecision;" alias used by Boost.Multiprecision's own
// serialization support (both are pulled in transitively via PreciseDouble).
namespace boost { namespace serialization {

template<class Archive>
void save(Archive& ar, const TemperatureSectionVariant& v, const unsigned int)
{
    std::size_t which = v.index();
    ar << BOOST_SERIALIZATION_NVP(which);

    std::visit([&ar](auto&& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (!std::is_same_v<T, std::monostate>) {
            ar << boost::serialization::make_nvp("value", value);
        }
    }, v);
}

template<class Archive>
void load(Archive& ar, TemperatureSectionVariant& v, const unsigned int)
{
    std::size_t which;
    ar >> BOOST_SERIALIZATION_NVP(which);

    switch(which)
    {
        case 0:
            v = std::monostate{};
            break;
        case 1:
        {
            PawarIndividualTraitTemperatureSection value;
            ar >> boost::serialization::make_nvp("value", value);
            v = std::move(value);
            break;
        }
        case 2:
        {
            TempSizeRuleIndividualTraitTemperatureSection value;
            ar >> boost::serialization::make_nvp("value", value);
            v = std::move(value);
            break;
        }
        default:
            throwLineInfoException("Unknown TemperatureSectionVariant alternative index during deserialization.");
    }
}

template<class Archive>
void serialize(Archive& ar, TemperatureSectionVariant& v, const unsigned int version)
{
    split_free(ar, v, version);
}

} } // namespace boost::serialization

#endif // TEMPERATURE_SECTION_VARIANT_H_