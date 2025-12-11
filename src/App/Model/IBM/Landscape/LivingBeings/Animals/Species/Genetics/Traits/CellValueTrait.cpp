#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/CellValueTrait.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;




namespace {
    const bool registered = []() {
        Trait::registryGenerateTraits[Trait::ExecutionOrder::CellValue] = &CellValueTrait::generateTraits;

		Trait::registryGetTraitStrVector[Trait::ExecutionOrder::CellValue] = &CellValueTrait::getTraitStrVector;

		return true;
    }();
}


vector<Trait*> CellValueTrait::generateTraits(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& definitionConfig, const vector<json>& individualLevelTraitsOrder)
{
	vector<Trait*> cellValueTraits;

	for(const CellValueTraitType type : EnumClass<CellValueTraitType>::getEnumValues())
	{
		cellValueTraits.push_back(new CellValueTrait(
			individualLevelTraits, type, definitionConfig[CellValueTrait::getJsonFieldName()].at(EnumClass<CellValueTraitType>::to_string(type)), 
			individualLevelTraitsOrder
		));
	}

	return cellValueTraits;
}


vector<string> CellValueTrait::getTraitStrVector()
{
	vector<string> traitStrVector;

	for(CellValueTraitType type : EnumClass<CellValueTraitType>::getEnumValues())
	{
		traitStrVector.push_back(EnumClass<CellValueTraitType>::to_string(type));
	}

	return traitStrVector;
}




TraitTemperatureSection* CellValueTrait::createTemperatureSection(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& temperatureConfig, const vector<json>& individualLevelTraitsOrder, const string& traitStr, const string& fileName)
{
	if(temperatureConfig["temperature"]["dependent"].get<bool>())
	{
		return new PawarTraitTemperatureSection(individualLevelTraits, temperatureConfig["temperature"], individualLevelTraitsOrder, false, true, traitStr, fileName);
	}
	else
	{
		return nullptr;
	}
}




CellValueTrait::CellValueTrait()
{
	
}

CellValueTrait::CellValueTrait(std::vector<IndividualLevelTrait*>& individualLevelTraits, const CellValueTraitType type, const json& config, const vector<json>& individualLevelTraitsOrder)
	: Trait(
		individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<CellValueTraitType>::to_string(type), 
		getFileName(),
		createTemperatureSection(
			individualLevelTraits, config, individualLevelTraitsOrder, getParentTraitStr() + "$" + EnumClass<CellValueTraitType>::to_string(type), 
			getFileName()
		)
	  )
{
	
}

CellValueTrait::~CellValueTrait() 
{
	
}


bool CellValueTrait::canBeNegative() const
{
	return false;
}


string CellValueTrait::getFileName() const
{
	return string("cellValue");
}

string CellValueTrait::getParentTraitStr() const
{
	return string("cellValue");
}

string CellValueTrait::getJsonFieldName()
{
	return string("cellValue");
}


BOOST_CLASS_EXPORT(CellValueTrait)

template <class Archive>
void CellValueTrait::serialize(Archive &ar, const unsigned int) {
	ar & boost::serialization::base_object<Trait>(*this);
} 

// Specialisation
template void CellValueTrait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void CellValueTrait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void CellValueTrait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void CellValueTrait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
