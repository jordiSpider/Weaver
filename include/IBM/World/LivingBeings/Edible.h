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
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>

#include "Misc/Types.h"
#include "IBM/World/LivingBeings/Species.h"
#include "IBM/World/LivingBeings/EdibleInterface.h"
#include "Exceptions/LineInfoException.h"



class TerrainCellInterface;


class Edible : public EdibleInterface {
private:
	id_type id;
	std::string idStr;

	friend class boost::serialization::access;

protected:
	Species* mySpecies;

	bool temporary;
	Instar instar;

	TerrainCellInterface* terrainCellInterface;

	Edible(TerrainCellInterface* terrainCellInterface);
	Edible(Species* const mySpecies, const Instar &instar, const bool temporary);
	Edible(Species* const mySpecies, TerrainCellInterface* terrainCellInterface, const Instar &instar, const bool temporary);
	~Edible();

public:
	// Getters
	const id_type getId() const { return id; }
	std::string_view getIdStr() const { return idStr; }
	Species* const getSpecies() const;
	const Instar& getInstar() const;

	// Setters
	void doDefinitive();
	inline void generateIdStr() { idStr = std::string(MAX_NUM_DIGITS_ID - std::to_string(id).length(), '0') + std::to_string(id); }
	virtual void setInstar(const Instar& newInstar);
	
	void setTerrainCellInterface(TerrainCellInterface* newTerrainCellInterface);

	const TerrainCellInterface* const getTerrainCellInterface() const;
	TerrainCellInterface* const getMutableTerrainCellInterface() const;

	/**
     * @brief Serialize the Edible object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};


std::ostream& operator<<(std::ostream& os, const Edible& edible);

#endif /* EDIBLE_H_ */
