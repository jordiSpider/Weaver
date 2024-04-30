#ifndef FIXED_TRAIT_H_
#define FIXED_TRAIT_H_


#include <string>
#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "IBM/World/LivingBeings/Animals/Genetics/Traits/Trait.h"


/**
 * @class FixedTrait
 * @brief Represents a fixed trait with a specific value.
 */
class FixedTrait: public Trait {
private:
    double value;

    friend class boost::serialization::access;

public:
    /**
     * @brief Constructor for FixedTrait.
     * @param typeStr The string representation of the trait type.
     * @param traitInfo Trait info.
     */
    FixedTrait(const std::string& typeStr, const nlohmann::json& traitInfo);
    
    /**
     * @brief Virtual destructor for FixedTrait.
     */
    virtual ~FixedTrait() {};

    /**
     * @brief Gets the value of the fixed trait.
     * @return The value of the fixed trait.
     */
    const double& getValue() const;

    /**
     * @brief Sets the value of the fixed trait.
     * @param value The new value of the fixed trait.
     */
    void setValue(const double& value);

    ValueUpdateMethod::MethodType getValueUpdateMethodType() const;

    /**
     * @brief Serialize the FixedTrait object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif // FIXED_TRAIT_H_
