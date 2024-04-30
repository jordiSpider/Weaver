/*
 * Edible.h
 *
 *  Created on: Apr 6, 2017
 *      Author: gabi
 */

#ifndef EDIBLE_H_
#define EDIBLE_H_

#include <ostream>
#include <string>
#include <magic_enum.hpp>
#include <list>
#include <vector>

#include "Misc/Types.h"
#include "World/LivingBeings/Species.h"
#include "World/LivingBeings/EdibleInterface.h"
#include "Exceptions/LineInfoException.h"



class TerrainCellInterface;


class Edible : public EdibleInterface {
private:
	static id_type edibleId;

	id_type id;
	std::string idStr;

protected:
	Species* const mySpecies;

	bool temporary;
	Instar instar;

	TerrainCellInterface* terrainCellInterface;

	Edible(Species* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar, const bool temporary);
	~Edible();

public:
	inline static void resetIds(id_type newValue) { edibleId = newValue; }

	// Getters
	inline const id_type getId() const { return id; }
	inline std::string_view getIdStr() const { return idStr; }
	Species* const getSpecies() const;
	const Instar& getInstar() const;

	// Setters
	inline void doDefinitive() { 
		id = Edible::edibleId++;
		generateIdStr();
		temporary = false;
	};
	inline void generateIdStr() { idStr = std::string(MAX_NUM_DIGITS_ID - std::to_string(id).length(), '0') + std::to_string(id); }
	virtual void setInstar(const Instar& newInstar);
	
	void setTerrainCellInterface(TerrainCellInterface* newTerrainCellInterface);

	const TerrainCellInterface* const getTerrainCellInterface() const;
	TerrainCellInterface* const getMutableTerrainCellInterface() const;
};


std::ostream& operator<<(std::ostream& os, const Edible& edible);

#endif /* EDIBLE_H_ */
