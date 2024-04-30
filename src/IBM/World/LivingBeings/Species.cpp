/*
 * species.cpp
 *
 *  Created on: May 24, 2013
 *      Author: jrbcast
 */

#include "IBM/World/LivingBeings/Species.h"

#include "IBM/World/WorldInterface.h"

using namespace std;



Species::ID::ID()
{

}

Species::ID::ID(const id_type& value)
	: value(value)
{

}

const id_type& Species::ID::getValue() const
{
	return value;
}

Species::ID::operator size_t() const {
	return static_cast<size_t>(value);
}

template <class Archive>
void Species::ID::serialize(Archive &ar, const unsigned int version) {
	ar & value;
}



Species::Species(WorldInterface* const worldInterface)
	: worldInterface(worldInterface)
{
	
}

Species::Species(const Species::ID& speciesId, const string& scientificName, const unsigned int numberOfInstars, WorldInterface* const worldInterface)
	: id(speciesId), scientificName(trim(scientificName)), extinguished(false), 
	  calculatedK_DensityPerInstar(numberOfInstars, false), K_DensityPerInstar(numberOfInstars, 0.0),
	  numberOfInstars(numberOfInstars), instarsRange(calculateInstarsRange()), worldInterface(worldInterface)
{
	cout << "numberOfInstars: " << numberOfInstars << endl;
}

Species::~Species()
{
}

bool Species::operator<(const Species& other) const
{
	return this->id < other.id;
}

bool Species::operator==(const Species& other) const {
	return (this->scientificName == other.scientificName);
}

vector<Instar> Species::calculateInstarsRange()
{
	#ifdef DEBUG
	if(getNumberOfInstars() == 0)
	{
		throwLineInfoException("The number of instars must be greater than zero.");
	}
	#endif

	vector<Instar> calculatedRange;

	for(unsigned int i = 1; i <= getNumberOfInstars(); i++)
	{
		calculatedRange.push_back(Instar(i));
	}

	return calculatedRange;
}

const vector<Instar>& Species::getInstarsRange() const
{
	return instarsRange;
}

const double Species::getInstarK_Value(const Instar &instar, const double &size) const
{
	return getInstarK_Density(instar) * pow(size, DIMENSIONS);
}

const double& Species::getInstarK_Density(const Instar &instar) const
{
	return K_DensityPerInstar[instar];
}

void Species::setInstarK_Density(const Instar &instar, const double &newK_Density)
{
	calculatedK_DensityPerInstar[instar] = true;
	K_DensityPerInstar[instar] = newK_Density;
}

const bool Species::isCalculatedInstarK_Density(const Instar &instar) const
{
	return calculatedK_DensityPerInstar[instar];
}

double Species::convertToWetMass(const double &dryMass) const
{
	return getConversionToWetMass() * dryMass;
}

double Species::convertToDryMass(const double &wetMass) const
{
	return wetMass / getConversionToWetMass();
}

const WorldInterface* const Species::getWorldInterface() const
{
	return worldInterface;
}

WorldInterface* const Species::getMutableWorldInterface()
{
	return worldInterface;
}

const unsigned int& Species::getNumberOfInstars() const 
{ 
	return numberOfInstars; 
}

template <class Archive>
void Species::serialize(Archive &ar, const unsigned int version) {
	ar & id;
	ar & scientificName;
	ar & conversionToWetMass;
	ar & extinguished;
	ar & calculatedK_DensityPerInstar;
	ar & K_DensityPerInstar;
	ar & numberOfInstars;
	ar & instarsRange;
}

// Specialisation
template void Species::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Species::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Species::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Species::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
