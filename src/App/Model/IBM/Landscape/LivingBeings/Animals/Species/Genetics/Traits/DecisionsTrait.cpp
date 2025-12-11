#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DecisionsTrait.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;





DecisionsTrait::DecisionsTrait()
{
	
}

DecisionsTrait::DecisionsTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& config, const vector<json>& individualLevelTraitsOrder, const string& traitStr, const string& fileName, TraitTemperatureSection* temperatureSection)
	: Trait(individualLevelTraits, config, individualLevelTraitsOrder, traitStr, fileName, temperatureSection)
{
	
}

DecisionsTrait::~DecisionsTrait() 
{
	
}


bool DecisionsTrait::canBeNegative() const
{
	return false;
}


std::string DecisionsTrait::getFileName() const
{
	return string("decisions");
}

string DecisionsTrait::getParentTraitStr() const
{
	return string("decisions");
}

string DecisionsTrait::getJsonFieldName()
{
	return string("decisions");
}


BOOST_SERIALIZATION_ASSUME_ABSTRACT(DecisionsTrait)

template <class Archive>
void DecisionsTrait::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<Trait>(*this);
} 


// Specialisation
template void DecisionsTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void DecisionsTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void DecisionsTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void DecisionsTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
