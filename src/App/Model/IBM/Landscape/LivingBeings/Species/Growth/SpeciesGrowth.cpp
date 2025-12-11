#include "App/Model/IBM/Landscape/LivingBeings/Species/Growth/SpeciesGrowth.h"


using namespace std;




SpeciesGrowth::SpeciesGrowth()
{

}

SpeciesGrowth::SpeciesGrowth(const unsigned int numberOfInstars, const vector<double>& newConversionToWetMass)
	: numberOfInstars(numberOfInstars), instarsRange(calculateInstarsRange()), lastInstar(Instar(numberOfInstars))
{
	setConversionToWetMass(newConversionToWetMass);
}

SpeciesGrowth::~SpeciesGrowth()
{
	
}

vector<Instar> SpeciesGrowth::calculateInstarsRange()
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

void SpeciesGrowth::setConversionToWetMass(const vector<double>& newConversionToWetMass)
{
	for(const double value : newConversionToWetMass)
	{
		conversionToWetMass.emplace_back(value);
	}
}

const Instar& SpeciesGrowth::getLastInstar() const
{
	return lastInstar;
}

const vector<Instar>& SpeciesGrowth::getInstarsRange() const
{
	return instarsRange;
}

const PreciseDouble& SpeciesGrowth::getConversionToWetMass(const Instar& instar) const 
{ 
	return conversionToWetMass[instar]; 
}

const unsigned int& SpeciesGrowth::getNumberOfInstars() const 
{ 
	return numberOfInstars; 
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(SpeciesGrowth)

template <class Archive>
void SpeciesGrowth::serialize(Archive &ar, const unsigned int) {
	ar & numberOfInstars;
	ar & instarsRange;
	ar & lastInstar;

	ar & conversionToWetMass;
}

// // Specialisation
template void SpeciesGrowth::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void SpeciesGrowth::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void SpeciesGrowth::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void SpeciesGrowth::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);

