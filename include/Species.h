/*
 * species.h
 *
 *  Created on: May 24, 2013
 *      Author: jrbcast
 */

#ifndef WEAVER_SPECIES_H_
#define WEAVER_SPECIES_H_

#include <string>

#include "string/trim.h"
#include "Types.h"
#include "LineInfoException.h"


class Species
{
private:
	static id_type speciesCounter;

	const id_type id;
	const std::string scientificName;
	float conversionToWetMass;

protected:
	explicit Species(const std::string& scientificName);
	virtual ~Species();

public:
	inline const id_type& getId() const { return id; };
	inline const std::string& getScientificName() const { return scientificName; };
	inline const float& getConversionToWetMass() const { return conversionToWetMass; }
	virtual double getCellMass() const { return 0; };
	inline void setConversionToWetMass(const float& conversionToWetMass) { this->conversionToWetMass = conversionToWetMass; }

	bool operator<(const Species& other) const;
	bool operator==(const Species& other) const ;

	virtual bool isMobile() const=0;
	virtual float getAttackProbability() const=0;
	virtual float getExposedAttackProbability() const=0;
};

#endif /* WEAVER_SPECIES_H_ */
