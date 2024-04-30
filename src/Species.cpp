/*
 * species.cpp
 *
 *  Created on: May 24, 2013
 *      Author: jrbcast
 */

#include "Species.h"
#include "LineInfoException.h"

size_t TraitConverter::enumSize = 0;

string TraitConverter::enumHeader = "";

unordered_map<string, Trait> TraitConverter::stringToEnum = {
    {"energy_tank", energy_tank}, //M1_TRAIT1: Energy tank at birth and after molting
	{"growth", growth}, //M1_TRAIT2: Growth
	{"pheno", pheno}, //M1_TRAIT3: Phenology

	{"factorEggMass", factorEggMass}, //M2_TRAIT1: Body size at birth
	{"assim", assim}, //M2_TRAIT2: Assimilation
	{"voracity", voracity}, //M2_TRAIT3: Voracity

	{"speed", speed}, //M3_TRAIT1: Speed
	{"search_area", search_area}, //M3_TRAIT2: Search area
	{"met_rate", met_rate}, //M3_TRAIT3: Metabolic rate

	{"actE_vor", actE_vor}, //M4_TRAIT1: Q10 on voracity
	{"actE_speed", actE_speed}, //M4_TRAIT2: Q10 on speed
	{"actE_search", actE_search}, //M4_TRAIT3: Q10 on search area

	{"dummy1", dummy1},
	{"dummy2", dummy2},
	{"dummy3", dummy3},

	{"dummy4", dummy4},
	{"dummy5", dummy5},
	{"dummy6", dummy6},

	{"dummy7", dummy7},
	{"dummy8", dummy8},
	{"dummy9", dummy9},

	{"dummy10", dummy10},
	{"dummy11", dummy11},
	{"dummy12", dummy12},

	{"actE_met", actE_met} //M5_SINGLETRAIT: Activation energy for metabolic rate
};

size_t TraitConverter::size()
{
	if(enumSize == 0) {
		while((Trait)enumSize != end_trait)
		{
			enumSize += 1;
		}
	}

	return enumSize;
}

string& TraitConverter::printAvailableTraits()
{
	if(enumHeader.empty()) {
		for(size_t i = 0; i < size(); ++i)
		{
			bool found = false;
			string traitName;
			for(auto it = stringToEnum.cbegin(); it != stringToEnum.cend() && !found; it++)
			{
				if(it->second == (Trait)i)
				{
					found = true;
					traitName = it->first;
				}
			}

			if(!found) {
				throwLineInfoException("Enumerated " + to_string(i) + " has no string defined as name");
			}

			if(enumHeader.empty())
			{
				enumHeader = traitName;
			}
			else
			{
				enumHeader += "\t" + traitName;
			}
		}
	}

	return enumHeader;
}

unsigned int Species::speciesCounter = 0;

Species::Species(const string& scientificName)
{
	// Make sure there are not whitespaces around
	this->scientificName = scientificName;
	trim(this->scientificName);

	// The id is used to speed-up comparison processes instead of using strings which
	// take way more time to be compared
	id = speciesCounter++;
}

Species::~Species()
{
}

void Species::modifyScientificName(string newScientificName)
{
	scientificName = newScientificName;
	trim(scientificName);
}

bool Species::operator<(const Species& other) const
{
	return this->id < other.id;
}

