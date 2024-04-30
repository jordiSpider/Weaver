#ifndef TRAIT_H_
#define TRAIT_H_

#include <string>
#include <unordered_map>
#include <cfloat>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/Physics/Temperature.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"
#include "Misc/EnumClass.h"


/**
 * @class Trait
 * @brief Represents a trait, which can be fixed or variable.
 */
class Trait {
private:
    friend class boost::serialization::access;

public:
    enum class ValueUpdateMethod : unsigned int
    {
        Fixed,
        Variable
    };

    /**
     * @enum Type
     * @brief Enumerates different trait types.
     */
	enum class Type : unsigned int {
		energy_tank,
		growth,
		pheno, 
		factorEggMass,
		assim,
		voracity,
		speed,
		search_area,
		met_rate,
		actE_vor,
		actE_speed,
		actE_search,
		shock_resistance,
		actE_met,
        memoryDepth,
        perception_area,
        interaction_area
	};


    static std::unique_ptr<Trait> createInstance(const std::string& traitName, const nlohmann::json& traitInfo, const std::vector<std::string>& variableTraitsOrder);
    static std::unique_ptr<Trait> createInstance(const Trait::ValueUpdateMethod &traitValueUpdateMethod);

    
    /**
     * @brief Gets the type of the trait.
     * @return The type of the trait.
     */
    inline const Type& getType() const { return type; }

    Trait();

    /**
     * @brief Constructor for Trait.
     * @param typeStr The string representation of the trait type.
     * @param traitInfo Trait info.
     */
    Trait(const std::string& typeStr, const nlohmann::json& traitInfo);
    
    /**
     * @brief Virtual destructor for Trait.
     */
    virtual ~Trait() {};

    const Temperature& getLowerThreshold() const;
    const Temperature& getUpperThreshold() const;

    virtual ValueUpdateMethod getValueUpdateMethodType() const=0;

    /**
     * @brief Serialize the Trait object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

protected:
    Type type;

    Temperature lowerThreshold;
    Temperature upperThreshold;
};

namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, Trait* &traitPtr, const unsigned int version);
    }
}


#endif // TRAIT_H_
