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

	double K_Value;
	unsigned int numberOfInstars;

public:
	static const id_type& getSpeciesCounter();

	inline const id_type& getId() const { return id; };
	inline const std::string& getScientificName() const { return scientificName; };
	inline const float& getConversionToWetMass() const { return conversionToWetMass; }
	virtual double getCellMass() const { return 0; };
	inline void setConversionToWetMass(const float& conversionToWetMass) { this->conversionToWetMass = conversionToWetMass; }
	const double& getK_Value() const;
	void setK_Value(const double &newK_Value);
	double convertToWetMass(const double &dryMass) const;
	double convertToDryMass(const double &wetMass) const;

	const unsigned int& getNumberOfInstars() const;
	void setNumberOfInstars(const unsigned int& numberOfInstars);

	bool operator<(const Species& other) const;
	bool operator==(const Species& other) const ;

	virtual bool isMobile() const=0;
	virtual float getAttackProbability() const=0;
	virtual float getExposedAttackProbability() const=0;
};

#endif /* WEAVER_SPECIES_H_ */
