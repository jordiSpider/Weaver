#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/TraitDefinitionSection.h"

#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/IndividualLevelTrait.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Genetics/Traits/DefinitionSection/SpeciesLevelTrait.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;



size_t TraitDefinitionSection::findOrder(Type type, const vector<json>& individualLevelTraitsOrder, const string& traitStr, const string& elementStr)
{
	size_t order = 0;
    bool found = false;

    for(size_t i = 0; i < individualLevelTraitsOrder.size() && !found; i++)
    {
        if(individualLevelTraitsOrder[i]["trait"].get<string>() == traitStr &&
           individualLevelTraitsOrder[i]["element"].get<string>() == elementStr)
        {
            found = true;
            order = i;
        }
    }

	switch(type) {
        case TraitDefinitionSection::Type::SpeciesLevel: {
            if(found)
            {
                throwLineInfoException("Error: The " + elementStr + " of trait '" + traitStr + "' is species level and must not have a defined order.");
            }
			break;
        }
        case TraitDefinitionSection::Type::IndividualLevel: {
            if(!found)
            {
                throwLineInfoException("Error: The " + elementStr + " of trait '" + traitStr + "' has no defined order.");
            }
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }

	return order;
}


TraitDefinitionSection* TraitDefinitionSection::createInstance(std::vector<IndividualLevelTrait*>& individualLevelTraits, const json& config, const vector<json>& individualLevelTraitsOrder, const string& traitStr, const string& elementStr, const string& fileName)
{
    Type type = EnumClass<Type>::stringToEnumValue(config["definitionType"].get<string>());

    size_t order = TraitDefinitionSection::findOrder(type, individualLevelTraitsOrder, traitStr, elementStr);

    switch(type) {
        case Type::SpeciesLevel: {
            return new SpeciesLevelTrait(config["speciesLevelParams"]);
            break;
        }
        case Type::IndividualLevel: {
            IndividualLevelTrait* trait = new IndividualLevelTrait(config["individualLevelParams"], order, fileName + "_" + elementStr, traitStr, elementStr);
            individualLevelTraits.push_back(trait);

            return trait;
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}



TraitDefinitionSection::TraitDefinitionSection()
{
	
}

TraitDefinitionSection::~TraitDefinitionSection() 
{
	
}



BOOST_SERIALIZATION_ASSUME_ABSTRACT(TraitDefinitionSection)

template <class Archive>
void TraitDefinitionSection::serialize(Archive &, const unsigned int) {
    
}

// // Specialisation
template void TraitDefinitionSection::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void TraitDefinitionSection::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void TraitDefinitionSection::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void TraitDefinitionSection::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
