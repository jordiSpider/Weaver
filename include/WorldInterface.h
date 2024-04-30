#ifndef WORLD_INTERFACE_HPP_
#define WORLD_INTERFACE_HPP_


#include <string>
#include <unordered_map>
#include "LineInfoException.h"

#include <magic_enum.hpp>

#define FMT_HEADER_ONLY
#include <fmt/core.h>


class TerrainCell;
class TerrainCellInterface;

class WorldInterface
{
public:
	class WorldType {
	public:
		enum WorldTypeValue {
			Old,
			Matrix3D
		};

		inline static WorldTypeValue stringToEnumValue(const std::string &str) { 
			try
			{
				return stringToEnum.at(str);
			}
			catch(const std::out_of_range& e) 
			{
				throwLineInfoException(fmt::format("Unknown world type '{}'. Valid values are {}", str, printAvailableValues()));
			}
		}

	private:
		static const std::unordered_map<std::string_view, const WorldTypeValue> stringToEnum;
		static const std::string enumValues;

		inline static std::string_view printAvailableValues() { return enumValues; }
		inline static std::string_view to_string(const WorldTypeValue& worldType) { return magic_enum::enum_name(worldType); }
		static const std::unordered_map<std::string_view, const WorldTypeValue> generateMap();
		inline static constexpr size_t size() { return magic_enum::enum_count<WorldTypeValue>(); }
		static const std::string generateAvailableValues();
	};

	virtual TerrainCell* getCell(unsigned int z, unsigned int y, unsigned int x)=0;
	virtual TerrainCellInterface* getCellByBearing(TerrainCellInterface* position, const TerrainCellInterface* const bestCell)=0;
};

typedef WorldInterface::WorldType WorldType;


#endif // WORLD_INTERFACE_HPP_
