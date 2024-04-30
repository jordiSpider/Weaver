#include "IBM/World/LivingBeings/Resources/Species/ResourceSpecies.h"
#include "IBM/Maths/Constants.h"

#include "IBM/World/LivingBeings/Resources/Species/BasalResourceSpecies.h"
#include "IBM/World/LivingBeings/Resources/Species/NoBasalResourceSpecies.h"


using namespace std;
using json = nlohmann::json;



ResourceSpecies::ResourceID::ResourceID()
{

}

ResourceSpecies::ResourceID::ResourceID(const id_type& value)
	: value(value)
{

}

const id_type& ResourceSpecies::ResourceID::getValue() const
{
	return value;
}

ResourceSpecies::ResourceID::operator size_t() const {
	return static_cast<size_t>(value);
}

size_t hash<ResourceSpecies::ResourceID>::operator()(const ResourceSpecies::ResourceID& resourceID) const
{
    return hash<unsigned int>{}(resourceID.getValue());
}

template <class Archive>
void ResourceSpecies::ResourceID::serialize(Archive &ar, const unsigned int version) {
	ar & value;
}

// Specialisation
template void ResourceSpecies::ResourceID::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ResourceSpecies::ResourceID::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ResourceSpecies::ResourceID::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ResourceSpecies::ResourceID::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);



unique_ptr<ResourceSpecies> ResourceSpecies::createInstance(const ResourceSpecies::ID& speciesId, const ResourceSpecies::ResourceID& resourceSpeciesId, const json &resourceSpeciesInfo, WorldInterface* const worldInterface)
{
    switch(EnumClass<Type>::stringToEnumValue(resourceSpeciesInfo["resourceType"])) {
        case Type::BasalResource: {
            return make_unique<BasalResourceSpecies>(speciesId, resourceSpeciesId, resourceSpeciesInfo, worldInterface);
            break;
        }
        case Type::NoBasalResource: {
            return make_unique<NoBasalResourceSpecies>(speciesId, resourceSpeciesId, resourceSpeciesInfo, worldInterface);
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}

unique_ptr<ResourceSpecies> ResourceSpecies::createInstance(const ResourceSpecies::Type& resourceSpeciesType, WorldInterface* const worldInterface)
{
    switch(resourceSpeciesType) {
        case Type::BasalResource: {
            return make_unique<BasalResourceSpecies>(worldInterface);
            break;
        }
        case Type::NoBasalResource: {
            return make_unique<NoBasalResourceSpecies>(worldInterface);
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}



ResourceSpecies::ResourceSpecies(WorldInterface* const worldInterface)
	: Species(worldInterface)
{
	
}

ResourceSpecies::ResourceSpecies(const ResourceSpecies::ID& speciesId, const ResourceSpecies::ResourceID& resourceSpeciesId, const json &resourceSpeciesInfo, WorldInterface* const worldInterface) :
	Species(speciesId, resourceSpeciesInfo["name"], 1, worldInterface), resourceSpeciesId(resourceSpeciesId),
	cellMass(resourceSpeciesInfo["cellMass"]),
	ACTIVATION_ENERGY(resourceSpeciesInfo["ACTIVATION_ENERGY"]),
	NORMALIZATION_B(resourceSpeciesInfo["NORMALIZATION_B"]),
	minHR(resourceSpeciesInfo["maxHR"]), maxHR(resourceSpeciesInfo["minHR"]),
	maxRScale(resourceSpeciesInfo["maxRScale"]), variableIntrinsicRateOfIncrease(resourceSpeciesInfo["variableIntrinsicRateOfIncrease"]),
	minimumEdibleBiomass(static_cast<unsigned int>(resourceSpeciesInfo["minimumNumberOfEdibleSpores"]) * static_cast<double>(resourceSpeciesInfo["cellMass"]))
{
	maxR = -1; // Will always have positive values

	this->setConversionToWetMass(resourceSpeciesInfo["conversionToWetMass"]);
}

ResourceSpecies::~ResourceSpecies()
{
	
}

const ResourceSpecies::ResourceID& ResourceSpecies::getResourceSpeciesId() const
{
	return resourceSpeciesId;
}

bool ResourceSpecies::getVariableIntrinsicRateOfIncrease() const 
{ 
	return variableIntrinsicRateOfIncrease; 
}

void ResourceSpecies::updateK_Density(const double &resourceMaximumCapacityDensity)
{
	double newK_Density = convertToDryMass(resourceMaximumCapacityDensity);

	Instar speciesInstar(getNumberOfInstars());
	setInstarK_Density(speciesInstar, (getInstarK_Density(speciesInstar) < newK_Density) ? newK_Density : getInstarK_Density(speciesInstar));
}

double ResourceSpecies::getMaxRScale() const 
{ 
	return maxRScale; 
}

void ResourceSpecies::setCellMass(const double& cellMass) 
{ 
	this->cellMass = cellMass; 
}

void ResourceSpecies::setMinimumEdibleBiomass(const double& minimumNumberOfEdibleSpores, const double& cellMass) 
{ 
	minimumEdibleBiomass = minimumNumberOfEdibleSpores * cellMass; 
}

double ResourceSpecies::getMinHR() const 
{ 
	return minHR; 
}

double ResourceSpecies::getMaxHR() const 
{ 
	return maxHR; 
}

double ResourceSpecies::getMinimumEdibleBiomass() const 
{ 
	return minimumEdibleBiomass; 
}

bool ResourceSpecies::isMobile() const 
{ 
	return false; 
}

float ResourceSpecies::getAttackProbability() const 
{ 
	return 0.0; 
}

float ResourceSpecies::getExposedAttackProbability() const 
{ 
	return 0.0; 
}

float ResourceSpecies::getKillProbability() const 
{ 
	return 0.0; 
}

float ResourceSpecies::getAttackAvoidingProbability() const 
{ 
	return -1.0; 
}

float ResourceSpecies::getKillAvoidingProbability() const 
{ 
	return -1.0; 
}

void ResourceSpecies::setGrowingRateParams(const double& minHR, const double& maxHR, const double& maxRScale, const bool& variableIntrinsicRateOfIncrease)
{
	this->maxHR = maxHR;
	this->minHR = minHR;
	this->maxRScale = maxRScale;
	this->variableIntrinsicRateOfIncrease = variableIntrinsicRateOfIncrease;
}

double ResourceSpecies::getRateOfGrowth(Temperature temperature, double moisture, double massRatio)
{
	double oldMaxR = maxR;

	maxR = maxRScale * exp(NORMALIZATION_B - ACTIVATION_ENERGY * ( INV_BOLTZMANN / (temperature.getTemperatureKelvin()))) * pow(cellMass*massRatio, -0.25);
	
	if (oldMaxR != maxR)
	{
		growingParabola.redefine(minHR, maxHR, maxR);
	}

	// An equation of the form aX^2+bX+c=Y (simplified to x(ax+b)+c=Y
	return growingParabola.getValue(moisture);
}

double ResourceSpecies::getCellMass() const
{
	return cellMass;
}

double ResourceSpecies::calculateInstarK_Density(const Instar &instar, 
		vector<CustomIndexedVector<Instar, bool>> &checkedAnimalSpecies, vector<CustomIndexedVector<Instar, bool>> &cannibalismAnimalSpecies)
{
	return getInstarK_Density(Instar(getNumberOfInstars())) * 0.1;
}

void ResourceSpecies::generateInstarInvolvedResourceSpecies(const Instar &instar, vector<ResourceSpecies::ResourceID> &instarInvolvedResourceSpecies, vector<CustomIndexedVector<Instar, bool>> &alreadyCheckedSpecies)
{
	instarInvolvedResourceSpecies.push_back(getResourceSpeciesId());
}

template <class Archive>
void ResourceSpecies::serialize(Archive &ar, const unsigned int version) {
    ar & boost::serialization::base_object<Species>(*this);

	ar & resourceSpeciesId;
	ar & cellMass;

	ar & ACTIVATION_ENERGY;
	ar & NORMALIZATION_B;

	ar & minHR;
	ar & maxHR;

	ar & maxR;
	ar & maxRScale;
	ar & variableIntrinsicRateOfIncrease;
	ar & minimumEdibleBiomass;
	ar & growingParabola;
}

// Specialisation
template void ResourceSpecies::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ResourceSpecies::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ResourceSpecies::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ResourceSpecies::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);



namespace boost {
    namespace serialization {
		template <class Archive>
        void serialize(Archive &ar, ResourceSpecies* &resourceSpeciesPtr, const unsigned int version, WorldInterface* const newWorldInterface) {
            ResourceSpecies::Type resourceSpeciesType;
            
            // For loading
            if(Archive::is_loading::value) 
            {
                ar & resourceSpeciesType;
                resourceSpeciesPtr = ResourceSpecies::createInstance(resourceSpeciesType, newWorldInterface).release();
            }
            else
            {
                resourceSpeciesType = resourceSpeciesPtr->getResourceSpeciesType();
                ar & resourceSpeciesType;
            }

            switch(resourceSpeciesType) {
				case ResourceSpecies::Type::BasalResource: {
					static_cast<BasalResourceSpecies*>(resourceSpeciesPtr)->serialize(ar, version);
					break;
				}
				case ResourceSpecies::Type::NoBasalResource: {
					static_cast<NoBasalResourceSpecies*>(resourceSpeciesPtr)->serialize(ar, version);
					break;
				}
                default: {
                    throwLineInfoException("Default case");
                    break;
                }
            }
        }

		// Specialisation
		template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, ResourceSpecies*&, const unsigned int, WorldInterface* const);
		template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, ResourceSpecies*&, const unsigned int, WorldInterface* const);

		template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, ResourceSpecies*&, const unsigned int, WorldInterface* const);
		template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, ResourceSpecies*&, const unsigned int, WorldInterface* const);
	}
}
