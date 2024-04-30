#ifndef RESOURCE_SPECIES_FACTORY_H_
#define RESOURCE_SPECIES_FACTORY_H_


#include <memory>
#include <string>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include "Exceptions/LineInfoException.h"
#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "IBM/World/LivingBeings/Resources/Species/UpdateMethodType/BasalResourceSpecies.h"
#include "IBM/World/LivingBeings/Resources/Species/UpdateMethodType/NoBasalResourceSpecies.h"
#include "Misc/Macros.h"


class ResourceSpeciesFactory
{
protected:
	class Type {
	public:
		enum TypeValue {
			BasalResource, 
			NoBasalResource
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
	static std::unique_ptr<ResourceSpecies> createInstance(const nlohmann::json &resourceSpeciesInfo);
};

#endif // RESOURCE_SPECIES_FACTORY_H_
