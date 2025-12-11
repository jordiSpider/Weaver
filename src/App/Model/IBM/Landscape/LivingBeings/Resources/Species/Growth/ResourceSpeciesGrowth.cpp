
#include "App/Model/IBM/Landscape/LivingBeings/Resources/Species/Growth/ResourceSpeciesGrowth.h"


using namespace std;
using json = nlohmann::json;


ResourceSpeciesGrowth::ResourceSpeciesGrowth()
    : SpeciesGrowth()
{

}

ResourceSpeciesGrowth::ResourceSpeciesGrowth(const json& config)
    : SpeciesGrowth(1, vector<double>{config["conversionToWetMass"].get<double>()}), cellMass(config["cellMass"].get<double>()),
      minRH(config["minRH"].get<double>()), maxRH(config["maxRH"].get<double>())
{
    
}

ResourceSpeciesGrowth::~ResourceSpeciesGrowth()
{
    
}

const WetMass& ResourceSpeciesGrowth::getCellMass() const
{
	return cellMass;
}

PreciseDouble ResourceSpeciesGrowth::getMinRH() const 
{ 
	return minRH; 
}

PreciseDouble ResourceSpeciesGrowth::getMaxRH() const 
{ 
	return maxRH; 
}



BOOST_CLASS_EXPORT(ResourceSpeciesGrowth)

template <class Archive>
void ResourceSpeciesGrowth::serialize(Archive &ar, const unsigned int) {
    ar & boost::serialization::base_object<SpeciesGrowth>(*this);

    ar & cellMass;

    ar & minRH;
	ar & maxRH;
}

// Specialisation
template void ResourceSpeciesGrowth::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void ResourceSpeciesGrowth::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void ResourceSpeciesGrowth::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void ResourceSpeciesGrowth::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
