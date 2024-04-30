#ifndef TRAIT_H_
#define TRAIT_H_

#include <string>
#include <unordered_map>
#include <cfloat>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "IBM/Physics/Temperature.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Macros.h"


/**
 * @class Trait
 * @brief Represents a trait, which can be fixed or variable.
 */
class Trait {
private:
    friend class boost::serialization::access;

public:
    class ValueUpdateMethod {
    public:
        enum MethodType : unsigned int
		{
            Fixed,
            Variable
		};

        static const MethodType stringToEnumValue(const std::string &str);
        static constexpr size_t size();

    private:
        static const std::unordered_map<std::string_view, const MethodType> stringToEnum;
        static const std::string enumValues;

        static std::string_view to_string(const MethodType& type);
        static const std::unordered_map<std::string_view, const MethodType> generateMap();
        static const std::string generateAvailableValues();
        static std::string_view printAvailableValues();
    };

    /**
     * @enum Type
     * @brief Enumerates different trait types.
     */
	enum Type : unsigned int {
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

    /**
     * @brief Converts a string to the corresponding Type enum value.
     * @param str The input string.
     * @return The corresponding Type enum value.
     * @throw LineInfoException if the string is unknown.
     */
	inline static const Type stringToEnumValue(const std::string &str) { 
        try
        {
            return stringToEnum.at(str);
        }
        catch(const std::out_of_range& e) 
        {
            throwLineInfoException(fmt::format("Unknown trait type '{}'. Valid values are {}", str, printAvailableValues()));
        }
    }

    /**
     * @brief Prints available trait types.
     * @return A string containing available trait types.
     */
	inline static std::string_view printAvailableTraits() { return enumHeader; }
	
    /**
     * @brief Gets the number of trait types.
     * @return The number of trait types.
     */
    inline static constexpr size_t size() { return magic_enum::enum_count<Type>(); }
    
    /**
     * @brief Gets the type of the trait.
     * @return The type of the trait.
     */
    inline const Type& getType() const { return type; }

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

    /**
     * @brief Converts Type enum value to a string.
     * @param trait The Type enum value.
     * @return The string representation of the trait type.
     */
    inline static std::string_view to_string(const Type& traitName) { return magic_enum::enum_name(traitName); }

    const Temperature& getLowerThreshold() const;
    const Temperature& getUpperThreshold() const;

    virtual ValueUpdateMethod::MethodType getValueUpdateMethodType() const=0;

protected:
    const Type type;

    const Temperature lowerThreshold;
    const Temperature upperThreshold;

private:
	static const std::unordered_map<std::string_view, const Type> stringToEnum;
	static const std::string enumHeader;
    static const std::string enumValues;

    /**
     * @brief Generates a map of string to Type.
     * @return The generated map.
     */
	static const std::unordered_map<std::string_view, const Type> generateMap();
	
    /**
     * @brief Generates the header string for trait types.
     * @return The generated header string.
     */
    static const std::string generateHeader(); 
    
    /**
     * @brief Prints available trait values.
     * @return A string containing available trait values.
     */
    inline static std::string_view printAvailableValues() { return enumValues; }
    
    /**
     * @brief Generates a string of available trait values.
     * @return The generated string.
     */
    static const std::string generateAvailableValues();  

    /**
     * @brief Serialize the Trait object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};


#endif // TRAIT_H_
