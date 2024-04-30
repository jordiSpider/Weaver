#ifndef ANIMAL_SPECIES_FACTORY_H_
#define ANIMAL_SPECIES_FACTORY_H_


#include <memory>
#include <string>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Exceptions/LineInfoException.h"
#include "IBM/World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "Misc/Macros.h"


class AnimalSpeciesFactory
{
private:
	friend class boost::serialization::access;

protected:
	class Type {
	public:
		enum TypeValue {
			Default
		};

		static TypeValue stringToEnumValue(const std::string &str);

		/**
		 * @brief Serialize the Trait object.
		 * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
		 * @param ar The archive to use.
		 * @param version The version of the serialization format.
		 */
		template <class Archive>
		void serialize(Archive &ar, const unsigned int version);

	private:
		static const std::unordered_map<std::string_view, const TypeValue> stringToEnum;
		static const std::string enumValues;

		friend class boost::serialization::access;

		static std::string_view printAvailableValues();
		static std::string_view to_string(const TypeValue& type);
		static const std::unordered_map<std::string_view, const TypeValue> generateMap();
		static constexpr size_t size();
		static const std::string generateAvailableValues();
	};

public:
	static std::unique_ptr<AnimalSpecies> createInstance(const nlohmann::json &animalSpeciesInfo, const bool initIndividualsPerDensities, World* const world);

	/**
	 * @brief Serialize the Trait object.
	 * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
	 * @param ar The archive to use.
	 * @param version The version of the serialization format.
	 */
	template <class Archive>
	void serialize(Archive &ar, const unsigned int version);
};

#endif // ANIMAL_SPECIES_FACTORY_H_
