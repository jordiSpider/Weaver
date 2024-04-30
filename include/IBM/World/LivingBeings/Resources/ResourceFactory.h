#ifndef RESOURCE_FACTORY_H_
#define RESOURCE_FACTORY_H_


#include <memory>
#include <string>
#include <magic_enum.hpp>

#include "Exceptions/LineInfoException.h"
#include "IBM/World/LivingBeings/Resources/Resource.h"
#include "IBM/World/LivingBeings/Resources/ResourceInterface.h"
#include "Misc/Macros.h"


class ResourceFactory
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
	static std::unique_ptr<ResourceInterface> createInstance(ResourceSpecies* const mySpecies, 
		TerrainCellInterface* terrainCellInterface, double biomass, double resourceMaximumCapacity, 
		double massRatio, bool patchSpread
	);
};

#endif // RESOURCE_FACTORY_H_
