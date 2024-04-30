/*
 * species.cpp
 *
 *  Created on: May 24, 2013
 *      Author: jrbcast
 */

#include "World/LivingBeings/Species.h"

using namespace std;



const typename Species::Type::TypeValue Species::Type::stringToEnumValue(const string &str) { 
	try
	{
		return stringToEnum.at(str);
	}
	catch(const out_of_range& e) 
	{
		throwLineInfoException("Unknown moisture type '" + str + "'. Valid values are " + string(printAvailableValues()));
	}
}




std::string_view Species::Type::to_string(const TypeValue& type) 
{ 
	return magic_enum::enum_name(type); 
}


const unordered_map<string_view, const typename Species::Type::TypeValue> Species::Type::generateMap() 
{
	unordered_map<string_view, const TypeValue> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const TypeValue type = static_cast<const TypeValue>(i);
		enumMap.insert({to_string(type), type});
	}

	return enumMap;
}


const unordered_map<string_view, const typename Species::Type::TypeValue> Species::Type::stringToEnum = Species::Type::generateMap();


const string Species::Type::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<TypeValue>();

	string values(typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += ", " + string(typeNames[i]);
	}

	return values;
}


const string Species::Type::enumValues = Species::Type::generateAvailableValues();


std::string_view Species::Type::printAvailableValues() 
{ 
	return enumValues; 
}




unsigned int Species::speciesCounter = 0;

const id_type& Species::getSpeciesCounter()
{
	return speciesCounter;
}


Species::Species(const string& scientificName, const unsigned int numberOfInstars)
	: id(speciesCounter++), scientificName(trim(scientificName)), 
	  calculatedK_DensityPerInstar(numberOfInstars, false), K_DensityPerInstar(numberOfInstars, 0.0),
	  numberOfInstars(numberOfInstars), instarsRange(calculateInstarsRange())
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

const unsigned int& Species::getNumberOfInstars() const 
{ 
	return numberOfInstars; 
}
