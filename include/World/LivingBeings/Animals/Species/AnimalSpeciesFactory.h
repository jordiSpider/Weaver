#ifndef ANIMAL_SPECIES_FACTORY_H_
#define ANIMAL_SPECIES_FACTORY_H_


#include <memory>
#include <string>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include "Exceptions/LineInfoException.h"
#include "World/LivingBeings/Animals/Species/AnimalSpecies.h"
#include "Misc/Macros.h"


class AnimalSpeciesFactory
{
protected:
	class Type {
	public:
		enum TypeValue {
			Default
		};

		static TypeValue stringToEnumValue(const std::string &str);

	private:
		static const std::unordered_map<std::string_view, const TypeValue> stringToEnum;
		static const std::string enumValues;

		static std::string_view printAvailableValues();
		static std::string_view to_string(const TypeValue& type);
		static const std::unordered_map<std::string_view, const TypeValue> generateMap();
		static constexpr size_t size();
		static const std::string generateAvailableValues();
	};

public:
	static std::unique_ptr<AnimalSpecies> createInstance(const nlohmann::json &animalSpeciesInfo, const bool initIndividualsPerDensities, WorldInterface* const worldInterface);
};

#endif // ANIMAL_SPECIES_FACTORY_H_
