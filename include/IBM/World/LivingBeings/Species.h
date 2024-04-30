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
#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <fstream>
#include <ostream>

#include "Misc/string/trim.h"
#include "IBM/World/LivingBeings/Instar.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Types.h"
#include "Misc/CustomIndexedVector.h"




class ResourceSpecies;
class AnimalSpecies;
class WorldInterface;


class Species
{
public:
	class ID
	{
	public:
		ID();
		ID(const id_type& value);

		const id_type& getValue() const;
		operator size_t() const;

		template <class Archive>
    	void serialize(Archive &ar, const unsigned int version);

	private:
		id_type value;
	};

	enum class Type : unsigned int
	{
		Resource,
		Animal
	};
	

	inline const ID& getId() const { return id; };
	inline const std::string& getScientificName() const { return scientificName; };
	inline const float& getConversionToWetMass() const { return conversionToWetMass; }
	virtual double getCellMass() const { return 0; };
	inline void setConversionToWetMass(const float& conversionToWetMass) { this->conversionToWetMass = conversionToWetMass; }
	const double getInstarK_Value(const Instar &instar, const double &size) const;
	void setInstarK_Density(const Instar &instar, const double &newK_Density);
	double convertToWetMass(const double &dryMass) const;
	double convertToDryMass(const double &wetMass) const;

	const WorldInterface* const getWorldInterface() const;
	WorldInterface* const getMutableWorldInterface();

	virtual bool isExtinguished() const { return extinguished; };
	virtual void setExtinguished(bool extinguished) { this->extinguished = extinguished; };

	const unsigned int& getNumberOfInstars() const;
	const std::vector<Instar>& getInstarsRange() const;

	bool operator<(const Species& other) const;
	bool operator==(const Species& other) const;

	virtual bool isMobile() const=0;
	virtual float getAttackProbability() const=0;
	virtual float getExposedAttackProbability() const=0;

	virtual double calculateInstarK_Density(const Instar &instar, std::vector<CustomIndexedVector<Instar, bool>> &checkedAnimalSpecies, 
		std::vector<CustomIndexedVector<Instar, bool>> &cannibalismAnimalSpecies
	)=0;

	/**
     * @brief Serialize the Species object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
	
private:
	ID id;
	std::string scientificName;
	float conversionToWetMass;

	bool extinguished;

	friend class boost::serialization::access;

protected:
	explicit Species(WorldInterface* const worldInterface);
	explicit Species(const Species::ID& speciesId, const std::string& scientificName, const unsigned int numberOfInstars, WorldInterface* const worldInterface);
	virtual ~Species();

	CustomIndexedVector<Instar, bool> calculatedK_DensityPerInstar;
	CustomIndexedVector<Instar, double> K_DensityPerInstar;
	unsigned int numberOfInstars;
	std::vector<Instar> instarsRange;
	WorldInterface* worldInterface;

	const double& getInstarK_Density(const Instar &instar) const;
	std::vector<Instar> calculateInstarsRange();

	const bool isCalculatedInstarK_Density(const Instar &instar) const;
};

#endif /* WEAVER_SPECIES_H_ */
