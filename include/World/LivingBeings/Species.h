/*
 * species.h
 *
 *  Created on: May 24, 2013
 *      Author: jrbcast
 */

#ifndef WEAVER_SPECIES_H_
#define WEAVER_SPECIES_H_

#include <string>
#include <cfloat>
#include <magic_enum.hpp>
#include <cmath>
#include <iostream>

#include "Misc/string/trim.h"
#include "World/LivingBeings/Instar.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Types.h"




class ResourceSpecies;
class AnimalSpecies;


class Species
{
private:
	static id_type speciesCounter;

	const id_type id;
	const std::string scientificName;
	float conversionToWetMass;

protected:
	explicit Species(const std::string& scientificName, const unsigned int numberOfInstars);
	virtual ~Species();

	InstarVector<bool> calculatedK_DensityPerInstar;
	InstarVector<double> K_DensityPerInstar;
	const unsigned int numberOfInstars;
	const std::vector<Instar> instarsRange;

	const double& getInstarK_Density(const Instar &instar) const;
	std::vector<Instar> calculateInstarsRange();

	const bool isCalculatedInstarK_Density(const Instar &instar) const;

public:
	class Type {
    public:
        enum TypeValue : unsigned int
		{
            Resource,
			Animal
		};

        static const TypeValue stringToEnumValue(const std::string &str);
        static constexpr size_t size(){ 
			return magic_enum::enum_count<TypeValue>(); 
		}

    private:
        static const std::unordered_map<std::string_view, const TypeValue> stringToEnum;
        static const std::string enumValues;

        static std::string_view to_string(const TypeValue& type);
        static const std::unordered_map<std::string_view, const TypeValue> generateMap();
        static const std::string generateAvailableValues();
        static std::string_view printAvailableValues();
    };


	static const id_type& getSpeciesCounter();

	inline const id_type& getId() const { return id; };
	inline const std::string& getScientificName() const { return scientificName; };
	inline const float& getConversionToWetMass() const { return conversionToWetMass; }
	virtual double getCellMass() const { return 0; };
	inline void setConversionToWetMass(const float& conversionToWetMass) { this->conversionToWetMass = conversionToWetMass; }
	const double getInstarK_Value(const Instar &instar, const double &size) const;
	void setInstarK_Density(const Instar &instar, const double &newK_Density);
	double convertToWetMass(const double &dryMass) const;
	double convertToDryMass(const double &wetMass) const;

	const unsigned int& getNumberOfInstars() const;
	const std::vector<Instar>& getInstarsRange() const;

	bool operator<(const Species& other) const;
	bool operator==(const Species& other) const;

	virtual bool isMobile() const=0;
	virtual float getAttackProbability() const=0;
	virtual float getExposedAttackProbability() const=0;

	virtual void generateInstarInvolvedResourceSpecies(
		const Instar &instar, std::vector<ResourceSpecies*> &instarInvolvedResourceSpecies, 
		std::vector<InstarVector<bool>> &alreadyCheckedSpecies
	)=0;

	virtual double calculateInstarK_Density(const Instar &instar, std::vector<InstarVector<bool>> &checkedAnimalSpecies, 
		std::vector<InstarVector<bool>> &cannibalismAnimalSpecies
	)=0;
};

#endif /* WEAVER_SPECIES_H_ */
